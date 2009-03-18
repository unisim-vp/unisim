#include "isa_tms320.hh"
#include <cassert>
#line 68 "isa/tms320.isa"

/* code to be included at the beginning of the implementation of the library */
#include <sstream>
#include "unisim/component/cxx/processor/tms320/exception.hh"
#line 9 "isa_tms320.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace tms320 { namespace isa { namespace tms320 {
template <
#line 45 "isa/tms320.isa"
class
#line 14 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33 "isa_tms320.tcc"
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
#line 45 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 48 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 52 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 55 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 60 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 64 "isa_tms320.tcc"
>::~Operation()
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 72 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 75 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 79 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 82 "isa_tms320.tcc"
>
#line 40 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 86 "isa_tms320.tcc"
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 90 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 94 "isa_tms320.tcc"
>::disasm(
#line 40 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 98 "isa_tms320.tcc"
#line 40 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 101 "isa_tms320.tcc"
,
#line 40 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::stringstream &
#line 105 "isa_tms320.tcc"
#line 40 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
buffer
#line 108 "isa_tms320.tcc"
)
{
#line 40 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	{
		buffer << "Unknown instruction";
	}
#line 115 "isa_tms320.tcc"
}
template <
#line 45 "isa/tms320.isa"
class
#line 120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 130 "isa_tms320.tcc"
>
#line 36 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 134 "isa_tms320.tcc"
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 142 "isa_tms320.tcc"
>::execute(
#line 36 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 146 "isa_tms320.tcc"
#line 36 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 149 "isa_tms320.tcc"
)
{
#line 36 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 156 "isa_tms320.tcc"
}
template <
#line 45 "isa/tms320.isa"
class
#line 161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 168 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 171 "isa_tms320.tcc"
>
class OpLDE_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 176 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 180 "isa_tms320.tcc"
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
#line 193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 203 "isa_tms320.tcc"
>
class OpLDE_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 208 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 212 "isa_tms320.tcc"
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
#line 225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 235 "isa_tms320.tcc"
>
class OpLDE_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 244 "isa_tms320.tcc"
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
#line 259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 269 "isa_tms320.tcc"
>
class OpLDE_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 278 "isa_tms320.tcc"
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
#line 291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 301 "isa_tms320.tcc"
>
class OpLDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 310 "isa_tms320.tcc"
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
#line 323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 333 "isa_tms320.tcc"
>
class OpLDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 342 "isa_tms320.tcc"
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
#line 355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 362 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 365 "isa_tms320.tcc"
>
class OpLDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 374 "isa_tms320.tcc"
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
#line 389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 399 "isa_tms320.tcc"
>
class OpLDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 408 "isa_tms320.tcc"
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
#line 421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 431 "isa_tms320.tcc"
>
class OpLDFcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 440 "isa_tms320.tcc"
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
#line 454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 464 "isa_tms320.tcc"
>
class OpLDFcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 473 "isa_tms320.tcc"
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
#line 487 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 497 "isa_tms320.tcc"
>
class OpLDFcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 506 "isa_tms320.tcc"
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
#line 522 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 525 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 532 "isa_tms320.tcc"
>
class OpLDFcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 541 "isa_tms320.tcc"
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
#line 555 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 562 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 565 "isa_tms320.tcc"
>
class OpLDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 574 "isa_tms320.tcc"
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
#line 587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 597 "isa_tms320.tcc"
>
class OpLDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 606 "isa_tms320.tcc"
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
#line 619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 629 "isa_tms320.tcc"
>
class OpLDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 638 "isa_tms320.tcc"
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
#line 653 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 663 "isa_tms320.tcc"
>
class OpLDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 668 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 672 "isa_tms320.tcc"
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
#line 685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 695 "isa_tms320.tcc"
>
class OpLDIcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 704 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 718 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 721 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 725 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 728 "isa_tms320.tcc"
>
class OpLDIcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 737 "isa_tms320.tcc"
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
#line 751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 754 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 758 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 761 "isa_tms320.tcc"
>
class OpLDIcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 770 "isa_tms320.tcc"
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
#line 786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 796 "isa_tms320.tcc"
>
class OpLDIcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 805 "isa_tms320.tcc"
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
#line 819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 829 "isa_tms320.tcc"
>
class OpLDM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 838 "isa_tms320.tcc"
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
#line 851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 861 "isa_tms320.tcc"
>
class OpLDM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 870 "isa_tms320.tcc"
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
#line 883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 890 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 893 "isa_tms320.tcc"
>
class OpLDM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 898 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 902 "isa_tms320.tcc"
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
#line 917 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 924 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 927 "isa_tms320.tcc"
>
class OpLDM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 936 "isa_tms320.tcc"
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
#line 949 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 952 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 959 "isa_tms320.tcc"
>
class OpLDP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 968 "isa_tms320.tcc"
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
#line 980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 990 "isa_tms320.tcc"
>
class OpPOP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 999 "isa_tms320.tcc"
>
{
public:
	OpPOP(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1021 "isa_tms320.tcc"
>
class OpPOPF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1030 "isa_tms320.tcc"
>
{
public:
	OpPOPF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1042 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1052 "isa_tms320.tcc"
>
class OpPUSH : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1057 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1061 "isa_tms320.tcc"
>
{
public:
	OpPUSH(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1083 "isa_tms320.tcc"
>
class OpPUSHF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1092 "isa_tms320.tcc"
>
{
public:
	OpPUSHF(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1104 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1111 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1114 "isa_tms320.tcc"
>
class OpSTF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1123 "isa_tms320.tcc"
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
#line 1136 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1139 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1143 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1146 "isa_tms320.tcc"
>
class OpSTF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1155 "isa_tms320.tcc"
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
#line 1170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1177 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1180 "isa_tms320.tcc"
>
class OpSTI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1189 "isa_tms320.tcc"
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
#line 1202 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1209 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1212 "isa_tms320.tcc"
>
class OpSTI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1217 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1221 "isa_tms320.tcc"
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
#line 1236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1246 "isa_tms320.tcc"
>
class OpABSF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1255 "isa_tms320.tcc"
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
#line 1268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1278 "isa_tms320.tcc"
>
class OpABSF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1287 "isa_tms320.tcc"
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
#line 1300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1307 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1310 "isa_tms320.tcc"
>
class OpABSF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1319 "isa_tms320.tcc"
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
#line 1334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1344 "isa_tms320.tcc"
>
class OpABSF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1353 "isa_tms320.tcc"
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
#line 1366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1373 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1376 "isa_tms320.tcc"
>
class OpABSI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1385 "isa_tms320.tcc"
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
#line 1398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1408 "isa_tms320.tcc"
>
class OpABSI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1417 "isa_tms320.tcc"
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
#line 1430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1437 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1440 "isa_tms320.tcc"
>
class OpABSI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1449 "isa_tms320.tcc"
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
#line 1464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1474 "isa_tms320.tcc"
>
class OpABSI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1483 "isa_tms320.tcc"
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
#line 1496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1506 "isa_tms320.tcc"
>
class OpADDC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1515 "isa_tms320.tcc"
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
#line 1528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1538 "isa_tms320.tcc"
>
class OpADDC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1547 "isa_tms320.tcc"
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
#line 1560 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1563 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1567 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1570 "isa_tms320.tcc"
>
class OpADDC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1575 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1579 "isa_tms320.tcc"
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
#line 1594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1604 "isa_tms320.tcc"
>
class OpADDC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1613 "isa_tms320.tcc"
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
#line 1626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1636 "isa_tms320.tcc"
>
class OpADDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1641 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1645 "isa_tms320.tcc"
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
#line 1658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1668 "isa_tms320.tcc"
>
class OpADDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1677 "isa_tms320.tcc"
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
#line 1690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1693 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1697 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1700 "isa_tms320.tcc"
>
class OpADDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1705 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1709 "isa_tms320.tcc"
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
#line 1724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1734 "isa_tms320.tcc"
>
class OpADDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1743 "isa_tms320.tcc"
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
#line 1756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1766 "isa_tms320.tcc"
>
class OpADDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1771 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1775 "isa_tms320.tcc"
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
#line 1788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1798 "isa_tms320.tcc"
>
class OpADDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1807 "isa_tms320.tcc"
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
#line 1820 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1827 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1830 "isa_tms320.tcc"
>
class OpADDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1839 "isa_tms320.tcc"
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
#line 1854 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1864 "isa_tms320.tcc"
>
class OpADDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1873 "isa_tms320.tcc"
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
#line 1886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1896 "isa_tms320.tcc"
>
class OpAND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1905 "isa_tms320.tcc"
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
#line 1918 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1928 "isa_tms320.tcc"
>
class OpAND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1937 "isa_tms320.tcc"
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
#line 1950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1957 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1960 "isa_tms320.tcc"
>
class OpAND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1969 "isa_tms320.tcc"
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
#line 1984 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1994 "isa_tms320.tcc"
>
class OpAND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2003 "isa_tms320.tcc"
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
#line 2016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2026 "isa_tms320.tcc"
>
class OpANDN_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2035 "isa_tms320.tcc"
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
#line 2048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2055 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2058 "isa_tms320.tcc"
>
class OpANDN_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2063 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2067 "isa_tms320.tcc"
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
#line 2080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2087 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2090 "isa_tms320.tcc"
>
class OpANDN_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2095 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2099 "isa_tms320.tcc"
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
#line 2114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2124 "isa_tms320.tcc"
>
class OpANDN_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2133 "isa_tms320.tcc"
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
#line 2146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2149 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2153 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2156 "isa_tms320.tcc"
>
class OpASH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2165 "isa_tms320.tcc"
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
#line 2178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2188 "isa_tms320.tcc"
>
class OpASH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2197 "isa_tms320.tcc"
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
#line 2210 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2220 "isa_tms320.tcc"
>
class OpASH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2225 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2229 "isa_tms320.tcc"
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
#line 2244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2254 "isa_tms320.tcc"
>
class OpASH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2263 "isa_tms320.tcc"
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
#line 2276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2283 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2286 "isa_tms320.tcc"
>
class OpCMPF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2295 "isa_tms320.tcc"
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
#line 2308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2318 "isa_tms320.tcc"
>
class OpCMPF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2323 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2327 "isa_tms320.tcc"
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
#line 2340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2350 "isa_tms320.tcc"
>
class OpCMPF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2359 "isa_tms320.tcc"
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
#line 2374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2384 "isa_tms320.tcc"
>
class OpCMPF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2393 "isa_tms320.tcc"
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
#line 2406 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2413 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2416 "isa_tms320.tcc"
>
class OpCMPI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2421 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2425 "isa_tms320.tcc"
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
#line 2438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2448 "isa_tms320.tcc"
>
class OpCMPI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2453 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2457 "isa_tms320.tcc"
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
#line 2470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2473 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2480 "isa_tms320.tcc"
>
class OpCMPI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2489 "isa_tms320.tcc"
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
#line 2504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2514 "isa_tms320.tcc"
>
class OpCMPI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2523 "isa_tms320.tcc"
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
#line 2536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2546 "isa_tms320.tcc"
>
class OpFIX_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2555 "isa_tms320.tcc"
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
#line 2568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2578 "isa_tms320.tcc"
>
class OpFIX_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2587 "isa_tms320.tcc"
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
#line 2600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2610 "isa_tms320.tcc"
>
class OpFIX_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2619 "isa_tms320.tcc"
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
#line 2634 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2644 "isa_tms320.tcc"
>
class OpFIX_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2649 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2653 "isa_tms320.tcc"
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
#line 2666 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2676 "isa_tms320.tcc"
>
class OpFLOAT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2685 "isa_tms320.tcc"
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
#line 2698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2708 "isa_tms320.tcc"
>
class OpFLOAT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2717 "isa_tms320.tcc"
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
#line 2730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2740 "isa_tms320.tcc"
>
class OpFLOAT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2749 "isa_tms320.tcc"
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
#line 2764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2774 "isa_tms320.tcc"
>
class OpFLOAT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2783 "isa_tms320.tcc"
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
#line 2796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2806 "isa_tms320.tcc"
>
class OpLSH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2815 "isa_tms320.tcc"
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
#line 2828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2838 "isa_tms320.tcc"
>
class OpLSH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2847 "isa_tms320.tcc"
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
#line 2860 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2863 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2870 "isa_tms320.tcc"
>
class OpLSH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2879 "isa_tms320.tcc"
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
#line 2894 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2897 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2901 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2904 "isa_tms320.tcc"
>
class OpLSH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2913 "isa_tms320.tcc"
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
#line 2926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2936 "isa_tms320.tcc"
>
class OpMPYF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2945 "isa_tms320.tcc"
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
#line 2958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2968 "isa_tms320.tcc"
>
class OpMPYF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2973 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2977 "isa_tms320.tcc"
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
#line 2990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2997 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3000 "isa_tms320.tcc"
>
class OpMPYF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3005 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3009 "isa_tms320.tcc"
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
#line 3024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3031 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3034 "isa_tms320.tcc"
>
class OpMPYF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3043 "isa_tms320.tcc"
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
#line 3056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3066 "isa_tms320.tcc"
>
class OpMPYI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3075 "isa_tms320.tcc"
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
#line 3088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3098 "isa_tms320.tcc"
>
class OpMPYI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3107 "isa_tms320.tcc"
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
#line 3120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3130 "isa_tms320.tcc"
>
class OpMPYI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3139 "isa_tms320.tcc"
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
#line 3154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3164 "isa_tms320.tcc"
>
class OpMPYI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3173 "isa_tms320.tcc"
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
#line 3186 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3196 "isa_tms320.tcc"
>
class OpNEGB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3205 "isa_tms320.tcc"
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
#line 3218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3228 "isa_tms320.tcc"
>
class OpNEGB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3237 "isa_tms320.tcc"
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
#line 3250 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3257 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3260 "isa_tms320.tcc"
>
class OpNEGB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3265 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3269 "isa_tms320.tcc"
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
#line 3284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3294 "isa_tms320.tcc"
>
class OpNEGB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3303 "isa_tms320.tcc"
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
#line 3316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3326 "isa_tms320.tcc"
>
class OpNEGF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3335 "isa_tms320.tcc"
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
#line 3348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3358 "isa_tms320.tcc"
>
class OpNEGF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3367 "isa_tms320.tcc"
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
#line 3380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3390 "isa_tms320.tcc"
>
class OpNEGF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3399 "isa_tms320.tcc"
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
#line 3414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3424 "isa_tms320.tcc"
>
class OpNEGF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3433 "isa_tms320.tcc"
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
#line 3446 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3449 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3453 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3456 "isa_tms320.tcc"
>
class OpNEGI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3465 "isa_tms320.tcc"
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
#line 3478 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3485 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3488 "isa_tms320.tcc"
>
class OpNEGI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3493 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3497 "isa_tms320.tcc"
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
#line 3510 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3513 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3517 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3520 "isa_tms320.tcc"
>
class OpNEGI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3525 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3529 "isa_tms320.tcc"
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
#line 3544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3554 "isa_tms320.tcc"
>
class OpNEGI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3563 "isa_tms320.tcc"
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
#line 3576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3586 "isa_tms320.tcc"
>
class OpNORM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3595 "isa_tms320.tcc"
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
#line 3608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3618 "isa_tms320.tcc"
>
class OpNORM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3627 "isa_tms320.tcc"
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
#line 3640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3643 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3650 "isa_tms320.tcc"
>
class OpNORM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3659 "isa_tms320.tcc"
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
#line 3674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3684 "isa_tms320.tcc"
>
class OpNORM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3693 "isa_tms320.tcc"
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
#line 3706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3709 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3716 "isa_tms320.tcc"
>
class OpNOT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3721 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3725 "isa_tms320.tcc"
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
#line 3738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3748 "isa_tms320.tcc"
>
class OpNOT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3757 "isa_tms320.tcc"
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
#line 3770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3773 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3777 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3780 "isa_tms320.tcc"
>
class OpNOT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3789 "isa_tms320.tcc"
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
#line 3804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3807 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3814 "isa_tms320.tcc"
>
class OpNOT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3823 "isa_tms320.tcc"
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
#line 3836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3846 "isa_tms320.tcc"
>
class OpOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3855 "isa_tms320.tcc"
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
#line 3868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3878 "isa_tms320.tcc"
>
class OpOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3883 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3887 "isa_tms320.tcc"
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
#line 3900 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3907 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3910 "isa_tms320.tcc"
>
class OpOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3915 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3919 "isa_tms320.tcc"
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
#line 3934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3944 "isa_tms320.tcc"
>
class OpOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3953 "isa_tms320.tcc"
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
#line 3966 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3969 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3973 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3976 "isa_tms320.tcc"
>
class OpRND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3985 "isa_tms320.tcc"
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
#line 3998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4005 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4008 "isa_tms320.tcc"
>
class OpRND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4013 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4017 "isa_tms320.tcc"
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
#line 4030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4040 "isa_tms320.tcc"
>
class OpRND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4049 "isa_tms320.tcc"
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
#line 4064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4074 "isa_tms320.tcc"
>
class OpRND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4083 "isa_tms320.tcc"
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
#line 4096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4099 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4106 "isa_tms320.tcc"
>
class OpROL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4115 "isa_tms320.tcc"
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
#line 4127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4134 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4137 "isa_tms320.tcc"
>
class OpROLC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4146 "isa_tms320.tcc"
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
#line 4158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4168 "isa_tms320.tcc"
>
class OpROR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4177 "isa_tms320.tcc"
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
class OpRORC : public Operation<
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
	OpRORC(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4220 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4230 "isa_tms320.tcc"
>
class OpSUBB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4239 "isa_tms320.tcc"
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
#line 4252 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4262 "isa_tms320.tcc"
>
class OpSUBB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4271 "isa_tms320.tcc"
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
#line 4284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4294 "isa_tms320.tcc"
>
class OpSUBB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4303 "isa_tms320.tcc"
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
#line 4318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4328 "isa_tms320.tcc"
>
class OpSUBB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4337 "isa_tms320.tcc"
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
#line 4350 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4357 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4360 "isa_tms320.tcc"
>
class OpSUBC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4365 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4369 "isa_tms320.tcc"
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
#line 4382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4392 "isa_tms320.tcc"
>
class OpSUBC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4401 "isa_tms320.tcc"
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
#line 4414 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4424 "isa_tms320.tcc"
>
class OpSUBC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4433 "isa_tms320.tcc"
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
#line 4448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4458 "isa_tms320.tcc"
>
class OpSUBC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4467 "isa_tms320.tcc"
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
#line 4480 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4487 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4490 "isa_tms320.tcc"
>
class OpSUBF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4495 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4499 "isa_tms320.tcc"
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
#line 4512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4519 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4522 "isa_tms320.tcc"
>
class OpSUBF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4531 "isa_tms320.tcc"
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
#line 4544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4554 "isa_tms320.tcc"
>
class OpSUBF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4563 "isa_tms320.tcc"
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
#line 4578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4588 "isa_tms320.tcc"
>
class OpSUBF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4597 "isa_tms320.tcc"
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
#line 4610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4620 "isa_tms320.tcc"
>
class OpSUBI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4629 "isa_tms320.tcc"
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
#line 4642 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4645 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4652 "isa_tms320.tcc"
>
class OpSUBI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4661 "isa_tms320.tcc"
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
#line 4674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4684 "isa_tms320.tcc"
>
class OpSUBI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4693 "isa_tms320.tcc"
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
#line 4708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4718 "isa_tms320.tcc"
>
class OpSUBI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4727 "isa_tms320.tcc"
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
class OpSUBRB_reg : public Operation<
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
	OpSUBRB_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4782 "isa_tms320.tcc"
>
class OpSUBRB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4791 "isa_tms320.tcc"
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
#line 4804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4807 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4814 "isa_tms320.tcc"
>
class OpSUBRB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4823 "isa_tms320.tcc"
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
#line 4838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4848 "isa_tms320.tcc"
>
class OpSUBRB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4857 "isa_tms320.tcc"
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
#line 4870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4880 "isa_tms320.tcc"
>
class OpSUBRF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4889 "isa_tms320.tcc"
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
#line 4902 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4912 "isa_tms320.tcc"
>
class OpSUBRF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4921 "isa_tms320.tcc"
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
#line 4934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4944 "isa_tms320.tcc"
>
class OpSUBRF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4953 "isa_tms320.tcc"
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
#line 4968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4978 "isa_tms320.tcc"
>
class OpSUBRF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4987 "isa_tms320.tcc"
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
#line 5000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5007 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5010 "isa_tms320.tcc"
>
class OpSUBRI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5019 "isa_tms320.tcc"
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
#line 5032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5042 "isa_tms320.tcc"
>
class OpSUBRI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5051 "isa_tms320.tcc"
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
#line 5064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5074 "isa_tms320.tcc"
>
class OpSUBRI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5083 "isa_tms320.tcc"
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
#line 5098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5108 "isa_tms320.tcc"
>
class OpSUBRI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5117 "isa_tms320.tcc"
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
#line 5130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5140 "isa_tms320.tcc"
>
class OpTSTB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5149 "isa_tms320.tcc"
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
#line 5162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5172 "isa_tms320.tcc"
>
class OpTSTB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5181 "isa_tms320.tcc"
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
#line 5194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5204 "isa_tms320.tcc"
>
class OpTSTB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5213 "isa_tms320.tcc"
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
#line 5228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5238 "isa_tms320.tcc"
>
class OpTSTB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5247 "isa_tms320.tcc"
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
#line 5260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5270 "isa_tms320.tcc"
>
class OpXOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5279 "isa_tms320.tcc"
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
#line 5292 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5302 "isa_tms320.tcc"
>
class OpXOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5311 "isa_tms320.tcc"
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
#line 5324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5331 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5334 "isa_tms320.tcc"
>
class OpXOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5339 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5343 "isa_tms320.tcc"
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
#line 5358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5368 "isa_tms320.tcc"
>
class OpXOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5373 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5377 "isa_tms320.tcc"
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
#line 5390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5397 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5400 "isa_tms320.tcc"
>
class OpADDC3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5409 "isa_tms320.tcc"
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
#line 5423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5433 "isa_tms320.tcc"
>
class OpADDC3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5442 "isa_tms320.tcc"
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
#line 5457 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5467 "isa_tms320.tcc"
>
class OpADDC3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5472 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5476 "isa_tms320.tcc"
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
#line 5491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5501 "isa_tms320.tcc"
>
class OpADDC3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5510 "isa_tms320.tcc"
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
#line 5526 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5536 "isa_tms320.tcc"
>
class OpADDF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5545 "isa_tms320.tcc"
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
#line 5559 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5566 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5569 "isa_tms320.tcc"
>
class OpADDF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5578 "isa_tms320.tcc"
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
#line 5593 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5603 "isa_tms320.tcc"
>
class OpADDF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5608 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5612 "isa_tms320.tcc"
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
#line 5627 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5630 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5634 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5637 "isa_tms320.tcc"
>
class OpADDF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5646 "isa_tms320.tcc"
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
#line 5662 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5672 "isa_tms320.tcc"
>
class OpADDI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5681 "isa_tms320.tcc"
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
#line 5695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5705 "isa_tms320.tcc"
>
class OpADDI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5714 "isa_tms320.tcc"
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
#line 5729 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5732 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5736 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5739 "isa_tms320.tcc"
>
class OpADDI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5748 "isa_tms320.tcc"
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
#line 5763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5773 "isa_tms320.tcc"
>
class OpADDI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5782 "isa_tms320.tcc"
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
#line 5798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5808 "isa_tms320.tcc"
>
class OpAND3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5817 "isa_tms320.tcc"
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
#line 5831 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5841 "isa_tms320.tcc"
>
class OpAND3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5846 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5850 "isa_tms320.tcc"
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
#line 5865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5875 "isa_tms320.tcc"
>
class OpAND3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5884 "isa_tms320.tcc"
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
#line 5899 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5902 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5909 "isa_tms320.tcc"
>
class OpAND3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5918 "isa_tms320.tcc"
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
#line 5934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5944 "isa_tms320.tcc"
>
class OpANDN3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5953 "isa_tms320.tcc"
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
#line 5967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5977 "isa_tms320.tcc"
>
class OpANDN3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5986 "isa_tms320.tcc"
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
#line 6001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6011 "isa_tms320.tcc"
>
class OpANDN3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6020 "isa_tms320.tcc"
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
#line 6035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6042 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6045 "isa_tms320.tcc"
>
class OpANDN3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6050 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6054 "isa_tms320.tcc"
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
class OpASH3_reg_reg : public Operation<
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
	OpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6113 "isa_tms320.tcc"
>
class OpASH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6122 "isa_tms320.tcc"
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
#line 6137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6147 "isa_tms320.tcc"
>
class OpASH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6156 "isa_tms320.tcc"
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
#line 6171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6181 "isa_tms320.tcc"
>
class OpASH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6190 "isa_tms320.tcc"
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
class OpCMPF3_reg_reg : public Operation<
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
	OpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6249 "isa_tms320.tcc"
>
class OpCMPF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6258 "isa_tms320.tcc"
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
#line 6273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6283 "isa_tms320.tcc"
>
class OpCMPF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6292 "isa_tms320.tcc"
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
#line 6307 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6317 "isa_tms320.tcc"
>
class OpCMPF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6326 "isa_tms320.tcc"
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
class OpCMPI3_reg_reg : public Operation<
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
	OpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6385 "isa_tms320.tcc"
>
class OpCMPI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6394 "isa_tms320.tcc"
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
#line 6409 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6412 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6416 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6419 "isa_tms320.tcc"
>
class OpCMPI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6428 "isa_tms320.tcc"
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
#line 6443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6450 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6453 "isa_tms320.tcc"
>
class OpCMPI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6462 "isa_tms320.tcc"
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
class OpLSH3_reg_reg : public Operation<
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
	OpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6518 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6521 "isa_tms320.tcc"
>
class OpLSH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6526 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6530 "isa_tms320.tcc"
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
#line 6545 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6555 "isa_tms320.tcc"
>
class OpLSH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6560 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6564 "isa_tms320.tcc"
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
#line 6579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6589 "isa_tms320.tcc"
>
class OpLSH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6598 "isa_tms320.tcc"
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
class OpMPYF3_reg_reg : public Operation<
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
	OpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6657 "isa_tms320.tcc"
>
class OpMPYF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6666 "isa_tms320.tcc"
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
#line 6681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6691 "isa_tms320.tcc"
>
class OpMPYF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6700 "isa_tms320.tcc"
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
#line 6715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6725 "isa_tms320.tcc"
>
class OpMPYF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6734 "isa_tms320.tcc"
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
class OpMPYI3_reg_reg : public Operation<
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
	OpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6783 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6790 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6793 "isa_tms320.tcc"
>
class OpMPYI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6802 "isa_tms320.tcc"
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
#line 6817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6824 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6827 "isa_tms320.tcc"
>
class OpMPYI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6836 "isa_tms320.tcc"
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
#line 6851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6861 "isa_tms320.tcc"
>
class OpMPYI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6870 "isa_tms320.tcc"
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
class OpOR3_reg_reg : public Operation<
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
	OpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6929 "isa_tms320.tcc"
>
class OpOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6938 "isa_tms320.tcc"
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
#line 6953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6963 "isa_tms320.tcc"
>
class OpOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6972 "isa_tms320.tcc"
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
#line 6987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6997 "isa_tms320.tcc"
>
class OpOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7006 "isa_tms320.tcc"
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
class OpSUBB3_reg_reg : public Operation<
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
	OpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7055 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7062 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7065 "isa_tms320.tcc"
>
class OpSUBB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7074 "isa_tms320.tcc"
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
#line 7089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7099 "isa_tms320.tcc"
>
class OpSUBB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7108 "isa_tms320.tcc"
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
#line 7123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7133 "isa_tms320.tcc"
>
class OpSUBB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7142 "isa_tms320.tcc"
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
class OpSUBF3_reg_reg : public Operation<
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
	OpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7201 "isa_tms320.tcc"
>
class OpSUBF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7210 "isa_tms320.tcc"
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
#line 7225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7235 "isa_tms320.tcc"
>
class OpSUBF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7244 "isa_tms320.tcc"
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
#line 7259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7269 "isa_tms320.tcc"
>
class OpSUBF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7278 "isa_tms320.tcc"
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
class OpSUBI3_reg_reg : public Operation<
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
	OpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7327 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7337 "isa_tms320.tcc"
>
class OpSUBI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7346 "isa_tms320.tcc"
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
#line 7361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7371 "isa_tms320.tcc"
>
class OpSUBI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7380 "isa_tms320.tcc"
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
#line 7395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7405 "isa_tms320.tcc"
>
class OpSUBI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7414 "isa_tms320.tcc"
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
class OpTSTB3_reg_reg : public Operation<
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
	OpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7473 "isa_tms320.tcc"
>
class OpTSTB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7482 "isa_tms320.tcc"
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
#line 7497 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7507 "isa_tms320.tcc"
>
class OpTSTB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7512 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7516 "isa_tms320.tcc"
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
#line 7531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7541 "isa_tms320.tcc"
>
class OpTSTB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7550 "isa_tms320.tcc"
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
class OpXOR3_reg_reg : public Operation<
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
	OpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7599 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7609 "isa_tms320.tcc"
>
class OpXOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7618 "isa_tms320.tcc"
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
#line 7633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7643 "isa_tms320.tcc"
>
class OpXOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7652 "isa_tms320.tcc"
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
#line 7667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7670 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7677 "isa_tms320.tcc"
>
class OpXOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7686 "isa_tms320.tcc"
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
class OpBcond_reg : public Operation<
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
	OpBcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t reg;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7744 "isa_tms320.tcc"
>
class OpBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7753 "isa_tms320.tcc"
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
#line 7766 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7769 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7773 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7776 "isa_tms320.tcc"
>
class OpBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7785 "isa_tms320.tcc"
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
#line 7798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7808 "isa_tms320.tcc"
>
class OpBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7817 "isa_tms320.tcc"
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
#line 7830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7840 "isa_tms320.tcc"
>
class OpBR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7849 "isa_tms320.tcc"
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
#line 7861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7871 "isa_tms320.tcc"
>
class OpBRD : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7880 "isa_tms320.tcc"
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
#line 7892 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7899 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7902 "isa_tms320.tcc"
>
class OpCALL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7911 "isa_tms320.tcc"
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
#line 7923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7933 "isa_tms320.tcc"
>
class OpCALLcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7942 "isa_tms320.tcc"
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
#line 7955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7965 "isa_tms320.tcc"
>
class OpCALLcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7974 "isa_tms320.tcc"
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
#line 7987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7997 "isa_tms320.tcc"
>
class OpDBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8006 "isa_tms320.tcc"
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
#line 8020 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8030 "isa_tms320.tcc"
>
class OpDBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8039 "isa_tms320.tcc"
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
#line 8053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8060 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8063 "isa_tms320.tcc"
>
class OpDBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8072 "isa_tms320.tcc"
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
#line 8086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8096 "isa_tms320.tcc"
>
class OpDBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8105 "isa_tms320.tcc"
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
#line 8119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8129 "isa_tms320.tcc"
>
class OpIACK_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8138 "isa_tms320.tcc"
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
#line 8150 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8160 "isa_tms320.tcc"
>
class OpIACK_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8169 "isa_tms320.tcc"
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
#line 8183 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8190 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8193 "isa_tms320.tcc"
>
class OpIDLE : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8202 "isa_tms320.tcc"
>
{
public:
	OpIDLE(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8220 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8223 "isa_tms320.tcc"
>
class OpNOP_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8232 "isa_tms320.tcc"
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
#line 8244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8247 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8254 "isa_tms320.tcc"
>
class OpNOP_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8263 "isa_tms320.tcc"
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
#line 8277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8287 "isa_tms320.tcc"
>
class OpRETIcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8296 "isa_tms320.tcc"
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
#line 8308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8318 "isa_tms320.tcc"
>
class OpRETScond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8323 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8327 "isa_tms320.tcc"
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
#line 8339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8349 "isa_tms320.tcc"
>
class OpRPTB : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8358 "isa_tms320.tcc"
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
#line 8370 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8373 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8380 "isa_tms320.tcc"
>
class OpRPTS_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8389 "isa_tms320.tcc"
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
#line 8401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8411 "isa_tms320.tcc"
>
class OpRPTS_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8420 "isa_tms320.tcc"
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
class OpRPTS_indir : public Operation<
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
	OpRPTS_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8472 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8475 "isa_tms320.tcc"
>
class OpRPTS_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8484 "isa_tms320.tcc"
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
#line 8496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8506 "isa_tms320.tcc"
>
class OpSWI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8515 "isa_tms320.tcc"
>
{
public:
	OpSWI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8526 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8536 "isa_tms320.tcc"
>
class OpTRAPcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8545 "isa_tms320.tcc"
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
#line 8558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8568 "isa_tms320.tcc"
>
class OpIDLE2 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8577 "isa_tms320.tcc"
>
{
public:
	OpIDLE2(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8598 "isa_tms320.tcc"
>
class OpLOPOWER : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8607 "isa_tms320.tcc"
>
{
public:
	OpLOPOWER(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8628 "isa_tms320.tcc"
>
class OpMAXSPEED : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8637 "isa_tms320.tcc"
>
{
public:
	OpMAXSPEED(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8658 "isa_tms320.tcc"
>
class OpLDFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8667 "isa_tms320.tcc"
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
#line 8680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8690 "isa_tms320.tcc"
>
class OpLDFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8695 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8699 "isa_tms320.tcc"
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
#line 8714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8724 "isa_tms320.tcc"
>
class OpLDII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8733 "isa_tms320.tcc"
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
#line 8746 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8753 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8756 "isa_tms320.tcc"
>
class OpLDII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8765 "isa_tms320.tcc"
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
#line 8780 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8783 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8790 "isa_tms320.tcc"
>
class OpSIGI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8799 "isa_tms320.tcc"
>
{
public:
	OpSIGI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8820 "isa_tms320.tcc"
>
class OpSTFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8829 "isa_tms320.tcc"
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
#line 8842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8852 "isa_tms320.tcc"
>
class OpSTFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8861 "isa_tms320.tcc"
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
#line 8876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8886 "isa_tms320.tcc"
>
class OpSTII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8895 "isa_tms320.tcc"
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
#line 8908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8918 "isa_tms320.tcc"
>
class OpSTII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8927 "isa_tms320.tcc"
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
#line 8942 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8949 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8952 "isa_tms320.tcc"
>
class OpFLOAT_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8957 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8961 "isa_tms320.tcc"
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
#line 8978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8988 "isa_tms320.tcc"
>
class OpFLOAT_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8997 "isa_tms320.tcc"
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
#line 9013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9020 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9023 "isa_tms320.tcc"
>
class OpLDF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9032 "isa_tms320.tcc"
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
#line 9049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9059 "isa_tms320.tcc"
>
class OpLDF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9068 "isa_tms320.tcc"
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
class OpLDI_STI : public Operation<
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
#line 9120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9130 "isa_tms320.tcc"
>
class OpLDI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9139 "isa_tms320.tcc"
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
#line 9155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9165 "isa_tms320.tcc"
>
class OpLSH3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9174 "isa_tms320.tcc"
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
#line 9192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9202 "isa_tms320.tcc"
>
class OpLSH3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9211 "isa_tms320.tcc"
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
#line 9228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9238 "isa_tms320.tcc"
>
class OpMPYF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9247 "isa_tms320.tcc"
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
#line 9265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9275 "isa_tms320.tcc"
>
class OpMPYF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9284 "isa_tms320.tcc"
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
#line 9301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9311 "isa_tms320.tcc"
>
class OpMPYI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9320 "isa_tms320.tcc"
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
#line 9338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9348 "isa_tms320.tcc"
>
class OpMPYI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9357 "isa_tms320.tcc"
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
#line 9374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9384 "isa_tms320.tcc"
>
class OpNEGF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9393 "isa_tms320.tcc"
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
#line 9410 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9420 "isa_tms320.tcc"
>
class OpNEGF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9429 "isa_tms320.tcc"
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
#line 9445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9452 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9455 "isa_tms320.tcc"
>
class OpNEGI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9464 "isa_tms320.tcc"
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
#line 9481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9491 "isa_tms320.tcc"
>
class OpNEGI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9500 "isa_tms320.tcc"
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
#line 9516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9526 "isa_tms320.tcc"
>
class OpNOT_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9535 "isa_tms320.tcc"
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
#line 9552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9559 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9562 "isa_tms320.tcc"
>
class OpNOT_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9571 "isa_tms320.tcc"
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
#line 9587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9597 "isa_tms320.tcc"
>
class OpOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9606 "isa_tms320.tcc"
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
#line 9624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9634 "isa_tms320.tcc"
>
class OpOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9643 "isa_tms320.tcc"
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
#line 9660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9670 "isa_tms320.tcc"
>
class OpSTF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9679 "isa_tms320.tcc"
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
#line 9696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9706 "isa_tms320.tcc"
>
class OpSTF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9715 "isa_tms320.tcc"
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
#line 9731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9734 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9741 "isa_tms320.tcc"
>
class OpSTI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9750 "isa_tms320.tcc"
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
#line 9767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9777 "isa_tms320.tcc"
>
class OpSTI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9786 "isa_tms320.tcc"
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
#line 9802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9812 "isa_tms320.tcc"
>
class OpSUBF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9821 "isa_tms320.tcc"
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
#line 9839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9846 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9849 "isa_tms320.tcc"
>
class OpSUBF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9858 "isa_tms320.tcc"
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
#line 9875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9882 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9885 "isa_tms320.tcc"
>
class OpSUBI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9894 "isa_tms320.tcc"
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
#line 9912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9915 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9922 "isa_tms320.tcc"
>
class OpSUBI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9931 "isa_tms320.tcc"
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
#line 9948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9958 "isa_tms320.tcc"
>
class OpXOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9967 "isa_tms320.tcc"
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
#line 9985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9995 "isa_tms320.tcc"
>
class OpXOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10004 "isa_tms320.tcc"
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
#line 10021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10031 "isa_tms320.tcc"
>
class OpLDF_LDF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10036 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10040 "isa_tms320.tcc"
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
#line 10057 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10067 "isa_tms320.tcc"
>
class OpLDF_LDF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10072 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10076 "isa_tms320.tcc"
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
#line 10092 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10095 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10099 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10102 "isa_tms320.tcc"
>
class OpLDI_LDI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10111 "isa_tms320.tcc"
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
#line 10128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10135 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10138 "isa_tms320.tcc"
>
class OpLDI_LDI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10143 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10147 "isa_tms320.tcc"
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
#line 10163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10173 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10182 "isa_tms320.tcc"
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
#line 10202 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10209 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10212 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10217 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10221 "isa_tms320.tcc"
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
#line 10239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10249 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10258 "isa_tms320.tcc"
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
#line 10278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10288 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10297 "isa_tms320.tcc"
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
#line 10315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10325 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10334 "isa_tms320.tcc"
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
#line 10354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10364 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10373 "isa_tms320.tcc"
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
#line 10391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10401 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10410 "isa_tms320.tcc"
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
#line 10430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10437 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10440 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10449 "isa_tms320.tcc"
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
#line 10521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10531 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10540 "isa_tms320.tcc"
> *DecodeOpLDE_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10546 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10550 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10568 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10577 "isa_tms320.tcc"
> *DecodeOpLDE_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10583 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10587 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10598 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10602 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10605 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10610 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10614 "isa_tms320.tcc"
> *DecodeOpLDE_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10620 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10624 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10632 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10639 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10642 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10647 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10651 "isa_tms320.tcc"
> *DecodeOpLDE_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10657 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10661 "isa_tms320.tcc"
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
#line 10675 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10678 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10682 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10685 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10694 "isa_tms320.tcc"
> *DecodeOpLDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10700 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10704 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10719 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10722 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10731 "isa_tms320.tcc"
> *DecodeOpLDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10737 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10741 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10752 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10759 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10768 "isa_tms320.tcc"
> *DecodeOpLDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10774 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10778 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10796 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10805 "isa_tms320.tcc"
> *DecodeOpLDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10811 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10815 "isa_tms320.tcc"
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
#line 10829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10839 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10848 "isa_tms320.tcc"
> *DecodeOpLDFcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10854 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10858 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10866 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10876 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10885 "isa_tms320.tcc"
> *DecodeOpLDFcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10891 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10895 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10903 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10910 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10913 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10918 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10922 "isa_tms320.tcc"
> *DecodeOpLDFcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10928 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10932 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 10940 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10947 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10950 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10959 "isa_tms320.tcc"
> *DecodeOpLDFcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 10965 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 10969 "isa_tms320.tcc"
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
#line 10983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10993 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11002 "isa_tms320.tcc"
> *DecodeOpLDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11008 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11012 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11020 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11030 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11039 "isa_tms320.tcc"
> *DecodeOpLDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11045 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11049 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11057 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11067 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11072 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11076 "isa_tms320.tcc"
> *DecodeOpLDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11082 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11086 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11094 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11104 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11113 "isa_tms320.tcc"
> *DecodeOpLDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11119 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11123 "isa_tms320.tcc"
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
#line 11137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11147 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11156 "isa_tms320.tcc"
> *DecodeOpLDIcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11162 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11166 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11174 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11184 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11193 "isa_tms320.tcc"
> *DecodeOpLDIcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11199 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11203 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11221 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11230 "isa_tms320.tcc"
> *DecodeOpLDIcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11236 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11240 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11255 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11258 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11267 "isa_tms320.tcc"
> *DecodeOpLDIcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11273 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11277 "isa_tms320.tcc"
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
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11310 "isa_tms320.tcc"
> *DecodeOpLDM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11316 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11320 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11338 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11347 "isa_tms320.tcc"
> *DecodeOpLDM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11353 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11357 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11372 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11375 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11384 "isa_tms320.tcc"
> *DecodeOpLDM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11390 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11394 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11409 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11412 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11421 "isa_tms320.tcc"
> *DecodeOpLDM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11427 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11431 "isa_tms320.tcc"
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
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11464 "isa_tms320.tcc"
> *DecodeOpLDP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11470 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11474 "isa_tms320.tcc"
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
#line 11488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11498 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11507 "isa_tms320.tcc"
> *DecodeOpPOP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11513 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11517 "isa_tms320.tcc"
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
#line 11531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11541 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11550 "isa_tms320.tcc"
> *DecodeOpPOPF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11556 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11560 "isa_tms320.tcc"
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
#line 11574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11584 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11593 "isa_tms320.tcc"
> *DecodeOpPUSH(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11599 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11603 "isa_tms320.tcc"
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
#line 11617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11627 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11636 "isa_tms320.tcc"
> *DecodeOpPUSHF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11642 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11646 "isa_tms320.tcc"
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
#line 11660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11670 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11679 "isa_tms320.tcc"
> *DecodeOpSTF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11685 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11689 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11697 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11707 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11716 "isa_tms320.tcc"
> *DecodeOpSTF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11722 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11726 "isa_tms320.tcc"
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
#line 11740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11747 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11750 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11755 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11759 "isa_tms320.tcc"
> *DecodeOpSTI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11765 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11769 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11777 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11784 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11787 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11792 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11796 "isa_tms320.tcc"
> *DecodeOpSTI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11802 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11806 "isa_tms320.tcc"
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
#line 11891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11901 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11910 "isa_tms320.tcc"
> *DecodeOpABSF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11916 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11920 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11938 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11947 "isa_tms320.tcc"
> *DecodeOpABSF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11957 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11975 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11980 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11984 "isa_tms320.tcc"
> *DecodeOpABSF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11990 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11994 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12005 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12009 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12012 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12021 "isa_tms320.tcc"
> *DecodeOpABSF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12027 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12031 "isa_tms320.tcc"
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
#line 12045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12055 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12064 "isa_tms320.tcc"
> *DecodeOpABSI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12070 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12074 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12092 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12101 "isa_tms320.tcc"
> *DecodeOpABSI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12107 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12111 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12129 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12138 "isa_tms320.tcc"
> *DecodeOpABSI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12144 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12148 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12166 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12175 "isa_tms320.tcc"
> *DecodeOpABSI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12181 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12185 "isa_tms320.tcc"
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
#line 12199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12209 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12218 "isa_tms320.tcc"
> *DecodeOpADDC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12224 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12228 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12246 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12255 "isa_tms320.tcc"
> *DecodeOpADDC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12265 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12283 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12292 "isa_tms320.tcc"
> *DecodeOpADDC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12298 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12302 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12320 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12329 "isa_tms320.tcc"
> *DecodeOpADDC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12335 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12339 "isa_tms320.tcc"
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
#line 12353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12363 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12372 "isa_tms320.tcc"
> *DecodeOpADDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12378 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12382 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12397 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12400 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12409 "isa_tms320.tcc"
> *DecodeOpADDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12415 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12419 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12434 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12437 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12446 "isa_tms320.tcc"
> *DecodeOpADDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12452 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12456 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12474 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12483 "isa_tms320.tcc"
> *DecodeOpADDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12489 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12493 "isa_tms320.tcc"
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
#line 12507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12517 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12526 "isa_tms320.tcc"
> *DecodeOpADDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12532 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12536 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12551 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12554 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12563 "isa_tms320.tcc"
> *DecodeOpADDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12569 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12573 "isa_tms320.tcc"
	>(code, addr);
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
> *DecodeOpADDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_indir<
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
#line 12618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12628 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12637 "isa_tms320.tcc"
> *DecodeOpADDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12643 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12647 "isa_tms320.tcc"
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
#line 12661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12671 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12680 "isa_tms320.tcc"
> *DecodeOpAND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12686 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12690 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12708 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12717 "isa_tms320.tcc"
> *DecodeOpAND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12727 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12745 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12754 "isa_tms320.tcc"
> *DecodeOpAND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12760 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12764 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12782 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12791 "isa_tms320.tcc"
> *DecodeOpAND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12797 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12801 "isa_tms320.tcc"
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
#line 12815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12825 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12830 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12834 "isa_tms320.tcc"
> *DecodeOpANDN_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12840 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12844 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12862 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12871 "isa_tms320.tcc"
> *DecodeOpANDN_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12877 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12881 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12899 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12908 "isa_tms320.tcc"
> *DecodeOpANDN_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12914 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12918 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12936 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12945 "isa_tms320.tcc"
> *DecodeOpANDN_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12951 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12955 "isa_tms320.tcc"
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
#line 12969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12972 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12976 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12979 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12984 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12988 "isa_tms320.tcc"
> *DecodeOpASH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12994 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12998 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13013 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13016 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13025 "isa_tms320.tcc"
> *DecodeOpASH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13031 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13035 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13053 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13062 "isa_tms320.tcc"
> *DecodeOpASH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13068 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13072 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13083 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13087 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13090 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13095 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13099 "isa_tms320.tcc"
> *DecodeOpASH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13105 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13109 "isa_tms320.tcc"
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
#line 13123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13133 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13142 "isa_tms320.tcc"
> *DecodeOpCMPF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13148 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13152 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13160 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13167 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13170 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13175 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13179 "isa_tms320.tcc"
> *DecodeOpCMPF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13185 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13189 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13207 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13216 "isa_tms320.tcc"
> *DecodeOpCMPF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13222 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13226 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13244 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13253 "isa_tms320.tcc"
> *DecodeOpCMPF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13259 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13263 "isa_tms320.tcc"
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
#line 13277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13287 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13296 "isa_tms320.tcc"
> *DecodeOpCMPI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13302 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13306 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13324 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13333 "isa_tms320.tcc"
> *DecodeOpCMPI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13339 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13343 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13361 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13370 "isa_tms320.tcc"
> *DecodeOpCMPI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13376 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13380 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13398 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13407 "isa_tms320.tcc"
> *DecodeOpCMPI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13417 "isa_tms320.tcc"
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
#line 13431 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13441 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13450 "isa_tms320.tcc"
> *DecodeOpFIX_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13456 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13460 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13475 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13478 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13487 "isa_tms320.tcc"
> *DecodeOpFIX_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13493 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13497 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13515 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13520 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13524 "isa_tms320.tcc"
> *DecodeOpFIX_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13530 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13534 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13552 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13557 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13561 "isa_tms320.tcc"
> *DecodeOpFIX_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13567 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13571 "isa_tms320.tcc"
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
#line 13585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13595 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13604 "isa_tms320.tcc"
> *DecodeOpFLOAT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13610 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13614 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13632 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13641 "isa_tms320.tcc"
> *DecodeOpFLOAT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13647 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13651 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13659 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13666 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13669 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13674 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13678 "isa_tms320.tcc"
> *DecodeOpFLOAT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13684 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13688 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13706 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13715 "isa_tms320.tcc"
> *DecodeOpFLOAT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13721 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13725 "isa_tms320.tcc"
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
#line 13739 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13742 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13746 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13749 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13754 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13758 "isa_tms320.tcc"
> *DecodeOpLSH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13764 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13768 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13783 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13786 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13795 "isa_tms320.tcc"
> *DecodeOpLSH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13801 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13805 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13813 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13820 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13823 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13832 "isa_tms320.tcc"
> *DecodeOpLSH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13838 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13842 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13860 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13869 "isa_tms320.tcc"
> *DecodeOpLSH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13875 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13879 "isa_tms320.tcc"
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
#line 13893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13896 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13900 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13903 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13912 "isa_tms320.tcc"
> *DecodeOpMPYF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13918 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13922 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13937 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13940 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13949 "isa_tms320.tcc"
> *DecodeOpMPYF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13955 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13959 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13977 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13986 "isa_tms320.tcc"
> *DecodeOpMPYF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13992 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13996 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14014 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14023 "isa_tms320.tcc"
> *DecodeOpMPYF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14029 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14033 "isa_tms320.tcc"
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
#line 14047 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14050 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14054 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14057 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14066 "isa_tms320.tcc"
> *DecodeOpMPYI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14072 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14076 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14094 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14099 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14103 "isa_tms320.tcc"
> *DecodeOpMPYI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14109 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14113 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14131 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14140 "isa_tms320.tcc"
> *DecodeOpMPYI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14146 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14150 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14168 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14177 "isa_tms320.tcc"
> *DecodeOpMPYI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14183 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14187 "isa_tms320.tcc"
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
#line 14201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14211 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14220 "isa_tms320.tcc"
> *DecodeOpNEGB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14226 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14230 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14241 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14245 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14248 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14257 "isa_tms320.tcc"
> *DecodeOpNEGB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14263 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14267 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14285 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14294 "isa_tms320.tcc"
> *DecodeOpNEGB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14300 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14304 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14322 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14331 "isa_tms320.tcc"
> *DecodeOpNEGB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14337 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14341 "isa_tms320.tcc"
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
#line 14355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14362 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14365 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14374 "isa_tms320.tcc"
> *DecodeOpNEGF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14380 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14384 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14392 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14399 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14402 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14411 "isa_tms320.tcc"
> *DecodeOpNEGF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14417 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14421 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14429 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14439 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14448 "isa_tms320.tcc"
> *DecodeOpNEGF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14454 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14458 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14476 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14485 "isa_tms320.tcc"
> *DecodeOpNEGF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14491 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14495 "isa_tms320.tcc"
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
#line 14509 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14512 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14519 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14528 "isa_tms320.tcc"
> *DecodeOpNEGI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14534 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14538 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14546 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14549 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14556 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14565 "isa_tms320.tcc"
> *DecodeOpNEGI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14571 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14575 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14590 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14593 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14598 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14602 "isa_tms320.tcc"
> *DecodeOpNEGI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14608 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14612 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14620 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14627 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14630 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14639 "isa_tms320.tcc"
> *DecodeOpNEGI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14645 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14649 "isa_tms320.tcc"
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
#line 14663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14670 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14673 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14678 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14682 "isa_tms320.tcc"
> *DecodeOpNORM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14688 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14692 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14700 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14707 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14710 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14719 "isa_tms320.tcc"
> *DecodeOpNORM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14725 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14729 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14744 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14747 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14752 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14756 "isa_tms320.tcc"
> *DecodeOpNORM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14762 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14766 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14781 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14784 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14793 "isa_tms320.tcc"
> *DecodeOpNORM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14799 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14803 "isa_tms320.tcc"
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
#line 14817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14824 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14827 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14836 "isa_tms320.tcc"
> *DecodeOpNOT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14842 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14846 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14854 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14864 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14873 "isa_tms320.tcc"
> *DecodeOpNOT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14879 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14883 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14901 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14910 "isa_tms320.tcc"
> *DecodeOpNOT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14916 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14920 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14938 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14947 "isa_tms320.tcc"
> *DecodeOpNOT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14957 "isa_tms320.tcc"
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
#line 14971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14981 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14990 "isa_tms320.tcc"
> *DecodeOpOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14996 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15000 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15018 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15027 "isa_tms320.tcc"
> *DecodeOpOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15033 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15037 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15055 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15064 "isa_tms320.tcc"
> *DecodeOpOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15070 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15074 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15092 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15101 "isa_tms320.tcc"
> *DecodeOpOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15107 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15111 "isa_tms320.tcc"
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
#line 15125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15135 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15144 "isa_tms320.tcc"
> *DecodeOpRND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15150 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15154 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15172 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15181 "isa_tms320.tcc"
> *DecodeOpRND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15187 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15191 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15209 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15218 "isa_tms320.tcc"
> *DecodeOpRND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15224 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15228 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15246 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15255 "isa_tms320.tcc"
> *DecodeOpRND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15265 "isa_tms320.tcc"
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
#line 15279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15286 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15289 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15298 "isa_tms320.tcc"
> *DecodeOpROL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15304 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15308 "isa_tms320.tcc"
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
#line 15322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15329 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15332 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15341 "isa_tms320.tcc"
> *DecodeOpROLC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15347 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15351 "isa_tms320.tcc"
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
#line 15365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15372 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15375 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15384 "isa_tms320.tcc"
> *DecodeOpROR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15390 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15394 "isa_tms320.tcc"
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
#line 15408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15418 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15427 "isa_tms320.tcc"
> *DecodeOpRORC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15433 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15437 "isa_tms320.tcc"
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
#line 15451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15461 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15470 "isa_tms320.tcc"
> *DecodeOpSUBB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15476 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15480 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15498 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15507 "isa_tms320.tcc"
> *DecodeOpSUBB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15513 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15517 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15525 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15535 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15544 "isa_tms320.tcc"
> *DecodeOpSUBB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15550 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15554 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15562 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15572 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15581 "isa_tms320.tcc"
> *DecodeOpSUBB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15587 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15591 "isa_tms320.tcc"
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
#line 15605 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15608 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15612 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15615 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15624 "isa_tms320.tcc"
> *DecodeOpSUBC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15630 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15634 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15642 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15645 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15652 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15661 "isa_tms320.tcc"
> *DecodeOpSUBC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15667 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15671 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15679 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15689 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15698 "isa_tms320.tcc"
> *DecodeOpSUBC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15704 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15708 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15716 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15726 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15735 "isa_tms320.tcc"
> *DecodeOpSUBC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15741 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15745 "isa_tms320.tcc"
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
#line 15759 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15766 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15769 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15778 "isa_tms320.tcc"
> *DecodeOpSUBF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15784 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15788 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15806 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15815 "isa_tms320.tcc"
> *DecodeOpSUBF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15821 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15825 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15843 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15852 "isa_tms320.tcc"
> *DecodeOpSUBF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15858 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15862 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15880 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15889 "isa_tms320.tcc"
> *DecodeOpSUBF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15895 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15899 "isa_tms320.tcc"
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
#line 15913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15920 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15923 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15932 "isa_tms320.tcc"
> *DecodeOpSUBI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15938 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15942 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15953 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15957 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15960 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15969 "isa_tms320.tcc"
> *DecodeOpSUBI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15975 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15979 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15997 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16006 "isa_tms320.tcc"
> *DecodeOpSUBI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16012 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16016 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16031 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16034 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16043 "isa_tms320.tcc"
> *DecodeOpSUBI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16049 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16053 "isa_tms320.tcc"
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
#line 16067 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16077 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16086 "isa_tms320.tcc"
> *DecodeOpSUBRB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16092 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16096 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16104 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16111 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16114 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16123 "isa_tms320.tcc"
> *DecodeOpSUBRB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16129 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16133 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16151 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16160 "isa_tms320.tcc"
> *DecodeOpSUBRB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16166 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16170 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16188 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16197 "isa_tms320.tcc"
> *DecodeOpSUBRB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16203 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16207 "isa_tms320.tcc"
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
#line 16221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16231 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16240 "isa_tms320.tcc"
> *DecodeOpSUBRF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16246 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16250 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16268 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16277 "isa_tms320.tcc"
> *DecodeOpSUBRF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16283 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16287 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16298 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16305 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16314 "isa_tms320.tcc"
> *DecodeOpSUBRF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16320 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16324 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16335 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16339 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16342 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16351 "isa_tms320.tcc"
> *DecodeOpSUBRF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16357 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16361 "isa_tms320.tcc"
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
#line 16375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16385 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16394 "isa_tms320.tcc"
> *DecodeOpSUBRI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16400 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16404 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16422 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16431 "isa_tms320.tcc"
> *DecodeOpSUBRI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16437 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16441 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16449 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16459 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16468 "isa_tms320.tcc"
> *DecodeOpSUBRI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16474 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16478 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16486 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16489 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16493 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16496 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16505 "isa_tms320.tcc"
> *DecodeOpSUBRI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16511 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16515 "isa_tms320.tcc"
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
#line 16529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16539 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16548 "isa_tms320.tcc"
> *DecodeOpTSTB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16554 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16558 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16566 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16569 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16576 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16585 "isa_tms320.tcc"
> *DecodeOpTSTB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16591 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16595 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16603 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16613 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16618 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16622 "isa_tms320.tcc"
> *DecodeOpTSTB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16628 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16632 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16643 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16650 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16659 "isa_tms320.tcc"
> *DecodeOpTSTB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16665 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16669 "isa_tms320.tcc"
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
#line 16683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16693 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16702 "isa_tms320.tcc"
> *DecodeOpXOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16708 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16712 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16723 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16730 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16739 "isa_tms320.tcc"
> *DecodeOpXOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16745 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16749 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16767 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16776 "isa_tms320.tcc"
> *DecodeOpXOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16782 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16786 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16794 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16797 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16801 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16804 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16813 "isa_tms320.tcc"
> *DecodeOpXOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16819 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16823 "isa_tms320.tcc"
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
#line 16890 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16897 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16900 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16909 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16915 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16919 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16930 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16937 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16946 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16952 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16956 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16974 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16983 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16989 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16993 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17011 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17020 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17026 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17030 "isa_tms320.tcc"
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
#line 17044 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17054 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17063 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17069 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17073 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17081 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17084 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17091 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17100 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17106 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17110 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17118 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17128 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17137 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17143 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17147 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17165 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17174 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17180 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17184 "isa_tms320.tcc"
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
#line 17198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17205 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17208 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17217 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17223 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17227 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17242 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17245 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17254 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17260 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17264 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17282 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17291 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17297 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17301 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17312 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17319 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17328 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17334 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17338 "isa_tms320.tcc"
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
#line 17352 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17362 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17367 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17371 "isa_tms320.tcc"
> *DecodeOpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17377 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17381 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17399 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17408 "isa_tms320.tcc"
> *DecodeOpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17414 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17418 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17436 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17445 "isa_tms320.tcc"
> *DecodeOpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17451 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17455 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17470 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17473 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17482 "isa_tms320.tcc"
> *DecodeOpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17488 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17492 "isa_tms320.tcc"
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
#line 17506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17516 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17521 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17525 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17531 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17535 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17553 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17562 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17568 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17572 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17590 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17599 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17605 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17609 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17627 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17636 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17642 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17646 "isa_tms320.tcc"
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
#line 17660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17670 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17679 "isa_tms320.tcc"
> *DecodeOpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17685 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17689 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17697 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17707 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17716 "isa_tms320.tcc"
> *DecodeOpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17722 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17726 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17744 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17753 "isa_tms320.tcc"
> *DecodeOpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17759 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17763 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17781 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17790 "isa_tms320.tcc"
> *DecodeOpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17800 "isa_tms320.tcc"
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
#line 17814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17824 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17833 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17839 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17843 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17861 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17870 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17880 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17898 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17907 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17913 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17917 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17935 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17944 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17950 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17954 "isa_tms320.tcc"
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
#line 17968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17978 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17987 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17993 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17997 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18005 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18012 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18015 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18024 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18030 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18034 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18042 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18052 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18057 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18061 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18067 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18071 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18079 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18089 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18098 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18104 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18108 "isa_tms320.tcc"
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
#line 18122 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18129 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18132 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18141 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18147 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18151 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18159 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18162 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18166 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18169 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18178 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18184 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18188 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18199 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18203 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18206 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18215 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18221 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18225 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18233 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18243 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18248 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18252 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18258 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18262 "isa_tms320.tcc"
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
#line 18276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18283 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18286 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18295 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18301 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18305 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18320 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18323 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18332 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18338 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18342 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18350 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18357 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18360 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18365 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18369 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18375 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18379 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18397 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18406 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18412 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18416 "isa_tms320.tcc"
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
#line 18430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18437 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18440 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18449 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18455 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18459 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18477 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18486 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18492 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18496 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18514 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18523 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18529 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18533 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18548 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18551 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18560 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18566 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18570 "isa_tms320.tcc"
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
#line 18584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18591 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18594 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18603 "isa_tms320.tcc"
> *DecodeOpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18609 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18613 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18628 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18631 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18640 "isa_tms320.tcc"
> *DecodeOpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18646 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18650 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18668 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18677 "isa_tms320.tcc"
> *DecodeOpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18683 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18687 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18705 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18714 "isa_tms320.tcc"
> *DecodeOpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18720 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18724 "isa_tms320.tcc"
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
#line 18738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18748 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18757 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18763 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18767 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18785 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18794 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18800 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18804 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18822 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18831 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18837 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18841 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18856 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18859 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18868 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18874 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18878 "isa_tms320.tcc"
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
#line 18892 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18899 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18902 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18911 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18917 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18921 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18936 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18939 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18948 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18954 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18958 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18966 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18969 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18973 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18976 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18985 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18991 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18995 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19010 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19013 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19022 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19028 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19032 "isa_tms320.tcc"
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
#line 19046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19056 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19065 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19071 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19075 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19083 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19086 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19090 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19093 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19102 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19108 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19112 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19123 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19130 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19139 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19145 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19149 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19167 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19176 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19186 "isa_tms320.tcc"
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
#line 19200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19207 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19210 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19219 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19225 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19229 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19237 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19247 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19256 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19262 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19266 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19284 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19293 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19299 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19303 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19321 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19330 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19336 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19340 "isa_tms320.tcc"
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
#line 19354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19364 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19373 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19379 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19383 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19401 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19410 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19416 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19420 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19438 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19447 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19453 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19457 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19472 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19475 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19484 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19494 "isa_tms320.tcc"
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
#line 19561 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19571 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19580 "isa_tms320.tcc"
> *DecodeOpBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19586 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19590 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19605 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19608 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19617 "isa_tms320.tcc"
> *DecodeOpBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19623 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19627 "isa_tms320.tcc"
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
#line 19641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19651 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19660 "isa_tms320.tcc"
> *DecodeOpBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19666 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19670 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19678 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19688 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19693 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19697 "isa_tms320.tcc"
> *DecodeOpBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19703 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19707 "isa_tms320.tcc"
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
> *DecodeOpBR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19746 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19750 "isa_tms320.tcc"
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
> *DecodeOpBRD(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19789 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19793 "isa_tms320.tcc"
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
#line 19807 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19817 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19826 "isa_tms320.tcc"
> *DecodeOpCALL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19832 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19836 "isa_tms320.tcc"
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
#line 19850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19860 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19869 "isa_tms320.tcc"
> *DecodeOpCALLcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19875 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19879 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19887 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19894 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19897 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19902 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19906 "isa_tms320.tcc"
> *DecodeOpCALLcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19912 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19916 "isa_tms320.tcc"
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
#line 19930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19937 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19940 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19949 "isa_tms320.tcc"
> *DecodeOpDBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19955 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19959 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19977 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19986 "isa_tms320.tcc"
> *DecodeOpDBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19992 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19996 "isa_tms320.tcc"
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
#line 20010 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20013 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20017 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20020 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20025 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20029 "isa_tms320.tcc"
> *DecodeOpDBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20035 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20039 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20047 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20050 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20054 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20057 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20066 "isa_tms320.tcc"
> *DecodeOpDBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20072 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20076 "isa_tms320.tcc"
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
#line 20090 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20097 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20100 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20105 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20109 "isa_tms320.tcc"
> *DecodeOpIACK_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20115 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20119 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20127 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20134 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20137 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20146 "isa_tms320.tcc"
> *DecodeOpIACK_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20152 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20156 "isa_tms320.tcc"
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
#line 20170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20177 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20180 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20189 "isa_tms320.tcc"
> *DecodeOpIDLE(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20195 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20199 "isa_tms320.tcc"
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
#line 20213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20220 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20223 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20232 "isa_tms320.tcc"
> *DecodeOpNOP_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20238 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20242 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20250 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20257 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20260 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20265 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20269 "isa_tms320.tcc"
> *DecodeOpNOP_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20275 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20279 "isa_tms320.tcc"
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
#line 20293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20303 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20312 "isa_tms320.tcc"
> *DecodeOpRETIcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20318 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20322 "isa_tms320.tcc"
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
#line 20336 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20339 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20346 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20355 "isa_tms320.tcc"
> *DecodeOpRETScond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20361 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20365 "isa_tms320.tcc"
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
#line 20379 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20389 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20398 "isa_tms320.tcc"
> *DecodeOpRPTB(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20404 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20408 "isa_tms320.tcc"
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
#line 20422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20429 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20432 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20441 "isa_tms320.tcc"
> *DecodeOpRPTS_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20447 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20451 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20469 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20478 "isa_tms320.tcc"
> *DecodeOpRPTS_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20484 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20488 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20506 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20515 "isa_tms320.tcc"
> *DecodeOpRPTS_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20521 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20525 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20543 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20552 "isa_tms320.tcc"
> *DecodeOpRPTS_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20562 "isa_tms320.tcc"
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
#line 20576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20586 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20595 "isa_tms320.tcc"
> *DecodeOpSWI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20605 "isa_tms320.tcc"
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
#line 20619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20629 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20638 "isa_tms320.tcc"
> *DecodeOpTRAPcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20644 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20648 "isa_tms320.tcc"
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
#line 20701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20711 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20720 "isa_tms320.tcc"
> *DecodeOpIDLE2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20726 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20730 "isa_tms320.tcc"
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
#line 20744 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20754 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20763 "isa_tms320.tcc"
> *DecodeOpLOPOWER(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20769 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20773 "isa_tms320.tcc"
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
#line 20787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20794 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20797 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20806 "isa_tms320.tcc"
> *DecodeOpMAXSPEED(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20812 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20816 "isa_tms320.tcc"
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
#line 20871 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20878 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20881 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20890 "isa_tms320.tcc"
> *DecodeOpLDFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20896 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20900 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20918 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20927 "isa_tms320.tcc"
> *DecodeOpLDFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20933 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20937 "isa_tms320.tcc"
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
#line 20951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20961 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20970 "isa_tms320.tcc"
> *DecodeOpLDII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20976 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20980 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20998 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21007 "isa_tms320.tcc"
> *DecodeOpLDII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21013 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21017 "isa_tms320.tcc"
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
#line 21031 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21034 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21041 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21050 "isa_tms320.tcc"
> *DecodeOpSIGI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21056 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21060 "isa_tms320.tcc"
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
> *DecodeOpSTFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_dir<
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
> *DecodeOpSTFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21136 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21140 "isa_tms320.tcc"
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
> *DecodeOpSTII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21179 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21183 "isa_tms320.tcc"
	>(code, addr);
}


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
> *DecodeOpSTII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21216 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21220 "isa_tms320.tcc"
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
#line 21291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21301 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21310 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21316 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21320 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21338 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21347 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21353 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21357 "isa_tms320.tcc"
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
#line 21371 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21381 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21390 "isa_tms320.tcc"
> *DecodeOpLDF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21396 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21400 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21418 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21427 "isa_tms320.tcc"
> *DecodeOpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21433 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21437 "isa_tms320.tcc"
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
#line 21451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21461 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21470 "isa_tms320.tcc"
> *DecodeOpLDI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21476 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21480 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21498 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21507 "isa_tms320.tcc"
> *DecodeOpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21513 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21517 "isa_tms320.tcc"
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
#line 21531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21541 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21550 "isa_tms320.tcc"
> *DecodeOpLSH3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21556 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21560 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21578 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21587 "isa_tms320.tcc"
> *DecodeOpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21593 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21597 "isa_tms320.tcc"
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
#line 21611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21621 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21630 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21636 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21640 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21658 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21667 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21673 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21677 "isa_tms320.tcc"
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
#line 21691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21701 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21710 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21716 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21720 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21738 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21747 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21753 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21757 "isa_tms320.tcc"
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
#line 21771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21781 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21790 "isa_tms320.tcc"
> *DecodeOpNEGF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21800 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21818 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21827 "isa_tms320.tcc"
> *DecodeOpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21837 "isa_tms320.tcc"
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
#line 21851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21858 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21861 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21870 "isa_tms320.tcc"
> *DecodeOpNEGI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21880 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21895 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21898 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21907 "isa_tms320.tcc"
> *DecodeOpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21913 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21917 "isa_tms320.tcc"
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
#line 21931 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21941 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21950 "isa_tms320.tcc"
> *DecodeOpNOT_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21956 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21960 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21978 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21987 "isa_tms320.tcc"
> *DecodeOpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21993 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21997 "isa_tms320.tcc"
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
#line 22011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22021 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22030 "isa_tms320.tcc"
> *DecodeOpOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22036 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22040 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22055 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22058 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22063 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22067 "isa_tms320.tcc"
> *DecodeOpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22073 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22077 "isa_tms320.tcc"
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
#line 22091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22101 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22110 "isa_tms320.tcc"
> *DecodeOpSTF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22116 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22120 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22135 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22138 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22143 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22147 "isa_tms320.tcc"
> *DecodeOpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22153 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22157 "isa_tms320.tcc"
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
#line 22171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22181 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22190 "isa_tms320.tcc"
> *DecodeOpSTI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22196 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22200 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22218 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22227 "isa_tms320.tcc"
> *DecodeOpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22233 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22237 "isa_tms320.tcc"
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
#line 22251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22261 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22270 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22276 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22280 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22288 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22298 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22307 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22313 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22317 "isa_tms320.tcc"
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
#line 22331 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22341 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22350 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22356 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22360 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22375 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22378 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22387 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22393 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22397 "isa_tms320.tcc"
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
#line 22411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22421 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22430 "isa_tms320.tcc"
> *DecodeOpXOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22436 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22440 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22458 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22467 "isa_tms320.tcc"
> *DecodeOpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22473 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22477 "isa_tms320.tcc"
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
#line 22491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22501 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22510 "isa_tms320.tcc"
> *DecodeOpLDF_LDF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22516 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22520 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22538 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22547 "isa_tms320.tcc"
> *DecodeOpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22557 "isa_tms320.tcc"
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
#line 22571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22581 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22590 "isa_tms320.tcc"
> *DecodeOpLDI_LDI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22600 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22618 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22627 "isa_tms320.tcc"
> *DecodeOpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22637 "isa_tms320.tcc"
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
#line 22651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22661 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22670 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22676 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22680 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22698 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22707 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22713 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22717 "isa_tms320.tcc"
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
#line 22731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22734 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22741 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22750 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22756 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22760 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22771 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22778 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22783 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22787 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22793 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22797 "isa_tms320.tcc"
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
#line 22811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22821 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22830 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22836 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22840 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22855 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22858 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22863 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22867 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22873 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22877 "isa_tms320.tcc"
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
#line 22891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22901 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22910 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22916 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22920 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22938 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22947 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22957 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 22964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22974 "isa_tms320.tcc"
>
OpLDE_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 22979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22983 "isa_tms320.tcc"
>::OpLDE_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22991 "isa_tms320.tcc"
>(code, addr, "LDE_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23004 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23011 "isa_tms320.tcc"
>
OpLDE_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23020 "isa_tms320.tcc"
>::OpLDE_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23028 "isa_tms320.tcc"
>(code, addr, "LDE_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23048 "isa_tms320.tcc"
>
OpLDE_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23057 "isa_tms320.tcc"
>::OpLDE_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23065 "isa_tms320.tcc"
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
#line 23077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23087 "isa_tms320.tcc"
>
OpLDE_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23096 "isa_tms320.tcc"
>::OpLDE_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23100 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23104 "isa_tms320.tcc"
>(code, addr, "LDE_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23124 "isa_tms320.tcc"
>
OpLDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23133 "isa_tms320.tcc"
>::OpLDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23141 "isa_tms320.tcc"
>(code, addr, "LDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23161 "isa_tms320.tcc"
>
OpLDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23170 "isa_tms320.tcc"
>::OpLDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23178 "isa_tms320.tcc"
>(code, addr, "LDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23198 "isa_tms320.tcc"
>
OpLDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23207 "isa_tms320.tcc"
>::OpLDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23211 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23215 "isa_tms320.tcc"
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
#line 23227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23237 "isa_tms320.tcc"
>
OpLDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23246 "isa_tms320.tcc"
>::OpLDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23254 "isa_tms320.tcc"
>(code, addr, "LDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23274 "isa_tms320.tcc"
>
OpLDFcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23283 "isa_tms320.tcc"
>::OpLDFcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23291 "isa_tms320.tcc"
>(code, addr, "LDFcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23312 "isa_tms320.tcc"
>
OpLDFcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23321 "isa_tms320.tcc"
>::OpLDFcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23329 "isa_tms320.tcc"
>(code, addr, "LDFcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23350 "isa_tms320.tcc"
>
OpLDFcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23359 "isa_tms320.tcc"
>::OpLDFcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23367 "isa_tms320.tcc"
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
#line 23380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23390 "isa_tms320.tcc"
>
OpLDFcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23399 "isa_tms320.tcc"
>::OpLDFcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23403 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23407 "isa_tms320.tcc"
>(code, addr, "LDFcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23421 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23428 "isa_tms320.tcc"
>
OpLDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23437 "isa_tms320.tcc"
>::OpLDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23445 "isa_tms320.tcc"
>(code, addr, "LDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23465 "isa_tms320.tcc"
>
OpLDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23474 "isa_tms320.tcc"
>::OpLDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23482 "isa_tms320.tcc"
>(code, addr, "LDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23495 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23502 "isa_tms320.tcc"
>
OpLDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23511 "isa_tms320.tcc"
>::OpLDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23519 "isa_tms320.tcc"
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
#line 23531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23541 "isa_tms320.tcc"
>
OpLDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23550 "isa_tms320.tcc"
>::OpLDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23558 "isa_tms320.tcc"
>(code, addr, "LDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23578 "isa_tms320.tcc"
>
OpLDIcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23587 "isa_tms320.tcc"
>::OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23595 "isa_tms320.tcc"
>(code, addr, "LDIcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23616 "isa_tms320.tcc"
>
OpLDIcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23625 "isa_tms320.tcc"
>::OpLDIcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23633 "isa_tms320.tcc"
>(code, addr, "LDIcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23644 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23647 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23654 "isa_tms320.tcc"
>
OpLDIcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23663 "isa_tms320.tcc"
>::OpLDIcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23671 "isa_tms320.tcc"
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
#line 23684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23687 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23694 "isa_tms320.tcc"
>
OpLDIcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23703 "isa_tms320.tcc"
>::OpLDIcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23711 "isa_tms320.tcc"
>(code, addr, "LDIcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23725 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23729 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23732 "isa_tms320.tcc"
>
OpLDM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 23737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23741 "isa_tms320.tcc"
>::OpLDM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23749 "isa_tms320.tcc"
>(code, addr, "LDM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23759 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23766 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23769 "isa_tms320.tcc"
>
OpLDM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23778 "isa_tms320.tcc"
>::OpLDM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23786 "isa_tms320.tcc"
>(code, addr, "LDM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23806 "isa_tms320.tcc"
>
OpLDM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 23811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23815 "isa_tms320.tcc"
>::OpLDM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23823 "isa_tms320.tcc"
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
#line 23835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23845 "isa_tms320.tcc"
>
OpLDM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 23850 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23854 "isa_tms320.tcc"
>::OpLDM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23862 "isa_tms320.tcc"
>(code, addr, "LDM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = ((code >> 0) & 0xfff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23882 "isa_tms320.tcc"
>
OpLDP<
#line 45 "isa/tms320.isa"
CONFIG
#line 23887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23891 "isa_tms320.tcc"
>::OpLDP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23899 "isa_tms320.tcc"
>(code, addr, "LDP")
{
	src = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23918 "isa_tms320.tcc"
>
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 23923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23927 "isa_tms320.tcc"
>::OpPOP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23935 "isa_tms320.tcc"
>(code, addr, "POP")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23944 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23947 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23954 "isa_tms320.tcc"
>
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 23959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23963 "isa_tms320.tcc"
>::OpPOPF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23971 "isa_tms320.tcc"
>(code, addr, "POPF")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 23980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23990 "isa_tms320.tcc"
>
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 23995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23999 "isa_tms320.tcc"
>::OpPUSH(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24007 "isa_tms320.tcc"
>(code, addr, "PUSH")
{
	src = ((code >> 16) & 0x1f);
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
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 24031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24035 "isa_tms320.tcc"
>::OpPUSHF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24043 "isa_tms320.tcc"
>(code, addr, "PUSHF")
{
	src = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24055 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24062 "isa_tms320.tcc"
>
OpSTF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24071 "isa_tms320.tcc"
>::OpSTF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24075 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24079 "isa_tms320.tcc"
>(code, addr, "STF_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24099 "isa_tms320.tcc"
>
OpSTF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24108 "isa_tms320.tcc"
>::OpSTF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24116 "isa_tms320.tcc"
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
#line 24128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24135 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24138 "isa_tms320.tcc"
>
OpSTI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24143 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24147 "isa_tms320.tcc"
>::OpSTI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24155 "isa_tms320.tcc"
>(code, addr, "STI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24172 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24175 "isa_tms320.tcc"
>
OpSTI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24184 "isa_tms320.tcc"
>::OpSTI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24192 "isa_tms320.tcc"
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
#line 24204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24214 "isa_tms320.tcc"
>
OpABSF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24223 "isa_tms320.tcc"
>::OpABSF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24231 "isa_tms320.tcc"
>(code, addr, "ABSF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24251 "isa_tms320.tcc"
>
OpABSF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24260 "isa_tms320.tcc"
>::OpABSF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24268 "isa_tms320.tcc"
>(code, addr, "ABSF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24288 "isa_tms320.tcc"
>
OpABSF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24297 "isa_tms320.tcc"
>::OpABSF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24305 "isa_tms320.tcc"
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
#line 24317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24327 "isa_tms320.tcc"
>
OpABSF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24332 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24336 "isa_tms320.tcc"
>::OpABSF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24344 "isa_tms320.tcc"
>(code, addr, "ABSF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
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
OpABSI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24373 "isa_tms320.tcc"
>::OpABSI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24381 "isa_tms320.tcc"
>(code, addr, "ABSI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24401 "isa_tms320.tcc"
>
OpABSI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24410 "isa_tms320.tcc"
>::OpABSI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24418 "isa_tms320.tcc"
>(code, addr, "ABSI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24438 "isa_tms320.tcc"
>
OpABSI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24447 "isa_tms320.tcc"
>::OpABSI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24455 "isa_tms320.tcc"
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
#line 24467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24477 "isa_tms320.tcc"
>
OpABSI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24486 "isa_tms320.tcc"
>::OpABSI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24494 "isa_tms320.tcc"
>(code, addr, "ABSI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24514 "isa_tms320.tcc"
>
OpADDC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24523 "isa_tms320.tcc"
>::OpADDC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24531 "isa_tms320.tcc"
>(code, addr, "ADDC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24548 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24551 "isa_tms320.tcc"
>
OpADDC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24560 "isa_tms320.tcc"
>::OpADDC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24568 "isa_tms320.tcc"
>(code, addr, "ADDC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24588 "isa_tms320.tcc"
>
OpADDC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24593 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24597 "isa_tms320.tcc"
>::OpADDC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24605 "isa_tms320.tcc"
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
#line 24617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24627 "isa_tms320.tcc"
>
OpADDC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24636 "isa_tms320.tcc"
>::OpADDC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24644 "isa_tms320.tcc"
>(code, addr, "ADDC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24664 "isa_tms320.tcc"
>
OpADDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24673 "isa_tms320.tcc"
>::OpADDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24681 "isa_tms320.tcc"
>(code, addr, "ADDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24701 "isa_tms320.tcc"
>
OpADDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24710 "isa_tms320.tcc"
>::OpADDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24718 "isa_tms320.tcc"
>(code, addr, "ADDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24738 "isa_tms320.tcc"
>
OpADDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24747 "isa_tms320.tcc"
>::OpADDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24755 "isa_tms320.tcc"
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
#line 24767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24777 "isa_tms320.tcc"
>
OpADDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24786 "isa_tms320.tcc"
>::OpADDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24794 "isa_tms320.tcc"
>(code, addr, "ADDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24807 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24814 "isa_tms320.tcc"
>
OpADDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24823 "isa_tms320.tcc"
>::OpADDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24831 "isa_tms320.tcc"
>(code, addr, "ADDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24851 "isa_tms320.tcc"
>
OpADDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24860 "isa_tms320.tcc"
>::OpADDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24868 "isa_tms320.tcc"
>(code, addr, "ADDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24878 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24888 "isa_tms320.tcc"
>
OpADDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 24893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24897 "isa_tms320.tcc"
>::OpADDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24905 "isa_tms320.tcc"
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
#line 24917 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24924 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24927 "isa_tms320.tcc"
>
OpADDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 24932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24936 "isa_tms320.tcc"
>::OpADDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24944 "isa_tms320.tcc"
>(code, addr, "ADDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24954 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24957 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24964 "isa_tms320.tcc"
>
OpAND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 24969 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24973 "isa_tms320.tcc"
>::OpAND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24981 "isa_tms320.tcc"
>(code, addr, "AND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 24991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25001 "isa_tms320.tcc"
>
OpAND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25010 "isa_tms320.tcc"
>::OpAND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25018 "isa_tms320.tcc"
>(code, addr, "AND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25028 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25038 "isa_tms320.tcc"
>
OpAND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25043 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25047 "isa_tms320.tcc"
>::OpAND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25055 "isa_tms320.tcc"
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
#line 25067 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25077 "isa_tms320.tcc"
>
OpAND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25086 "isa_tms320.tcc"
>::OpAND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25094 "isa_tms320.tcc"
>(code, addr, "AND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25104 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25111 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25114 "isa_tms320.tcc"
>
OpANDN_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25123 "isa_tms320.tcc"
>::OpANDN_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25131 "isa_tms320.tcc"
>(code, addr, "ANDN_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25151 "isa_tms320.tcc"
>
OpANDN_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25160 "isa_tms320.tcc"
>::OpANDN_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25168 "isa_tms320.tcc"
>(code, addr, "ANDN_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25185 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25188 "isa_tms320.tcc"
>
OpANDN_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25197 "isa_tms320.tcc"
>::OpANDN_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25205 "isa_tms320.tcc"
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
#line 25217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25220 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25224 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25227 "isa_tms320.tcc"
>
OpANDN_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25232 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25236 "isa_tms320.tcc"
>::OpANDN_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25244 "isa_tms320.tcc"
>(code, addr, "ANDN_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25254 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25257 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25261 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25264 "isa_tms320.tcc"
>
OpASH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25269 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25273 "isa_tms320.tcc"
>::OpASH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25281 "isa_tms320.tcc"
>(code, addr, "ASH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25301 "isa_tms320.tcc"
>
OpASH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25310 "isa_tms320.tcc"
>::OpASH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25318 "isa_tms320.tcc"
>(code, addr, "ASH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25338 "isa_tms320.tcc"
>
OpASH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25347 "isa_tms320.tcc"
>::OpASH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25355 "isa_tms320.tcc"
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
#line 25367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25374 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25377 "isa_tms320.tcc"
>
OpASH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25386 "isa_tms320.tcc"
>::OpASH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25394 "isa_tms320.tcc"
>(code, addr, "ASH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25404 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25414 "isa_tms320.tcc"
>
OpCMPF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25423 "isa_tms320.tcc"
>::OpCMPF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25431 "isa_tms320.tcc"
>(code, addr, "CMPF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25451 "isa_tms320.tcc"
>
OpCMPF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25460 "isa_tms320.tcc"
>::OpCMPF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25468 "isa_tms320.tcc"
>(code, addr, "CMPF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25478 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25485 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25488 "isa_tms320.tcc"
>
OpCMPF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25493 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25497 "isa_tms320.tcc"
>::OpCMPF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25505 "isa_tms320.tcc"
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
#line 25517 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25520 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25524 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25527 "isa_tms320.tcc"
>
OpCMPF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25536 "isa_tms320.tcc"
>::OpCMPF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25544 "isa_tms320.tcc"
>(code, addr, "CMPF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25557 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25561 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25564 "isa_tms320.tcc"
>
OpCMPI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25569 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25573 "isa_tms320.tcc"
>::OpCMPI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25581 "isa_tms320.tcc"
>(code, addr, "CMPI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25591 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25601 "isa_tms320.tcc"
>
OpCMPI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25610 "isa_tms320.tcc"
>::OpCMPI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25618 "isa_tms320.tcc"
>(code, addr, "CMPI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25628 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25631 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25635 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25638 "isa_tms320.tcc"
>
OpCMPI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25643 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25647 "isa_tms320.tcc"
>::OpCMPI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25655 "isa_tms320.tcc"
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
#line 25667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25670 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25677 "isa_tms320.tcc"
>
OpCMPI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25682 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25686 "isa_tms320.tcc"
>::OpCMPI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25694 "isa_tms320.tcc"
>(code, addr, "CMPI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25711 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25714 "isa_tms320.tcc"
>
OpFIX_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25723 "isa_tms320.tcc"
>::OpFIX_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25731 "isa_tms320.tcc"
>(code, addr, "FIX_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25751 "isa_tms320.tcc"
>
OpFIX_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25756 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25760 "isa_tms320.tcc"
>::OpFIX_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25768 "isa_tms320.tcc"
>(code, addr, "FIX_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25788 "isa_tms320.tcc"
>
OpFIX_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25797 "isa_tms320.tcc"
>::OpFIX_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25805 "isa_tms320.tcc"
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
#line 25817 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25824 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25827 "isa_tms320.tcc"
>
OpFIX_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25836 "isa_tms320.tcc"
>::OpFIX_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25844 "isa_tms320.tcc"
>(code, addr, "FIX_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25854 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25864 "isa_tms320.tcc"
>
OpFLOAT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25873 "isa_tms320.tcc"
>::OpFLOAT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25881 "isa_tms320.tcc"
>(code, addr, "FLOAT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25901 "isa_tms320.tcc"
>
OpFLOAT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25910 "isa_tms320.tcc"
>::OpFLOAT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25918 "isa_tms320.tcc"
>(code, addr, "FLOAT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25938 "isa_tms320.tcc"
>
OpFLOAT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25947 "isa_tms320.tcc"
>::OpFLOAT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25955 "isa_tms320.tcc"
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
#line 25967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25977 "isa_tms320.tcc"
>
OpFLOAT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25986 "isa_tms320.tcc"
>::OpFLOAT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25994 "isa_tms320.tcc"
>(code, addr, "FLOAT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26014 "isa_tms320.tcc"
>
OpLSH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26023 "isa_tms320.tcc"
>::OpLSH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26031 "isa_tms320.tcc"
>(code, addr, "LSH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26041 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26044 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26051 "isa_tms320.tcc"
>
OpLSH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26060 "isa_tms320.tcc"
>::OpLSH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26068 "isa_tms320.tcc"
>(code, addr, "LSH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26088 "isa_tms320.tcc"
>
OpLSH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26097 "isa_tms320.tcc"
>::OpLSH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26105 "isa_tms320.tcc"
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
#line 26117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26127 "isa_tms320.tcc"
>
OpLSH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26136 "isa_tms320.tcc"
>::OpLSH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26144 "isa_tms320.tcc"
>(code, addr, "LSH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26164 "isa_tms320.tcc"
>
OpMPYF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26173 "isa_tms320.tcc"
>::OpMPYF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26181 "isa_tms320.tcc"
>(code, addr, "MPYF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26201 "isa_tms320.tcc"
>
OpMPYF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26210 "isa_tms320.tcc"
>::OpMPYF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26218 "isa_tms320.tcc"
>(code, addr, "MPYF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26238 "isa_tms320.tcc"
>
OpMPYF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26247 "isa_tms320.tcc"
>::OpMPYF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26255 "isa_tms320.tcc"
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
#line 26267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26270 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26277 "isa_tms320.tcc"
>
OpMPYF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26286 "isa_tms320.tcc"
>::OpMPYF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26294 "isa_tms320.tcc"
>(code, addr, "MPYF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26304 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26314 "isa_tms320.tcc"
>
OpMPYI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26323 "isa_tms320.tcc"
>::OpMPYI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26331 "isa_tms320.tcc"
>(code, addr, "MPYI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26344 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26351 "isa_tms320.tcc"
>
OpMPYI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26360 "isa_tms320.tcc"
>::OpMPYI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26368 "isa_tms320.tcc"
>(code, addr, "MPYI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26388 "isa_tms320.tcc"
>
OpMPYI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26397 "isa_tms320.tcc"
>::OpMPYI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26405 "isa_tms320.tcc"
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
#line 26417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26424 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26427 "isa_tms320.tcc"
>
OpMPYI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26436 "isa_tms320.tcc"
>::OpMPYI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26440 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26444 "isa_tms320.tcc"
>(code, addr, "MPYI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26464 "isa_tms320.tcc"
>
OpNEGB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26473 "isa_tms320.tcc"
>::OpNEGB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26481 "isa_tms320.tcc"
>(code, addr, "NEGB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26501 "isa_tms320.tcc"
>
OpNEGB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26510 "isa_tms320.tcc"
>::OpNEGB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26518 "isa_tms320.tcc"
>(code, addr, "NEGB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26538 "isa_tms320.tcc"
>
OpNEGB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26547 "isa_tms320.tcc"
>::OpNEGB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26555 "isa_tms320.tcc"
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
#line 26567 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26577 "isa_tms320.tcc"
>
OpNEGB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26586 "isa_tms320.tcc"
>::OpNEGB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26594 "isa_tms320.tcc"
>(code, addr, "NEGB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26604 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26607 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26614 "isa_tms320.tcc"
>
OpNEGF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26623 "isa_tms320.tcc"
>::OpNEGF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26631 "isa_tms320.tcc"
>(code, addr, "NEGF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26651 "isa_tms320.tcc"
>
OpNEGF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26660 "isa_tms320.tcc"
>::OpNEGF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26668 "isa_tms320.tcc"
>(code, addr, "NEGF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26678 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26688 "isa_tms320.tcc"
>
OpNEGF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26693 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26697 "isa_tms320.tcc"
>::OpNEGF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26705 "isa_tms320.tcc"
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
#line 26717 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26720 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26727 "isa_tms320.tcc"
>
OpNEGF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26732 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26736 "isa_tms320.tcc"
>::OpNEGF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26744 "isa_tms320.tcc"
>(code, addr, "NEGF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26754 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26764 "isa_tms320.tcc"
>
OpNEGI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26769 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26773 "isa_tms320.tcc"
>::OpNEGI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26781 "isa_tms320.tcc"
>(code, addr, "NEGI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26791 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26794 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26801 "isa_tms320.tcc"
>
OpNEGI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26810 "isa_tms320.tcc"
>::OpNEGI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26818 "isa_tms320.tcc"
>(code, addr, "NEGI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26838 "isa_tms320.tcc"
>
OpNEGI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26847 "isa_tms320.tcc"
>::OpNEGI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26855 "isa_tms320.tcc"
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
#line 26867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26870 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26877 "isa_tms320.tcc"
>
OpNEGI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26886 "isa_tms320.tcc"
>::OpNEGI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26894 "isa_tms320.tcc"
>(code, addr, "NEGI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26914 "isa_tms320.tcc"
>
OpNORM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26923 "isa_tms320.tcc"
>::OpNORM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26931 "isa_tms320.tcc"
>(code, addr, "NORM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26951 "isa_tms320.tcc"
>
OpNORM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26960 "isa_tms320.tcc"
>::OpNORM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26968 "isa_tms320.tcc"
>(code, addr, "NORM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26988 "isa_tms320.tcc"
>
OpNORM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26997 "isa_tms320.tcc"
>::OpNORM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27005 "isa_tms320.tcc"
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
#line 27017 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27027 "isa_tms320.tcc"
>
OpNORM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27036 "isa_tms320.tcc"
>::OpNORM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27044 "isa_tms320.tcc"
>(code, addr, "NORM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27054 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27057 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27061 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27064 "isa_tms320.tcc"
>
OpNOT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27069 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27073 "isa_tms320.tcc"
>::OpNOT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27081 "isa_tms320.tcc"
>(code, addr, "NOT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27101 "isa_tms320.tcc"
>
OpNOT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27110 "isa_tms320.tcc"
>::OpNOT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27114 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27118 "isa_tms320.tcc"
>(code, addr, "NOT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27135 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27138 "isa_tms320.tcc"
>
OpNOT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27143 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27147 "isa_tms320.tcc"
>::OpNOT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27155 "isa_tms320.tcc"
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
#line 27167 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27174 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27177 "isa_tms320.tcc"
>
OpNOT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27182 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27186 "isa_tms320.tcc"
>::OpNOT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27194 "isa_tms320.tcc"
>(code, addr, "NOT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27214 "isa_tms320.tcc"
>
OpOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27223 "isa_tms320.tcc"
>::OpOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27231 "isa_tms320.tcc"
>(code, addr, "OR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27251 "isa_tms320.tcc"
>
OpOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27260 "isa_tms320.tcc"
>::OpOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27268 "isa_tms320.tcc"
>(code, addr, "OR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27288 "isa_tms320.tcc"
>
OpOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27297 "isa_tms320.tcc"
>::OpOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27305 "isa_tms320.tcc"
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
#line 27317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27327 "isa_tms320.tcc"
>
OpOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27332 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27336 "isa_tms320.tcc"
>::OpOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27344 "isa_tms320.tcc"
>(code, addr, "OR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27364 "isa_tms320.tcc"
>
OpRND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27373 "isa_tms320.tcc"
>::OpRND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27381 "isa_tms320.tcc"
>(code, addr, "RND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27401 "isa_tms320.tcc"
>
OpRND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27410 "isa_tms320.tcc"
>::OpRND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27418 "isa_tms320.tcc"
>(code, addr, "RND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27431 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27438 "isa_tms320.tcc"
>
OpRND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27447 "isa_tms320.tcc"
>::OpRND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27455 "isa_tms320.tcc"
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
#line 27467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27477 "isa_tms320.tcc"
>
OpRND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27486 "isa_tms320.tcc"
>::OpRND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27494 "isa_tms320.tcc"
>(code, addr, "RND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27514 "isa_tms320.tcc"
>
OpROL<
#line 45 "isa/tms320.isa"
CONFIG
#line 27519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27523 "isa_tms320.tcc"
>::OpROL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27531 "isa_tms320.tcc"
>(code, addr, "ROL")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27550 "isa_tms320.tcc"
>
OpROLC<
#line 45 "isa/tms320.isa"
CONFIG
#line 27555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27559 "isa_tms320.tcc"
>::OpROLC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27563 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27567 "isa_tms320.tcc"
>(code, addr, "ROLC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27586 "isa_tms320.tcc"
>
OpROR<
#line 45 "isa/tms320.isa"
CONFIG
#line 27591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27595 "isa_tms320.tcc"
>::OpROR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27603 "isa_tms320.tcc"
>(code, addr, "ROR")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27612 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27622 "isa_tms320.tcc"
>
OpRORC<
#line 45 "isa/tms320.isa"
CONFIG
#line 27627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27631 "isa_tms320.tcc"
>::OpRORC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27639 "isa_tms320.tcc"
>(code, addr, "RORC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27658 "isa_tms320.tcc"
>
OpSUBB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27667 "isa_tms320.tcc"
>::OpSUBB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27675 "isa_tms320.tcc"
>(code, addr, "SUBB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27695 "isa_tms320.tcc"
>
OpSUBB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27704 "isa_tms320.tcc"
>::OpSUBB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27712 "isa_tms320.tcc"
>(code, addr, "SUBB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27725 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27729 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27732 "isa_tms320.tcc"
>
OpSUBB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27741 "isa_tms320.tcc"
>::OpSUBB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27749 "isa_tms320.tcc"
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
#line 27761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27771 "isa_tms320.tcc"
>
OpSUBB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27780 "isa_tms320.tcc"
>::OpSUBB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27788 "isa_tms320.tcc"
>(code, addr, "SUBB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27808 "isa_tms320.tcc"
>
OpSUBC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27817 "isa_tms320.tcc"
>::OpSUBC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27825 "isa_tms320.tcc"
>(code, addr, "SUBC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27845 "isa_tms320.tcc"
>
OpSUBC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27850 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27854 "isa_tms320.tcc"
>::OpSUBC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27862 "isa_tms320.tcc"
>(code, addr, "SUBC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27882 "isa_tms320.tcc"
>
OpSUBC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27891 "isa_tms320.tcc"
>::OpSUBC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27899 "isa_tms320.tcc"
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
#line 27911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27918 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27921 "isa_tms320.tcc"
>
OpSUBC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27930 "isa_tms320.tcc"
>::OpSUBC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27938 "isa_tms320.tcc"
>(code, addr, "SUBC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27958 "isa_tms320.tcc"
>
OpSUBF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27967 "isa_tms320.tcc"
>::OpSUBF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27975 "isa_tms320.tcc"
>(code, addr, "SUBF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27995 "isa_tms320.tcc"
>
OpSUBF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28004 "isa_tms320.tcc"
>::OpSUBF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28012 "isa_tms320.tcc"
>(code, addr, "SUBF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
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
OpSUBF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28041 "isa_tms320.tcc"
>::OpSUBF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28049 "isa_tms320.tcc"
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
#line 28061 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28071 "isa_tms320.tcc"
>
OpSUBF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28080 "isa_tms320.tcc"
>::OpSUBF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28084 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28088 "isa_tms320.tcc"
>(code, addr, "SUBF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28108 "isa_tms320.tcc"
>
OpSUBI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28117 "isa_tms320.tcc"
>::OpSUBI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28125 "isa_tms320.tcc"
>(code, addr, "SUBI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28135 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28142 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28145 "isa_tms320.tcc"
>
OpSUBI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28150 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28154 "isa_tms320.tcc"
>::OpSUBI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28162 "isa_tms320.tcc"
>(code, addr, "SUBI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28172 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28175 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28179 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28182 "isa_tms320.tcc"
>
OpSUBI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28191 "isa_tms320.tcc"
>::OpSUBI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28199 "isa_tms320.tcc"
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
#line 28211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28221 "isa_tms320.tcc"
>
OpSUBI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28230 "isa_tms320.tcc"
>::OpSUBI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28238 "isa_tms320.tcc"
>(code, addr, "SUBI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28255 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28258 "isa_tms320.tcc"
>
OpSUBRB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28267 "isa_tms320.tcc"
>::OpSUBRB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28275 "isa_tms320.tcc"
>(code, addr, "SUBRB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28285 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28292 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28295 "isa_tms320.tcc"
>
OpSUBRB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28304 "isa_tms320.tcc"
>::OpSUBRB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28312 "isa_tms320.tcc"
>(code, addr, "SUBRB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28329 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28332 "isa_tms320.tcc"
>
OpSUBRB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28341 "isa_tms320.tcc"
>::OpSUBRB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28345 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28349 "isa_tms320.tcc"
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
#line 28361 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28368 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28371 "isa_tms320.tcc"
>
OpSUBRB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28380 "isa_tms320.tcc"
>::OpSUBRB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28388 "isa_tms320.tcc"
>(code, addr, "SUBRB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28405 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28408 "isa_tms320.tcc"
>
OpSUBRF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28417 "isa_tms320.tcc"
>::OpSUBRF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28421 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28425 "isa_tms320.tcc"
>(code, addr, "SUBRF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28435 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28442 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28445 "isa_tms320.tcc"
>
OpSUBRF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28454 "isa_tms320.tcc"
>::OpSUBRF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28462 "isa_tms320.tcc"
>(code, addr, "SUBRF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28472 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28475 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28479 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28482 "isa_tms320.tcc"
>
OpSUBRF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28487 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28491 "isa_tms320.tcc"
>::OpSUBRF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28495 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28499 "isa_tms320.tcc"
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
#line 28511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28518 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28521 "isa_tms320.tcc"
>
OpSUBRF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28526 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28530 "isa_tms320.tcc"
>::OpSUBRF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28538 "isa_tms320.tcc"
>(code, addr, "SUBRF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28548 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28555 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28558 "isa_tms320.tcc"
>
OpSUBRI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28563 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28567 "isa_tms320.tcc"
>::OpSUBRI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28575 "isa_tms320.tcc"
>(code, addr, "SUBRI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28595 "isa_tms320.tcc"
>
OpSUBRI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28604 "isa_tms320.tcc"
>::OpSUBRI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28608 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28612 "isa_tms320.tcc"
>(code, addr, "SUBRI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28632 "isa_tms320.tcc"
>
OpSUBRI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28641 "isa_tms320.tcc"
>::OpSUBRI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28645 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28649 "isa_tms320.tcc"
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
#line 28661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28671 "isa_tms320.tcc"
>
OpSUBRI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28680 "isa_tms320.tcc"
>::OpSUBRI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28688 "isa_tms320.tcc"
>(code, addr, "SUBRI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28708 "isa_tms320.tcc"
>
OpTSTB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28713 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28717 "isa_tms320.tcc"
>::OpTSTB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28721 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28725 "isa_tms320.tcc"
>(code, addr, "TSTB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28742 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28745 "isa_tms320.tcc"
>
OpTSTB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28754 "isa_tms320.tcc"
>::OpTSTB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28762 "isa_tms320.tcc"
>(code, addr, "TSTB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28782 "isa_tms320.tcc"
>
OpTSTB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28791 "isa_tms320.tcc"
>::OpTSTB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28799 "isa_tms320.tcc"
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
#line 28811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28818 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28821 "isa_tms320.tcc"
>
OpTSTB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28830 "isa_tms320.tcc"
>::OpTSTB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28838 "isa_tms320.tcc"
>(code, addr, "TSTB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28855 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28858 "isa_tms320.tcc"
>
OpXOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28863 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28867 "isa_tms320.tcc"
>::OpXOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28875 "isa_tms320.tcc"
>(code, addr, "XOR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28892 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28895 "isa_tms320.tcc"
>
OpXOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28904 "isa_tms320.tcc"
>::OpXOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28912 "isa_tms320.tcc"
>(code, addr, "XOR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28932 "isa_tms320.tcc"
>
OpXOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28941 "isa_tms320.tcc"
>::OpXOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28945 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28949 "isa_tms320.tcc"
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
#line 28961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28971 "isa_tms320.tcc"
>
OpXOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28980 "isa_tms320.tcc"
>::OpXOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28984 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28988 "isa_tms320.tcc"
>(code, addr, "XOR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28998 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29005 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29008 "isa_tms320.tcc"
>
OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29013 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29017 "isa_tms320.tcc"
>::OpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29025 "isa_tms320.tcc"
>(code, addr, "ADDC3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29046 "isa_tms320.tcc"
>
OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29055 "isa_tms320.tcc"
>::OpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29063 "isa_tms320.tcc"
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
#line 29075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29085 "isa_tms320.tcc"
>
OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29094 "isa_tms320.tcc"
>::OpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29102 "isa_tms320.tcc"
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
#line 29114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29124 "isa_tms320.tcc"
>
OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29133 "isa_tms320.tcc"
>::OpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29141 "isa_tms320.tcc"
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
#line 29154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29164 "isa_tms320.tcc"
>
OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29173 "isa_tms320.tcc"
>::OpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29181 "isa_tms320.tcc"
>(code, addr, "ADDF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29202 "isa_tms320.tcc"
>
OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29211 "isa_tms320.tcc"
>::OpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29219 "isa_tms320.tcc"
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
#line 29231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29241 "isa_tms320.tcc"
>
OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29250 "isa_tms320.tcc"
>::OpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29258 "isa_tms320.tcc"
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
#line 29270 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29280 "isa_tms320.tcc"
>
OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29289 "isa_tms320.tcc"
>::OpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29293 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29297 "isa_tms320.tcc"
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
#line 29310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29320 "isa_tms320.tcc"
>
OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29329 "isa_tms320.tcc"
>::OpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29337 "isa_tms320.tcc"
>(code, addr, "ADDI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29358 "isa_tms320.tcc"
>
OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29367 "isa_tms320.tcc"
>::OpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29375 "isa_tms320.tcc"
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
#line 29387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29397 "isa_tms320.tcc"
>
OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29406 "isa_tms320.tcc"
>::OpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29414 "isa_tms320.tcc"
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
#line 29426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29429 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29436 "isa_tms320.tcc"
>
OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29445 "isa_tms320.tcc"
>::OpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29449 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29453 "isa_tms320.tcc"
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
#line 29466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29476 "isa_tms320.tcc"
>
OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29485 "isa_tms320.tcc"
>::OpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29489 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29493 "isa_tms320.tcc"
>(code, addr, "AND3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29507 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29511 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29514 "isa_tms320.tcc"
>
OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29523 "isa_tms320.tcc"
>::OpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29531 "isa_tms320.tcc"
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
#line 29543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29553 "isa_tms320.tcc"
>
OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29562 "isa_tms320.tcc"
>::OpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29570 "isa_tms320.tcc"
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
#line 29582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29589 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29592 "isa_tms320.tcc"
>
OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29601 "isa_tms320.tcc"
>::OpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29609 "isa_tms320.tcc"
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
#line 29622 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29632 "isa_tms320.tcc"
>
OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29641 "isa_tms320.tcc"
>::OpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29645 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29649 "isa_tms320.tcc"
>(code, addr, "ANDN3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29660 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29667 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29670 "isa_tms320.tcc"
>
OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29675 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29679 "isa_tms320.tcc"
>::OpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29687 "isa_tms320.tcc"
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
#line 29699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29709 "isa_tms320.tcc"
>
OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29718 "isa_tms320.tcc"
>::OpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29726 "isa_tms320.tcc"
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
#line 29738 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29748 "isa_tms320.tcc"
>
OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29757 "isa_tms320.tcc"
>::OpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29765 "isa_tms320.tcc"
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
#line 29778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29788 "isa_tms320.tcc"
>
OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29797 "isa_tms320.tcc"
>::OpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29805 "isa_tms320.tcc"
>(code, addr, "ASH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29816 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29823 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29826 "isa_tms320.tcc"
>
OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29835 "isa_tms320.tcc"
>::OpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29843 "isa_tms320.tcc"
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
#line 29855 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29862 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29865 "isa_tms320.tcc"
>
OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29870 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29874 "isa_tms320.tcc"
>::OpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29882 "isa_tms320.tcc"
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
#line 29894 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29897 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29901 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29904 "isa_tms320.tcc"
>
OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29913 "isa_tms320.tcc"
>::OpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29921 "isa_tms320.tcc"
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
#line 29934 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29944 "isa_tms320.tcc"
>
OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29949 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29953 "isa_tms320.tcc"
>::OpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29957 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29961 "isa_tms320.tcc"
>(code, addr, "CMPF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29982 "isa_tms320.tcc"
>
OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29991 "isa_tms320.tcc"
>::OpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29999 "isa_tms320.tcc"
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
#line 30011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30021 "isa_tms320.tcc"
>
OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30030 "isa_tms320.tcc"
>::OpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30034 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30038 "isa_tms320.tcc"
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
#line 30050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30057 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30060 "isa_tms320.tcc"
>
OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30065 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30069 "isa_tms320.tcc"
>::OpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30077 "isa_tms320.tcc"
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
#line 30090 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30097 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30100 "isa_tms320.tcc"
>
OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30105 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30109 "isa_tms320.tcc"
>::OpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30117 "isa_tms320.tcc"
>(code, addr, "CMPI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30128 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30135 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30138 "isa_tms320.tcc"
>
OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30143 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30147 "isa_tms320.tcc"
>::OpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30155 "isa_tms320.tcc"
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
OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30182 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30186 "isa_tms320.tcc"
>::OpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30194 "isa_tms320.tcc"
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
OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30225 "isa_tms320.tcc"
>::OpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30233 "isa_tms320.tcc"
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
#line 30246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30256 "isa_tms320.tcc"
>
OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30265 "isa_tms320.tcc"
>::OpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30269 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30273 "isa_tms320.tcc"
>(code, addr, "LSH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30291 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30294 "isa_tms320.tcc"
>
OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30303 "isa_tms320.tcc"
>::OpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30311 "isa_tms320.tcc"
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
#line 30323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30333 "isa_tms320.tcc"
>
OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30342 "isa_tms320.tcc"
>::OpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30350 "isa_tms320.tcc"
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
#line 30362 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30365 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30369 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30372 "isa_tms320.tcc"
>
OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30381 "isa_tms320.tcc"
>::OpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30389 "isa_tms320.tcc"
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
#line 30402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30409 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30412 "isa_tms320.tcc"
>
OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30417 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30421 "isa_tms320.tcc"
>::OpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30429 "isa_tms320.tcc"
>(code, addr, "MPYF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30450 "isa_tms320.tcc"
>
OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30459 "isa_tms320.tcc"
>::OpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30467 "isa_tms320.tcc"
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
#line 30479 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30486 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30489 "isa_tms320.tcc"
>
OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30498 "isa_tms320.tcc"
>::OpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30506 "isa_tms320.tcc"
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
#line 30518 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30521 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30525 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30528 "isa_tms320.tcc"
>
OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30533 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30537 "isa_tms320.tcc"
>::OpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30541 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30545 "isa_tms320.tcc"
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
#line 30558 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30561 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30568 "isa_tms320.tcc"
>
OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30573 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30577 "isa_tms320.tcc"
>::OpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30585 "isa_tms320.tcc"
>(code, addr, "MPYI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30596 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30603 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30606 "isa_tms320.tcc"
>
OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30615 "isa_tms320.tcc"
>::OpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30623 "isa_tms320.tcc"
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
#line 30635 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30638 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30642 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30645 "isa_tms320.tcc"
>
OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30654 "isa_tms320.tcc"
>::OpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30658 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30662 "isa_tms320.tcc"
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
#line 30674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30684 "isa_tms320.tcc"
>
OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30693 "isa_tms320.tcc"
>::OpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30697 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30701 "isa_tms320.tcc"
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
#line 30714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30724 "isa_tms320.tcc"
>
OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30733 "isa_tms320.tcc"
>::OpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30741 "isa_tms320.tcc"
>(code, addr, "OR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30752 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30755 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30759 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30762 "isa_tms320.tcc"
>
OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30771 "isa_tms320.tcc"
>::OpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30779 "isa_tms320.tcc"
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
#line 30791 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30794 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30801 "isa_tms320.tcc"
>
OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30810 "isa_tms320.tcc"
>::OpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30818 "isa_tms320.tcc"
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
#line 30830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30840 "isa_tms320.tcc"
>
OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30849 "isa_tms320.tcc"
>::OpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30857 "isa_tms320.tcc"
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
#line 30870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30880 "isa_tms320.tcc"
>
OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30889 "isa_tms320.tcc"
>::OpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30897 "isa_tms320.tcc"
>(code, addr, "SUBB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30908 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30911 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30915 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30918 "isa_tms320.tcc"
>
OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30927 "isa_tms320.tcc"
>::OpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30935 "isa_tms320.tcc"
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
#line 30947 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30950 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30954 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30957 "isa_tms320.tcc"
>
OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30966 "isa_tms320.tcc"
>::OpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30974 "isa_tms320.tcc"
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
#line 30986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30996 "isa_tms320.tcc"
>
OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31005 "isa_tms320.tcc"
>::OpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31013 "isa_tms320.tcc"
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
#line 31026 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31033 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31036 "isa_tms320.tcc"
>
OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31045 "isa_tms320.tcc"
>::OpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31053 "isa_tms320.tcc"
>(code, addr, "SUBF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31064 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31067 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31074 "isa_tms320.tcc"
>
OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31083 "isa_tms320.tcc"
>::OpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31091 "isa_tms320.tcc"
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
#line 31103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31113 "isa_tms320.tcc"
>
OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31122 "isa_tms320.tcc"
>::OpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31130 "isa_tms320.tcc"
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
#line 31142 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31152 "isa_tms320.tcc"
>
OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31161 "isa_tms320.tcc"
>::OpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31169 "isa_tms320.tcc"
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
#line 31182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31192 "isa_tms320.tcc"
>
OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31197 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31201 "isa_tms320.tcc"
>::OpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31209 "isa_tms320.tcc"
>(code, addr, "SUBI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
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
OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31239 "isa_tms320.tcc"
>::OpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31247 "isa_tms320.tcc"
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
OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31278 "isa_tms320.tcc"
>::OpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31286 "isa_tms320.tcc"
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
#line 31298 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31308 "isa_tms320.tcc"
>
OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31317 "isa_tms320.tcc"
>::OpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31325 "isa_tms320.tcc"
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
#line 31338 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31341 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31348 "isa_tms320.tcc"
>
OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31353 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31357 "isa_tms320.tcc"
>::OpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31365 "isa_tms320.tcc"
>(code, addr, "TSTB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
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
OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31395 "isa_tms320.tcc"
>::OpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31403 "isa_tms320.tcc"
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
OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31434 "isa_tms320.tcc"
>::OpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31442 "isa_tms320.tcc"
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
#line 31454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31464 "isa_tms320.tcc"
>
OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31473 "isa_tms320.tcc"
>::OpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31481 "isa_tms320.tcc"
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
#line 31494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31504 "isa_tms320.tcc"
>
OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31513 "isa_tms320.tcc"
>::OpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31521 "isa_tms320.tcc"
>(code, addr, "XOR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
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
OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31551 "isa_tms320.tcc"
>::OpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31559 "isa_tms320.tcc"
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
OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31590 "isa_tms320.tcc"
>::OpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31598 "isa_tms320.tcc"
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
#line 31610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31620 "isa_tms320.tcc"
>
OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31629 "isa_tms320.tcc"
>::OpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31637 "isa_tms320.tcc"
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
#line 31650 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31657 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31660 "isa_tms320.tcc"
>
OpBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31669 "isa_tms320.tcc"
>::OpBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31677 "isa_tms320.tcc"
>(code, addr, "Bcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31697 "isa_tms320.tcc"
>
OpBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 31702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31706 "isa_tms320.tcc"
>::OpBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31714 "isa_tms320.tcc"
>(code, addr, "Bcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31724 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31731 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31734 "isa_tms320.tcc"
>
OpBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31743 "isa_tms320.tcc"
>::OpBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31751 "isa_tms320.tcc"
>(code, addr, "BcondD_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31761 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31771 "isa_tms320.tcc"
>
OpBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 31776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31780 "isa_tms320.tcc"
>::OpBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31784 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31788 "isa_tms320.tcc"
>(code, addr, "BcondD_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31808 "isa_tms320.tcc"
>
OpBR<
#line 45 "isa/tms320.isa"
CONFIG
#line 31813 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31817 "isa_tms320.tcc"
>::OpBR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31825 "isa_tms320.tcc"
>(code, addr, "BR")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31834 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31844 "isa_tms320.tcc"
>
OpBRD<
#line 45 "isa/tms320.isa"
CONFIG
#line 31849 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31853 "isa_tms320.tcc"
>::OpBRD(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31861 "isa_tms320.tcc"
>(code, addr, "BRD")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31873 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31877 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31880 "isa_tms320.tcc"
>
OpCALL<
#line 45 "isa/tms320.isa"
CONFIG
#line 31885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31889 "isa_tms320.tcc"
>::OpCALL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31897 "isa_tms320.tcc"
>(code, addr, "CALL")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31916 "isa_tms320.tcc"
>
OpCALLcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31925 "isa_tms320.tcc"
>::OpCALLcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31933 "isa_tms320.tcc"
>(code, addr, "CALLcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31953 "isa_tms320.tcc"
>
OpCALLcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 31958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31962 "isa_tms320.tcc"
>::OpCALLcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31970 "isa_tms320.tcc"
>(code, addr, "CALLcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31990 "isa_tms320.tcc"
>
OpDBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31999 "isa_tms320.tcc"
>::OpDBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32007 "isa_tms320.tcc"
>(code, addr, "DBcond_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32021 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32028 "isa_tms320.tcc"
>
OpDBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32037 "isa_tms320.tcc"
>::OpDBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32045 "isa_tms320.tcc"
>(code, addr, "DBcond_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32066 "isa_tms320.tcc"
>
OpDBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32075 "isa_tms320.tcc"
>::OpDBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32083 "isa_tms320.tcc"
>(code, addr, "DBcondD_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32094 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32104 "isa_tms320.tcc"
>
OpDBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 32109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32113 "isa_tms320.tcc"
>::OpDBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32121 "isa_tms320.tcc"
>(code, addr, "DBcondD_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32142 "isa_tms320.tcc"
>
OpIACK_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32151 "isa_tms320.tcc"
>::OpIACK_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32155 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32159 "isa_tms320.tcc"
>(code, addr, "IACK_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32168 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32178 "isa_tms320.tcc"
>
OpIACK_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32183 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32187 "isa_tms320.tcc"
>::OpIACK_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32195 "isa_tms320.tcc"
>(code, addr, "IACK_indir")
{
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32216 "isa_tms320.tcc"
>
OpIDLE<
#line 45 "isa/tms320.isa"
CONFIG
#line 32221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32225 "isa_tms320.tcc"
>::OpIDLE(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32233 "isa_tms320.tcc"
>(code, addr, "IDLE")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32251 "isa_tms320.tcc"
>
OpNOP_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32260 "isa_tms320.tcc"
>::OpNOP_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32268 "isa_tms320.tcc"
>(code, addr, "NOP_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32287 "isa_tms320.tcc"
>
OpNOP_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32296 "isa_tms320.tcc"
>::OpNOP_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32304 "isa_tms320.tcc"
>(code, addr, "NOP_indir")
{
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32325 "isa_tms320.tcc"
>
OpRETIcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32334 "isa_tms320.tcc"
>::OpRETIcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32342 "isa_tms320.tcc"
>(code, addr, "RETIcond")
{
	cond = ((code >> 16) & 0x1f);
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
OpRETScond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32370 "isa_tms320.tcc"
>::OpRETScond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32378 "isa_tms320.tcc"
>(code, addr, "RETScond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32390 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32397 "isa_tms320.tcc"
>
OpRPTB<
#line 45 "isa/tms320.isa"
CONFIG
#line 32402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32406 "isa_tms320.tcc"
>::OpRPTB(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32414 "isa_tms320.tcc"
>(code, addr, "RPTB")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32433 "isa_tms320.tcc"
>
OpRPTS_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32442 "isa_tms320.tcc"
>::OpRPTS_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32450 "isa_tms320.tcc"
>(code, addr, "RPTS_reg")
{
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32469 "isa_tms320.tcc"
>
OpRPTS_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32478 "isa_tms320.tcc"
>::OpRPTS_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32486 "isa_tms320.tcc"
>(code, addr, "RPTS_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32502 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32505 "isa_tms320.tcc"
>
OpRPTS_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32514 "isa_tms320.tcc"
>::OpRPTS_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32522 "isa_tms320.tcc"
>(code, addr, "RPTS_indir")
{
	mod = ((code >> 12) & 0xf);
	ar = ((code >> 8) & 0xf);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32543 "isa_tms320.tcc"
>
OpRPTS_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 32548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32552 "isa_tms320.tcc"
>::OpRPTS_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32560 "isa_tms320.tcc"
>(code, addr, "RPTS_imm")
{
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32579 "isa_tms320.tcc"
>
OpSWI<
#line 45 "isa/tms320.isa"
CONFIG
#line 32584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32588 "isa_tms320.tcc"
>::OpSWI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32592 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32596 "isa_tms320.tcc"
>(code, addr, "SWI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32604 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32607 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32614 "isa_tms320.tcc"
>
OpTRAPcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 32619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32623 "isa_tms320.tcc"
>::OpTRAPcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32631 "isa_tms320.tcc"
>(code, addr, "TRAPcond")
{
	cond = ((code >> 16) & 0x1f);
	n = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32651 "isa_tms320.tcc"
>
OpIDLE2<
#line 45 "isa/tms320.isa"
CONFIG
#line 32656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32660 "isa_tms320.tcc"
>::OpIDLE2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32668 "isa_tms320.tcc"
>(code, addr, "IDLE2")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32686 "isa_tms320.tcc"
>
OpLOPOWER<
#line 45 "isa/tms320.isa"
CONFIG
#line 32691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32695 "isa_tms320.tcc"
>::OpLOPOWER(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32703 "isa_tms320.tcc"
>(code, addr, "LOPOWER")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32711 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32718 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32721 "isa_tms320.tcc"
>
OpMAXSPEED<
#line 45 "isa/tms320.isa"
CONFIG
#line 32726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32730 "isa_tms320.tcc"
>::OpMAXSPEED(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32734 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32738 "isa_tms320.tcc"
>(code, addr, "MAXSPEED")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32746 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32753 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32756 "isa_tms320.tcc"
>
OpLDFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32761 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32765 "isa_tms320.tcc"
>::OpLDFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32769 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32773 "isa_tms320.tcc"
>(code, addr, "LDFI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32783 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32790 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32793 "isa_tms320.tcc"
>
OpLDFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32802 "isa_tms320.tcc"
>::OpLDFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32810 "isa_tms320.tcc"
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
#line 32822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32825 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32832 "isa_tms320.tcc"
>
OpLDII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32837 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32841 "isa_tms320.tcc"
>::OpLDII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32849 "isa_tms320.tcc"
>(code, addr, "LDII_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
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
OpLDII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32878 "isa_tms320.tcc"
>::OpLDII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32886 "isa_tms320.tcc"
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
#line 32898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32908 "isa_tms320.tcc"
>
OpSIGI<
#line 45 "isa/tms320.isa"
CONFIG
#line 32913 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32917 "isa_tms320.tcc"
>::OpSIGI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32925 "isa_tms320.tcc"
>(code, addr, "SIGI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 32933 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32940 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32943 "isa_tms320.tcc"
>
OpSTFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32952 "isa_tms320.tcc"
>::OpSTFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32960 "isa_tms320.tcc"
>(code, addr, "STFI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32970 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32973 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32977 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32980 "isa_tms320.tcc"
>
OpSTFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32985 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32989 "isa_tms320.tcc"
>::OpSTFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32997 "isa_tms320.tcc"
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
#line 33009 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33012 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33019 "isa_tms320.tcc"
>
OpSTII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33024 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33028 "isa_tms320.tcc"
>::OpSTII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33036 "isa_tms320.tcc"
>(code, addr, "STII_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33056 "isa_tms320.tcc"
>
OpSTII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33065 "isa_tms320.tcc"
>::OpSTII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33069 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33073 "isa_tms320.tcc"
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
#line 33085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33092 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33095 "isa_tms320.tcc"
>
OpFLOAT_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33100 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33104 "isa_tms320.tcc"
>::OpFLOAT_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33112 "isa_tms320.tcc"
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
#line 33126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33136 "isa_tms320.tcc"
>
OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33145 "isa_tms320.tcc"
>::OpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33149 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33153 "isa_tms320.tcc"
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
#line 33166 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33173 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33176 "isa_tms320.tcc"
>
OpLDF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33181 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33185 "isa_tms320.tcc"
>::OpLDF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33193 "isa_tms320.tcc"
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
#line 33207 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33210 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33214 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33217 "isa_tms320.tcc"
>
OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33226 "isa_tms320.tcc"
>::OpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33234 "isa_tms320.tcc"
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
#line 33247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33254 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33257 "isa_tms320.tcc"
>
OpLDI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33266 "isa_tms320.tcc"
>::OpLDI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33270 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33274 "isa_tms320.tcc"
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
#line 33288 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33295 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33298 "isa_tms320.tcc"
>
OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33307 "isa_tms320.tcc"
>::OpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33315 "isa_tms320.tcc"
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
#line 33328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33338 "isa_tms320.tcc"
>
OpLSH3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33347 "isa_tms320.tcc"
>::OpLSH3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33355 "isa_tms320.tcc"
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
#line 33370 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33373 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33380 "isa_tms320.tcc"
>
OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33389 "isa_tms320.tcc"
>::OpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33397 "isa_tms320.tcc"
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
#line 33411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33421 "isa_tms320.tcc"
>
OpMPYF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33430 "isa_tms320.tcc"
>::OpMPYF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33438 "isa_tms320.tcc"
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
#line 33453 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33463 "isa_tms320.tcc"
>
OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33468 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33472 "isa_tms320.tcc"
>::OpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33480 "isa_tms320.tcc"
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
#line 33494 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33497 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33501 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33504 "isa_tms320.tcc"
>
OpMPYI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33513 "isa_tms320.tcc"
>::OpMPYI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33517 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33521 "isa_tms320.tcc"
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
OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33555 "isa_tms320.tcc"
>::OpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33563 "isa_tms320.tcc"
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
#line 33577 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33584 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33587 "isa_tms320.tcc"
>
OpNEGF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33592 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33596 "isa_tms320.tcc"
>::OpNEGF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33604 "isa_tms320.tcc"
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
#line 33618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33625 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33628 "isa_tms320.tcc"
>
OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33633 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33637 "isa_tms320.tcc"
>::OpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33641 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33645 "isa_tms320.tcc"
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
#line 33658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33661 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33668 "isa_tms320.tcc"
>
OpNEGI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33677 "isa_tms320.tcc"
>::OpNEGI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33685 "isa_tms320.tcc"
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
#line 33699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33709 "isa_tms320.tcc"
>
OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33718 "isa_tms320.tcc"
>::OpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33726 "isa_tms320.tcc"
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
#line 33739 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33742 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33746 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33749 "isa_tms320.tcc"
>
OpNOT_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33754 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33758 "isa_tms320.tcc"
>::OpNOT_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33766 "isa_tms320.tcc"
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
OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33799 "isa_tms320.tcc"
>::OpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33807 "isa_tms320.tcc"
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
#line 33820 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33827 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33830 "isa_tms320.tcc"
>
OpOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33839 "isa_tms320.tcc"
>::OpOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33847 "isa_tms320.tcc"
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
#line 33862 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33872 "isa_tms320.tcc"
>
OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33881 "isa_tms320.tcc"
>::OpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33889 "isa_tms320.tcc"
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
#line 33903 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33910 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33913 "isa_tms320.tcc"
>
OpSTF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 33918 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33922 "isa_tms320.tcc"
>::OpSTF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33930 "isa_tms320.tcc"
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
#line 33944 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33947 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33954 "isa_tms320.tcc"
>
OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 33959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33963 "isa_tms320.tcc"
>::OpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33971 "isa_tms320.tcc"
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
#line 33984 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33987 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33991 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33994 "isa_tms320.tcc"
>
OpSTI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 33999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34003 "isa_tms320.tcc"
>::OpSTI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34011 "isa_tms320.tcc"
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
#line 34025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34035 "isa_tms320.tcc"
>
OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34044 "isa_tms320.tcc"
>::OpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34052 "isa_tms320.tcc"
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
#line 34065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34072 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34075 "isa_tms320.tcc"
>
OpSUBF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34084 "isa_tms320.tcc"
>::OpSUBF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34092 "isa_tms320.tcc"
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
#line 34107 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34117 "isa_tms320.tcc"
>
OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34126 "isa_tms320.tcc"
>::OpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34134 "isa_tms320.tcc"
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
#line 34148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34158 "isa_tms320.tcc"
>
OpSUBI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34167 "isa_tms320.tcc"
>::OpSUBI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34175 "isa_tms320.tcc"
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
OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34209 "isa_tms320.tcc"
>::OpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34217 "isa_tms320.tcc"
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
#line 34231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34241 "isa_tms320.tcc"
>
OpXOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34250 "isa_tms320.tcc"
>::OpXOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34258 "isa_tms320.tcc"
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
#line 34273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34283 "isa_tms320.tcc"
>
OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34292 "isa_tms320.tcc"
>::OpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34300 "isa_tms320.tcc"
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
#line 34314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34324 "isa_tms320.tcc"
>
OpLDF_LDF<
#line 45 "isa/tms320.isa"
CONFIG
#line 34329 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34333 "isa_tms320.tcc"
>::OpLDF_LDF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34337 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34341 "isa_tms320.tcc"
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
#line 34355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34362 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34365 "isa_tms320.tcc"
>
OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34370 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34374 "isa_tms320.tcc"
>::OpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34382 "isa_tms320.tcc"
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
#line 34395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34405 "isa_tms320.tcc"
>
OpLDI_LDI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34414 "isa_tms320.tcc"
>::OpLDI_LDI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34422 "isa_tms320.tcc"
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
#line 34436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34446 "isa_tms320.tcc"
>
OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34455 "isa_tms320.tcc"
>::OpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34463 "isa_tms320.tcc"
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
#line 34476 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34486 "isa_tms320.tcc"
>
OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34495 "isa_tms320.tcc"
>::OpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34503 "isa_tms320.tcc"
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
#line 34520 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34527 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34530 "isa_tms320.tcc"
>
OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34539 "isa_tms320.tcc"
>::OpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34547 "isa_tms320.tcc"
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
OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34581 "isa_tms320.tcc"
>::OpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34589 "isa_tms320.tcc"
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
#line 34606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34616 "isa_tms320.tcc"
>
OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34625 "isa_tms320.tcc"
>::OpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34633 "isa_tms320.tcc"
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
#line 34648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34658 "isa_tms320.tcc"
>
OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34667 "isa_tms320.tcc"
>::OpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34675 "isa_tms320.tcc"
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
#line 34692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34695 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34702 "isa_tms320.tcc"
>
OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34711 "isa_tms320.tcc"
>::OpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34719 "isa_tms320.tcc"
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
#line 34734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34744 "isa_tms320.tcc"
>
OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 34749 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34753 "isa_tms320.tcc"
>::OpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34757 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34761 "isa_tms320.tcc"
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
#line 34778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34788 "isa_tms320.tcc"
>
OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 34793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34797 "isa_tms320.tcc"
>::OpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34805 "isa_tms320.tcc"
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
#line 34820 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34827 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34830 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 34835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34839 "isa_tms320.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 45 "isa/tms320.isa"
class
#line 34850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34860 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 34865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34869 "isa_tms320.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 45 "isa/tms320.isa"
class
#line 34880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34883 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34887 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34890 "isa_tms320.tcc"
>
DecodeTableEntry<
#line 45 "isa/tms320.isa"
CONFIG
#line 34895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34899 "isa_tms320.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34907 "isa_tms320.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 45 "isa/tms320.isa"
class
#line 34918 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34928 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 34933 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34937 "isa_tms320.tcc"
>::Decoder()
{
	little_endian = false;
	mru_page = 0;
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34946 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 34950 "isa_tms320.tcc"
	>(0x8c00e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34954 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 34958 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34963 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 34967 "isa_tms320.tcc"
	>(0x8c000000UL, 0xfc000000UL, DecodeOpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34971 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 34975 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34980 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 34984 "isa_tms320.tcc"
	>(0x8800e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34988 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 34992 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 34997 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35001 "isa_tms320.tcc"
	>(0x88000000UL, 0xfc000000UL, DecodeOpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35005 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35009 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35014 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35018 "isa_tms320.tcc"
	>(0x8400e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35022 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35026 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35031 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35035 "isa_tms320.tcc"
	>(0x84000000UL, 0xfc000000UL, DecodeOpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35039 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35043 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35048 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35052 "isa_tms320.tcc"
	>(0x8000e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35056 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35060 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35065 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35069 "isa_tms320.tcc"
	>(0x80000000UL, 0xfc000000UL, DecodeOpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35073 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35077 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35082 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35086 "isa_tms320.tcc"
	>(0xc60000e0UL, 0xfe0700e0UL, DecodeOpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35090 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35094 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35099 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35103 "isa_tms320.tcc"
	>(0xc6000000UL, 0xfe070000UL, DecodeOpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35107 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35111 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35116 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35120 "isa_tms320.tcc"
	>(0xc40000e0UL, 0xfe0700e0UL, DecodeOpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35124 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35128 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35133 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35137 "isa_tms320.tcc"
	>(0xc4000000UL, 0xfe070000UL, DecodeOpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35141 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35145 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35150 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35154 "isa_tms320.tcc"
	>(0xee0000e0UL, 0xfe0000e0UL, DecodeOpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35158 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35162 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35167 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35171 "isa_tms320.tcc"
	>(0xee000000UL, 0xfe000000UL, DecodeOpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35175 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35179 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35184 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35188 "isa_tms320.tcc"
	>(0xec0000e0UL, 0xfe0000e0UL, DecodeOpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35192 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35196 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35201 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35205 "isa_tms320.tcc"
	>(0xec000000UL, 0xfe000000UL, DecodeOpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35209 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35213 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35218 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35222 "isa_tms320.tcc"
	>(0xea0000e0UL, 0xfe0000e0UL, DecodeOpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35226 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35230 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35235 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35239 "isa_tms320.tcc"
	>(0xea000000UL, 0xfe000000UL, DecodeOpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35243 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35247 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35256 "isa_tms320.tcc"
	>(0xc20000e0UL, 0xfe3800e0UL, DecodeOpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35260 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35264 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35269 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35273 "isa_tms320.tcc"
	>(0xc2000000UL, 0xfe380000UL, DecodeOpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35277 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35281 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35290 "isa_tms320.tcc"
	>(0xc00000e0UL, 0xfe3800e0UL, DecodeOpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35294 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35298 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35303 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35307 "isa_tms320.tcc"
	>(0xc0000000UL, 0xfe380000UL, DecodeOpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35311 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35315 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35320 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35324 "isa_tms320.tcc"
	>(0xe80000e0UL, 0xfe0000e0UL, DecodeOpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35328 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35332 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35337 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35341 "isa_tms320.tcc"
	>(0xe8000000UL, 0xfe000000UL, DecodeOpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35345 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35349 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35354 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35358 "isa_tms320.tcc"
	>(0xe60000e0UL, 0xfe3800e0UL, DecodeOpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35362 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35366 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35371 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35375 "isa_tms320.tcc"
	>(0xe6000000UL, 0xfe380000UL, DecodeOpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35379 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35383 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35388 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35392 "isa_tms320.tcc"
	>(0xe40000e0UL, 0xfe3800e0UL, DecodeOpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35396 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35400 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35405 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35409 "isa_tms320.tcc"
	>(0xe4000000UL, 0xfe380000UL, DecodeOpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35413 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35417 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35422 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35426 "isa_tms320.tcc"
	>(0xe20000e0UL, 0xfe3800e0UL, DecodeOpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35430 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35434 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35439 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35443 "isa_tms320.tcc"
	>(0xe2000000UL, 0xfe380000UL, DecodeOpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35447 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35451 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35456 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35460 "isa_tms320.tcc"
	>(0xe00000e0UL, 0xfe0000e0UL, DecodeOpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35464 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35468 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35473 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35477 "isa_tms320.tcc"
	>(0xe0000000UL, 0xfe000000UL, DecodeOpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35485 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35494 "isa_tms320.tcc"
	>(0xde0000e0UL, 0xfe0000e0UL, DecodeOpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35498 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35502 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35507 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35511 "isa_tms320.tcc"
	>(0xde000000UL, 0xfe000000UL, DecodeOpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35515 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35519 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35528 "isa_tms320.tcc"
	>(0xdc0000e0UL, 0xfe0000e0UL, DecodeOpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35532 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35536 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35541 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35545 "isa_tms320.tcc"
	>(0xdc000000UL, 0xfe000000UL, DecodeOpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35549 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35553 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35562 "isa_tms320.tcc"
	>(0xda0000e0UL, 0xfe3800e0UL, DecodeOpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35566 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35570 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35575 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35579 "isa_tms320.tcc"
	>(0xda000000UL, 0xfe380000UL, DecodeOpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35583 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35587 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35592 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35596 "isa_tms320.tcc"
	>(0xd80000e0UL, 0xfe3800e0UL, DecodeOpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35600 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35604 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35609 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35613 "isa_tms320.tcc"
	>(0xd8000000UL, 0xfe380000UL, DecodeOpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35617 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35621 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35626 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35630 "isa_tms320.tcc"
	>(0xd60000e0UL, 0xfe3800e0UL, DecodeOpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35634 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35638 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35643 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35647 "isa_tms320.tcc"
	>(0xd6000000UL, 0xfe380000UL, DecodeOpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35651 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35655 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35660 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35664 "isa_tms320.tcc"
	>(0x15c00000UL, 0xffe00000UL, DecodeOpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35668 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35672 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35677 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35681 "isa_tms320.tcc"
	>(0x15a00000UL, 0xffe00000UL, DecodeOpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35685 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35689 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35694 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35698 "isa_tms320.tcc"
	>(0x14c00000UL, 0xffe00000UL, DecodeOpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35702 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35706 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35711 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35715 "isa_tms320.tcc"
	>(0x14a00000UL, 0xffe00000UL, DecodeOpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35719 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35723 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35728 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35732 "isa_tms320.tcc"
	>(0x16000000UL, 0xffffffffUL, DecodeOpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35736 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35740 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35745 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35749 "isa_tms320.tcc"
	>(0x8c00000UL, 0xffe00000UL, DecodeOpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35753 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35757 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35762 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35766 "isa_tms320.tcc"
	>(0x8a00000UL, 0xffe00000UL, DecodeOpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35770 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35774 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35779 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35783 "isa_tms320.tcc"
	>(0x7c00000UL, 0xffe00000UL, DecodeOpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35787 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35791 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35800 "isa_tms320.tcc"
	>(0x7a00000UL, 0xffe00000UL, DecodeOpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35804 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35808 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35813 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35817 "isa_tms320.tcc"
	>(0x10800000UL, 0xffffffffUL, DecodeOpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35821 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35825 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35830 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35834 "isa_tms320.tcc"
	>(0x10800001UL, 0xffffffffUL, DecodeOpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35838 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35842 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35847 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35851 "isa_tms320.tcc"
	>(0x6000001UL, 0xffffffffUL, DecodeOpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35855 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35859 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35864 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35868 "isa_tms320.tcc"
	>(0x74000020UL, 0xffe0ffe0UL, DecodeOpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35872 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35876 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35881 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35885 "isa_tms320.tcc"
	>(0x66000000UL, 0xffffffffUL, DecodeOpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35889 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35893 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35898 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35902 "isa_tms320.tcc"
	>(0x13fb0000UL, 0xffff0000UL, DecodeOpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35906 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35910 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35915 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35919 "isa_tms320.tcc"
	>(0x13db0000UL, 0xffff0000UL, DecodeOpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35923 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35927 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35932 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35936 "isa_tms320.tcc"
	>(0x13bb0000UL, 0xffff0000UL, DecodeOpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35940 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35944 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35949 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35953 "isa_tms320.tcc"
	>(0x139b0000UL, 0xffffffe0UL, DecodeOpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35957 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35961 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35966 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35970 "isa_tms320.tcc"
	>(0x64000000UL, 0xff000000UL, DecodeOpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35974 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35978 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35983 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35987 "isa_tms320.tcc"
	>(0x78800000UL, 0xffe0ffffUL, DecodeOpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 35991 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 35995 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36000 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36004 "isa_tms320.tcc"
	>(0x78000000UL, 0xffe0ffffUL, DecodeOpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36008 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36012 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36017 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36021 "isa_tms320.tcc"
	>(0xcc00000UL, 0xffff0000UL, DecodeOpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36025 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36029 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36034 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36038 "isa_tms320.tcc"
	>(0xca00000UL, 0xffff0000UL, DecodeOpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36042 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36046 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36051 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36055 "isa_tms320.tcc"
	>(0x6000000UL, 0xffffffffUL, DecodeOpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36059 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36063 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36068 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36072 "isa_tms320.tcc"
	>(0x1b400000UL, 0xffff0000UL, DecodeOpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36076 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36080 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36085 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36089 "isa_tms320.tcc"
	>(0x1b200000UL, 0xffff0000UL, DecodeOpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36093 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36097 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36102 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36106 "isa_tms320.tcc"
	>(0x6e200000UL, 0xfe200000UL, DecodeOpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36110 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36114 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36119 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36123 "isa_tms320.tcc"
	>(0x6c200000UL, 0xfe20ffe0UL, DecodeOpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36127 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36131 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36136 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36140 "isa_tms320.tcc"
	>(0x6e000000UL, 0xfe200000UL, DecodeOpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36144 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36148 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36153 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36157 "isa_tms320.tcc"
	>(0x6c000000UL, 0xfe20ffe0UL, DecodeOpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36161 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36165 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36170 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36174 "isa_tms320.tcc"
	>(0x72000000UL, 0xffe00000UL, DecodeOpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36178 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36182 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36187 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36191 "isa_tms320.tcc"
	>(0x70000000UL, 0xffe0ffe0UL, DecodeOpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36195 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36199 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36204 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36208 "isa_tms320.tcc"
	>(0x62000000UL, 0xff000000UL, DecodeOpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36212 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36216 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36221 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36225 "isa_tms320.tcc"
	>(0x61000000UL, 0xff000000UL, DecodeOpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36229 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36233 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36238 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36242 "isa_tms320.tcc"
	>(0x60000000UL, 0xff000000UL, DecodeOpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36246 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36250 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36255 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36259 "isa_tms320.tcc"
	>(0x6a200000UL, 0xffe00000UL, DecodeOpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36263 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36267 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36272 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36276 "isa_tms320.tcc"
	>(0x68200000UL, 0xffe0ffe0UL, DecodeOpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36280 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36284 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36289 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36293 "isa_tms320.tcc"
	>(0x6a000000UL, 0xffe00000UL, DecodeOpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36297 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36301 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36306 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36310 "isa_tms320.tcc"
	>(0x68000000UL, 0xffe0ffe0UL, DecodeOpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36314 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36318 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36323 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36327 "isa_tms320.tcc"
	>(0x28600000UL, 0xffe00000UL, DecodeOpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36331 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36335 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36340 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36344 "isa_tms320.tcc"
	>(0x28400000UL, 0xffe0e000UL, DecodeOpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36348 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36352 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36357 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36361 "isa_tms320.tcc"
	>(0x28200000UL, 0xffe000e0UL, DecodeOpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36365 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36369 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36374 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36378 "isa_tms320.tcc"
	>(0x28000000UL, 0xffe0e0e0UL, DecodeOpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36382 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36386 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36391 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36395 "isa_tms320.tcc"
	>(0x27e00000UL, 0xffe00000UL, DecodeOpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36399 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36403 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36408 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36412 "isa_tms320.tcc"
	>(0x27c00000UL, 0xffe0e000UL, DecodeOpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36416 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36420 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36425 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36429 "isa_tms320.tcc"
	>(0x27a00000UL, 0xffe000e0UL, DecodeOpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36433 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36437 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36442 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36446 "isa_tms320.tcc"
	>(0x27800000UL, 0xffe0e0e0UL, DecodeOpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36450 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36454 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36459 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36463 "isa_tms320.tcc"
	>(0x27600000UL, 0xffe00000UL, DecodeOpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36467 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36471 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36476 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36480 "isa_tms320.tcc"
	>(0x27400000UL, 0xffe0e000UL, DecodeOpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36484 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36488 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36493 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36497 "isa_tms320.tcc"
	>(0x27200000UL, 0xffe000e0UL, DecodeOpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36501 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36505 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36510 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36514 "isa_tms320.tcc"
	>(0x27000000UL, 0xffe0e0e0UL, DecodeOpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36518 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36522 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36527 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36531 "isa_tms320.tcc"
	>(0x26e00000UL, 0xffe00000UL, DecodeOpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36535 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36539 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36544 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36548 "isa_tms320.tcc"
	>(0x26c00000UL, 0xffe0e000UL, DecodeOpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36552 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36556 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36561 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36565 "isa_tms320.tcc"
	>(0x26a00000UL, 0xffe000e0UL, DecodeOpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36569 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36573 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36578 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36582 "isa_tms320.tcc"
	>(0x26800000UL, 0xffe0e0e0UL, DecodeOpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36586 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36590 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36595 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36599 "isa_tms320.tcc"
	>(0x26600000UL, 0xffe00000UL, DecodeOpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36603 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36607 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36612 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36616 "isa_tms320.tcc"
	>(0x26400000UL, 0xffe0e000UL, DecodeOpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36620 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36624 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36629 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36633 "isa_tms320.tcc"
	>(0x26200000UL, 0xffe000e0UL, DecodeOpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36637 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36641 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36646 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36650 "isa_tms320.tcc"
	>(0x26000000UL, 0xffe0e0e0UL, DecodeOpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36654 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36658 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36663 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36667 "isa_tms320.tcc"
	>(0x25e00000UL, 0xffe00000UL, DecodeOpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36671 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36675 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36680 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36684 "isa_tms320.tcc"
	>(0x25c00000UL, 0xffe0e000UL, DecodeOpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36688 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36692 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36697 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36701 "isa_tms320.tcc"
	>(0x25a00000UL, 0xffe000e0UL, DecodeOpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36705 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36709 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36714 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36718 "isa_tms320.tcc"
	>(0x25800000UL, 0xffe0e0e0UL, DecodeOpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36722 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36726 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36731 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36735 "isa_tms320.tcc"
	>(0x25600000UL, 0xffe00000UL, DecodeOpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36739 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36743 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36748 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36752 "isa_tms320.tcc"
	>(0x25400000UL, 0xffe0e000UL, DecodeOpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36756 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36760 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36765 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36769 "isa_tms320.tcc"
	>(0x25200000UL, 0xffe000e0UL, DecodeOpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36773 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36777 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36782 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36786 "isa_tms320.tcc"
	>(0x25000000UL, 0xffe0e0e0UL, DecodeOpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36790 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36794 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36799 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36803 "isa_tms320.tcc"
	>(0x24e00000UL, 0xffe00000UL, DecodeOpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36807 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36811 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36816 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36820 "isa_tms320.tcc"
	>(0x24c00000UL, 0xffe0e000UL, DecodeOpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36824 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36828 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36837 "isa_tms320.tcc"
	>(0x24a00000UL, 0xffe000e0UL, DecodeOpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36841 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36845 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36850 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36854 "isa_tms320.tcc"
	>(0x24800000UL, 0xffe0e0e0UL, DecodeOpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36858 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36862 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36867 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36871 "isa_tms320.tcc"
	>(0x24600000UL, 0xffe00000UL, DecodeOpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36875 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36879 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36884 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36888 "isa_tms320.tcc"
	>(0x24400000UL, 0xffe0e000UL, DecodeOpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36892 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36896 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36901 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36905 "isa_tms320.tcc"
	>(0x24200000UL, 0xffe000e0UL, DecodeOpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36909 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36913 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36918 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36922 "isa_tms320.tcc"
	>(0x24000000UL, 0xffe0e0e0UL, DecodeOpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36926 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36930 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36935 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36939 "isa_tms320.tcc"
	>(0x23e00000UL, 0xffe00000UL, DecodeOpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36943 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36947 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36952 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36956 "isa_tms320.tcc"
	>(0x23c00000UL, 0xffe0e000UL, DecodeOpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36960 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36964 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36969 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36973 "isa_tms320.tcc"
	>(0x23a00000UL, 0xffe000e0UL, DecodeOpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36977 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36981 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36986 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36990 "isa_tms320.tcc"
	>(0x23800000UL, 0xffe0e0e0UL, DecodeOpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 36994 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 36998 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37003 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37007 "isa_tms320.tcc"
	>(0x23600000UL, 0xffe00000UL, DecodeOpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37011 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37015 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37020 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37024 "isa_tms320.tcc"
	>(0x23400000UL, 0xffe0e000UL, DecodeOpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37028 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37032 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37037 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37041 "isa_tms320.tcc"
	>(0x23200000UL, 0xffe000e0UL, DecodeOpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37045 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37049 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37054 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37058 "isa_tms320.tcc"
	>(0x23000000UL, 0xffe0e0e0UL, DecodeOpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37062 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37066 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37071 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37075 "isa_tms320.tcc"
	>(0x22e00000UL, 0xffe00000UL, DecodeOpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37079 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37083 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37088 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37092 "isa_tms320.tcc"
	>(0x22c00000UL, 0xffe0e000UL, DecodeOpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37096 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37100 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37105 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37109 "isa_tms320.tcc"
	>(0x22a00000UL, 0xffe000e0UL, DecodeOpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37113 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37117 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37122 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37126 "isa_tms320.tcc"
	>(0x22800000UL, 0xffe0e0e0UL, DecodeOpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37130 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37134 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37139 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37143 "isa_tms320.tcc"
	>(0x22600000UL, 0xffe00000UL, DecodeOpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37147 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37151 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37156 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37160 "isa_tms320.tcc"
	>(0x22400000UL, 0xffe0e000UL, DecodeOpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37164 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37168 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37173 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37177 "isa_tms320.tcc"
	>(0x22200000UL, 0xffe000e0UL, DecodeOpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37181 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37185 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37190 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37194 "isa_tms320.tcc"
	>(0x22000000UL, 0xffe0e0e0UL, DecodeOpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37198 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37202 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37207 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37211 "isa_tms320.tcc"
	>(0x21e00000UL, 0xffe00000UL, DecodeOpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37215 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37219 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37224 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37228 "isa_tms320.tcc"
	>(0x21c00000UL, 0xffe0e000UL, DecodeOpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37232 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37236 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37241 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37245 "isa_tms320.tcc"
	>(0x21a00000UL, 0xffe000e0UL, DecodeOpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37249 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37253 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37258 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37262 "isa_tms320.tcc"
	>(0x21800000UL, 0xffe0e0e0UL, DecodeOpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37266 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37270 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37275 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37279 "isa_tms320.tcc"
	>(0x21600000UL, 0xffe00000UL, DecodeOpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37283 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37287 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37292 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37296 "isa_tms320.tcc"
	>(0x21400000UL, 0xffe0e000UL, DecodeOpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37300 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37304 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37309 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37313 "isa_tms320.tcc"
	>(0x21200000UL, 0xffe000e0UL, DecodeOpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37317 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37321 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37326 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37330 "isa_tms320.tcc"
	>(0x21000000UL, 0xffe0e0e0UL, DecodeOpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37334 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37338 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37343 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37347 "isa_tms320.tcc"
	>(0x20e00000UL, 0xffe00000UL, DecodeOpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37351 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37355 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37360 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37364 "isa_tms320.tcc"
	>(0x20c00000UL, 0xffe0e000UL, DecodeOpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37368 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37372 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37377 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37381 "isa_tms320.tcc"
	>(0x20a00000UL, 0xffe000e0UL, DecodeOpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37385 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37389 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37394 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37398 "isa_tms320.tcc"
	>(0x20800000UL, 0xffe0e0e0UL, DecodeOpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37402 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37406 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37411 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37415 "isa_tms320.tcc"
	>(0x20600000UL, 0xffe00000UL, DecodeOpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37419 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37423 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37428 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37432 "isa_tms320.tcc"
	>(0x20400000UL, 0xffe0e000UL, DecodeOpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37436 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37440 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37445 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37449 "isa_tms320.tcc"
	>(0x20200000UL, 0xffe000e0UL, DecodeOpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37453 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37457 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37462 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37466 "isa_tms320.tcc"
	>(0x20000000UL, 0xffe0e0e0UL, DecodeOpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37470 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37474 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37479 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37483 "isa_tms320.tcc"
	>(0x1ae00000UL, 0xffe00000UL, DecodeOpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37487 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37491 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37496 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37500 "isa_tms320.tcc"
	>(0x1ac00000UL, 0xffe00000UL, DecodeOpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37504 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37508 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37513 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37517 "isa_tms320.tcc"
	>(0x1aa00000UL, 0xffe00000UL, DecodeOpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37521 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37525 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37530 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37534 "isa_tms320.tcc"
	>(0x1a800000UL, 0xffe0ffe0UL, DecodeOpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37538 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37542 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37547 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37551 "isa_tms320.tcc"
	>(0x1a600000UL, 0xffe00000UL, DecodeOpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37555 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37559 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37564 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37568 "isa_tms320.tcc"
	>(0x1a400000UL, 0xffe00000UL, DecodeOpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37572 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37576 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37581 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37585 "isa_tms320.tcc"
	>(0x1a200000UL, 0xffe00000UL, DecodeOpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37589 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37593 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37598 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37602 "isa_tms320.tcc"
	>(0x1a000000UL, 0xffe0ffe0UL, DecodeOpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37606 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37610 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37615 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37619 "isa_tms320.tcc"
	>(0x19e00000UL, 0xffe00000UL, DecodeOpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37623 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37627 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37632 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37636 "isa_tms320.tcc"
	>(0x19c00000UL, 0xffe00000UL, DecodeOpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37640 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37644 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37649 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37653 "isa_tms320.tcc"
	>(0x19a00000UL, 0xffe00000UL, DecodeOpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37657 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37661 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37666 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37670 "isa_tms320.tcc"
	>(0x19800000UL, 0xffe0ffe0UL, DecodeOpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37674 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37678 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37683 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37687 "isa_tms320.tcc"
	>(0x19600000UL, 0xffe00000UL, DecodeOpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37691 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37695 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37700 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37704 "isa_tms320.tcc"
	>(0x19400000UL, 0xffe00000UL, DecodeOpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37708 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37712 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37717 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37721 "isa_tms320.tcc"
	>(0x19200000UL, 0xffe00000UL, DecodeOpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37725 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37729 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37734 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37738 "isa_tms320.tcc"
	>(0x19000000UL, 0xffe0ffe0UL, DecodeOpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37742 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37746 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37751 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37755 "isa_tms320.tcc"
	>(0x18e00000UL, 0xffe00000UL, DecodeOpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37759 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37763 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37768 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37772 "isa_tms320.tcc"
	>(0x18c00000UL, 0xffe00000UL, DecodeOpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37776 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37780 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37785 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37789 "isa_tms320.tcc"
	>(0x18a00000UL, 0xffe00000UL, DecodeOpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37793 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37797 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37802 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37806 "isa_tms320.tcc"
	>(0x18800000UL, 0xffe0ffe0UL, DecodeOpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37810 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37814 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37819 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37823 "isa_tms320.tcc"
	>(0x18600000UL, 0xffe00000UL, DecodeOpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37827 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37831 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37836 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37840 "isa_tms320.tcc"
	>(0x18400000UL, 0xffe00000UL, DecodeOpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37844 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37848 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37853 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37857 "isa_tms320.tcc"
	>(0x18200000UL, 0xffe00000UL, DecodeOpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37861 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37865 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37870 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37874 "isa_tms320.tcc"
	>(0x18000000UL, 0xffe0ffe0UL, DecodeOpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37878 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37882 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37887 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37891 "isa_tms320.tcc"
	>(0x17e00000UL, 0xffe00000UL, DecodeOpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37895 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37899 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37904 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37908 "isa_tms320.tcc"
	>(0x17c00000UL, 0xffe00000UL, DecodeOpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37912 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37916 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37921 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37925 "isa_tms320.tcc"
	>(0x17a00000UL, 0xffe00000UL, DecodeOpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37929 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37933 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37938 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37942 "isa_tms320.tcc"
	>(0x17800000UL, 0xffe0ffe0UL, DecodeOpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37946 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37950 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37955 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37959 "isa_tms320.tcc"
	>(0x17600000UL, 0xffe00000UL, DecodeOpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37963 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37967 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37972 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37976 "isa_tms320.tcc"
	>(0x17400000UL, 0xffe00000UL, DecodeOpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37980 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37984 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37989 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37993 "isa_tms320.tcc"
	>(0x17200000UL, 0xffe00000UL, DecodeOpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37997 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38001 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38006 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38010 "isa_tms320.tcc"
	>(0x17000000UL, 0xffe0ffe0UL, DecodeOpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38014 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38018 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38023 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38027 "isa_tms320.tcc"
	>(0x16e00000UL, 0xffe00000UL, DecodeOpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38031 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38035 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38040 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38044 "isa_tms320.tcc"
	>(0x16c00000UL, 0xffe00000UL, DecodeOpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38048 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38052 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38057 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38061 "isa_tms320.tcc"
	>(0x16a00000UL, 0xffe00000UL, DecodeOpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38065 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38069 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38074 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38078 "isa_tms320.tcc"
	>(0x16800000UL, 0xffe0ffe0UL, DecodeOpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38082 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38086 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38091 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38095 "isa_tms320.tcc"
	>(0x1360ffffUL, 0xffe0ffffUL, DecodeOpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38099 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38103 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38108 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38112 "isa_tms320.tcc"
	>(0x12e0ffffUL, 0xffe0ffffUL, DecodeOpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38116 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38120 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38125 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38129 "isa_tms320.tcc"
	>(0x12600001UL, 0xffe0ffffUL, DecodeOpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38133 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38137 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38142 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38146 "isa_tms320.tcc"
	>(0x11e00001UL, 0xffe0ffffUL, DecodeOpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38150 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38154 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38159 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38163 "isa_tms320.tcc"
	>(0x11600000UL, 0xffe00000UL, DecodeOpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38167 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38171 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38176 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38180 "isa_tms320.tcc"
	>(0x11400000UL, 0xffe00000UL, DecodeOpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38184 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38188 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38193 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38197 "isa_tms320.tcc"
	>(0x11200000UL, 0xffe00000UL, DecodeOpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38201 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38205 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38210 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38214 "isa_tms320.tcc"
	>(0x11000000UL, 0xffe0ffe0UL, DecodeOpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38218 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38222 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38227 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38231 "isa_tms320.tcc"
	>(0x10600000UL, 0xffe00000UL, DecodeOpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38235 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38239 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38244 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38248 "isa_tms320.tcc"
	>(0x10400000UL, 0xffe00000UL, DecodeOpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38256 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38265 "isa_tms320.tcc"
	>(0x10200000UL, 0xffe00000UL, DecodeOpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38269 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38273 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38278 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38282 "isa_tms320.tcc"
	>(0x10000000UL, 0xffe0ffe0UL, DecodeOpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38290 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38295 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38299 "isa_tms320.tcc"
	>(0xde00000UL, 0xffe00000UL, DecodeOpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38303 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38307 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38312 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38316 "isa_tms320.tcc"
	>(0xdc00000UL, 0xffe00000UL, DecodeOpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38320 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38324 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38329 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38333 "isa_tms320.tcc"
	>(0xda00000UL, 0xffe00000UL, DecodeOpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38337 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38341 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38346 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38350 "isa_tms320.tcc"
	>(0xd800000UL, 0xffe0ffe0UL, DecodeOpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38354 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38358 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38363 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38367 "isa_tms320.tcc"
	>(0xd600000UL, 0xffe00000UL, DecodeOpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38371 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38375 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38380 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38384 "isa_tms320.tcc"
	>(0xd400000UL, 0xffe00000UL, DecodeOpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38388 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38392 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38397 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38401 "isa_tms320.tcc"
	>(0xd200000UL, 0xffe00000UL, DecodeOpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38405 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38409 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38414 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38418 "isa_tms320.tcc"
	>(0xd000000UL, 0xffe0ffe0UL, DecodeOpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38422 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38426 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38431 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38435 "isa_tms320.tcc"
	>(0xc600000UL, 0xffe00000UL, DecodeOpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38439 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38443 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38448 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38452 "isa_tms320.tcc"
	>(0xc400000UL, 0xffe00000UL, DecodeOpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38456 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38460 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38465 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38469 "isa_tms320.tcc"
	>(0xc200000UL, 0xffe00000UL, DecodeOpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38473 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38477 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38482 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38486 "isa_tms320.tcc"
	>(0xc000000UL, 0xffe0ffe0UL, DecodeOpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38494 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38499 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38503 "isa_tms320.tcc"
	>(0xbe00000UL, 0xffe00000UL, DecodeOpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38507 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38511 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38516 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38520 "isa_tms320.tcc"
	>(0xbc00000UL, 0xffe00000UL, DecodeOpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38528 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38533 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38537 "isa_tms320.tcc"
	>(0xba00000UL, 0xffe00000UL, DecodeOpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38541 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38545 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38550 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38554 "isa_tms320.tcc"
	>(0xb800000UL, 0xffe0ffe0UL, DecodeOpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38562 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38567 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38571 "isa_tms320.tcc"
	>(0xb600000UL, 0xffe00000UL, DecodeOpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38575 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38579 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38584 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38588 "isa_tms320.tcc"
	>(0xb400000UL, 0xffe00000UL, DecodeOpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38592 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38596 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38605 "isa_tms320.tcc"
	>(0xb200000UL, 0xffe00000UL, DecodeOpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38609 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38613 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38618 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38622 "isa_tms320.tcc"
	>(0xb000000UL, 0xffe0ffe0UL, DecodeOpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38626 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38630 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38635 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38639 "isa_tms320.tcc"
	>(0xae00000UL, 0xffe00000UL, DecodeOpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38643 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38647 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38652 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38656 "isa_tms320.tcc"
	>(0xac00000UL, 0xffe00000UL, DecodeOpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38660 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38664 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38669 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38673 "isa_tms320.tcc"
	>(0xaa00000UL, 0xffe00000UL, DecodeOpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38677 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38681 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38686 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38690 "isa_tms320.tcc"
	>(0xa800000UL, 0xffe0ffe0UL, DecodeOpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38694 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38698 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38703 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38707 "isa_tms320.tcc"
	>(0xa600000UL, 0xffe00000UL, DecodeOpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38711 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38715 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38720 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38724 "isa_tms320.tcc"
	>(0xa400000UL, 0xffe00000UL, DecodeOpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38728 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38732 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38737 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38741 "isa_tms320.tcc"
	>(0xa200000UL, 0xffe00000UL, DecodeOpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38745 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38749 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38754 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38758 "isa_tms320.tcc"
	>(0xa000000UL, 0xffe0ffe0UL, DecodeOpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38762 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38766 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38771 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38775 "isa_tms320.tcc"
	>(0x9e00000UL, 0xffe00000UL, DecodeOpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38779 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38783 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38788 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38792 "isa_tms320.tcc"
	>(0x9c00000UL, 0xffe00000UL, DecodeOpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38800 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38805 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38809 "isa_tms320.tcc"
	>(0x9a00000UL, 0xffe00000UL, DecodeOpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38813 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38817 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38822 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38826 "isa_tms320.tcc"
	>(0x9800000UL, 0xffe0ffe0UL, DecodeOpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38830 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38834 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38839 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38843 "isa_tms320.tcc"
	>(0x5e00000UL, 0xffe00000UL, DecodeOpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38847 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38851 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38856 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38860 "isa_tms320.tcc"
	>(0x5c00000UL, 0xffe00000UL, DecodeOpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38864 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38868 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38873 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38877 "isa_tms320.tcc"
	>(0x5a00000UL, 0xffe00000UL, DecodeOpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38881 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38885 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38890 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38894 "isa_tms320.tcc"
	>(0x5800000UL, 0xffe0ffe0UL, DecodeOpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38898 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38902 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38907 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38911 "isa_tms320.tcc"
	>(0x5600000UL, 0xffe00000UL, DecodeOpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38915 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38919 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38924 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38928 "isa_tms320.tcc"
	>(0x5400000UL, 0xffe00000UL, DecodeOpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38932 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38936 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38941 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38945 "isa_tms320.tcc"
	>(0x5200000UL, 0xffe00000UL, DecodeOpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38949 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38953 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38958 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38962 "isa_tms320.tcc"
	>(0x5000000UL, 0xffe0ffe0UL, DecodeOpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38966 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38970 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38975 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38979 "isa_tms320.tcc"
	>(0x4e00000UL, 0xffe00000UL, DecodeOpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38983 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38987 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38992 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38996 "isa_tms320.tcc"
	>(0x4c00000UL, 0xffe00000UL, DecodeOpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39000 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39004 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39009 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39013 "isa_tms320.tcc"
	>(0x4a00000UL, 0xffe00000UL, DecodeOpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39017 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39021 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39026 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39030 "isa_tms320.tcc"
	>(0x4800000UL, 0xffe0ffe0UL, DecodeOpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39034 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39038 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39043 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39047 "isa_tms320.tcc"
	>(0x4600000UL, 0xffe00000UL, DecodeOpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39051 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39055 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39060 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39064 "isa_tms320.tcc"
	>(0x4400000UL, 0xffe00000UL, DecodeOpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39068 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39072 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39077 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39081 "isa_tms320.tcc"
	>(0x4200000UL, 0xffe00000UL, DecodeOpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39085 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39089 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39094 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39098 "isa_tms320.tcc"
	>(0x4000000UL, 0xffe0ffe0UL, DecodeOpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39102 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39106 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39111 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39115 "isa_tms320.tcc"
	>(0x3e00000UL, 0xffe00000UL, DecodeOpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39119 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39123 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39128 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39132 "isa_tms320.tcc"
	>(0x3c00000UL, 0xffe00000UL, DecodeOpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39136 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39140 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39145 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39149 "isa_tms320.tcc"
	>(0x3a00000UL, 0xffe00000UL, DecodeOpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39153 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39157 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39162 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39166 "isa_tms320.tcc"
	>(0x3800000UL, 0xffe0ffe0UL, DecodeOpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39170 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39174 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39179 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39183 "isa_tms320.tcc"
	>(0x3600000UL, 0xffe00000UL, DecodeOpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39187 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39191 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39196 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39200 "isa_tms320.tcc"
	>(0x3400000UL, 0xffe00000UL, DecodeOpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39204 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39208 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39213 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39217 "isa_tms320.tcc"
	>(0x3200000UL, 0xffe00000UL, DecodeOpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39221 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39225 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39230 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39234 "isa_tms320.tcc"
	>(0x3000000UL, 0xffe0ffe0UL, DecodeOpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39238 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39242 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39247 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39251 "isa_tms320.tcc"
	>(0x2e00000UL, 0xffe00000UL, DecodeOpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39255 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39259 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39264 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39268 "isa_tms320.tcc"
	>(0x2c00000UL, 0xffe00000UL, DecodeOpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39272 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39276 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39281 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39285 "isa_tms320.tcc"
	>(0x2a00000UL, 0xffe00000UL, DecodeOpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39289 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39293 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39298 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39302 "isa_tms320.tcc"
	>(0x2800000UL, 0xffe0ffe0UL, DecodeOpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39306 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39310 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39315 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39319 "isa_tms320.tcc"
	>(0x2600000UL, 0xffe00000UL, DecodeOpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39323 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39327 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39332 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39336 "isa_tms320.tcc"
	>(0x2400000UL, 0xffe00000UL, DecodeOpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39340 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39344 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39349 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39353 "isa_tms320.tcc"
	>(0x2200000UL, 0xffe00000UL, DecodeOpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39357 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39361 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39366 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39370 "isa_tms320.tcc"
	>(0x2000000UL, 0xffe0ffe0UL, DecodeOpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39374 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39378 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39383 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39387 "isa_tms320.tcc"
	>(0x1e00000UL, 0xffe00000UL, DecodeOpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39391 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39395 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39400 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39404 "isa_tms320.tcc"
	>(0x1c00000UL, 0xffe00000UL, DecodeOpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39408 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39412 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39417 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39421 "isa_tms320.tcc"
	>(0x1a00000UL, 0xffe00000UL, DecodeOpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39425 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39429 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39434 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39438 "isa_tms320.tcc"
	>(0x1800000UL, 0xffe0ffe0UL, DecodeOpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39442 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39446 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39451 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39455 "isa_tms320.tcc"
	>(0x1600000UL, 0xffe00000UL, DecodeOpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39459 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39463 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39468 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39472 "isa_tms320.tcc"
	>(0x1400000UL, 0xffe00000UL, DecodeOpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39476 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39480 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39485 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39489 "isa_tms320.tcc"
	>(0x1200000UL, 0xffe00000UL, DecodeOpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39493 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39497 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39502 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39506 "isa_tms320.tcc"
	>(0x1000000UL, 0xffe0ffe0UL, DecodeOpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39510 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39514 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39519 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39523 "isa_tms320.tcc"
	>(0xe00000UL, 0xffe00000UL, DecodeOpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39527 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39531 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39536 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39540 "isa_tms320.tcc"
	>(0xc00000UL, 0xffe00000UL, DecodeOpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39544 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39548 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39557 "isa_tms320.tcc"
	>(0xa00000UL, 0xffe00000UL, DecodeOpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39561 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39565 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39570 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39574 "isa_tms320.tcc"
	>(0x800000UL, 0xffe0ffe0UL, DecodeOpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39578 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39582 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39587 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39591 "isa_tms320.tcc"
	>(0x600000UL, 0xffe00000UL, DecodeOpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39595 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39599 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39604 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39608 "isa_tms320.tcc"
	>(0x400000UL, 0xffe00000UL, DecodeOpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39612 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39616 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39621 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39625 "isa_tms320.tcc"
	>(0x200000UL, 0xffe00000UL, DecodeOpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39629 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39633 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39638 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39642 "isa_tms320.tcc"
	>(0x0UL, 0xffe0ffe0UL, DecodeOpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39646 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39650 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39655 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39659 "isa_tms320.tcc"
	>(0x15400000UL, 0xffe00000UL, DecodeOpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39663 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39667 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39672 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39676 "isa_tms320.tcc"
	>(0x15200000UL, 0xffe00000UL, DecodeOpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39680 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39684 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39689 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39693 "isa_tms320.tcc"
	>(0x14400000UL, 0xffe00000UL, DecodeOpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39697 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39701 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39706 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39710 "isa_tms320.tcc"
	>(0x14200000UL, 0xffe00000UL, DecodeOpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39714 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39718 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39727 "isa_tms320.tcc"
	>(0xfa00000UL, 0xffe0ffffUL, DecodeOpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39731 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39735 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39740 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39744 "isa_tms320.tcc"
	>(0xf200000UL, 0xffe0ffffUL, DecodeOpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39748 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39752 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39757 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39761 "isa_tms320.tcc"
	>(0xea00000UL, 0xffe0ffffUL, DecodeOpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39765 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39769 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39774 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39778 "isa_tms320.tcc"
	>(0xe200000UL, 0xffe0ffffUL, DecodeOpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39782 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39786 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39791 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39795 "isa_tms320.tcc"
	>(0x8700000UL, 0xffffff00UL, DecodeOpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39799 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39803 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39808 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39812 "isa_tms320.tcc"
	>(0x9600000UL, 0xffe0f000UL, DecodeOpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39816 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39820 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39825 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39829 "isa_tms320.tcc"
	>(0x9400000UL, 0xffe00000UL, DecodeOpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39837 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39842 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39846 "isa_tms320.tcc"
	>(0x9200000UL, 0xffe00000UL, DecodeOpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39850 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39854 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39859 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39863 "isa_tms320.tcc"
	>(0x9000000UL, 0xffe0ffe0UL, DecodeOpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39867 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39871 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39880 "isa_tms320.tcc"
	>(0x50600000UL, 0xf0600000UL, DecodeOpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39884 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39888 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39893 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39897 "isa_tms320.tcc"
	>(0x50400000UL, 0xf0600000UL, DecodeOpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39901 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39905 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39910 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39914 "isa_tms320.tcc"
	>(0x50200000UL, 0xf0600000UL, DecodeOpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39918 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39922 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39927 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39931 "isa_tms320.tcc"
	>(0x50000000UL, 0xf060ffe0UL, DecodeOpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39935 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39939 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39948 "isa_tms320.tcc"
	>(0x8600000UL, 0xffe00000UL, DecodeOpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39952 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39956 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39961 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39965 "isa_tms320.tcc"
	>(0x8400000UL, 0xffe00000UL, DecodeOpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39969 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39973 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39978 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39982 "isa_tms320.tcc"
	>(0x8200000UL, 0xffe00000UL, DecodeOpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39986 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39990 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39995 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39999 "isa_tms320.tcc"
	>(0x8000000UL, 0xffe0ffe0UL, DecodeOpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40003 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40007 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40012 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40016 "isa_tms320.tcc"
	>(0x40600000UL, 0xf0600000UL, DecodeOpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40020 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40024 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40029 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40033 "isa_tms320.tcc"
	>(0x40400000UL, 0xf0600000UL, DecodeOpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40037 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40041 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40046 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40050 "isa_tms320.tcc"
	>(0x40200000UL, 0xf0600000UL, DecodeOpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40054 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40058 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40063 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40067 "isa_tms320.tcc"
	>(0x40000000UL, 0xf060ffe0UL, DecodeOpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40071 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40075 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40080 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40084 "isa_tms320.tcc"
	>(0x7600000UL, 0xffe00000UL, DecodeOpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40088 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40092 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40097 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40101 "isa_tms320.tcc"
	>(0x7400000UL, 0xffe00000UL, DecodeOpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40105 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40109 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40114 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40118 "isa_tms320.tcc"
	>(0x7200000UL, 0xffe00000UL, DecodeOpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40122 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40126 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40131 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40135 "isa_tms320.tcc"
	>(0x7000000UL, 0xffe0ffe0UL, DecodeOpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40139 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40143 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40148 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40152 "isa_tms320.tcc"
	>(0x6e00000UL, 0xffe00000UL, DecodeOpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40156 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40160 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40165 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40169 "isa_tms320.tcc"
	>(0x6c00000UL, 0xffe00000UL, DecodeOpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40173 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40177 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40186 "isa_tms320.tcc"
	>(0x6a00000UL, 0xffe00000UL, DecodeOpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40190 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40194 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40199 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40203 "isa_tms320.tcc"
	>(0x6800000UL, 0xffe0ffe0UL, DecodeOpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40207 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40211 "isa_tms320.tcc"
	>));
}

template <
#line 45 "isa/tms320.isa"
class
#line 40218 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40228 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40237 "isa_tms320.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 45 "isa/tms320.isa"
class
#line 40246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40256 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40265 "isa_tms320.tcc"
>::Fetch(void *, typename CONFIG::address_t, uint32_t)
{
	assert( ! "Calling unimplemented virtual method Fetch" );
}

template <
#line 45 "isa/tms320.isa"
class
#line 40274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40284 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40293 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40297 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40301 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40307 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40311 "isa_tms320.tcc"
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
#line 40336 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40340 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40355 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40358 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40367 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40375 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40381 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40385 "isa_tms320.tcc"
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
#line 40400 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40404 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40422 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40431 "isa_tms320.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 40441 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 40445 "isa_tms320.tcc"
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
#line 40464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40474 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40483 "isa_tms320.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40492 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40496 "isa_tms320.tcc"
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
#line 40529 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40532 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40539 "isa_tms320.tcc"
>
inline DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 40544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40548 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40556 "isa_tms320.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40564 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40568 "isa_tms320.tcc"
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
#line 40602 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40605 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40609 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40612 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40621 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40629 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40635 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40639 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40645 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40649 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 40657 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 40661 "isa_tms320.tcc"
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
#line 40681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40691 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 40696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40700 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40708 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40714 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40718 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40724 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40728 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 40736 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 40740 "isa_tms320.tcc"
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
#line 40762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40772 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40781 "isa_tms320.tcc"
>::SetLittleEndian()
{
	little_endian = true;
}

template <
#line 45 "isa/tms320.isa"
class
#line 40790 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 40793 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 40797 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 40800 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 40805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 40809 "isa_tms320.tcc"
>::SetBigEndian()
{
	little_endian = false;
}

} } } } } } }
