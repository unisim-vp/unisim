#include "isa_tms320.hh"
#include <cassert>
#line 70 "isa/tms320.isa"

/* code to be included at the beginning of the implementation of the library */
#include <iostream>
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

					static const char *REG_NAME[] = {
						"R0",  "R1",  "R2",  "R3",  "R4",  "R5",  "R6",  "R7",
						"AR0", "AR1", "AR2", "AR3", "AR4", "AR5", "AR6", "AR7",
						"DP", "IR0", "IR1",  "BK",  "SP",  "ST",  "IE",  "IF",
						"IOF",  "RS",  "RE",  "RC", "#28", "#29", "#30",  "#31"
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
						"?",    // 01011
						"NV",   // 01100
						"V",    // 01101
						"NUF",  // 01110
						"UF",   // 01111
						"NLV",  // 10000
						"LV",   // 10001
						"NLUV", // 10010
						"LUF",  // 10011
						"ZUF",  // 10100
						"?",    // 10101
						"?",    // 10110
						"?",    // 10111
						"?",    // 11000
						"?",    // 11001
						"?",    // 11010
						"?",    // 11011
						"?",    // 11100
						"?",    // 11101
						"?",    // 11110
						"?",    // 11111
					};

				} // end of namespace tms320
			} // end of namespace processor
		} // end of namespace cxx
	} // end of namespace component
} // end of namespace unisim

#line 69 "isa_tms320.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace tms320 { namespace isa { namespace tms320 {
template <
#line 45 "isa/tms320.isa"
class
#line 74 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 77 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 81 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 84 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 89 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 93 "isa_tms320.tcc"
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
#line 105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 115 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 124 "isa_tms320.tcc"
>::~Operation()
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 142 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 146 "isa_tms320.tcc"
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 150 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 154 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 158 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 161 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 165 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 168 "isa_tms320.tcc"
)
{
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	{
		os << "Unknown instruction";
	}
#line 175 "isa_tms320.tcc"
}
template <
#line 45 "isa/tms320.isa"
class
#line 180 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 183 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 190 "isa_tms320.tcc"
>
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 194 "isa_tms320.tcc"
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 202 "isa_tms320.tcc"
>::execute(
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 206 "isa_tms320.tcc"
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 209 "isa_tms320.tcc"
)
{
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 216 "isa_tms320.tcc"
}
template <
#line 45 "isa/tms320.isa"
class
#line 221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 231 "isa_tms320.tcc"
>
class OpLDE_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 240 "isa_tms320.tcc"
>
{
public:
	OpLDE_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 263 "isa_tms320.tcc"
>
class OpLDE_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 272 "isa_tms320.tcc"
>
{
public:
	OpLDE_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 292 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 295 "isa_tms320.tcc"
>
class OpLDE_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 304 "isa_tms320.tcc"
>
{
public:
	OpLDE_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 326 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 329 "isa_tms320.tcc"
>
class OpLDE_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 338 "isa_tms320.tcc"
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
#line 351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 361 "isa_tms320.tcc"
>
class OpLDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 370 "isa_tms320.tcc"
>
{
public:
	OpLDF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 393 "isa_tms320.tcc"
>
class OpLDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 402 "isa_tms320.tcc"
>
{
public:
	OpLDF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 425 "isa_tms320.tcc"
>
class OpLDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 434 "isa_tms320.tcc"
>
{
public:
	OpLDF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 449 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 459 "isa_tms320.tcc"
>
class OpLDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 468 "isa_tms320.tcc"
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
#line 481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 491 "isa_tms320.tcc"
>
class OpLDFcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 500 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 524 "isa_tms320.tcc"
>
class OpLDFcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 533 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 557 "isa_tms320.tcc"
>
class OpLDFcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 566 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 589 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 592 "isa_tms320.tcc"
>
class OpLDFcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 601 "isa_tms320.tcc"
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
#line 615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 618 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 625 "isa_tms320.tcc"
>
class OpLDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 630 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 634 "isa_tms320.tcc"
>
{
public:
	OpLDI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 657 "isa_tms320.tcc"
>
class OpLDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 666 "isa_tms320.tcc"
>
{
public:
	OpLDI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 679 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 689 "isa_tms320.tcc"
>
class OpLDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 698 "isa_tms320.tcc"
>
{
public:
	OpLDI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 723 "isa_tms320.tcc"
>
class OpLDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 732 "isa_tms320.tcc"
>
{
public:
	OpLDI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 752 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 755 "isa_tms320.tcc"
>
class OpLDIcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 764 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	void
#line 775 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 779 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 782 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 786 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 789 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 807 "isa_tms320.tcc"
>
class OpLDIcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 816 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 840 "isa_tms320.tcc"
>
class OpLDIcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 849 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 875 "isa_tms320.tcc"
>
class OpLDIcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 884 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 908 "isa_tms320.tcc"
>
class OpLDM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 913 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 917 "isa_tms320.tcc"
>
{
public:
	OpLDM_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 937 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 940 "isa_tms320.tcc"
>
class OpLDM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 949 "isa_tms320.tcc"
>
{
public:
	OpLDM_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 972 "isa_tms320.tcc"
>
class OpLDM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 981 "isa_tms320.tcc"
>
{
public:
	OpLDM_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 996 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1006 "isa_tms320.tcc"
>
class OpLDM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1015 "isa_tms320.tcc"
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
#line 1028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1038 "isa_tms320.tcc"
>
class OpLDP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1047 "isa_tms320.tcc"
>
{
public:
	OpLDP(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1066 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1069 "isa_tms320.tcc"
>
class OpPOP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1078 "isa_tms320.tcc"
>
{
public:
	OpPOP(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	void
#line 1087 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1091 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1094 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1098 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1101 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1119 "isa_tms320.tcc"
>
class OpPOPF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1128 "isa_tms320.tcc"
>
{
public:
	OpPOPF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	void
#line 1137 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1141 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1144 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1148 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1151 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1159 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1162 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1166 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1169 "isa_tms320.tcc"
>
class OpPUSH : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1178 "isa_tms320.tcc"
>
{
public:
	OpPUSH(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	void
#line 1187 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1191 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1194 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1198 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1201 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1209 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1216 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1219 "isa_tms320.tcc"
>
class OpPUSHF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1228 "isa_tms320.tcc"
>
{
public:
	OpPUSHF(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	void
#line 1237 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1241 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1244 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1248 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1251 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1269 "isa_tms320.tcc"
>
class OpSTF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1278 "isa_tms320.tcc"
>
{
public:
	OpSTF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1301 "isa_tms320.tcc"
>
class OpSTF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1310 "isa_tms320.tcc"
>
{
public:
	OpSTF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1335 "isa_tms320.tcc"
>
class OpSTI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1344 "isa_tms320.tcc"
>
{
public:
	OpSTI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1357 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1364 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1367 "isa_tms320.tcc"
>
class OpSTI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1376 "isa_tms320.tcc"
>
{
public:
	OpSTI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
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
class OpABSF_reg : public Operation<
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
	OpABSF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1433 "isa_tms320.tcc"
>
class OpABSF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1442 "isa_tms320.tcc"
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
#line 1455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1465 "isa_tms320.tcc"
>
class OpABSF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1474 "isa_tms320.tcc"
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
#line 1489 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1499 "isa_tms320.tcc"
>
class OpABSF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1508 "isa_tms320.tcc"
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
#line 1521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1531 "isa_tms320.tcc"
>
class OpABSI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1540 "isa_tms320.tcc"
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
#line 1553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1560 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1563 "isa_tms320.tcc"
>
class OpABSI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1572 "isa_tms320.tcc"
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
#line 1585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1595 "isa_tms320.tcc"
>
class OpABSI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1604 "isa_tms320.tcc"
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
#line 1619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1629 "isa_tms320.tcc"
>
class OpABSI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1638 "isa_tms320.tcc"
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
#line 1651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1661 "isa_tms320.tcc"
>
class OpADDC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1670 "isa_tms320.tcc"
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
#line 1683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1693 "isa_tms320.tcc"
>
class OpADDC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1702 "isa_tms320.tcc"
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
#line 1715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1725 "isa_tms320.tcc"
>
class OpADDC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1734 "isa_tms320.tcc"
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
#line 1749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1752 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1759 "isa_tms320.tcc"
>
class OpADDC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1768 "isa_tms320.tcc"
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
#line 1781 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1791 "isa_tms320.tcc"
>
class OpADDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1800 "isa_tms320.tcc"
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
#line 1813 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1820 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1823 "isa_tms320.tcc"
>
class OpADDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1832 "isa_tms320.tcc"
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
#line 1845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1855 "isa_tms320.tcc"
>
class OpADDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1864 "isa_tms320.tcc"
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
#line 1879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1889 "isa_tms320.tcc"
>
class OpADDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1898 "isa_tms320.tcc"
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
#line 1911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1918 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1921 "isa_tms320.tcc"
>
class OpADDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1930 "isa_tms320.tcc"
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
#line 1943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1953 "isa_tms320.tcc"
>
class OpADDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1962 "isa_tms320.tcc"
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
#line 1975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1982 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1985 "isa_tms320.tcc"
>
class OpADDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1994 "isa_tms320.tcc"
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
#line 2009 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2012 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2019 "isa_tms320.tcc"
>
class OpADDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2028 "isa_tms320.tcc"
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
#line 2041 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2051 "isa_tms320.tcc"
>
class OpAND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2060 "isa_tms320.tcc"
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
#line 2073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2083 "isa_tms320.tcc"
>
class OpAND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2092 "isa_tms320.tcc"
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
#line 2105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2115 "isa_tms320.tcc"
>
class OpAND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2124 "isa_tms320.tcc"
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
#line 2139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2149 "isa_tms320.tcc"
>
class OpAND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2158 "isa_tms320.tcc"
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
#line 2171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2181 "isa_tms320.tcc"
>
class OpANDN_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2190 "isa_tms320.tcc"
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
#line 2203 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2210 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2213 "isa_tms320.tcc"
>
class OpANDN_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2218 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2222 "isa_tms320.tcc"
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
#line 2235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2242 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2245 "isa_tms320.tcc"
>
class OpANDN_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2254 "isa_tms320.tcc"
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
#line 2269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2272 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2279 "isa_tms320.tcc"
>
class OpANDN_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2284 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2288 "isa_tms320.tcc"
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
#line 2301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2311 "isa_tms320.tcc"
>
class OpASH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2320 "isa_tms320.tcc"
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
#line 2333 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2343 "isa_tms320.tcc"
>
class OpASH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2348 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2352 "isa_tms320.tcc"
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
#line 2365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2372 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2375 "isa_tms320.tcc"
>
class OpASH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2384 "isa_tms320.tcc"
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
#line 2399 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2406 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2409 "isa_tms320.tcc"
>
class OpASH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2418 "isa_tms320.tcc"
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
#line 2431 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2441 "isa_tms320.tcc"
>
class OpCMPF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2450 "isa_tms320.tcc"
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
#line 2463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2473 "isa_tms320.tcc"
>
class OpCMPF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2482 "isa_tms320.tcc"
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
#line 2495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2502 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2505 "isa_tms320.tcc"
>
class OpCMPF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2514 "isa_tms320.tcc"
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
#line 2529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2539 "isa_tms320.tcc"
>
class OpCMPF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2548 "isa_tms320.tcc"
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
#line 2561 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2571 "isa_tms320.tcc"
>
class OpCMPI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2580 "isa_tms320.tcc"
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
#line 2593 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2603 "isa_tms320.tcc"
>
class OpCMPI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2608 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2612 "isa_tms320.tcc"
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
#line 2625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2635 "isa_tms320.tcc"
>
class OpCMPI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2644 "isa_tms320.tcc"
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
#line 2659 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2666 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2669 "isa_tms320.tcc"
>
class OpCMPI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2678 "isa_tms320.tcc"
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
#line 2691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2701 "isa_tms320.tcc"
>
class OpFIX_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2710 "isa_tms320.tcc"
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
#line 2723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2733 "isa_tms320.tcc"
>
class OpFIX_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2742 "isa_tms320.tcc"
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
#line 2755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2765 "isa_tms320.tcc"
>
class OpFIX_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2774 "isa_tms320.tcc"
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
#line 2789 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2792 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2799 "isa_tms320.tcc"
>
class OpFIX_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2808 "isa_tms320.tcc"
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
#line 2821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2831 "isa_tms320.tcc"
>
class OpFLOAT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2840 "isa_tms320.tcc"
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
#line 2853 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2860 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2863 "isa_tms320.tcc"
>
class OpFLOAT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2872 "isa_tms320.tcc"
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
#line 2885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2892 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2895 "isa_tms320.tcc"
>
class OpFLOAT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2904 "isa_tms320.tcc"
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
#line 2919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2929 "isa_tms320.tcc"
>
class OpFLOAT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2938 "isa_tms320.tcc"
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
#line 2951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2961 "isa_tms320.tcc"
>
class OpLSH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2970 "isa_tms320.tcc"
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
#line 2983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2993 "isa_tms320.tcc"
>
class OpLSH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3002 "isa_tms320.tcc"
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
#line 3015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3025 "isa_tms320.tcc"
>
class OpLSH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3034 "isa_tms320.tcc"
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
#line 3049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3059 "isa_tms320.tcc"
>
class OpLSH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3068 "isa_tms320.tcc"
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
#line 3081 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3084 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3091 "isa_tms320.tcc"
>
class OpMPYF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3100 "isa_tms320.tcc"
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
#line 3113 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3123 "isa_tms320.tcc"
>
class OpMPYF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3132 "isa_tms320.tcc"
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
#line 3145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3155 "isa_tms320.tcc"
>
class OpMPYF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3164 "isa_tms320.tcc"
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
#line 3179 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3182 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3186 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3189 "isa_tms320.tcc"
>
class OpMPYF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3198 "isa_tms320.tcc"
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
#line 3211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3221 "isa_tms320.tcc"
>
class OpMPYI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3230 "isa_tms320.tcc"
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
#line 3243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3250 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3253 "isa_tms320.tcc"
>
class OpMPYI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3262 "isa_tms320.tcc"
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
#line 3275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3285 "isa_tms320.tcc"
>
class OpMPYI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3294 "isa_tms320.tcc"
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
#line 3309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3312 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3319 "isa_tms320.tcc"
>
class OpMPYI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3328 "isa_tms320.tcc"
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
#line 3341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3344 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3351 "isa_tms320.tcc"
>
class OpNEGB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3360 "isa_tms320.tcc"
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
#line 3373 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3383 "isa_tms320.tcc"
>
class OpNEGB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3392 "isa_tms320.tcc"
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
#line 3405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3408 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3415 "isa_tms320.tcc"
>
class OpNEGB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3424 "isa_tms320.tcc"
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
#line 3439 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3446 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3449 "isa_tms320.tcc"
>
class OpNEGB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3458 "isa_tms320.tcc"
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
#line 3471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3478 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3481 "isa_tms320.tcc"
>
class OpNEGF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3490 "isa_tms320.tcc"
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
#line 3503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3510 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3513 "isa_tms320.tcc"
>
class OpNEGF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3522 "isa_tms320.tcc"
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
#line 3535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3545 "isa_tms320.tcc"
>
class OpNEGF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3554 "isa_tms320.tcc"
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
#line 3569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3579 "isa_tms320.tcc"
>
class OpNEGF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3588 "isa_tms320.tcc"
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
#line 3601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3604 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3611 "isa_tms320.tcc"
>
class OpNEGI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3620 "isa_tms320.tcc"
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
#line 3633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3643 "isa_tms320.tcc"
>
class OpNEGI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3652 "isa_tms320.tcc"
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
#line 3665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3668 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3672 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3675 "isa_tms320.tcc"
>
class OpNEGI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3684 "isa_tms320.tcc"
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
#line 3699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3709 "isa_tms320.tcc"
>
class OpNEGI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3718 "isa_tms320.tcc"
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
#line 3731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3734 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3741 "isa_tms320.tcc"
>
class OpNORM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3750 "isa_tms320.tcc"
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
#line 3763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3773 "isa_tms320.tcc"
>
class OpNORM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3782 "isa_tms320.tcc"
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
#line 3795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3805 "isa_tms320.tcc"
>
class OpNORM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3814 "isa_tms320.tcc"
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
#line 3829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3839 "isa_tms320.tcc"
>
class OpNORM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3848 "isa_tms320.tcc"
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
#line 3861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3871 "isa_tms320.tcc"
>
class OpNOT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3880 "isa_tms320.tcc"
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
#line 3893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3896 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3900 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3903 "isa_tms320.tcc"
>
class OpNOT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3912 "isa_tms320.tcc"
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
#line 3925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3935 "isa_tms320.tcc"
>
class OpNOT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3944 "isa_tms320.tcc"
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
#line 3959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3966 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3969 "isa_tms320.tcc"
>
class OpNOT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3978 "isa_tms320.tcc"
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
#line 3991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4001 "isa_tms320.tcc"
>
class OpOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4010 "isa_tms320.tcc"
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
#line 4023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4033 "isa_tms320.tcc"
>
class OpOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4042 "isa_tms320.tcc"
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
#line 4055 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4062 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4065 "isa_tms320.tcc"
>
class OpOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4074 "isa_tms320.tcc"
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
#line 4089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4099 "isa_tms320.tcc"
>
class OpOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4108 "isa_tms320.tcc"
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
#line 4121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4131 "isa_tms320.tcc"
>
class OpRND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4140 "isa_tms320.tcc"
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
#line 4153 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4160 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4163 "isa_tms320.tcc"
>
class OpRND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4172 "isa_tms320.tcc"
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
#line 4185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4195 "isa_tms320.tcc"
>
class OpRND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4204 "isa_tms320.tcc"
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
#line 4219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4229 "isa_tms320.tcc"
>
class OpRND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4238 "isa_tms320.tcc"
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
#line 4251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4261 "isa_tms320.tcc"
>
class OpROL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4270 "isa_tms320.tcc"
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
#line 4282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4289 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4292 "isa_tms320.tcc"
>
class OpROLC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4297 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4301 "isa_tms320.tcc"
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
#line 4313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4320 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4323 "isa_tms320.tcc"
>
class OpROR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4332 "isa_tms320.tcc"
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
#line 4344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4354 "isa_tms320.tcc"
>
class OpRORC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4363 "isa_tms320.tcc"
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
#line 4375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4385 "isa_tms320.tcc"
>
class OpSUBB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4394 "isa_tms320.tcc"
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
#line 4407 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4417 "isa_tms320.tcc"
>
class OpSUBB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4422 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4426 "isa_tms320.tcc"
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
#line 4439 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4446 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4449 "isa_tms320.tcc"
>
class OpSUBB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4458 "isa_tms320.tcc"
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
#line 4473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4480 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4483 "isa_tms320.tcc"
>
class OpSUBB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4492 "isa_tms320.tcc"
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
#line 4505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4515 "isa_tms320.tcc"
>
class OpSUBC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4520 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4524 "isa_tms320.tcc"
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
#line 4537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4547 "isa_tms320.tcc"
>
class OpSUBC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4556 "isa_tms320.tcc"
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
#line 4569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4579 "isa_tms320.tcc"
>
class OpSUBC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4588 "isa_tms320.tcc"
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
#line 4603 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4613 "isa_tms320.tcc"
>
class OpSUBC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4618 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4622 "isa_tms320.tcc"
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
#line 4635 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4638 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4642 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4645 "isa_tms320.tcc"
>
class OpSUBF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4654 "isa_tms320.tcc"
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
#line 4667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4670 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4677 "isa_tms320.tcc"
>
class OpSUBF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4686 "isa_tms320.tcc"
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
#line 4699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4709 "isa_tms320.tcc"
>
class OpSUBF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4718 "isa_tms320.tcc"
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
#line 4733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4743 "isa_tms320.tcc"
>
class OpSUBF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4752 "isa_tms320.tcc"
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
#line 4765 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4768 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4775 "isa_tms320.tcc"
>
class OpSUBI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4784 "isa_tms320.tcc"
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
#line 4797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4807 "isa_tms320.tcc"
>
class OpSUBI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4816 "isa_tms320.tcc"
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
#line 4829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4839 "isa_tms320.tcc"
>
class OpSUBI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4848 "isa_tms320.tcc"
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
#line 4863 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4873 "isa_tms320.tcc"
>
class OpSUBI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4882 "isa_tms320.tcc"
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
#line 4895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4898 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4902 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4905 "isa_tms320.tcc"
>
class OpSUBRB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4910 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4914 "isa_tms320.tcc"
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
#line 4927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4930 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4937 "isa_tms320.tcc"
>
class OpSUBRB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4946 "isa_tms320.tcc"
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
#line 4959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4966 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4969 "isa_tms320.tcc"
>
class OpSUBRB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4978 "isa_tms320.tcc"
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
#line 4993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5003 "isa_tms320.tcc"
>
class OpSUBRB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5012 "isa_tms320.tcc"
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
#line 5025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5035 "isa_tms320.tcc"
>
class OpSUBRF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5044 "isa_tms320.tcc"
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
#line 5057 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5067 "isa_tms320.tcc"
>
class OpSUBRF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5072 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5076 "isa_tms320.tcc"
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
#line 5089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5099 "isa_tms320.tcc"
>
class OpSUBRF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5108 "isa_tms320.tcc"
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
#line 5123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5133 "isa_tms320.tcc"
>
class OpSUBRF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5142 "isa_tms320.tcc"
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
#line 5155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5165 "isa_tms320.tcc"
>
class OpSUBRI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5174 "isa_tms320.tcc"
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
#line 5187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5197 "isa_tms320.tcc"
>
class OpSUBRI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5206 "isa_tms320.tcc"
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
#line 5219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5229 "isa_tms320.tcc"
>
class OpSUBRI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5238 "isa_tms320.tcc"
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
#line 5253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5263 "isa_tms320.tcc"
>
class OpSUBRI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5272 "isa_tms320.tcc"
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
#line 5285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5292 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5295 "isa_tms320.tcc"
>
class OpTSTB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5304 "isa_tms320.tcc"
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
#line 5317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5327 "isa_tms320.tcc"
>
class OpTSTB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5332 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5336 "isa_tms320.tcc"
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
#line 5349 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5356 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5359 "isa_tms320.tcc"
>
class OpTSTB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5368 "isa_tms320.tcc"
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
#line 5383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5393 "isa_tms320.tcc"
>
class OpTSTB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5402 "isa_tms320.tcc"
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
#line 5415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5425 "isa_tms320.tcc"
>
class OpXOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5434 "isa_tms320.tcc"
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
#line 5447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5457 "isa_tms320.tcc"
>
class OpXOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5466 "isa_tms320.tcc"
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
#line 5479 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5486 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5489 "isa_tms320.tcc"
>
class OpXOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5498 "isa_tms320.tcc"
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
#line 5513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5520 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5523 "isa_tms320.tcc"
>
class OpXOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5532 "isa_tms320.tcc"
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
#line 5545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5555 "isa_tms320.tcc"
>
class OpADDC3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5560 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5564 "isa_tms320.tcc"
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
#line 5578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5588 "isa_tms320.tcc"
>
class OpADDC3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5597 "isa_tms320.tcc"
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
#line 5612 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5622 "isa_tms320.tcc"
>
class OpADDC3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5631 "isa_tms320.tcc"
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
#line 5646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5649 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5653 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5656 "isa_tms320.tcc"
>
class OpADDC3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5665 "isa_tms320.tcc"
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
#line 5681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5691 "isa_tms320.tcc"
>
class OpADDF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5700 "isa_tms320.tcc"
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
#line 5714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5724 "isa_tms320.tcc"
>
class OpADDF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5733 "isa_tms320.tcc"
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
#line 5748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5758 "isa_tms320.tcc"
>
class OpADDF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5763 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5767 "isa_tms320.tcc"
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
#line 5782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5789 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5792 "isa_tms320.tcc"
>
class OpADDF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5797 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5801 "isa_tms320.tcc"
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
#line 5817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5824 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5827 "isa_tms320.tcc"
>
class OpADDI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5836 "isa_tms320.tcc"
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
#line 5850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5860 "isa_tms320.tcc"
>
class OpADDI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5869 "isa_tms320.tcc"
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
#line 5884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5894 "isa_tms320.tcc"
>
class OpADDI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5903 "isa_tms320.tcc"
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
#line 5918 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5928 "isa_tms320.tcc"
>
class OpADDI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5937 "isa_tms320.tcc"
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
#line 5953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5963 "isa_tms320.tcc"
>
class OpAND3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5972 "isa_tms320.tcc"
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
#line 5986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5996 "isa_tms320.tcc"
>
class OpAND3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6005 "isa_tms320.tcc"
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
#line 6020 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6030 "isa_tms320.tcc"
>
class OpAND3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6039 "isa_tms320.tcc"
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
#line 6054 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6057 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6061 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6064 "isa_tms320.tcc"
>
class OpAND3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6069 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6073 "isa_tms320.tcc"
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
#line 6089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6099 "isa_tms320.tcc"
>
class OpANDN3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6108 "isa_tms320.tcc"
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
#line 6122 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6129 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6132 "isa_tms320.tcc"
>
class OpANDN3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6141 "isa_tms320.tcc"
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
#line 6156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6166 "isa_tms320.tcc"
>
class OpANDN3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6175 "isa_tms320.tcc"
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
#line 6190 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6200 "isa_tms320.tcc"
>
class OpANDN3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6209 "isa_tms320.tcc"
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
#line 6225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6235 "isa_tms320.tcc"
>
class OpASH3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6244 "isa_tms320.tcc"
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
#line 6258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6268 "isa_tms320.tcc"
>
class OpASH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6277 "isa_tms320.tcc"
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
#line 6292 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6302 "isa_tms320.tcc"
>
class OpASH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6311 "isa_tms320.tcc"
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
#line 6326 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6333 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6336 "isa_tms320.tcc"
>
class OpASH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6345 "isa_tms320.tcc"
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
#line 6361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6371 "isa_tms320.tcc"
>
class OpCMPF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6380 "isa_tms320.tcc"
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
#line 6394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6404 "isa_tms320.tcc"
>
class OpCMPF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6413 "isa_tms320.tcc"
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
#line 6428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6438 "isa_tms320.tcc"
>
class OpCMPF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6447 "isa_tms320.tcc"
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
#line 6462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6465 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6469 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6472 "isa_tms320.tcc"
>
class OpCMPF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6481 "isa_tms320.tcc"
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
#line 6497 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6507 "isa_tms320.tcc"
>
class OpCMPI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6512 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6516 "isa_tms320.tcc"
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
#line 6530 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6533 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6540 "isa_tms320.tcc"
>
class OpCMPI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6549 "isa_tms320.tcc"
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
#line 6564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6574 "isa_tms320.tcc"
>
class OpCMPI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6583 "isa_tms320.tcc"
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
#line 6598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6605 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6608 "isa_tms320.tcc"
>
class OpCMPI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6617 "isa_tms320.tcc"
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
#line 6633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6643 "isa_tms320.tcc"
>
class OpLSH3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6652 "isa_tms320.tcc"
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
#line 6666 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6676 "isa_tms320.tcc"
>
class OpLSH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6685 "isa_tms320.tcc"
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
#line 6700 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6707 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6710 "isa_tms320.tcc"
>
class OpLSH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6719 "isa_tms320.tcc"
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
#line 6734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6744 "isa_tms320.tcc"
>
class OpLSH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6753 "isa_tms320.tcc"
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
#line 6769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6779 "isa_tms320.tcc"
>
class OpMPYF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6788 "isa_tms320.tcc"
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
#line 6802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6812 "isa_tms320.tcc"
>
class OpMPYF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6821 "isa_tms320.tcc"
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
#line 6836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6846 "isa_tms320.tcc"
>
class OpMPYF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6855 "isa_tms320.tcc"
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
#line 6870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6880 "isa_tms320.tcc"
>
class OpMPYF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6889 "isa_tms320.tcc"
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
#line 6905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6915 "isa_tms320.tcc"
>
class OpMPYI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6924 "isa_tms320.tcc"
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
#line 6938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6945 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6948 "isa_tms320.tcc"
>
class OpMPYI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6957 "isa_tms320.tcc"
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
#line 6972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6982 "isa_tms320.tcc"
>
class OpMPYI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6991 "isa_tms320.tcc"
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
#line 7006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7016 "isa_tms320.tcc"
>
class OpMPYI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7025 "isa_tms320.tcc"
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
#line 7041 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7051 "isa_tms320.tcc"
>
class OpOR3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7060 "isa_tms320.tcc"
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
#line 7074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7081 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7084 "isa_tms320.tcc"
>
class OpOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7093 "isa_tms320.tcc"
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
#line 7108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7118 "isa_tms320.tcc"
>
class OpOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7127 "isa_tms320.tcc"
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
#line 7142 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7152 "isa_tms320.tcc"
>
class OpOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7161 "isa_tms320.tcc"
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
#line 7177 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7184 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7187 "isa_tms320.tcc"
>
class OpSUBB3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7196 "isa_tms320.tcc"
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
#line 7210 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7220 "isa_tms320.tcc"
>
class OpSUBB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7225 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7229 "isa_tms320.tcc"
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
#line 7244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7254 "isa_tms320.tcc"
>
class OpSUBB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7263 "isa_tms320.tcc"
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
#line 7278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7288 "isa_tms320.tcc"
>
class OpSUBB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7297 "isa_tms320.tcc"
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
#line 7313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7320 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7323 "isa_tms320.tcc"
>
class OpSUBF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7332 "isa_tms320.tcc"
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
#line 7346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7356 "isa_tms320.tcc"
>
class OpSUBF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7365 "isa_tms320.tcc"
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
#line 7380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7390 "isa_tms320.tcc"
>
class OpSUBF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7399 "isa_tms320.tcc"
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
#line 7414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7424 "isa_tms320.tcc"
>
class OpSUBF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7433 "isa_tms320.tcc"
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
#line 7449 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7459 "isa_tms320.tcc"
>
class OpSUBI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7468 "isa_tms320.tcc"
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
#line 7482 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7489 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7492 "isa_tms320.tcc"
>
class OpSUBI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7501 "isa_tms320.tcc"
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
#line 7516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7526 "isa_tms320.tcc"
>
class OpSUBI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7535 "isa_tms320.tcc"
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
#line 7550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7557 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7560 "isa_tms320.tcc"
>
class OpSUBI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7569 "isa_tms320.tcc"
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
#line 7585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7595 "isa_tms320.tcc"
>
class OpTSTB3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7604 "isa_tms320.tcc"
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
#line 7618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7628 "isa_tms320.tcc"
>
class OpTSTB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7637 "isa_tms320.tcc"
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
#line 7652 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7659 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7662 "isa_tms320.tcc"
>
class OpTSTB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7671 "isa_tms320.tcc"
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
#line 7686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7696 "isa_tms320.tcc"
>
class OpTSTB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7705 "isa_tms320.tcc"
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
#line 7721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7731 "isa_tms320.tcc"
>
class OpXOR3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7740 "isa_tms320.tcc"
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
#line 7754 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7764 "isa_tms320.tcc"
>
class OpXOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7769 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7773 "isa_tms320.tcc"
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
#line 7788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7798 "isa_tms320.tcc"
>
class OpXOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7807 "isa_tms320.tcc"
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
#line 7822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7832 "isa_tms320.tcc"
>
class OpXOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7841 "isa_tms320.tcc"
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
#line 7857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7867 "isa_tms320.tcc"
>
class OpBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7876 "isa_tms320.tcc"
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
#line 7889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7899 "isa_tms320.tcc"
>
class OpBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7908 "isa_tms320.tcc"
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
#line 7921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7931 "isa_tms320.tcc"
>
class OpBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7940 "isa_tms320.tcc"
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
#line 7953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7963 "isa_tms320.tcc"
>
class OpBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7972 "isa_tms320.tcc"
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
#line 7985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7995 "isa_tms320.tcc"
>
class OpBR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8004 "isa_tms320.tcc"
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
#line 8016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8026 "isa_tms320.tcc"
>
class OpBRD : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8035 "isa_tms320.tcc"
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
#line 8047 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8050 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8054 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8057 "isa_tms320.tcc"
>
class OpCALL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8066 "isa_tms320.tcc"
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
#line 8078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8088 "isa_tms320.tcc"
>
class OpCALLcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8097 "isa_tms320.tcc"
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
class OpCALLcond_disp : public Operation<
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
	OpCALLcond_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8142 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8152 "isa_tms320.tcc"
>
class OpDBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8161 "isa_tms320.tcc"
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
#line 8175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8185 "isa_tms320.tcc"
>
class OpDBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8194 "isa_tms320.tcc"
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
#line 8208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8218 "isa_tms320.tcc"
>
class OpDBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8227 "isa_tms320.tcc"
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
#line 8241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8251 "isa_tms320.tcc"
>
class OpDBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8260 "isa_tms320.tcc"
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
#line 8274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8284 "isa_tms320.tcc"
>
class OpIACK_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8293 "isa_tms320.tcc"
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
#line 8305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8315 "isa_tms320.tcc"
>
class OpIACK_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8324 "isa_tms320.tcc"
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
#line 8338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8348 "isa_tms320.tcc"
>
class OpIDLE : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8357 "isa_tms320.tcc"
>
{
public:
	OpIDLE(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8378 "isa_tms320.tcc"
>
class OpNOP_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8387 "isa_tms320.tcc"
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
#line 8399 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8406 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8409 "isa_tms320.tcc"
>
class OpNOP_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8418 "isa_tms320.tcc"
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
#line 8432 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8439 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8442 "isa_tms320.tcc"
>
class OpRETIcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8447 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8451 "isa_tms320.tcc"
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
#line 8463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8473 "isa_tms320.tcc"
>
class OpRETScond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8482 "isa_tms320.tcc"
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
#line 8494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8504 "isa_tms320.tcc"
>
class OpRPTB : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8513 "isa_tms320.tcc"
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
#line 8525 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8535 "isa_tms320.tcc"
>
class OpRPTS_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8544 "isa_tms320.tcc"
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
#line 8556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8563 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8566 "isa_tms320.tcc"
>
class OpRPTS_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8575 "isa_tms320.tcc"
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
#line 8587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8597 "isa_tms320.tcc"
>
class OpRPTS_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8606 "isa_tms320.tcc"
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
#line 8620 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8627 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8630 "isa_tms320.tcc"
>
class OpRPTS_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8639 "isa_tms320.tcc"
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
#line 8651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8661 "isa_tms320.tcc"
>
class OpSWI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8670 "isa_tms320.tcc"
>
{
public:
	OpSWI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8691 "isa_tms320.tcc"
>
class OpTRAPcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8700 "isa_tms320.tcc"
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
#line 8713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8723 "isa_tms320.tcc"
>
class OpIDLE2 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8732 "isa_tms320.tcc"
>
{
public:
	OpIDLE2(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8753 "isa_tms320.tcc"
>
class OpLOPOWER : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8762 "isa_tms320.tcc"
>
{
public:
	OpLOPOWER(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8773 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8780 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8783 "isa_tms320.tcc"
>
class OpMAXSPEED : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8792 "isa_tms320.tcc"
>
{
public:
	OpMAXSPEED(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8813 "isa_tms320.tcc"
>
class OpLDFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8822 "isa_tms320.tcc"
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
#line 8835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8845 "isa_tms320.tcc"
>
class OpLDFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8850 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8854 "isa_tms320.tcc"
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
#line 8869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8879 "isa_tms320.tcc"
>
class OpLDII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8884 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8888 "isa_tms320.tcc"
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
#line 8901 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8911 "isa_tms320.tcc"
>
class OpLDII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8920 "isa_tms320.tcc"
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
#line 8935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8942 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8945 "isa_tms320.tcc"
>
class OpSIGI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8950 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8954 "isa_tms320.tcc"
>
{
public:
	OpSIGI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8975 "isa_tms320.tcc"
>
class OpSTFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8980 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8984 "isa_tms320.tcc"
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
#line 8997 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9007 "isa_tms320.tcc"
>
class OpSTFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9012 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9016 "isa_tms320.tcc"
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
#line 9031 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9034 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9041 "isa_tms320.tcc"
>
class OpSTII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9050 "isa_tms320.tcc"
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
#line 9063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9073 "isa_tms320.tcc"
>
class OpSTII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9082 "isa_tms320.tcc"
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
#line 9097 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9100 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9104 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9107 "isa_tms320.tcc"
>
class OpFLOAT_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9116 "isa_tms320.tcc"
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
#line 9133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9140 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9143 "isa_tms320.tcc"
>
class OpFLOAT_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9152 "isa_tms320.tcc"
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
#line 9168 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9178 "isa_tms320.tcc"
>
class OpLDF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9183 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9187 "isa_tms320.tcc"
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
#line 9204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9214 "isa_tms320.tcc"
>
class OpLDF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9223 "isa_tms320.tcc"
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
#line 9239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9249 "isa_tms320.tcc"
>
class OpLDI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9258 "isa_tms320.tcc"
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
#line 9275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9285 "isa_tms320.tcc"
>
class OpLDI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9294 "isa_tms320.tcc"
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
#line 9310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9320 "isa_tms320.tcc"
>
class OpLSH3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9329 "isa_tms320.tcc"
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
#line 9347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9357 "isa_tms320.tcc"
>
class OpLSH3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9366 "isa_tms320.tcc"
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
#line 9383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9393 "isa_tms320.tcc"
>
class OpMPYF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9402 "isa_tms320.tcc"
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
#line 9420 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9430 "isa_tms320.tcc"
>
class OpMPYF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9439 "isa_tms320.tcc"
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
#line 9456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9466 "isa_tms320.tcc"
>
class OpMPYI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9475 "isa_tms320.tcc"
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
#line 9493 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9503 "isa_tms320.tcc"
>
class OpMPYI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9512 "isa_tms320.tcc"
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
#line 9529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9539 "isa_tms320.tcc"
>
class OpNEGF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9548 "isa_tms320.tcc"
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
#line 9565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9572 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9575 "isa_tms320.tcc"
>
class OpNEGF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9584 "isa_tms320.tcc"
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
#line 9600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9610 "isa_tms320.tcc"
>
class OpNEGI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9619 "isa_tms320.tcc"
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
#line 9636 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9643 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9646 "isa_tms320.tcc"
>
class OpNEGI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9655 "isa_tms320.tcc"
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
#line 9671 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9678 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9681 "isa_tms320.tcc"
>
class OpNOT_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9690 "isa_tms320.tcc"
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
#line 9707 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9717 "isa_tms320.tcc"
>
class OpNOT_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9726 "isa_tms320.tcc"
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
#line 9742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9752 "isa_tms320.tcc"
>
class OpOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9761 "isa_tms320.tcc"
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
#line 9779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9789 "isa_tms320.tcc"
>
class OpOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9794 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9798 "isa_tms320.tcc"
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
#line 9815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9825 "isa_tms320.tcc"
>
class OpSTF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9830 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9834 "isa_tms320.tcc"
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
#line 9851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9861 "isa_tms320.tcc"
>
class OpSTF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9870 "isa_tms320.tcc"
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
#line 9886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9896 "isa_tms320.tcc"
>
class OpSTI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9905 "isa_tms320.tcc"
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
#line 9922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9932 "isa_tms320.tcc"
>
class OpSTI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9941 "isa_tms320.tcc"
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
#line 9957 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9960 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9967 "isa_tms320.tcc"
>
class OpSUBF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9972 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9976 "isa_tms320.tcc"
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
#line 9994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9997 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10004 "isa_tms320.tcc"
>
class OpSUBF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10013 "isa_tms320.tcc"
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
#line 10030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10040 "isa_tms320.tcc"
>
class OpSUBI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10049 "isa_tms320.tcc"
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
#line 10067 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10077 "isa_tms320.tcc"
>
class OpSUBI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10086 "isa_tms320.tcc"
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
#line 10103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10113 "isa_tms320.tcc"
>
class OpXOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10122 "isa_tms320.tcc"
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
#line 10140 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10143 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10147 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10150 "isa_tms320.tcc"
>
class OpXOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10155 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10159 "isa_tms320.tcc"
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
#line 10176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10183 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10186 "isa_tms320.tcc"
>
class OpLDF_LDF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10195 "isa_tms320.tcc"
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
#line 10212 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10222 "isa_tms320.tcc"
>
class OpLDF_LDF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10231 "isa_tms320.tcc"
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
#line 10247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10254 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10257 "isa_tms320.tcc"
>
class OpLDI_LDI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10266 "isa_tms320.tcc"
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
#line 10283 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10286 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10290 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10293 "isa_tms320.tcc"
>
class OpLDI_LDI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10302 "isa_tms320.tcc"
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
#line 10318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10328 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10337 "isa_tms320.tcc"
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
#line 10357 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10364 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10367 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10376 "isa_tms320.tcc"
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
#line 10394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10404 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10413 "isa_tms320.tcc"
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
#line 10433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10443 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10452 "isa_tms320.tcc"
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
#line 10470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10473 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10480 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10489 "isa_tms320.tcc"
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
#line 10509 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10512 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10519 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10528 "isa_tms320.tcc"
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
#line 10546 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10549 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10556 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10565 "isa_tms320.tcc"
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
#line 10585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10595 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10604 "isa_tms320.tcc"
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
#line 10676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10686 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10695 "isa_tms320.tcc"
> *DecodeOpLDE_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10701 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10705 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10723 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10732 "isa_tms320.tcc"
> *DecodeOpLDE_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10738 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10742 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10760 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10769 "isa_tms320.tcc"
> *DecodeOpLDE_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10775 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10779 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10794 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10797 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10806 "isa_tms320.tcc"
> *DecodeOpLDE_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10812 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10816 "isa_tms320.tcc"
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
#line 10830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10840 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10849 "isa_tms320.tcc"
> *DecodeOpLDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10855 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10859 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10870 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10877 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10886 "isa_tms320.tcc"
> *DecodeOpLDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10892 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10896 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10914 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10923 "isa_tms320.tcc"
> *DecodeOpLDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10929 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10933 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10951 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10960 "isa_tms320.tcc"
> *DecodeOpLDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10966 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10970 "isa_tms320.tcc"
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
#line 10984 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10994 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11003 "isa_tms320.tcc"
> *DecodeOpLDFcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11009 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11013 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11031 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11036 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11040 "isa_tms320.tcc"
> *DecodeOpLDFcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11046 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11050 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11068 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11077 "isa_tms320.tcc"
> *DecodeOpLDFcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11083 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11087 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11105 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11114 "isa_tms320.tcc"
> *DecodeOpLDFcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11120 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11124 "isa_tms320.tcc"
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
#line 11138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11148 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11157 "isa_tms320.tcc"
> *DecodeOpLDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11163 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11167 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11185 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11194 "isa_tms320.tcc"
> *DecodeOpLDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11200 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11204 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11212 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11222 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11231 "isa_tms320.tcc"
> *DecodeOpLDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11237 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11241 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11249 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11259 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11268 "isa_tms320.tcc"
> *DecodeOpLDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11274 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11278 "isa_tms320.tcc"
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
#line 11291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11301 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 11305 "isa_tms320.tcc"
OpLDIcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 11309 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11313 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11317 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11320 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11324 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11327 "isa_tms320.tcc"
)
{
#line 111 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		os << "LDI" << COND_NAME[cond] << " " << REG_NAME[src] << ", " << REG_NAME[dst];
	}
#line 11334 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11350 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11359 "isa_tms320.tcc"
> *DecodeOpLDIcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11365 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11369 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11387 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11396 "isa_tms320.tcc"
> *DecodeOpLDIcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11402 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11406 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11424 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11433 "isa_tms320.tcc"
> *DecodeOpLDIcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11439 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11443 "isa_tms320.tcc"
	>(code, addr);
}

//LDIcond_indir.disasm = {
	//	os << "LDI" << COND_NAME[cond] << " " <<
	//}

template <
#line 45 "isa/tms320.isa"
class
#line 11454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11464 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11473 "isa_tms320.tcc"
> *DecodeOpLDIcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11479 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11483 "isa_tms320.tcc"
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
#line 11497 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11507 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11512 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11516 "isa_tms320.tcc"
> *DecodeOpLDM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11522 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11526 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11534 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11544 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11549 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11553 "isa_tms320.tcc"
> *DecodeOpLDM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11559 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11563 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11581 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11590 "isa_tms320.tcc"
> *DecodeOpLDM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11600 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11618 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11627 "isa_tms320.tcc"
> *DecodeOpLDM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11637 "isa_tms320.tcc"
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
#line 11651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11661 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11670 "isa_tms320.tcc"
> *DecodeOpLDP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11676 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11680 "isa_tms320.tcc"
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
#line 11693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11700 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11703 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 11707 "isa_tms320.tcc"
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 11711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11715 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11719 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11722 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11726 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11729 "isa_tms320.tcc"
)
{
#line 158 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		if(dst < 28)
		os << "POP " << REG_NAME[dst];
		else
		os << "?";
	}
#line 11739 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11752 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11755 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11764 "isa_tms320.tcc"
> *DecodeOpPOP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11770 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11774 "isa_tms320.tcc"
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
#line 11787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11794 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11797 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 11801 "isa_tms320.tcc"
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 11805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11809 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11813 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11816 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11820 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11823 "isa_tms320.tcc"
)
{
#line 175 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		if(dst < 8)
		os << "POPF " << REG_NAME[dst];
		else
		os << "?";
	}
#line 11833 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11846 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11849 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11858 "isa_tms320.tcc"
> *DecodeOpPOPF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11864 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11868 "isa_tms320.tcc"
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
#line 11881 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11884 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11891 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 11895 "isa_tms320.tcc"
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 11899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11903 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11907 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11910 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11914 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11917 "isa_tms320.tcc"
)
{
#line 192 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		if(src < 28)
		os << "PUSH " << REG_NAME[src];
		else
		os << "?";
	}
#line 11927 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11940 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11943 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11952 "isa_tms320.tcc"
> *DecodeOpPUSH(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11958 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11962 "isa_tms320.tcc"
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
#line 11975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11982 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11985 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 11989 "isa_tms320.tcc"
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 11993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11997 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12001 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12004 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12008 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12011 "isa_tms320.tcc"
)
{
#line 209 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		if(src < 8)
		os << "PUSH " << REG_NAME[src];
		else
		os << "?";
	}
#line 12021 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12037 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12046 "isa_tms320.tcc"
> *DecodeOpPUSHF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12052 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12056 "isa_tms320.tcc"
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
#line 12070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12080 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12089 "isa_tms320.tcc"
> *DecodeOpSTF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12095 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12099 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12107 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12117 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12126 "isa_tms320.tcc"
> *DecodeOpSTF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12132 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12136 "isa_tms320.tcc"
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
#line 12150 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12160 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12169 "isa_tms320.tcc"
> *DecodeOpSTI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12175 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12179 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12197 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12206 "isa_tms320.tcc"
> *DecodeOpSTI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12212 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12216 "isa_tms320.tcc"
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
#line 12301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12311 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12320 "isa_tms320.tcc"
> *DecodeOpABSF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12326 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12330 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12348 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12357 "isa_tms320.tcc"
> *DecodeOpABSF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12363 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12367 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12385 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12394 "isa_tms320.tcc"
> *DecodeOpABSF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12400 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12404 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12422 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12431 "isa_tms320.tcc"
> *DecodeOpABSF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12437 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12441 "isa_tms320.tcc"
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
#line 12455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12465 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12474 "isa_tms320.tcc"
> *DecodeOpABSI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12480 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12484 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12495 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12502 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12511 "isa_tms320.tcc"
> *DecodeOpABSI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12517 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12521 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12539 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12548 "isa_tms320.tcc"
> *DecodeOpABSI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12554 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12558 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12566 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12569 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12576 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12585 "isa_tms320.tcc"
> *DecodeOpABSI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12591 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12595 "isa_tms320.tcc"
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
#line 12609 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12619 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12628 "isa_tms320.tcc"
> *DecodeOpADDC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12634 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12638 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12649 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12653 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12656 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12665 "isa_tms320.tcc"
> *DecodeOpADDC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12671 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12675 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12693 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12702 "isa_tms320.tcc"
> *DecodeOpADDC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12708 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12712 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12730 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12739 "isa_tms320.tcc"
> *DecodeOpADDC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12745 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12749 "isa_tms320.tcc"
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
#line 12763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12773 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12782 "isa_tms320.tcc"
> *DecodeOpADDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12788 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12792 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12800 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12807 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12810 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12819 "isa_tms320.tcc"
> *DecodeOpADDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12825 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12829 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12844 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12847 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12856 "isa_tms320.tcc"
> *DecodeOpADDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12862 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12866 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12881 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12884 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12893 "isa_tms320.tcc"
> *DecodeOpADDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12899 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12903 "isa_tms320.tcc"
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
#line 12917 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12924 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12927 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12936 "isa_tms320.tcc"
> *DecodeOpADDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12942 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12946 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12954 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12957 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12964 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12969 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12973 "isa_tms320.tcc"
> *DecodeOpADDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12979 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12983 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13001 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13010 "isa_tms320.tcc"
> *DecodeOpADDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13016 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13020 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13038 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13047 "isa_tms320.tcc"
> *DecodeOpADDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13053 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13057 "isa_tms320.tcc"
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
#line 13071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13081 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13086 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13090 "isa_tms320.tcc"
> *DecodeOpAND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13096 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13100 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13118 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13127 "isa_tms320.tcc"
> *DecodeOpAND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13133 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13137 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13155 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13164 "isa_tms320.tcc"
> *DecodeOpAND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13170 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13174 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13192 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13201 "isa_tms320.tcc"
> *DecodeOpAND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13207 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13211 "isa_tms320.tcc"
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
#line 13225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13235 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13244 "isa_tms320.tcc"
> *DecodeOpANDN_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13250 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13254 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13262 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13265 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13272 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13281 "isa_tms320.tcc"
> *DecodeOpANDN_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13287 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13291 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13309 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13318 "isa_tms320.tcc"
> *DecodeOpANDN_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13324 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13328 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13336 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13339 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13346 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13355 "isa_tms320.tcc"
> *DecodeOpANDN_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13361 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13365 "isa_tms320.tcc"
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
#line 13379 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13389 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13398 "isa_tms320.tcc"
> *DecodeOpASH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13404 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13408 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13416 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13426 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13435 "isa_tms320.tcc"
> *DecodeOpASH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13441 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13445 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13453 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13463 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13472 "isa_tms320.tcc"
> *DecodeOpASH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13478 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13482 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13490 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13493 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13497 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13500 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13505 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13509 "isa_tms320.tcc"
> *DecodeOpASH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13515 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13519 "isa_tms320.tcc"
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
#line 13533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13543 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13552 "isa_tms320.tcc"
> *DecodeOpCMPF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13562 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13570 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13577 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13580 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13589 "isa_tms320.tcc"
> *DecodeOpCMPF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13595 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13599 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13610 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13614 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13617 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13626 "isa_tms320.tcc"
> *DecodeOpCMPF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13632 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13636 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13644 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13647 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13654 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13663 "isa_tms320.tcc"
> *DecodeOpCMPF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13669 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13673 "isa_tms320.tcc"
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
#line 13687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13697 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13706 "isa_tms320.tcc"
> *DecodeOpCMPI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13712 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13716 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13734 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13743 "isa_tms320.tcc"
> *DecodeOpCMPI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13749 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13753 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13771 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13780 "isa_tms320.tcc"
> *DecodeOpCMPI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13786 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13790 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13808 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13817 "isa_tms320.tcc"
> *DecodeOpCMPI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13823 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13827 "isa_tms320.tcc"
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
#line 13841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13851 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13860 "isa_tms320.tcc"
> *DecodeOpFIX_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13866 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13870 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13878 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13888 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13897 "isa_tms320.tcc"
> *DecodeOpFIX_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13903 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13907 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13918 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13925 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13930 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13934 "isa_tms320.tcc"
> *DecodeOpFIX_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13940 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13944 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13952 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13962 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13971 "isa_tms320.tcc"
> *DecodeOpFIX_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13977 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13981 "isa_tms320.tcc"
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
#line 13995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14005 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14014 "isa_tms320.tcc"
> *DecodeOpFLOAT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14020 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14024 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14042 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14051 "isa_tms320.tcc"
> *DecodeOpFLOAT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14057 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14061 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14069 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14072 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14076 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14079 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14084 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14088 "isa_tms320.tcc"
> *DecodeOpFLOAT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14094 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14098 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14106 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14113 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14116 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14125 "isa_tms320.tcc"
> *DecodeOpFLOAT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14131 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14135 "isa_tms320.tcc"
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
#line 14149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14159 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14168 "isa_tms320.tcc"
> *DecodeOpLSH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14174 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14178 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14186 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14196 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14205 "isa_tms320.tcc"
> *DecodeOpLSH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14211 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14215 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14223 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14233 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14242 "isa_tms320.tcc"
> *DecodeOpLSH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14248 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14252 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14270 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14279 "isa_tms320.tcc"
> *DecodeOpLSH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14285 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14289 "isa_tms320.tcc"
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
#line 14303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14313 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14322 "isa_tms320.tcc"
> *DecodeOpMPYF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14328 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14332 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14350 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14359 "isa_tms320.tcc"
> *DecodeOpMPYF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14365 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14369 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14387 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14396 "isa_tms320.tcc"
> *DecodeOpMPYF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14402 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14406 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14424 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14433 "isa_tms320.tcc"
> *DecodeOpMPYF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14439 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14443 "isa_tms320.tcc"
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
#line 14457 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14467 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14472 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14476 "isa_tms320.tcc"
> *DecodeOpMPYI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14482 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14486 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14504 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14513 "isa_tms320.tcc"
> *DecodeOpMPYI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14519 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14523 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14541 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14550 "isa_tms320.tcc"
> *DecodeOpMPYI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14556 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14560 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14578 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14587 "isa_tms320.tcc"
> *DecodeOpMPYI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14593 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14597 "isa_tms320.tcc"
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
#line 14611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14621 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14630 "isa_tms320.tcc"
> *DecodeOpNEGB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14636 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14640 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14658 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14667 "isa_tms320.tcc"
> *DecodeOpNEGB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14673 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14677 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14695 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14704 "isa_tms320.tcc"
> *DecodeOpNEGB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14710 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14714 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14725 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14729 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14732 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14741 "isa_tms320.tcc"
> *DecodeOpNEGB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14747 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14751 "isa_tms320.tcc"
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
#line 14765 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14768 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14775 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14784 "isa_tms320.tcc"
> *DecodeOpNEGF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14790 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14794 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14812 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14821 "isa_tms320.tcc"
> *DecodeOpNEGF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14827 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14831 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14846 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14849 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14858 "isa_tms320.tcc"
> *DecodeOpNEGF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14864 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14868 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14886 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14895 "isa_tms320.tcc"
> *DecodeOpNEGF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14901 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14905 "isa_tms320.tcc"
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
#line 14919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14929 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14938 "isa_tms320.tcc"
> *DecodeOpNEGI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14948 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14966 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14975 "isa_tms320.tcc"
> *DecodeOpNEGI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14981 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14985 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15003 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15012 "isa_tms320.tcc"
> *DecodeOpNEGI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15018 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15022 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15040 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15049 "isa_tms320.tcc"
> *DecodeOpNEGI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15059 "isa_tms320.tcc"
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
#line 15073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15083 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15092 "isa_tms320.tcc"
> *DecodeOpNORM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15102 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15120 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15129 "isa_tms320.tcc"
> *DecodeOpNORM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15135 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15139 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15147 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15150 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15157 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15162 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15166 "isa_tms320.tcc"
> *DecodeOpNORM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15172 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15176 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15184 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15194 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15203 "isa_tms320.tcc"
> *DecodeOpNORM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15209 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15213 "isa_tms320.tcc"
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
#line 15227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15237 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15246 "isa_tms320.tcc"
> *DecodeOpNOT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15256 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15274 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15283 "isa_tms320.tcc"
> *DecodeOpNOT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15289 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15293 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15311 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15320 "isa_tms320.tcc"
> *DecodeOpNOT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15326 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15330 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15348 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15357 "isa_tms320.tcc"
> *DecodeOpNOT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15363 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15367 "isa_tms320.tcc"
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
#line 15381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15391 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15396 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15400 "isa_tms320.tcc"
> *DecodeOpOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15406 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15410 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15421 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15428 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15437 "isa_tms320.tcc"
> *DecodeOpOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15443 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15447 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15465 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15474 "isa_tms320.tcc"
> *DecodeOpOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15480 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15484 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15495 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15502 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15511 "isa_tms320.tcc"
> *DecodeOpOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15517 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15521 "isa_tms320.tcc"
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
#line 15535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15545 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15554 "isa_tms320.tcc"
> *DecodeOpRND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15560 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15564 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15572 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15575 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15582 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15591 "isa_tms320.tcc"
> *DecodeOpRND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15597 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15601 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15609 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15619 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15628 "isa_tms320.tcc"
> *DecodeOpRND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15634 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15638 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15649 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15653 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15656 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15665 "isa_tms320.tcc"
> *DecodeOpRND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15671 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15675 "isa_tms320.tcc"
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
#line 15689 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15699 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15708 "isa_tms320.tcc"
> *DecodeOpROL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15714 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15718 "isa_tms320.tcc"
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
#line 15732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15739 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15742 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15751 "isa_tms320.tcc"
> *DecodeOpROLC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15757 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15761 "isa_tms320.tcc"
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
#line 15775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15785 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15794 "isa_tms320.tcc"
> *DecodeOpROR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15800 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15804 "isa_tms320.tcc"
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
#line 15818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15828 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15837 "isa_tms320.tcc"
> *DecodeOpRORC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15843 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15847 "isa_tms320.tcc"
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
#line 15861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15871 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15880 "isa_tms320.tcc"
> *DecodeOpSUBB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15886 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15890 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15908 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15913 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15917 "isa_tms320.tcc"
> *DecodeOpSUBB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15923 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15927 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15942 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15945 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15950 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15954 "isa_tms320.tcc"
> *DecodeOpSUBB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15960 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15964 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15982 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15991 "isa_tms320.tcc"
> *DecodeOpSUBB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15997 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16001 "isa_tms320.tcc"
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
#line 16015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16025 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16034 "isa_tms320.tcc"
> *DecodeOpSUBC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16040 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16044 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16062 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16071 "isa_tms320.tcc"
> *DecodeOpSUBC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16077 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16081 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16099 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16108 "isa_tms320.tcc"
> *DecodeOpSUBC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16114 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16118 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16136 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16145 "isa_tms320.tcc"
> *DecodeOpSUBC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16151 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16155 "isa_tms320.tcc"
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
#line 16169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16179 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16188 "isa_tms320.tcc"
> *DecodeOpSUBF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16194 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16198 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16216 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16225 "isa_tms320.tcc"
> *DecodeOpSUBF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16231 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16235 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16250 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16253 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16262 "isa_tms320.tcc"
> *DecodeOpSUBF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16268 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16272 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16290 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16299 "isa_tms320.tcc"
> *DecodeOpSUBF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16305 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16309 "isa_tms320.tcc"
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
#line 16323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16333 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16342 "isa_tms320.tcc"
> *DecodeOpSUBI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16348 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16352 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16370 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16375 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16379 "isa_tms320.tcc"
> *DecodeOpSUBI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16385 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16389 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16397 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16404 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16407 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16412 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16416 "isa_tms320.tcc"
> *DecodeOpSUBI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16422 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16426 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16434 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16444 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16449 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16453 "isa_tms320.tcc"
> *DecodeOpSUBI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16459 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16463 "isa_tms320.tcc"
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
#line 16477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16487 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16496 "isa_tms320.tcc"
> *DecodeOpSUBRB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16502 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16506 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16524 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16533 "isa_tms320.tcc"
> *DecodeOpSUBRB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16539 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16543 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16561 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16570 "isa_tms320.tcc"
> *DecodeOpSUBRB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16576 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16580 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16598 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16607 "isa_tms320.tcc"
> *DecodeOpSUBRB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16613 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16617 "isa_tms320.tcc"
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
#line 16631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16638 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16641 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16646 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16650 "isa_tms320.tcc"
> *DecodeOpSUBRF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16656 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16660 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16675 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16678 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16687 "isa_tms320.tcc"
> *DecodeOpSUBRF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16693 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16697 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16715 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16720 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16724 "isa_tms320.tcc"
> *DecodeOpSUBRF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16730 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16734 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16752 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16761 "isa_tms320.tcc"
> *DecodeOpSUBRF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16767 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16771 "isa_tms320.tcc"
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
#line 16785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16792 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16795 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16804 "isa_tms320.tcc"
> *DecodeOpSUBRI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16810 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16814 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16832 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16841 "isa_tms320.tcc"
> *DecodeOpSUBRI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16847 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16851 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16862 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16866 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16869 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16878 "isa_tms320.tcc"
> *DecodeOpSUBRI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16884 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16888 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16903 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16906 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16915 "isa_tms320.tcc"
> *DecodeOpSUBRI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16921 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16925 "isa_tms320.tcc"
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
#line 16939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16949 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16958 "isa_tms320.tcc"
> *DecodeOpTSTB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16964 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16968 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16976 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16986 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16995 "isa_tms320.tcc"
> *DecodeOpTSTB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17001 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17005 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17020 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17023 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17032 "isa_tms320.tcc"
> *DecodeOpTSTB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17038 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17042 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17057 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17060 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17065 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17069 "isa_tms320.tcc"
> *DecodeOpTSTB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17075 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17079 "isa_tms320.tcc"
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
#line 17093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17100 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17103 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17112 "isa_tms320.tcc"
> *DecodeOpXOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17118 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17122 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17140 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17149 "isa_tms320.tcc"
> *DecodeOpXOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17155 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17159 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17167 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17174 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17177 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17182 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17186 "isa_tms320.tcc"
> *DecodeOpXOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17192 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17196 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17214 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17223 "isa_tms320.tcc"
> *DecodeOpXOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17229 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17233 "isa_tms320.tcc"
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
#line 17300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17307 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17310 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17319 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17325 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17329 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17337 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17347 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17356 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17362 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17366 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17384 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17393 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17399 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17403 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17421 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17430 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17436 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17440 "isa_tms320.tcc"
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
#line 17454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17464 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17473 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17479 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17483 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17501 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17510 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17516 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17520 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17538 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17547 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17557 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17572 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17575 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17584 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17590 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17594 "isa_tms320.tcc"
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
#line 17608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17618 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17627 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17637 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17645 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17652 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17655 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17664 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17670 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17674 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17682 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17685 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17689 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17692 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17697 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17701 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17707 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17711 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17719 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17726 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17729 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17734 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17738 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17744 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17748 "isa_tms320.tcc"
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
#line 17762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17772 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17781 "isa_tms320.tcc"
> *DecodeOpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17787 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17791 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17799 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17809 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17818 "isa_tms320.tcc"
> *DecodeOpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17824 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17828 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17846 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17855 "isa_tms320.tcc"
> *DecodeOpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17861 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17865 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17883 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17892 "isa_tms320.tcc"
> *DecodeOpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17898 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17902 "isa_tms320.tcc"
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
#line 17916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17926 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17935 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17941 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17945 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17963 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17972 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17978 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17982 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17997 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18000 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18005 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18009 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18015 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18019 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18037 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18046 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18052 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18056 "isa_tms320.tcc"
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
#line 18070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18080 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18089 "isa_tms320.tcc"
> *DecodeOpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18095 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18099 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18107 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18117 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18126 "isa_tms320.tcc"
> *DecodeOpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18132 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18136 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18154 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18163 "isa_tms320.tcc"
> *DecodeOpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18169 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18173 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18191 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18200 "isa_tms320.tcc"
> *DecodeOpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18206 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18210 "isa_tms320.tcc"
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
#line 18224 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18234 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18243 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18249 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18253 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18261 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18271 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18280 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18290 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18308 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18317 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18323 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18327 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18342 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18345 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18354 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18360 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18364 "isa_tms320.tcc"
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
#line 18378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18388 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18397 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18403 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18407 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18425 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18434 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18440 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18444 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18452 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18462 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18471 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18477 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18481 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18489 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18499 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18508 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18514 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18518 "isa_tms320.tcc"
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
#line 18532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18542 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18551 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18557 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18561 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18579 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18588 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18594 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18598 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18616 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18625 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18631 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18635 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18643 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18646 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18650 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18653 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18658 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18662 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18668 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18672 "isa_tms320.tcc"
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
#line 18686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18696 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18705 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18711 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18715 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18733 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18742 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18748 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18752 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18760 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18763 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18770 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18779 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18785 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18789 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18807 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18816 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18822 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18826 "isa_tms320.tcc"
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
#line 18840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18847 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18850 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18859 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18865 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18869 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18887 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18896 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18902 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18906 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18924 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18933 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18939 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18943 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18961 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18970 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18976 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18980 "isa_tms320.tcc"
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
#line 18994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18997 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19004 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19013 "isa_tms320.tcc"
> *DecodeOpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19019 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19023 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19031 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19034 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19041 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19050 "isa_tms320.tcc"
> *DecodeOpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19056 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19060 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19078 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19087 "isa_tms320.tcc"
> *DecodeOpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19093 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19097 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19115 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19124 "isa_tms320.tcc"
> *DecodeOpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19130 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19134 "isa_tms320.tcc"
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
#line 19148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19158 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19167 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19173 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19177 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19195 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19204 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19210 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19214 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19222 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19225 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19229 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19232 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19241 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19247 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19251 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19269 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19278 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19284 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19288 "isa_tms320.tcc"
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
#line 19302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19312 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19321 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19327 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19331 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19349 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19358 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19364 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19368 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19386 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19395 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19401 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19405 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19413 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19420 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19423 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19432 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19438 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19442 "isa_tms320.tcc"
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
#line 19456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19466 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19475 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19485 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19493 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19503 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19512 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19518 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19522 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19530 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19533 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19540 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19549 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19555 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19559 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19567 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19577 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19586 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19592 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19596 "isa_tms320.tcc"
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
#line 19610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19620 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19629 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19635 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19639 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19657 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19666 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19672 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19676 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19694 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19703 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19709 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19713 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19731 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19740 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19746 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19750 "isa_tms320.tcc"
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
#line 19764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19774 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19783 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19789 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19793 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19801 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19811 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19820 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19826 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19830 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19848 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19857 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19863 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19867 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19882 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19885 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19894 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19900 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19904 "isa_tms320.tcc"
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
#line 19971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19981 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19990 "isa_tms320.tcc"
> *DecodeOpBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19996 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20000 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20018 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20027 "isa_tms320.tcc"
> *DecodeOpBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20033 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20037 "isa_tms320.tcc"
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
#line 20051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20061 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20070 "isa_tms320.tcc"
> *DecodeOpBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20076 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20080 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20098 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20107 "isa_tms320.tcc"
> *DecodeOpBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20113 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20117 "isa_tms320.tcc"
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
#line 20131 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20141 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20146 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20150 "isa_tms320.tcc"
> *DecodeOpBR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20156 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20160 "isa_tms320.tcc"
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
#line 20174 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20184 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20193 "isa_tms320.tcc"
> *DecodeOpBRD(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20199 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20203 "isa_tms320.tcc"
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
#line 20217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20220 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20224 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20227 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20232 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20236 "isa_tms320.tcc"
> *DecodeOpCALL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20242 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20246 "isa_tms320.tcc"
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
#line 20260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20270 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20279 "isa_tms320.tcc"
> *DecodeOpCALLcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20285 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20289 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20297 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20304 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20307 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20312 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20316 "isa_tms320.tcc"
> *DecodeOpCALLcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20322 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20326 "isa_tms320.tcc"
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
#line 20340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20350 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20359 "isa_tms320.tcc"
> *DecodeOpDBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20365 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20369 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20387 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20396 "isa_tms320.tcc"
> *DecodeOpDBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20402 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20406 "isa_tms320.tcc"
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
#line 20420 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20430 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20439 "isa_tms320.tcc"
> *DecodeOpDBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20445 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20449 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20457 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20467 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20472 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20476 "isa_tms320.tcc"
> *DecodeOpDBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20482 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20486 "isa_tms320.tcc"
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
#line 20500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20510 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20519 "isa_tms320.tcc"
> *DecodeOpIACK_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20525 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20529 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20547 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20556 "isa_tms320.tcc"
> *DecodeOpIACK_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20562 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20566 "isa_tms320.tcc"
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
#line 20580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20590 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20599 "isa_tms320.tcc"
> *DecodeOpIDLE(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20605 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20609 "isa_tms320.tcc"
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
#line 20623 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20630 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20633 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20638 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20642 "isa_tms320.tcc"
> *DecodeOpNOP_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20648 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20652 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20670 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20679 "isa_tms320.tcc"
> *DecodeOpNOP_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20685 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20689 "isa_tms320.tcc"
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
#line 20703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20710 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20713 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20722 "isa_tms320.tcc"
> *DecodeOpRETIcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20728 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20732 "isa_tms320.tcc"
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
#line 20746 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20753 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20756 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20765 "isa_tms320.tcc"
> *DecodeOpRETScond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20771 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20775 "isa_tms320.tcc"
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
#line 20789 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20792 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20799 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20808 "isa_tms320.tcc"
> *DecodeOpRPTB(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20814 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20818 "isa_tms320.tcc"
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
#line 20832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20842 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20847 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20851 "isa_tms320.tcc"
> *DecodeOpRPTS_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20857 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20861 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20879 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20884 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20888 "isa_tms320.tcc"
> *DecodeOpRPTS_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20894 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20898 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20916 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20925 "isa_tms320.tcc"
> *DecodeOpRPTS_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20931 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20935 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20953 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20962 "isa_tms320.tcc"
> *DecodeOpRPTS_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20968 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20972 "isa_tms320.tcc"
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
#line 20986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20996 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21005 "isa_tms320.tcc"
> *DecodeOpSWI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21011 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21015 "isa_tms320.tcc"
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
#line 21029 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21039 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21048 "isa_tms320.tcc"
> *DecodeOpTRAPcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21054 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21058 "isa_tms320.tcc"
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
> *DecodeOpIDLE2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21136 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21140 "isa_tms320.tcc"
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
#line 21154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21164 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21173 "isa_tms320.tcc"
> *DecodeOpLOPOWER(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21179 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21183 "isa_tms320.tcc"
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
#line 21197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21207 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21216 "isa_tms320.tcc"
> *DecodeOpMAXSPEED(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21222 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21226 "isa_tms320.tcc"
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
#line 21281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21284 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21288 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21291 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21300 "isa_tms320.tcc"
> *DecodeOpLDFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21306 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21310 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21328 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21337 "isa_tms320.tcc"
> *DecodeOpLDFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21343 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21347 "isa_tms320.tcc"
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
#line 21361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21371 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21380 "isa_tms320.tcc"
> *DecodeOpLDII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21386 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21390 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21408 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21417 "isa_tms320.tcc"
> *DecodeOpLDII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21423 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21427 "isa_tms320.tcc"
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
#line 21441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21451 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21460 "isa_tms320.tcc"
> *DecodeOpSIGI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21466 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21470 "isa_tms320.tcc"
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
#line 21484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21494 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21503 "isa_tms320.tcc"
> *DecodeOpSTFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21509 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21513 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21531 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21540 "isa_tms320.tcc"
> *DecodeOpSTFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21546 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21550 "isa_tms320.tcc"
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
#line 21564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21574 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21583 "isa_tms320.tcc"
> *DecodeOpSTII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21589 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21593 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21604 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21611 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21620 "isa_tms320.tcc"
> *DecodeOpSTII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21626 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21630 "isa_tms320.tcc"
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
#line 21701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21711 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21720 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21726 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21730 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21748 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21757 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21763 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21767 "isa_tms320.tcc"
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
#line 21781 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21791 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21800 "isa_tms320.tcc"
> *DecodeOpLDF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21806 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21810 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21828 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21837 "isa_tms320.tcc"
> *DecodeOpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21843 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21847 "isa_tms320.tcc"
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
#line 21861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21871 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21880 "isa_tms320.tcc"
> *DecodeOpLDI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21886 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21890 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21908 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21913 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21917 "isa_tms320.tcc"
> *DecodeOpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21923 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21927 "isa_tms320.tcc"
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
#line 21941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21951 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21960 "isa_tms320.tcc"
> *DecodeOpLSH3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21966 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21970 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21988 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21997 "isa_tms320.tcc"
> *DecodeOpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22003 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22007 "isa_tms320.tcc"
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
#line 22021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22031 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22036 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22040 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22046 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22050 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22068 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22077 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22083 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22087 "isa_tms320.tcc"
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
#line 22101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22111 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22120 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22126 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22130 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22148 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22157 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22163 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22167 "isa_tms320.tcc"
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
#line 22181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22191 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22200 "isa_tms320.tcc"
> *DecodeOpNEGF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22206 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22210 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22228 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22237 "isa_tms320.tcc"
> *DecodeOpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22243 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22247 "isa_tms320.tcc"
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
#line 22261 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22271 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22280 "isa_tms320.tcc"
> *DecodeOpNEGI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22290 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22308 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22317 "isa_tms320.tcc"
> *DecodeOpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22323 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22327 "isa_tms320.tcc"
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
#line 22341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22344 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22351 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22360 "isa_tms320.tcc"
> *DecodeOpNOT_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22366 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22370 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22388 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22397 "isa_tms320.tcc"
> *DecodeOpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22403 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22407 "isa_tms320.tcc"
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
#line 22421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22431 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22440 "isa_tms320.tcc"
> *DecodeOpOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22446 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22450 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22468 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22473 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22477 "isa_tms320.tcc"
> *DecodeOpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22483 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22487 "isa_tms320.tcc"
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
#line 22501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22511 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22520 "isa_tms320.tcc"
> *DecodeOpSTF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22526 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22530 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22548 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22557 "isa_tms320.tcc"
> *DecodeOpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22563 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22567 "isa_tms320.tcc"
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
#line 22581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22591 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22600 "isa_tms320.tcc"
> *DecodeOpSTI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22606 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22610 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22628 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22637 "isa_tms320.tcc"
> *DecodeOpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22643 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22647 "isa_tms320.tcc"
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
#line 22661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22671 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22680 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22686 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22690 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22708 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22717 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22727 "isa_tms320.tcc"
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
#line 22741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22751 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22756 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22760 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22766 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22770 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22788 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22797 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22803 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22807 "isa_tms320.tcc"
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
#line 22821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22831 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22840 "isa_tms320.tcc"
> *DecodeOpXOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22846 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22850 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22868 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22877 "isa_tms320.tcc"
> *DecodeOpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22883 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22887 "isa_tms320.tcc"
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
#line 22901 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22911 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22920 "isa_tms320.tcc"
> *DecodeOpLDF_LDF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22926 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22930 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22945 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22948 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22957 "isa_tms320.tcc"
> *DecodeOpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22963 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22967 "isa_tms320.tcc"
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
#line 22981 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22984 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22991 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23000 "isa_tms320.tcc"
> *DecodeOpLDI_LDI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23006 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23010 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23028 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23037 "isa_tms320.tcc"
> *DecodeOpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23043 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23047 "isa_tms320.tcc"
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
#line 23061 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23071 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23080 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23086 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23090 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23108 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23117 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23123 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23127 "isa_tms320.tcc"
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
#line 23141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23151 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23160 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23166 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23170 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23188 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23197 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23203 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23207 "isa_tms320.tcc"
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
#line 23221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23231 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23240 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23246 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23250 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23268 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23277 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23283 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23287 "isa_tms320.tcc"
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
#line 23301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23311 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23320 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23326 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23330 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23348 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23357 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23363 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23367 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23384 "isa_tms320.tcc"
>
OpLDE_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23393 "isa_tms320.tcc"
>::OpLDE_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23401 "isa_tms320.tcc"
>(code, addr, "LDE_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23421 "isa_tms320.tcc"
>
OpLDE_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23430 "isa_tms320.tcc"
>::OpLDE_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23438 "isa_tms320.tcc"
>(code, addr, "LDE_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23458 "isa_tms320.tcc"
>
OpLDE_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23467 "isa_tms320.tcc"
>::OpLDE_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23475 "isa_tms320.tcc"
>(code, addr, "LDE_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23487 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23497 "isa_tms320.tcc"
>
OpLDE_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23506 "isa_tms320.tcc"
>::OpLDE_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23514 "isa_tms320.tcc"
>(code, addr, "LDE_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23524 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23534 "isa_tms320.tcc"
>
OpLDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23543 "isa_tms320.tcc"
>::OpLDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23551 "isa_tms320.tcc"
>(code, addr, "LDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23561 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23571 "isa_tms320.tcc"
>
OpLDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23580 "isa_tms320.tcc"
>::OpLDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23588 "isa_tms320.tcc"
>(code, addr, "LDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23605 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23608 "isa_tms320.tcc"
>
OpLDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23617 "isa_tms320.tcc"
>::OpLDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23625 "isa_tms320.tcc"
>(code, addr, "LDF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23637 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23644 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23647 "isa_tms320.tcc"
>
OpLDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23656 "isa_tms320.tcc"
>::OpLDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23664 "isa_tms320.tcc"
>(code, addr, "LDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23684 "isa_tms320.tcc"
>
OpLDFcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23693 "isa_tms320.tcc"
>::OpLDFcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23697 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23701 "isa_tms320.tcc"
>(code, addr, "LDFcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23719 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23722 "isa_tms320.tcc"
>
OpLDFcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23731 "isa_tms320.tcc"
>::OpLDFcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23739 "isa_tms320.tcc"
>(code, addr, "LDFcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23760 "isa_tms320.tcc"
>
OpLDFcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23769 "isa_tms320.tcc"
>::OpLDFcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23777 "isa_tms320.tcc"
>(code, addr, "LDFcond_indir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23790 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23800 "isa_tms320.tcc"
>
OpLDFcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23809 "isa_tms320.tcc"
>::OpLDFcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23817 "isa_tms320.tcc"
>(code, addr, "LDFcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23838 "isa_tms320.tcc"
>
OpLDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23847 "isa_tms320.tcc"
>::OpLDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23855 "isa_tms320.tcc"
>(code, addr, "LDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23875 "isa_tms320.tcc"
>
OpLDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23884 "isa_tms320.tcc"
>::OpLDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23892 "isa_tms320.tcc"
>(code, addr, "LDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23902 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23912 "isa_tms320.tcc"
>
OpLDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23921 "isa_tms320.tcc"
>::OpLDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23929 "isa_tms320.tcc"
>(code, addr, "LDI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23951 "isa_tms320.tcc"
>
OpLDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23960 "isa_tms320.tcc"
>::OpLDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23968 "isa_tms320.tcc"
>(code, addr, "LDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23988 "isa_tms320.tcc"
>
OpLDIcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23997 "isa_tms320.tcc"
>::OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24005 "isa_tms320.tcc"
>(code, addr, "LDIcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24026 "isa_tms320.tcc"
>
OpLDIcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24035 "isa_tms320.tcc"
>::OpLDIcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24043 "isa_tms320.tcc"
>(code, addr, "LDIcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24054 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24057 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24061 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24064 "isa_tms320.tcc"
>
OpLDIcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24069 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24073 "isa_tms320.tcc"
>::OpLDIcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24081 "isa_tms320.tcc"
>(code, addr, "LDIcond_indir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24094 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24104 "isa_tms320.tcc"
>
OpLDIcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24113 "isa_tms320.tcc"
>::OpLDIcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24121 "isa_tms320.tcc"
>(code, addr, "LDIcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24142 "isa_tms320.tcc"
>
OpLDM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24151 "isa_tms320.tcc"
>::OpLDM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24155 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24159 "isa_tms320.tcc"
>(code, addr, "LDM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24179 "isa_tms320.tcc"
>
OpLDM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24188 "isa_tms320.tcc"
>::OpLDM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24196 "isa_tms320.tcc"
>(code, addr, "LDM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24216 "isa_tms320.tcc"
>
OpLDM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24225 "isa_tms320.tcc"
>::OpLDM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24233 "isa_tms320.tcc"
>(code, addr, "LDM_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24245 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24248 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24252 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24255 "isa_tms320.tcc"
>
OpLDM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24260 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24264 "isa_tms320.tcc"
>::OpLDM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24272 "isa_tms320.tcc"
>(code, addr, "LDM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = ((code >> 0) & 0xfff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24289 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24292 "isa_tms320.tcc"
>
OpLDP<
#line 45 "isa/tms320.isa"
CONFIG
#line 24297 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24301 "isa_tms320.tcc"
>::OpLDP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24309 "isa_tms320.tcc"
>(code, addr, "LDP")
{
	src = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24328 "isa_tms320.tcc"
>
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 24333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24337 "isa_tms320.tcc"
>::OpPOP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24345 "isa_tms320.tcc"
>(code, addr, "POP")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24364 "isa_tms320.tcc"
>
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 24369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24373 "isa_tms320.tcc"
>::OpPOPF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24381 "isa_tms320.tcc"
>(code, addr, "POPF")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24397 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24400 "isa_tms320.tcc"
>
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 24405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24409 "isa_tms320.tcc"
>::OpPUSH(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24417 "isa_tms320.tcc"
>(code, addr, "PUSH")
{
	src = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24436 "isa_tms320.tcc"
>
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 24441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24445 "isa_tms320.tcc"
>::OpPUSHF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24449 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24453 "isa_tms320.tcc"
>(code, addr, "PUSHF")
{
	src = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24465 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24469 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24472 "isa_tms320.tcc"
>
OpSTF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24481 "isa_tms320.tcc"
>::OpSTF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24489 "isa_tms320.tcc"
>(code, addr, "STF_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24509 "isa_tms320.tcc"
>
OpSTF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24518 "isa_tms320.tcc"
>::OpSTF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24526 "isa_tms320.tcc"
>(code, addr, "STF_indir")
{
	src = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24548 "isa_tms320.tcc"
>
OpSTI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24557 "isa_tms320.tcc"
>::OpSTI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24565 "isa_tms320.tcc"
>(code, addr, "STI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24578 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24585 "isa_tms320.tcc"
>
OpSTI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24594 "isa_tms320.tcc"
>::OpSTI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24598 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24602 "isa_tms320.tcc"
>(code, addr, "STI_indir")
{
	src = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24614 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24624 "isa_tms320.tcc"
>
OpABSF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24633 "isa_tms320.tcc"
>::OpABSF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24641 "isa_tms320.tcc"
>(code, addr, "ABSF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24661 "isa_tms320.tcc"
>
OpABSF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24670 "isa_tms320.tcc"
>::OpABSF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24678 "isa_tms320.tcc"
>(code, addr, "ABSF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24698 "isa_tms320.tcc"
>
OpABSF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24707 "isa_tms320.tcc"
>::OpABSF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24715 "isa_tms320.tcc"
>(code, addr, "ABSF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24737 "isa_tms320.tcc"
>
OpABSF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24742 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24746 "isa_tms320.tcc"
>::OpABSF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24754 "isa_tms320.tcc"
>(code, addr, "ABSF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24774 "isa_tms320.tcc"
>
OpABSI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24783 "isa_tms320.tcc"
>::OpABSI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24791 "isa_tms320.tcc"
>(code, addr, "ABSI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24801 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24811 "isa_tms320.tcc"
>
OpABSI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24820 "isa_tms320.tcc"
>::OpABSI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24828 "isa_tms320.tcc"
>(code, addr, "ABSI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24848 "isa_tms320.tcc"
>
OpABSI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24857 "isa_tms320.tcc"
>::OpABSI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24865 "isa_tms320.tcc"
>(code, addr, "ABSI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24887 "isa_tms320.tcc"
>
OpABSI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24896 "isa_tms320.tcc"
>::OpABSI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24904 "isa_tms320.tcc"
>(code, addr, "ABSI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24924 "isa_tms320.tcc"
>
OpADDC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24933 "isa_tms320.tcc"
>::OpADDC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24941 "isa_tms320.tcc"
>(code, addr, "ADDC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24961 "isa_tms320.tcc"
>
OpADDC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24970 "isa_tms320.tcc"
>::OpADDC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24978 "isa_tms320.tcc"
>(code, addr, "ADDC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24998 "isa_tms320.tcc"
>
OpADDC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25007 "isa_tms320.tcc"
>::OpADDC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25015 "isa_tms320.tcc"
>(code, addr, "ADDC_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25037 "isa_tms320.tcc"
>
OpADDC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25046 "isa_tms320.tcc"
>::OpADDC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25050 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25054 "isa_tms320.tcc"
>(code, addr, "ADDC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25074 "isa_tms320.tcc"
>
OpADDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25083 "isa_tms320.tcc"
>::OpADDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25091 "isa_tms320.tcc"
>(code, addr, "ADDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25111 "isa_tms320.tcc"
>
OpADDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25120 "isa_tms320.tcc"
>::OpADDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25128 "isa_tms320.tcc"
>(code, addr, "ADDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25148 "isa_tms320.tcc"
>
OpADDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25157 "isa_tms320.tcc"
>::OpADDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25165 "isa_tms320.tcc"
>(code, addr, "ADDF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25177 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25184 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25187 "isa_tms320.tcc"
>
OpADDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25196 "isa_tms320.tcc"
>::OpADDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25204 "isa_tms320.tcc"
>(code, addr, "ADDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25214 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25217 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25224 "isa_tms320.tcc"
>
OpADDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25233 "isa_tms320.tcc"
>::OpADDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25241 "isa_tms320.tcc"
>(code, addr, "ADDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25261 "isa_tms320.tcc"
>
OpADDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25270 "isa_tms320.tcc"
>::OpADDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25278 "isa_tms320.tcc"
>(code, addr, "ADDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25288 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25298 "isa_tms320.tcc"
>
OpADDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25307 "isa_tms320.tcc"
>::OpADDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25315 "isa_tms320.tcc"
>(code, addr, "ADDI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25327 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25337 "isa_tms320.tcc"
>
OpADDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25346 "isa_tms320.tcc"
>::OpADDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25354 "isa_tms320.tcc"
>(code, addr, "ADDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25364 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25367 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25371 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25374 "isa_tms320.tcc"
>
OpAND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25383 "isa_tms320.tcc"
>::OpAND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25387 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25391 "isa_tms320.tcc"
>(code, addr, "AND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25411 "isa_tms320.tcc"
>
OpAND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25420 "isa_tms320.tcc"
>::OpAND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25428 "isa_tms320.tcc"
>(code, addr, "AND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25448 "isa_tms320.tcc"
>
OpAND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25453 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25457 "isa_tms320.tcc"
>::OpAND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25465 "isa_tms320.tcc"
>(code, addr, "AND_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25487 "isa_tms320.tcc"
>
OpAND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25496 "isa_tms320.tcc"
>::OpAND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25504 "isa_tms320.tcc"
>(code, addr, "AND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25524 "isa_tms320.tcc"
>
OpANDN_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25533 "isa_tms320.tcc"
>::OpANDN_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25541 "isa_tms320.tcc"
>(code, addr, "ANDN_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25561 "isa_tms320.tcc"
>
OpANDN_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25570 "isa_tms320.tcc"
>::OpANDN_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25578 "isa_tms320.tcc"
>(code, addr, "ANDN_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25598 "isa_tms320.tcc"
>
OpANDN_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25607 "isa_tms320.tcc"
>::OpANDN_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25615 "isa_tms320.tcc"
>(code, addr, "ANDN_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25627 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25630 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25634 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25637 "isa_tms320.tcc"
>
OpANDN_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25646 "isa_tms320.tcc"
>::OpANDN_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25654 "isa_tms320.tcc"
>(code, addr, "ANDN_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25664 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25671 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25674 "isa_tms320.tcc"
>
OpASH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25683 "isa_tms320.tcc"
>::OpASH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25691 "isa_tms320.tcc"
>(code, addr, "ASH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25711 "isa_tms320.tcc"
>
OpASH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25720 "isa_tms320.tcc"
>::OpASH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25728 "isa_tms320.tcc"
>(code, addr, "ASH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25748 "isa_tms320.tcc"
>
OpASH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25757 "isa_tms320.tcc"
>::OpASH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25765 "isa_tms320.tcc"
>(code, addr, "ASH_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25777 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25784 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25787 "isa_tms320.tcc"
>
OpASH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25792 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25796 "isa_tms320.tcc"
>::OpASH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25804 "isa_tms320.tcc"
>(code, addr, "ASH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25824 "isa_tms320.tcc"
>
OpCMPF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25833 "isa_tms320.tcc"
>::OpCMPF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25841 "isa_tms320.tcc"
>(code, addr, "CMPF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25861 "isa_tms320.tcc"
>
OpCMPF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25870 "isa_tms320.tcc"
>::OpCMPF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25878 "isa_tms320.tcc"
>(code, addr, "CMPF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25898 "isa_tms320.tcc"
>
OpCMPF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25907 "isa_tms320.tcc"
>::OpCMPF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25915 "isa_tms320.tcc"
>(code, addr, "CMPF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25930 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25937 "isa_tms320.tcc"
>
OpCMPF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25946 "isa_tms320.tcc"
>::OpCMPF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25950 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25954 "isa_tms320.tcc"
>(code, addr, "CMPF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25974 "isa_tms320.tcc"
>
OpCMPI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25983 "isa_tms320.tcc"
>::OpCMPI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25991 "isa_tms320.tcc"
>(code, addr, "CMPI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26011 "isa_tms320.tcc"
>
OpCMPI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26020 "isa_tms320.tcc"
>::OpCMPI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26028 "isa_tms320.tcc"
>(code, addr, "CMPI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26048 "isa_tms320.tcc"
>
OpCMPI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26057 "isa_tms320.tcc"
>::OpCMPI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26065 "isa_tms320.tcc"
>(code, addr, "CMPI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26087 "isa_tms320.tcc"
>
OpCMPI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26096 "isa_tms320.tcc"
>::OpCMPI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26100 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26104 "isa_tms320.tcc"
>(code, addr, "CMPI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26124 "isa_tms320.tcc"
>
OpFIX_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26133 "isa_tms320.tcc"
>::OpFIX_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26141 "isa_tms320.tcc"
>(code, addr, "FIX_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26161 "isa_tms320.tcc"
>
OpFIX_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26170 "isa_tms320.tcc"
>::OpFIX_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26178 "isa_tms320.tcc"
>(code, addr, "FIX_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26198 "isa_tms320.tcc"
>
OpFIX_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26207 "isa_tms320.tcc"
>::OpFIX_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26215 "isa_tms320.tcc"
>(code, addr, "FIX_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26237 "isa_tms320.tcc"
>
OpFIX_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26246 "isa_tms320.tcc"
>::OpFIX_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26254 "isa_tms320.tcc"
>(code, addr, "FIX_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26274 "isa_tms320.tcc"
>
OpFLOAT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26283 "isa_tms320.tcc"
>::OpFLOAT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26291 "isa_tms320.tcc"
>(code, addr, "FLOAT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26311 "isa_tms320.tcc"
>
OpFLOAT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26320 "isa_tms320.tcc"
>::OpFLOAT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26328 "isa_tms320.tcc"
>(code, addr, "FLOAT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26348 "isa_tms320.tcc"
>
OpFLOAT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26357 "isa_tms320.tcc"
>::OpFLOAT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26365 "isa_tms320.tcc"
>(code, addr, "FLOAT_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26387 "isa_tms320.tcc"
>
OpFLOAT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26396 "isa_tms320.tcc"
>::OpFLOAT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26404 "isa_tms320.tcc"
>(code, addr, "FLOAT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26424 "isa_tms320.tcc"
>
OpLSH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26433 "isa_tms320.tcc"
>::OpLSH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26441 "isa_tms320.tcc"
>(code, addr, "LSH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26461 "isa_tms320.tcc"
>
OpLSH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26470 "isa_tms320.tcc"
>::OpLSH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26478 "isa_tms320.tcc"
>(code, addr, "LSH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26498 "isa_tms320.tcc"
>
OpLSH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26507 "isa_tms320.tcc"
>::OpLSH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26515 "isa_tms320.tcc"
>(code, addr, "LSH_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26527 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26534 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26537 "isa_tms320.tcc"
>
OpLSH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26542 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26546 "isa_tms320.tcc"
>::OpLSH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26554 "isa_tms320.tcc"
>(code, addr, "LSH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26574 "isa_tms320.tcc"
>
OpMPYF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26583 "isa_tms320.tcc"
>::OpMPYF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26591 "isa_tms320.tcc"
>(code, addr, "MPYF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26604 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26611 "isa_tms320.tcc"
>
OpMPYF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26620 "isa_tms320.tcc"
>::OpMPYF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26628 "isa_tms320.tcc"
>(code, addr, "MPYF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26638 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26641 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26645 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26648 "isa_tms320.tcc"
>
OpMPYF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26657 "isa_tms320.tcc"
>::OpMPYF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26665 "isa_tms320.tcc"
>(code, addr, "MPYF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26677 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26687 "isa_tms320.tcc"
>
OpMPYF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26696 "isa_tms320.tcc"
>::OpMPYF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26704 "isa_tms320.tcc"
>(code, addr, "MPYF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26724 "isa_tms320.tcc"
>
OpMPYI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26733 "isa_tms320.tcc"
>::OpMPYI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26741 "isa_tms320.tcc"
>(code, addr, "MPYI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26754 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26758 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26761 "isa_tms320.tcc"
>
OpMPYI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26770 "isa_tms320.tcc"
>::OpMPYI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26778 "isa_tms320.tcc"
>(code, addr, "MPYI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26798 "isa_tms320.tcc"
>
OpMPYI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26807 "isa_tms320.tcc"
>::OpMPYI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26815 "isa_tms320.tcc"
>(code, addr, "MPYI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26827 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26830 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26834 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26837 "isa_tms320.tcc"
>
OpMPYI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26846 "isa_tms320.tcc"
>::OpMPYI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26850 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26854 "isa_tms320.tcc"
>(code, addr, "MPYI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26871 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26874 "isa_tms320.tcc"
>
OpNEGB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26883 "isa_tms320.tcc"
>::OpNEGB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26891 "isa_tms320.tcc"
>(code, addr, "NEGB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26901 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26911 "isa_tms320.tcc"
>
OpNEGB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26920 "isa_tms320.tcc"
>::OpNEGB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26928 "isa_tms320.tcc"
>(code, addr, "NEGB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26945 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26948 "isa_tms320.tcc"
>
OpNEGB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26957 "isa_tms320.tcc"
>::OpNEGB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26965 "isa_tms320.tcc"
>(code, addr, "NEGB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26977 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26980 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26984 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26987 "isa_tms320.tcc"
>
OpNEGB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26992 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26996 "isa_tms320.tcc"
>::OpNEGB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27004 "isa_tms320.tcc"
>(code, addr, "NEGB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27024 "isa_tms320.tcc"
>
OpNEGF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27033 "isa_tms320.tcc"
>::OpNEGF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27041 "isa_tms320.tcc"
>(code, addr, "NEGF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27061 "isa_tms320.tcc"
>
OpNEGF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27070 "isa_tms320.tcc"
>::OpNEGF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27078 "isa_tms320.tcc"
>(code, addr, "NEGF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27098 "isa_tms320.tcc"
>
OpNEGF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27107 "isa_tms320.tcc"
>::OpNEGF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27115 "isa_tms320.tcc"
>(code, addr, "NEGF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27134 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27137 "isa_tms320.tcc"
>
OpNEGF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27146 "isa_tms320.tcc"
>::OpNEGF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27150 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27154 "isa_tms320.tcc"
>(code, addr, "NEGF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27167 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27174 "isa_tms320.tcc"
>
OpNEGI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27183 "isa_tms320.tcc"
>::OpNEGI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27191 "isa_tms320.tcc"
>(code, addr, "NEGI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27211 "isa_tms320.tcc"
>
OpNEGI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27220 "isa_tms320.tcc"
>::OpNEGI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27228 "isa_tms320.tcc"
>(code, addr, "NEGI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27241 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27245 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27248 "isa_tms320.tcc"
>
OpNEGI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27257 "isa_tms320.tcc"
>::OpNEGI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27265 "isa_tms320.tcc"
>(code, addr, "NEGI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27287 "isa_tms320.tcc"
>
OpNEGI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27296 "isa_tms320.tcc"
>::OpNEGI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27304 "isa_tms320.tcc"
>(code, addr, "NEGI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27324 "isa_tms320.tcc"
>
OpNORM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27333 "isa_tms320.tcc"
>::OpNORM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27341 "isa_tms320.tcc"
>(code, addr, "NORM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27361 "isa_tms320.tcc"
>
OpNORM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27370 "isa_tms320.tcc"
>::OpNORM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27378 "isa_tms320.tcc"
>(code, addr, "NORM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27398 "isa_tms320.tcc"
>
OpNORM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27407 "isa_tms320.tcc"
>::OpNORM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27415 "isa_tms320.tcc"
>(code, addr, "NORM_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27434 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27437 "isa_tms320.tcc"
>
OpNORM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27446 "isa_tms320.tcc"
>::OpNORM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27454 "isa_tms320.tcc"
>(code, addr, "NORM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27474 "isa_tms320.tcc"
>
OpNOT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27483 "isa_tms320.tcc"
>::OpNOT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27491 "isa_tms320.tcc"
>(code, addr, "NOT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27511 "isa_tms320.tcc"
>
OpNOT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27520 "isa_tms320.tcc"
>::OpNOT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27528 "isa_tms320.tcc"
>(code, addr, "NOT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27548 "isa_tms320.tcc"
>
OpNOT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27557 "isa_tms320.tcc"
>::OpNOT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27565 "isa_tms320.tcc"
>(code, addr, "NOT_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27577 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27587 "isa_tms320.tcc"
>
OpNOT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27592 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27596 "isa_tms320.tcc"
>::OpNOT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27604 "isa_tms320.tcc"
>(code, addr, "NOT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27614 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27624 "isa_tms320.tcc"
>
OpOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27633 "isa_tms320.tcc"
>::OpOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27641 "isa_tms320.tcc"
>(code, addr, "OR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27661 "isa_tms320.tcc"
>
OpOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27670 "isa_tms320.tcc"
>::OpOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27678 "isa_tms320.tcc"
>(code, addr, "OR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27698 "isa_tms320.tcc"
>
OpOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27707 "isa_tms320.tcc"
>::OpOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27715 "isa_tms320.tcc"
>(code, addr, "OR_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27737 "isa_tms320.tcc"
>
OpOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27742 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27746 "isa_tms320.tcc"
>::OpOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27754 "isa_tms320.tcc"
>(code, addr, "OR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27774 "isa_tms320.tcc"
>
OpRND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27783 "isa_tms320.tcc"
>::OpRND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27791 "isa_tms320.tcc"
>(code, addr, "RND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27801 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27811 "isa_tms320.tcc"
>
OpRND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27820 "isa_tms320.tcc"
>::OpRND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27828 "isa_tms320.tcc"
>(code, addr, "RND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27848 "isa_tms320.tcc"
>
OpRND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27857 "isa_tms320.tcc"
>::OpRND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27865 "isa_tms320.tcc"
>(code, addr, "RND_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27887 "isa_tms320.tcc"
>
OpRND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27896 "isa_tms320.tcc"
>::OpRND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27904 "isa_tms320.tcc"
>(code, addr, "RND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27924 "isa_tms320.tcc"
>
OpROL<
#line 45 "isa/tms320.isa"
CONFIG
#line 27929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27933 "isa_tms320.tcc"
>::OpROL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27941 "isa_tms320.tcc"
>(code, addr, "ROL")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27957 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27960 "isa_tms320.tcc"
>
OpROLC<
#line 45 "isa/tms320.isa"
CONFIG
#line 27965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27969 "isa_tms320.tcc"
>::OpROLC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27973 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27977 "isa_tms320.tcc"
>(code, addr, "ROLC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27996 "isa_tms320.tcc"
>
OpROR<
#line 45 "isa/tms320.isa"
CONFIG
#line 28001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28005 "isa_tms320.tcc"
>::OpROR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28013 "isa_tms320.tcc"
>(code, addr, "ROR")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28025 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28029 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28032 "isa_tms320.tcc"
>
OpRORC<
#line 45 "isa/tms320.isa"
CONFIG
#line 28037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28041 "isa_tms320.tcc"
>::OpRORC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28049 "isa_tms320.tcc"
>(code, addr, "RORC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28068 "isa_tms320.tcc"
>
OpSUBB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28077 "isa_tms320.tcc"
>::OpSUBB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28085 "isa_tms320.tcc"
>(code, addr, "SUBB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28105 "isa_tms320.tcc"
>
OpSUBB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28114 "isa_tms320.tcc"
>::OpSUBB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28122 "isa_tms320.tcc"
>(code, addr, "SUBB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28142 "isa_tms320.tcc"
>
OpSUBB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28151 "isa_tms320.tcc"
>::OpSUBB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28155 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28159 "isa_tms320.tcc"
>(code, addr, "SUBB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28181 "isa_tms320.tcc"
>
OpSUBB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28190 "isa_tms320.tcc"
>::OpSUBB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28198 "isa_tms320.tcc"
>(code, addr, "SUBB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28218 "isa_tms320.tcc"
>
OpSUBC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28227 "isa_tms320.tcc"
>::OpSUBC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28235 "isa_tms320.tcc"
>(code, addr, "SUBC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28245 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28248 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28252 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28255 "isa_tms320.tcc"
>
OpSUBC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28260 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28264 "isa_tms320.tcc"
>::OpSUBC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28272 "isa_tms320.tcc"
>(code, addr, "SUBC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28289 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28292 "isa_tms320.tcc"
>
OpSUBC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28297 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28301 "isa_tms320.tcc"
>::OpSUBC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28309 "isa_tms320.tcc"
>(code, addr, "SUBC_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28331 "isa_tms320.tcc"
>
OpSUBC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28340 "isa_tms320.tcc"
>::OpSUBC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28344 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28348 "isa_tms320.tcc"
>(code, addr, "SUBC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28368 "isa_tms320.tcc"
>
OpSUBF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28373 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28377 "isa_tms320.tcc"
>::OpSUBF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28385 "isa_tms320.tcc"
>(code, addr, "SUBF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28405 "isa_tms320.tcc"
>
OpSUBF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28414 "isa_tms320.tcc"
>::OpSUBF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28422 "isa_tms320.tcc"
>(code, addr, "SUBF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28432 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28439 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28442 "isa_tms320.tcc"
>
OpSUBF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28447 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28451 "isa_tms320.tcc"
>::OpSUBF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28459 "isa_tms320.tcc"
>(code, addr, "SUBF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28478 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28481 "isa_tms320.tcc"
>
OpSUBF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28490 "isa_tms320.tcc"
>::OpSUBF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28498 "isa_tms320.tcc"
>(code, addr, "SUBF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28518 "isa_tms320.tcc"
>
OpSUBI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28527 "isa_tms320.tcc"
>::OpSUBI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28535 "isa_tms320.tcc"
>(code, addr, "SUBI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28555 "isa_tms320.tcc"
>
OpSUBI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28560 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28564 "isa_tms320.tcc"
>::OpSUBI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28572 "isa_tms320.tcc"
>(code, addr, "SUBI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28589 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28592 "isa_tms320.tcc"
>
OpSUBI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28601 "isa_tms320.tcc"
>::OpSUBI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28609 "isa_tms320.tcc"
>(code, addr, "SUBI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28628 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28631 "isa_tms320.tcc"
>
OpSUBI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28640 "isa_tms320.tcc"
>::OpSUBI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28648 "isa_tms320.tcc"
>(code, addr, "SUBI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28668 "isa_tms320.tcc"
>
OpSUBRB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28677 "isa_tms320.tcc"
>::OpSUBRB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28685 "isa_tms320.tcc"
>(code, addr, "SUBRB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28705 "isa_tms320.tcc"
>
OpSUBRB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28714 "isa_tms320.tcc"
>::OpSUBRB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28722 "isa_tms320.tcc"
>(code, addr, "SUBRB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28739 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28742 "isa_tms320.tcc"
>
OpSUBRB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28751 "isa_tms320.tcc"
>::OpSUBRB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28755 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28759 "isa_tms320.tcc"
>(code, addr, "SUBRB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28781 "isa_tms320.tcc"
>
OpSUBRB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28790 "isa_tms320.tcc"
>::OpSUBRB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28794 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28798 "isa_tms320.tcc"
>(code, addr, "SUBRB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28818 "isa_tms320.tcc"
>
OpSUBRF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28827 "isa_tms320.tcc"
>::OpSUBRF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28835 "isa_tms320.tcc"
>(code, addr, "SUBRF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28855 "isa_tms320.tcc"
>
OpSUBRF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28864 "isa_tms320.tcc"
>::OpSUBRF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28872 "isa_tms320.tcc"
>(code, addr, "SUBRF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28882 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28892 "isa_tms320.tcc"
>
OpSUBRF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28897 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28901 "isa_tms320.tcc"
>::OpSUBRF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28909 "isa_tms320.tcc"
>(code, addr, "SUBRF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28931 "isa_tms320.tcc"
>
OpSUBRF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28940 "isa_tms320.tcc"
>::OpSUBRF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28948 "isa_tms320.tcc"
>(code, addr, "SUBRF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28968 "isa_tms320.tcc"
>
OpSUBRI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28973 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28977 "isa_tms320.tcc"
>::OpSUBRI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28985 "isa_tms320.tcc"
>(code, addr, "SUBRI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29005 "isa_tms320.tcc"
>
OpSUBRI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29014 "isa_tms320.tcc"
>::OpSUBRI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29022 "isa_tms320.tcc"
>(code, addr, "SUBRI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29042 "isa_tms320.tcc"
>
OpSUBRI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29051 "isa_tms320.tcc"
>::OpSUBRI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29059 "isa_tms320.tcc"
>(code, addr, "SUBRI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29081 "isa_tms320.tcc"
>
OpSUBRI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29086 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29090 "isa_tms320.tcc"
>::OpSUBRI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29098 "isa_tms320.tcc"
>(code, addr, "SUBRI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29118 "isa_tms320.tcc"
>
OpTSTB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29127 "isa_tms320.tcc"
>::OpTSTB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29135 "isa_tms320.tcc"
>(code, addr, "TSTB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29155 "isa_tms320.tcc"
>
OpTSTB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29164 "isa_tms320.tcc"
>::OpTSTB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29172 "isa_tms320.tcc"
>(code, addr, "TSTB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29192 "isa_tms320.tcc"
>
OpTSTB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29201 "isa_tms320.tcc"
>::OpTSTB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29209 "isa_tms320.tcc"
>(code, addr, "TSTB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29231 "isa_tms320.tcc"
>
OpTSTB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29240 "isa_tms320.tcc"
>::OpTSTB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29248 "isa_tms320.tcc"
>(code, addr, "TSTB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29268 "isa_tms320.tcc"
>
OpXOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29277 "isa_tms320.tcc"
>::OpXOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29285 "isa_tms320.tcc"
>(code, addr, "XOR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29305 "isa_tms320.tcc"
>
OpXOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29314 "isa_tms320.tcc"
>::OpXOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29322 "isa_tms320.tcc"
>(code, addr, "XOR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29335 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29342 "isa_tms320.tcc"
>
OpXOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29351 "isa_tms320.tcc"
>::OpXOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29359 "isa_tms320.tcc"
>(code, addr, "XOR_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29371 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29381 "isa_tms320.tcc"
>
OpXOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29390 "isa_tms320.tcc"
>::OpXOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29398 "isa_tms320.tcc"
>(code, addr, "XOR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29418 "isa_tms320.tcc"
>
OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29427 "isa_tms320.tcc"
>::OpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29435 "isa_tms320.tcc"
>(code, addr, "ADDC3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29446 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29449 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29453 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29456 "isa_tms320.tcc"
>
OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29465 "isa_tms320.tcc"
>::OpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29473 "isa_tms320.tcc"
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
#line 29485 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29495 "isa_tms320.tcc"
>
OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29504 "isa_tms320.tcc"
>::OpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29512 "isa_tms320.tcc"
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
#line 29524 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29534 "isa_tms320.tcc"
>
OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29543 "isa_tms320.tcc"
>::OpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29551 "isa_tms320.tcc"
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
#line 29564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29574 "isa_tms320.tcc"
>
OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29583 "isa_tms320.tcc"
>::OpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29591 "isa_tms320.tcc"
>(code, addr, "ADDF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29602 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29609 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29612 "isa_tms320.tcc"
>
OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29621 "isa_tms320.tcc"
>::OpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29629 "isa_tms320.tcc"
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
#line 29641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29651 "isa_tms320.tcc"
>
OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29660 "isa_tms320.tcc"
>::OpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29668 "isa_tms320.tcc"
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
#line 29680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29690 "isa_tms320.tcc"
>
OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29695 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29699 "isa_tms320.tcc"
>::OpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29707 "isa_tms320.tcc"
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
#line 29720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29730 "isa_tms320.tcc"
>
OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29739 "isa_tms320.tcc"
>::OpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29747 "isa_tms320.tcc"
>(code, addr, "ADDI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29758 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29765 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29768 "isa_tms320.tcc"
>
OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29777 "isa_tms320.tcc"
>::OpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29785 "isa_tms320.tcc"
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
#line 29797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29807 "isa_tms320.tcc"
>
OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29816 "isa_tms320.tcc"
>::OpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29824 "isa_tms320.tcc"
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
#line 29836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29846 "isa_tms320.tcc"
>
OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29855 "isa_tms320.tcc"
>::OpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29859 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29863 "isa_tms320.tcc"
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
#line 29876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29886 "isa_tms320.tcc"
>
OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29895 "isa_tms320.tcc"
>::OpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29903 "isa_tms320.tcc"
>(code, addr, "AND3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29924 "isa_tms320.tcc"
>
OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29933 "isa_tms320.tcc"
>::OpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29941 "isa_tms320.tcc"
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
#line 29953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29963 "isa_tms320.tcc"
>
OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29972 "isa_tms320.tcc"
>::OpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29980 "isa_tms320.tcc"
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
#line 29992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29999 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30002 "isa_tms320.tcc"
>
OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30011 "isa_tms320.tcc"
>::OpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30019 "isa_tms320.tcc"
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
#line 30032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30042 "isa_tms320.tcc"
>
OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30051 "isa_tms320.tcc"
>::OpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30059 "isa_tms320.tcc"
>(code, addr, "ANDN3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30080 "isa_tms320.tcc"
>
OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30089 "isa_tms320.tcc"
>::OpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30097 "isa_tms320.tcc"
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
#line 30109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30119 "isa_tms320.tcc"
>
OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30128 "isa_tms320.tcc"
>::OpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30136 "isa_tms320.tcc"
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
#line 30148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30158 "isa_tms320.tcc"
>
OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30167 "isa_tms320.tcc"
>::OpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30175 "isa_tms320.tcc"
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
#line 30188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30198 "isa_tms320.tcc"
>
OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30207 "isa_tms320.tcc"
>::OpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30215 "isa_tms320.tcc"
>(code, addr, "ASH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30233 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30236 "isa_tms320.tcc"
>
OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30241 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30245 "isa_tms320.tcc"
>::OpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30253 "isa_tms320.tcc"
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
#line 30265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30275 "isa_tms320.tcc"
>
OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30284 "isa_tms320.tcc"
>::OpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30292 "isa_tms320.tcc"
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
#line 30304 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30314 "isa_tms320.tcc"
>
OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30323 "isa_tms320.tcc"
>::OpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30331 "isa_tms320.tcc"
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
#line 30344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30354 "isa_tms320.tcc"
>
OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30363 "isa_tms320.tcc"
>::OpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30367 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30371 "isa_tms320.tcc"
>(code, addr, "CMPF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30392 "isa_tms320.tcc"
>
OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30401 "isa_tms320.tcc"
>::OpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30409 "isa_tms320.tcc"
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
#line 30421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30431 "isa_tms320.tcc"
>
OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30440 "isa_tms320.tcc"
>::OpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30448 "isa_tms320.tcc"
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
#line 30460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30470 "isa_tms320.tcc"
>
OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30475 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30479 "isa_tms320.tcc"
>::OpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30487 "isa_tms320.tcc"
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
#line 30500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30510 "isa_tms320.tcc"
>
OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30519 "isa_tms320.tcc"
>::OpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30527 "isa_tms320.tcc"
>(code, addr, "CMPI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30548 "isa_tms320.tcc"
>
OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30557 "isa_tms320.tcc"
>::OpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30565 "isa_tms320.tcc"
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
#line 30577 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30587 "isa_tms320.tcc"
>
OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30592 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30596 "isa_tms320.tcc"
>::OpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30604 "isa_tms320.tcc"
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
#line 30616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30623 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30626 "isa_tms320.tcc"
>
OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30631 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30635 "isa_tms320.tcc"
>::OpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30643 "isa_tms320.tcc"
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
OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30675 "isa_tms320.tcc"
>::OpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30683 "isa_tms320.tcc"
>(code, addr, "LSH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30697 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30704 "isa_tms320.tcc"
>
OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30709 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30713 "isa_tms320.tcc"
>::OpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30721 "isa_tms320.tcc"
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
#line 30733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30743 "isa_tms320.tcc"
>
OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30752 "isa_tms320.tcc"
>::OpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30756 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30760 "isa_tms320.tcc"
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
#line 30772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30782 "isa_tms320.tcc"
>
OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30791 "isa_tms320.tcc"
>::OpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30799 "isa_tms320.tcc"
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
#line 30812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30822 "isa_tms320.tcc"
>
OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30831 "isa_tms320.tcc"
>::OpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30839 "isa_tms320.tcc"
>(code, addr, "MPYF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30860 "isa_tms320.tcc"
>
OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30869 "isa_tms320.tcc"
>::OpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30877 "isa_tms320.tcc"
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
#line 30889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30899 "isa_tms320.tcc"
>
OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30908 "isa_tms320.tcc"
>::OpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30916 "isa_tms320.tcc"
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
#line 30928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30938 "isa_tms320.tcc"
>
OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30947 "isa_tms320.tcc"
>::OpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30955 "isa_tms320.tcc"
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
#line 30968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30978 "isa_tms320.tcc"
>
OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30987 "isa_tms320.tcc"
>::OpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30995 "isa_tms320.tcc"
>(code, addr, "MPYI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31016 "isa_tms320.tcc"
>
OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31025 "isa_tms320.tcc"
>::OpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31033 "isa_tms320.tcc"
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
#line 31045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31055 "isa_tms320.tcc"
>
OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31064 "isa_tms320.tcc"
>::OpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31072 "isa_tms320.tcc"
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
#line 31084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31094 "isa_tms320.tcc"
>
OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31099 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31103 "isa_tms320.tcc"
>::OpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31111 "isa_tms320.tcc"
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
#line 31124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31131 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31134 "isa_tms320.tcc"
>
OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31139 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31143 "isa_tms320.tcc"
>::OpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31151 "isa_tms320.tcc"
>(code, addr, "OR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31172 "isa_tms320.tcc"
>
OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31181 "isa_tms320.tcc"
>::OpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31189 "isa_tms320.tcc"
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
#line 31201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31211 "isa_tms320.tcc"
>
OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31220 "isa_tms320.tcc"
>::OpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31228 "isa_tms320.tcc"
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
#line 31240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31250 "isa_tms320.tcc"
>
OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31259 "isa_tms320.tcc"
>::OpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31267 "isa_tms320.tcc"
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
#line 31280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31290 "isa_tms320.tcc"
>
OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31299 "isa_tms320.tcc"
>::OpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31307 "isa_tms320.tcc"
>(code, addr, "SUBB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31328 "isa_tms320.tcc"
>
OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31337 "isa_tms320.tcc"
>::OpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31345 "isa_tms320.tcc"
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
#line 31357 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31364 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31367 "isa_tms320.tcc"
>
OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31376 "isa_tms320.tcc"
>::OpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31384 "isa_tms320.tcc"
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
#line 31396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31406 "isa_tms320.tcc"
>
OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31415 "isa_tms320.tcc"
>::OpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31423 "isa_tms320.tcc"
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
#line 31436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31446 "isa_tms320.tcc"
>
OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31455 "isa_tms320.tcc"
>::OpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31463 "isa_tms320.tcc"
>(code, addr, "SUBF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31484 "isa_tms320.tcc"
>
OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31489 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31493 "isa_tms320.tcc"
>::OpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31501 "isa_tms320.tcc"
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
#line 31513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31520 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31523 "isa_tms320.tcc"
>
OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31532 "isa_tms320.tcc"
>::OpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31540 "isa_tms320.tcc"
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
#line 31552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31559 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31562 "isa_tms320.tcc"
>
OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31571 "isa_tms320.tcc"
>::OpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31575 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31579 "isa_tms320.tcc"
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
#line 31592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31599 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31602 "isa_tms320.tcc"
>
OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31607 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31611 "isa_tms320.tcc"
>::OpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31619 "isa_tms320.tcc"
>(code, addr, "SUBI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31630 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31637 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31640 "isa_tms320.tcc"
>
OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31645 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31649 "isa_tms320.tcc"
>::OpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31657 "isa_tms320.tcc"
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
#line 31669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31679 "isa_tms320.tcc"
>
OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31688 "isa_tms320.tcc"
>::OpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31696 "isa_tms320.tcc"
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
#line 31708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31718 "isa_tms320.tcc"
>
OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31727 "isa_tms320.tcc"
>::OpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31735 "isa_tms320.tcc"
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
#line 31748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31758 "isa_tms320.tcc"
>
OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31763 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31767 "isa_tms320.tcc"
>::OpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31771 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31775 "isa_tms320.tcc"
>(code, addr, "TSTB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31796 "isa_tms320.tcc"
>
OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31805 "isa_tms320.tcc"
>::OpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31813 "isa_tms320.tcc"
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
#line 31825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31835 "isa_tms320.tcc"
>
OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31844 "isa_tms320.tcc"
>::OpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31852 "isa_tms320.tcc"
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
#line 31864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31871 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31874 "isa_tms320.tcc"
>
OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31883 "isa_tms320.tcc"
>::OpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31891 "isa_tms320.tcc"
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
#line 31904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31914 "isa_tms320.tcc"
>
OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31923 "isa_tms320.tcc"
>::OpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31931 "isa_tms320.tcc"
>(code, addr, "XOR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31942 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31949 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31952 "isa_tms320.tcc"
>
OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31957 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31961 "isa_tms320.tcc"
>::OpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31969 "isa_tms320.tcc"
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
#line 31981 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31984 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31991 "isa_tms320.tcc"
>
OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32000 "isa_tms320.tcc"
>::OpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32008 "isa_tms320.tcc"
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
#line 32020 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32030 "isa_tms320.tcc"
>
OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32039 "isa_tms320.tcc"
>::OpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32047 "isa_tms320.tcc"
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
#line 32060 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32063 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32067 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32070 "isa_tms320.tcc"
>
OpBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32075 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32079 "isa_tms320.tcc"
>::OpBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32087 "isa_tms320.tcc"
>(code, addr, "Bcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32097 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32100 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32104 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32107 "isa_tms320.tcc"
>
OpBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32116 "isa_tms320.tcc"
>::OpBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32124 "isa_tms320.tcc"
>(code, addr, "Bcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32134 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32144 "isa_tms320.tcc"
>
OpBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32149 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32153 "isa_tms320.tcc"
>::OpBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32161 "isa_tms320.tcc"
>(code, addr, "BcondD_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32181 "isa_tms320.tcc"
>
OpBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32190 "isa_tms320.tcc"
>::OpBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32198 "isa_tms320.tcc"
>(code, addr, "BcondD_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32218 "isa_tms320.tcc"
>
OpBR<
#line 45 "isa/tms320.isa"
CONFIG
#line 32223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32227 "isa_tms320.tcc"
>::OpBR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32235 "isa_tms320.tcc"
>(code, addr, "BR")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32254 "isa_tms320.tcc"
>
OpBRD<
#line 45 "isa/tms320.isa"
CONFIG
#line 32259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32263 "isa_tms320.tcc"
>::OpBRD(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32271 "isa_tms320.tcc"
>(code, addr, "BRD")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32290 "isa_tms320.tcc"
>
OpCALL<
#line 45 "isa/tms320.isa"
CONFIG
#line 32295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32299 "isa_tms320.tcc"
>::OpCALL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32307 "isa_tms320.tcc"
>(code, addr, "CALL")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32326 "isa_tms320.tcc"
>
OpCALLcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32335 "isa_tms320.tcc"
>::OpCALLcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32339 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32343 "isa_tms320.tcc"
>(code, addr, "CALLcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32363 "isa_tms320.tcc"
>
OpCALLcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32372 "isa_tms320.tcc"
>::OpCALLcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32380 "isa_tms320.tcc"
>(code, addr, "CALLcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32397 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32400 "isa_tms320.tcc"
>
OpDBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32409 "isa_tms320.tcc"
>::OpDBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32417 "isa_tms320.tcc"
>(code, addr, "DBcond_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32438 "isa_tms320.tcc"
>
OpDBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32447 "isa_tms320.tcc"
>::OpDBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32455 "isa_tms320.tcc"
>(code, addr, "DBcond_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32476 "isa_tms320.tcc"
>
OpDBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32485 "isa_tms320.tcc"
>::OpDBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32489 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32493 "isa_tms320.tcc"
>(code, addr, "DBcondD_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32514 "isa_tms320.tcc"
>
OpDBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32523 "isa_tms320.tcc"
>::OpDBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32531 "isa_tms320.tcc"
>(code, addr, "DBcondD_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32552 "isa_tms320.tcc"
>
OpIACK_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32557 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32561 "isa_tms320.tcc"
>::OpIACK_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32569 "isa_tms320.tcc"
>(code, addr, "IACK_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32588 "isa_tms320.tcc"
>
OpIACK_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32597 "isa_tms320.tcc"
>::OpIACK_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32605 "isa_tms320.tcc"
>(code, addr, "IACK_indir")
{
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32623 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32626 "isa_tms320.tcc"
>
OpIDLE<
#line 45 "isa/tms320.isa"
CONFIG
#line 32631 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32635 "isa_tms320.tcc"
>::OpIDLE(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32643 "isa_tms320.tcc"
>(code, addr, "IDLE")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32661 "isa_tms320.tcc"
>
OpNOP_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32670 "isa_tms320.tcc"
>::OpNOP_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32678 "isa_tms320.tcc"
>(code, addr, "NOP_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32697 "isa_tms320.tcc"
>
OpNOP_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32706 "isa_tms320.tcc"
>::OpNOP_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32714 "isa_tms320.tcc"
>(code, addr, "NOP_indir")
{
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32725 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32735 "isa_tms320.tcc"
>
OpRETIcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32744 "isa_tms320.tcc"
>::OpRETIcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32752 "isa_tms320.tcc"
>(code, addr, "RETIcond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32771 "isa_tms320.tcc"
>
OpRETScond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32780 "isa_tms320.tcc"
>::OpRETScond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32788 "isa_tms320.tcc"
>(code, addr, "RETScond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32807 "isa_tms320.tcc"
>
OpRPTB<
#line 45 "isa/tms320.isa"
CONFIG
#line 32812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32816 "isa_tms320.tcc"
>::OpRPTB(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32824 "isa_tms320.tcc"
>(code, addr, "RPTB")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32843 "isa_tms320.tcc"
>
OpRPTS_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32852 "isa_tms320.tcc"
>::OpRPTS_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32860 "isa_tms320.tcc"
>(code, addr, "RPTS_reg")
{
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32879 "isa_tms320.tcc"
>
OpRPTS_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32884 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32888 "isa_tms320.tcc"
>::OpRPTS_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32896 "isa_tms320.tcc"
>(code, addr, "RPTS_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32915 "isa_tms320.tcc"
>
OpRPTS_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32924 "isa_tms320.tcc"
>::OpRPTS_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32932 "isa_tms320.tcc"
>(code, addr, "RPTS_indir")
{
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32953 "isa_tms320.tcc"
>
OpRPTS_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 32958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32962 "isa_tms320.tcc"
>::OpRPTS_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32970 "isa_tms320.tcc"
>(code, addr, "RPTS_imm")
{
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32989 "isa_tms320.tcc"
>
OpSWI<
#line 45 "isa/tms320.isa"
CONFIG
#line 32994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32998 "isa_tms320.tcc"
>::OpSWI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33006 "isa_tms320.tcc"
>(code, addr, "SWI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33024 "isa_tms320.tcc"
>
OpTRAPcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 33029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33033 "isa_tms320.tcc"
>::OpTRAPcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33041 "isa_tms320.tcc"
>(code, addr, "TRAPcond")
{
	cond = ((code >> 16) & 0x1f);
	n = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33061 "isa_tms320.tcc"
>
OpIDLE2<
#line 45 "isa/tms320.isa"
CONFIG
#line 33066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33070 "isa_tms320.tcc"
>::OpIDLE2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33078 "isa_tms320.tcc"
>(code, addr, "IDLE2")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33096 "isa_tms320.tcc"
>
OpLOPOWER<
#line 45 "isa/tms320.isa"
CONFIG
#line 33101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33105 "isa_tms320.tcc"
>::OpLOPOWER(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33113 "isa_tms320.tcc"
>(code, addr, "LOPOWER")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33131 "isa_tms320.tcc"
>
OpMAXSPEED<
#line 45 "isa/tms320.isa"
CONFIG
#line 33136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33140 "isa_tms320.tcc"
>::OpMAXSPEED(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33148 "isa_tms320.tcc"
>(code, addr, "MAXSPEED")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33166 "isa_tms320.tcc"
>
OpLDFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33175 "isa_tms320.tcc"
>::OpLDFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33183 "isa_tms320.tcc"
>(code, addr, "LDFI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33203 "isa_tms320.tcc"
>
OpLDFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33208 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33212 "isa_tms320.tcc"
>::OpLDFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33220 "isa_tms320.tcc"
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
#line 33232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33242 "isa_tms320.tcc"
>
OpLDII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33251 "isa_tms320.tcc"
>::OpLDII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33259 "isa_tms320.tcc"
>(code, addr, "LDII_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33272 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33279 "isa_tms320.tcc"
>
OpLDII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33284 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33288 "isa_tms320.tcc"
>::OpLDII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33296 "isa_tms320.tcc"
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
#line 33308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33318 "isa_tms320.tcc"
>
OpSIGI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33323 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33327 "isa_tms320.tcc"
>::OpSIGI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33335 "isa_tms320.tcc"
>(code, addr, "SIGI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33350 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33353 "isa_tms320.tcc"
>
OpSTFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33362 "isa_tms320.tcc"
>::OpSTFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33370 "isa_tms320.tcc"
>(code, addr, "STFI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33390 "isa_tms320.tcc"
>
OpSTFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33399 "isa_tms320.tcc"
>::OpSTFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33407 "isa_tms320.tcc"
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
#line 33419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33422 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33429 "isa_tms320.tcc"
>
OpSTII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33438 "isa_tms320.tcc"
>::OpSTII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33446 "isa_tms320.tcc"
>(code, addr, "STII_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33466 "isa_tms320.tcc"
>
OpSTII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33475 "isa_tms320.tcc"
>::OpSTII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33483 "isa_tms320.tcc"
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
#line 33495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33502 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33505 "isa_tms320.tcc"
>
OpFLOAT_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33514 "isa_tms320.tcc"
>::OpFLOAT_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33522 "isa_tms320.tcc"
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
#line 33536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33546 "isa_tms320.tcc"
>
OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33555 "isa_tms320.tcc"
>::OpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33563 "isa_tms320.tcc"
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
#line 33576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33586 "isa_tms320.tcc"
>
OpLDF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33595 "isa_tms320.tcc"
>::OpLDF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33603 "isa_tms320.tcc"
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
#line 33617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33627 "isa_tms320.tcc"
>
OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33636 "isa_tms320.tcc"
>::OpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33644 "isa_tms320.tcc"
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
#line 33657 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33664 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33667 "isa_tms320.tcc"
>
OpLDI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33676 "isa_tms320.tcc"
>::OpLDI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33684 "isa_tms320.tcc"
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
#line 33698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33708 "isa_tms320.tcc"
>
OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33717 "isa_tms320.tcc"
>::OpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33721 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33725 "isa_tms320.tcc"
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
#line 33738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33748 "isa_tms320.tcc"
>
OpLSH3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33757 "isa_tms320.tcc"
>::OpLSH3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33765 "isa_tms320.tcc"
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
#line 33780 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33783 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33790 "isa_tms320.tcc"
>
OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33799 "isa_tms320.tcc"
>::OpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33807 "isa_tms320.tcc"
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
#line 33821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33831 "isa_tms320.tcc"
>
OpMPYF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33840 "isa_tms320.tcc"
>::OpMPYF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33848 "isa_tms320.tcc"
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
#line 33863 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33873 "isa_tms320.tcc"
>
OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33882 "isa_tms320.tcc"
>::OpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33890 "isa_tms320.tcc"
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
#line 33904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33914 "isa_tms320.tcc"
>
OpMPYI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33923 "isa_tms320.tcc"
>::OpMPYI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33931 "isa_tms320.tcc"
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
#line 33946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33956 "isa_tms320.tcc"
>
OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33965 "isa_tms320.tcc"
>::OpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33969 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33973 "isa_tms320.tcc"
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
#line 33987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33997 "isa_tms320.tcc"
>
OpNEGF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34006 "isa_tms320.tcc"
>::OpNEGF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34014 "isa_tms320.tcc"
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
#line 34028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34038 "isa_tms320.tcc"
>
OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34047 "isa_tms320.tcc"
>::OpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34055 "isa_tms320.tcc"
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
#line 34068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34078 "isa_tms320.tcc"
>
OpNEGI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34087 "isa_tms320.tcc"
>::OpNEGI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34095 "isa_tms320.tcc"
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
#line 34109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34119 "isa_tms320.tcc"
>
OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34128 "isa_tms320.tcc"
>::OpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34136 "isa_tms320.tcc"
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
#line 34149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34159 "isa_tms320.tcc"
>
OpNOT_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34168 "isa_tms320.tcc"
>::OpNOT_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34176 "isa_tms320.tcc"
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
#line 34190 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34200 "isa_tms320.tcc"
>
OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34209 "isa_tms320.tcc"
>::OpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34217 "isa_tms320.tcc"
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
#line 34230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34237 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34240 "isa_tms320.tcc"
>
OpOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34245 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34249 "isa_tms320.tcc"
>::OpOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34257 "isa_tms320.tcc"
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
#line 34272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34282 "isa_tms320.tcc"
>
OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34291 "isa_tms320.tcc"
>::OpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34299 "isa_tms320.tcc"
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
OpSTF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34332 "isa_tms320.tcc"
>::OpSTF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34340 "isa_tms320.tcc"
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
#line 34354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34364 "isa_tms320.tcc"
>
OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34373 "isa_tms320.tcc"
>::OpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34381 "isa_tms320.tcc"
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
#line 34394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34404 "isa_tms320.tcc"
>
OpSTI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34413 "isa_tms320.tcc"
>::OpSTI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34421 "isa_tms320.tcc"
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
#line 34435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34442 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34445 "isa_tms320.tcc"
>
OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34454 "isa_tms320.tcc"
>::OpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34462 "isa_tms320.tcc"
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
#line 34475 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34482 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34485 "isa_tms320.tcc"
>
OpSUBF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34494 "isa_tms320.tcc"
>::OpSUBF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34502 "isa_tms320.tcc"
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
#line 34517 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34520 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34524 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34527 "isa_tms320.tcc"
>
OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34536 "isa_tms320.tcc"
>::OpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34544 "isa_tms320.tcc"
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
#line 34558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34568 "isa_tms320.tcc"
>
OpSUBI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34577 "isa_tms320.tcc"
>::OpSUBI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34585 "isa_tms320.tcc"
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
#line 34600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34610 "isa_tms320.tcc"
>
OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34619 "isa_tms320.tcc"
>::OpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34627 "isa_tms320.tcc"
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
#line 34641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34651 "isa_tms320.tcc"
>
OpXOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34660 "isa_tms320.tcc"
>::OpXOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34668 "isa_tms320.tcc"
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
#line 34683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34693 "isa_tms320.tcc"
>
OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34702 "isa_tms320.tcc"
>::OpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34710 "isa_tms320.tcc"
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
#line 34724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34734 "isa_tms320.tcc"
>
OpLDF_LDF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34743 "isa_tms320.tcc"
>::OpLDF_LDF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34751 "isa_tms320.tcc"
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
#line 34765 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34768 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34775 "isa_tms320.tcc"
>
OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34784 "isa_tms320.tcc"
>::OpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34792 "isa_tms320.tcc"
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
#line 34805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34815 "isa_tms320.tcc"
>
OpLDI_LDI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34824 "isa_tms320.tcc"
>::OpLDI_LDI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34832 "isa_tms320.tcc"
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
#line 34846 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34849 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34853 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34856 "isa_tms320.tcc"
>
OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34865 "isa_tms320.tcc"
>::OpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34873 "isa_tms320.tcc"
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
#line 34886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34896 "isa_tms320.tcc"
>
OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34905 "isa_tms320.tcc"
>::OpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34913 "isa_tms320.tcc"
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
#line 34930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34937 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34940 "isa_tms320.tcc"
>
OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34949 "isa_tms320.tcc"
>::OpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34957 "isa_tms320.tcc"
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
#line 34972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34982 "isa_tms320.tcc"
>
OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34991 "isa_tms320.tcc"
>::OpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34999 "isa_tms320.tcc"
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
#line 35016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35026 "isa_tms320.tcc"
>
OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35035 "isa_tms320.tcc"
>::OpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35043 "isa_tms320.tcc"
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
#line 35058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35068 "isa_tms320.tcc"
>
OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 35073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35077 "isa_tms320.tcc"
>::OpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35085 "isa_tms320.tcc"
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
#line 35102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35105 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35112 "isa_tms320.tcc"
>
OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35121 "isa_tms320.tcc"
>::OpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35129 "isa_tms320.tcc"
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
#line 35144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35154 "isa_tms320.tcc"
>
OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 35159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35163 "isa_tms320.tcc"
>::OpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35167 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35171 "isa_tms320.tcc"
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
#line 35188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35198 "isa_tms320.tcc"
>
OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35207 "isa_tms320.tcc"
>::OpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35215 "isa_tms320.tcc"
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
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 35245 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35249 "isa_tms320.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 45 "isa/tms320.isa"
class
#line 35260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35270 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 35275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35279 "isa_tms320.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 45 "isa/tms320.isa"
class
#line 35290 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35297 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35300 "isa_tms320.tcc"
>
DecodeTableEntry<
#line 45 "isa/tms320.isa"
CONFIG
#line 35305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35309 "isa_tms320.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35317 "isa_tms320.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 45 "isa/tms320.isa"
class
#line 35328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35338 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 35343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35347 "isa_tms320.tcc"
>::Decoder()
{
	little_endian = false;
	mru_page = 0;
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35356 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35360 "isa_tms320.tcc"
	>(0x8c00e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35364 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35368 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35373 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35377 "isa_tms320.tcc"
	>(0x8c000000UL, 0xfc000000UL, DecodeOpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35381 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35385 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35390 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35394 "isa_tms320.tcc"
	>(0x8800e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35398 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35402 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35407 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35411 "isa_tms320.tcc"
	>(0x88000000UL, 0xfc000000UL, DecodeOpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35415 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35419 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35424 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35428 "isa_tms320.tcc"
	>(0x8400e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35432 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35436 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35441 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35445 "isa_tms320.tcc"
	>(0x84000000UL, 0xfc000000UL, DecodeOpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35449 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35453 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35458 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35462 "isa_tms320.tcc"
	>(0x8000e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35466 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35470 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35475 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35479 "isa_tms320.tcc"
	>(0x80000000UL, 0xfc000000UL, DecodeOpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35483 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35487 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35492 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35496 "isa_tms320.tcc"
	>(0xc60000e0UL, 0xfe0700e0UL, DecodeOpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35500 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35504 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35509 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35513 "isa_tms320.tcc"
	>(0xc6000000UL, 0xfe070000UL, DecodeOpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35517 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35521 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35526 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35530 "isa_tms320.tcc"
	>(0xc40000e0UL, 0xfe0700e0UL, DecodeOpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35534 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35538 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35543 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35547 "isa_tms320.tcc"
	>(0xc4000000UL, 0xfe070000UL, DecodeOpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35551 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35555 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35560 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35564 "isa_tms320.tcc"
	>(0xee0000e0UL, 0xfe0000e0UL, DecodeOpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35568 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35572 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35577 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35581 "isa_tms320.tcc"
	>(0xee000000UL, 0xfe000000UL, DecodeOpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35585 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35589 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35594 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35598 "isa_tms320.tcc"
	>(0xec0000e0UL, 0xfe0000e0UL, DecodeOpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35602 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35606 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35611 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35615 "isa_tms320.tcc"
	>(0xec000000UL, 0xfe000000UL, DecodeOpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35619 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35623 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35628 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35632 "isa_tms320.tcc"
	>(0xea0000e0UL, 0xfe0000e0UL, DecodeOpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35636 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35640 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35645 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35649 "isa_tms320.tcc"
	>(0xea000000UL, 0xfe000000UL, DecodeOpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35653 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35657 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35662 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35666 "isa_tms320.tcc"
	>(0xc20000e0UL, 0xfe3800e0UL, DecodeOpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35670 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35674 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35679 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35683 "isa_tms320.tcc"
	>(0xc2000000UL, 0xfe380000UL, DecodeOpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35687 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35691 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35696 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35700 "isa_tms320.tcc"
	>(0xc00000e0UL, 0xfe3800e0UL, DecodeOpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35704 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35708 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35713 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35717 "isa_tms320.tcc"
	>(0xc0000000UL, 0xfe380000UL, DecodeOpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35721 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35725 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35730 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35734 "isa_tms320.tcc"
	>(0xe80000e0UL, 0xfe0000e0UL, DecodeOpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35738 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35742 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35747 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35751 "isa_tms320.tcc"
	>(0xe8000000UL, 0xfe000000UL, DecodeOpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35755 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35759 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35764 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35768 "isa_tms320.tcc"
	>(0xe60000e0UL, 0xfe3800e0UL, DecodeOpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35772 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35776 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35781 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35785 "isa_tms320.tcc"
	>(0xe6000000UL, 0xfe380000UL, DecodeOpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35789 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35793 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35798 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35802 "isa_tms320.tcc"
	>(0xe40000e0UL, 0xfe3800e0UL, DecodeOpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35806 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35810 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35815 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35819 "isa_tms320.tcc"
	>(0xe4000000UL, 0xfe380000UL, DecodeOpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35823 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35827 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35832 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35836 "isa_tms320.tcc"
	>(0xe20000e0UL, 0xfe3800e0UL, DecodeOpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35840 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35844 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35849 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35853 "isa_tms320.tcc"
	>(0xe2000000UL, 0xfe380000UL, DecodeOpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35857 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35861 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35866 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35870 "isa_tms320.tcc"
	>(0xe00000e0UL, 0xfe0000e0UL, DecodeOpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35874 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35878 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35883 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35887 "isa_tms320.tcc"
	>(0xe0000000UL, 0xfe000000UL, DecodeOpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35891 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35895 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35900 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35904 "isa_tms320.tcc"
	>(0xde0000e0UL, 0xfe0000e0UL, DecodeOpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35908 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35912 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35917 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35921 "isa_tms320.tcc"
	>(0xde000000UL, 0xfe000000UL, DecodeOpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35925 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35929 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35934 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35938 "isa_tms320.tcc"
	>(0xdc0000e0UL, 0xfe0000e0UL, DecodeOpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35942 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35946 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35951 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35955 "isa_tms320.tcc"
	>(0xdc000000UL, 0xfe000000UL, DecodeOpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35959 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35963 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35968 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35972 "isa_tms320.tcc"
	>(0xda0000e0UL, 0xfe3800e0UL, DecodeOpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35976 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35980 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35985 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35989 "isa_tms320.tcc"
	>(0xda000000UL, 0xfe380000UL, DecodeOpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35993 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35997 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36002 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36006 "isa_tms320.tcc"
	>(0xd80000e0UL, 0xfe3800e0UL, DecodeOpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36010 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36014 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36019 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36023 "isa_tms320.tcc"
	>(0xd8000000UL, 0xfe380000UL, DecodeOpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36027 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36031 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36036 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36040 "isa_tms320.tcc"
	>(0xd60000e0UL, 0xfe3800e0UL, DecodeOpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36044 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36048 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36053 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36057 "isa_tms320.tcc"
	>(0xd6000000UL, 0xfe380000UL, DecodeOpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36061 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36065 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36070 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36074 "isa_tms320.tcc"
	>(0x15c00000UL, 0xffe00000UL, DecodeOpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36078 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36082 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36087 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36091 "isa_tms320.tcc"
	>(0x15a00000UL, 0xffe00000UL, DecodeOpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36095 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36099 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36104 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36108 "isa_tms320.tcc"
	>(0x14c00000UL, 0xffe00000UL, DecodeOpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36112 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36116 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36121 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36125 "isa_tms320.tcc"
	>(0x14a00000UL, 0xffe00000UL, DecodeOpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36129 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36133 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36138 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36142 "isa_tms320.tcc"
	>(0x16000000UL, 0xffffffffUL, DecodeOpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36146 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36150 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36155 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36159 "isa_tms320.tcc"
	>(0x8c00000UL, 0xffe00000UL, DecodeOpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36163 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36167 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36172 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36176 "isa_tms320.tcc"
	>(0x8a00000UL, 0xffe00000UL, DecodeOpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36180 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36184 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36189 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36193 "isa_tms320.tcc"
	>(0x7c00000UL, 0xffe00000UL, DecodeOpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36197 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36201 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36206 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36210 "isa_tms320.tcc"
	>(0x7a00000UL, 0xffe00000UL, DecodeOpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36214 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36218 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36223 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36227 "isa_tms320.tcc"
	>(0x10800000UL, 0xffffffffUL, DecodeOpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36231 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36235 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36240 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36244 "isa_tms320.tcc"
	>(0x10800001UL, 0xffffffffUL, DecodeOpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36248 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36252 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36257 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36261 "isa_tms320.tcc"
	>(0x6000001UL, 0xffffffffUL, DecodeOpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36265 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36269 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36274 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36278 "isa_tms320.tcc"
	>(0x74000020UL, 0xffe0ffe0UL, DecodeOpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36282 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36286 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36291 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36295 "isa_tms320.tcc"
	>(0x66000000UL, 0xffffffffUL, DecodeOpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36299 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36303 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36308 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36312 "isa_tms320.tcc"
	>(0x13fb0000UL, 0xffff0000UL, DecodeOpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36316 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36320 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36325 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36329 "isa_tms320.tcc"
	>(0x13db0000UL, 0xffff0000UL, DecodeOpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36333 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36337 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36342 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36346 "isa_tms320.tcc"
	>(0x13bb0000UL, 0xffff0000UL, DecodeOpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36350 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36354 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36359 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36363 "isa_tms320.tcc"
	>(0x139b0000UL, 0xffffffe0UL, DecodeOpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36367 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36371 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36376 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36380 "isa_tms320.tcc"
	>(0x64000000UL, 0xff000000UL, DecodeOpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36384 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36388 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36393 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36397 "isa_tms320.tcc"
	>(0x78800000UL, 0xffe0ffffUL, DecodeOpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36401 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36405 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36410 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36414 "isa_tms320.tcc"
	>(0x78000000UL, 0xffe0ffffUL, DecodeOpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36418 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36422 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36427 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36431 "isa_tms320.tcc"
	>(0xcc00000UL, 0xffff0000UL, DecodeOpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36435 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36439 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36444 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36448 "isa_tms320.tcc"
	>(0xca00000UL, 0xffff0000UL, DecodeOpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36452 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36456 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36461 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36465 "isa_tms320.tcc"
	>(0x6000000UL, 0xffffffffUL, DecodeOpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36469 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36473 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36478 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36482 "isa_tms320.tcc"
	>(0x1b400000UL, 0xffff0000UL, DecodeOpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36486 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36490 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36495 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36499 "isa_tms320.tcc"
	>(0x1b200000UL, 0xffff0000UL, DecodeOpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36503 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36507 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36512 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36516 "isa_tms320.tcc"
	>(0x6e200000UL, 0xfe200000UL, DecodeOpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36520 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36524 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36529 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36533 "isa_tms320.tcc"
	>(0x6c200000UL, 0xfe20ffe0UL, DecodeOpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36537 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36541 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36546 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36550 "isa_tms320.tcc"
	>(0x6e000000UL, 0xfe200000UL, DecodeOpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36554 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36558 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36563 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36567 "isa_tms320.tcc"
	>(0x6c000000UL, 0xfe20ffe0UL, DecodeOpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36571 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36575 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36580 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36584 "isa_tms320.tcc"
	>(0x72000000UL, 0xffe00000UL, DecodeOpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36588 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36592 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36597 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36601 "isa_tms320.tcc"
	>(0x70000000UL, 0xffe0ffe0UL, DecodeOpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36605 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36609 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36614 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36618 "isa_tms320.tcc"
	>(0x62000000UL, 0xff000000UL, DecodeOpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36622 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36626 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36631 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36635 "isa_tms320.tcc"
	>(0x61000000UL, 0xff000000UL, DecodeOpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36639 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36643 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36648 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36652 "isa_tms320.tcc"
	>(0x60000000UL, 0xff000000UL, DecodeOpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36656 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36660 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36665 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36669 "isa_tms320.tcc"
	>(0x6a200000UL, 0xffe00000UL, DecodeOpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36673 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36677 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36682 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36686 "isa_tms320.tcc"
	>(0x68200000UL, 0xffe0ffe0UL, DecodeOpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36690 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36694 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36699 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36703 "isa_tms320.tcc"
	>(0x6a000000UL, 0xffe00000UL, DecodeOpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36707 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36711 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36716 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36720 "isa_tms320.tcc"
	>(0x68000000UL, 0xffe0ffe0UL, DecodeOpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36724 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36728 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36733 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36737 "isa_tms320.tcc"
	>(0x28600000UL, 0xffe00000UL, DecodeOpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36741 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36745 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36750 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36754 "isa_tms320.tcc"
	>(0x28400000UL, 0xffe0e000UL, DecodeOpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36758 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36762 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36767 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36771 "isa_tms320.tcc"
	>(0x28200000UL, 0xffe000e0UL, DecodeOpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36775 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36779 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36784 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36788 "isa_tms320.tcc"
	>(0x28000000UL, 0xffe0e0e0UL, DecodeOpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36792 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36796 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36801 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36805 "isa_tms320.tcc"
	>(0x27e00000UL, 0xffe00000UL, DecodeOpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36809 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36813 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36818 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36822 "isa_tms320.tcc"
	>(0x27c00000UL, 0xffe0e000UL, DecodeOpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36826 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36830 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36835 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36839 "isa_tms320.tcc"
	>(0x27a00000UL, 0xffe000e0UL, DecodeOpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36843 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36847 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36852 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36856 "isa_tms320.tcc"
	>(0x27800000UL, 0xffe0e0e0UL, DecodeOpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36860 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36864 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36869 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36873 "isa_tms320.tcc"
	>(0x27600000UL, 0xffe00000UL, DecodeOpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36877 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36881 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36886 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36890 "isa_tms320.tcc"
	>(0x27400000UL, 0xffe0e000UL, DecodeOpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36894 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36898 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36903 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36907 "isa_tms320.tcc"
	>(0x27200000UL, 0xffe000e0UL, DecodeOpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36911 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36915 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36920 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36924 "isa_tms320.tcc"
	>(0x27000000UL, 0xffe0e0e0UL, DecodeOpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36928 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36932 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36937 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36941 "isa_tms320.tcc"
	>(0x26e00000UL, 0xffe00000UL, DecodeOpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36945 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36949 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36954 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36958 "isa_tms320.tcc"
	>(0x26c00000UL, 0xffe0e000UL, DecodeOpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36962 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36966 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36971 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36975 "isa_tms320.tcc"
	>(0x26a00000UL, 0xffe000e0UL, DecodeOpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36979 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36983 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36988 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36992 "isa_tms320.tcc"
	>(0x26800000UL, 0xffe0e0e0UL, DecodeOpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36996 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37000 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37005 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37009 "isa_tms320.tcc"
	>(0x26600000UL, 0xffe00000UL, DecodeOpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37013 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37017 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37022 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37026 "isa_tms320.tcc"
	>(0x26400000UL, 0xffe0e000UL, DecodeOpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37030 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37034 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37039 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37043 "isa_tms320.tcc"
	>(0x26200000UL, 0xffe000e0UL, DecodeOpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37047 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37051 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37056 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37060 "isa_tms320.tcc"
	>(0x26000000UL, 0xffe0e0e0UL, DecodeOpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37064 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37068 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37073 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37077 "isa_tms320.tcc"
	>(0x25e00000UL, 0xffe00000UL, DecodeOpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37081 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37085 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37090 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37094 "isa_tms320.tcc"
	>(0x25c00000UL, 0xffe0e000UL, DecodeOpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37102 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37107 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37111 "isa_tms320.tcc"
	>(0x25a00000UL, 0xffe000e0UL, DecodeOpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37115 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37119 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37124 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37128 "isa_tms320.tcc"
	>(0x25800000UL, 0xffe0e0e0UL, DecodeOpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37132 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37136 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37141 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37145 "isa_tms320.tcc"
	>(0x25600000UL, 0xffe00000UL, DecodeOpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37149 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37153 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37158 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37162 "isa_tms320.tcc"
	>(0x25400000UL, 0xffe0e000UL, DecodeOpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37166 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37170 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37175 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37179 "isa_tms320.tcc"
	>(0x25200000UL, 0xffe000e0UL, DecodeOpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37183 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37187 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37192 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37196 "isa_tms320.tcc"
	>(0x25000000UL, 0xffe0e0e0UL, DecodeOpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37200 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37204 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37209 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37213 "isa_tms320.tcc"
	>(0x24e00000UL, 0xffe00000UL, DecodeOpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37217 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37221 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37226 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37230 "isa_tms320.tcc"
	>(0x24c00000UL, 0xffe0e000UL, DecodeOpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37234 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37238 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37243 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37247 "isa_tms320.tcc"
	>(0x24a00000UL, 0xffe000e0UL, DecodeOpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37251 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37255 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37260 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37264 "isa_tms320.tcc"
	>(0x24800000UL, 0xffe0e0e0UL, DecodeOpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37268 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37272 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37277 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37281 "isa_tms320.tcc"
	>(0x24600000UL, 0xffe00000UL, DecodeOpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37285 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37289 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37294 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37298 "isa_tms320.tcc"
	>(0x24400000UL, 0xffe0e000UL, DecodeOpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37302 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37306 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37311 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37315 "isa_tms320.tcc"
	>(0x24200000UL, 0xffe000e0UL, DecodeOpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37319 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37323 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37328 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37332 "isa_tms320.tcc"
	>(0x24000000UL, 0xffe0e0e0UL, DecodeOpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37336 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37340 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37345 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37349 "isa_tms320.tcc"
	>(0x23e00000UL, 0xffe00000UL, DecodeOpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37353 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37357 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37362 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37366 "isa_tms320.tcc"
	>(0x23c00000UL, 0xffe0e000UL, DecodeOpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37370 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37374 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37379 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37383 "isa_tms320.tcc"
	>(0x23a00000UL, 0xffe000e0UL, DecodeOpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37387 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37391 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37396 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37400 "isa_tms320.tcc"
	>(0x23800000UL, 0xffe0e0e0UL, DecodeOpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37404 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37408 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37417 "isa_tms320.tcc"
	>(0x23600000UL, 0xffe00000UL, DecodeOpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37421 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37425 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37430 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37434 "isa_tms320.tcc"
	>(0x23400000UL, 0xffe0e000UL, DecodeOpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37438 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37442 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37447 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37451 "isa_tms320.tcc"
	>(0x23200000UL, 0xffe000e0UL, DecodeOpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37455 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37459 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37464 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37468 "isa_tms320.tcc"
	>(0x23000000UL, 0xffe0e0e0UL, DecodeOpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37472 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37476 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37485 "isa_tms320.tcc"
	>(0x22e00000UL, 0xffe00000UL, DecodeOpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37489 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37493 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37498 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37502 "isa_tms320.tcc"
	>(0x22c00000UL, 0xffe0e000UL, DecodeOpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37506 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37510 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37515 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37519 "isa_tms320.tcc"
	>(0x22a00000UL, 0xffe000e0UL, DecodeOpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37523 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37527 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37532 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37536 "isa_tms320.tcc"
	>(0x22800000UL, 0xffe0e0e0UL, DecodeOpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37540 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37544 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37549 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37553 "isa_tms320.tcc"
	>(0x22600000UL, 0xffe00000UL, DecodeOpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37557 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37561 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37566 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37570 "isa_tms320.tcc"
	>(0x22400000UL, 0xffe0e000UL, DecodeOpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37574 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37578 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37583 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37587 "isa_tms320.tcc"
	>(0x22200000UL, 0xffe000e0UL, DecodeOpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37591 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37595 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37600 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37604 "isa_tms320.tcc"
	>(0x22000000UL, 0xffe0e0e0UL, DecodeOpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37608 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37612 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37617 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37621 "isa_tms320.tcc"
	>(0x21e00000UL, 0xffe00000UL, DecodeOpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37625 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37629 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37634 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37638 "isa_tms320.tcc"
	>(0x21c00000UL, 0xffe0e000UL, DecodeOpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37642 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37646 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37651 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37655 "isa_tms320.tcc"
	>(0x21a00000UL, 0xffe000e0UL, DecodeOpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37659 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37663 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37668 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37672 "isa_tms320.tcc"
	>(0x21800000UL, 0xffe0e0e0UL, DecodeOpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37676 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37680 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37685 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37689 "isa_tms320.tcc"
	>(0x21600000UL, 0xffe00000UL, DecodeOpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37693 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37697 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37702 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37706 "isa_tms320.tcc"
	>(0x21400000UL, 0xffe0e000UL, DecodeOpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37710 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37714 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37719 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37723 "isa_tms320.tcc"
	>(0x21200000UL, 0xffe000e0UL, DecodeOpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37727 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37731 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37736 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37740 "isa_tms320.tcc"
	>(0x21000000UL, 0xffe0e0e0UL, DecodeOpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37744 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37748 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37753 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37757 "isa_tms320.tcc"
	>(0x20e00000UL, 0xffe00000UL, DecodeOpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37761 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37765 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37770 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37774 "isa_tms320.tcc"
	>(0x20c00000UL, 0xffe0e000UL, DecodeOpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37778 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37782 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37787 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37791 "isa_tms320.tcc"
	>(0x20a00000UL, 0xffe000e0UL, DecodeOpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37795 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37799 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37804 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37808 "isa_tms320.tcc"
	>(0x20800000UL, 0xffe0e0e0UL, DecodeOpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37812 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37816 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37821 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37825 "isa_tms320.tcc"
	>(0x20600000UL, 0xffe00000UL, DecodeOpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37829 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37833 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37838 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37842 "isa_tms320.tcc"
	>(0x20400000UL, 0xffe0e000UL, DecodeOpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37846 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37850 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37855 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37859 "isa_tms320.tcc"
	>(0x20200000UL, 0xffe000e0UL, DecodeOpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37863 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37867 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37872 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37876 "isa_tms320.tcc"
	>(0x20000000UL, 0xffe0e0e0UL, DecodeOpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37880 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37884 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37889 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37893 "isa_tms320.tcc"
	>(0x1ae00000UL, 0xffe00000UL, DecodeOpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37897 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37901 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37906 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37910 "isa_tms320.tcc"
	>(0x1ac00000UL, 0xffe00000UL, DecodeOpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37914 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37918 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37923 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37927 "isa_tms320.tcc"
	>(0x1aa00000UL, 0xffe00000UL, DecodeOpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37931 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37935 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37940 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37944 "isa_tms320.tcc"
	>(0x1a800000UL, 0xffe0ffe0UL, DecodeOpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37948 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37952 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37957 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37961 "isa_tms320.tcc"
	>(0x1a600000UL, 0xffe00000UL, DecodeOpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37965 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37969 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37974 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37978 "isa_tms320.tcc"
	>(0x1a400000UL, 0xffe00000UL, DecodeOpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37982 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37986 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37991 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37995 "isa_tms320.tcc"
	>(0x1a200000UL, 0xffe00000UL, DecodeOpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37999 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38003 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38008 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38012 "isa_tms320.tcc"
	>(0x1a000000UL, 0xffe0ffe0UL, DecodeOpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38016 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38020 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38025 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38029 "isa_tms320.tcc"
	>(0x19e00000UL, 0xffe00000UL, DecodeOpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38033 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38037 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38042 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38046 "isa_tms320.tcc"
	>(0x19c00000UL, 0xffe00000UL, DecodeOpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38050 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38054 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38059 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38063 "isa_tms320.tcc"
	>(0x19a00000UL, 0xffe00000UL, DecodeOpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38067 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38071 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38076 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38080 "isa_tms320.tcc"
	>(0x19800000UL, 0xffe0ffe0UL, DecodeOpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38084 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38088 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38093 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38097 "isa_tms320.tcc"
	>(0x19600000UL, 0xffe00000UL, DecodeOpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38101 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38105 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38110 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38114 "isa_tms320.tcc"
	>(0x19400000UL, 0xffe00000UL, DecodeOpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38118 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38122 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38127 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38131 "isa_tms320.tcc"
	>(0x19200000UL, 0xffe00000UL, DecodeOpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38135 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38139 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38144 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38148 "isa_tms320.tcc"
	>(0x19000000UL, 0xffe0ffe0UL, DecodeOpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38152 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38156 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38161 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38165 "isa_tms320.tcc"
	>(0x18e00000UL, 0xffe00000UL, DecodeOpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38169 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38173 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38178 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38182 "isa_tms320.tcc"
	>(0x18c00000UL, 0xffe00000UL, DecodeOpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38186 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38190 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38195 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38199 "isa_tms320.tcc"
	>(0x18a00000UL, 0xffe00000UL, DecodeOpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38203 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38207 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38212 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38216 "isa_tms320.tcc"
	>(0x18800000UL, 0xffe0ffe0UL, DecodeOpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38220 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38224 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38229 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38233 "isa_tms320.tcc"
	>(0x18600000UL, 0xffe00000UL, DecodeOpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38237 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38241 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38246 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38250 "isa_tms320.tcc"
	>(0x18400000UL, 0xffe00000UL, DecodeOpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38254 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38258 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38263 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38267 "isa_tms320.tcc"
	>(0x18200000UL, 0xffe00000UL, DecodeOpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38271 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38275 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38280 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38284 "isa_tms320.tcc"
	>(0x18000000UL, 0xffe0ffe0UL, DecodeOpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38288 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38292 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38297 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38301 "isa_tms320.tcc"
	>(0x17e00000UL, 0xffe00000UL, DecodeOpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38305 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38309 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38314 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38318 "isa_tms320.tcc"
	>(0x17c00000UL, 0xffe00000UL, DecodeOpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38322 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38326 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38331 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38335 "isa_tms320.tcc"
	>(0x17a00000UL, 0xffe00000UL, DecodeOpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38339 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38343 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38348 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38352 "isa_tms320.tcc"
	>(0x17800000UL, 0xffe0ffe0UL, DecodeOpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38356 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38360 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38365 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38369 "isa_tms320.tcc"
	>(0x17600000UL, 0xffe00000UL, DecodeOpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38373 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38377 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38382 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38386 "isa_tms320.tcc"
	>(0x17400000UL, 0xffe00000UL, DecodeOpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38390 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38394 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38399 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38403 "isa_tms320.tcc"
	>(0x17200000UL, 0xffe00000UL, DecodeOpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38407 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38411 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38416 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38420 "isa_tms320.tcc"
	>(0x17000000UL, 0xffe0ffe0UL, DecodeOpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38424 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38428 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38433 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38437 "isa_tms320.tcc"
	>(0x16e00000UL, 0xffe00000UL, DecodeOpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38441 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38445 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38450 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38454 "isa_tms320.tcc"
	>(0x16c00000UL, 0xffe00000UL, DecodeOpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38458 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38462 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38467 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38471 "isa_tms320.tcc"
	>(0x16a00000UL, 0xffe00000UL, DecodeOpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38475 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38479 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38484 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38488 "isa_tms320.tcc"
	>(0x16800000UL, 0xffe0ffe0UL, DecodeOpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38492 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38496 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38501 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38505 "isa_tms320.tcc"
	>(0x1360ffffUL, 0xffe0ffffUL, DecodeOpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38509 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38513 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38518 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38522 "isa_tms320.tcc"
	>(0x12e0ffffUL, 0xffe0ffffUL, DecodeOpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38526 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38530 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38535 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38539 "isa_tms320.tcc"
	>(0x12600001UL, 0xffe0ffffUL, DecodeOpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38543 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38547 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38552 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38556 "isa_tms320.tcc"
	>(0x11e00001UL, 0xffe0ffffUL, DecodeOpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38560 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38564 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38569 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38573 "isa_tms320.tcc"
	>(0x11600000UL, 0xffe00000UL, DecodeOpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38577 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38581 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38586 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38590 "isa_tms320.tcc"
	>(0x11400000UL, 0xffe00000UL, DecodeOpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38594 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38598 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38603 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38607 "isa_tms320.tcc"
	>(0x11200000UL, 0xffe00000UL, DecodeOpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38611 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38615 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38620 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38624 "isa_tms320.tcc"
	>(0x11000000UL, 0xffe0ffe0UL, DecodeOpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38628 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38632 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38637 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38641 "isa_tms320.tcc"
	>(0x10600000UL, 0xffe00000UL, DecodeOpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38645 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38649 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38654 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38658 "isa_tms320.tcc"
	>(0x10400000UL, 0xffe00000UL, DecodeOpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38662 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38666 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38671 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38675 "isa_tms320.tcc"
	>(0x10200000UL, 0xffe00000UL, DecodeOpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38679 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38683 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38688 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38692 "isa_tms320.tcc"
	>(0x10000000UL, 0xffe0ffe0UL, DecodeOpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38696 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38700 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38705 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38709 "isa_tms320.tcc"
	>(0xde00000UL, 0xffe00000UL, DecodeOpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38713 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38717 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38722 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38726 "isa_tms320.tcc"
	>(0xdc00000UL, 0xffe00000UL, DecodeOpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38730 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38734 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38739 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38743 "isa_tms320.tcc"
	>(0xda00000UL, 0xffe00000UL, DecodeOpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38747 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38751 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38756 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38760 "isa_tms320.tcc"
	>(0xd800000UL, 0xffe0ffe0UL, DecodeOpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38764 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38768 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38773 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38777 "isa_tms320.tcc"
	>(0xd600000UL, 0xffe00000UL, DecodeOpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38781 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38785 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38790 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38794 "isa_tms320.tcc"
	>(0xd400000UL, 0xffe00000UL, DecodeOpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38798 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38802 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38807 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38811 "isa_tms320.tcc"
	>(0xd200000UL, 0xffe00000UL, DecodeOpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38815 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38819 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38824 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38828 "isa_tms320.tcc"
	>(0xd000000UL, 0xffe0ffe0UL, DecodeOpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38832 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38836 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38841 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38845 "isa_tms320.tcc"
	>(0xc600000UL, 0xffe00000UL, DecodeOpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38849 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38853 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38858 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38862 "isa_tms320.tcc"
	>(0xc400000UL, 0xffe00000UL, DecodeOpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38866 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38870 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38875 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38879 "isa_tms320.tcc"
	>(0xc200000UL, 0xffe00000UL, DecodeOpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38883 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38887 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38892 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38896 "isa_tms320.tcc"
	>(0xc000000UL, 0xffe0ffe0UL, DecodeOpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38900 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38904 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38909 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38913 "isa_tms320.tcc"
	>(0xbe00000UL, 0xffe00000UL, DecodeOpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38917 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38921 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38926 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38930 "isa_tms320.tcc"
	>(0xbc00000UL, 0xffe00000UL, DecodeOpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38934 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38938 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38943 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38947 "isa_tms320.tcc"
	>(0xba00000UL, 0xffe00000UL, DecodeOpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38951 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38955 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38960 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38964 "isa_tms320.tcc"
	>(0xb800000UL, 0xffe0ffe0UL, DecodeOpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38968 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38972 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38977 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38981 "isa_tms320.tcc"
	>(0xb600000UL, 0xffe00000UL, DecodeOpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38985 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38989 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38994 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38998 "isa_tms320.tcc"
	>(0xb400000UL, 0xffe00000UL, DecodeOpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39002 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39006 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39011 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39015 "isa_tms320.tcc"
	>(0xb200000UL, 0xffe00000UL, DecodeOpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39019 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39023 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39028 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39032 "isa_tms320.tcc"
	>(0xb000000UL, 0xffe0ffe0UL, DecodeOpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39036 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39040 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39045 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39049 "isa_tms320.tcc"
	>(0xae00000UL, 0xffe00000UL, DecodeOpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39053 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39057 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39062 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39066 "isa_tms320.tcc"
	>(0xac00000UL, 0xffe00000UL, DecodeOpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39070 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39074 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39079 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39083 "isa_tms320.tcc"
	>(0xaa00000UL, 0xffe00000UL, DecodeOpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39087 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39091 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39096 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39100 "isa_tms320.tcc"
	>(0xa800000UL, 0xffe0ffe0UL, DecodeOpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39104 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39108 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39113 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39117 "isa_tms320.tcc"
	>(0xa600000UL, 0xffe00000UL, DecodeOpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39121 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39125 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39130 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39134 "isa_tms320.tcc"
	>(0xa400000UL, 0xffe00000UL, DecodeOpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39138 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39142 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39147 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39151 "isa_tms320.tcc"
	>(0xa200000UL, 0xffe00000UL, DecodeOpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39155 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39159 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39164 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39168 "isa_tms320.tcc"
	>(0xa000000UL, 0xffe0ffe0UL, DecodeOpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39172 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39176 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39181 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39185 "isa_tms320.tcc"
	>(0x9e00000UL, 0xffe00000UL, DecodeOpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39189 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39193 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39198 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39202 "isa_tms320.tcc"
	>(0x9c00000UL, 0xffe00000UL, DecodeOpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39206 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39210 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39215 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39219 "isa_tms320.tcc"
	>(0x9a00000UL, 0xffe00000UL, DecodeOpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39223 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39227 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39232 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39236 "isa_tms320.tcc"
	>(0x9800000UL, 0xffe0ffe0UL, DecodeOpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39240 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39244 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39249 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39253 "isa_tms320.tcc"
	>(0x5e00000UL, 0xffe00000UL, DecodeOpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39257 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39261 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39266 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39270 "isa_tms320.tcc"
	>(0x5c00000UL, 0xffe00000UL, DecodeOpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39274 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39278 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39283 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39287 "isa_tms320.tcc"
	>(0x5a00000UL, 0xffe00000UL, DecodeOpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39291 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39295 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39300 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39304 "isa_tms320.tcc"
	>(0x5800000UL, 0xffe0ffe0UL, DecodeOpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39308 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39312 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39317 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39321 "isa_tms320.tcc"
	>(0x5600000UL, 0xffe00000UL, DecodeOpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39325 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39329 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39334 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39338 "isa_tms320.tcc"
	>(0x5400000UL, 0xffe00000UL, DecodeOpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39342 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39346 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39351 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39355 "isa_tms320.tcc"
	>(0x5200000UL, 0xffe00000UL, DecodeOpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39359 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39363 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39368 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39372 "isa_tms320.tcc"
	>(0x5000000UL, 0xffe0ffe0UL, DecodeOpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39376 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39380 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39385 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39389 "isa_tms320.tcc"
	>(0x4e00000UL, 0xffe00000UL, DecodeOpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39393 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39397 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39402 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39406 "isa_tms320.tcc"
	>(0x4c00000UL, 0xffe00000UL, DecodeOpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39410 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39414 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39419 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39423 "isa_tms320.tcc"
	>(0x4a00000UL, 0xffe00000UL, DecodeOpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39427 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39431 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39436 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39440 "isa_tms320.tcc"
	>(0x4800000UL, 0xffe0ffe0UL, DecodeOpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39444 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39448 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39453 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39457 "isa_tms320.tcc"
	>(0x4600000UL, 0xffe00000UL, DecodeOpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39461 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39465 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39470 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39474 "isa_tms320.tcc"
	>(0x4400000UL, 0xffe00000UL, DecodeOpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39478 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39482 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39487 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39491 "isa_tms320.tcc"
	>(0x4200000UL, 0xffe00000UL, DecodeOpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39495 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39499 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39504 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39508 "isa_tms320.tcc"
	>(0x4000000UL, 0xffe0ffe0UL, DecodeOpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39512 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39516 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39521 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39525 "isa_tms320.tcc"
	>(0x3e00000UL, 0xffe00000UL, DecodeOpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39529 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39533 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39538 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39542 "isa_tms320.tcc"
	>(0x3c00000UL, 0xffe00000UL, DecodeOpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39546 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39550 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39555 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39559 "isa_tms320.tcc"
	>(0x3a00000UL, 0xffe00000UL, DecodeOpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39563 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39567 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39572 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39576 "isa_tms320.tcc"
	>(0x3800000UL, 0xffe0ffe0UL, DecodeOpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39580 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39584 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39589 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39593 "isa_tms320.tcc"
	>(0x3600000UL, 0xffe00000UL, DecodeOpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39597 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39601 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39606 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39610 "isa_tms320.tcc"
	>(0x3400000UL, 0xffe00000UL, DecodeOpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39614 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39618 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39623 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39627 "isa_tms320.tcc"
	>(0x3200000UL, 0xffe00000UL, DecodeOpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39631 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39635 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39640 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39644 "isa_tms320.tcc"
	>(0x3000000UL, 0xffe0ffe0UL, DecodeOpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39648 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39652 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39657 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39661 "isa_tms320.tcc"
	>(0x2e00000UL, 0xffe00000UL, DecodeOpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39665 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39669 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39674 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39678 "isa_tms320.tcc"
	>(0x2c00000UL, 0xffe00000UL, DecodeOpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39682 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39686 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39691 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39695 "isa_tms320.tcc"
	>(0x2a00000UL, 0xffe00000UL, DecodeOpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39699 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39703 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39708 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39712 "isa_tms320.tcc"
	>(0x2800000UL, 0xffe0ffe0UL, DecodeOpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39716 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39720 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39725 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39729 "isa_tms320.tcc"
	>(0x2600000UL, 0xffe00000UL, DecodeOpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39733 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39737 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39742 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39746 "isa_tms320.tcc"
	>(0x2400000UL, 0xffe00000UL, DecodeOpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39750 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39754 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39759 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39763 "isa_tms320.tcc"
	>(0x2200000UL, 0xffe00000UL, DecodeOpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39767 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39771 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39776 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39780 "isa_tms320.tcc"
	>(0x2000000UL, 0xffe0ffe0UL, DecodeOpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39784 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39788 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39793 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39797 "isa_tms320.tcc"
	>(0x1e00000UL, 0xffe00000UL, DecodeOpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39801 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39805 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39810 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39814 "isa_tms320.tcc"
	>(0x1c00000UL, 0xffe00000UL, DecodeOpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39818 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39822 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39827 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39831 "isa_tms320.tcc"
	>(0x1a00000UL, 0xffe00000UL, DecodeOpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39835 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39839 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39844 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39848 "isa_tms320.tcc"
	>(0x1800000UL, 0xffe0ffe0UL, DecodeOpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39852 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39856 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39861 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39865 "isa_tms320.tcc"
	>(0x1600000UL, 0xffe00000UL, DecodeOpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39869 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39873 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39878 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39882 "isa_tms320.tcc"
	>(0x1400000UL, 0xffe00000UL, DecodeOpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39886 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39890 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39895 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39899 "isa_tms320.tcc"
	>(0x1200000UL, 0xffe00000UL, DecodeOpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39903 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39907 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39912 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39916 "isa_tms320.tcc"
	>(0x1000000UL, 0xffe0ffe0UL, DecodeOpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39920 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39924 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39929 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39933 "isa_tms320.tcc"
	>(0xe00000UL, 0xffe00000UL, DecodeOpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39937 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39941 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39946 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39950 "isa_tms320.tcc"
	>(0xc00000UL, 0xffe00000UL, DecodeOpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39954 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39958 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39963 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39967 "isa_tms320.tcc"
	>(0xa00000UL, 0xffe00000UL, DecodeOpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39971 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39975 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39980 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39984 "isa_tms320.tcc"
	>(0x800000UL, 0xffe0ffe0UL, DecodeOpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39988 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39992 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39997 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40001 "isa_tms320.tcc"
	>(0x600000UL, 0xffe00000UL, DecodeOpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40005 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40009 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40014 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40018 "isa_tms320.tcc"
	>(0x400000UL, 0xffe00000UL, DecodeOpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40022 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40026 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40031 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40035 "isa_tms320.tcc"
	>(0x200000UL, 0xffe00000UL, DecodeOpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40039 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40043 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40048 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40052 "isa_tms320.tcc"
	>(0x0UL, 0xffe0ffe0UL, DecodeOpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40056 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40060 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40065 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40069 "isa_tms320.tcc"
	>(0x15400000UL, 0xffe00000UL, DecodeOpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40073 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40077 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40082 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40086 "isa_tms320.tcc"
	>(0x15200000UL, 0xffe00000UL, DecodeOpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40090 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40094 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40099 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40103 "isa_tms320.tcc"
	>(0x14400000UL, 0xffe00000UL, DecodeOpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40107 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40111 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40116 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40120 "isa_tms320.tcc"
	>(0x14200000UL, 0xffe00000UL, DecodeOpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40124 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40128 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40133 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40137 "isa_tms320.tcc"
	>(0xfa00000UL, 0xffe0ffffUL, DecodeOpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40141 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40145 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40150 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40154 "isa_tms320.tcc"
	>(0xf200000UL, 0xffe0ffffUL, DecodeOpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40158 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40162 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40167 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40171 "isa_tms320.tcc"
	>(0xea00000UL, 0xffe0ffffUL, DecodeOpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40175 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40179 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40184 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40188 "isa_tms320.tcc"
	>(0xe200000UL, 0xffe0ffffUL, DecodeOpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40192 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40196 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40201 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40205 "isa_tms320.tcc"
	>(0x8700000UL, 0xffffff00UL, DecodeOpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40209 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40213 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40218 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40222 "isa_tms320.tcc"
	>(0x9600000UL, 0xffe0f000UL, DecodeOpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40226 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40230 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40235 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40239 "isa_tms320.tcc"
	>(0x9400000UL, 0xffe00000UL, DecodeOpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40243 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40247 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40256 "isa_tms320.tcc"
	>(0x9200000UL, 0xffe00000UL, DecodeOpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40260 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40264 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40269 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40273 "isa_tms320.tcc"
	>(0x9000000UL, 0xffe0ffe0UL, DecodeOpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40277 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40281 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40290 "isa_tms320.tcc"
	>(0x50600000UL, 0xf0600000UL, DecodeOpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40294 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40298 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40303 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40307 "isa_tms320.tcc"
	>(0x50400000UL, 0xf0600000UL, DecodeOpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40311 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40315 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40320 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40324 "isa_tms320.tcc"
	>(0x50200000UL, 0xf0600000UL, DecodeOpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40328 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40332 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40337 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40341 "isa_tms320.tcc"
	>(0x50000000UL, 0xf060ffe0UL, DecodeOpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40345 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40349 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40354 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40358 "isa_tms320.tcc"
	>(0x8600000UL, 0xffe00000UL, DecodeOpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40362 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40366 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40371 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40375 "isa_tms320.tcc"
	>(0x8400000UL, 0xffe00000UL, DecodeOpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40379 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40383 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40388 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40392 "isa_tms320.tcc"
	>(0x8200000UL, 0xffe00000UL, DecodeOpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40396 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40400 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40405 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40409 "isa_tms320.tcc"
	>(0x8000000UL, 0xffe0ffe0UL, DecodeOpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40417 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40422 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40426 "isa_tms320.tcc"
	>(0x40600000UL, 0xf0600000UL, DecodeOpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40430 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40434 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40439 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40443 "isa_tms320.tcc"
	>(0x40400000UL, 0xf0600000UL, DecodeOpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40447 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40451 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40456 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40460 "isa_tms320.tcc"
	>(0x40200000UL, 0xf0600000UL, DecodeOpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40464 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40468 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40473 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40477 "isa_tms320.tcc"
	>(0x40000000UL, 0xf060ffe0UL, DecodeOpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40485 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40494 "isa_tms320.tcc"
	>(0x7600000UL, 0xffe00000UL, DecodeOpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40498 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40502 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40507 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40511 "isa_tms320.tcc"
	>(0x7400000UL, 0xffe00000UL, DecodeOpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40515 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40519 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40528 "isa_tms320.tcc"
	>(0x7200000UL, 0xffe00000UL, DecodeOpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40532 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40536 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40541 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40545 "isa_tms320.tcc"
	>(0x7000000UL, 0xffe0ffe0UL, DecodeOpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40549 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40553 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40562 "isa_tms320.tcc"
	>(0x6e00000UL, 0xffe00000UL, DecodeOpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40566 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40570 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40575 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40579 "isa_tms320.tcc"
	>(0x6c00000UL, 0xffe00000UL, DecodeOpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40583 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40587 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40592 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40596 "isa_tms320.tcc"
	>(0x6a00000UL, 0xffe00000UL, DecodeOpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40600 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40604 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40609 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40613 "isa_tms320.tcc"
	>(0x6800000UL, 0xffe0ffe0UL, DecodeOpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40617 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40621 "isa_tms320.tcc"
	>));
}

template <
#line 45 "isa/tms320.isa"
class
#line 40628 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40631 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40635 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40638 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40643 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40647 "isa_tms320.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 45 "isa/tms320.isa"
class
#line 40656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40666 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40675 "isa_tms320.tcc"
>::Fetch(void *, typename CONFIG::address_t, uint32_t)
{
	assert( ! "Calling unimplemented virtual method Fetch" );
}

template <
#line 45 "isa/tms320.isa"
class
#line 40684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40694 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40703 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40711 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40717 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40721 "isa_tms320.tcc"
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
#line 40746 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40750 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40758 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40765 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40768 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40777 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40785 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40791 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40795 "isa_tms320.tcc"
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
#line 40810 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40814 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40832 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40841 "isa_tms320.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 40851 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 40855 "isa_tms320.tcc"
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
#line 40874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40881 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40884 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40893 "isa_tms320.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40902 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40906 "isa_tms320.tcc"
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
#line 40939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40949 "isa_tms320.tcc"
>
inline DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 40954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40958 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40966 "isa_tms320.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40974 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40978 "isa_tms320.tcc"
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
#line 41012 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41019 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41022 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 41027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41031 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41039 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41045 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41049 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41059 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 41067 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 41071 "isa_tms320.tcc"
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
#line 41091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41101 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 41106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41110 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41114 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41118 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41124 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41128 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41134 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41138 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 41146 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 41150 "isa_tms320.tcc"
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
#line 41172 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41175 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41179 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41182 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41191 "isa_tms320.tcc"
>::SetLittleEndian()
{
	little_endian = true;
}

template <
#line 45 "isa/tms320.isa"
class
#line 41200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41207 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41210 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41219 "isa_tms320.tcc"
>::SetBigEndian()
{
	little_endian = false;
}

} } } } } } }
