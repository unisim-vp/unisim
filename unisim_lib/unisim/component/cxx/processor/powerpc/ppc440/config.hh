/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CONFIG_HH__

#include <unisim/component/cxx/processor/powerpc/config.hh>
#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
	
template <class CONFIG>
class CacheLine;
	
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

namespace unisim {
namespace component {
namespace cxx {
namespace tlb {
	
template <class CONFIG>
class TLBEntry;
	
} // end of namespace tlb
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
class CPU;

class MSRLayout
{
public:
	// MSR fields offsets
	static const unsigned int MSR_WE_OFFSET = 18;
	static const unsigned int MSR_CE_OFFSET = 17;
	static const unsigned int MSR_EE_OFFSET = 15;
	static const unsigned int MSR_PR_OFFSET = 14;
	static const unsigned int MSR_FP_OFFSET = 13;
	static const unsigned int MSR_ME_OFFSET = 12;
	static const unsigned int MSR_FE0_OFFSET = 11;
	static const unsigned int MSR_DWE_OFFSET = 10;
	static const unsigned int MSR_DE_OFFSET = 9;
	static const unsigned int MSR_FE1_OFFSET = 8;
	static const unsigned int MSR_IS_OFFSET = 5;
	static const unsigned int MSR_DS_OFFSET = 4;

	// MSR fields sizes
	static const unsigned int MSR_WE_BITSIZE = 1;
	static const unsigned int MSR_CE_BITSIZE = 1;
	static const unsigned int MSR_EE_BITSIZE = 1;
	static const unsigned int MSR_PR_BITSIZE = 1;
	static const unsigned int MSR_FP_BITSIZE = 1;
	static const unsigned int MSR_ME_BITSIZE = 1;
	static const unsigned int MSR_FE0_BITSIZE = 1;
	static const unsigned int MSR_DWE_BITSIZE = 1;
	static const unsigned int MSR_DE_BITSIZE = 1;
	static const unsigned int MSR_FE1_BITSIZE = 1;
	static const unsigned int MSR_IS_BITSIZE = 1;
	static const unsigned int MSR_DS_BITSIZE = 1;

	static const uint32_t MSR_WE_MASK = ((1UL << MSR_WE_BITSIZE) - 1) << MSR_WE_OFFSET;
	static const uint32_t MSR_CE_MASK = ((1UL << MSR_CE_BITSIZE) - 1) << MSR_CE_OFFSET;
	static const uint32_t MSR_EE_MASK = ((1UL << MSR_EE_BITSIZE) - 1) << MSR_EE_OFFSET;
	static const uint32_t MSR_PR_MASK = ((1UL << MSR_PR_BITSIZE) - 1) << MSR_PR_OFFSET;
	static const uint32_t MSR_FP_MASK = ((1UL << MSR_FP_BITSIZE) - 1) << MSR_FP_OFFSET;
	static const uint32_t MSR_ME_MASK = ((1UL << MSR_ME_BITSIZE) - 1) << MSR_ME_OFFSET;
	static const uint32_t MSR_FE0_MASK = ((1UL << MSR_FE0_BITSIZE) - 1) << MSR_FE0_OFFSET;
	static const uint32_t MSR_DWE_MASK = ((1UL << MSR_DWE_BITSIZE) - 1) << MSR_DWE_OFFSET;
	static const uint32_t MSR_DE_MASK = ((1UL << MSR_DE_BITSIZE) - 1) << MSR_DE_OFFSET;
	static const uint32_t MSR_FE1_MASK = ((1UL << MSR_FE1_BITSIZE) - 1) << MSR_FE1_OFFSET;
	static const uint32_t MSR_IS_MASK = ((1UL << MSR_IS_BITSIZE) - 1) << MSR_IS_OFFSET;
	static const uint32_t MSR_DS_MASK = ((1UL << MSR_DS_BITSIZE) - 1) << MSR_DS_OFFSET;

	// MSR bitwise AND mask
	static const uint32_t MSR_MASK = MSR_WE_MASK | MSR_CE_MASK | MSR_EE_MASK | MSR_PR_MASK |
	                                 MSR_FP_MASK | MSR_ME_MASK | MSR_FE0_MASK | MSR_DWE_MASK |
	                                 MSR_DE_MASK | MSR_FE1_MASK | MSR_IS_MASK | MSR_DS_MASK;

	// MSR reset value
	static const uint32_t MSR_RESET_VALUE = 0;
};

class RSTCFGLayout
{
public:
	static const unsigned int RSTCFG_U0_OFFSET = 15;
	static const unsigned int RSTCFG_U1_OFFSET = 14;
	static const unsigned int RSTCFG_U2_OFFSET = 13;
	static const unsigned int RSTCFG_U3_OFFSET = 12;
	static const unsigned int RSTCFG_E_OFFSET = 7;
	static const unsigned int RSTCFG_ERPN_OFFSET = 0;

	static const unsigned int RSTCFG_U0_BITSIZE = 1;
	static const unsigned int RSTCFG_U1_BITSIZE = 1;
	static const unsigned int RSTCFG_U2_BITSIZE = 1;
	static const unsigned int RSTCFG_U3_BITSIZE = 1;
	static const unsigned int RSTCFG_E_BITSIZE = 1;
	static const unsigned int RSTCFG_ERPN_BITSIZE = 4;

	static const uint32_t RSTCFG_U0_MASK = ((1UL << RSTCFG_U0_BITSIZE) - 1) << RSTCFG_U0_OFFSET;
	static const uint32_t RSTCFG_U1_MASK = ((1UL << RSTCFG_U1_BITSIZE) - 1) << RSTCFG_U1_OFFSET;
	static const uint32_t RSTCFG_U2_MASK = ((1UL << RSTCFG_U2_BITSIZE) - 1) << RSTCFG_U2_OFFSET;
	static const uint32_t RSTCFG_U3_MASK = ((1UL << RSTCFG_U3_BITSIZE) - 1) << RSTCFG_U3_OFFSET;
	static const uint32_t RSTCFG_E_MASK = ((1UL << RSTCFG_E_BITSIZE) - 1) << RSTCFG_E_OFFSET;
	static const uint32_t RSTCFG_ERPN_MASK = ((1UL << RSTCFG_ERPN_BITSIZE) - 1) << RSTCFG_ERPN_OFFSET;
	
	static const uint32_t RSTCFG_MASK = RSTCFG_U0_MASK | RSTCFG_U1_MASK | RSTCFG_U2_MASK | RSTCFG_U3_MASK | RSTCFG_E_MASK | RSTCFG_ERPN_MASK;
	
	static const uint32_t RSTCFG_RESET_VALUE = 0; // this value should reflect the core input signals (here: U0=0,U1=0,U2=0,U3=0,E=0,ERPN=0)
};

class MMUCRLayout
{
public:
	static const unsigned int MMUCR_SWOA_OFFSET = 24;
	static const unsigned int MMUCR_U1TE_OFFSET = 22;
	static const unsigned int MMUCR_U2SWOAE_OFFSET = 21;
	static const unsigned int MMUCR_DULXE_OFFSET = 19;
	static const unsigned int MMUCR_IULXE_OFFSET = 18;
	static const unsigned int MMUCR_STS_OFFSET = 16;
	static const unsigned int MMUCR_STID_OFFSET = 0;

	static const unsigned int MMUCR_SWOA_BITSIZE = 1;
	static const unsigned int MMUCR_U1TE_BITSIZE = 1;
	static const unsigned int MMUCR_U2SWOAE_BITSIZE = 1;
	static const unsigned int MMUCR_DULXE_BITSIZE = 1;
	static const unsigned int MMUCR_IULXE_BITSIZE = 1;
	static const unsigned int MMUCR_STS_BITSIZE = 1;
	static const unsigned int MMUCR_STID_BITSIZE = 8;

	static const uint32_t MMUCR_SWOA_MASK = ((1UL << MMUCR_SWOA_BITSIZE) - 1) << MMUCR_SWOA_OFFSET;
	static const uint32_t MMUCR_U1TE_MASK = ((1UL << MMUCR_U1TE_BITSIZE) - 1) << MMUCR_U1TE_OFFSET;
	static const uint32_t MMUCR_U2SWOAE_MASK = ((1UL << MMUCR_U2SWOAE_BITSIZE) - 1) << MMUCR_U2SWOAE_OFFSET;
	static const uint32_t MMUCR_DULXE_MASK = ((1UL << MMUCR_DULXE_BITSIZE) - 1) << MMUCR_DULXE_OFFSET;
	static const uint32_t MMUCR_IULXE_MASK = ((1UL << MMUCR_IULXE_BITSIZE) - 1) << MMUCR_IULXE_OFFSET;
	static const uint32_t MMUCR_STS_MASK = ((1UL << MMUCR_STS_BITSIZE) - 1) << MMUCR_STS_OFFSET;
	static const uint32_t MMUCR_STID_MASK = ((1UL << MMUCR_STID_BITSIZE) - 1) << MMUCR_STID_OFFSET;
	
	static const uint32_t MMUCR_MASK = MMUCR_SWOA_MASK | MMUCR_U1TE_MASK | MMUCR_U2SWOAE_MASK | MMUCR_DULXE_MASK | MMUCR_IULXE_MASK | MMUCR_STS_MASK | MMUCR_STID_MASK;
	
	static const uint32_t MMUCR_RESET_VALUE = 0; // undefined
};

class CCR0Layout
{
public:
	static const unsigned int CCR0_PRE_OFFSET = 30;
	static const unsigned int CCR0_CRPE_OFFSET = 27;
	static const unsigned int CCR0_DSTG_OFFSET = 21;
	static const unsigned int CCR0_DAPUIB_OFFSET = 20;
	static const unsigned int CCR0_DTB_OFFSET = 15;
	static const unsigned int CCR0_GICBT_OFFSET = 14;
	static const unsigned int CCR0_GDCBT_OFFSET = 13;
	static const unsigned int CCR0_FLSTA_OFFSET = 8;
	static const unsigned int CCR0_ICSLC_OFFSET = 2;
	static const unsigned int CCR0_ICSLT_OFFSET = 0;

	static const unsigned int CCR0_PRE_BITSIZE = 1;
	static const unsigned int CCR0_CRPE_BITSIZE = 1;
	static const unsigned int CCR0_DSTG_BITSIZE = 1;
	static const unsigned int CCR0_DAPUIB_BITSIZE = 1;
	static const unsigned int CCR0_DTB_BITSIZE = 1;
	static const unsigned int CCR0_GICBT_BITSIZE = 1;
	static const unsigned int CCR0_GDCBT_BITSIZE = 1;
	static const unsigned int CCR0_FLSTA_BITSIZE = 1;
	static const unsigned int CCR0_ICSLC_BITSIZE = 2;
	static const unsigned int CCR0_ICSLT_BITSIZE = 2;

	static const uint32_t CCR0_PRE_MASK = ((1UL << CCR0_PRE_BITSIZE) - 1) << CCR0_PRE_OFFSET;
	static const uint32_t CCR0_CRPE_MASK = ((1UL << CCR0_CRPE_BITSIZE) - 1) << CCR0_CRPE_OFFSET;
	static const uint32_t CCR0_DSTG_MASK = ((1UL << CCR0_DSTG_BITSIZE) - 1) << CCR0_DSTG_OFFSET;
	static const uint32_t CCR0_DAPUIB_MASK = ((1UL << CCR0_DAPUIB_BITSIZE) - 1) << CCR0_DAPUIB_OFFSET;
	static const uint32_t CCR0_DTB_MASK = ((1UL << CCR0_DTB_BITSIZE) - 1) << CCR0_DTB_OFFSET;
	static const uint32_t CCR0_GICBT_MASK = ((1UL << CCR0_GICBT_BITSIZE) - 1) << CCR0_GICBT_OFFSET;
	static const uint32_t CCR0_GDCBT_MASK = ((1UL << CCR0_GDCBT_BITSIZE) - 1) << CCR0_GDCBT_OFFSET;
	static const uint32_t CCR0_FLSTA_MASK = ((1UL << CCR0_FLSTA_BITSIZE) - 1) << CCR0_FLSTA_OFFSET;
	static const uint32_t CCR0_ICSLC_MASK = ((1UL << CCR0_ICSLC_BITSIZE) - 1) << CCR0_ICSLC_OFFSET;
	static const uint32_t CCR0_ICSLT_MASK = ((1UL << CCR0_ICSLT_BITSIZE) - 1) << CCR0_ICSLT_OFFSET;
	
	static const uint32_t CCR0_MASK = CCR0_PRE_MASK | CCR0_CRPE_MASK | CCR0_DSTG_MASK | CCR0_DAPUIB_MASK |
	                                  CCR0_DTB_MASK | CCR0_GICBT_MASK | CCR0_GDCBT_MASK | CCR0_FLSTA_MASK |
	                                  CCR0_ICSLC_MASK | CCR0_ICSLT_MASK;

	static const uint32_t CCR0_RESET_VALUE = 0;
};

class CCR1Layout
{
public:
	static const unsigned int CCR1_ICDPEI_OFFSET = 24;
	static const unsigned int CCR1_ICTPEI_OFFSET = 22;
	static const unsigned int CCR1_DCTPEI_OFFSET = 20;
	static const unsigned int CCR1_DCDPEI_OFFSET = 19;
	static const unsigned int CCR1_DCUPEI_OFFSET = 18;
	static const unsigned int CCR1_FCOM_OFFSET = 16;
	static const unsigned int CCR1_MMUPEI_OFFSET = 12;
	static const unsigned int CCR1_FFF_OFFSET = 11;
	static const unsigned int CCR1_TCS_OFFSET = 7;

	static const unsigned int CCR1_ICDPEI_BITSIZE = 8;
	static const unsigned int CCR1_ICTPEI_BITSIZE = 2;
	static const unsigned int CCR1_DCTPEI_BITSIZE = 2;
	static const unsigned int CCR1_DCDPEI_BITSIZE = 1;
	static const unsigned int CCR1_DCUPEI_BITSIZE = 1;
	static const unsigned int CCR1_FCOM_BITSIZE = 1;
	static const unsigned int CCR1_MMUPEI_BITSIZE = 4;
	static const unsigned int CCR1_FFF_BITSIZE = 1;
	static const unsigned int CCR1_TCS_BITSIZE = 1;

	static const uint32_t CCR1_ICDPEI_MASK = ((1UL << CCR1_ICDPEI_BITSIZE) - 1) << CCR1_ICDPEI_OFFSET;
	static const uint32_t CCR1_ICTPEI_MASK = ((1UL << CCR1_ICTPEI_BITSIZE) - 1) << CCR1_ICTPEI_OFFSET;
	static const uint32_t CCR1_DCTPEI_MASK = ((1UL << CCR1_DCTPEI_BITSIZE) - 1) << CCR1_DCTPEI_OFFSET;
	static const uint32_t CCR1_DCDPEI_MASK = ((1UL << CCR1_DCDPEI_BITSIZE) - 1) << CCR1_DCDPEI_OFFSET;
	static const uint32_t CCR1_DCUPEI_MASK = ((1UL << CCR1_DCUPEI_BITSIZE) - 1) << CCR1_DCUPEI_OFFSET;
	static const uint32_t CCR1_FCOM_MASK = ((1UL << CCR1_FCOM_BITSIZE) - 1) << CCR1_FCOM_OFFSET;
	static const uint32_t CCR1_MMUPEI_MASK = ((1UL << CCR1_MMUPEI_BITSIZE) - 1) << CCR1_MMUPEI_OFFSET;
	static const uint32_t CCR1_FFF_MASK = ((1UL << CCR1_FFF_BITSIZE) - 1) << CCR1_FFF_OFFSET;
	static const uint32_t CCR1_TCS_MASK = ((1UL << CCR1_TCS_BITSIZE) - 1) << CCR1_TCS_OFFSET;
	
	static const uint32_t CCR1_MASK = CCR1_ICDPEI_MASK | CCR1_ICTPEI_MASK | CCR1_DCTPEI_MASK | CCR1_DCDPEI_MASK |
	                                  CCR1_DCUPEI_MASK | CCR1_FCOM_MASK | CCR1_MMUPEI_MASK | CCR1_FFF_MASK |
	                                  CCR1_TCS_MASK;

	static const uint32_t CCR1_RESET_VALUE = 0;
};

class IVLIMLayout
{
public:
	static const unsigned int IVLIM_TFLOOR_OFFSET = 22;
	static const unsigned int IVLIM_TCEILING_OFFSET = 11;
	static const unsigned int IVLIM_NFLOOR_OFFSET = 0;

	static const unsigned int IVLIM_TFLOOR_BITSIZE = 8;
	static const unsigned int IVLIM_TCEILING_BITSIZE = 8;
	static const unsigned int IVLIM_NFLOOR_BITSIZE = 8;

	static const uint32_t IVLIM_TFLOOR_MASK = ((1UL << IVLIM_TFLOOR_BITSIZE) - 1) << IVLIM_TFLOOR_OFFSET;
	static const uint32_t IVLIM_TCEILING_MASK = ((1UL << IVLIM_TCEILING_BITSIZE) - 1) << IVLIM_TCEILING_OFFSET;
	static const uint32_t IVLIM_NFLOOR_MASK = ((1UL << IVLIM_NFLOOR_BITSIZE) - 1) << IVLIM_NFLOOR_OFFSET;
	
	static const uint32_t IVLIM_MASK = IVLIM_TFLOOR_MASK | IVLIM_TCEILING_MASK | IVLIM_NFLOOR_MASK;
	
	static const uint32_t IVLIM_RESET_VALUE = 0; // undefined
};

class DVLIMLayout
{
public:
	static const unsigned int DVLIM_TFLOOR_OFFSET = 22;
	static const unsigned int DVLIM_TCEILING_OFFSET = 11;
	static const unsigned int DVLIM_NFLOOR_OFFSET = 0;

	static const unsigned int DVLIM_TFLOOR_BITSIZE = 8;
	static const unsigned int DVLIM_TCEILING_BITSIZE = 8;
	static const unsigned int DVLIM_NFLOOR_BITSIZE = 8;

	static const uint32_t DVLIM_TFLOOR_MASK = ((1UL << DVLIM_TFLOOR_BITSIZE) - 1) << DVLIM_TFLOOR_OFFSET;
	static const uint32_t DVLIM_TCEILING_MASK = ((1UL << DVLIM_TCEILING_BITSIZE) - 1) << DVLIM_TCEILING_OFFSET;
	static const uint32_t DVLIM_NFLOOR_MASK = ((1UL << DVLIM_NFLOOR_BITSIZE) - 1) << DVLIM_NFLOOR_OFFSET;
	
	static const uint32_t DVLIM_MASK = DVLIM_TFLOOR_MASK | DVLIM_TCEILING_MASK | DVLIM_NFLOOR_MASK;
	
	static const uint32_t DVLIM_RESET_VALUE = 0; // undefined
};

class DCDBTRHLayout
{
public:
	static const unsigned int DCDBTRH_TRA_OFFSET = 8;
	static const unsigned int DCDBTRH_V_OFFSET = 7;
	static const unsigned int DCDBTRH_TERA_OFFSET = 0;

	static const unsigned int DCDBTRH_TRA_BITSIZE = 24;
	static const unsigned int DCDBTRH_V_BITSIZE = 1;
	static const unsigned int DCDBTRH_TERA_BITSIZE = 4;

	static const uint32_t DCDBTRH_TRA_MASK = ((1UL << DCDBTRH_TRA_BITSIZE) - 1) << DCDBTRH_TRA_OFFSET;
	static const uint32_t DCDBTRH_V_MASK = ((1UL << DCDBTRH_V_BITSIZE) - 1) << DCDBTRH_V_OFFSET;
	static const uint32_t DCDBTRH_TERA_MASK = ((1UL << DCDBTRH_TERA_BITSIZE) - 1) << DCDBTRH_TERA_OFFSET;
	
	static const uint32_t DCDBTRH_MASK = DCDBTRH_TRA_MASK | DCDBTRH_V_MASK | DCDBTRH_TERA_MASK;
	
	static const unsigned int DCDBTRH_BITSIZE = DCDBTRH_TRA_BITSIZE + DCDBTRH_V_BITSIZE + DCDBTRH_TERA_BITSIZE;
};

class DCDBTRLLayout
{
public:
	static const unsigned int DCDBTRL_D_OFFSET = 4;
	static const unsigned int DCDBTRL_U0_OFFSET = 3;
	static const unsigned int DCDBTRL_U1_OFFSET = 2;
	static const unsigned int DCDBTRL_U2_OFFSET = 1;
	static const unsigned int DCDBTRL_U3_OFFSET = 0;

	static const unsigned int DCDBTRL_D_BITSIZE = 4;
	static const unsigned int DCDBTRL_U0_BITSIZE = 1;
	static const unsigned int DCDBTRL_U1_BITSIZE = 1;
	static const unsigned int DCDBTRL_U2_BITSIZE = 1;
	static const unsigned int DCDBTRL_U3_BITSIZE = 1;

	static const uint32_t DCDBTRL_D_MASK = ((1UL << DCDBTRL_D_BITSIZE) - 1) << DCDBTRL_D_OFFSET;
	static const uint32_t DCDBTRL_U0_MASK = ((1UL << DCDBTRL_U0_BITSIZE) - 1) << DCDBTRL_U0_OFFSET;
	static const uint32_t DCDBTRL_U1_MASK = ((1UL << DCDBTRL_U1_BITSIZE) - 1) << DCDBTRL_U1_OFFSET;
	static const uint32_t DCDBTRL_U2_MASK = ((1UL << DCDBTRL_U2_BITSIZE) - 1) << DCDBTRL_U2_OFFSET;
	static const uint32_t DCDBTRL_U3_MASK = ((1UL << DCDBTRL_U3_BITSIZE) - 1) << DCDBTRL_U3_OFFSET;
	
	static const uint32_t DCDBTRL_MASK = DCDBTRL_D_MASK | DCDBTRL_U0_MASK | DCDBTRL_U1_MASK | DCDBTRL_U2_MASK | DCDBTRL_U3_MASK;
	
	static const unsigned int DCDBTRL_BITSIZE = DCDBTRL_D_BITSIZE + DCDBTRL_U0_BITSIZE + DCDBTRL_U1_BITSIZE + DCDBTRL_U2_BITSIZE + DCDBTRL_U3_BITSIZE;
};

class ICDBTRHLayout
{
public:
	static const unsigned int ICDBTRH_TEA_OFFSET = 8;
	static const unsigned int ICDBTRH_V_OFFSET = 7;

	static const unsigned int ICDBTRH_TEA_BITSIZE = 24;
	static const unsigned int ICDBTRH_V_BITSIZE = 1;

	static const uint32_t ICDBTRH_TEA_MASK = ((1UL << ICDBTRH_TEA_BITSIZE) - 1) << ICDBTRH_TEA_OFFSET;
	static const uint32_t ICDBTRH_V_MASK = ((1UL << ICDBTRH_V_BITSIZE) - 1) << ICDBTRH_V_OFFSET;
	
	static const uint32_t ICDBTRH_MASK = ICDBTRH_TEA_MASK | ICDBTRH_V_MASK;
	
	static const unsigned int ICDBTRH_BITSIZE = ICDBTRH_TEA_BITSIZE + ICDBTRH_V_BITSIZE;
};

class ICDBTRLLayout
{
public:
	static const unsigned int ICDBTRL_TS_OFFSET = 9;
	static const unsigned int ICDBTRL_TD_OFFSET = 8;
	static const unsigned int ICDBTRL_TID_OFFSET = 0;

	static const unsigned int ICDBTRL_TS_BITSIZE = 1;
	static const unsigned int ICDBTRL_TD_BITSIZE = 1;
	static const unsigned int ICDBTRL_TID_BITSIZE = 8;

	static const uint32_t ICDBTRL_TS_MASK = ((1UL << ICDBTRL_TS_BITSIZE) - 1) << ICDBTRL_TS_OFFSET;
	static const uint32_t ICDBTRL_TD_MASK = ((1UL << ICDBTRL_TD_BITSIZE) - 1) << ICDBTRL_TD_OFFSET;
	static const uint32_t ICDBTRL_TID_MASK = ((1UL << ICDBTRL_TID_BITSIZE) - 1) << ICDBTRL_TID_OFFSET;
	
	static const uint32_t ICDBTRL_MASK = ICDBTRL_TS_MASK | ICDBTRL_TD_MASK | ICDBTRL_TID_MASK;
	
	static const unsigned int ICDBTRL_BITSIZE = ICDBTRL_TS_BITSIZE + ICDBTRL_TD_BITSIZE + ICDBTRL_TID_BITSIZE;
};

class ESRLayout
{
public:
	static const unsigned int ESR_MCI_OFFSET = 31;
	static const unsigned int ESR_PIL_OFFSET = 27;
	static const unsigned int ESR_PPR_OFFSET = 26;
	static const unsigned int ESR_PTR_OFFSET = 25;
	static const unsigned int ESR_FP_OFFSET = 24;
	static const unsigned int ESR_ST_OFFSET = 23;
	static const unsigned int ESR_DLK_OFFSET = 20;
	static const unsigned int ESR_AP_OFFSET = 19;
	static const unsigned int ESR_PUO_OFFSET = 18;
	static const unsigned int ESR_BO_OFFSET = 17;
	static const unsigned int ESR_PIE_OFFSET = 16;
	static const unsigned int ESR_PCRE_OFFSET = 4;
	static const unsigned int ESR_PCMP_OFFSET = 3;
	static const unsigned int ESR_PCRF_OFFSET = 0;
	
	static const unsigned int ESR_MCI_BITSIZE = 1;
	static const unsigned int ESR_PIL_BITSIZE = 1;
	static const unsigned int ESR_PPR_BITSIZE = 1;
	static const unsigned int ESR_PTR_BITSIZE = 1;
	static const unsigned int ESR_FP_BITSIZE = 1;
	static const unsigned int ESR_ST_BITSIZE = 1;
	static const unsigned int ESR_DLK_BITSIZE = 2;
	static const unsigned int ESR_AP_BITSIZE = 1;
	static const unsigned int ESR_PUO_BITSIZE = 1;
	static const unsigned int ESR_BO_BITSIZE = 1;
	static const unsigned int ESR_PIE_BITSIZE = 1;
	static const unsigned int ESR_PCRE_BITSIZE = 1;
	static const unsigned int ESR_PCMP_BITSIZE = 1;
	static const unsigned int ESR_PCRF_BITSIZE = 3;
	
	static const uint32_t ESR_MCI_MASK = ((1UL << ESR_MCI_BITSIZE) - 1) << ESR_MCI_OFFSET;
	static const uint32_t ESR_PIL_MASK = ((1UL << ESR_PIL_BITSIZE) - 1) << ESR_PIL_OFFSET;
	static const uint32_t ESR_PPR_MASK = ((1UL << ESR_PPR_BITSIZE) - 1) << ESR_PPR_OFFSET;
	static const uint32_t ESR_PTR_MASK = ((1UL << ESR_PTR_BITSIZE) - 1) << ESR_PTR_OFFSET;
	static const uint32_t ESR_FP_MASK = ((1UL << ESR_FP_BITSIZE) - 1) << ESR_FP_OFFSET;
	static const uint32_t ESR_ST_MASK = ((1UL << ESR_ST_BITSIZE) - 1) << ESR_ST_OFFSET;
	static const uint32_t ESR_DLK_MASK = ((1UL << ESR_DLK_BITSIZE) - 1) << ESR_DLK_OFFSET;
	static const uint32_t ESR_AP_MASK = ((1UL << ESR_AP_BITSIZE) - 1) << ESR_AP_OFFSET;
	static const uint32_t ESR_PUO_MASK = ((1UL << ESR_PUO_BITSIZE) - 1) << ESR_PUO_OFFSET;
	static const uint32_t ESR_BO_MASK = ((1UL << ESR_BO_BITSIZE) - 1) << ESR_BO_OFFSET;
	static const uint32_t ESR_PIE_MASK = ((1UL << ESR_PIE_BITSIZE) - 1) << ESR_PIE_OFFSET;
	static const uint32_t ESR_PCRE_MASK = ((1UL << ESR_PCRE_BITSIZE) - 1) << ESR_PCRE_OFFSET;
	static const uint32_t ESR_PCMP_MASK = ((1UL << ESR_PCMP_BITSIZE) - 1) << ESR_PCMP_OFFSET;
	static const uint32_t ESR_PCRF_MASK = ((1UL << ESR_PCRF_BITSIZE) - 1) << ESR_PCRF_OFFSET;
	
	static const uint32_t ESR_MASK = ESR_MCI_MASK | ESR_PIL_MASK | ESR_PPR_MASK | ESR_PTR_MASK |
	                                 ESR_FP_MASK | ESR_ST_MASK | ESR_DLK_MASK | ESR_AP_MASK |
	                                 ESR_PUO_MASK | ESR_BO_MASK | ESR_PIE_MASK | ESR_PCRE_MASK |
	                                 ESR_PCMP_MASK | ESR_PCRF_MASK;
	
	static const uint32_t ESR_RESET_VALUE = 0; // undefined
};

class TLBELayout
{
public:
	// Note: word 0 is 32-bit instead of 40-bit because 8-bit TID is in MMUCR from a programmer's point of view
	static const unsigned int TLBE0_EPN_OFFSET = 10;
	static const unsigned int TLBE0_V_OFFSET = 9;
	static const unsigned int TLBE0_TS_OFFSET = 8;
	static const unsigned int TLBE0_SIZE_OFFSET = 4;
	static const unsigned int TLBE0_TPAR_OFFSET = 0;
	
	static const unsigned int TLBE1_RPN_OFFSET = 10;
	static const unsigned int TLBE1_PAR1_OFFSET = 8;
	static const unsigned int TLBE1_ERPN_OFFSET = 0;
	
	static const unsigned int TLBE2_PAR2_OFFSET = 30;
	static const unsigned int TLBE2_U0_OFFSET = 15;
	static const unsigned int TLBE2_U1_OFFSET = 14;
	static const unsigned int TLBE2_U2_OFFSET = 13;
	static const unsigned int TLBE2_U3_OFFSET = 12;
	static const unsigned int TLBE2_W_OFFSET = 11;
	static const unsigned int TLBE2_I_OFFSET = 10;
	static const unsigned int TLBE2_M_OFFSET = 9;
	static const unsigned int TLBE2_G_OFFSET = 8;
	static const unsigned int TLBE2_E_OFFSET = 7;
	static const unsigned int TLBE2_STORAGE_ATTR_OFFSET = TLBE2_E_OFFSET;
	static const unsigned int TLBE2_UX_OFFSET = 5;
	static const unsigned int TLBE2_UW_OFFSET = 4;
	static const unsigned int TLBE2_UR_OFFSET = 3;
	static const unsigned int TLBE2_SX_OFFSET = 2;
	static const unsigned int TLBE2_SW_OFFSET = 1;
	static const unsigned int TLBE2_SR_OFFSET = 0;
	static const unsigned int TLBE2_ACCESS_CTRL_OFFSET = TLBE2_SR_OFFSET;

	static const unsigned int TLBE0_EPN_BITSIZE = 22;
	static const unsigned int TLBE0_V_BITSIZE = 1;
	static const unsigned int TLBE0_TS_BITSIZE = 1;
	static const unsigned int TLBE0_SIZE_BITSIZE = 4;
	static const unsigned int TLBE0_TPAR_BITSIZE = 4;
	static const unsigned int TLBE0_TAG_BITSIZE = TLBE0_EPN_BITSIZE + TLBE0_V_BITSIZE + TLBE0_TS_BITSIZE + TLBE0_SIZE_BITSIZE + TLBE0_TPAR_BITSIZE;

	static const unsigned int TLBE1_RPN_BITSIZE = 22;
	static const unsigned int TLBE1_PAR1_BITSIZE = 2;
	static const unsigned int TLBE1_ERPN_BITSIZE = 4;
	static const unsigned int TLBE1_DATA_BITSIZE = TLBE1_RPN_BITSIZE + TLBE1_PAR1_BITSIZE + TLBE1_ERPN_BITSIZE;
	
	static const unsigned int TLBE2_PAR2_BITSIZE = 2;
	static const unsigned int TLBE2_U0_BITSIZE = 1;
	static const unsigned int TLBE2_U1_BITSIZE = 1;
	static const unsigned int TLBE2_U2_BITSIZE = 1;
	static const unsigned int TLBE2_U3_BITSIZE = 1;
	static const unsigned int TLBE2_W_BITSIZE = 1;
	static const unsigned int TLBE2_I_BITSIZE = 1;
	static const unsigned int TLBE2_M_BITSIZE = 1;
	static const unsigned int TLBE2_G_BITSIZE = 1;
	static const unsigned int TLBE2_E_BITSIZE = 1;
	static const unsigned int TLBE2_UX_BITSIZE = 1;
	static const unsigned int TLBE2_UW_BITSIZE = 1;
	static const unsigned int TLBE2_UR_BITSIZE = 1;
	static const unsigned int TLBE2_SX_BITSIZE = 1;
	static const unsigned int TLBE2_SW_BITSIZE = 1;
	static const unsigned int TLBE2_SR_BITSIZE = 1;
	static const unsigned int TLBE2_DATA_BITSIZE = TLBE2_PAR2_BITSIZE + TLBE2_U0_BITSIZE + TLBE2_U1_BITSIZE + TLBE2_U2_BITSIZE + TLBE2_U3_BITSIZE +
	                                               TLBE2_W_BITSIZE + TLBE2_I_BITSIZE + TLBE2_M_BITSIZE + TLBE2_G_BITSIZE + TLBE2_E_BITSIZE +
	                                               TLBE2_UX_BITSIZE + TLBE2_UW_BITSIZE + TLBE2_UR_BITSIZE + TLBE2_SX_BITSIZE + TLBE2_SW_BITSIZE + TLBE2_SR_BITSIZE;

	static const uint32_t TLBE0_EPN_MASK = ((1UL << TLBE0_EPN_BITSIZE) - 1) << TLBE0_EPN_OFFSET;
	static const uint32_t TLBE0_V_MASK = ((1UL << TLBE0_V_BITSIZE) - 1) << TLBE0_V_OFFSET;
	static const uint32_t TLBE0_TS_MASK = ((1UL << TLBE0_TS_BITSIZE) - 1) << TLBE0_TS_OFFSET;
	static const uint32_t TLBE0_SIZE_MASK = ((1UL << TLBE0_SIZE_BITSIZE) - 1) << TLBE0_SIZE_OFFSET;
	static const uint32_t TLBE0_TPAR_MASK = ((1UL << TLBE0_TPAR_BITSIZE) - 1) << TLBE0_TPAR_OFFSET;
	
	static const uint32_t TLBE1_RPN_MASK = ((1UL << TLBE1_RPN_BITSIZE) - 1) << TLBE1_RPN_OFFSET;
	static const uint32_t TLBE1_PAR1_MASK = ((1UL << TLBE1_PAR1_BITSIZE) - 1) << TLBE1_PAR1_OFFSET;
	static const uint32_t TLBE1_ERPN_MASK = ((1UL << TLBE1_ERPN_BITSIZE) - 1) << TLBE1_ERPN_OFFSET;
	
	static const uint32_t TLBE2_PAR2_MASK = ((1UL << TLBE2_PAR2_BITSIZE) - 1) << TLBE2_PAR2_OFFSET;
	static const uint32_t TLBE2_U0_MASK = ((1UL << TLBE2_U0_BITSIZE) - 1) << TLBE2_U0_OFFSET;
	static const uint32_t TLBE2_U1_MASK = ((1UL << TLBE2_U1_BITSIZE) - 1) << TLBE2_U1_OFFSET;
	static const uint32_t TLBE2_U2_MASK = ((1UL << TLBE2_U2_BITSIZE) - 1) << TLBE2_U2_OFFSET;
	static const uint32_t TLBE2_U3_MASK = ((1UL << TLBE2_U3_BITSIZE) - 1) << TLBE2_U3_OFFSET;
	static const uint32_t TLBE2_W_MASK = ((1UL << TLBE2_W_BITSIZE) - 1) << TLBE2_W_OFFSET;
	static const uint32_t TLBE2_I_MASK = ((1UL << TLBE2_I_BITSIZE) - 1) << TLBE2_I_OFFSET;
	static const uint32_t TLBE2_M_MASK = ((1UL << TLBE2_M_BITSIZE) - 1) << TLBE2_M_OFFSET;
	static const uint32_t TLBE2_G_MASK = ((1UL << TLBE2_G_BITSIZE) - 1) << TLBE2_G_OFFSET;
	static const uint32_t TLBE2_E_MASK = ((1UL << TLBE2_E_BITSIZE) - 1) << TLBE2_E_OFFSET;
	static const uint32_t TLBE2_STORAGE_ATTR_MASK = TLBE2_U0_MASK | TLBE2_U1_MASK | TLBE2_U2_MASK | TLBE2_U3_MASK
	                                              | TLBE2_W_MASK | TLBE2_I_MASK | TLBE2_M_MASK | TLBE2_G_MASK | TLBE2_E_MASK;
	static const uint32_t TLBE2_UX_MASK = ((1UL << TLBE2_UX_BITSIZE) - 1) << TLBE2_UX_OFFSET;
	static const uint32_t TLBE2_UW_MASK = ((1UL << TLBE2_UW_BITSIZE) - 1) << TLBE2_UW_OFFSET;
	static const uint32_t TLBE2_UR_MASK = ((1UL << TLBE2_UR_BITSIZE) - 1) << TLBE2_UR_OFFSET;
	static const uint32_t TLBE2_SX_MASK = ((1UL << TLBE2_SX_BITSIZE) - 1) << TLBE2_SX_OFFSET;
	static const uint32_t TLBE2_SW_MASK = ((1UL << TLBE2_SW_BITSIZE) - 1) << TLBE2_SW_OFFSET;
	static const uint32_t TLBE2_SR_MASK = ((1UL << TLBE2_SR_BITSIZE) - 1) << TLBE2_SR_OFFSET;
	static const uint32_t TLBE2_ACCESS_CTRL_MASK = TLBE2_UX_MASK | TLBE2_UW_MASK | TLBE2_UR_MASK
	                                             | TLBE2_SX_MASK | TLBE2_SW_MASK | TLBE2_SR_MASK;
	
	static const uint32_t TLBE0_TAG_MASK = TLBE0_EPN_MASK | TLBE0_V_MASK | TLBE0_TS_MASK | TLBE0_SIZE_MASK;

	static const uint32_t TLBE0_MASK = TLBE0_TAG_MASK | TLBE0_TPAR_MASK;

	static const uint32_t TLBE1_DATA_MASK = TLBE1_RPN_MASK | TLBE1_ERPN_MASK;

	static const uint32_t TLBE1_MASK = TLBE1_DATA_MASK | TLBE1_PAR1_MASK;

	static const uint32_t TLBE2_DATA_MASK = TLBE2_U0_MASK | TLBE2_U1_MASK | TLBE2_U2_MASK | TLBE2_U3_MASK |
	                                        TLBE2_W_MASK | TLBE2_I_MASK | TLBE2_M_MASK | TLBE2_G_MASK | TLBE2_E_MASK |
	                                        TLBE2_UX_MASK | TLBE2_UW_MASK | TLBE2_UR_MASK |
	                                        TLBE2_SX_MASK | TLBE2_SW_MASK | TLBE2_SR_MASK;

	static const uint32_t TLBE2_MASK = TLBE2_DATA_MASK | TLBE2_PAR2_MASK;
	
	static const uint32_t TLBE_TAG_BITSIZE = TLBE0_TAG_BITSIZE;
	static const uint32_t TLBE_DATA_BITSIZE = TLBE1_DATA_BITSIZE + TLBE2_DATA_BITSIZE + 8; // account also for the 8-bit PID
};

class VirtualAddressLayout
{
public:
	static const unsigned int VADDR_EA_OFFSET = 0;
	static const unsigned int VADDR_PID_OFFSET = 32;
	static const unsigned int VADDR_AS_OFFSET = 40;
	
	static const unsigned int VADDR_EA_BITSIZE = 32;
	static const unsigned int VADDR_PID_BITSIZE = 8;
	static const unsigned int VADDR_AS_BITSIZE = 1;
	
	static const uint64_t VADDR_EA_MASK = ((1ULL << VADDR_EA_BITSIZE) - 1) << VADDR_EA_OFFSET;
	static const uint64_t VADDR_PID_MASK = ((1ULL << VADDR_PID_BITSIZE) - 1) << VADDR_PID_OFFSET;
	static const uint64_t VADDR_AS_MASK = ((1ULL << VADDR_AS_BITSIZE) - 1) << VADDR_AS_OFFSET;
};

class PhysicalAddressLayout
{
public:
	static const unsigned int PADDR_ERPN_OFFSET = 32;
	static const unsigned int PADDR_RPN_OFFSET = 10;
	static const unsigned int PADDR_OFS_OFFSET = 0;
	
	static const unsigned int PADDR_ERPN_BITSIZE = 4;
	static const unsigned int PADDR_RPN_BITSIZE = 22;
	static const unsigned int PADDR_OFS_BITSIZE = 10;
	
	static const uint64_t PADDR_ERPN_MASK = ((1ULL << PADDR_ERPN_BITSIZE) - 1) << PADDR_ERPN_OFFSET;
	static const uint64_t PADDR_RPN_MASK = ((1ULL << PADDR_RPN_BITSIZE) - 1) << PADDR_RPN_OFFSET;
	static const uint64_t PADDR_OFS_MASK = ((1ULL << PADDR_OFS_BITSIZE) - 1) << PADDR_OFS_OFFSET;
};

class PageSizeCap
{
public:
	static const uint32_t PS_1K = 1;
	static const uint32_t PS_4K = 2;
	static const uint32_t PS_16K = 4;
	static const uint32_t PS_64K = 8;
	static const uint32_t PS_256K = 16;
	static const uint32_t PS_1MB = 32;
	static const uint32_t PS_4MB = 64;
	static const uint32_t PS_16MB = 128;
	static const uint32_t PS_64M = 256;
	static const uint32_t PS_256MB = 512;
	static const uint32_t PS_1GB = 1024;
	static const uint32_t PS_4GB = 2048;
	static const uint32_t PS_16GB = 4096;
	static const uint32_t PS_64GB = 8192;
	static const uint32_t PS_256GB = 16384;
	static const uint32_t PS_1TB = 32768;
};

class TSRLayout
{
public:
	static const unsigned int TSR_ENW_OFFSET = 31;
	static const unsigned int TSR_WIS_OFFSET = 30;
	static const unsigned int TSR_WRS_OFFSET = 28;
	static const unsigned int TSR_DIS_OFFSET = 27;
	static const unsigned int TSR_FIS_OFFSET = 26;

	static const unsigned int TSR_ENW_BITSIZE = 1;
	static const unsigned int TSR_WIS_BITSIZE = 1;
	static const unsigned int TSR_WRS_BITSIZE = 2;
	static const unsigned int TSR_DIS_BITSIZE = 1;
	static const unsigned int TSR_FIS_BITSIZE = 1;

	static const uint32_t TSR_ENW_MASK = ((1UL << TSR_ENW_BITSIZE) - 1) << TSR_ENW_OFFSET;
	static const uint32_t TSR_WIS_MASK = ((1UL << TSR_WIS_BITSIZE) - 1) << TSR_WIS_OFFSET;
	static const uint32_t TSR_WRS_MASK = ((1UL << TSR_WRS_BITSIZE) - 1) << TSR_WRS_OFFSET;
	static const uint32_t TSR_DIS_MASK = ((1UL << TSR_DIS_BITSIZE) - 1) << TSR_DIS_OFFSET;
	static const uint32_t TSR_FIS_MASK = ((1UL << TSR_FIS_BITSIZE) - 1) << TSR_FIS_OFFSET;

	static const uint32_t TSR_MASK = TSR_ENW_MASK | TSR_WIS_MASK | TSR_WRS_MASK | TSR_DIS_MASK | TSR_FIS_MASK;
};

class TCRLayout
{
public:
	static const unsigned int TCR_WP_OFFSET = 30;
	static const unsigned int TCR_WRC_OFFSET = 28;
	static const unsigned int TCR_WIE_OFFSET = 27;
	static const unsigned int TCR_DIE_OFFSET = 26;
	static const unsigned int TCR_FP_OFFSET = 24;
	static const unsigned int TCR_FIE_OFFSET = 23;
	static const unsigned int TCR_ARE_OFFSET = 22;

	static const unsigned int TCR_WP_BITSIZE = 2;
	static const unsigned int TCR_WRC_BITSIZE = 2;
	static const unsigned int TCR_WIE_BITSIZE = 1;
	static const unsigned int TCR_DIE_BITSIZE = 1;
	static const unsigned int TCR_FP_BITSIZE = 2;
	static const unsigned int TCR_FIE_BITSIZE = 1;
	static const unsigned int TCR_ARE_BITSIZE = 1;

	static const uint32_t TCR_WP_MASK = ((1UL << TCR_WP_BITSIZE) - 1) << TCR_WP_OFFSET;
	static const uint32_t TCR_WRC_MASK = ((1UL << TCR_WRC_BITSIZE) - 1) << TCR_WRC_OFFSET;
	static const uint32_t TCR_WIE_MASK = ((1UL << TCR_WIE_BITSIZE) - 1) << TCR_WIE_OFFSET;
	static const uint32_t TCR_DIE_MASK = ((1UL << TCR_DIE_BITSIZE) - 1) << TCR_DIE_OFFSET;
	static const uint32_t TCR_FP_MASK = ((1UL << TCR_FP_BITSIZE) - 1) << TCR_FP_OFFSET;
	static const uint32_t TCR_FIE_MASK = ((1UL << TCR_FIE_BITSIZE) - 1) << TCR_FIE_OFFSET;
	static const uint32_t TCR_ARE_MASK = ((1UL << TCR_ARE_BITSIZE) - 1) << TCR_ARE_OFFSET;

	static const uint32_t TCR_MASK = TCR_WP_MASK | TCR_WRC_MASK | TCR_WIE_MASK | TCR_DIE_MASK | TCR_FP_MASK | TCR_FIE_MASK | TCR_ARE_MASK;
};

class Config :
	public unisim::component::cxx::processor::powerpc::Config,
	public MSRLayout,
	public RSTCFGLayout,
	public MMUCRLayout,
	public CCR0Layout,
	public CCR1Layout,
	public IVLIMLayout,
	public DVLIMLayout,
	public DCDBTRHLayout,
	public DCDBTRLLayout,
	public ICDBTRHLayout,
	public ICDBTRLLayout,
	public ESRLayout,
	public TLBELayout,
	public VirtualAddressLayout,
	public PhysicalAddressLayout,
	public PageSizeCap,
	public TSRLayout,
	public TCRLayout
{
public:
	typedef CPU<Config> STATE;

	static const Model MODEL = PPC440;
	
	static const uint32_t PROCESSOR_VERSION = 0x7ff21912UL; // PPC440x5 in Virtex-5 FXT
	 
	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint64_t virtual_address_t;     // only 41 bits are used, all remaining bits *must* be set to zero
	typedef uint64_t physical_address_t;    // 36-bit physical address, only 36 bits are used, all remaining bits *must* be set to zero
	typedef uint8_t process_id_t;           // 8-bit process ID
	typedef enum
	{
		AS_SYSTEM = 0,         // system address space
		AS_APPLICATION = 1     // application address space
	} address_space_t;         // address space

	// Hardware interrupt signals
	static const unsigned int IRQ_DECREMENTER_INTERRUPT = 1;            // decrementer interrupt (internal) = TSR[DIS]
	static const unsigned int IRQ_FIXED_INTERVAL_TIMER_INTERRUPT = 2;   // fixed interval timer interrupt (internal) = TSR[FIS]
	static const unsigned int IRQ_WATCHDOG_TIMER_INTERRUPT = 4;         // watchdog timer interrupt (internal) = TSR[WIS]
	static const unsigned int IRQ_EXTERNAL_INPUT_INTERRUPT = 8;         // external interrupt (external)
	static const unsigned int IRQ_CRITICAL_INPUT_INTERRUPT = 16;        // critical interrupt (external)

	// start address
	static const uint32_t START_ADDR = 0xfffffffcUL; // processor starts at the last word of memory space

	// Debug stuff
	static const bool DEBUG_ENABLE = false; // Debug is disabled by default
	static const bool DEBUG_SETUP_ENABLE = false;
	static const bool DEBUG_STEP_ENABLE = false;
	static const bool DEBUG_DTLB_ENABLE = false;
	static const bool DEBUG_ITLB_ENABLE = false;
	static const bool DEBUG_UTLB_ENABLE = false;
	static const bool DEBUG_DL1_ENABLE = false;
	static const bool DEBUG_IL1_ENABLE = false;
	static const bool DEBUG_LOAD_ENABLE = false;
	static const bool DEBUG_STORE_ENABLE = false;
	static const bool DEBUG_READ_MEMORY_ENABLE = false;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = false;
	static const bool DEBUG_EXCEPTION_ENABLE = false;
	static const bool DEBUG_SET_MSR_ENABLE = false;
	static const bool DEBUG_TLBWE_ENABLE = false;
	static const bool DEBUG_PRINTK_ENABLE = false;

	// MMU storage attribute
	typedef enum
	{
		SA_U0      = 256,
		SA_U1      = 128,
		SA_U2      = 64,
		SA_U3      = 32,
		SA_W       = 16,
		SA_I       = 8,
		SA_M       = 4,
		SA_G       = 2,
		SA_E       = 1,
		SA_DEFAULT = 0
	} STORAGE_ATTR;
	
	// MMU access control
	typedef enum
	{
		AC_UX      = 32,
		AC_UW      = 16,
		AC_UR      = 8,
		AC_SX      = 4,
		AC_SW      = 2,
		AC_SR      = 1,
		AC_DEFAULT = 0
	} ACCESS_CTRL;

	// L1 Data cache parameters
	class DL1_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			uint8_t dirty; // one bit per double word
		};

		class LINE_STATUS
		{
		public:
			bool valid;
			uint32_t way;
			STORAGE_ATTR storage_attr;
			unisim::component::cxx::cache::CacheLine<DL1_CONFIG> *next;
			unisim::component::cxx::cache::CacheLine<DL1_CONFIG> *prev;
		};

		class SET_STATUS
		{
		public:
			unisim::component::cxx::cache::CacheLine<DL1_CONFIG> *mru_line;
			unisim::component::cxx::cache::CacheLine<DL1_CONFIG> *lru_line;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef Config::physical_address_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; //32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 6; // 64-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 64-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
		typedef Config::STORAGE_ATTR STORAGE_ATTR;
	};

	// L1 Instruction cache parameters
	class IL1_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
			uint32_t way;
			unisim::component::cxx::cache::CacheLine<IL1_CONFIG> *next;
			unisim::component::cxx::cache::CacheLine<IL1_CONFIG> *prev;
		};

		class SET_STATUS
		{
		public:
			unisim::component::cxx::cache::CacheLine<IL1_CONFIG> *mru_line;
			unisim::component::cxx::cache::CacheLine<IL1_CONFIG> *lru_line;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef Config::virtual_address_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 6; // 64-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 64-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
		typedef Config::STORAGE_ATTR STORAGE_ATTR;
	};

	class TLB_ENTRY
	{
	public:
		void Reset()
		{
			w[0] = 0;
			w[1] = 0;
			w[2] = 0;
			tid = 0;
		}
		void Invalidate() { w[0] = w[0] & ~TLBE0_V_MASK; }
		uint32_t Get(unsigned int ws) const { return w[ws]; }
		process_id_t GetTID() const { return tid; }
		
		void Set(unsigned int ws, uint32_t value, process_id_t _tid = 0)
		{
			if(ws < 3)
			{
				w[ws] = value;
				if(ws == 0) tid = _tid;
			}
		}
		void SetTID(process_id_t _tid) { tid = _tid; }
		void Set(uint32_t epn, uint32_t v, address_space_t ts, uint32_t size, process_id_t _tid, uint32_t rpn, uint32_t erpn, STORAGE_ATTR storage_attr, ACCESS_CTRL access_ctrl)
		{
			Set(0, ((epn << TLBE0_EPN_OFFSET) & TLBE0_EPN_MASK) | ((v << TLBE0_V_OFFSET) & TLBE0_V_MASK) | (((uint32_t) ts << TLBE0_TS_OFFSET) & TLBE0_TS_MASK) | ((size << TLBE0_SIZE_OFFSET) & TLBE0_SIZE_MASK), _tid);
			Set(1, ((rpn << TLBE1_RPN_OFFSET) & TLBE1_RPN_MASK) | ((erpn << TLBE1_ERPN_OFFSET) & TLBE1_ERPN_MASK));
			Set(2, (((uint32_t) storage_attr << TLBE2_STORAGE_ATTR_OFFSET) & TLBE2_STORAGE_ATTR_MASK) | (((uint32_t) access_ctrl << TLBE2_ACCESS_CTRL_OFFSET) & TLBE2_ACCESS_CTRL_MASK));
		}
		
		uint32_t GetEPN() const { return (w[0] & TLBELayout::TLBE0_EPN_MASK) >> TLBELayout::TLBE0_EPN_OFFSET; }
		uint32_t GetV() const { return (w[0] & TLBELayout::TLBE0_V_MASK) >> TLBELayout::TLBE0_V_OFFSET; }
		address_space_t GetTS() const { return (address_space_t)((w[0] & TLBELayout::TLBE0_TS_MASK) >> TLBELayout::TLBE0_TS_OFFSET); }
		uint32_t GetSIZE() const { return (w[0] & TLBELayout::TLBE0_SIZE_MASK) >> TLBELayout::TLBE0_SIZE_OFFSET; }
		uint32_t GetTPAR() const { return (w[0] & TLBELayout::TLBE0_TPAR_MASK) >> TLBELayout::TLBE0_TPAR_OFFSET; }
		uint32_t GetRPN() const { return (w[1] & TLBELayout::TLBE1_RPN_MASK) >> TLBELayout::TLBE1_RPN_OFFSET; }
		uint32_t GetPAR1() const { return (w[1] & TLBELayout::TLBE1_PAR1_MASK) >> TLBELayout::TLBE1_PAR1_OFFSET; }
		uint32_t GetERPN() const { return (w[1] & TLBELayout::TLBE1_ERPN_MASK) >> TLBELayout::TLBE1_ERPN_OFFSET; }
		uint32_t GetPAR2() const { return (w[2] & TLBELayout::TLBE2_PAR2_MASK) >> TLBELayout::TLBE2_PAR2_OFFSET; }
		uint32_t GetU0() const { return (w[2] & TLBELayout::TLBE2_U0_MASK) >> TLBELayout::TLBE2_U0_OFFSET; }
		uint32_t GetU1() const { return (w[2] & TLBELayout::TLBE2_U1_MASK) >> TLBELayout::TLBE2_U1_OFFSET; }
		uint32_t GetU2() const { return (w[2] & TLBELayout::TLBE2_U2_MASK) >> TLBELayout::TLBE2_U2_OFFSET; }
		uint32_t GetU3() const { return (w[2] & TLBELayout::TLBE2_U3_MASK) >> TLBELayout::TLBE2_U3_OFFSET; }
		uint32_t GetW() const { return (w[2] & TLBELayout::TLBE2_W_MASK) >> TLBELayout::TLBE2_W_OFFSET; }
		uint32_t GetI() const { return (w[2] & TLBELayout::TLBE2_I_MASK) >> TLBELayout::TLBE2_I_OFFSET; }
		uint32_t GetM() const { return (w[2] & TLBELayout::TLBE2_M_MASK) >> TLBELayout::TLBE2_M_OFFSET; }
		uint32_t GetG() const { return (w[2] & TLBELayout::TLBE2_G_MASK) >> TLBELayout::TLBE2_G_OFFSET; }
		uint32_t GetE() const { return (w[2] & TLBELayout::TLBE2_E_MASK) >> TLBELayout::TLBE2_E_OFFSET; }
		STORAGE_ATTR GetStorageAttr() const { return (STORAGE_ATTR)((w[2] & TLBELayout::TLBE2_STORAGE_ATTR_MASK) >> TLBELayout::TLBE2_STORAGE_ATTR_OFFSET); }
		uint32_t GetUX() const { return (w[2] & TLBELayout::TLBE2_UX_MASK) >> TLBELayout::TLBE2_UX_OFFSET; }
		uint32_t GetUW() const { return (w[2] & TLBELayout::TLBE2_UW_MASK) >> TLBELayout::TLBE2_UW_OFFSET; }
		uint32_t GetUR() const { return (w[2] & TLBELayout::TLBE2_UR_MASK) >> TLBELayout::TLBE2_UR_OFFSET; }
		uint32_t GetSX() const { return (w[2] & TLBELayout::TLBE2_SX_MASK) >> TLBELayout::TLBE2_SX_OFFSET; }
		uint32_t GetSW() const { return (w[2] & TLBELayout::TLBE2_SW_MASK) >> TLBELayout::TLBE2_SW_OFFSET; }
		uint32_t GetSR() const { return (w[2] & TLBELayout::TLBE2_SR_MASK) >> TLBELayout::TLBE2_SR_OFFSET; }
		ACCESS_CTRL GetAccessCtrl() const { return (ACCESS_CTRL)((w[2] & TLBELayout::TLBE2_ACCESS_CTRL_MASK) >> TLBELayout::TLBE2_ACCESS_CTRL_OFFSET); }
		friend std::ostream& operator << (std::ostream& os, const TLB_ENTRY& tlbe);
	//private:
		uint32_t w[3];
		process_id_t tid;                       // transaction ID (TID)
	};
	
	// Shadow ITLB parameters
	class ITLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
//			bool valid;
			uint32_t way;
			unisim::component::cxx::tlb::TLBEntry<ITLB_CONFIG> *next;
			unisim::component::cxx::tlb::TLBEntry<ITLB_CONFIG> *prev;
		};

		class SET_STATUS
		{
		public:
			uint8_t victim_way;
			unisim::component::cxx::tlb::TLBEntry<ITLB_CONFIG> *mru_entry;
			unisim::component::cxx::tlb::TLBEntry<ITLB_CONFIG> *lru_entry;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 4;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 2; // 4-way set associative (fully associative)
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = 0; // unused because page size is variable, do not use DecodeAddress !
		typedef Config::TLB_ENTRY PTE;
	};

	// Shadow DTLB parameters
	class DTLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
//			bool valid;
			uint32_t way;
			unisim::component::cxx::tlb::TLBEntry<DTLB_CONFIG> *next;
			unisim::component::cxx::tlb::TLBEntry<DTLB_CONFIG> *prev;
		};

		class SET_STATUS
		{
		public:
			uint8_t victim_way;
			unisim::component::cxx::tlb::TLBEntry<DTLB_CONFIG> *mru_entry;
			unisim::component::cxx::tlb::TLBEntry<DTLB_CONFIG> *lru_entry;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 8;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 3; // 8-way set associative (fully associative)
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = 0; // unused because page size is variable, do not use DecodeAddress !
		typedef Config::TLB_ENTRY PTE;
	};

	// Unified TLB parameters
	class UTLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
//			bool valid;
			uint32_t way;
			unisim::component::cxx::tlb::TLBEntry<UTLB_CONFIG> *next;
			unisim::component::cxx::tlb::TLBEntry<UTLB_CONFIG> *prev;
		};

		class SET_STATUS
		{
		public:
			unisim::component::cxx::tlb::TLBEntry<UTLB_CONFIG> *mru_entry;
			unisim::component::cxx::tlb::TLBEntry<UTLB_CONFIG> *lru_entry;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 64;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 6; // 64-way set associative (fully associative)
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = 0; // unused, do not use DecodeAddress !
		typedef Config::TLB_ENTRY PTE;
	};

	// Front side bus parameters
	static const uint32_t FSB_BURST_SIZE = 32; // 256-bit burst
	static const uint32_t FSB_WIDTH = 16; // 128-bit front side bus

	// Simulation performance speed-up features
	static const bool PREFETCH_BUFFER_ENABLE = true; // enable faster fetch
	static const unsigned int NUM_PREFETCH_BUFFER_ENTRIES = 8; //!< Maximum number of instruction in the prefetch buffer
	static const bool FAST_DL1_LOOKUP_ENABLE = true;
	static const bool FAST_IL1_LOOKUP_ENABLE = true;
	static const bool FAST_DTLB_LOOKUP_ENABLE = true;
	static const bool FAST_ITLB_LOOKUP_ENABLE = true;
	static const bool FAST_UTLB_LOOKUP_ENABLE = true;

	// Programming model parameters
	static const unsigned int NUM_GPRS = 32;
	static const unsigned int NUM_FPRS = 32;
	static const unsigned int NUM_SPRGS = 8;
	static const unsigned int NUM_DNVS = 4;
	static const unsigned int NUM_DTVS = 4;
	static const unsigned int NUM_INVS = 4;
	static const unsigned int NUM_ITVS = 4;
	static const unsigned int NUM_DACS = 2;
	static const unsigned int NUM_DBCRS = 3;
	static const unsigned int NUM_DVCS = 3;
	static const unsigned int NUM_IACS = 4;
	static const unsigned int NUM_IVORS = 16;
	static const uint32_t PS_CAP = PS_1K | PS_4K | PS_16K | PS_64K | PS_256K | PS_1MB | PS_16MB | PS_256MB; // supported page sizes
	
	// Interrupt vector numbers
	static const unsigned int IVOR_CRITICAL_INPUT = 0;
	static const unsigned int IVOR_MACHINE_CHECK = 1;
	static const unsigned int IVOR_DATA_STORAGE = 2;
	static const unsigned int IVOR_INSTRUCTION_STORAGE = 3;
	static const unsigned int IVOR_EXTERNAL_INPUT = 4;
	static const unsigned int IVOR_ALIGNMENT = 5;
	static const unsigned int IVOR_PROGRAM = 6;
	static const unsigned int IVOR_FLOATING_POINT_UNAVAILABLE = 7;
	static const unsigned int IVOR_SYSTEM_CALL = 8;
	static const unsigned int IVOR_AUXILIARY_PROCESSOR_UNAVAILABLE = 9;
	static const unsigned int IVOR_DECREMENTER = 10;
	static const unsigned int IVOR_FIXED_INTERVAL_TIMER = 11;
	static const unsigned int IVOR_WATCHDOG_TIMER = 12;
	static const unsigned int IVOR_DATA_TLB_ERROR = 13;
	static const unsigned int IVOR_INSTRUCTION_TLB_ERROR = 14;
	static const unsigned int IVOR_DEBUG = 15;
	
	// MMU
	static const bool HAS_MMU = true;
	
	// Altivec
	static const bool HAS_ALTIVEC = false;
	
	// Floating point
	static const bool HAS_FPU = false;
	
	// Optional floating-point sqrt
	static const bool HAS_FLOATING_POINT_SQRT = true;
	
	// Optional floating-point graphics instructions
	static const bool HAS_FLOATING_POINT_GRAPHICS_INSTRUCTIONS = false;
};

inline std::ostream& operator << (std::ostream& os, const Config::TLB_ENTRY& tlbe)
{
	os << std::hex << "EPN=0x" << tlbe.GetEPN()
	   << " V=" << tlbe.GetV()
	   << " TS=" << tlbe.GetTS()
	   << " SIZE=0x" << tlbe.GetSIZE()
	   << " TPAR=0x" << tlbe.GetTPAR()
	   << " RPN=0x" << tlbe.GetRPN()
	   << " PAR1=0x" << tlbe.GetPAR1()
	   << " TID=0x" << (unsigned int) tlbe.GetTID()
	   << " ERPN=0x" << tlbe.GetERPN()
	   << " PAR2=0x" << tlbe.GetPAR2()
	   << " U0=" << tlbe.GetU0()
	   << " U1=" << tlbe.GetU1()
	   << " U2=" << tlbe.GetU2()
	   << " U3=" << tlbe.GetU3()
	   << " W=" << tlbe.GetW()
	   << " I=" << tlbe.GetI()
	   << " M=" << tlbe.GetM()
	   << " G=" << tlbe.GetG()
	   << " E=" << tlbe.GetE()
	   << " UX=" << tlbe.GetUX()
	   << " UW=" << tlbe.GetUW()
	   << " UR=" << tlbe.GetUR()
	   << " SX=" << tlbe.GetSX()
	   << " SW=" << tlbe.GetSW()
	   << " SR=" << tlbe.GetSR();
	return os;
}

class Config_wFPU : public Config
{
public:
	typedef CPU<Config_wFPU> STATE;

	// Floating point
	static const bool HAS_FPU = true;
};

class Config_woMMU_wFPU : public Config
{
public:
	typedef CPU<Config_woMMU_wFPU> STATE;

	// MMU
	static const bool HAS_MMU = false;
	
	// Floating point
	static const bool HAS_FPU = true;
};

class DebugConfig : public Config
{
public:
	typedef CPU<DebugConfig> STATE;

	// Debug stuff
	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_SETUP_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	static const bool DEBUG_DTLB_ENABLE = true;
	static const bool DEBUG_ITLB_ENABLE = true;
	static const bool DEBUG_UTLB_ENABLE = true;
	static const bool DEBUG_DL1_ENABLE = true;
	static const bool DEBUG_IL1_ENABLE = true;
	static const bool DEBUG_LOAD_ENABLE = true;
	static const bool DEBUG_STORE_ENABLE = true;
	static const bool DEBUG_READ_MEMORY_ENABLE = true;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = true;
	static const bool DEBUG_EXCEPTION_ENABLE = true;
	static const bool DEBUG_SET_MSR_ENABLE = true;
	static const bool DEBUG_PRINTK_ENABLE = true;
	static const bool DEBUG_TLBWE_ENABLE = true;
};

class DebugConfig_wFPU : public Config_wFPU
{
public:
	typedef CPU<DebugConfig_wFPU> STATE;

	// Debug stuff
	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_SETUP_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	static const bool DEBUG_DTLB_ENABLE = true;
	static const bool DEBUG_ITLB_ENABLE = true;
	static const bool DEBUG_UTLB_ENABLE = true;
	static const bool DEBUG_DL1_ENABLE = true;
	static const bool DEBUG_IL1_ENABLE = true;
	static const bool DEBUG_LOAD_ENABLE = true;
	static const bool DEBUG_STORE_ENABLE = true;
	static const bool DEBUG_READ_MEMORY_ENABLE = true;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = true;
	static const bool DEBUG_EXCEPTION_ENABLE = true;
	static const bool DEBUG_SET_MSR_ENABLE = true;
	static const bool DEBUG_PRINTK_ENABLE = true;
	static const bool DEBUG_TLBWE_ENABLE = true;
};

class DebugConfig_woMMU_wFPU : public Config_woMMU_wFPU
{
public:
	typedef CPU<DebugConfig_woMMU_wFPU> STATE;

	// Debug stuff
	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_SETUP_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	static const bool DEBUG_DTLB_ENABLE = true;
	static const bool DEBUG_ITLB_ENABLE = true;
	static const bool DEBUG_UTLB_ENABLE = true;
	static const bool DEBUG_DL1_ENABLE = true;
	static const bool DEBUG_IL1_ENABLE = true;
	static const bool DEBUG_LOAD_ENABLE = true;
	static const bool DEBUG_STORE_ENABLE = true;
	static const bool DEBUG_READ_MEMORY_ENABLE = true;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = true;
	static const bool DEBUG_EXCEPTION_ENABLE = true;
	static const bool DEBUG_SET_MSR_ENABLE = true;
	static const bool DEBUG_PRINTK_ENABLE = true;
	static const bool DEBUG_TLBWE_ENABLE = true;
};

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
