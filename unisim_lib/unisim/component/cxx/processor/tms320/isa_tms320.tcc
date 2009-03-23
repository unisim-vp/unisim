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
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 255 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 262 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 265 "isa_tms320.tcc"
>
class OpLDE_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 270 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 274 "isa_tms320.tcc"
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
#line 287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 294 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 297 "isa_tms320.tcc"
>
class OpLDE_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 306 "isa_tms320.tcc"
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
#line 321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 331 "isa_tms320.tcc"
>
class OpLDE_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 340 "isa_tms320.tcc"
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
#line 353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 363 "isa_tms320.tcc"
>
class OpLDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 372 "isa_tms320.tcc"
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
#line 385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 392 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 395 "isa_tms320.tcc"
>
class OpLDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 404 "isa_tms320.tcc"
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
#line 417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 424 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 427 "isa_tms320.tcc"
>
class OpLDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 436 "isa_tms320.tcc"
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
#line 451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 461 "isa_tms320.tcc"
>
class OpLDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 470 "isa_tms320.tcc"
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
#line 483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 490 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 493 "isa_tms320.tcc"
>
class OpLDFcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 502 "isa_tms320.tcc"
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
#line 516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 526 "isa_tms320.tcc"
>
class OpLDFcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 535 "isa_tms320.tcc"
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
#line 549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 559 "isa_tms320.tcc"
>
class OpLDFcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 568 "isa_tms320.tcc"
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
#line 584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 591 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 594 "isa_tms320.tcc"
>
class OpLDFcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 603 "isa_tms320.tcc"
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
#line 617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 627 "isa_tms320.tcc"
>
class OpLDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 636 "isa_tms320.tcc"
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
class OpLDI_dir : public Operation<
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
	OpLDI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 691 "isa_tms320.tcc"
>
class OpLDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 700 "isa_tms320.tcc"
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
#line 715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 725 "isa_tms320.tcc"
>
class OpLDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 734 "isa_tms320.tcc"
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
#line 747 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 754 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 757 "isa_tms320.tcc"
>
class OpLDIcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 766 "isa_tms320.tcc"
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
#line 777 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 781 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 784 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 788 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 791 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 799 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 809 "isa_tms320.tcc"
>
class OpLDIcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 818 "isa_tms320.tcc"
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
#line 832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 842 "isa_tms320.tcc"
>
class OpLDIcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 847 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 851 "isa_tms320.tcc"
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
#line 864 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 868 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 871 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 875 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 878 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 896 "isa_tms320.tcc"
>
class OpLDIcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 905 "isa_tms320.tcc"
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
#line 919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 929 "isa_tms320.tcc"
>
class OpLDM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 938 "isa_tms320.tcc"
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
#line 951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 961 "isa_tms320.tcc"
>
class OpLDM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 970 "isa_tms320.tcc"
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
#line 983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 993 "isa_tms320.tcc"
>
class OpLDM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1002 "isa_tms320.tcc"
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
#line 1017 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1027 "isa_tms320.tcc"
>
class OpLDM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1036 "isa_tms320.tcc"
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
#line 1049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1059 "isa_tms320.tcc"
>
class OpLDP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1068 "isa_tms320.tcc"
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
#line 1080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1087 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1090 "isa_tms320.tcc"
>
class OpPOP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1095 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1099 "isa_tms320.tcc"
>
{
public:
	OpPOP(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1108 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1112 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1115 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1119 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1122 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1140 "isa_tms320.tcc"
>
class OpPOPF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1149 "isa_tms320.tcc"
>
{
public:
	OpPOPF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1158 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1162 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1165 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1169 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1172 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1180 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1183 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1190 "isa_tms320.tcc"
>
class OpPUSH : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1199 "isa_tms320.tcc"
>
{
public:
	OpPUSH(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1208 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1212 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1215 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1219 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1222 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1237 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1240 "isa_tms320.tcc"
>
class OpPUSHF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1245 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1249 "isa_tms320.tcc"
>
{
public:
	OpPUSHF(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1258 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1262 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1265 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1269 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1272 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1290 "isa_tms320.tcc"
>
class OpSTF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1299 "isa_tms320.tcc"
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
#line 1312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1322 "isa_tms320.tcc"
>
class OpSTF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1331 "isa_tms320.tcc"
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
#line 1346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1356 "isa_tms320.tcc"
>
class OpSTI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1365 "isa_tms320.tcc"
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
#line 1378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1388 "isa_tms320.tcc"
>
class OpSTI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1397 "isa_tms320.tcc"
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
#line 1412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1422 "isa_tms320.tcc"
>
class OpABSF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1431 "isa_tms320.tcc"
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
#line 1444 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1447 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1454 "isa_tms320.tcc"
>
class OpABSF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1463 "isa_tms320.tcc"
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
#line 1476 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1486 "isa_tms320.tcc"
>
class OpABSF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1495 "isa_tms320.tcc"
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
#line 1510 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1513 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1517 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1520 "isa_tms320.tcc"
>
class OpABSF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1525 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1529 "isa_tms320.tcc"
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
#line 1542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1552 "isa_tms320.tcc"
>
class OpABSI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1557 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1561 "isa_tms320.tcc"
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
#line 1574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1584 "isa_tms320.tcc"
>
class OpABSI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1593 "isa_tms320.tcc"
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
#line 1606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1616 "isa_tms320.tcc"
>
class OpABSI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1625 "isa_tms320.tcc"
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
#line 1640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1643 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1650 "isa_tms320.tcc"
>
class OpABSI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1659 "isa_tms320.tcc"
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
#line 1672 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1679 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1682 "isa_tms320.tcc"
>
class OpADDC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1691 "isa_tms320.tcc"
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
#line 1704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1711 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1714 "isa_tms320.tcc"
>
class OpADDC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1723 "isa_tms320.tcc"
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
#line 1736 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1746 "isa_tms320.tcc"
>
class OpADDC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1755 "isa_tms320.tcc"
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
#line 1770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1777 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1780 "isa_tms320.tcc"
>
class OpADDC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1789 "isa_tms320.tcc"
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
#line 1802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1812 "isa_tms320.tcc"
>
class OpADDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1821 "isa_tms320.tcc"
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
#line 1834 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1844 "isa_tms320.tcc"
>
class OpADDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1849 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1853 "isa_tms320.tcc"
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
#line 1866 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1876 "isa_tms320.tcc"
>
class OpADDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1885 "isa_tms320.tcc"
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
#line 1900 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1907 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1910 "isa_tms320.tcc"
>
class OpADDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1915 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1919 "isa_tms320.tcc"
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
#line 1932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1942 "isa_tms320.tcc"
>
class OpADDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1947 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1951 "isa_tms320.tcc"
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
#line 1964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1974 "isa_tms320.tcc"
>
class OpADDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1983 "isa_tms320.tcc"
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
#line 1996 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2006 "isa_tms320.tcc"
>
class OpADDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2015 "isa_tms320.tcc"
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
#line 2030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2040 "isa_tms320.tcc"
>
class OpADDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2049 "isa_tms320.tcc"
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
#line 2062 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2065 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2069 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2072 "isa_tms320.tcc"
>
class OpAND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2081 "isa_tms320.tcc"
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
#line 2094 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2104 "isa_tms320.tcc"
>
class OpAND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2113 "isa_tms320.tcc"
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
#line 2126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2136 "isa_tms320.tcc"
>
class OpAND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2145 "isa_tms320.tcc"
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
#line 2160 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2167 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2170 "isa_tms320.tcc"
>
class OpAND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2175 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2179 "isa_tms320.tcc"
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
#line 2192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2202 "isa_tms320.tcc"
>
class OpANDN_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2211 "isa_tms320.tcc"
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
#line 2224 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2234 "isa_tms320.tcc"
>
class OpANDN_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2243 "isa_tms320.tcc"
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
#line 2256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2263 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2266 "isa_tms320.tcc"
>
class OpANDN_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2275 "isa_tms320.tcc"
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
#line 2290 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2297 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2300 "isa_tms320.tcc"
>
class OpANDN_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2309 "isa_tms320.tcc"
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
#line 2322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2329 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2332 "isa_tms320.tcc"
>
class OpASH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2341 "isa_tms320.tcc"
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
#line 2354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2364 "isa_tms320.tcc"
>
class OpASH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2373 "isa_tms320.tcc"
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
#line 2386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2396 "isa_tms320.tcc"
>
class OpASH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2405 "isa_tms320.tcc"
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
#line 2420 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2430 "isa_tms320.tcc"
>
class OpASH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2439 "isa_tms320.tcc"
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
#line 2452 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2462 "isa_tms320.tcc"
>
class OpCMPF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2471 "isa_tms320.tcc"
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
#line 2484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2494 "isa_tms320.tcc"
>
class OpCMPF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2503 "isa_tms320.tcc"
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
#line 2516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2526 "isa_tms320.tcc"
>
class OpCMPF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2535 "isa_tms320.tcc"
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
#line 2550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2557 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2560 "isa_tms320.tcc"
>
class OpCMPF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2569 "isa_tms320.tcc"
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
#line 2582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2589 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2592 "isa_tms320.tcc"
>
class OpCMPI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2601 "isa_tms320.tcc"
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
#line 2614 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2624 "isa_tms320.tcc"
>
class OpCMPI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2633 "isa_tms320.tcc"
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
#line 2646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2649 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2653 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2656 "isa_tms320.tcc"
>
class OpCMPI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2665 "isa_tms320.tcc"
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
#line 2680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2690 "isa_tms320.tcc"
>
class OpCMPI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2695 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2699 "isa_tms320.tcc"
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
#line 2712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2719 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2722 "isa_tms320.tcc"
>
class OpFIX_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2731 "isa_tms320.tcc"
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
#line 2744 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2754 "isa_tms320.tcc"
>
class OpFIX_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2763 "isa_tms320.tcc"
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
#line 2776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2783 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2786 "isa_tms320.tcc"
>
class OpFIX_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2795 "isa_tms320.tcc"
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
#line 2810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2820 "isa_tms320.tcc"
>
class OpFIX_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2829 "isa_tms320.tcc"
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
#line 2842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2852 "isa_tms320.tcc"
>
class OpFLOAT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2861 "isa_tms320.tcc"
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
#line 2874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2881 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2884 "isa_tms320.tcc"
>
class OpFLOAT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2893 "isa_tms320.tcc"
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
#line 2906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2916 "isa_tms320.tcc"
>
class OpFLOAT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2925 "isa_tms320.tcc"
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
#line 2940 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2947 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2950 "isa_tms320.tcc"
>
class OpFLOAT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2959 "isa_tms320.tcc"
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
#line 2972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2982 "isa_tms320.tcc"
>
class OpLSH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2991 "isa_tms320.tcc"
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
#line 3004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3014 "isa_tms320.tcc"
>
class OpLSH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3023 "isa_tms320.tcc"
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
#line 3036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3046 "isa_tms320.tcc"
>
class OpLSH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3055 "isa_tms320.tcc"
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
#line 3070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3080 "isa_tms320.tcc"
>
class OpLSH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3089 "isa_tms320.tcc"
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
#line 3102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3105 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3112 "isa_tms320.tcc"
>
class OpMPYF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3121 "isa_tms320.tcc"
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
#line 3134 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3144 "isa_tms320.tcc"
>
class OpMPYF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3149 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3153 "isa_tms320.tcc"
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
#line 3166 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3173 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3176 "isa_tms320.tcc"
>
class OpMPYF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3185 "isa_tms320.tcc"
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
#line 3200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3207 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3210 "isa_tms320.tcc"
>
class OpMPYF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3219 "isa_tms320.tcc"
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
#line 3232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3242 "isa_tms320.tcc"
>
class OpMPYI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3251 "isa_tms320.tcc"
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
#line 3264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3274 "isa_tms320.tcc"
>
class OpMPYI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3283 "isa_tms320.tcc"
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
#line 3296 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3306 "isa_tms320.tcc"
>
class OpMPYI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3315 "isa_tms320.tcc"
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
#line 3330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3337 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3340 "isa_tms320.tcc"
>
class OpMPYI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3345 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3349 "isa_tms320.tcc"
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
#line 3362 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3365 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3369 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3372 "isa_tms320.tcc"
>
class OpNEGB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3381 "isa_tms320.tcc"
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
#line 3394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3404 "isa_tms320.tcc"
>
class OpNEGB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3413 "isa_tms320.tcc"
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
#line 3426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3436 "isa_tms320.tcc"
>
class OpNEGB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3445 "isa_tms320.tcc"
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
#line 3460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3470 "isa_tms320.tcc"
>
class OpNEGB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3475 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3479 "isa_tms320.tcc"
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
#line 3492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3495 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3502 "isa_tms320.tcc"
>
class OpNEGF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3511 "isa_tms320.tcc"
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
#line 3524 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3534 "isa_tms320.tcc"
>
class OpNEGF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3543 "isa_tms320.tcc"
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
#line 3556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3563 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3566 "isa_tms320.tcc"
>
class OpNEGF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3575 "isa_tms320.tcc"
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
#line 3590 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3597 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3600 "isa_tms320.tcc"
>
class OpNEGF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3609 "isa_tms320.tcc"
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
#line 3622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3632 "isa_tms320.tcc"
>
class OpNEGI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3641 "isa_tms320.tcc"
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
#line 3654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3664 "isa_tms320.tcc"
>
class OpNEGI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3673 "isa_tms320.tcc"
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
#line 3686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3696 "isa_tms320.tcc"
>
class OpNEGI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3705 "isa_tms320.tcc"
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
#line 3720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3730 "isa_tms320.tcc"
>
class OpNEGI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3739 "isa_tms320.tcc"
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
#line 3752 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3755 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3759 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3762 "isa_tms320.tcc"
>
class OpNORM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3771 "isa_tms320.tcc"
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
#line 3784 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3791 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3794 "isa_tms320.tcc"
>
class OpNORM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3803 "isa_tms320.tcc"
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
#line 3816 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3823 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3826 "isa_tms320.tcc"
>
class OpNORM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3835 "isa_tms320.tcc"
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
#line 3850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3860 "isa_tms320.tcc"
>
class OpNORM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3869 "isa_tms320.tcc"
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
#line 3882 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3892 "isa_tms320.tcc"
>
class OpNOT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3897 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3901 "isa_tms320.tcc"
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
#line 3914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3924 "isa_tms320.tcc"
>
class OpNOT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3933 "isa_tms320.tcc"
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
#line 3946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3956 "isa_tms320.tcc"
>
class OpNOT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3965 "isa_tms320.tcc"
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
#line 3980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3990 "isa_tms320.tcc"
>
class OpNOT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3999 "isa_tms320.tcc"
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
#line 4012 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4019 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4022 "isa_tms320.tcc"
>
class OpOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4031 "isa_tms320.tcc"
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
#line 4044 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4054 "isa_tms320.tcc"
>
class OpOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4063 "isa_tms320.tcc"
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
#line 4076 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4083 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4086 "isa_tms320.tcc"
>
class OpOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4095 "isa_tms320.tcc"
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
#line 4110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4120 "isa_tms320.tcc"
>
class OpOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4129 "isa_tms320.tcc"
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
#line 4142 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4152 "isa_tms320.tcc"
>
class OpRND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4161 "isa_tms320.tcc"
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
#line 4174 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4184 "isa_tms320.tcc"
>
class OpRND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4193 "isa_tms320.tcc"
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
#line 4206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4216 "isa_tms320.tcc"
>
class OpRND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4225 "isa_tms320.tcc"
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
#line 4240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4250 "isa_tms320.tcc"
>
class OpRND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4259 "isa_tms320.tcc"
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
#line 4272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4282 "isa_tms320.tcc"
>
class OpROL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4291 "isa_tms320.tcc"
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
#line 4303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4313 "isa_tms320.tcc"
>
class OpROLC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4322 "isa_tms320.tcc"
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
#line 4334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4344 "isa_tms320.tcc"
>
class OpROR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4353 "isa_tms320.tcc"
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
#line 4365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4372 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4375 "isa_tms320.tcc"
>
class OpRORC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4384 "isa_tms320.tcc"
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
#line 4396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4406 "isa_tms320.tcc"
>
class OpSUBB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4415 "isa_tms320.tcc"
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
#line 4428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4438 "isa_tms320.tcc"
>
class OpSUBB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4447 "isa_tms320.tcc"
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
#line 4460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4470 "isa_tms320.tcc"
>
class OpSUBB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4475 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4479 "isa_tms320.tcc"
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
#line 4494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4504 "isa_tms320.tcc"
>
class OpSUBB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4513 "isa_tms320.tcc"
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
#line 4526 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4536 "isa_tms320.tcc"
>
class OpSUBC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4545 "isa_tms320.tcc"
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
#line 4558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4568 "isa_tms320.tcc"
>
class OpSUBC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4577 "isa_tms320.tcc"
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
#line 4590 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4597 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4600 "isa_tms320.tcc"
>
class OpSUBC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4609 "isa_tms320.tcc"
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
#line 4624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4634 "isa_tms320.tcc"
>
class OpSUBC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4643 "isa_tms320.tcc"
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
#line 4656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4666 "isa_tms320.tcc"
>
class OpSUBF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4675 "isa_tms320.tcc"
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
#line 4688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4698 "isa_tms320.tcc"
>
class OpSUBF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4707 "isa_tms320.tcc"
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
#line 4720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4730 "isa_tms320.tcc"
>
class OpSUBF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4739 "isa_tms320.tcc"
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
#line 4754 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4764 "isa_tms320.tcc"
>
class OpSUBF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4769 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4773 "isa_tms320.tcc"
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
#line 4786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4796 "isa_tms320.tcc"
>
class OpSUBI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4805 "isa_tms320.tcc"
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
#line 4818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4828 "isa_tms320.tcc"
>
class OpSUBI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4837 "isa_tms320.tcc"
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
#line 4850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4860 "isa_tms320.tcc"
>
class OpSUBI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4869 "isa_tms320.tcc"
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
#line 4884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4894 "isa_tms320.tcc"
>
class OpSUBI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4903 "isa_tms320.tcc"
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
#line 4916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4926 "isa_tms320.tcc"
>
class OpSUBRB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4935 "isa_tms320.tcc"
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
#line 4948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4958 "isa_tms320.tcc"
>
class OpSUBRB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4967 "isa_tms320.tcc"
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
#line 4980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4990 "isa_tms320.tcc"
>
class OpSUBRB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4999 "isa_tms320.tcc"
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
#line 5014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5024 "isa_tms320.tcc"
>
class OpSUBRB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5033 "isa_tms320.tcc"
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
#line 5046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5056 "isa_tms320.tcc"
>
class OpSUBRF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5065 "isa_tms320.tcc"
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
#line 5078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5088 "isa_tms320.tcc"
>
class OpSUBRF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5097 "isa_tms320.tcc"
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
#line 5110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5120 "isa_tms320.tcc"
>
class OpSUBRF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5129 "isa_tms320.tcc"
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
#line 5144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5154 "isa_tms320.tcc"
>
class OpSUBRF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5163 "isa_tms320.tcc"
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
#line 5176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5183 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5186 "isa_tms320.tcc"
>
class OpSUBRI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5195 "isa_tms320.tcc"
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
#line 5208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5218 "isa_tms320.tcc"
>
class OpSUBRI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5227 "isa_tms320.tcc"
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
#line 5240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5250 "isa_tms320.tcc"
>
class OpSUBRI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5259 "isa_tms320.tcc"
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
#line 5274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5284 "isa_tms320.tcc"
>
class OpSUBRI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5293 "isa_tms320.tcc"
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
#line 5306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5309 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5316 "isa_tms320.tcc"
>
class OpTSTB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5325 "isa_tms320.tcc"
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
#line 5338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5348 "isa_tms320.tcc"
>
class OpTSTB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5357 "isa_tms320.tcc"
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
#line 5370 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5373 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5380 "isa_tms320.tcc"
>
class OpTSTB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5389 "isa_tms320.tcc"
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
#line 5404 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5414 "isa_tms320.tcc"
>
class OpTSTB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5423 "isa_tms320.tcc"
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
#line 5436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5446 "isa_tms320.tcc"
>
class OpXOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5455 "isa_tms320.tcc"
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
#line 5468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5475 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5478 "isa_tms320.tcc"
>
class OpXOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5487 "isa_tms320.tcc"
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
#line 5500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5510 "isa_tms320.tcc"
>
class OpXOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5519 "isa_tms320.tcc"
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
#line 5534 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5544 "isa_tms320.tcc"
>
class OpXOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5549 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5553 "isa_tms320.tcc"
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
#line 5566 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5569 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5576 "isa_tms320.tcc"
>
class OpADDC3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5585 "isa_tms320.tcc"
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
#line 5599 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5609 "isa_tms320.tcc"
>
class OpADDC3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5618 "isa_tms320.tcc"
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
#line 5633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5643 "isa_tms320.tcc"
>
class OpADDC3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5652 "isa_tms320.tcc"
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
#line 5667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5670 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5677 "isa_tms320.tcc"
>
class OpADDC3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5686 "isa_tms320.tcc"
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
#line 5702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5705 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5709 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5712 "isa_tms320.tcc"
>
class OpADDF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5721 "isa_tms320.tcc"
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
#line 5735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5745 "isa_tms320.tcc"
>
class OpADDF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5754 "isa_tms320.tcc"
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
#line 5769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5779 "isa_tms320.tcc"
>
class OpADDF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5788 "isa_tms320.tcc"
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
#line 5803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5813 "isa_tms320.tcc"
>
class OpADDF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5822 "isa_tms320.tcc"
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
#line 5838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5848 "isa_tms320.tcc"
>
class OpADDI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5857 "isa_tms320.tcc"
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
#line 5871 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5878 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5881 "isa_tms320.tcc"
>
class OpADDI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5890 "isa_tms320.tcc"
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
class OpADDI3_reg_indir : public Operation<
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
#line 5939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5949 "isa_tms320.tcc"
>
class OpADDI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5958 "isa_tms320.tcc"
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
#line 5974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5981 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5984 "isa_tms320.tcc"
>
class OpAND3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5993 "isa_tms320.tcc"
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
#line 6007 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6017 "isa_tms320.tcc"
>
class OpAND3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6022 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6026 "isa_tms320.tcc"
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
#line 6041 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6051 "isa_tms320.tcc"
>
class OpAND3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6060 "isa_tms320.tcc"
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
#line 6075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6085 "isa_tms320.tcc"
>
class OpAND3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6094 "isa_tms320.tcc"
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
#line 6110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6120 "isa_tms320.tcc"
>
class OpANDN3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6129 "isa_tms320.tcc"
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
#line 6143 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6146 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6150 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6153 "isa_tms320.tcc"
>
class OpANDN3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6162 "isa_tms320.tcc"
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
#line 6177 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6184 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6187 "isa_tms320.tcc"
>
class OpANDN3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6196 "isa_tms320.tcc"
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
#line 6211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6221 "isa_tms320.tcc"
>
class OpANDN3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6230 "isa_tms320.tcc"
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
#line 6246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6256 "isa_tms320.tcc"
>
class OpASH3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6265 "isa_tms320.tcc"
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
#line 6279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6286 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6289 "isa_tms320.tcc"
>
class OpASH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6298 "isa_tms320.tcc"
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
#line 6313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6320 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6323 "isa_tms320.tcc"
>
class OpASH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6332 "isa_tms320.tcc"
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
#line 6347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6357 "isa_tms320.tcc"
>
class OpASH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6366 "isa_tms320.tcc"
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
#line 6382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6392 "isa_tms320.tcc"
>
class OpCMPF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6401 "isa_tms320.tcc"
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
#line 6415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6425 "isa_tms320.tcc"
>
class OpCMPF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6434 "isa_tms320.tcc"
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
#line 6449 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6459 "isa_tms320.tcc"
>
class OpCMPF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6468 "isa_tms320.tcc"
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
#line 6483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6490 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6493 "isa_tms320.tcc"
>
class OpCMPF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6502 "isa_tms320.tcc"
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
#line 6518 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6521 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6525 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6528 "isa_tms320.tcc"
>
class OpCMPI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6533 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6537 "isa_tms320.tcc"
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
#line 6551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6561 "isa_tms320.tcc"
>
class OpCMPI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6570 "isa_tms320.tcc"
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
#line 6585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6595 "isa_tms320.tcc"
>
class OpCMPI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6604 "isa_tms320.tcc"
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
#line 6619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6629 "isa_tms320.tcc"
>
class OpCMPI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6638 "isa_tms320.tcc"
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
#line 6654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6664 "isa_tms320.tcc"
>
class OpLSH3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6673 "isa_tms320.tcc"
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
#line 6687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6697 "isa_tms320.tcc"
>
class OpLSH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6706 "isa_tms320.tcc"
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
#line 6721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6731 "isa_tms320.tcc"
>
class OpLSH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6740 "isa_tms320.tcc"
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
#line 6755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6765 "isa_tms320.tcc"
>
class OpLSH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6774 "isa_tms320.tcc"
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
#line 6790 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6800 "isa_tms320.tcc"
>
class OpMPYF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6809 "isa_tms320.tcc"
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
#line 6823 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6833 "isa_tms320.tcc"
>
class OpMPYF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6842 "isa_tms320.tcc"
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
#line 6857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6867 "isa_tms320.tcc"
>
class OpMPYF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6876 "isa_tms320.tcc"
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
#line 6891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6901 "isa_tms320.tcc"
>
class OpMPYF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6910 "isa_tms320.tcc"
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
#line 6926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6936 "isa_tms320.tcc"
>
class OpMPYI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6945 "isa_tms320.tcc"
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
#line 6959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6966 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6969 "isa_tms320.tcc"
>
class OpMPYI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6978 "isa_tms320.tcc"
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
#line 6993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7003 "isa_tms320.tcc"
>
class OpMPYI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7012 "isa_tms320.tcc"
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
#line 7027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7037 "isa_tms320.tcc"
>
class OpMPYI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7046 "isa_tms320.tcc"
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
#line 7062 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7065 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7069 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7072 "isa_tms320.tcc"
>
class OpOR3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7081 "isa_tms320.tcc"
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
#line 7095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7105 "isa_tms320.tcc"
>
class OpOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7114 "isa_tms320.tcc"
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
#line 7129 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7136 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7139 "isa_tms320.tcc"
>
class OpOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7148 "isa_tms320.tcc"
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
#line 7163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7173 "isa_tms320.tcc"
>
class OpOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7182 "isa_tms320.tcc"
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
#line 7198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7205 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7208 "isa_tms320.tcc"
>
class OpSUBB3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7217 "isa_tms320.tcc"
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
#line 7231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7241 "isa_tms320.tcc"
>
class OpSUBB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7250 "isa_tms320.tcc"
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
#line 7265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7275 "isa_tms320.tcc"
>
class OpSUBB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7284 "isa_tms320.tcc"
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
#line 7299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7309 "isa_tms320.tcc"
>
class OpSUBB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7318 "isa_tms320.tcc"
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
#line 7334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7344 "isa_tms320.tcc"
>
class OpSUBF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7353 "isa_tms320.tcc"
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
#line 7367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7377 "isa_tms320.tcc"
>
class OpSUBF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7386 "isa_tms320.tcc"
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
#line 7401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7411 "isa_tms320.tcc"
>
class OpSUBF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7420 "isa_tms320.tcc"
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
#line 7435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7442 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7445 "isa_tms320.tcc"
>
class OpSUBF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7454 "isa_tms320.tcc"
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
#line 7470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7473 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7480 "isa_tms320.tcc"
>
class OpSUBI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7489 "isa_tms320.tcc"
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
#line 7503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7510 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7513 "isa_tms320.tcc"
>
class OpSUBI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7522 "isa_tms320.tcc"
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
#line 7537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7547 "isa_tms320.tcc"
>
class OpSUBI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7556 "isa_tms320.tcc"
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
#line 7571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7581 "isa_tms320.tcc"
>
class OpSUBI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7590 "isa_tms320.tcc"
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
#line 7606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7616 "isa_tms320.tcc"
>
class OpTSTB3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7625 "isa_tms320.tcc"
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
#line 7639 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7649 "isa_tms320.tcc"
>
class OpTSTB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7658 "isa_tms320.tcc"
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
#line 7673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7683 "isa_tms320.tcc"
>
class OpTSTB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7692 "isa_tms320.tcc"
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
#line 7707 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7717 "isa_tms320.tcc"
>
class OpTSTB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7726 "isa_tms320.tcc"
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
#line 7742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7752 "isa_tms320.tcc"
>
class OpXOR3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7761 "isa_tms320.tcc"
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
#line 7775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7785 "isa_tms320.tcc"
>
class OpXOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7794 "isa_tms320.tcc"
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
#line 7809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7816 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7819 "isa_tms320.tcc"
>
class OpXOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7828 "isa_tms320.tcc"
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
#line 7843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7846 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7853 "isa_tms320.tcc"
>
class OpXOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7862 "isa_tms320.tcc"
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
#line 7878 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7888 "isa_tms320.tcc"
>
class OpBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7897 "isa_tms320.tcc"
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
#line 7910 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7913 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7917 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7920 "isa_tms320.tcc"
>
class OpBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7929 "isa_tms320.tcc"
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
#line 7942 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7949 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7952 "isa_tms320.tcc"
>
class OpBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7957 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7961 "isa_tms320.tcc"
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
class OpBcondD_disp : public Operation<
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
	OpBcondD_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8016 "isa_tms320.tcc"
>
class OpBR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8025 "isa_tms320.tcc"
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
#line 8037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8044 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8047 "isa_tms320.tcc"
>
class OpBRD : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8056 "isa_tms320.tcc"
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
#line 8068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8078 "isa_tms320.tcc"
>
class OpCALL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8087 "isa_tms320.tcc"
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
#line 8099 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8102 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8106 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8109 "isa_tms320.tcc"
>
class OpCALLcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8114 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8118 "isa_tms320.tcc"
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
#line 8131 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8141 "isa_tms320.tcc"
>
class OpCALLcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8146 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8150 "isa_tms320.tcc"
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
#line 8163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8173 "isa_tms320.tcc"
>
class OpDBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8182 "isa_tms320.tcc"
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
#line 8196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8203 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8206 "isa_tms320.tcc"
>
class OpDBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8215 "isa_tms320.tcc"
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
#line 8229 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8232 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8239 "isa_tms320.tcc"
>
class OpDBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8248 "isa_tms320.tcc"
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
#line 8262 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8265 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8272 "isa_tms320.tcc"
>
class OpDBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8281 "isa_tms320.tcc"
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
#line 8295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8305 "isa_tms320.tcc"
>
class OpIACK_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8314 "isa_tms320.tcc"
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
#line 8326 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8333 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8336 "isa_tms320.tcc"
>
class OpIACK_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8345 "isa_tms320.tcc"
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
#line 8359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8369 "isa_tms320.tcc"
>
class OpIDLE : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8378 "isa_tms320.tcc"
>
{
public:
	OpIDLE(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8399 "isa_tms320.tcc"
>
class OpNOP_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8408 "isa_tms320.tcc"
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
#line 8420 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8430 "isa_tms320.tcc"
>
class OpNOP_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8439 "isa_tms320.tcc"
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
#line 8453 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8463 "isa_tms320.tcc"
>
class OpRETIcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8472 "isa_tms320.tcc"
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
#line 8484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8494 "isa_tms320.tcc"
>
class OpRETScond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8503 "isa_tms320.tcc"
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
#line 8515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8522 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8525 "isa_tms320.tcc"
>
class OpRPTB : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8534 "isa_tms320.tcc"
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
#line 8546 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8549 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8556 "isa_tms320.tcc"
>
class OpRPTS_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8565 "isa_tms320.tcc"
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
#line 8577 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8587 "isa_tms320.tcc"
>
class OpRPTS_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8592 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8596 "isa_tms320.tcc"
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
#line 8608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8618 "isa_tms320.tcc"
>
class OpRPTS_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8627 "isa_tms320.tcc"
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
#line 8641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8651 "isa_tms320.tcc"
>
class OpRPTS_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8660 "isa_tms320.tcc"
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
#line 8672 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8679 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8682 "isa_tms320.tcc"
>
class OpSWI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8691 "isa_tms320.tcc"
>
{
public:
	OpSWI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8705 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8709 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8712 "isa_tms320.tcc"
>
class OpTRAPcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8721 "isa_tms320.tcc"
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
#line 8734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8744 "isa_tms320.tcc"
>
class OpIDLE2 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8753 "isa_tms320.tcc"
>
{
public:
	OpIDLE2(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8774 "isa_tms320.tcc"
>
class OpLOPOWER : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8783 "isa_tms320.tcc"
>
{
public:
	OpLOPOWER(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8794 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8797 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8801 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8804 "isa_tms320.tcc"
>
class OpMAXSPEED : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8813 "isa_tms320.tcc"
>
{
public:
	OpMAXSPEED(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8824 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8831 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8834 "isa_tms320.tcc"
>
class OpLDFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8843 "isa_tms320.tcc"
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
#line 8856 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8859 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8863 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8866 "isa_tms320.tcc"
>
class OpLDFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8875 "isa_tms320.tcc"
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
class OpLDII_dir : public Operation<
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
	OpLDII_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8932 "isa_tms320.tcc"
>
class OpLDII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8941 "isa_tms320.tcc"
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
#line 8956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8966 "isa_tms320.tcc"
>
class OpSIGI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8975 "isa_tms320.tcc"
>
{
public:
	OpSIGI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8996 "isa_tms320.tcc"
>
class OpSTFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9005 "isa_tms320.tcc"
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
#line 9018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9028 "isa_tms320.tcc"
>
class OpSTFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9037 "isa_tms320.tcc"
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
#line 9052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9062 "isa_tms320.tcc"
>
class OpSTII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9071 "isa_tms320.tcc"
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
#line 9084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9094 "isa_tms320.tcc"
>
class OpSTII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9099 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9103 "isa_tms320.tcc"
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
#line 9118 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9128 "isa_tms320.tcc"
>
class OpFLOAT_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9137 "isa_tms320.tcc"
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
#line 9154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9164 "isa_tms320.tcc"
>
class OpFLOAT_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9173 "isa_tms320.tcc"
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
#line 9189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9199 "isa_tms320.tcc"
>
class OpLDF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9208 "isa_tms320.tcc"
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
#line 9225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9235 "isa_tms320.tcc"
>
class OpLDF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9244 "isa_tms320.tcc"
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
#line 9260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9270 "isa_tms320.tcc"
>
class OpLDI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9279 "isa_tms320.tcc"
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
#line 9296 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9306 "isa_tms320.tcc"
>
class OpLDI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9315 "isa_tms320.tcc"
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
#line 9331 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9341 "isa_tms320.tcc"
>
class OpLSH3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9350 "isa_tms320.tcc"
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
#line 9368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9378 "isa_tms320.tcc"
>
class OpLSH3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9387 "isa_tms320.tcc"
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
#line 9404 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9414 "isa_tms320.tcc"
>
class OpMPYF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9423 "isa_tms320.tcc"
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
#line 9441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9451 "isa_tms320.tcc"
>
class OpMPYF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9460 "isa_tms320.tcc"
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
#line 9477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9487 "isa_tms320.tcc"
>
class OpMPYI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9496 "isa_tms320.tcc"
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
#line 9514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9524 "isa_tms320.tcc"
>
class OpMPYI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9533 "isa_tms320.tcc"
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
#line 9550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9557 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9560 "isa_tms320.tcc"
>
class OpNEGF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9569 "isa_tms320.tcc"
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
#line 9586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9593 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9596 "isa_tms320.tcc"
>
class OpNEGF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9605 "isa_tms320.tcc"
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
#line 9621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9628 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9631 "isa_tms320.tcc"
>
class OpNEGI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9640 "isa_tms320.tcc"
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
#line 9657 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9664 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9667 "isa_tms320.tcc"
>
class OpNEGI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9676 "isa_tms320.tcc"
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
#line 9692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9695 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9702 "isa_tms320.tcc"
>
class OpNOT_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9711 "isa_tms320.tcc"
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
#line 9728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9738 "isa_tms320.tcc"
>
class OpNOT_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9747 "isa_tms320.tcc"
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
#line 9763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9773 "isa_tms320.tcc"
>
class OpOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9782 "isa_tms320.tcc"
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
#line 9800 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9807 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9810 "isa_tms320.tcc"
>
class OpOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9819 "isa_tms320.tcc"
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
#line 9836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9846 "isa_tms320.tcc"
>
class OpSTF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9855 "isa_tms320.tcc"
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
#line 9872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9882 "isa_tms320.tcc"
>
class OpSTF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9891 "isa_tms320.tcc"
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
#line 9907 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9910 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9917 "isa_tms320.tcc"
>
class OpSTI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9926 "isa_tms320.tcc"
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
#line 9943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9953 "isa_tms320.tcc"
>
class OpSTI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9962 "isa_tms320.tcc"
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
#line 9978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9988 "isa_tms320.tcc"
>
class OpSUBF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9997 "isa_tms320.tcc"
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
#line 10015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10025 "isa_tms320.tcc"
>
class OpSUBF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10034 "isa_tms320.tcc"
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
#line 10051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10061 "isa_tms320.tcc"
>
class OpSUBI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10070 "isa_tms320.tcc"
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
#line 10088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10098 "isa_tms320.tcc"
>
class OpSUBI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10107 "isa_tms320.tcc"
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
#line 10124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10131 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10134 "isa_tms320.tcc"
>
class OpXOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10139 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10143 "isa_tms320.tcc"
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
#line 10161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10168 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10171 "isa_tms320.tcc"
>
class OpXOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10176 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10180 "isa_tms320.tcc"
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
#line 10197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10207 "isa_tms320.tcc"
>
class OpLDF_LDF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10216 "isa_tms320.tcc"
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
#line 10233 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10243 "isa_tms320.tcc"
>
class OpLDF_LDF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10248 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10252 "isa_tms320.tcc"
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
#line 10268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10278 "isa_tms320.tcc"
>
class OpLDI_LDI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10287 "isa_tms320.tcc"
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
#line 10304 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10314 "isa_tms320.tcc"
>
class OpLDI_LDI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10323 "isa_tms320.tcc"
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
#line 10339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10349 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10358 "isa_tms320.tcc"
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
#line 10378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10388 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10397 "isa_tms320.tcc"
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
class OpMPYF3_SUBF3 : public Operation<
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
#line 10454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10464 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10473 "isa_tms320.tcc"
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
#line 10491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10501 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10510 "isa_tms320.tcc"
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
#line 10530 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10533 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10540 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10549 "isa_tms320.tcc"
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
#line 10567 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10577 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10586 "isa_tms320.tcc"
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
#line 10606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10616 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10625 "isa_tms320.tcc"
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
#line 10697 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10707 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10716 "isa_tms320.tcc"
> *DecodeOpLDE_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10722 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10726 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10744 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10753 "isa_tms320.tcc"
> *DecodeOpLDE_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10759 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10763 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10781 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10790 "isa_tms320.tcc"
> *DecodeOpLDE_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10800 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10818 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10827 "isa_tms320.tcc"
> *DecodeOpLDE_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10837 "isa_tms320.tcc"
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
#line 10851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10861 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10870 "isa_tms320.tcc"
> *DecodeOpLDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10880 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10898 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10907 "isa_tms320.tcc"
> *DecodeOpLDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10913 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10917 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10935 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10944 "isa_tms320.tcc"
> *DecodeOpLDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10950 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10954 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10972 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10981 "isa_tms320.tcc"
> *DecodeOpLDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10987 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10991 "isa_tms320.tcc"
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
#line 11005 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11012 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11015 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11024 "isa_tms320.tcc"
> *DecodeOpLDFcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11030 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11034 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11042 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11052 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11057 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11061 "isa_tms320.tcc"
> *DecodeOpLDFcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11067 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11071 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11079 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11089 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11098 "isa_tms320.tcc"
> *DecodeOpLDFcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11104 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11108 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11126 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11135 "isa_tms320.tcc"
> *DecodeOpLDFcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11141 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11145 "isa_tms320.tcc"
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
#line 11159 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11162 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11166 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11169 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11178 "isa_tms320.tcc"
> *DecodeOpLDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11184 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11188 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11203 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11206 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11215 "isa_tms320.tcc"
> *DecodeOpLDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11221 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11225 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11233 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11243 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11248 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11252 "isa_tms320.tcc"
> *DecodeOpLDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11258 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11262 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11270 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11280 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11289 "isa_tms320.tcc"
> *DecodeOpLDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11295 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11299 "isa_tms320.tcc"
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
OpLDIcond_reg<
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
#line 111 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *cond_name = COND_NAME[cond];
		const char *src_reg_name = REG_NAME[src];
		const char *dst_reg_name = REG_NAME[dst];
		if(!cond_name || !src_reg_name || !dst_reg_name) return false;

		os << "LDI" << cond_name << " " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 11361 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11377 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11386 "isa_tms320.tcc"
> *DecodeOpLDIcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11392 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11396 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11404 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11414 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11423 "isa_tms320.tcc"
> *DecodeOpLDIcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11429 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11433 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11450 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11454 "isa_tms320.tcc"
OpLDIcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11462 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11466 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11469 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11473 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11476 "isa_tms320.tcc"
)
{
#line 124 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];

		if(!cond_name || !dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDI" << cond_name << " " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 11490 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11506 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11515 "isa_tms320.tcc"
> *DecodeOpLDIcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11521 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11525 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11543 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11552 "isa_tms320.tcc"
> *DecodeOpLDIcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11562 "isa_tms320.tcc"
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
#line 11576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11586 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11595 "isa_tms320.tcc"
> *DecodeOpLDM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11605 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11620 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11623 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11632 "isa_tms320.tcc"
> *DecodeOpLDM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11638 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11642 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11650 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11657 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11660 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11669 "isa_tms320.tcc"
> *DecodeOpLDM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11675 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11679 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11697 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11706 "isa_tms320.tcc"
> *DecodeOpLDM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11712 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11716 "isa_tms320.tcc"
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
#line 11730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11740 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11749 "isa_tms320.tcc"
> *DecodeOpLDP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11755 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11759 "isa_tms320.tcc"
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
#line 11772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11782 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11786 "isa_tms320.tcc"
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 11790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11794 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11798 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11801 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11805 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11808 "isa_tms320.tcc"
)
{
#line 170 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];

		if(!dst_reg_name) return false;

		os << "POP " << dst_reg_name;
		return true;
	}
#line 11820 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11836 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11845 "isa_tms320.tcc"
> *DecodeOpPOP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11851 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11855 "isa_tms320.tcc"
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
#line 11868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11878 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11882 "isa_tms320.tcc"
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 11886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11890 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11894 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11897 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11901 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11904 "isa_tms320.tcc"
)
{
#line 189 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];

		if(!dst_reg_name) return false;

		os << "POPF " << dst_reg_name;
		return true;
	}
#line 11916 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11932 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11941 "isa_tms320.tcc"
> *DecodeOpPOPF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11947 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11951 "isa_tms320.tcc"
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
#line 11964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11974 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11978 "isa_tms320.tcc"
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 11982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11986 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11990 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11993 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11997 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12000 "isa_tms320.tcc"
)
{
#line 208 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *src_reg_name = REG_NAME[src];

		if(!src_reg_name) return false;

		os << "PUSH " << src_reg_name;
		return true;
	}
#line 12012 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12028 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12037 "isa_tms320.tcc"
> *DecodeOpPUSH(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12043 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12047 "isa_tms320.tcc"
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
#line 12060 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12063 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12067 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12070 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12074 "isa_tms320.tcc"
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 12078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12082 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12086 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12089 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12093 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12096 "isa_tms320.tcc"
)
{
#line 227 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *src_reg_name = REG_NAME[src];

		if(!src_reg_name) return false;

		os << "PUSHF " << src_reg_name;
		return true;
	}
#line 12108 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12124 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12133 "isa_tms320.tcc"
> *DecodeOpPUSHF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12139 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12143 "isa_tms320.tcc"
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
#line 12157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12167 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12176 "isa_tms320.tcc"
> *DecodeOpSTF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12186 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12204 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12213 "isa_tms320.tcc"
> *DecodeOpSTF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12219 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12223 "isa_tms320.tcc"
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
#line 12237 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12247 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12256 "isa_tms320.tcc"
> *DecodeOpSTI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12262 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12266 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12284 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12293 "isa_tms320.tcc"
> *DecodeOpSTI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12299 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12303 "isa_tms320.tcc"
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
#line 12388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12398 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12407 "isa_tms320.tcc"
> *DecodeOpABSF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12417 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12432 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12435 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12440 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12444 "isa_tms320.tcc"
> *DecodeOpABSF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12450 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12454 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12465 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12469 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12472 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12481 "isa_tms320.tcc"
> *DecodeOpABSF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12487 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12491 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12509 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12518 "isa_tms320.tcc"
> *DecodeOpABSF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12528 "isa_tms320.tcc"
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
#line 12542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12552 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12557 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12561 "isa_tms320.tcc"
> *DecodeOpABSI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12567 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12571 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12589 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12598 "isa_tms320.tcc"
> *DecodeOpABSI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12604 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12608 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12623 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12626 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12631 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12635 "isa_tms320.tcc"
> *DecodeOpABSI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12641 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12645 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12653 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12663 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12668 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12672 "isa_tms320.tcc"
> *DecodeOpABSI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12678 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12682 "isa_tms320.tcc"
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
#line 12696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12706 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12715 "isa_tms320.tcc"
> *DecodeOpADDC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12721 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12725 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12743 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12752 "isa_tms320.tcc"
> *DecodeOpADDC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12758 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12762 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12777 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12780 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12789 "isa_tms320.tcc"
> *DecodeOpADDC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12795 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12799 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12807 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12817 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12826 "isa_tms320.tcc"
> *DecodeOpADDC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12832 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12836 "isa_tms320.tcc"
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
#line 12850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12860 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12869 "isa_tms320.tcc"
> *DecodeOpADDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12875 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12879 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12887 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12894 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12897 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12902 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12906 "isa_tms320.tcc"
> *DecodeOpADDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12912 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12916 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12924 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12931 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12934 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12939 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12943 "isa_tms320.tcc"
> *DecodeOpADDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12949 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12953 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12971 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12980 "isa_tms320.tcc"
> *DecodeOpADDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12986 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12990 "isa_tms320.tcc"
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
#line 13004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13014 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13023 "isa_tms320.tcc"
> *DecodeOpADDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13029 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13033 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13041 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13051 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13060 "isa_tms320.tcc"
> *DecodeOpADDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13066 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13070 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13088 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13097 "isa_tms320.tcc"
> *DecodeOpADDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13103 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13107 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13122 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13125 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13134 "isa_tms320.tcc"
> *DecodeOpADDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13140 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13144 "isa_tms320.tcc"
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
#line 13158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13168 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13177 "isa_tms320.tcc"
> *DecodeOpAND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13183 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13187 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13202 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13205 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13210 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13214 "isa_tms320.tcc"
> *DecodeOpAND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13220 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13224 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13242 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13251 "isa_tms320.tcc"
> *DecodeOpAND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13257 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13261 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13272 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13279 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13284 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13288 "isa_tms320.tcc"
> *DecodeOpAND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13294 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13298 "isa_tms320.tcc"
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
#line 13312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13322 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13331 "isa_tms320.tcc"
> *DecodeOpANDN_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13337 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13341 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13349 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13356 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13359 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13368 "isa_tms320.tcc"
> *DecodeOpANDN_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13374 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13378 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13396 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13405 "isa_tms320.tcc"
> *DecodeOpANDN_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13411 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13415 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13433 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13442 "isa_tms320.tcc"
> *DecodeOpANDN_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13448 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13452 "isa_tms320.tcc"
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
#line 13466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13476 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13485 "isa_tms320.tcc"
> *DecodeOpASH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13491 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13495 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13510 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13513 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13522 "isa_tms320.tcc"
> *DecodeOpASH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13528 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13532 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13550 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13559 "isa_tms320.tcc"
> *DecodeOpASH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13565 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13569 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13577 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13587 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13592 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13596 "isa_tms320.tcc"
> *DecodeOpASH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13602 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13606 "isa_tms320.tcc"
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
#line 13620 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13627 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13630 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13639 "isa_tms320.tcc"
> *DecodeOpCMPF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13645 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13649 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13657 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13664 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13667 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13676 "isa_tms320.tcc"
> *DecodeOpCMPF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13682 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13686 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13697 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13704 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13709 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13713 "isa_tms320.tcc"
> *DecodeOpCMPF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13719 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13723 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13734 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13741 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13750 "isa_tms320.tcc"
> *DecodeOpCMPF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13756 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13760 "isa_tms320.tcc"
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
#line 13774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13781 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13784 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13793 "isa_tms320.tcc"
> *DecodeOpCMPI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13799 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13803 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13821 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13830 "isa_tms320.tcc"
> *DecodeOpCMPI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13836 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13840 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13855 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13858 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13863 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13867 "isa_tms320.tcc"
> *DecodeOpCMPI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13873 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13877 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13892 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13895 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13904 "isa_tms320.tcc"
> *DecodeOpCMPI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13910 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13914 "isa_tms320.tcc"
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
#line 13928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13938 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13947 "isa_tms320.tcc"
> *DecodeOpFIX_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13957 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13975 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13980 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13984 "isa_tms320.tcc"
> *DecodeOpFIX_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13990 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13994 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14005 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14009 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14012 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14021 "isa_tms320.tcc"
> *DecodeOpFIX_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14027 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14031 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14049 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14058 "isa_tms320.tcc"
> *DecodeOpFIX_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14064 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14068 "isa_tms320.tcc"
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
#line 14082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14092 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14101 "isa_tms320.tcc"
> *DecodeOpFLOAT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14107 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14111 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14129 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14138 "isa_tms320.tcc"
> *DecodeOpFLOAT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14144 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14148 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14166 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14175 "isa_tms320.tcc"
> *DecodeOpFLOAT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14181 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14185 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14203 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14208 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14212 "isa_tms320.tcc"
> *DecodeOpFLOAT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14218 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14222 "isa_tms320.tcc"
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
#line 14236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14246 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14255 "isa_tms320.tcc"
> *DecodeOpLSH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14265 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14283 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14292 "isa_tms320.tcc"
> *DecodeOpLSH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14298 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14302 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14320 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14329 "isa_tms320.tcc"
> *DecodeOpLSH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14335 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14339 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14357 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14366 "isa_tms320.tcc"
> *DecodeOpLSH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14372 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14376 "isa_tms320.tcc"
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
#line 14390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14397 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14400 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14409 "isa_tms320.tcc"
> *DecodeOpMPYF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14415 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14419 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14434 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14437 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14446 "isa_tms320.tcc"
> *DecodeOpMPYF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14452 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14456 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14474 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14483 "isa_tms320.tcc"
> *DecodeOpMPYF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14489 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14493 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14511 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14520 "isa_tms320.tcc"
> *DecodeOpMPYF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14526 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14530 "isa_tms320.tcc"
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
#line 14544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14554 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14563 "isa_tms320.tcc"
> *DecodeOpMPYI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14569 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14573 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14591 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14600 "isa_tms320.tcc"
> *DecodeOpMPYI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14606 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14610 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14628 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14637 "isa_tms320.tcc"
> *DecodeOpMPYI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14643 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14647 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14658 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14662 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14665 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14670 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14674 "isa_tms320.tcc"
> *DecodeOpMPYI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14680 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14684 "isa_tms320.tcc"
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
#line 14698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14708 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14717 "isa_tms320.tcc"
> *DecodeOpNEGB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14727 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14745 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14754 "isa_tms320.tcc"
> *DecodeOpNEGB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14760 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14764 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14782 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14791 "isa_tms320.tcc"
> *DecodeOpNEGB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14797 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14801 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14816 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14819 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14828 "isa_tms320.tcc"
> *DecodeOpNEGB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14834 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14838 "isa_tms320.tcc"
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
#line 14852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14862 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14871 "isa_tms320.tcc"
> *DecodeOpNEGF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14877 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14881 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14899 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14908 "isa_tms320.tcc"
> *DecodeOpNEGF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14914 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14918 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14936 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14945 "isa_tms320.tcc"
> *DecodeOpNEGF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14951 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14955 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14970 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14973 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14982 "isa_tms320.tcc"
> *DecodeOpNEGF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14988 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14992 "isa_tms320.tcc"
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
#line 15006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15016 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15025 "isa_tms320.tcc"
> *DecodeOpNEGI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15031 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15035 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15053 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15062 "isa_tms320.tcc"
> *DecodeOpNEGI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15068 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15072 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15087 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15090 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15095 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15099 "isa_tms320.tcc"
> *DecodeOpNEGI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15105 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15109 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15127 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15136 "isa_tms320.tcc"
> *DecodeOpNEGI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15142 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15146 "isa_tms320.tcc"
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
#line 15160 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15167 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15170 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15175 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15179 "isa_tms320.tcc"
> *DecodeOpNORM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15185 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15189 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15207 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15216 "isa_tms320.tcc"
> *DecodeOpNORM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15222 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15226 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15244 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15253 "isa_tms320.tcc"
> *DecodeOpNORM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15259 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15263 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15281 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15286 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15290 "isa_tms320.tcc"
> *DecodeOpNORM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15296 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15300 "isa_tms320.tcc"
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
#line 15314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15324 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15333 "isa_tms320.tcc"
> *DecodeOpNOT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15339 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15343 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15361 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15370 "isa_tms320.tcc"
> *DecodeOpNOT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15376 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15380 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15398 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15407 "isa_tms320.tcc"
> *DecodeOpNOT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15417 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15432 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15435 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15440 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15444 "isa_tms320.tcc"
> *DecodeOpNOT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15450 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15454 "isa_tms320.tcc"
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
#line 15468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15475 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15478 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15487 "isa_tms320.tcc"
> *DecodeOpOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15493 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15497 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15515 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15520 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15524 "isa_tms320.tcc"
> *DecodeOpOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15530 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15534 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15552 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15557 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15561 "isa_tms320.tcc"
> *DecodeOpOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15567 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15571 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15589 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15598 "isa_tms320.tcc"
> *DecodeOpOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15604 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15608 "isa_tms320.tcc"
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
#line 15622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15632 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15641 "isa_tms320.tcc"
> *DecodeOpRND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15647 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15651 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15659 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15666 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15669 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15678 "isa_tms320.tcc"
> *DecodeOpRND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15684 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15688 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15706 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15715 "isa_tms320.tcc"
> *DecodeOpRND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15721 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15725 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15743 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15752 "isa_tms320.tcc"
> *DecodeOpRND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15758 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15762 "isa_tms320.tcc"
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
#line 15776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15783 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15786 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15795 "isa_tms320.tcc"
> *DecodeOpROL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15801 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15805 "isa_tms320.tcc"
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
#line 15819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15829 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15838 "isa_tms320.tcc"
> *DecodeOpROLC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15844 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15848 "isa_tms320.tcc"
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
#line 15862 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15872 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15881 "isa_tms320.tcc"
> *DecodeOpROR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15887 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15891 "isa_tms320.tcc"
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
#line 15905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15915 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15924 "isa_tms320.tcc"
> *DecodeOpRORC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15930 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15934 "isa_tms320.tcc"
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
#line 15948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15958 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15967 "isa_tms320.tcc"
> *DecodeOpSUBB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15973 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15977 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15995 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16004 "isa_tms320.tcc"
> *DecodeOpSUBB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16010 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16014 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16025 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16029 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16032 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16041 "isa_tms320.tcc"
> *DecodeOpSUBB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16047 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16051 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16066 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16069 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16078 "isa_tms320.tcc"
> *DecodeOpSUBB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16084 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16088 "isa_tms320.tcc"
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
#line 16102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16105 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16112 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16121 "isa_tms320.tcc"
> *DecodeOpSUBC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16127 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16131 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16149 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16158 "isa_tms320.tcc"
> *DecodeOpSUBC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16164 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16168 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16183 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16186 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16195 "isa_tms320.tcc"
> *DecodeOpSUBC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16201 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16205 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16220 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16223 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16232 "isa_tms320.tcc"
> *DecodeOpSUBC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16238 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16242 "isa_tms320.tcc"
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
#line 16256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16263 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16266 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16275 "isa_tms320.tcc"
> *DecodeOpSUBF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16281 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16285 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16303 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16312 "isa_tms320.tcc"
> *DecodeOpSUBF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16318 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16322 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16337 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16340 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16345 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16349 "isa_tms320.tcc"
> *DecodeOpSUBF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16355 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16359 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16377 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16386 "isa_tms320.tcc"
> *DecodeOpSUBF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16392 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16396 "isa_tms320.tcc"
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
#line 16410 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16420 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16429 "isa_tms320.tcc"
> *DecodeOpSUBI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16435 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16439 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16457 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16466 "isa_tms320.tcc"
> *DecodeOpSUBI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16472 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16476 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16494 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16503 "isa_tms320.tcc"
> *DecodeOpSUBI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16509 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16513 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16531 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16540 "isa_tms320.tcc"
> *DecodeOpSUBI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16546 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16550 "isa_tms320.tcc"
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
#line 16564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16574 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16583 "isa_tms320.tcc"
> *DecodeOpSUBRB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16589 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16593 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16604 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16611 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16620 "isa_tms320.tcc"
> *DecodeOpSUBRB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16626 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16630 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16638 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16641 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16645 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16648 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16657 "isa_tms320.tcc"
> *DecodeOpSUBRB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16663 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16667 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16675 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16678 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16682 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16685 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16694 "isa_tms320.tcc"
> *DecodeOpSUBRB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16700 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16704 "isa_tms320.tcc"
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
#line 16718 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16721 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16725 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16728 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16737 "isa_tms320.tcc"
> *DecodeOpSUBRF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16743 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16747 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16765 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16774 "isa_tms320.tcc"
> *DecodeOpSUBRF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16780 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16784 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16792 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16799 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16802 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16807 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16811 "isa_tms320.tcc"
> *DecodeOpSUBRF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16817 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16821 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16839 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16848 "isa_tms320.tcc"
> *DecodeOpSUBRF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16854 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16858 "isa_tms320.tcc"
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
#line 16872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16882 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16891 "isa_tms320.tcc"
> *DecodeOpSUBRI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16897 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16901 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16919 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16928 "isa_tms320.tcc"
> *DecodeOpSUBRI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16934 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16938 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16956 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16965 "isa_tms320.tcc"
> *DecodeOpSUBRI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16971 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16975 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16993 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17002 "isa_tms320.tcc"
> *DecodeOpSUBRI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17008 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17012 "isa_tms320.tcc"
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
#line 17026 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17033 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17036 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17045 "isa_tms320.tcc"
> *DecodeOpTSTB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17051 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17055 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17073 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17082 "isa_tms320.tcc"
> *DecodeOpTSTB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17088 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17092 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17100 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17107 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17110 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17115 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17119 "isa_tms320.tcc"
> *DecodeOpTSTB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17125 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17129 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17147 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17156 "isa_tms320.tcc"
> *DecodeOpTSTB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17162 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17166 "isa_tms320.tcc"
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
#line 17180 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17183 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17190 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17199 "isa_tms320.tcc"
> *DecodeOpXOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17205 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17209 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17220 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17224 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17227 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17232 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17236 "isa_tms320.tcc"
> *DecodeOpXOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17242 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17246 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17254 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17257 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17261 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17264 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17269 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17273 "isa_tms320.tcc"
> *DecodeOpXOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17279 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17283 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17301 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17310 "isa_tms320.tcc"
> *DecodeOpXOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17316 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17320 "isa_tms320.tcc"
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
#line 17387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17397 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17406 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17412 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17416 "isa_tms320.tcc"
	>(code, addr);
}


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
> *DecodeOpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17449 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17453 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17471 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17480 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17486 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17490 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17508 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17513 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17517 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17523 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17527 "isa_tms320.tcc"
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
#line 17541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17548 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17551 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17560 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17566 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17570 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17588 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17597 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17603 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17607 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17618 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17625 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17630 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17634 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17640 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17644 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17652 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17659 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17662 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17671 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17677 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17681 "isa_tms320.tcc"
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
#line 17695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17705 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17714 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17720 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17724 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17739 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17742 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17751 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17757 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17761 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17779 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17788 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17794 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17798 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17813 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17816 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17825 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17831 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17835 "isa_tms320.tcc"
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
#line 17849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17856 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17859 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17868 "isa_tms320.tcc"
> *DecodeOpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17874 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17878 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17896 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17905 "isa_tms320.tcc"
> *DecodeOpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17911 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17915 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17933 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17942 "isa_tms320.tcc"
> *DecodeOpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17948 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17952 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17970 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17979 "isa_tms320.tcc"
> *DecodeOpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17985 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17989 "isa_tms320.tcc"
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
#line 18003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18010 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18013 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18022 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18028 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18032 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18040 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18047 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18050 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18059 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18065 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18069 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18087 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18096 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18102 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18106 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18124 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18133 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18139 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18143 "isa_tms320.tcc"
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
#line 18157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18167 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18176 "isa_tms320.tcc"
> *DecodeOpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18186 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18204 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18213 "isa_tms320.tcc"
> *DecodeOpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18219 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18223 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18241 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18250 "isa_tms320.tcc"
> *DecodeOpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18256 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18260 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18278 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18287 "isa_tms320.tcc"
> *DecodeOpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18293 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18297 "isa_tms320.tcc"
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
#line 18311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18321 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18330 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18336 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18340 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18358 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18367 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18373 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18377 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18392 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18395 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18404 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18410 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18414 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18429 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18432 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18441 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18447 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18451 "isa_tms320.tcc"
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
#line 18465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18472 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18475 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18484 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18494 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18502 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18505 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18509 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18512 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18521 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18527 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18531 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18542 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18546 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18549 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18558 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18564 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18568 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18586 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18595 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18605 "isa_tms320.tcc"
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
#line 18619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18629 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18638 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18644 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18648 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18666 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18675 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18681 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18685 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18700 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18703 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18712 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18718 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18722 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18740 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18749 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18755 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18759 "isa_tms320.tcc"
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
#line 18773 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18780 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18783 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18792 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18798 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18802 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18820 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18829 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18835 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18839 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18847 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18850 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18854 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18857 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18862 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18866 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18872 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18876 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18894 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18903 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18909 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18913 "isa_tms320.tcc"
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
#line 18927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18930 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18937 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18946 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18952 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18956 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18974 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18983 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18989 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18993 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19011 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19020 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19026 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19030 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19048 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19057 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19063 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19067 "isa_tms320.tcc"
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
#line 19081 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19084 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19091 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19100 "isa_tms320.tcc"
> *DecodeOpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19106 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19110 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19118 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19128 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19137 "isa_tms320.tcc"
> *DecodeOpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19143 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19147 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19165 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19174 "isa_tms320.tcc"
> *DecodeOpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19180 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19184 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19202 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19211 "isa_tms320.tcc"
> *DecodeOpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19217 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19221 "isa_tms320.tcc"
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
#line 19235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19242 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19245 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19254 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19260 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19264 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19282 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19291 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19297 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19301 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19312 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19319 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19328 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19334 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19338 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19356 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19365 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19371 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19375 "isa_tms320.tcc"
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
#line 19389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19399 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19408 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19414 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19418 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19436 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19445 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19451 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19455 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19473 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19482 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19488 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19492 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19510 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19519 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19525 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19529 "isa_tms320.tcc"
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
#line 19543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19553 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19562 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19568 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19572 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19590 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19599 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19605 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19609 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19627 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19636 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19642 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19646 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19664 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19673 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19679 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19683 "isa_tms320.tcc"
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
#line 19697 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19707 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19716 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19722 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19726 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19744 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19753 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19759 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19763 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19781 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19790 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19800 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19818 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19827 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19837 "isa_tms320.tcc"
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
#line 19851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19861 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19870 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19880 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19898 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19907 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19913 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19917 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19935 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19944 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19950 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19954 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19972 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19981 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19987 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19991 "isa_tms320.tcc"
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
#line 20058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20068 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20077 "isa_tms320.tcc"
> *DecodeOpBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20083 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20087 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20105 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20114 "isa_tms320.tcc"
> *DecodeOpBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20120 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20124 "isa_tms320.tcc"
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
#line 20138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20148 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20157 "isa_tms320.tcc"
> *DecodeOpBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20163 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20167 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20185 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20194 "isa_tms320.tcc"
> *DecodeOpBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20200 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20204 "isa_tms320.tcc"
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
#line 20218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20228 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20237 "isa_tms320.tcc"
> *DecodeOpBR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20243 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20247 "isa_tms320.tcc"
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
#line 20261 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20271 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20280 "isa_tms320.tcc"
> *DecodeOpBRD(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20290 "isa_tms320.tcc"
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
> *DecodeOpCALL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20329 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20333 "isa_tms320.tcc"
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
#line 20347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20357 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20366 "isa_tms320.tcc"
> *DecodeOpCALLcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20372 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20376 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20387 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20394 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20403 "isa_tms320.tcc"
> *DecodeOpCALLcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20409 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20413 "isa_tms320.tcc"
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
#line 20427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20434 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20437 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20446 "isa_tms320.tcc"
> *DecodeOpDBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20452 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20456 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20474 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20483 "isa_tms320.tcc"
> *DecodeOpDBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20489 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20493 "isa_tms320.tcc"
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
#line 20507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20517 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20526 "isa_tms320.tcc"
> *DecodeOpDBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20532 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20536 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20554 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20563 "isa_tms320.tcc"
> *DecodeOpDBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20569 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20573 "isa_tms320.tcc"
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
#line 20587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20597 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20606 "isa_tms320.tcc"
> *DecodeOpIACK_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20612 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20616 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20634 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20643 "isa_tms320.tcc"
> *DecodeOpIACK_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20649 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20653 "isa_tms320.tcc"
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
#line 20667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20670 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20677 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20686 "isa_tms320.tcc"
> *DecodeOpIDLE(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20692 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20696 "isa_tms320.tcc"
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
#line 20710 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20717 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20720 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20725 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20729 "isa_tms320.tcc"
> *DecodeOpNOP_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20735 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20739 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20747 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20754 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20757 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20766 "isa_tms320.tcc"
> *DecodeOpNOP_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20772 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20776 "isa_tms320.tcc"
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
#line 20790 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20800 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20809 "isa_tms320.tcc"
> *DecodeOpRETIcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20815 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20819 "isa_tms320.tcc"
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
#line 20833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20843 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20852 "isa_tms320.tcc"
> *DecodeOpRETScond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20858 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20862 "isa_tms320.tcc"
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
#line 20876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20886 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20895 "isa_tms320.tcc"
> *DecodeOpRPTB(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20901 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20905 "isa_tms320.tcc"
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
#line 20919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20929 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20938 "isa_tms320.tcc"
> *DecodeOpRPTS_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20948 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20966 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20975 "isa_tms320.tcc"
> *DecodeOpRPTS_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20981 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20985 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21003 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21012 "isa_tms320.tcc"
> *DecodeOpRPTS_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21018 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21022 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21040 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21049 "isa_tms320.tcc"
> *DecodeOpRPTS_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21059 "isa_tms320.tcc"
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
#line 21073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21083 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21092 "isa_tms320.tcc"
> *DecodeOpSWI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21102 "isa_tms320.tcc"
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
#line 21116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21126 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21135 "isa_tms320.tcc"
> *DecodeOpTRAPcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21141 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21145 "isa_tms320.tcc"
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
#line 21198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21205 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21208 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21217 "isa_tms320.tcc"
> *DecodeOpIDLE2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21223 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21227 "isa_tms320.tcc"
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
#line 21241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21251 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21260 "isa_tms320.tcc"
> *DecodeOpLOPOWER(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21266 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21270 "isa_tms320.tcc"
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
#line 21284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21294 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21303 "isa_tms320.tcc"
> *DecodeOpMAXSPEED(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21309 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21313 "isa_tms320.tcc"
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
#line 21368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21378 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21387 "isa_tms320.tcc"
> *DecodeOpLDFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21393 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21397 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21408 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21415 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21424 "isa_tms320.tcc"
> *DecodeOpLDFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21430 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21434 "isa_tms320.tcc"
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
#line 21448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21458 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21467 "isa_tms320.tcc"
> *DecodeOpLDII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21473 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21477 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21485 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21495 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21504 "isa_tms320.tcc"
> *DecodeOpLDII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21510 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21514 "isa_tms320.tcc"
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
#line 21528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21538 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21547 "isa_tms320.tcc"
> *DecodeOpSIGI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21557 "isa_tms320.tcc"
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
#line 21571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21581 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21590 "isa_tms320.tcc"
> *DecodeOpSTFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21600 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21618 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21627 "isa_tms320.tcc"
> *DecodeOpSTFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21637 "isa_tms320.tcc"
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
#line 21651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21661 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21670 "isa_tms320.tcc"
> *DecodeOpSTII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21676 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21680 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21698 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21707 "isa_tms320.tcc"
> *DecodeOpSTII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21713 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21717 "isa_tms320.tcc"
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
#line 21788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21798 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21807 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21813 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21817 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21835 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21844 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21850 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21854 "isa_tms320.tcc"
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
#line 21868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21878 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21883 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21887 "isa_tms320.tcc"
> *DecodeOpLDF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21893 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21897 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21915 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21924 "isa_tms320.tcc"
> *DecodeOpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21930 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21934 "isa_tms320.tcc"
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
#line 21948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21958 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21967 "isa_tms320.tcc"
> *DecodeOpLDI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21973 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21977 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21995 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22004 "isa_tms320.tcc"
> *DecodeOpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22010 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22014 "isa_tms320.tcc"
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
#line 22028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22038 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22047 "isa_tms320.tcc"
> *DecodeOpLSH3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22053 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22057 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22072 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22075 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22084 "isa_tms320.tcc"
> *DecodeOpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22090 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22094 "isa_tms320.tcc"
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
#line 22108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22118 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22127 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22133 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22137 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22155 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22164 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22170 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22174 "isa_tms320.tcc"
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
#line 22188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22198 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22207 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22213 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22217 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22235 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22244 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22250 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22254 "isa_tms320.tcc"
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
#line 22268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22278 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22287 "isa_tms320.tcc"
> *DecodeOpNEGF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22293 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22297 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22315 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22324 "isa_tms320.tcc"
> *DecodeOpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22330 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22334 "isa_tms320.tcc"
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
#line 22348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22358 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22367 "isa_tms320.tcc"
> *DecodeOpNEGI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22373 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22377 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22392 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22395 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22404 "isa_tms320.tcc"
> *DecodeOpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22410 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22414 "isa_tms320.tcc"
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
#line 22428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22438 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22447 "isa_tms320.tcc"
> *DecodeOpNOT_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22453 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22457 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22472 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22475 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22484 "isa_tms320.tcc"
> *DecodeOpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22494 "isa_tms320.tcc"
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
#line 22508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22518 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22527 "isa_tms320.tcc"
> *DecodeOpOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22533 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22537 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22555 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22560 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22564 "isa_tms320.tcc"
> *DecodeOpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22570 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22574 "isa_tms320.tcc"
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
#line 22588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22598 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22607 "isa_tms320.tcc"
> *DecodeOpSTF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22613 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22617 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22635 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22644 "isa_tms320.tcc"
> *DecodeOpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22650 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22654 "isa_tms320.tcc"
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
#line 22668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22675 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22678 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22687 "isa_tms320.tcc"
> *DecodeOpSTI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22693 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22697 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22715 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22720 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22724 "isa_tms320.tcc"
> *DecodeOpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22730 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22734 "isa_tms320.tcc"
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
#line 22748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22758 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22763 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22767 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22773 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22777 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22792 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22795 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22804 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22810 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22814 "isa_tms320.tcc"
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
#line 22828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22838 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22847 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22853 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22857 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22875 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22884 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22890 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22894 "isa_tms320.tcc"
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
#line 22908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22918 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22927 "isa_tms320.tcc"
> *DecodeOpXOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22933 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22937 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22945 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22952 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22955 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22960 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22964 "isa_tms320.tcc"
> *DecodeOpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22970 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22974 "isa_tms320.tcc"
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
#line 22988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22998 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23007 "isa_tms320.tcc"
> *DecodeOpLDF_LDF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23013 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23017 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23035 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23044 "isa_tms320.tcc"
> *DecodeOpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23050 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23054 "isa_tms320.tcc"
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
#line 23068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23078 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23087 "isa_tms320.tcc"
> *DecodeOpLDI_LDI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23093 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23097 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23115 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23124 "isa_tms320.tcc"
> *DecodeOpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23130 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23134 "isa_tms320.tcc"
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
#line 23148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23158 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23167 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23173 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23177 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23195 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23204 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23210 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23214 "isa_tms320.tcc"
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
#line 23228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23238 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23247 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23253 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23257 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23275 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23284 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23290 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23294 "isa_tms320.tcc"
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
#line 23308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23318 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23323 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23327 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23333 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23337 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23348 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23352 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23355 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23364 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23370 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23374 "isa_tms320.tcc"
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
#line 23388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23398 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23407 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23417 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23432 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23435 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23440 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23444 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23450 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23454 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23471 "isa_tms320.tcc"
>
OpLDE_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23480 "isa_tms320.tcc"
>::OpLDE_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23488 "isa_tms320.tcc"
>(code, addr, "LDE_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23508 "isa_tms320.tcc"
>
OpLDE_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23513 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23517 "isa_tms320.tcc"
>::OpLDE_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23521 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23525 "isa_tms320.tcc"
>(code, addr, "LDE_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23545 "isa_tms320.tcc"
>
OpLDE_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23554 "isa_tms320.tcc"
>::OpLDE_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23562 "isa_tms320.tcc"
>(code, addr, "LDE_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23584 "isa_tms320.tcc"
>
OpLDE_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23593 "isa_tms320.tcc"
>::OpLDE_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23601 "isa_tms320.tcc"
>(code, addr, "LDE_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23621 "isa_tms320.tcc"
>
OpLDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23630 "isa_tms320.tcc"
>::OpLDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23638 "isa_tms320.tcc"
>(code, addr, "LDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23658 "isa_tms320.tcc"
>
OpLDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23667 "isa_tms320.tcc"
>::OpLDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23675 "isa_tms320.tcc"
>(code, addr, "LDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23695 "isa_tms320.tcc"
>
OpLDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23704 "isa_tms320.tcc"
>::OpLDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23712 "isa_tms320.tcc"
>(code, addr, "LDF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23734 "isa_tms320.tcc"
>
OpLDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23743 "isa_tms320.tcc"
>::OpLDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23751 "isa_tms320.tcc"
>(code, addr, "LDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23771 "isa_tms320.tcc"
>
OpLDFcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23780 "isa_tms320.tcc"
>::OpLDFcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23788 "isa_tms320.tcc"
>(code, addr, "LDFcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23799 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23809 "isa_tms320.tcc"
>
OpLDFcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23818 "isa_tms320.tcc"
>::OpLDFcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23826 "isa_tms320.tcc"
>(code, addr, "LDFcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23844 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23847 "isa_tms320.tcc"
>
OpLDFcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23856 "isa_tms320.tcc"
>::OpLDFcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23864 "isa_tms320.tcc"
>(code, addr, "LDFcond_indir")
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
#line 23877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23887 "isa_tms320.tcc"
>
OpLDFcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23896 "isa_tms320.tcc"
>::OpLDFcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23904 "isa_tms320.tcc"
>(code, addr, "LDFcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23918 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23925 "isa_tms320.tcc"
>
OpLDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23930 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23934 "isa_tms320.tcc"
>::OpLDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23942 "isa_tms320.tcc"
>(code, addr, "LDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23952 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23962 "isa_tms320.tcc"
>
OpLDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23971 "isa_tms320.tcc"
>::OpLDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23979 "isa_tms320.tcc"
>(code, addr, "LDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23989 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23992 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23996 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23999 "isa_tms320.tcc"
>
OpLDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24008 "isa_tms320.tcc"
>::OpLDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24012 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24016 "isa_tms320.tcc"
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
#line 24028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24038 "isa_tms320.tcc"
>
OpLDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24047 "isa_tms320.tcc"
>::OpLDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24055 "isa_tms320.tcc"
>(code, addr, "LDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24072 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24075 "isa_tms320.tcc"
>
OpLDIcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24084 "isa_tms320.tcc"
>::OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24092 "isa_tms320.tcc"
>(code, addr, "LDIcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24113 "isa_tms320.tcc"
>
OpLDIcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24122 "isa_tms320.tcc"
>::OpLDIcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24130 "isa_tms320.tcc"
>(code, addr, "LDIcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24151 "isa_tms320.tcc"
>
OpLDIcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24160 "isa_tms320.tcc"
>::OpLDIcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24168 "isa_tms320.tcc"
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
#line 24181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24191 "isa_tms320.tcc"
>
OpLDIcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24200 "isa_tms320.tcc"
>::OpLDIcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24208 "isa_tms320.tcc"
>(code, addr, "LDIcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24229 "isa_tms320.tcc"
>
OpLDM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24238 "isa_tms320.tcc"
>::OpLDM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24246 "isa_tms320.tcc"
>(code, addr, "LDM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24263 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24266 "isa_tms320.tcc"
>
OpLDM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24275 "isa_tms320.tcc"
>::OpLDM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24283 "isa_tms320.tcc"
>(code, addr, "LDM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24303 "isa_tms320.tcc"
>
OpLDM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24312 "isa_tms320.tcc"
>::OpLDM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24320 "isa_tms320.tcc"
>(code, addr, "LDM_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24335 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24342 "isa_tms320.tcc"
>
OpLDM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24351 "isa_tms320.tcc"
>::OpLDM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24359 "isa_tms320.tcc"
>(code, addr, "LDM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = ((code >> 0) & 0xfff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24369 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24379 "isa_tms320.tcc"
>
OpLDP<
#line 45 "isa/tms320.isa"
CONFIG
#line 24384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24388 "isa_tms320.tcc"
>::OpLDP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24396 "isa_tms320.tcc"
>(code, addr, "LDP")
{
	src = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24408 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24415 "isa_tms320.tcc"
>
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 24420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24424 "isa_tms320.tcc"
>::OpPOP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24432 "isa_tms320.tcc"
>(code, addr, "POP")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24451 "isa_tms320.tcc"
>
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 24456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24460 "isa_tms320.tcc"
>::OpPOPF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24468 "isa_tms320.tcc"
>(code, addr, "POPF")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24487 "isa_tms320.tcc"
>
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 24492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24496 "isa_tms320.tcc"
>::OpPUSH(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24504 "isa_tms320.tcc"
>(code, addr, "PUSH")
{
	src = ((code >> 16) & 0x1f);
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
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 24528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24532 "isa_tms320.tcc"
>::OpPUSHF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24540 "isa_tms320.tcc"
>(code, addr, "PUSHF")
{
	src = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24559 "isa_tms320.tcc"
>
OpSTF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24568 "isa_tms320.tcc"
>::OpSTF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24576 "isa_tms320.tcc"
>(code, addr, "STF_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24593 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24596 "isa_tms320.tcc"
>
OpSTF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24605 "isa_tms320.tcc"
>::OpSTF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24613 "isa_tms320.tcc"
>(code, addr, "STF_indir")
{
	src = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24635 "isa_tms320.tcc"
>
OpSTI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24644 "isa_tms320.tcc"
>::OpSTI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24652 "isa_tms320.tcc"
>(code, addr, "STI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24662 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24672 "isa_tms320.tcc"
>
OpSTI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24681 "isa_tms320.tcc"
>::OpSTI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24685 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24689 "isa_tms320.tcc"
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
#line 24701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24711 "isa_tms320.tcc"
>
OpABSF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24720 "isa_tms320.tcc"
>::OpABSF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24728 "isa_tms320.tcc"
>(code, addr, "ABSF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24748 "isa_tms320.tcc"
>
OpABSF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24757 "isa_tms320.tcc"
>::OpABSF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24765 "isa_tms320.tcc"
>(code, addr, "ABSF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24785 "isa_tms320.tcc"
>
OpABSF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24794 "isa_tms320.tcc"
>::OpABSF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24802 "isa_tms320.tcc"
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
#line 24814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24824 "isa_tms320.tcc"
>
OpABSF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24833 "isa_tms320.tcc"
>::OpABSF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24841 "isa_tms320.tcc"
>(code, addr, "ABSF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24861 "isa_tms320.tcc"
>
OpABSI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24870 "isa_tms320.tcc"
>::OpABSI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24878 "isa_tms320.tcc"
>(code, addr, "ABSI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24898 "isa_tms320.tcc"
>
OpABSI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24907 "isa_tms320.tcc"
>::OpABSI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24915 "isa_tms320.tcc"
>(code, addr, "ABSI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24935 "isa_tms320.tcc"
>
OpABSI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24944 "isa_tms320.tcc"
>::OpABSI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24952 "isa_tms320.tcc"
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
#line 24964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24974 "isa_tms320.tcc"
>
OpABSI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24983 "isa_tms320.tcc"
>::OpABSI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24991 "isa_tms320.tcc"
>(code, addr, "ABSI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25011 "isa_tms320.tcc"
>
OpADDC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25020 "isa_tms320.tcc"
>::OpADDC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25028 "isa_tms320.tcc"
>(code, addr, "ADDC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25048 "isa_tms320.tcc"
>
OpADDC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25057 "isa_tms320.tcc"
>::OpADDC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25065 "isa_tms320.tcc"
>(code, addr, "ADDC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25085 "isa_tms320.tcc"
>
OpADDC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25094 "isa_tms320.tcc"
>::OpADDC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25102 "isa_tms320.tcc"
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
#line 25114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25124 "isa_tms320.tcc"
>
OpADDC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25133 "isa_tms320.tcc"
>::OpADDC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25141 "isa_tms320.tcc"
>(code, addr, "ADDC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25161 "isa_tms320.tcc"
>
OpADDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25170 "isa_tms320.tcc"
>::OpADDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25178 "isa_tms320.tcc"
>(code, addr, "ADDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25198 "isa_tms320.tcc"
>
OpADDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25207 "isa_tms320.tcc"
>::OpADDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25215 "isa_tms320.tcc"
>(code, addr, "ADDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25235 "isa_tms320.tcc"
>
OpADDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25244 "isa_tms320.tcc"
>::OpADDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25248 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25252 "isa_tms320.tcc"
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
#line 25264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25274 "isa_tms320.tcc"
>
OpADDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25283 "isa_tms320.tcc"
>::OpADDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25291 "isa_tms320.tcc"
>(code, addr, "ADDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25311 "isa_tms320.tcc"
>
OpADDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25320 "isa_tms320.tcc"
>::OpADDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25328 "isa_tms320.tcc"
>(code, addr, "ADDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25348 "isa_tms320.tcc"
>
OpADDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25357 "isa_tms320.tcc"
>::OpADDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25365 "isa_tms320.tcc"
>(code, addr, "ADDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25385 "isa_tms320.tcc"
>
OpADDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25394 "isa_tms320.tcc"
>::OpADDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25402 "isa_tms320.tcc"
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
#line 25414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25424 "isa_tms320.tcc"
>
OpADDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25433 "isa_tms320.tcc"
>::OpADDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25441 "isa_tms320.tcc"
>(code, addr, "ADDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25461 "isa_tms320.tcc"
>
OpAND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25470 "isa_tms320.tcc"
>::OpAND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25478 "isa_tms320.tcc"
>(code, addr, "AND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25498 "isa_tms320.tcc"
>
OpAND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25507 "isa_tms320.tcc"
>::OpAND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25515 "isa_tms320.tcc"
>(code, addr, "AND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
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
OpAND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25544 "isa_tms320.tcc"
>::OpAND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25552 "isa_tms320.tcc"
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
#line 25564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25574 "isa_tms320.tcc"
>
OpAND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25583 "isa_tms320.tcc"
>::OpAND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25591 "isa_tms320.tcc"
>(code, addr, "AND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25604 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25611 "isa_tms320.tcc"
>
OpANDN_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25620 "isa_tms320.tcc"
>::OpANDN_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25628 "isa_tms320.tcc"
>(code, addr, "ANDN_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25638 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25641 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25645 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25648 "isa_tms320.tcc"
>
OpANDN_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25657 "isa_tms320.tcc"
>::OpANDN_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25665 "isa_tms320.tcc"
>(code, addr, "ANDN_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25675 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25678 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25682 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25685 "isa_tms320.tcc"
>
OpANDN_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25694 "isa_tms320.tcc"
>::OpANDN_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25702 "isa_tms320.tcc"
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
#line 25714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25724 "isa_tms320.tcc"
>
OpANDN_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25733 "isa_tms320.tcc"
>::OpANDN_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25741 "isa_tms320.tcc"
>(code, addr, "ANDN_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
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
OpASH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25770 "isa_tms320.tcc"
>::OpASH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25778 "isa_tms320.tcc"
>(code, addr, "ASH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25798 "isa_tms320.tcc"
>
OpASH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25807 "isa_tms320.tcc"
>::OpASH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25815 "isa_tms320.tcc"
>(code, addr, "ASH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25835 "isa_tms320.tcc"
>
OpASH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25844 "isa_tms320.tcc"
>::OpASH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25852 "isa_tms320.tcc"
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
#line 25864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25871 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25874 "isa_tms320.tcc"
>
OpASH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25883 "isa_tms320.tcc"
>::OpASH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25891 "isa_tms320.tcc"
>(code, addr, "ASH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25901 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25911 "isa_tms320.tcc"
>
OpCMPF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25920 "isa_tms320.tcc"
>::OpCMPF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25928 "isa_tms320.tcc"
>(code, addr, "CMPF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25945 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25948 "isa_tms320.tcc"
>
OpCMPF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25957 "isa_tms320.tcc"
>::OpCMPF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25965 "isa_tms320.tcc"
>(code, addr, "CMPF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25982 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25985 "isa_tms320.tcc"
>
OpCMPF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25994 "isa_tms320.tcc"
>::OpCMPF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26002 "isa_tms320.tcc"
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
#line 26014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26024 "isa_tms320.tcc"
>
OpCMPF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26033 "isa_tms320.tcc"
>::OpCMPF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26041 "isa_tms320.tcc"
>(code, addr, "CMPF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26061 "isa_tms320.tcc"
>
OpCMPI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26070 "isa_tms320.tcc"
>::OpCMPI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26078 "isa_tms320.tcc"
>(code, addr, "CMPI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26098 "isa_tms320.tcc"
>
OpCMPI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26107 "isa_tms320.tcc"
>::OpCMPI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26115 "isa_tms320.tcc"
>(code, addr, "CMPI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
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
OpCMPI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26144 "isa_tms320.tcc"
>::OpCMPI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26152 "isa_tms320.tcc"
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
#line 26164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26167 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26174 "isa_tms320.tcc"
>
OpCMPI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26183 "isa_tms320.tcc"
>::OpCMPI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26191 "isa_tms320.tcc"
>(code, addr, "CMPI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26211 "isa_tms320.tcc"
>
OpFIX_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26220 "isa_tms320.tcc"
>::OpFIX_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26228 "isa_tms320.tcc"
>(code, addr, "FIX_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26241 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26245 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26248 "isa_tms320.tcc"
>
OpFIX_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26257 "isa_tms320.tcc"
>::OpFIX_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26265 "isa_tms320.tcc"
>(code, addr, "FIX_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26285 "isa_tms320.tcc"
>
OpFIX_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26294 "isa_tms320.tcc"
>::OpFIX_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26302 "isa_tms320.tcc"
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
#line 26314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26324 "isa_tms320.tcc"
>
OpFIX_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26333 "isa_tms320.tcc"
>::OpFIX_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26341 "isa_tms320.tcc"
>(code, addr, "FIX_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26361 "isa_tms320.tcc"
>
OpFLOAT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26370 "isa_tms320.tcc"
>::OpFLOAT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26378 "isa_tms320.tcc"
>(code, addr, "FLOAT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26398 "isa_tms320.tcc"
>
OpFLOAT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26407 "isa_tms320.tcc"
>::OpFLOAT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26415 "isa_tms320.tcc"
>(code, addr, "FLOAT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26432 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26435 "isa_tms320.tcc"
>
OpFLOAT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26440 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26444 "isa_tms320.tcc"
>::OpFLOAT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26452 "isa_tms320.tcc"
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
#line 26464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26474 "isa_tms320.tcc"
>
OpFLOAT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26483 "isa_tms320.tcc"
>::OpFLOAT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26491 "isa_tms320.tcc"
>(code, addr, "FLOAT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26511 "isa_tms320.tcc"
>
OpLSH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26520 "isa_tms320.tcc"
>::OpLSH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26528 "isa_tms320.tcc"
>(code, addr, "LSH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26548 "isa_tms320.tcc"
>
OpLSH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26557 "isa_tms320.tcc"
>::OpLSH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26565 "isa_tms320.tcc"
>(code, addr, "LSH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26578 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26585 "isa_tms320.tcc"
>
OpLSH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26594 "isa_tms320.tcc"
>::OpLSH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26598 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26602 "isa_tms320.tcc"
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
#line 26614 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26624 "isa_tms320.tcc"
>
OpLSH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26633 "isa_tms320.tcc"
>::OpLSH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26641 "isa_tms320.tcc"
>(code, addr, "LSH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26661 "isa_tms320.tcc"
>
OpMPYF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26670 "isa_tms320.tcc"
>::OpMPYF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26678 "isa_tms320.tcc"
>(code, addr, "MPYF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26698 "isa_tms320.tcc"
>
OpMPYF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26707 "isa_tms320.tcc"
>::OpMPYF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26715 "isa_tms320.tcc"
>(code, addr, "MPYF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26725 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26735 "isa_tms320.tcc"
>
OpMPYF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26744 "isa_tms320.tcc"
>::OpMPYF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26752 "isa_tms320.tcc"
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
#line 26764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26774 "isa_tms320.tcc"
>
OpMPYF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26783 "isa_tms320.tcc"
>::OpMPYF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26791 "isa_tms320.tcc"
>(code, addr, "MPYF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26801 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26811 "isa_tms320.tcc"
>
OpMPYI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26820 "isa_tms320.tcc"
>::OpMPYI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26828 "isa_tms320.tcc"
>(code, addr, "MPYI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26848 "isa_tms320.tcc"
>
OpMPYI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26857 "isa_tms320.tcc"
>::OpMPYI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26865 "isa_tms320.tcc"
>(code, addr, "MPYI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26882 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26885 "isa_tms320.tcc"
>
OpMPYI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26894 "isa_tms320.tcc"
>::OpMPYI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26898 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26902 "isa_tms320.tcc"
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
#line 26914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26924 "isa_tms320.tcc"
>
OpMPYI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26933 "isa_tms320.tcc"
>::OpMPYI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26941 "isa_tms320.tcc"
>(code, addr, "MPYI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26961 "isa_tms320.tcc"
>
OpNEGB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26970 "isa_tms320.tcc"
>::OpNEGB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26978 "isa_tms320.tcc"
>(code, addr, "NEGB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26998 "isa_tms320.tcc"
>
OpNEGB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27007 "isa_tms320.tcc"
>::OpNEGB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27015 "isa_tms320.tcc"
>(code, addr, "NEGB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27035 "isa_tms320.tcc"
>
OpNEGB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27044 "isa_tms320.tcc"
>::OpNEGB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27052 "isa_tms320.tcc"
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
#line 27064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27074 "isa_tms320.tcc"
>
OpNEGB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27083 "isa_tms320.tcc"
>::OpNEGB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27091 "isa_tms320.tcc"
>(code, addr, "NEGB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27111 "isa_tms320.tcc"
>
OpNEGF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27120 "isa_tms320.tcc"
>::OpNEGF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27128 "isa_tms320.tcc"
>(code, addr, "NEGF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27148 "isa_tms320.tcc"
>
OpNEGF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27157 "isa_tms320.tcc"
>::OpNEGF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27165 "isa_tms320.tcc"
>(code, addr, "NEGF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27185 "isa_tms320.tcc"
>
OpNEGF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27194 "isa_tms320.tcc"
>::OpNEGF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27202 "isa_tms320.tcc"
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
#line 27214 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27217 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27224 "isa_tms320.tcc"
>
OpNEGF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27233 "isa_tms320.tcc"
>::OpNEGF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27241 "isa_tms320.tcc"
>(code, addr, "NEGF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27261 "isa_tms320.tcc"
>
OpNEGI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27270 "isa_tms320.tcc"
>::OpNEGI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27278 "isa_tms320.tcc"
>(code, addr, "NEGI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27288 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27298 "isa_tms320.tcc"
>
OpNEGI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27307 "isa_tms320.tcc"
>::OpNEGI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27315 "isa_tms320.tcc"
>(code, addr, "NEGI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27335 "isa_tms320.tcc"
>
OpNEGI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27344 "isa_tms320.tcc"
>::OpNEGI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27348 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27352 "isa_tms320.tcc"
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
#line 27364 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27367 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27371 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27374 "isa_tms320.tcc"
>
OpNEGI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27383 "isa_tms320.tcc"
>::OpNEGI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27387 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27391 "isa_tms320.tcc"
>(code, addr, "NEGI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27411 "isa_tms320.tcc"
>
OpNORM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27420 "isa_tms320.tcc"
>::OpNORM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27428 "isa_tms320.tcc"
>(code, addr, "NORM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27448 "isa_tms320.tcc"
>
OpNORM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27453 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27457 "isa_tms320.tcc"
>::OpNORM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27465 "isa_tms320.tcc"
>(code, addr, "NORM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27475 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27482 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27485 "isa_tms320.tcc"
>
OpNORM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27494 "isa_tms320.tcc"
>::OpNORM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27502 "isa_tms320.tcc"
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
#line 27514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27524 "isa_tms320.tcc"
>
OpNORM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27533 "isa_tms320.tcc"
>::OpNORM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27541 "isa_tms320.tcc"
>(code, addr, "NORM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27561 "isa_tms320.tcc"
>
OpNOT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27570 "isa_tms320.tcc"
>::OpNOT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27578 "isa_tms320.tcc"
>(code, addr, "NOT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27598 "isa_tms320.tcc"
>
OpNOT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27607 "isa_tms320.tcc"
>::OpNOT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27615 "isa_tms320.tcc"
>(code, addr, "NOT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27635 "isa_tms320.tcc"
>
OpNOT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27644 "isa_tms320.tcc"
>::OpNOT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27652 "isa_tms320.tcc"
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
#line 27664 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27671 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27674 "isa_tms320.tcc"
>
OpNOT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27683 "isa_tms320.tcc"
>::OpNOT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27691 "isa_tms320.tcc"
>(code, addr, "NOT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27711 "isa_tms320.tcc"
>
OpOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27720 "isa_tms320.tcc"
>::OpOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27728 "isa_tms320.tcc"
>(code, addr, "OR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27748 "isa_tms320.tcc"
>
OpOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27757 "isa_tms320.tcc"
>::OpOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27765 "isa_tms320.tcc"
>(code, addr, "OR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27785 "isa_tms320.tcc"
>
OpOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27794 "isa_tms320.tcc"
>::OpOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27802 "isa_tms320.tcc"
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
#line 27814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27824 "isa_tms320.tcc"
>
OpOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27833 "isa_tms320.tcc"
>::OpOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27841 "isa_tms320.tcc"
>(code, addr, "OR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27861 "isa_tms320.tcc"
>
OpRND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27870 "isa_tms320.tcc"
>::OpRND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27878 "isa_tms320.tcc"
>(code, addr, "RND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27898 "isa_tms320.tcc"
>
OpRND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27907 "isa_tms320.tcc"
>::OpRND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27915 "isa_tms320.tcc"
>(code, addr, "RND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27935 "isa_tms320.tcc"
>
OpRND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27944 "isa_tms320.tcc"
>::OpRND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27952 "isa_tms320.tcc"
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
#line 27964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27974 "isa_tms320.tcc"
>
OpRND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27983 "isa_tms320.tcc"
>::OpRND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27991 "isa_tms320.tcc"
>(code, addr, "RND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28011 "isa_tms320.tcc"
>
OpROL<
#line 45 "isa/tms320.isa"
CONFIG
#line 28016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28020 "isa_tms320.tcc"
>::OpROL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28028 "isa_tms320.tcc"
>(code, addr, "ROL")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28044 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28047 "isa_tms320.tcc"
>
OpROLC<
#line 45 "isa/tms320.isa"
CONFIG
#line 28052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28056 "isa_tms320.tcc"
>::OpROLC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28064 "isa_tms320.tcc"
>(code, addr, "ROLC")
{
	dst = ((code >> 16) & 0x1f);
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
OpROR<
#line 45 "isa/tms320.isa"
CONFIG
#line 28088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28092 "isa_tms320.tcc"
>::OpROR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28100 "isa_tms320.tcc"
>(code, addr, "ROR")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28119 "isa_tms320.tcc"
>
OpRORC<
#line 45 "isa/tms320.isa"
CONFIG
#line 28124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28128 "isa_tms320.tcc"
>::OpRORC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28136 "isa_tms320.tcc"
>(code, addr, "RORC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28155 "isa_tms320.tcc"
>
OpSUBB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28164 "isa_tms320.tcc"
>::OpSUBB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28172 "isa_tms320.tcc"
>(code, addr, "SUBB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28192 "isa_tms320.tcc"
>
OpSUBB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28201 "isa_tms320.tcc"
>::OpSUBB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28209 "isa_tms320.tcc"
>(code, addr, "SUBB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28229 "isa_tms320.tcc"
>
OpSUBB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28238 "isa_tms320.tcc"
>::OpSUBB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28246 "isa_tms320.tcc"
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
#line 28258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28268 "isa_tms320.tcc"
>
OpSUBB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28277 "isa_tms320.tcc"
>::OpSUBB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28285 "isa_tms320.tcc"
>(code, addr, "SUBB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28305 "isa_tms320.tcc"
>
OpSUBC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28314 "isa_tms320.tcc"
>::OpSUBC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28322 "isa_tms320.tcc"
>(code, addr, "SUBC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28335 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28342 "isa_tms320.tcc"
>
OpSUBC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28351 "isa_tms320.tcc"
>::OpSUBC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28359 "isa_tms320.tcc"
>(code, addr, "SUBC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28369 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28379 "isa_tms320.tcc"
>
OpSUBC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28388 "isa_tms320.tcc"
>::OpSUBC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28396 "isa_tms320.tcc"
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
#line 28408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28418 "isa_tms320.tcc"
>
OpSUBC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28427 "isa_tms320.tcc"
>::OpSUBC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28435 "isa_tms320.tcc"
>(code, addr, "SUBC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28452 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28455 "isa_tms320.tcc"
>
OpSUBF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28464 "isa_tms320.tcc"
>::OpSUBF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28472 "isa_tms320.tcc"
>(code, addr, "SUBF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28482 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28489 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28492 "isa_tms320.tcc"
>
OpSUBF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28501 "isa_tms320.tcc"
>::OpSUBF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28505 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28509 "isa_tms320.tcc"
>(code, addr, "SUBF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28519 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28526 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28529 "isa_tms320.tcc"
>
OpSUBF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28538 "isa_tms320.tcc"
>::OpSUBF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28542 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28546 "isa_tms320.tcc"
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
#line 28558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28568 "isa_tms320.tcc"
>
OpSUBF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28577 "isa_tms320.tcc"
>::OpSUBF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28585 "isa_tms320.tcc"
>(code, addr, "SUBF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28598 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28602 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28605 "isa_tms320.tcc"
>
OpSUBI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28610 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28614 "isa_tms320.tcc"
>::OpSUBI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28618 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28622 "isa_tms320.tcc"
>(code, addr, "SUBI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28639 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28642 "isa_tms320.tcc"
>
OpSUBI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28647 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28651 "isa_tms320.tcc"
>::OpSUBI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28659 "isa_tms320.tcc"
>(code, addr, "SUBI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28679 "isa_tms320.tcc"
>
OpSUBI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28688 "isa_tms320.tcc"
>::OpSUBI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28696 "isa_tms320.tcc"
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
#line 28708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28718 "isa_tms320.tcc"
>
OpSUBI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28727 "isa_tms320.tcc"
>::OpSUBI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28735 "isa_tms320.tcc"
>(code, addr, "SUBI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28752 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28755 "isa_tms320.tcc"
>
OpSUBRB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28764 "isa_tms320.tcc"
>::OpSUBRB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28768 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28772 "isa_tms320.tcc"
>(code, addr, "SUBRB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28789 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28792 "isa_tms320.tcc"
>
OpSUBRB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28797 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28801 "isa_tms320.tcc"
>::OpSUBRB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28809 "isa_tms320.tcc"
>(code, addr, "SUBRB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28829 "isa_tms320.tcc"
>
OpSUBRB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28838 "isa_tms320.tcc"
>::OpSUBRB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28846 "isa_tms320.tcc"
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
#line 28858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28868 "isa_tms320.tcc"
>
OpSUBRB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28877 "isa_tms320.tcc"
>::OpSUBRB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28885 "isa_tms320.tcc"
>(code, addr, "SUBRB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28898 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28902 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28905 "isa_tms320.tcc"
>
OpSUBRF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28910 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28914 "isa_tms320.tcc"
>::OpSUBRF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28918 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28922 "isa_tms320.tcc"
>(code, addr, "SUBRF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28942 "isa_tms320.tcc"
>
OpSUBRF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28947 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28951 "isa_tms320.tcc"
>::OpSUBRF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28959 "isa_tms320.tcc"
>(code, addr, "SUBRF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28972 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28976 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28979 "isa_tms320.tcc"
>
OpSUBRF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28984 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28988 "isa_tms320.tcc"
>::OpSUBRF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28992 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28996 "isa_tms320.tcc"
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
#line 29008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29018 "isa_tms320.tcc"
>
OpSUBRF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29027 "isa_tms320.tcc"
>::OpSUBRF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29035 "isa_tms320.tcc"
>(code, addr, "SUBRF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29055 "isa_tms320.tcc"
>
OpSUBRI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29064 "isa_tms320.tcc"
>::OpSUBRI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29072 "isa_tms320.tcc"
>(code, addr, "SUBRI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29092 "isa_tms320.tcc"
>
OpSUBRI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29101 "isa_tms320.tcc"
>::OpSUBRI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29105 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29109 "isa_tms320.tcc"
>(code, addr, "SUBRI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29129 "isa_tms320.tcc"
>
OpSUBRI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29138 "isa_tms320.tcc"
>::OpSUBRI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29146 "isa_tms320.tcc"
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
#line 29158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29168 "isa_tms320.tcc"
>
OpSUBRI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29177 "isa_tms320.tcc"
>::OpSUBRI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29185 "isa_tms320.tcc"
>(code, addr, "SUBRI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29202 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29205 "isa_tms320.tcc"
>
OpTSTB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29210 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29214 "isa_tms320.tcc"
>::OpTSTB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29218 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29222 "isa_tms320.tcc"
>(code, addr, "TSTB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29242 "isa_tms320.tcc"
>
OpTSTB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29251 "isa_tms320.tcc"
>::OpTSTB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29259 "isa_tms320.tcc"
>(code, addr, "TSTB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29272 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29279 "isa_tms320.tcc"
>
OpTSTB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29284 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29288 "isa_tms320.tcc"
>::OpTSTB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29296 "isa_tms320.tcc"
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
#line 29308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29318 "isa_tms320.tcc"
>
OpTSTB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29323 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29327 "isa_tms320.tcc"
>::OpTSTB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29335 "isa_tms320.tcc"
>(code, addr, "TSTB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29348 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29352 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29355 "isa_tms320.tcc"
>
OpXOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29364 "isa_tms320.tcc"
>::OpXOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29372 "isa_tms320.tcc"
>(code, addr, "XOR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29392 "isa_tms320.tcc"
>
OpXOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29401 "isa_tms320.tcc"
>::OpXOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29409 "isa_tms320.tcc"
>(code, addr, "XOR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29422 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29429 "isa_tms320.tcc"
>
OpXOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29438 "isa_tms320.tcc"
>::OpXOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29446 "isa_tms320.tcc"
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
#line 29458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29468 "isa_tms320.tcc"
>
OpXOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29473 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29477 "isa_tms320.tcc"
>::OpXOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29485 "isa_tms320.tcc"
>(code, addr, "XOR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29502 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29505 "isa_tms320.tcc"
>
OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29514 "isa_tms320.tcc"
>::OpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29522 "isa_tms320.tcc"
>(code, addr, "ADDC3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29543 "isa_tms320.tcc"
>
OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29552 "isa_tms320.tcc"
>::OpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29560 "isa_tms320.tcc"
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
#line 29572 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29575 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29582 "isa_tms320.tcc"
>
OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29591 "isa_tms320.tcc"
>::OpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29599 "isa_tms320.tcc"
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
#line 29611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29621 "isa_tms320.tcc"
>
OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29630 "isa_tms320.tcc"
>::OpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29638 "isa_tms320.tcc"
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
#line 29651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29661 "isa_tms320.tcc"
>
OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29670 "isa_tms320.tcc"
>::OpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29678 "isa_tms320.tcc"
>(code, addr, "ADDF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29689 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29699 "isa_tms320.tcc"
>
OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29708 "isa_tms320.tcc"
>::OpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29716 "isa_tms320.tcc"
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
#line 29728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29738 "isa_tms320.tcc"
>
OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29747 "isa_tms320.tcc"
>::OpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29755 "isa_tms320.tcc"
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
#line 29767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29777 "isa_tms320.tcc"
>
OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29786 "isa_tms320.tcc"
>::OpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29794 "isa_tms320.tcc"
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
#line 29807 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29817 "isa_tms320.tcc"
>
OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29826 "isa_tms320.tcc"
>::OpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29830 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29834 "isa_tms320.tcc"
>(code, addr, "ADDI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29855 "isa_tms320.tcc"
>
OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29864 "isa_tms320.tcc"
>::OpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29872 "isa_tms320.tcc"
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
#line 29884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29894 "isa_tms320.tcc"
>
OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29903 "isa_tms320.tcc"
>::OpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29911 "isa_tms320.tcc"
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
#line 29923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29933 "isa_tms320.tcc"
>
OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29942 "isa_tms320.tcc"
>::OpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29950 "isa_tms320.tcc"
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
#line 29963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29970 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29973 "isa_tms320.tcc"
>
OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29982 "isa_tms320.tcc"
>::OpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29990 "isa_tms320.tcc"
>(code, addr, "AND3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30011 "isa_tms320.tcc"
>
OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30020 "isa_tms320.tcc"
>::OpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30028 "isa_tms320.tcc"
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
#line 30040 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30047 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30050 "isa_tms320.tcc"
>
OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30059 "isa_tms320.tcc"
>::OpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30063 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30067 "isa_tms320.tcc"
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
#line 30079 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30089 "isa_tms320.tcc"
>
OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30098 "isa_tms320.tcc"
>::OpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30102 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30106 "isa_tms320.tcc"
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
#line 30119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30129 "isa_tms320.tcc"
>
OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30138 "isa_tms320.tcc"
>::OpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30146 "isa_tms320.tcc"
>(code, addr, "ANDN3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30167 "isa_tms320.tcc"
>
OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30176 "isa_tms320.tcc"
>::OpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30184 "isa_tms320.tcc"
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
#line 30196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30203 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30206 "isa_tms320.tcc"
>
OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30215 "isa_tms320.tcc"
>::OpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30223 "isa_tms320.tcc"
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
#line 30235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30242 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30245 "isa_tms320.tcc"
>
OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30254 "isa_tms320.tcc"
>::OpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30262 "isa_tms320.tcc"
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
#line 30275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30285 "isa_tms320.tcc"
>
OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30294 "isa_tms320.tcc"
>::OpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30302 "isa_tms320.tcc"
>(code, addr, "ASH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30320 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30323 "isa_tms320.tcc"
>
OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30332 "isa_tms320.tcc"
>::OpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30340 "isa_tms320.tcc"
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
#line 30352 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30362 "isa_tms320.tcc"
>
OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30367 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30371 "isa_tms320.tcc"
>::OpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30375 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30379 "isa_tms320.tcc"
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
#line 30391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30401 "isa_tms320.tcc"
>
OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30410 "isa_tms320.tcc"
>::OpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30418 "isa_tms320.tcc"
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
#line 30431 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30441 "isa_tms320.tcc"
>
OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30450 "isa_tms320.tcc"
>::OpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30458 "isa_tms320.tcc"
>(code, addr, "CMPF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30469 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30472 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30476 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30479 "isa_tms320.tcc"
>
OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30488 "isa_tms320.tcc"
>::OpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30496 "isa_tms320.tcc"
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
#line 30508 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30518 "isa_tms320.tcc"
>
OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30527 "isa_tms320.tcc"
>::OpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30535 "isa_tms320.tcc"
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
#line 30547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30557 "isa_tms320.tcc"
>
OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30566 "isa_tms320.tcc"
>::OpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30574 "isa_tms320.tcc"
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
#line 30587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30597 "isa_tms320.tcc"
>
OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30606 "isa_tms320.tcc"
>::OpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30610 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30614 "isa_tms320.tcc"
>(code, addr, "CMPI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30635 "isa_tms320.tcc"
>
OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30644 "isa_tms320.tcc"
>::OpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30652 "isa_tms320.tcc"
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
#line 30664 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30671 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30674 "isa_tms320.tcc"
>
OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30683 "isa_tms320.tcc"
>::OpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30691 "isa_tms320.tcc"
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
#line 30703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30710 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30713 "isa_tms320.tcc"
>
OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30722 "isa_tms320.tcc"
>::OpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30730 "isa_tms320.tcc"
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
#line 30743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30753 "isa_tms320.tcc"
>
OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30762 "isa_tms320.tcc"
>::OpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30770 "isa_tms320.tcc"
>(code, addr, "LSH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30781 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30791 "isa_tms320.tcc"
>
OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30800 "isa_tms320.tcc"
>::OpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30808 "isa_tms320.tcc"
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
#line 30820 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30827 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30830 "isa_tms320.tcc"
>
OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30839 "isa_tms320.tcc"
>::OpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30847 "isa_tms320.tcc"
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
#line 30859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30862 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30866 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30869 "isa_tms320.tcc"
>
OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30878 "isa_tms320.tcc"
>::OpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30886 "isa_tms320.tcc"
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
#line 30899 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30902 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30909 "isa_tms320.tcc"
>
OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30918 "isa_tms320.tcc"
>::OpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30926 "isa_tms320.tcc"
>(code, addr, "MPYF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30937 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30944 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30947 "isa_tms320.tcc"
>
OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30952 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30956 "isa_tms320.tcc"
>::OpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30960 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30964 "isa_tms320.tcc"
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
#line 30976 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30986 "isa_tms320.tcc"
>
OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30995 "isa_tms320.tcc"
>::OpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31003 "isa_tms320.tcc"
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
#line 31015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31025 "isa_tms320.tcc"
>
OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31034 "isa_tms320.tcc"
>::OpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31042 "isa_tms320.tcc"
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
#line 31055 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31062 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31065 "isa_tms320.tcc"
>
OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31074 "isa_tms320.tcc"
>::OpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31082 "isa_tms320.tcc"
>(code, addr, "MPYI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31100 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31103 "isa_tms320.tcc"
>
OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31112 "isa_tms320.tcc"
>::OpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31120 "isa_tms320.tcc"
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
#line 31132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31142 "isa_tms320.tcc"
>
OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31151 "isa_tms320.tcc"
>::OpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31155 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31159 "isa_tms320.tcc"
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
#line 31171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31181 "isa_tms320.tcc"
>
OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31190 "isa_tms320.tcc"
>::OpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31198 "isa_tms320.tcc"
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
#line 31211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31221 "isa_tms320.tcc"
>
OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31230 "isa_tms320.tcc"
>::OpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31238 "isa_tms320.tcc"
>(code, addr, "OR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31249 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31259 "isa_tms320.tcc"
>
OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31268 "isa_tms320.tcc"
>::OpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31272 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31276 "isa_tms320.tcc"
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
#line 31288 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31298 "isa_tms320.tcc"
>
OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31307 "isa_tms320.tcc"
>::OpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31315 "isa_tms320.tcc"
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
#line 31327 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31337 "isa_tms320.tcc"
>
OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31346 "isa_tms320.tcc"
>::OpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31354 "isa_tms320.tcc"
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
#line 31367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31377 "isa_tms320.tcc"
>
OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31386 "isa_tms320.tcc"
>::OpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31394 "isa_tms320.tcc"
>(code, addr, "SUBB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31408 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31415 "isa_tms320.tcc"
>
OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31424 "isa_tms320.tcc"
>::OpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31428 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31432 "isa_tms320.tcc"
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
#line 31444 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31447 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31454 "isa_tms320.tcc"
>
OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31463 "isa_tms320.tcc"
>::OpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31471 "isa_tms320.tcc"
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
#line 31483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31490 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31493 "isa_tms320.tcc"
>
OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31502 "isa_tms320.tcc"
>::OpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31510 "isa_tms320.tcc"
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
#line 31523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31526 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31530 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31533 "isa_tms320.tcc"
>
OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31542 "isa_tms320.tcc"
>::OpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31550 "isa_tms320.tcc"
>(code, addr, "SUBF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31561 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31571 "isa_tms320.tcc"
>
OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31580 "isa_tms320.tcc"
>::OpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31588 "isa_tms320.tcc"
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
#line 31600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31610 "isa_tms320.tcc"
>
OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31619 "isa_tms320.tcc"
>::OpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31627 "isa_tms320.tcc"
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
#line 31639 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31649 "isa_tms320.tcc"
>
OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31658 "isa_tms320.tcc"
>::OpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31666 "isa_tms320.tcc"
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
#line 31679 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31689 "isa_tms320.tcc"
>
OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31698 "isa_tms320.tcc"
>::OpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31706 "isa_tms320.tcc"
>(code, addr, "SUBI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31717 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31720 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31727 "isa_tms320.tcc"
>
OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31732 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31736 "isa_tms320.tcc"
>::OpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31744 "isa_tms320.tcc"
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
#line 31756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31766 "isa_tms320.tcc"
>
OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31771 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31775 "isa_tms320.tcc"
>::OpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31783 "isa_tms320.tcc"
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
#line 31795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31805 "isa_tms320.tcc"
>
OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31814 "isa_tms320.tcc"
>::OpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31822 "isa_tms320.tcc"
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
#line 31835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31845 "isa_tms320.tcc"
>
OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31850 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31854 "isa_tms320.tcc"
>::OpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31862 "isa_tms320.tcc"
>(code, addr, "TSTB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31883 "isa_tms320.tcc"
>
OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31892 "isa_tms320.tcc"
>::OpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31896 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31900 "isa_tms320.tcc"
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
#line 31912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31915 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31922 "isa_tms320.tcc"
>
OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31931 "isa_tms320.tcc"
>::OpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31939 "isa_tms320.tcc"
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
#line 31951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31961 "isa_tms320.tcc"
>
OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31970 "isa_tms320.tcc"
>::OpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31978 "isa_tms320.tcc"
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
#line 31991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32001 "isa_tms320.tcc"
>
OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32010 "isa_tms320.tcc"
>::OpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32018 "isa_tms320.tcc"
>(code, addr, "XOR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32029 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32039 "isa_tms320.tcc"
>
OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32048 "isa_tms320.tcc"
>::OpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32056 "isa_tms320.tcc"
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
#line 32068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32078 "isa_tms320.tcc"
>
OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32087 "isa_tms320.tcc"
>::OpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32095 "isa_tms320.tcc"
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
#line 32107 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32117 "isa_tms320.tcc"
>
OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32126 "isa_tms320.tcc"
>::OpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32134 "isa_tms320.tcc"
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
#line 32147 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32150 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32157 "isa_tms320.tcc"
>
OpBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32162 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32166 "isa_tms320.tcc"
>::OpBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32174 "isa_tms320.tcc"
>(code, addr, "Bcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32184 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32194 "isa_tms320.tcc"
>
OpBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32203 "isa_tms320.tcc"
>::OpBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32211 "isa_tms320.tcc"
>(code, addr, "Bcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32231 "isa_tms320.tcc"
>
OpBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32240 "isa_tms320.tcc"
>::OpBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32248 "isa_tms320.tcc"
>(code, addr, "BcondD_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32268 "isa_tms320.tcc"
>
OpBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32277 "isa_tms320.tcc"
>::OpBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32285 "isa_tms320.tcc"
>(code, addr, "BcondD_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32305 "isa_tms320.tcc"
>
OpBR<
#line 45 "isa/tms320.isa"
CONFIG
#line 32310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32314 "isa_tms320.tcc"
>::OpBR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32322 "isa_tms320.tcc"
>(code, addr, "BR")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32331 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32341 "isa_tms320.tcc"
>
OpBRD<
#line 45 "isa/tms320.isa"
CONFIG
#line 32346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32350 "isa_tms320.tcc"
>::OpBRD(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32358 "isa_tms320.tcc"
>(code, addr, "BRD")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32377 "isa_tms320.tcc"
>
OpCALL<
#line 45 "isa/tms320.isa"
CONFIG
#line 32382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32386 "isa_tms320.tcc"
>::OpCALL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32394 "isa_tms320.tcc"
>(code, addr, "CALL")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32410 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32413 "isa_tms320.tcc"
>
OpCALLcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32422 "isa_tms320.tcc"
>::OpCALLcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32430 "isa_tms320.tcc"
>(code, addr, "CALLcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32450 "isa_tms320.tcc"
>
OpCALLcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32459 "isa_tms320.tcc"
>::OpCALLcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32467 "isa_tms320.tcc"
>(code, addr, "CALLcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32487 "isa_tms320.tcc"
>
OpDBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32496 "isa_tms320.tcc"
>::OpDBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32504 "isa_tms320.tcc"
>(code, addr, "DBcond_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32522 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32525 "isa_tms320.tcc"
>
OpDBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32534 "isa_tms320.tcc"
>::OpDBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32542 "isa_tms320.tcc"
>(code, addr, "DBcond_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32560 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32563 "isa_tms320.tcc"
>
OpDBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32572 "isa_tms320.tcc"
>::OpDBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32580 "isa_tms320.tcc"
>(code, addr, "DBcondD_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32591 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32601 "isa_tms320.tcc"
>
OpDBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32610 "isa_tms320.tcc"
>::OpDBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32618 "isa_tms320.tcc"
>(code, addr, "DBcondD_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32636 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32639 "isa_tms320.tcc"
>
OpIACK_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32648 "isa_tms320.tcc"
>::OpIACK_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32656 "isa_tms320.tcc"
>(code, addr, "IACK_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32668 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32672 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32675 "isa_tms320.tcc"
>
OpIACK_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32684 "isa_tms320.tcc"
>::OpIACK_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32692 "isa_tms320.tcc"
>(code, addr, "IACK_indir")
{
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
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
OpIDLE<
#line 45 "isa/tms320.isa"
CONFIG
#line 32718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32722 "isa_tms320.tcc"
>::OpIDLE(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32730 "isa_tms320.tcc"
>(code, addr, "IDLE")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32748 "isa_tms320.tcc"
>
OpNOP_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32757 "isa_tms320.tcc"
>::OpNOP_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32765 "isa_tms320.tcc"
>(code, addr, "NOP_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32781 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32784 "isa_tms320.tcc"
>
OpNOP_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32793 "isa_tms320.tcc"
>::OpNOP_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32797 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32801 "isa_tms320.tcc"
>(code, addr, "NOP_indir")
{
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32822 "isa_tms320.tcc"
>
OpRETIcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32831 "isa_tms320.tcc"
>::OpRETIcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32839 "isa_tms320.tcc"
>(code, addr, "RETIcond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32855 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32858 "isa_tms320.tcc"
>
OpRETScond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32863 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32867 "isa_tms320.tcc"
>::OpRETScond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32875 "isa_tms320.tcc"
>(code, addr, "RETScond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32894 "isa_tms320.tcc"
>
OpRPTB<
#line 45 "isa/tms320.isa"
CONFIG
#line 32899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32903 "isa_tms320.tcc"
>::OpRPTB(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32911 "isa_tms320.tcc"
>(code, addr, "RPTB")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32920 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32930 "isa_tms320.tcc"
>
OpRPTS_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32939 "isa_tms320.tcc"
>::OpRPTS_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32947 "isa_tms320.tcc"
>(code, addr, "RPTS_reg")
{
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32966 "isa_tms320.tcc"
>
OpRPTS_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32975 "isa_tms320.tcc"
>::OpRPTS_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32983 "isa_tms320.tcc"
>(code, addr, "RPTS_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32999 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33002 "isa_tms320.tcc"
>
OpRPTS_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33011 "isa_tms320.tcc"
>::OpRPTS_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33019 "isa_tms320.tcc"
>(code, addr, "RPTS_indir")
{
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33040 "isa_tms320.tcc"
>
OpRPTS_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 33045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33049 "isa_tms320.tcc"
>::OpRPTS_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33057 "isa_tms320.tcc"
>(code, addr, "RPTS_imm")
{
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33066 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33069 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33076 "isa_tms320.tcc"
>
OpSWI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33085 "isa_tms320.tcc"
>::OpSWI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33093 "isa_tms320.tcc"
>(code, addr, "SWI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33108 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33111 "isa_tms320.tcc"
>
OpTRAPcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 33116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33120 "isa_tms320.tcc"
>::OpTRAPcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33128 "isa_tms320.tcc"
>(code, addr, "TRAPcond")
{
	cond = ((code >> 16) & 0x1f);
	n = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33148 "isa_tms320.tcc"
>
OpIDLE2<
#line 45 "isa/tms320.isa"
CONFIG
#line 33153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33157 "isa_tms320.tcc"
>::OpIDLE2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33165 "isa_tms320.tcc"
>(code, addr, "IDLE2")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33173 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33176 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33180 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33183 "isa_tms320.tcc"
>
OpLOPOWER<
#line 45 "isa/tms320.isa"
CONFIG
#line 33188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33192 "isa_tms320.tcc"
>::OpLOPOWER(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33200 "isa_tms320.tcc"
>(code, addr, "LOPOWER")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33218 "isa_tms320.tcc"
>
OpMAXSPEED<
#line 45 "isa/tms320.isa"
CONFIG
#line 33223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33227 "isa_tms320.tcc"
>::OpMAXSPEED(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33235 "isa_tms320.tcc"
>(code, addr, "MAXSPEED")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33250 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33253 "isa_tms320.tcc"
>
OpLDFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33262 "isa_tms320.tcc"
>::OpLDFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33270 "isa_tms320.tcc"
>(code, addr, "LDFI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33290 "isa_tms320.tcc"
>
OpLDFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33299 "isa_tms320.tcc"
>::OpLDFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33307 "isa_tms320.tcc"
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
#line 33319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33326 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33329 "isa_tms320.tcc"
>
OpLDII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33338 "isa_tms320.tcc"
>::OpLDII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33346 "isa_tms320.tcc"
>(code, addr, "LDII_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33356 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33363 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33366 "isa_tms320.tcc"
>
OpLDII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33375 "isa_tms320.tcc"
>::OpLDII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33383 "isa_tms320.tcc"
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
#line 33395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33405 "isa_tms320.tcc"
>
OpSIGI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33414 "isa_tms320.tcc"
>::OpSIGI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33422 "isa_tms320.tcc"
>(code, addr, "SIGI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 33430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33437 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33440 "isa_tms320.tcc"
>
OpSTFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33449 "isa_tms320.tcc"
>::OpSTFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33453 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33457 "isa_tms320.tcc"
>(code, addr, "STFI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33477 "isa_tms320.tcc"
>
OpSTFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33486 "isa_tms320.tcc"
>::OpSTFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33494 "isa_tms320.tcc"
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
#line 33506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33516 "isa_tms320.tcc"
>
OpSTII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33521 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33525 "isa_tms320.tcc"
>::OpSTII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33533 "isa_tms320.tcc"
>(code, addr, "STII_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33553 "isa_tms320.tcc"
>
OpSTII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33562 "isa_tms320.tcc"
>::OpSTII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33570 "isa_tms320.tcc"
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
#line 33582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33589 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33592 "isa_tms320.tcc"
>
OpFLOAT_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33601 "isa_tms320.tcc"
>::OpFLOAT_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33609 "isa_tms320.tcc"
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
#line 33623 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33630 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33633 "isa_tms320.tcc"
>
OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33638 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33642 "isa_tms320.tcc"
>::OpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33646 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33650 "isa_tms320.tcc"
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
#line 33663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33670 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33673 "isa_tms320.tcc"
>
OpLDF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33678 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33682 "isa_tms320.tcc"
>::OpLDF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33690 "isa_tms320.tcc"
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
#line 33704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33711 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33714 "isa_tms320.tcc"
>
OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33723 "isa_tms320.tcc"
>::OpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33731 "isa_tms320.tcc"
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
#line 33744 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33754 "isa_tms320.tcc"
>
OpLDI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33763 "isa_tms320.tcc"
>::OpLDI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33771 "isa_tms320.tcc"
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
#line 33785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33792 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33795 "isa_tms320.tcc"
>
OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33804 "isa_tms320.tcc"
>::OpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33812 "isa_tms320.tcc"
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
#line 33825 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33835 "isa_tms320.tcc"
>
OpLSH3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33844 "isa_tms320.tcc"
>::OpLSH3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33852 "isa_tms320.tcc"
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
#line 33867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33870 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33877 "isa_tms320.tcc"
>
OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33886 "isa_tms320.tcc"
>::OpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33894 "isa_tms320.tcc"
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
#line 33908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33918 "isa_tms320.tcc"
>
OpMPYF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33927 "isa_tms320.tcc"
>::OpMPYF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33935 "isa_tms320.tcc"
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
#line 33950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33957 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33960 "isa_tms320.tcc"
>
OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33969 "isa_tms320.tcc"
>::OpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33973 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33977 "isa_tms320.tcc"
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
#line 33991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34001 "isa_tms320.tcc"
>
OpMPYI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34010 "isa_tms320.tcc"
>::OpMPYI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34018 "isa_tms320.tcc"
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
#line 34033 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34036 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34040 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34043 "isa_tms320.tcc"
>
OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34052 "isa_tms320.tcc"
>::OpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34060 "isa_tms320.tcc"
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
#line 34074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34081 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34084 "isa_tms320.tcc"
>
OpNEGF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34093 "isa_tms320.tcc"
>::OpNEGF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34101 "isa_tms320.tcc"
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
#line 34115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34122 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34125 "isa_tms320.tcc"
>
OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34134 "isa_tms320.tcc"
>::OpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34142 "isa_tms320.tcc"
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
#line 34155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34165 "isa_tms320.tcc"
>
OpNEGI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34174 "isa_tms320.tcc"
>::OpNEGI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34182 "isa_tms320.tcc"
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
#line 34196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34203 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34206 "isa_tms320.tcc"
>
OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34215 "isa_tms320.tcc"
>::OpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34223 "isa_tms320.tcc"
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
#line 34236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34246 "isa_tms320.tcc"
>
OpNOT_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34255 "isa_tms320.tcc"
>::OpNOT_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34263 "isa_tms320.tcc"
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
OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34296 "isa_tms320.tcc"
>::OpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34304 "isa_tms320.tcc"
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
#line 34317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34327 "isa_tms320.tcc"
>
OpOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34332 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34336 "isa_tms320.tcc"
>::OpOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34344 "isa_tms320.tcc"
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
#line 34359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34369 "isa_tms320.tcc"
>
OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34378 "isa_tms320.tcc"
>::OpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34386 "isa_tms320.tcc"
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
#line 34400 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34407 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34410 "isa_tms320.tcc"
>
OpSTF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34419 "isa_tms320.tcc"
>::OpSTF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34427 "isa_tms320.tcc"
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
#line 34441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34451 "isa_tms320.tcc"
>
OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34460 "isa_tms320.tcc"
>::OpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34468 "isa_tms320.tcc"
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
#line 34481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34491 "isa_tms320.tcc"
>
OpSTI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34500 "isa_tms320.tcc"
>::OpSTI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34508 "isa_tms320.tcc"
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
#line 34522 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34525 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34532 "isa_tms320.tcc"
>
OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34541 "isa_tms320.tcc"
>::OpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34549 "isa_tms320.tcc"
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
#line 34562 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34572 "isa_tms320.tcc"
>
OpSUBF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34581 "isa_tms320.tcc"
>::OpSUBF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34589 "isa_tms320.tcc"
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
OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34623 "isa_tms320.tcc"
>::OpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34631 "isa_tms320.tcc"
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
#line 34645 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34652 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34655 "isa_tms320.tcc"
>
OpSUBI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34664 "isa_tms320.tcc"
>::OpSUBI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34668 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34672 "isa_tms320.tcc"
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
#line 34687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34697 "isa_tms320.tcc"
>
OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34706 "isa_tms320.tcc"
>::OpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34714 "isa_tms320.tcc"
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
#line 34728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34738 "isa_tms320.tcc"
>
OpXOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34747 "isa_tms320.tcc"
>::OpXOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34755 "isa_tms320.tcc"
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
#line 34770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34777 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34780 "isa_tms320.tcc"
>
OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34789 "isa_tms320.tcc"
>::OpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34797 "isa_tms320.tcc"
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
#line 34811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34821 "isa_tms320.tcc"
>
OpLDF_LDF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34830 "isa_tms320.tcc"
>::OpLDF_LDF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34838 "isa_tms320.tcc"
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
#line 34852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34862 "isa_tms320.tcc"
>
OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34871 "isa_tms320.tcc"
>::OpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34879 "isa_tms320.tcc"
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
#line 34892 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34899 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34902 "isa_tms320.tcc"
>
OpLDI_LDI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34911 "isa_tms320.tcc"
>::OpLDI_LDI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34915 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34919 "isa_tms320.tcc"
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
#line 34933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34940 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34943 "isa_tms320.tcc"
>
OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34952 "isa_tms320.tcc"
>::OpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34960 "isa_tms320.tcc"
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
#line 34973 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34983 "isa_tms320.tcc"
>
OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34992 "isa_tms320.tcc"
>::OpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35000 "isa_tms320.tcc"
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
#line 35017 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35027 "isa_tms320.tcc"
>
OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35036 "isa_tms320.tcc"
>::OpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35044 "isa_tms320.tcc"
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
#line 35059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35066 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35069 "isa_tms320.tcc"
>
OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 35074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35078 "isa_tms320.tcc"
>::OpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35086 "isa_tms320.tcc"
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
#line 35103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35113 "isa_tms320.tcc"
>
OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35122 "isa_tms320.tcc"
>::OpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35130 "isa_tms320.tcc"
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
#line 35145 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35155 "isa_tms320.tcc"
>
OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 35160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35164 "isa_tms320.tcc"
>::OpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35172 "isa_tms320.tcc"
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
#line 35189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35199 "isa_tms320.tcc"
>
OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35208 "isa_tms320.tcc"
>::OpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35216 "isa_tms320.tcc"
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
#line 35231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35241 "isa_tms320.tcc"
>
OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 35246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35250 "isa_tms320.tcc"
>::OpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35258 "isa_tms320.tcc"
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
#line 35275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35285 "isa_tms320.tcc"
>
OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35294 "isa_tms320.tcc"
>::OpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35302 "isa_tms320.tcc"
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
#line 35317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35327 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 35332 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35336 "isa_tms320.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 45 "isa/tms320.isa"
class
#line 35347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35357 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 35362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35366 "isa_tms320.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 45 "isa/tms320.isa"
class
#line 35377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35387 "isa_tms320.tcc"
>
DecodeTableEntry<
#line 45 "isa/tms320.isa"
CONFIG
#line 35392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35396 "isa_tms320.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35404 "isa_tms320.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 45 "isa/tms320.isa"
class
#line 35415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35425 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 35430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35434 "isa_tms320.tcc"
>::Decoder()
{
	little_endian = true;
	mru_page = 0;
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35443 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35447 "isa_tms320.tcc"
	>(0x8c00e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35451 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35455 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35460 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35464 "isa_tms320.tcc"
	>(0x8c000000UL, 0xfc000000UL, DecodeOpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35468 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35472 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35477 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35481 "isa_tms320.tcc"
	>(0x8800e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35485 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35489 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35494 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35498 "isa_tms320.tcc"
	>(0x88000000UL, 0xfc000000UL, DecodeOpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35502 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35506 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35511 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35515 "isa_tms320.tcc"
	>(0x8400e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35519 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35523 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35528 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35532 "isa_tms320.tcc"
	>(0x84000000UL, 0xfc000000UL, DecodeOpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35536 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35540 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35545 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35549 "isa_tms320.tcc"
	>(0x8000e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35557 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35562 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35566 "isa_tms320.tcc"
	>(0x80000000UL, 0xfc000000UL, DecodeOpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35570 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35574 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35579 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35583 "isa_tms320.tcc"
	>(0xc60000e0UL, 0xfe0700e0UL, DecodeOpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35587 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35591 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35600 "isa_tms320.tcc"
	>(0xc6000000UL, 0xfe070000UL, DecodeOpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35604 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35608 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35613 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35617 "isa_tms320.tcc"
	>(0xc40000e0UL, 0xfe0700e0UL, DecodeOpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35621 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35625 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35630 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35634 "isa_tms320.tcc"
	>(0xc4000000UL, 0xfe070000UL, DecodeOpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35638 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35642 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35647 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35651 "isa_tms320.tcc"
	>(0xee0000e0UL, 0xfe0000e0UL, DecodeOpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35655 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35659 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35664 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35668 "isa_tms320.tcc"
	>(0xee000000UL, 0xfe000000UL, DecodeOpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35672 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35676 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35681 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35685 "isa_tms320.tcc"
	>(0xec0000e0UL, 0xfe0000e0UL, DecodeOpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35689 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35693 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35698 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35702 "isa_tms320.tcc"
	>(0xec000000UL, 0xfe000000UL, DecodeOpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35706 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35710 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35715 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35719 "isa_tms320.tcc"
	>(0xea0000e0UL, 0xfe0000e0UL, DecodeOpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35727 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35732 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35736 "isa_tms320.tcc"
	>(0xea000000UL, 0xfe000000UL, DecodeOpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35740 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35744 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35749 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35753 "isa_tms320.tcc"
	>(0xc20000e0UL, 0xfe3800e0UL, DecodeOpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35757 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35761 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35766 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35770 "isa_tms320.tcc"
	>(0xc2000000UL, 0xfe380000UL, DecodeOpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35774 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35778 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35783 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35787 "isa_tms320.tcc"
	>(0xc00000e0UL, 0xfe3800e0UL, DecodeOpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35791 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35795 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35800 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35804 "isa_tms320.tcc"
	>(0xc0000000UL, 0xfe380000UL, DecodeOpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35808 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35812 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35817 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35821 "isa_tms320.tcc"
	>(0xe80000e0UL, 0xfe0000e0UL, DecodeOpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35825 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35829 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35834 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35838 "isa_tms320.tcc"
	>(0xe8000000UL, 0xfe000000UL, DecodeOpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35842 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35846 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35851 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35855 "isa_tms320.tcc"
	>(0xe60000e0UL, 0xfe3800e0UL, DecodeOpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35859 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35863 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35868 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35872 "isa_tms320.tcc"
	>(0xe6000000UL, 0xfe380000UL, DecodeOpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35880 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35885 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35889 "isa_tms320.tcc"
	>(0xe40000e0UL, 0xfe3800e0UL, DecodeOpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35893 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35897 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35902 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35906 "isa_tms320.tcc"
	>(0xe4000000UL, 0xfe380000UL, DecodeOpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35910 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35914 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35919 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35923 "isa_tms320.tcc"
	>(0xe20000e0UL, 0xfe3800e0UL, DecodeOpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35927 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35931 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35936 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35940 "isa_tms320.tcc"
	>(0xe2000000UL, 0xfe380000UL, DecodeOpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35948 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35957 "isa_tms320.tcc"
	>(0xe00000e0UL, 0xfe0000e0UL, DecodeOpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35961 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35965 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35970 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35974 "isa_tms320.tcc"
	>(0xe0000000UL, 0xfe000000UL, DecodeOpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35978 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35982 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35987 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35991 "isa_tms320.tcc"
	>(0xde0000e0UL, 0xfe0000e0UL, DecodeOpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35995 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35999 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36004 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36008 "isa_tms320.tcc"
	>(0xde000000UL, 0xfe000000UL, DecodeOpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36012 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36016 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36021 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36025 "isa_tms320.tcc"
	>(0xdc0000e0UL, 0xfe0000e0UL, DecodeOpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36029 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36033 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36038 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36042 "isa_tms320.tcc"
	>(0xdc000000UL, 0xfe000000UL, DecodeOpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36046 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36050 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36059 "isa_tms320.tcc"
	>(0xda0000e0UL, 0xfe3800e0UL, DecodeOpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36063 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36067 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36072 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36076 "isa_tms320.tcc"
	>(0xda000000UL, 0xfe380000UL, DecodeOpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36080 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36084 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36089 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36093 "isa_tms320.tcc"
	>(0xd80000e0UL, 0xfe3800e0UL, DecodeOpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36097 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36101 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36106 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36110 "isa_tms320.tcc"
	>(0xd8000000UL, 0xfe380000UL, DecodeOpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36114 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36118 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36123 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36127 "isa_tms320.tcc"
	>(0xd60000e0UL, 0xfe3800e0UL, DecodeOpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36131 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36135 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36140 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36144 "isa_tms320.tcc"
	>(0xd6000000UL, 0xfe380000UL, DecodeOpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36148 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36152 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36157 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36161 "isa_tms320.tcc"
	>(0x15c00000UL, 0xffe00000UL, DecodeOpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36165 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36169 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36174 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36178 "isa_tms320.tcc"
	>(0x15a00000UL, 0xffe00000UL, DecodeOpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36186 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36191 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36195 "isa_tms320.tcc"
	>(0x14c00000UL, 0xffe00000UL, DecodeOpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36199 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36203 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36208 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36212 "isa_tms320.tcc"
	>(0x14a00000UL, 0xffe00000UL, DecodeOpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36216 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36220 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36225 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36229 "isa_tms320.tcc"
	>(0x16000000UL, 0xffffffffUL, DecodeOpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36233 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36237 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36242 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36246 "isa_tms320.tcc"
	>(0x8c00000UL, 0xffe00000UL, DecodeOpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36250 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36254 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36259 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36263 "isa_tms320.tcc"
	>(0x8a00000UL, 0xffe00000UL, DecodeOpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36267 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36271 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36276 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36280 "isa_tms320.tcc"
	>(0x7c00000UL, 0xffe00000UL, DecodeOpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36284 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36288 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36293 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36297 "isa_tms320.tcc"
	>(0x7a00000UL, 0xffe00000UL, DecodeOpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36301 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36305 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36310 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36314 "isa_tms320.tcc"
	>(0x10800000UL, 0xffffffffUL, DecodeOpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36318 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36322 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36327 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36331 "isa_tms320.tcc"
	>(0x10800001UL, 0xffffffffUL, DecodeOpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36335 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36339 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36344 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36348 "isa_tms320.tcc"
	>(0x6000001UL, 0xffffffffUL, DecodeOpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36352 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36356 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36361 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36365 "isa_tms320.tcc"
	>(0x74000020UL, 0xffe0ffe0UL, DecodeOpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36369 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36373 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36378 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36382 "isa_tms320.tcc"
	>(0x66000000UL, 0xffffffffUL, DecodeOpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36386 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36390 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36395 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36399 "isa_tms320.tcc"
	>(0x13fb0000UL, 0xffff0000UL, DecodeOpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36403 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36407 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36412 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36416 "isa_tms320.tcc"
	>(0x13db0000UL, 0xffff0000UL, DecodeOpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36420 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36424 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36429 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36433 "isa_tms320.tcc"
	>(0x13bb0000UL, 0xffff0000UL, DecodeOpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36437 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36441 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36446 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36450 "isa_tms320.tcc"
	>(0x139b0000UL, 0xffffffe0UL, DecodeOpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36454 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36458 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36463 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36467 "isa_tms320.tcc"
	>(0x64000000UL, 0xff000000UL, DecodeOpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36471 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36475 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36480 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36484 "isa_tms320.tcc"
	>(0x78800000UL, 0xffe0ffffUL, DecodeOpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36488 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36492 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36497 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36501 "isa_tms320.tcc"
	>(0x78000000UL, 0xffe0ffffUL, DecodeOpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36505 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36509 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36514 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36518 "isa_tms320.tcc"
	>(0xcc00000UL, 0xffff0000UL, DecodeOpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36522 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36526 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36531 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36535 "isa_tms320.tcc"
	>(0xca00000UL, 0xffff0000UL, DecodeOpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36539 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36543 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36548 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36552 "isa_tms320.tcc"
	>(0x6000000UL, 0xffffffffUL, DecodeOpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36556 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36560 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36565 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36569 "isa_tms320.tcc"
	>(0x1b400000UL, 0xffff0000UL, DecodeOpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36573 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36577 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36582 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36586 "isa_tms320.tcc"
	>(0x1b200000UL, 0xffff0000UL, DecodeOpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36590 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36594 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36599 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36603 "isa_tms320.tcc"
	>(0x6e200000UL, 0xfe200000UL, DecodeOpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36607 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36611 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36616 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36620 "isa_tms320.tcc"
	>(0x6c200000UL, 0xfe20ffe0UL, DecodeOpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36624 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36628 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36637 "isa_tms320.tcc"
	>(0x6e000000UL, 0xfe200000UL, DecodeOpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36641 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36645 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36650 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36654 "isa_tms320.tcc"
	>(0x6c000000UL, 0xfe20ffe0UL, DecodeOpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36658 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36662 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36667 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36671 "isa_tms320.tcc"
	>(0x72000000UL, 0xffe00000UL, DecodeOpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36675 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36679 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36684 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36688 "isa_tms320.tcc"
	>(0x70000000UL, 0xffe0ffe0UL, DecodeOpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36692 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36696 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36701 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36705 "isa_tms320.tcc"
	>(0x62000000UL, 0xff000000UL, DecodeOpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36709 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36713 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36718 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36722 "isa_tms320.tcc"
	>(0x61000000UL, 0xff000000UL, DecodeOpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36726 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36730 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36735 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36739 "isa_tms320.tcc"
	>(0x60000000UL, 0xff000000UL, DecodeOpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36743 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36747 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36752 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36756 "isa_tms320.tcc"
	>(0x6a200000UL, 0xffe00000UL, DecodeOpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36760 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36764 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36769 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36773 "isa_tms320.tcc"
	>(0x68200000UL, 0xffe0ffe0UL, DecodeOpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36777 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36781 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36786 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36790 "isa_tms320.tcc"
	>(0x6a000000UL, 0xffe00000UL, DecodeOpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36794 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36798 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36803 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36807 "isa_tms320.tcc"
	>(0x68000000UL, 0xffe0ffe0UL, DecodeOpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36811 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36815 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36820 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36824 "isa_tms320.tcc"
	>(0x28600000UL, 0xffe00000UL, DecodeOpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36828 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36832 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36837 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36841 "isa_tms320.tcc"
	>(0x28400000UL, 0xffe0e000UL, DecodeOpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36845 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36849 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36854 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36858 "isa_tms320.tcc"
	>(0x28200000UL, 0xffe000e0UL, DecodeOpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36862 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36866 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36871 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36875 "isa_tms320.tcc"
	>(0x28000000UL, 0xffe0e0e0UL, DecodeOpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36879 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36883 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36888 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36892 "isa_tms320.tcc"
	>(0x27e00000UL, 0xffe00000UL, DecodeOpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36896 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36900 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36905 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36909 "isa_tms320.tcc"
	>(0x27c00000UL, 0xffe0e000UL, DecodeOpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36913 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36917 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36922 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36926 "isa_tms320.tcc"
	>(0x27a00000UL, 0xffe000e0UL, DecodeOpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36930 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36934 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36939 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36943 "isa_tms320.tcc"
	>(0x27800000UL, 0xffe0e0e0UL, DecodeOpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36947 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36951 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36956 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36960 "isa_tms320.tcc"
	>(0x27600000UL, 0xffe00000UL, DecodeOpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36964 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36968 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36973 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36977 "isa_tms320.tcc"
	>(0x27400000UL, 0xffe0e000UL, DecodeOpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36981 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36985 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36990 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36994 "isa_tms320.tcc"
	>(0x27200000UL, 0xffe000e0UL, DecodeOpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36998 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37002 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37007 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37011 "isa_tms320.tcc"
	>(0x27000000UL, 0xffe0e0e0UL, DecodeOpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37015 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37019 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37024 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37028 "isa_tms320.tcc"
	>(0x26e00000UL, 0xffe00000UL, DecodeOpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37032 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37036 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37041 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37045 "isa_tms320.tcc"
	>(0x26c00000UL, 0xffe0e000UL, DecodeOpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37049 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37053 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37058 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37062 "isa_tms320.tcc"
	>(0x26a00000UL, 0xffe000e0UL, DecodeOpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37066 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37070 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37075 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37079 "isa_tms320.tcc"
	>(0x26800000UL, 0xffe0e0e0UL, DecodeOpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37083 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37087 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37092 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37096 "isa_tms320.tcc"
	>(0x26600000UL, 0xffe00000UL, DecodeOpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37100 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37104 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37109 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37113 "isa_tms320.tcc"
	>(0x26400000UL, 0xffe0e000UL, DecodeOpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37117 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37121 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37126 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37130 "isa_tms320.tcc"
	>(0x26200000UL, 0xffe000e0UL, DecodeOpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37134 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37138 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37143 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37147 "isa_tms320.tcc"
	>(0x26000000UL, 0xffe0e0e0UL, DecodeOpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37151 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37155 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37160 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37164 "isa_tms320.tcc"
	>(0x25e00000UL, 0xffe00000UL, DecodeOpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37168 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37172 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37177 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37181 "isa_tms320.tcc"
	>(0x25c00000UL, 0xffe0e000UL, DecodeOpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37185 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37189 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37194 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37198 "isa_tms320.tcc"
	>(0x25a00000UL, 0xffe000e0UL, DecodeOpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37202 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37206 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37211 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37215 "isa_tms320.tcc"
	>(0x25800000UL, 0xffe0e0e0UL, DecodeOpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37219 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37223 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37228 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37232 "isa_tms320.tcc"
	>(0x25600000UL, 0xffe00000UL, DecodeOpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37236 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37240 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37245 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37249 "isa_tms320.tcc"
	>(0x25400000UL, 0xffe0e000UL, DecodeOpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37253 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37257 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37262 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37266 "isa_tms320.tcc"
	>(0x25200000UL, 0xffe000e0UL, DecodeOpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37270 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37274 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37279 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37283 "isa_tms320.tcc"
	>(0x25000000UL, 0xffe0e0e0UL, DecodeOpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37287 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37291 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37296 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37300 "isa_tms320.tcc"
	>(0x24e00000UL, 0xffe00000UL, DecodeOpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37304 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37308 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37313 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37317 "isa_tms320.tcc"
	>(0x24c00000UL, 0xffe0e000UL, DecodeOpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37321 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37325 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37330 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37334 "isa_tms320.tcc"
	>(0x24a00000UL, 0xffe000e0UL, DecodeOpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37338 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37342 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37347 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37351 "isa_tms320.tcc"
	>(0x24800000UL, 0xffe0e0e0UL, DecodeOpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37355 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37359 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37364 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37368 "isa_tms320.tcc"
	>(0x24600000UL, 0xffe00000UL, DecodeOpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37372 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37376 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37381 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37385 "isa_tms320.tcc"
	>(0x24400000UL, 0xffe0e000UL, DecodeOpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37389 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37393 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37398 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37402 "isa_tms320.tcc"
	>(0x24200000UL, 0xffe000e0UL, DecodeOpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37406 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37410 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37415 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37419 "isa_tms320.tcc"
	>(0x24000000UL, 0xffe0e0e0UL, DecodeOpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37423 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37427 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37432 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37436 "isa_tms320.tcc"
	>(0x23e00000UL, 0xffe00000UL, DecodeOpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37440 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37444 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37449 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37453 "isa_tms320.tcc"
	>(0x23c00000UL, 0xffe0e000UL, DecodeOpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37457 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37461 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37466 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37470 "isa_tms320.tcc"
	>(0x23a00000UL, 0xffe000e0UL, DecodeOpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37474 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37478 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37483 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37487 "isa_tms320.tcc"
	>(0x23800000UL, 0xffe0e0e0UL, DecodeOpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37491 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37495 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37500 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37504 "isa_tms320.tcc"
	>(0x23600000UL, 0xffe00000UL, DecodeOpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37508 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37512 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37517 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37521 "isa_tms320.tcc"
	>(0x23400000UL, 0xffe0e000UL, DecodeOpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37525 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37529 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37534 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37538 "isa_tms320.tcc"
	>(0x23200000UL, 0xffe000e0UL, DecodeOpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37542 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37546 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37551 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37555 "isa_tms320.tcc"
	>(0x23000000UL, 0xffe0e0e0UL, DecodeOpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37559 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37563 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37568 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37572 "isa_tms320.tcc"
	>(0x22e00000UL, 0xffe00000UL, DecodeOpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37576 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37580 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37585 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37589 "isa_tms320.tcc"
	>(0x22c00000UL, 0xffe0e000UL, DecodeOpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37593 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37597 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37602 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37606 "isa_tms320.tcc"
	>(0x22a00000UL, 0xffe000e0UL, DecodeOpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37610 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37614 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37619 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37623 "isa_tms320.tcc"
	>(0x22800000UL, 0xffe0e0e0UL, DecodeOpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37627 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37631 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37636 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37640 "isa_tms320.tcc"
	>(0x22600000UL, 0xffe00000UL, DecodeOpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37644 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37648 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37653 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37657 "isa_tms320.tcc"
	>(0x22400000UL, 0xffe0e000UL, DecodeOpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37661 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37665 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37670 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37674 "isa_tms320.tcc"
	>(0x22200000UL, 0xffe000e0UL, DecodeOpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37678 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37682 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37687 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37691 "isa_tms320.tcc"
	>(0x22000000UL, 0xffe0e0e0UL, DecodeOpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37695 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37699 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37704 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37708 "isa_tms320.tcc"
	>(0x21e00000UL, 0xffe00000UL, DecodeOpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37712 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37716 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37721 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37725 "isa_tms320.tcc"
	>(0x21c00000UL, 0xffe0e000UL, DecodeOpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37729 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37733 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37738 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37742 "isa_tms320.tcc"
	>(0x21a00000UL, 0xffe000e0UL, DecodeOpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37746 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37750 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37755 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37759 "isa_tms320.tcc"
	>(0x21800000UL, 0xffe0e0e0UL, DecodeOpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37763 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37767 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37772 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37776 "isa_tms320.tcc"
	>(0x21600000UL, 0xffe00000UL, DecodeOpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37780 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37784 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37789 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37793 "isa_tms320.tcc"
	>(0x21400000UL, 0xffe0e000UL, DecodeOpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37797 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37801 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37806 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37810 "isa_tms320.tcc"
	>(0x21200000UL, 0xffe000e0UL, DecodeOpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37814 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37818 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37823 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37827 "isa_tms320.tcc"
	>(0x21000000UL, 0xffe0e0e0UL, DecodeOpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37831 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37835 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37840 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37844 "isa_tms320.tcc"
	>(0x20e00000UL, 0xffe00000UL, DecodeOpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37848 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37852 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37857 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37861 "isa_tms320.tcc"
	>(0x20c00000UL, 0xffe0e000UL, DecodeOpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37865 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37869 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37874 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37878 "isa_tms320.tcc"
	>(0x20a00000UL, 0xffe000e0UL, DecodeOpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37882 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37886 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37891 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37895 "isa_tms320.tcc"
	>(0x20800000UL, 0xffe0e0e0UL, DecodeOpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37899 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37903 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37908 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37912 "isa_tms320.tcc"
	>(0x20600000UL, 0xffe00000UL, DecodeOpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37916 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37920 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37925 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37929 "isa_tms320.tcc"
	>(0x20400000UL, 0xffe0e000UL, DecodeOpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37933 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37937 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37942 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37946 "isa_tms320.tcc"
	>(0x20200000UL, 0xffe000e0UL, DecodeOpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37950 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37954 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37959 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37963 "isa_tms320.tcc"
	>(0x20000000UL, 0xffe0e0e0UL, DecodeOpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37967 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37971 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37976 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37980 "isa_tms320.tcc"
	>(0x1ae00000UL, 0xffe00000UL, DecodeOpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37984 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37988 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37993 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37997 "isa_tms320.tcc"
	>(0x1ac00000UL, 0xffe00000UL, DecodeOpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38001 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38005 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38010 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38014 "isa_tms320.tcc"
	>(0x1aa00000UL, 0xffe00000UL, DecodeOpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38018 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38022 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38027 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38031 "isa_tms320.tcc"
	>(0x1a800000UL, 0xffe0ffe0UL, DecodeOpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38035 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38039 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38044 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38048 "isa_tms320.tcc"
	>(0x1a600000UL, 0xffe00000UL, DecodeOpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38052 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38056 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38061 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38065 "isa_tms320.tcc"
	>(0x1a400000UL, 0xffe00000UL, DecodeOpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38069 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38073 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38078 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38082 "isa_tms320.tcc"
	>(0x1a200000UL, 0xffe00000UL, DecodeOpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38086 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38090 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38095 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38099 "isa_tms320.tcc"
	>(0x1a000000UL, 0xffe0ffe0UL, DecodeOpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38103 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38107 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38112 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38116 "isa_tms320.tcc"
	>(0x19e00000UL, 0xffe00000UL, DecodeOpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38120 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38124 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38129 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38133 "isa_tms320.tcc"
	>(0x19c00000UL, 0xffe00000UL, DecodeOpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38137 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38141 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38146 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38150 "isa_tms320.tcc"
	>(0x19a00000UL, 0xffe00000UL, DecodeOpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38154 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38158 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38163 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38167 "isa_tms320.tcc"
	>(0x19800000UL, 0xffe0ffe0UL, DecodeOpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38171 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38175 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38180 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38184 "isa_tms320.tcc"
	>(0x19600000UL, 0xffe00000UL, DecodeOpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38188 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38192 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38197 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38201 "isa_tms320.tcc"
	>(0x19400000UL, 0xffe00000UL, DecodeOpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38205 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38209 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38214 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38218 "isa_tms320.tcc"
	>(0x19200000UL, 0xffe00000UL, DecodeOpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38222 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38226 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38231 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38235 "isa_tms320.tcc"
	>(0x19000000UL, 0xffe0ffe0UL, DecodeOpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38239 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38243 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38248 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38252 "isa_tms320.tcc"
	>(0x18e00000UL, 0xffe00000UL, DecodeOpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38256 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38260 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38265 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38269 "isa_tms320.tcc"
	>(0x18c00000UL, 0xffe00000UL, DecodeOpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38273 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38277 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38282 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38286 "isa_tms320.tcc"
	>(0x18a00000UL, 0xffe00000UL, DecodeOpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38290 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38294 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38299 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38303 "isa_tms320.tcc"
	>(0x18800000UL, 0xffe0ffe0UL, DecodeOpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38307 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38311 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38316 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38320 "isa_tms320.tcc"
	>(0x18600000UL, 0xffe00000UL, DecodeOpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38324 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38328 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38333 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38337 "isa_tms320.tcc"
	>(0x18400000UL, 0xffe00000UL, DecodeOpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38341 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38345 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38350 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38354 "isa_tms320.tcc"
	>(0x18200000UL, 0xffe00000UL, DecodeOpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38358 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38362 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38367 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38371 "isa_tms320.tcc"
	>(0x18000000UL, 0xffe0ffe0UL, DecodeOpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38375 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38379 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38384 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38388 "isa_tms320.tcc"
	>(0x17e00000UL, 0xffe00000UL, DecodeOpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38392 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38396 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38401 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38405 "isa_tms320.tcc"
	>(0x17c00000UL, 0xffe00000UL, DecodeOpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38409 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38413 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38418 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38422 "isa_tms320.tcc"
	>(0x17a00000UL, 0xffe00000UL, DecodeOpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38426 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38430 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38435 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38439 "isa_tms320.tcc"
	>(0x17800000UL, 0xffe0ffe0UL, DecodeOpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38443 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38447 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38452 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38456 "isa_tms320.tcc"
	>(0x17600000UL, 0xffe00000UL, DecodeOpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38460 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38464 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38469 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38473 "isa_tms320.tcc"
	>(0x17400000UL, 0xffe00000UL, DecodeOpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38477 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38481 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38486 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38490 "isa_tms320.tcc"
	>(0x17200000UL, 0xffe00000UL, DecodeOpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38494 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38498 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38503 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38507 "isa_tms320.tcc"
	>(0x17000000UL, 0xffe0ffe0UL, DecodeOpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38511 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38515 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38520 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38524 "isa_tms320.tcc"
	>(0x16e00000UL, 0xffe00000UL, DecodeOpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38528 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38532 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38537 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38541 "isa_tms320.tcc"
	>(0x16c00000UL, 0xffe00000UL, DecodeOpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38545 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38549 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38554 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38558 "isa_tms320.tcc"
	>(0x16a00000UL, 0xffe00000UL, DecodeOpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38562 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38566 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38571 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38575 "isa_tms320.tcc"
	>(0x16800000UL, 0xffe0ffe0UL, DecodeOpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38579 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38583 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38588 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38592 "isa_tms320.tcc"
	>(0x1360ffffUL, 0xffe0ffffUL, DecodeOpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38600 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38605 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38609 "isa_tms320.tcc"
	>(0x12e0ffffUL, 0xffe0ffffUL, DecodeOpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38613 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38617 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38622 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38626 "isa_tms320.tcc"
	>(0x12600001UL, 0xffe0ffffUL, DecodeOpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38630 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38634 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38639 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38643 "isa_tms320.tcc"
	>(0x11e00001UL, 0xffe0ffffUL, DecodeOpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38647 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38651 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38656 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38660 "isa_tms320.tcc"
	>(0x11600000UL, 0xffe00000UL, DecodeOpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38664 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38668 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38673 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38677 "isa_tms320.tcc"
	>(0x11400000UL, 0xffe00000UL, DecodeOpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38681 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38685 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38690 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38694 "isa_tms320.tcc"
	>(0x11200000UL, 0xffe00000UL, DecodeOpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38698 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38702 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38707 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38711 "isa_tms320.tcc"
	>(0x11000000UL, 0xffe0ffe0UL, DecodeOpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38715 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38719 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38724 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38728 "isa_tms320.tcc"
	>(0x10600000UL, 0xffe00000UL, DecodeOpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38732 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38736 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38741 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38745 "isa_tms320.tcc"
	>(0x10400000UL, 0xffe00000UL, DecodeOpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38749 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38753 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38758 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38762 "isa_tms320.tcc"
	>(0x10200000UL, 0xffe00000UL, DecodeOpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38766 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38770 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38775 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38779 "isa_tms320.tcc"
	>(0x10000000UL, 0xffe0ffe0UL, DecodeOpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38783 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38787 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38792 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38796 "isa_tms320.tcc"
	>(0xde00000UL, 0xffe00000UL, DecodeOpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38800 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38804 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38809 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38813 "isa_tms320.tcc"
	>(0xdc00000UL, 0xffe00000UL, DecodeOpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38817 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38821 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38826 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38830 "isa_tms320.tcc"
	>(0xda00000UL, 0xffe00000UL, DecodeOpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38834 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38838 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38843 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38847 "isa_tms320.tcc"
	>(0xd800000UL, 0xffe0ffe0UL, DecodeOpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38851 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38855 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38860 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38864 "isa_tms320.tcc"
	>(0xd600000UL, 0xffe00000UL, DecodeOpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38868 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38872 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38877 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38881 "isa_tms320.tcc"
	>(0xd400000UL, 0xffe00000UL, DecodeOpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38885 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38889 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38894 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38898 "isa_tms320.tcc"
	>(0xd200000UL, 0xffe00000UL, DecodeOpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38902 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38906 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38911 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38915 "isa_tms320.tcc"
	>(0xd000000UL, 0xffe0ffe0UL, DecodeOpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38919 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38923 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38928 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38932 "isa_tms320.tcc"
	>(0xc600000UL, 0xffe00000UL, DecodeOpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38936 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38940 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38945 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38949 "isa_tms320.tcc"
	>(0xc400000UL, 0xffe00000UL, DecodeOpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38957 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38962 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38966 "isa_tms320.tcc"
	>(0xc200000UL, 0xffe00000UL, DecodeOpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38970 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38974 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38979 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38983 "isa_tms320.tcc"
	>(0xc000000UL, 0xffe0ffe0UL, DecodeOpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38987 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38991 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38996 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39000 "isa_tms320.tcc"
	>(0xbe00000UL, 0xffe00000UL, DecodeOpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39004 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39008 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39013 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39017 "isa_tms320.tcc"
	>(0xbc00000UL, 0xffe00000UL, DecodeOpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39021 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39025 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39030 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39034 "isa_tms320.tcc"
	>(0xba00000UL, 0xffe00000UL, DecodeOpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39038 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39042 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39047 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39051 "isa_tms320.tcc"
	>(0xb800000UL, 0xffe0ffe0UL, DecodeOpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39059 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39064 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39068 "isa_tms320.tcc"
	>(0xb600000UL, 0xffe00000UL, DecodeOpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39072 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39076 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39081 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39085 "isa_tms320.tcc"
	>(0xb400000UL, 0xffe00000UL, DecodeOpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39089 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39093 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39102 "isa_tms320.tcc"
	>(0xb200000UL, 0xffe00000UL, DecodeOpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39106 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39110 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39115 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39119 "isa_tms320.tcc"
	>(0xb000000UL, 0xffe0ffe0UL, DecodeOpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39123 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39127 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39132 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39136 "isa_tms320.tcc"
	>(0xae00000UL, 0xffe00000UL, DecodeOpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39140 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39144 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39149 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39153 "isa_tms320.tcc"
	>(0xac00000UL, 0xffe00000UL, DecodeOpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39157 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39161 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39166 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39170 "isa_tms320.tcc"
	>(0xaa00000UL, 0xffe00000UL, DecodeOpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39174 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39178 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39183 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39187 "isa_tms320.tcc"
	>(0xa800000UL, 0xffe0ffe0UL, DecodeOpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39191 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39195 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39200 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39204 "isa_tms320.tcc"
	>(0xa600000UL, 0xffe00000UL, DecodeOpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39208 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39212 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39217 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39221 "isa_tms320.tcc"
	>(0xa400000UL, 0xffe00000UL, DecodeOpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39225 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39229 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39234 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39238 "isa_tms320.tcc"
	>(0xa200000UL, 0xffe00000UL, DecodeOpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39242 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39246 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39251 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39255 "isa_tms320.tcc"
	>(0xa000000UL, 0xffe0ffe0UL, DecodeOpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39259 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39263 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39268 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39272 "isa_tms320.tcc"
	>(0x9e00000UL, 0xffe00000UL, DecodeOpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39276 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39280 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39285 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39289 "isa_tms320.tcc"
	>(0x9c00000UL, 0xffe00000UL, DecodeOpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39293 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39297 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39302 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39306 "isa_tms320.tcc"
	>(0x9a00000UL, 0xffe00000UL, DecodeOpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39310 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39314 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39319 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39323 "isa_tms320.tcc"
	>(0x9800000UL, 0xffe0ffe0UL, DecodeOpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39327 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39331 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39336 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39340 "isa_tms320.tcc"
	>(0x5e00000UL, 0xffe00000UL, DecodeOpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39344 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39348 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39353 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39357 "isa_tms320.tcc"
	>(0x5c00000UL, 0xffe00000UL, DecodeOpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39361 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39365 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39370 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39374 "isa_tms320.tcc"
	>(0x5a00000UL, 0xffe00000UL, DecodeOpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39378 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39382 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39387 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39391 "isa_tms320.tcc"
	>(0x5800000UL, 0xffe0ffe0UL, DecodeOpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39395 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39399 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39404 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39408 "isa_tms320.tcc"
	>(0x5600000UL, 0xffe00000UL, DecodeOpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39412 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39416 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39421 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39425 "isa_tms320.tcc"
	>(0x5400000UL, 0xffe00000UL, DecodeOpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39429 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39433 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39438 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39442 "isa_tms320.tcc"
	>(0x5200000UL, 0xffe00000UL, DecodeOpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39446 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39450 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39455 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39459 "isa_tms320.tcc"
	>(0x5000000UL, 0xffe0ffe0UL, DecodeOpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39463 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39467 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39472 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39476 "isa_tms320.tcc"
	>(0x4e00000UL, 0xffe00000UL, DecodeOpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39480 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39484 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39489 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39493 "isa_tms320.tcc"
	>(0x4c00000UL, 0xffe00000UL, DecodeOpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39497 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39501 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39506 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39510 "isa_tms320.tcc"
	>(0x4a00000UL, 0xffe00000UL, DecodeOpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39514 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39518 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39523 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39527 "isa_tms320.tcc"
	>(0x4800000UL, 0xffe0ffe0UL, DecodeOpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39531 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39535 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39540 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39544 "isa_tms320.tcc"
	>(0x4600000UL, 0xffe00000UL, DecodeOpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39548 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39552 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39557 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39561 "isa_tms320.tcc"
	>(0x4400000UL, 0xffe00000UL, DecodeOpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39565 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39569 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39574 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39578 "isa_tms320.tcc"
	>(0x4200000UL, 0xffe00000UL, DecodeOpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39582 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39586 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39591 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39595 "isa_tms320.tcc"
	>(0x4000000UL, 0xffe0ffe0UL, DecodeOpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39599 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39603 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39608 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39612 "isa_tms320.tcc"
	>(0x3e00000UL, 0xffe00000UL, DecodeOpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39616 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39620 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39625 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39629 "isa_tms320.tcc"
	>(0x3c00000UL, 0xffe00000UL, DecodeOpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39637 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39642 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39646 "isa_tms320.tcc"
	>(0x3a00000UL, 0xffe00000UL, DecodeOpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39650 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39654 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39659 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39663 "isa_tms320.tcc"
	>(0x3800000UL, 0xffe0ffe0UL, DecodeOpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39667 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39671 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39676 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39680 "isa_tms320.tcc"
	>(0x3600000UL, 0xffe00000UL, DecodeOpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39684 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39688 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39693 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39697 "isa_tms320.tcc"
	>(0x3400000UL, 0xffe00000UL, DecodeOpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39701 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39705 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39710 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39714 "isa_tms320.tcc"
	>(0x3200000UL, 0xffe00000UL, DecodeOpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39718 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39722 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39727 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39731 "isa_tms320.tcc"
	>(0x3000000UL, 0xffe0ffe0UL, DecodeOpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39735 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39739 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39744 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39748 "isa_tms320.tcc"
	>(0x2e00000UL, 0xffe00000UL, DecodeOpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39752 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39756 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39761 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39765 "isa_tms320.tcc"
	>(0x2c00000UL, 0xffe00000UL, DecodeOpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39769 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39773 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39778 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39782 "isa_tms320.tcc"
	>(0x2a00000UL, 0xffe00000UL, DecodeOpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39786 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39790 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39795 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39799 "isa_tms320.tcc"
	>(0x2800000UL, 0xffe0ffe0UL, DecodeOpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39803 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39807 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39812 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39816 "isa_tms320.tcc"
	>(0x2600000UL, 0xffe00000UL, DecodeOpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39820 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39824 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39829 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39833 "isa_tms320.tcc"
	>(0x2400000UL, 0xffe00000UL, DecodeOpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39837 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39841 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39846 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39850 "isa_tms320.tcc"
	>(0x2200000UL, 0xffe00000UL, DecodeOpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39854 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39858 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39863 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39867 "isa_tms320.tcc"
	>(0x2000000UL, 0xffe0ffe0UL, DecodeOpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39871 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39875 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39880 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39884 "isa_tms320.tcc"
	>(0x1e00000UL, 0xffe00000UL, DecodeOpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39888 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39892 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39897 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39901 "isa_tms320.tcc"
	>(0x1c00000UL, 0xffe00000UL, DecodeOpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39905 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39909 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39914 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39918 "isa_tms320.tcc"
	>(0x1a00000UL, 0xffe00000UL, DecodeOpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39922 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39926 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39931 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39935 "isa_tms320.tcc"
	>(0x1800000UL, 0xffe0ffe0UL, DecodeOpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39939 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39943 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39948 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39952 "isa_tms320.tcc"
	>(0x1600000UL, 0xffe00000UL, DecodeOpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39956 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39960 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39965 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39969 "isa_tms320.tcc"
	>(0x1400000UL, 0xffe00000UL, DecodeOpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39973 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39977 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39982 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39986 "isa_tms320.tcc"
	>(0x1200000UL, 0xffe00000UL, DecodeOpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39990 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39994 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39999 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40003 "isa_tms320.tcc"
	>(0x1000000UL, 0xffe0ffe0UL, DecodeOpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40007 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40011 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40016 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40020 "isa_tms320.tcc"
	>(0xe00000UL, 0xffe00000UL, DecodeOpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40024 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40028 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40033 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40037 "isa_tms320.tcc"
	>(0xc00000UL, 0xffe00000UL, DecodeOpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40041 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40045 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40050 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40054 "isa_tms320.tcc"
	>(0xa00000UL, 0xffe00000UL, DecodeOpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40058 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40062 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40067 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40071 "isa_tms320.tcc"
	>(0x800000UL, 0xffe0ffe0UL, DecodeOpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40075 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40079 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40084 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40088 "isa_tms320.tcc"
	>(0x600000UL, 0xffe00000UL, DecodeOpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40092 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40096 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40101 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40105 "isa_tms320.tcc"
	>(0x400000UL, 0xffe00000UL, DecodeOpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40109 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40113 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40118 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40122 "isa_tms320.tcc"
	>(0x200000UL, 0xffe00000UL, DecodeOpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40126 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40130 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40135 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40139 "isa_tms320.tcc"
	>(0x0UL, 0xffe0ffe0UL, DecodeOpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40143 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40147 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40152 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40156 "isa_tms320.tcc"
	>(0x15400000UL, 0xffe00000UL, DecodeOpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40160 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40164 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40169 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40173 "isa_tms320.tcc"
	>(0x15200000UL, 0xffe00000UL, DecodeOpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40177 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40181 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40186 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40190 "isa_tms320.tcc"
	>(0x14400000UL, 0xffe00000UL, DecodeOpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40194 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40198 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40203 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40207 "isa_tms320.tcc"
	>(0x14200000UL, 0xffe00000UL, DecodeOpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40211 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40215 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40220 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40224 "isa_tms320.tcc"
	>(0xfa00000UL, 0xffe0ffffUL, DecodeOpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40228 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40232 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40237 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40241 "isa_tms320.tcc"
	>(0xf200000UL, 0xffe0ffffUL, DecodeOpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40245 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40249 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40254 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40258 "isa_tms320.tcc"
	>(0xea00000UL, 0xffe0ffffUL, DecodeOpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40262 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40266 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40271 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40275 "isa_tms320.tcc"
	>(0xe200000UL, 0xffe0ffffUL, DecodeOpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40279 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40283 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40288 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40292 "isa_tms320.tcc"
	>(0x8700000UL, 0xffffff00UL, DecodeOpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40296 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40300 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40305 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40309 "isa_tms320.tcc"
	>(0x9600000UL, 0xffe0f000UL, DecodeOpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40313 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40317 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40322 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40326 "isa_tms320.tcc"
	>(0x9400000UL, 0xffe00000UL, DecodeOpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40330 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40334 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40339 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40343 "isa_tms320.tcc"
	>(0x9200000UL, 0xffe00000UL, DecodeOpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40347 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40351 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40356 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40360 "isa_tms320.tcc"
	>(0x9000000UL, 0xffe0ffe0UL, DecodeOpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40364 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40368 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40373 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40377 "isa_tms320.tcc"
	>(0x50600000UL, 0xf0600000UL, DecodeOpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40381 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40385 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40390 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40394 "isa_tms320.tcc"
	>(0x50400000UL, 0xf0600000UL, DecodeOpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40398 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40402 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40407 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40411 "isa_tms320.tcc"
	>(0x50200000UL, 0xf0600000UL, DecodeOpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40415 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40419 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40424 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40428 "isa_tms320.tcc"
	>(0x50000000UL, 0xf060ffe0UL, DecodeOpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40432 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40436 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40441 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40445 "isa_tms320.tcc"
	>(0x8600000UL, 0xffe00000UL, DecodeOpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40449 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40453 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40458 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40462 "isa_tms320.tcc"
	>(0x8400000UL, 0xffe00000UL, DecodeOpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40466 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40470 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40475 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40479 "isa_tms320.tcc"
	>(0x8200000UL, 0xffe00000UL, DecodeOpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40483 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40487 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40492 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40496 "isa_tms320.tcc"
	>(0x8000000UL, 0xffe0ffe0UL, DecodeOpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40500 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40504 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40509 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40513 "isa_tms320.tcc"
	>(0x40600000UL, 0xf0600000UL, DecodeOpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40517 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40521 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40526 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40530 "isa_tms320.tcc"
	>(0x40400000UL, 0xf0600000UL, DecodeOpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40534 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40538 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40543 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40547 "isa_tms320.tcc"
	>(0x40200000UL, 0xf0600000UL, DecodeOpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40551 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40555 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40560 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40564 "isa_tms320.tcc"
	>(0x40000000UL, 0xf060ffe0UL, DecodeOpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40568 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40572 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40577 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40581 "isa_tms320.tcc"
	>(0x7600000UL, 0xffe00000UL, DecodeOpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40585 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40589 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40594 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40598 "isa_tms320.tcc"
	>(0x7400000UL, 0xffe00000UL, DecodeOpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40602 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40606 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40611 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40615 "isa_tms320.tcc"
	>(0x7200000UL, 0xffe00000UL, DecodeOpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40619 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40623 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40628 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40632 "isa_tms320.tcc"
	>(0x7000000UL, 0xffe0ffe0UL, DecodeOpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40636 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40640 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40645 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40649 "isa_tms320.tcc"
	>(0x6e00000UL, 0xffe00000UL, DecodeOpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40653 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40657 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40662 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40666 "isa_tms320.tcc"
	>(0x6c00000UL, 0xffe00000UL, DecodeOpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40670 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40674 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40679 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40683 "isa_tms320.tcc"
	>(0x6a00000UL, 0xffe00000UL, DecodeOpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40687 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40691 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40696 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40700 "isa_tms320.tcc"
	>(0x6800000UL, 0xffe0ffe0UL, DecodeOpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40704 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40708 "isa_tms320.tcc"
	>));
}

template <
#line 45 "isa/tms320.isa"
class
#line 40715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40725 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40734 "isa_tms320.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 45 "isa/tms320.isa"
class
#line 40743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40753 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40762 "isa_tms320.tcc"
>::Fetch(void *, typename CONFIG::address_t, uint32_t)
{
	assert( ! "Calling unimplemented virtual method Fetch" );
}

template <
#line 45 "isa/tms320.isa"
class
#line 40771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40781 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40790 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40794 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40798 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40804 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40808 "isa_tms320.tcc"
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
#line 40833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40837 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40855 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40864 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40872 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40878 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40882 "isa_tms320.tcc"
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
#line 40897 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40901 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40919 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40928 "isa_tms320.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 40938 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 40942 "isa_tms320.tcc"
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
#line 40961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40971 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40980 "isa_tms320.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40989 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40993 "isa_tms320.tcc"
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
#line 41026 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41033 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41036 "isa_tms320.tcc"
>
inline DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 41041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41045 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41053 "isa_tms320.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41061 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41065 "isa_tms320.tcc"
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
#line 41099 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41102 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41106 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41109 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 41114 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41118 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41126 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41132 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41136 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41142 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41146 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 41154 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 41158 "isa_tms320.tcc"
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
#line 41178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41188 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 41193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41197 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41205 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41211 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41215 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41221 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41225 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 41233 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 41237 "isa_tms320.tcc"
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
#line 41259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41269 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41278 "isa_tms320.tcc"
>::SetLittleEndian()
{
	little_endian = true;
}

template <
#line 45 "isa/tms320.isa"
class
#line 41287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 41290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 41294 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 41297 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 41302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 41306 "isa_tms320.tcc"
>::SetBigEndian()
{
	little_endian = false;
}

} } } } } } }
