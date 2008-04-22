/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_FPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_FPU_TCC__

#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

//using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;

template <class CONFIG>
FPU<CONFIG>::FPU(const char *name, Object *parent) :
	Object(name, parent),
	Client<Logger>(name, parent),
	logger_import("logger-import", this),
	fp32_estimate_inv_warning(false),
	fp64_estimate_inv_sqrt_warning(false)
{
	int i;

	registers_registry["fpscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpscr", &fpscr);

	for(i = 0; i < 32; i++)
	{
		stringstream sstr;
		sstr << "f" << i;
		registers_registry[sstr.str()] = new FloatingPointRegisterInterface(sstr.str().c_str(), &fpr[i]);
	}
}

template <class CONFIG>
FPU<CONFIG>::~FPU()
{
	typename map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
}

template <class CONFIG>
void FPU<CONFIG>::EnableException()
{
	exception_enabled = true;
}

template <class CONFIG>
void FPU<CONFIG>::DisableException()
{
	exception_enabled = false;
}

template <class CONFIG>
unisim::util::debug::Register *FPU<CONFIG>::GetRegister(const char *name)
{
	typename map<string, unisim::util::debug::Register *>::iterator reg_iter = registers_registry.find(name);
	return reg_iter != registers_registry.end() ? (*reg_iter).second : 0;
}

template <class CONFIG>
void FPU<CONFIG>::Reset()
{
	int i;
	
	SetFPSCR(0);
	exception_enabled = false;
	flags.clear();
	for(i = 0; i < 32; i++) SetFp64(i, 0);
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_FEX()
{
	if((GetFPSCR_VX() & GetFPSCR_VE()) | (GetFPSCR_OX() & GetFPSCR_OE()) | (GetFPSCR_UX() & GetFPSCR_UE()) | (GetFPSCR_ZX() & GetFPSCR_ZE()) | (GetFPSCR_XX() & GetFPSCR_XE()))
		SetFPSCR_FEX();
	else
		ResetFPSCR_FEX();
	
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_FPRF(const SoftFloat& result)
{
	if(result.isQNaN())
	{
		SetFPSCR_FPRF(FPRF_FOR_QNAN);
	}
	else if(result.isInfty())
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_INFINITY);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_INFINITY);
	}
	else if(result.isDenormalized())
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_DENORMAL);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_DENORMAL);
	}
	else if(result.isZero())
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_ZERO);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_ZERO);
	}
	else
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_NORMAL);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_NORMAL);
	}
}


template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_FPRF(const SoftDouble& result)
{
	if(result.isQNaN())
	{
		SetFPSCR_FPRF(FPRF_FOR_QNAN);
	}
	else if(result.isInfty())
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_INFINITY);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_INFINITY);
	}
	else if(result.isDenormalized())
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_DENORMAL);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_DENORMAL);
	}
	else if(result.isZero())
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_ZERO);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_ZERO);
	}
	else
	{
		if(result.isNegative())
			SetFPSCR_FPRF(FPRF_FOR_NEGATIVE_NORMAL);
		else
			SetFPSCR_FPRF(FPRF_FOR_POSITIVE_NORMAL);
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_FI(const Flags& flags)
{
	if(flags.isApproximate())
	{
		SetFPSCR_FI();
		SetFPSCR_FX();
		SetFPSCR_XX();
	}
	else
	{
		ResetFPSCR_FI();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_OX(const Flags& flags)
{
	if(flags.hasFlowException() && flags.isApproximate())
	{
		if(flags.isOverflow())
		{
			SetFPSCR_OX();
			SetFPSCR_FX();
		}
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_UX(const Flags& flags)
{
	if(flags.hasFlowException() && flags.isApproximate())
	{
		if(flags.isUnderflow())
		{
			SetFPSCR_UX();
			SetFPSCR_FX();
		}
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_ZX(const Flags& flags)
{
	if(flags.isDivisionByZero())
	{
		SetFPSCR_ZX();
		SetFPSCR_FX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_VXSNAN(const Flags& flags)
{
	if(flags.hasSNaNOperand())
	{
		SetFPSCR_VXSNAN();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_VXISI(const Flags& flags)
{
	if(flags.isInftyMinusInfty())
	{
		SetFPSCR_VXISI();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_VXIDI(const Flags& flags)
{
	if(flags.isInftyOnInfty())
	{
		SetFPSCR_VXIDI();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_VXZDZ(const Flags& flags)
{
	if(flags.isZeroOnZero())
	{
		SetFPSCR_VXZDZ();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_VXIMZ(const Flags& flags)
{
	if(flags.isInftyMultZero())
	{
		SetFPSCR_VXIMZ();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_FR(const SoftFloat& result, const Flags& flags)
{
	if(flags.hasIncrementFraction(result.isNegative()))
		SetFPSCR_FR();
	else
		ResetFPSCR_FR();
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_VXCVI(const Flags& flags)
{
	if(flags.isOverflow())
	{
		SetFPSCR_VXCVI();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
}

template <class CONFIG>
void FPU<CONFIG>::ComputeFPSCR_FR(const SoftDouble& result, const Flags& flags)
{
	if(flags.hasIncrementFraction(result.isNegative()))
		SetFPSCR_FR();
	else
		ResetFPSCR_FR();
}


template <class CONFIG>
void FPU<CONFIG>::Fp32Add(unsigned int fd, unsigned int fa, unsigned int fb)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	SoftFloat b(fpr[fb], flagsConvert);
	
	// Compute the result
	result.plusAssign(b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flagsConvert);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();

	flags.clear();
	
	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32Sub(unsigned int fd, unsigned int fa, unsigned int fb)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	SoftFloat b(fpr[fb], flagsConvert);

	// Compute the result	
	result.minusAssign(b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flagsConvert);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32Mul(unsigned int fd, unsigned int fa, unsigned int fb)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	SoftFloat b(fpr[fb], flagsConvert);

	// Compute the result	
	result.multAssign(b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flagsConvert);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32Div(unsigned int fd, unsigned int fa, unsigned int fb)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	SoftFloat b(fpr[fb], flagsConvert);

	// Compute the result	
	result.divAssign(b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	// Compute FPSCR[ZX]
	ComputeFPSCR_ZX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flagsConvert);
	
	// Compute FPSCR[VXIDI]
	ComputeFPSCR_VXIDI(flags);
	
	// Compute FPSCR[VXZDZ]
	ComputeFPSCR_VXZDZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32EstimateInv(unsigned int fd, unsigned int fa)
{
	if(logger_import && !fp32_estimate_inv_warning)
	{
		fp32_estimate_inv_warning = true;
		(*logger_import) << DebugWarning << "Floating point 1/x estimation (fres) is implemented with a precise algorithm, not an estimation algorithm. FPSCR flags may be incorrect." << Endl << EndDebugWarning;
	}

	Flags flagsConvert = flags;
	SoftFloat result(0x3f800000ULL); // 1.0
	SoftFloat a(fpr[fa], flagsConvert);

	// Compute the result	
	result.divAssign(a, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	// Compute FPSCR[ZX]
	ComputeFPSCR_ZX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flagsConvert);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32MulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	flagsConvert.clear();
	SoftFloat b(fpr[fb], flagsConvert);
	flagsConvert.clear();
	SoftFloat c(fpr[fc], flagsConvert);
	
	// Compute the result
	result.multAndAddAssign(c, b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32MulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	flagsConvert.clear();
	SoftFloat b(fpr[fb], flagsConvert);
	flagsConvert.clear();
	SoftFloat c(fpr[fc], flagsConvert);
	
	// Compute the result
	result.multAndSubAssign(c, b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32NegMulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	flagsConvert.clear();
	SoftFloat b(fpr[fb], flagsConvert);
	flagsConvert.clear();
	SoftFloat c(fpr[fc], flagsConvert);
	
	// Compute the result
	result.multAndAddAssign(c, b, flags);

	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	if(!result.isQNaN()) result.opposite();
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp32NegMulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	Flags flagsConvert = flags;
	SoftFloat result(fpr[fa], flagsConvert);
	flagsConvert.clear();
	SoftFloat b(fpr[fb], flagsConvert);
	flagsConvert.clear();
	SoftFloat c(fpr[fc], flagsConvert);
	
	// Compute the result
	result.multAndSubAssign(c, b, flags);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	if(!result.isQNaN()) result.opposite();
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Add(unsigned int fd, unsigned int fa, unsigned int fb)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];

	// Compute the result
	result.plusAssign(b, flags);
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Sub(unsigned int fd, unsigned int fa, unsigned int fb)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];
	
	// Compute the result
	result.minusAssign(b, flags);
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Mul(unsigned int fd, unsigned int fa, unsigned int fb)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];

	// Compute the result
	result.multAssign(b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Div(unsigned int fd, unsigned int fa, unsigned int fb)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];

	// Compute the result
	result.divAssign(b, flags);

	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	// Compute FPSCR[ZX]
	ComputeFPSCR_ZX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXIDI]
	ComputeFPSCR_VXIDI(flags);
	
	// Compute FPSCR[VXZDZ]
	ComputeFPSCR_VXZDZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64EstimateInvSqrt(unsigned int fd, unsigned int fa)
{
	if(logger_import && !fp64_estimate_inv_sqrt_warning)
	{
		fp64_estimate_inv_sqrt_warning = true;
		(*logger_import) << DebugWarning << "Floating point 1/square_root(x) estimation (frsqrte) is is implemented with a precise algorithm, not an estimation algorithm. FPSCR flags may be incorrect." << Endl << EndDebugWarning;
	}

	SoftDouble& b = fpr[fa];
	SoftDouble result(0x3ff0000000000000ULL); // 1.0

	if(b.isNegative() && !b.isZero())
	{
		SetFPSCR_VXSQRT();
	}
	else
	{
		ResetFPSCR_VXSQRT();

		SoftDouble half(0x3fe0000000000000ULL); // 0.5
		SoftDouble u(b);
		SoftDouble v;
	
		// Compute the square root using the babylonian method
		// U[i+1] = 0.5 * (U[i] + b/U[i])
		while(1)
		{
			SoftDouble b_over_u(b); // b
			b_over_u.divAssign(u, flags); // b / u
			SoftDouble u_plus_b_over_u(u); // u
			u_plus_b_over_u.plusAssign(b_over_u, flags); // u + b/u
			u = u_plus_b_over_u;
			u.multAssign(half, flags); // 0.5 * (u + b/u)
			if(u.compare(v) == SoftDouble::CREqual) break;
			v = u;
		}
		// u = sqrt(b)

		result.divAssign(u, flags); // result = 1/sqrt(b)
	
		// Compute FPSCR[FPRF]
		ComputeFPSCR_FPRF(result);
		
		// Compute FPSCR[FR]
		ComputeFPSCR_FR(result, flags);
		
		// Compute FPSCR[FI]
		ComputeFPSCR_FI(flags);
		
		// Compute FPSCR[ZX]
		ComputeFPSCR_ZX(flags);
		
		ResetFPSCR_VX();
		
		// Compute FPSCR[VXSNAN]
		ComputeFPSCR_VXSNAN(flags);
	}
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64MulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];
	SoftDouble& c = fpr[fc];

	// Compute the result
	result.multAndAddAssign(c, b, flags);
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64MulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];
	SoftDouble& c = fpr[fc];

	// Compute the result
	result.multAndSubAssign(c, b, flags);
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64NegMulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];
	SoftDouble& c = fpr[fc];

	// Compute the result
	result.multAndAddAssign(c, b, flags);

	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	if(!result.isQNaN()) result.opposite();
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64NegMulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc)
{
	SoftDouble result = fpr[fa];
	SoftDouble& b = fpr[fb];
	SoftDouble& c = fpr[fc];

	// Compute the result
	result.multAndSubAssign(c, b, flags);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	if(!result.isQNaN()) result.opposite();
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXISI]
	ComputeFPSCR_VXISI(flags);
	
	// Compute FPSCR[VXIMZ]
	ComputeFPSCR_VXIMZ(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = result;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Neg(unsigned int fd, unsigned int fa)
{
	SoftDouble& result = fpr[fd];
	SoftDouble& a = fpr[fa];
	if(fd == fa)
		result.opposite();
	else
		(result = a).opposite();
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Abs(unsigned int fd, unsigned int fa)
{
	SoftDouble& result = fpr[fd];
	SoftDouble& a = fpr[fa];

	if(fd != fa)
	{
		result = a;
	}
	if(result.isNegative())
	{
		result.opposite();
	}
}

template <class CONFIG>
void FPU<CONFIG>::Fp64NegAbs(unsigned int fd, unsigned int fa)
{
	SoftDouble& result = fpr[fd];
	SoftDouble& a = fpr[fa];

	if(fd != fa)
	{
		result = a;
	}
	if(result.isPositive())
	{
		result.opposite();
	}
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Select(unsigned int fd, unsigned int fa, unsigned int fb, unsigned int fc)
{
	SoftDouble& a = fpr[fa];
	SoftDouble& b = fpr[fb];
	SoftDouble& c = fpr[fc];
	
	fpr[fd] = ((!a.isZero() && a.isNegative()) || a.isNaN()) ? b : c;
}

template <class CONFIG>
uint32_t FPU<CONFIG>::Fp64CompareOrdered(unsigned int fa, unsigned int fb)
{
	uint32_t c;
	SoftDouble& a = fpr[fa];
	SoftDouble& b = fpr[fb];
	
	// Do the floating point comparison
	switch(a.compare(b))
	{
		case SoftDouble::CRNaN: c = 1; break;
		case SoftDouble::CRLess: c = 8; break;
		case SoftDouble::CREqual: c = 2; break;
		case SoftDouble::CRGreater: c = 4; break;
		default:
			if(logger_import)
				(*logger_import) << DebugError << "Internal error" << Endl << EndDebugError;
			abort();
	}

	// Compute FPSCR[FPCC]
	SetFPSCR_FPCC(c);

	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN] and FPSCR[VXVC]
	if(a.isSNaN() || b.isSNaN())
	{
		SetFPSCR_VXSNAN();
		if(!GetFPSCR_VE()) SetFPSCR_VXVC();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
	if(a.isQNaN() || b.isQNaN())
	{
		SetFPSCR_VXVC();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
	
	// return value to set into CR[crfD]
	return c;
}

template <class CONFIG>
uint32_t FPU<CONFIG>::Fp64CompareUnordered(unsigned int fa, unsigned int fb)
{
	uint32_t c;
	SoftDouble& a = fpr[fa];
	SoftDouble& b = fpr[fb];
	
	// Do the floating point comparison
	switch(a.compare(b))
	{
		case SoftDouble::CRNaN: c = 1; break;
		case SoftDouble::CRLess: c = 8; break;
		case SoftDouble::CREqual: c = 2; break;
		case SoftDouble::CRGreater: c = 4; break;
		default:
			if(logger_import)
				(*logger_import) << DebugError << "Internal error" << Endl << EndDebugError;
			abort();
	}

	// Compute FPSCR[FPCC]	
	SetFPSCR_FPCC(c);

	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	if(a.isSNaN() || b.isSNaN())
	{
		SetFPSCR_VXSNAN();
		SetFPSCR_FX();
		SetFPSCR_VX();
	}
	
	// return value to put into CR[crfD]
	return c;
}

template <class CONFIG>
void FPU<CONFIG>::Fp64ToInt32(unsigned int fd, unsigned int fa)
{
	SoftDouble::IntConversion icResult;
	SoftDouble& a = fpr[fa];
	uint32_t result32;
	uint64_t result64;
	
    flags.clearKeepSignalingConversion();
	a.retrieveInteger(icResult.setSigned(), flags);
    flags.setKeepSignalingConversion();
	result32 = (uint32_t) icResult.queryInt();
	result64 = 0xfff8000000000000ULL | result32;
	if(result32 == 0 && a.isNegative())
		result64 |= 0x0000000100000000ULL;
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(a, flags);
	
	// Compute FPSCR[FI]
	if(!flags.isOverflow())
		ComputeFPSCR_FI(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXICI]
	ComputeFPSCR_VXCVI(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = SoftDouble(result64);
}

template <class CONFIG>
void FPU<CONFIG>::Fp64ToInt32TowardZero(unsigned int fd, unsigned int fa)
{
	SoftDouble::IntConversion icResult;
	SoftDouble& a = fpr[fa];
	uint32_t result32;
	uint64_t result64;
	
	flags.setRoundingMode(RN_ZERO);	
    flags.clearKeepSignalingConversion();
	a.retrieveInteger(icResult.setSigned(), flags);
	flags.setRoundingMode(GetFPSCR_RN() & 3);
    flags.setKeepSignalingConversion();
	result32 = (uint32_t) icResult.queryInt();
	result64 = 0xfff8000000000000ULL | result32;
	if(result32 == 0 && a.isNegative())
		result64 |= 0x0000000100000000ULL;
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(a, flags);
	
	// Compute FPSCR[FI]
	if(!flags.isOverflow())
		ComputeFPSCR_FI(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[VXCVI]
	ComputeFPSCR_VXCVI(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd] = SoftDouble(result64);
}

template <class CONFIG>
void FPU<CONFIG>::Fp64ToFp32(unsigned int fd, unsigned int fa)
{
	Flags flagsConvert;
	SoftDouble& a = fpr[fa];
	
    flags.clearKeepSignalingConversion();
	flags.setUpApproximateInfty();
	SoftFloat result(a, flags);
	flags.clearUpApproximateInfty();
    flags.setKeepSignalingConversion();
	
	// Compute FPSCR[FPRF]
	ComputeFPSCR_FPRF(result);
	
	// Compute FPSCR[FR]
	ComputeFPSCR_FR(result, flags);
	
	// Compute FPSCR[FI]
	ComputeFPSCR_FI(flags);
	
	// Compute FPSCR[OX]
	ComputeFPSCR_OX(flags);	
	
	// Compute FPSCR[UX]
	ComputeFPSCR_UX(flags);
	
	ResetFPSCR_VX();
	
	// Compute FPSCR[VXSNAN]
	ComputeFPSCR_VXSNAN(flags);
	
	// Compute FPSCR[FEX]
	ComputeFPSCR_FEX();
	
	flags.clear();

	if(exception_enabled && GetFPSCR_FEX())
	{
		// raise a floating point exception if FPSCR[FEX] is set
		throw FloatingPointException<CONFIG>();
	}

	// write back the result
	fpr[fd].assign(result, flagsConvert);
}

template <class CONFIG>
void FPU<CONFIG>::Fp64Move(unsigned int fd, unsigned int fa)
{
	fpr[fd] = fpr[fa];
}

template <class CONFIG>
uint64_t FPU<CONFIG>::GetFp64(unsigned int n)
{
	return fpr[n].queryValue();
}

template <class CONFIG>
void FPU<CONFIG>::SetFp64(unsigned int n, uint64_t value)
{
	fpr[n] = SoftDouble(value);
}

template <class CONFIG>
uint32_t FPU<CONFIG>::GetFp32(unsigned int n)
{
	uint32_t value = SoftFloat(fpr[n], flags).queryValue();
	flags.clear();
	return value;
}

template <class CONFIG>
void FPU<CONFIG>::SetFp32(unsigned int n, uint32_t value)
{
	SoftDouble& result = fpr[n];
	result.assign(SoftFloat(value), flags);
	flags.clear();
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
