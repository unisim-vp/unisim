/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_FLOATING_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_FLOATING_HH__

#include <unisim/component/cxx/processor/powerpc/floating.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template <class CONFIG, class FLOAT>
inline void GenFPSCR_FPRF(uint32_t& fpscr, const FLOAT& result)
{
	fpscr = fpscr & ~CONFIG::FPSCR_FPRF_MASK;
	
	if(unlikely(result.isQNaN()))
	{
		fpscr = fpscr | (CONFIG::FPRF_FOR_QNAN << CONFIG::FPSCR_FPRF_OFFSET);
	}
	else if(unlikely(result.isInfty()))
	{
		if(result.isNegative())
			fpscr = fpscr | (CONFIG::FPRF_FOR_NEGATIVE_INFINITY << CONFIG::FPSCR_FPRF_OFFSET);
		else
			fpscr = fpscr | (CONFIG::FPRF_FOR_POSITIVE_INFINITY << CONFIG::FPSCR_FPRF_OFFSET);
	}
	else if(unlikely(result.isDenormalized()))
	{
		if(result.isNegative())
			fpscr = fpscr | (CONFIG::FPRF_FOR_NEGATIVE_DENORMAL << CONFIG::FPSCR_FPRF_OFFSET);
		else
			fpscr = fpscr | (CONFIG::FPRF_FOR_POSITIVE_DENORMAL << CONFIG::FPSCR_FPRF_OFFSET);
	}
	else if(unlikely(result.isZero()))
	{
		if(result.isNegative())
			fpscr = fpscr | (CONFIG::FPRF_FOR_NEGATIVE_ZERO << CONFIG::FPSCR_FPRF_OFFSET);
		else
			fpscr = fpscr | (CONFIG::FPRF_FOR_POSITIVE_ZERO << CONFIG::FPSCR_FPRF_OFFSET);
	}
	else
	{
		if(result.isNegative())
			fpscr = fpscr | (CONFIG::FPRF_FOR_NEGATIVE_NORMAL << CONFIG::FPSCR_FPRF_OFFSET);
		else
			fpscr = fpscr | (CONFIG::FPRF_FOR_POSITIVE_NORMAL << CONFIG::FPSCR_FPRF_OFFSET);
	}
}

template <class CONFIG, class FLOAT>
inline void GenFPSCR_FR(uint32_t& fpscr, const FLOAT& result, const Flags& flags)
{
	if(flags.hasIncrementFraction(result.isNegative()))
		fpscr = fpscr | CONFIG::FPSCR_FR_MASK;
	else
		fpscr = fpscr & ~CONFIG::FPSCR_FR_MASK;
}

template <class CONFIG>
inline void GenFPSCR_FI(uint32_t& fpscr, const Flags& flags)
{
	if(flags.isApproximate())
		fpscr = fpscr | CONFIG::FPSCR_FI_MASK;
	else
		fpscr = fpscr & ~CONFIG::FPSCR_FI_MASK;
}

template <class CONFIG>
inline void GenFPSCR_OX(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.hasFlowException() && flags.isApproximate()))
	{
		if(flags.isOverflow())
		{
			fpscr = fpscr | CONFIG::FPSCR_OX_MASK;
		}
	}
}

template <class CONFIG>
inline void GenFPSCR_UX(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.hasFlowException() && flags.isApproximate()))
	{
		if(flags.isUnderflow())
		{
			fpscr = fpscr | CONFIG::FPSCR_UX_MASK;
		}
	}
}

template <class CONFIG>
inline void GenFPSCR_ZX(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.isDivisionByZero()))
	{
		fpscr = fpscr | CONFIG::FPSCR_ZX_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_VXSNAN(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.hasSNaNOperand()))
	{
		fpscr = fpscr | CONFIG::FPSCR_VXSNAN_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_VXISI(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.isInftyMinusInfty()))
	{
		fpscr = fpscr | CONFIG::FPSCR_VXISI_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_VXIDI(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.isInftyOnInfty()))
	{
		fpscr = fpscr | CONFIG::FPSCR_VXIDI_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_VXZDZ(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.isZeroOnZero()))
	{
		fpscr = fpscr | CONFIG::FPSCR_VXZDZ_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_VXIMZ(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.isInftyMultZero()))
	{
		fpscr = fpscr | CONFIG::FPSCR_VXIMZ_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_VXCVI(uint32_t& fpscr, const Flags& flags)
{
	if(unlikely(flags.isOverflow()))
	{
		fpscr = fpscr | CONFIG::FPSCR_VXCVI_MASK;
	}
}

template <class CONFIG>
inline void GenFPSCR_XX(uint32_t& fpscr)
{
	fpscr = fpscr | ((fpscr & CONFIG::FPSCR_FI_MASK) ? CONFIG::FPSCR_XX_MASK : 0);
}

template <class CONFIG>
inline void GenFPSCR_FX(uint32_t& fpscr, uint32_t old_fpscr)
{
	fpscr = fpscr |
	       (((~old_fpscr & fpscr & CONFIG::FPSCR_OX_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_UX_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_ZX_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_XX_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXSNAN_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXISI_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXIDI_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXZDZ_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXIMZ_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXVC_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXSOFT_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXSQRT_MASK) |
	         (~old_fpscr & fpscr & CONFIG::FPSCR_VXCVI_MASK)) ? CONFIG::FPSCR_FX_MASK : 0);
}

template <class CONFIG>
inline void GenFPSCR_VX(uint32_t& fpscr)
{
	fpscr = (fpscr & ~CONFIG::FPSCR_VX_MASK) |
	         (((fpscr & CONFIG::FPSCR_VXSNAN_MASK) |
	           (fpscr & CONFIG::FPSCR_VXISI_MASK) |
	           (fpscr & CONFIG::FPSCR_VXIDI_MASK) |
	           (fpscr & CONFIG::FPSCR_VXZDZ_MASK) |
	           (fpscr & CONFIG::FPSCR_VXIMZ_MASK) |
	           (fpscr & CONFIG::FPSCR_VXVC_MASK) |
	           (fpscr & CONFIG::FPSCR_VXSOFT_MASK) |
	           (fpscr & CONFIG::FPSCR_VXSQRT_MASK) |
	           (fpscr & CONFIG::FPSCR_VXCVI_MASK)) ? CONFIG::FPSCR_VX_MASK : 0);
}

template <class CONFIG>
inline void GenFPSCR_FEX(uint32_t& fpscr)
{
	fpscr = (fpscr & ~CONFIG::FPSCR_FEX_MASK) |
	         ((((fpscr & CONFIG::FPSCR_VX_MASK) && (fpscr & CONFIG::FPSCR_VE_MASK)) ||
	           ((fpscr & CONFIG::FPSCR_OX_MASK) && (fpscr & CONFIG::FPSCR_OE_MASK)) ||
	           ((fpscr & CONFIG::FPSCR_UX_MASK) && (fpscr & CONFIG::FPSCR_UE_MASK)) ||
	           ((fpscr & CONFIG::FPSCR_ZX_MASK) && (fpscr & CONFIG::FPSCR_ZE_MASK)) ||
	           ((fpscr & CONFIG::FPSCR_XX_MASK) && (fpscr & CONFIG::FPSCR_XE_MASK))) ? CONFIG::FPSCR_FEX_MASK : 0);
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_FLOATING_HH__
