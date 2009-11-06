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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template <class CONFIG>
class CPU;

//=====================================================================
//=                  PowerPC common configuration                     =
//=====================================================================

class CRLayout
{
public:
	// CR fields offsets (here CR0)
	static const unsigned int CR0_LT_OFFSET = 31;
	static const unsigned int CR0_GT_OFFSET = 30;
	static const unsigned int CR0_EQ_OFFSET = 29;
	static const unsigned int CR0_SO_OFFSET = 28;

	// CR fields sizes
	static const unsigned int CR0_LT_BITSIZE = 1;
	static const unsigned int CR0_GT_BITSIZE = 1;
	static const unsigned int CR0_EQ_BITSIZE = 1;
	static const unsigned int CR0_SO_BITSIZE = 1;
	
	static const uint32_t CR0_LT_MASK = ((1UL << CR0_LT_BITSIZE) - 1) << CR0_LT_OFFSET;
	static const uint32_t CR0_GT_MASK = ((1UL << CR0_GT_BITSIZE) - 1) << CR0_GT_OFFSET;
	static const uint32_t CR0_EQ_MASK = ((1UL << CR0_EQ_BITSIZE) - 1) << CR0_EQ_OFFSET;
	static const uint32_t CR0_SO_MASK = ((1UL << CR0_SO_BITSIZE) - 1) << CR0_SO_OFFSET;
	static const uint32_t CR0_MASK = CR0_LT_MASK | CR0_GT_MASK | CR0_EQ_MASK | CR0_SO_MASK;
};

class XERLayout
{
public:
	// XER fields offsets
	static const unsigned int XER_SO_OFFSET = 31;
	static const unsigned int XER_OV_OFFSET = 30;
	static const unsigned int XER_CA_OFFSET = 29;
	static const unsigned int XER_LEGACY_OFFSET = 8;
	static const unsigned int XER_BYTE_COUNT_OFFSET = 0;

	// XER fields sizes
	static const unsigned int XER_SO_BITSIZE = 1;
	static const unsigned int XER_OV_BITSIZE = 1;
	static const unsigned int XER_CA_BITSIZE = 1;
	static const unsigned int XER_LEGACY_BITSIZE = 8;
	static const unsigned int XER_BYTE_COUNT_BITSIZE = 7;
	
	static const uint32_t XER_SO_MASK = ((1UL << XER_SO_BITSIZE) - 1) << XER_SO_OFFSET;
	static const uint32_t XER_OV_MASK = ((1UL << XER_OV_BITSIZE) - 1) << XER_OV_OFFSET;
	static const uint32_t XER_CA_MASK = ((1UL << XER_CA_BITSIZE) - 1) << XER_CA_OFFSET;
	static const uint32_t XER_LEGACY_MASK = ((1UL << XER_LEGACY_BITSIZE) - 1) << XER_LEGACY_OFFSET;
	static const uint32_t XER_BYTE_COUNT_MASK = ((1UL << XER_BYTE_COUNT_BITSIZE) - 1) << XER_BYTE_COUNT_OFFSET;

	static const uint32_t XER_MASK = XER_SO_MASK | XER_OV_MASK | XER_CA_MASK | XER_LEGACY_MASK | XER_BYTE_COUNT_MASK;

};

class FPSCRLayout
{
public:
	// Bit fields layout: FX FEX VX OX UX ZX XX VXSNAN VXISI VXIDI VXZDZ VXIMZ VXVC FR FI FPRF - VXSOFT VXSQRT VXCVI VE OE UE ZE XE NI RN

	// FPSCR write only bit fields while floating point computations
	static const unsigned int FPSCR_FX_OFFSET = 31;
	static const unsigned int FPSCR_FEX_OFFSET = 30;
	static const unsigned int FPSCR_VX_OFFSET = 29;
	static const unsigned int FPSCR_OX_OFFSET = 28;
	static const unsigned int FPSCR_UX_OFFSET = 27;
	static const unsigned int FPSCR_ZX_OFFSET = 26;
	static const unsigned int FPSCR_XX_OFFSET = 25;
	static const unsigned int FPSCR_VXSNAN_OFFSET = 24;
	static const unsigned int FPSCR_VXISI_OFFSET = 23;
	static const unsigned int FPSCR_VXIDI_OFFSET = 22;
	static const unsigned int FPSCR_VXZDZ_OFFSET = 21;
	static const unsigned int FPSCR_VXIMZ_OFFSET = 20;
	static const unsigned int FPSCR_VXVC_OFFSET = 19;
	static const unsigned int FPSCR_FR_OFFSET = 18;
	static const unsigned int FPSCR_FI_OFFSET = 17;
	static const unsigned int FPSCR_FPRF_OFFSET = 12;
	static const unsigned int FPSCR_FPCC_OFFSET = 12;
	static const unsigned int FPSCR_VXSOFT_OFFSET = 10;
	static const unsigned int FPSCR_VXSQRT_OFFSET = 9;
	static const unsigned int FPSCR_VXCVI_OFFSET = 8;
	
	// FPSCR read only bit fields while floating point computations
	static const unsigned int FPSCR_VE_OFFSET = 7;
	static const unsigned int FPSCR_OE_OFFSET = 6;
	static const unsigned int FPSCR_UE_OFFSET = 5;
	static const unsigned int FPSCR_ZE_OFFSET = 4;
	static const unsigned int FPSCR_XE_OFFSET = 3;
	static const unsigned int FPSCR_NI_OFFSET = 2;
	static const unsigned int FPSCR_RN_OFFSET = 0;

	// bit sizes of FPSCR write only bit fields while floating point computations
	static const unsigned int FPSCR_FX_BITSIZE = 1;
	static const unsigned int FPSCR_FEX_BITSIZE = 1;
	static const unsigned int FPSCR_VX_BITSIZE = 1;
	static const unsigned int FPSCR_OX_BITSIZE = 1;
	static const unsigned int FPSCR_UX_BITSIZE = 1;
	static const unsigned int FPSCR_ZX_BITSIZE = 1;
	static const unsigned int FPSCR_XX_BITSIZE = 1;
	static const unsigned int FPSCR_VXSNAN_BITSIZE = 1;
	static const unsigned int FPSCR_VXISI_BITSIZE = 1;
	static const unsigned int FPSCR_VXIDI_BITSIZE = 1;
	static const unsigned int FPSCR_VXZDZ_BITSIZE = 1;
	static const unsigned int FPSCR_VXIMZ_BITSIZE = 1;
	static const unsigned int FPSCR_VXVC_BITSIZE = 1;
	static const unsigned int FPSCR_FR_BITSIZE = 1;
	static const unsigned int FPSCR_FI_BITSIZE = 1;
	static const unsigned int FPSCR_FPRF_BITSIZE = 5;
	static const unsigned int FPSCR_FPCC_BITSIZE = 4;
	static const unsigned int FPSCR_VXSOFT_BITSIZE = 1;
	static const unsigned int FPSCR_VXSQRT_BITSIZE = 1;
	static const unsigned int FPSCR_VXCVI_BITSIZE = 1;

	// bit sizes of FPSCR read only bit fields while floating point computations
	static const unsigned int FPSCR_VE_BITSIZE = 1;
	static const unsigned int FPSCR_OE_BITSIZE = 1;
	static const unsigned int FPSCR_UE_BITSIZE = 1;
	static const unsigned int FPSCR_ZE_BITSIZE = 1;
	static const unsigned int FPSCR_XE_BITSIZE = 1;
	static const unsigned int FPSCR_NI_BITSIZE = 1;
	static const unsigned int FPSCR_RN_BITSIZE = 2;

	// binary masks of FPSCR write only bit fields while floating point computations
	static const uint32_t FPSCR_FX_MASK = ((1UL << FPSCR_FX_BITSIZE) - 1) << FPSCR_FX_OFFSET;
	static const uint32_t FPSCR_FEX_MASK = ((1UL << FPSCR_FEX_BITSIZE) - 1) << FPSCR_FEX_OFFSET;
	static const uint32_t FPSCR_VX_MASK = ((1UL << FPSCR_VX_BITSIZE) - 1) << FPSCR_VX_OFFSET;
	static const uint32_t FPSCR_OX_MASK = ((1UL << FPSCR_OX_BITSIZE) - 1) << FPSCR_OX_OFFSET;
	static const uint32_t FPSCR_UX_MASK = ((1UL << FPSCR_UX_BITSIZE) - 1) << FPSCR_UX_OFFSET;
	static const uint32_t FPSCR_ZX_MASK = ((1UL << FPSCR_ZX_BITSIZE) - 1) << FPSCR_ZX_OFFSET;
	static const uint32_t FPSCR_XX_MASK = ((1UL << FPSCR_XX_BITSIZE) - 1) << FPSCR_XX_OFFSET;
	static const uint32_t FPSCR_VXSNAN_MASK = ((1UL << FPSCR_VXSNAN_BITSIZE) - 1) << FPSCR_VXSNAN_OFFSET;
	static const uint32_t FPSCR_VXISI_MASK = ((1UL << FPSCR_VXISI_BITSIZE) - 1) << FPSCR_VXISI_OFFSET;
	static const uint32_t FPSCR_VXIDI_MASK = ((1UL << FPSCR_VXIDI_BITSIZE) - 1) << FPSCR_VXIDI_OFFSET;
	static const uint32_t FPSCR_VXZDZ_MASK = ((1UL << FPSCR_VXZDZ_BITSIZE) - 1) << FPSCR_VXZDZ_OFFSET;
	static const uint32_t FPSCR_VXIMZ_MASK = ((1UL << FPSCR_VXIMZ_BITSIZE) - 1) << FPSCR_VXIMZ_OFFSET;
	static const uint32_t FPSCR_VXVC_MASK = ((1UL << FPSCR_VXVC_BITSIZE) - 1) << FPSCR_VXVC_OFFSET;
	static const uint32_t FPSCR_FR_MASK = ((1UL << FPSCR_FR_BITSIZE) - 1) << FPSCR_FR_OFFSET;
	static const uint32_t FPSCR_FI_MASK = ((1UL << FPSCR_FI_BITSIZE) - 1) << FPSCR_FI_OFFSET;
	static const uint32_t FPSCR_FPRF_MASK = ((1UL << FPSCR_FPRF_BITSIZE) - 1) << FPSCR_FPRF_OFFSET;
	static const uint32_t FPSCR_FPCC_MASK = ((1UL << FPSCR_FPCC_BITSIZE) - 1) << FPSCR_FPCC_OFFSET;
	static const uint32_t FPSCR_VXSOFT_MASK = ((1UL << FPSCR_VXSOFT_BITSIZE) - 1) << FPSCR_VXSOFT_OFFSET;
	static const uint32_t FPSCR_VXSQRT_MASK = ((1UL << FPSCR_VXSQRT_BITSIZE) - 1) << FPSCR_VXSQRT_OFFSET;
	static const uint32_t FPSCR_VXCVI_MASK = ((1UL << FPSCR_VXCVI_BITSIZE) - 1) << FPSCR_VXCVI_OFFSET;

	// binary masks of FPSCR read only bit fields while floating point computations
	static const uint32_t FPSCR_VE_MASK = ((1UL << FPSCR_VE_BITSIZE) - 1) << FPSCR_VE_OFFSET;
	static const uint32_t FPSCR_OE_MASK = ((1UL << FPSCR_OE_BITSIZE) - 1) << FPSCR_OE_OFFSET;
	static const uint32_t FPSCR_UE_MASK = ((1UL << FPSCR_UE_BITSIZE) - 1) << FPSCR_UE_OFFSET;
	static const uint32_t FPSCR_ZE_MASK = ((1UL << FPSCR_ZE_BITSIZE) - 1) << FPSCR_ZE_OFFSET;
	static const uint32_t FPSCR_XE_MASK = ((1UL << FPSCR_XE_BITSIZE) - 1) << FPSCR_XE_OFFSET;
	static const uint32_t FPSCR_NI_MASK = ((1UL << FPSCR_NI_BITSIZE) - 1) << FPSCR_NI_OFFSET;
	static const uint32_t FPSCR_RN_MASK = ((1UL << FPSCR_RN_BITSIZE) - 1) << FPSCR_RN_OFFSET;

	// binary mask of FPSCR write only bit fields while floating point computations
	static const uint32_t FPSCR_WRONLY_MASK = FPSCR_FX_MASK | FPSCR_FEX_MASK | FPSCR_VX_MASK | FPSCR_OX_MASK | FPSCR_UX_MASK | FPSCR_ZX_MASK |
	                                          FPSCR_XX_MASK | FPSCR_VXSNAN_MASK | FPSCR_VXISI_MASK | FPSCR_VXIDI_MASK | FPSCR_VXZDZ_MASK |
	                                          FPSCR_VXIMZ_MASK | FPSCR_VXVC_MASK | FPSCR_FR_MASK | FPSCR_FI_MASK | FPSCR_FPRF_MASK | FPSCR_FPCC_MASK |
	                                          FPSCR_VXSOFT_MASK | FPSCR_VXSQRT_MASK | FPSCR_VXCVI_MASK;

	// binary mask of FPSCR read only bit fields while floating point computations
	static const uint32_t FPSCR_RDONLY_MASK = FPSCR_VE_MASK | FPSCR_OE_MASK | FPSCR_UE_MASK | FPSCR_ZE_MASK | FPSCR_XE_MASK | FPSCR_NI_MASK | FPSCR_RN_MASK;

	// binary mask of FPSCR sticky write only bit fields while floating point computations
	static const uint32_t FPSCR_STICKY_MASK = FPSCR_FX_MASK | FPSCR_OX_MASK | FPSCR_UX_MASK | FPSCR_ZX_MASK | FPSCR_XX_MASK | FPSCR_VXSNAN_MASK |
	                                          FPSCR_VXISI_MASK | FPSCR_VXIDI_MASK | FPSCR_VXZDZ_MASK | FPSCR_VXIMZ_MASK | FPSCR_VXVC_MASK |
	                                          FPSCR_VXSOFT_MASK | FPSCR_VXSQRT_MASK | FPSCR_VXCVI_MASK;

	// binary mask of FPSCR not sticky write only bit fields while floating point computations
	static const uint32_t FPSCR_NOT_STICKY_MASK = FPSCR_FEX_MASK | FPSCR_VX_MASK | FPSCR_FR_MASK | FPSCR_FI_MASK | FPSCR_FPRF_MASK;

	// meaning of FPRF bit field
	static const uint32_t FPRF_FOR_QNAN = 0x11UL;
	static const uint32_t FPRF_FOR_NEGATIVE_INFINITY = 0x9UL;
	static const uint32_t FPRF_FOR_NEGATIVE_NORMAL = 0x8UL;
	static const uint32_t FPRF_FOR_NEGATIVE_DENORMAL = 0x18UL;
	static const uint32_t FPRF_FOR_NEGATIVE_ZERO = 0x12UL;
	static const uint32_t FPRF_FOR_POSITIVE_ZERO = 0x2UL;
	static const uint32_t FPRF_FOR_POSITIVE_DENORMAL = 0x14UL;
	static const uint32_t FPRF_FOR_POSITIVE_NORMAL = 0x4UL;
	static const uint32_t FPRF_FOR_POSITIVE_INFINITY = 0x5UL;
};
	
class MSRLayout
{
public:
	// MSR fields offsets
	static const unsigned int MSR_VEC_OFFSET = 25;
	static const unsigned int MSR_POW_OFFSET = 18;
	static const unsigned int MSR_ILE_OFFSET = 16;
	static const unsigned int MSR_EE_OFFSET = 15;
	static const unsigned int MSR_PR_OFFSET = 14;
	static const unsigned int MSR_FP_OFFSET = 13;
	static const unsigned int MSR_ME_OFFSET = 12;
	static const unsigned int MSR_FE0_OFFSET = 11;
	static const unsigned int MSR_SE_OFFSET = 10;
	static const unsigned int MSR_BE_OFFSET = 9;
	static const unsigned int MSR_FE1_OFFSET = 8;
	static const unsigned int MSR_IP_OFFSET = 6;
	static const unsigned int MSR_IR_OFFSET = 5;
	static const unsigned int MSR_DR_OFFSET = 4;
	static const unsigned int MSR_PM_OFFSET = 2;
	static const unsigned int MSR_RI_OFFSET = 1;
	static const unsigned int MSR_LE_OFFSET = 0;

	// MSR fields sizes
	static const unsigned int MSR_VEC_BITSIZE = 1;
	static const unsigned int MSR_POW_BITSIZE = 1;
	static const unsigned int MSR_ILE_BITSIZE = 1;
	static const unsigned int MSR_EE_BITSIZE = 1;
	static const unsigned int MSR_PR_BITSIZE = 1;
	static const unsigned int MSR_FP_BITSIZE = 1;
	static const unsigned int MSR_ME_BITSIZE = 1;
	static const unsigned int MSR_FE0_BITSIZE = 1;
	static const unsigned int MSR_SE_BITSIZE = 1;
	static const unsigned int MSR_BE_BITSIZE = 1;
	static const unsigned int MSR_FE1_BITSIZE = 1;
	static const unsigned int MSR_IP_BITSIZE = 1;
	static const unsigned int MSR_IR_BITSIZE = 1;
	static const unsigned int MSR_DR_BITSIZE = 1;
	static const unsigned int MSR_PM_BITSIZE = 1;
	static const unsigned int MSR_RI_BITSIZE = 1;
	static const unsigned int MSR_LE_BITSIZE = 1;

	static const uint32_t MSR_VEC_MASK = ((1UL << MSR_VEC_BITSIZE) - 1) << MSR_VEC_OFFSET;
	static const uint32_t MSR_POW_MASK = ((1UL << MSR_POW_BITSIZE) - 1) << MSR_POW_OFFSET;
	static const uint32_t MSR_ILE_MASK = ((1UL << MSR_ILE_BITSIZE) - 1) << MSR_ILE_OFFSET;
	static const uint32_t MSR_EE_MASK = ((1UL << MSR_EE_BITSIZE) - 1) << MSR_EE_OFFSET;
	static const uint32_t MSR_PR_MASK = ((1UL << MSR_PR_BITSIZE) - 1) << MSR_PR_OFFSET;
	static const uint32_t MSR_FP_MASK = ((1UL << MSR_FP_BITSIZE) - 1) << MSR_FP_OFFSET;
	static const uint32_t MSR_ME_MASK = ((1UL << MSR_ME_BITSIZE) - 1) << MSR_ME_OFFSET;
	static const uint32_t MSR_FE0_MASK = ((1UL << MSR_FE0_BITSIZE) - 1) << MSR_FE0_OFFSET;
	static const uint32_t MSR_SE_MASK = ((1UL << MSR_SE_BITSIZE) - 1) << MSR_SE_OFFSET;
	static const uint32_t MSR_BE_MASK = ((1UL << MSR_BE_BITSIZE) - 1) << MSR_BE_OFFSET;
	static const uint32_t MSR_FE1_MASK = ((1UL << MSR_FE1_BITSIZE) - 1) << MSR_FE1_OFFSET;
	static const uint32_t MSR_IP_MASK = ((1UL << MSR_IP_BITSIZE) - 1) << MSR_IP_OFFSET;
	static const uint32_t MSR_IR_MASK = ((1UL << MSR_IR_BITSIZE) - 1) << MSR_IR_OFFSET;
	static const uint32_t MSR_DR_MASK = ((1UL << MSR_DR_BITSIZE) - 1) << MSR_DR_OFFSET;
	static const uint32_t MSR_PM_MASK = ((1UL << MSR_PM_BITSIZE) - 1) << MSR_PM_OFFSET;
	static const uint32_t MSR_RI_MASK = ((1UL << MSR_RI_BITSIZE) - 1) << MSR_RI_OFFSET;
	static const uint32_t MSR_LE_MASK = ((1UL << MSR_RI_BITSIZE) - 1) << MSR_LE_OFFSET;
	
	static const uint32_t MSR_MASK = MSR_VEC_MASK | MSR_POW_MASK | MSR_ILE_MASK | MSR_EE_MASK | MSR_PR_MASK | MSR_FP_MASK | MSR_ME_MASK | MSR_FE0_MASK |
	                                 MSR_SE_MASK | MSR_BE_MASK | MSR_FE1_MASK | MSR_IP_MASK | MSR_IR_MASK | MSR_DR_MASK | MSR_PM_MASK | MSR_RI_MASK | MSR_LE_MASK;

};

class L2CRLayout
{
public:
	static const unsigned int L2CR_L2E_OFFSET = 0;
	static const unsigned int L2CR_L2PE_OFFSET = 0;
	static const unsigned int L2CR_L2SIZ_OFFSET = 0;
	static const unsigned int L2CR_L2CLK_OFFSET = 0;
	static const unsigned int L2CR_L2RAM_OFFSET = 0;
	static const unsigned int L2CR_L2DR_OFFSET = 0;
	static const unsigned int L2CR_L2DO_OFFSET = 0;
	static const unsigned int L2CR_L2I_OFFSET = 0;
	static const unsigned int L2CR_L2CTL_OFFSET = 0;
	static const unsigned int L2CR_L2WT_OFFSET = 0;
	static const unsigned int L2CR_L2TS_OFFSET = 0;
	static const unsigned int L2CR_L2OH_OFFSET = 0;
	static const unsigned int L2CR_L2SL_OFFSET = 0;
	static const unsigned int L2CR_L2DF_OFFSET = 0;
	static const unsigned int L2CR_L2BYP_OFFSET = 0;
	static const unsigned int L2CR_L2FA_OFFSET = 0;
	static const unsigned int L2CR_L2REP_OFFSET = 0;
	static const unsigned int L2CR_L2HWF_OFFSET = 0;
	static const unsigned int L2CR_L2IO_OFFSET = 0;
	static const unsigned int L2CR_L2CLKSTP_OFFSET = 0;
	static const unsigned int L2CR_L2DRO_OFFSET = 0;
	static const unsigned int L2CR_L2IP_OFFSET = 0;
	static const unsigned int L2CR_LVRAME_OFFSET = 0;
	static const unsigned int L2CR_LVRAMM_OFFSET = 0;
	
	static const uint32_t L2CR_L2E_BITSIZE = 0;
	static const uint32_t L2CR_L2PE_BITSIZE = 0;
	static const uint32_t L2CR_L2SIZ_BITSIZE = 0;
	static const uint32_t L2CR_L2CLK_BITSIZE = 0;
	static const uint32_t L2CR_L2RAM_BITSIZE = 0;
	static const uint32_t L2CR_L2DR_BITSIZE = 0;
	static const uint32_t L2CR_L2DO_BITSIZE = 0;
	static const uint32_t L2CR_L2I_BITSIZE = 0;
	static const uint32_t L2CR_L2CTL_BITSIZE = 0;
	static const uint32_t L2CR_L2WT_BITSIZE = 0;
	static const uint32_t L2CR_L2TS_BITSIZE = 0;
	static const uint32_t L2CR_L2OH_BITSIZE = 0;
	static const uint32_t L2CR_L2SL_BITSIZE = 0;
	static const uint32_t L2CR_L2DF_BITSIZE = 0;
	static const uint32_t L2CR_L2BYP_BITSIZE = 0;
	static const uint32_t L2CR_L2FA_BITSIZE = 0;
	static const uint32_t L2CR_L2REP_BITSIZE = 0;
	static const uint32_t L2CR_L2HWF_BITSIZE = 0;
	static const uint32_t L2CR_L2IO_BITSIZE = 0;
	static const uint32_t L2CR_L2CLKSTP_BITSIZE = 0;
	static const uint32_t L2CR_L2DRO_BITSIZE = 0;
	static const uint32_t L2CR_L2IP_BITSIZE = 0;
	static const uint32_t L2CR_LVRAME_BITSIZE = 0;
	static const uint32_t L2CR_LVRAMM_BITSIZE = 0;
};

class HID0Layout
{
public:
	// HID0 bits offsets
	static const unsigned int HID0_EMCP_OFFSET = 31;
	static const unsigned int HID0_DBP_OFFSET = 30;
	static const unsigned int HID0_EBA_OFFSET = 29;
	static const unsigned int HID0_EBD_OFFSET = 28;
	static const unsigned int HID0_SBCLK_OFFSET = 27;
	static const unsigned int HID0_BCLK_OFFSET = 27;
	static const unsigned int HID0_TBEN_OFFSET = 26;
	static const unsigned int HID0_EICE_OFFSET = 26;
	static const unsigned int HID0_ECLK_OFFSET = 25;
	static const unsigned int HID0_PAR_OFFSET = 24;
	static const unsigned int HID0_STEN_OFFSET = 24;
	static const unsigned int HID0_DOZE_OFFSET = 23;
	static const unsigned int HID0_HIGH_BAT_EN_OFFSET = 23;
	static const unsigned int HID0_NAP_OFFSET = 22;
	static const unsigned int HID0_SLEEP_OFFSET = 21;
	static const unsigned int HID0_DPM_OFFSET = 20;
	static const unsigned int HID0_RISEG_OFFSET = 19;
	static const unsigned int HID0_BHTCLR_OFFSET = 18;
	static const unsigned int HID0_EIEC_OFFSET = 17;
	static const unsigned int HID0_XAEN_OFFSET = 17;
	static const unsigned int HID0_NHR_OFFSET = 16;
	static const unsigned int HID0_ICE_OFFSET = 15;
	static const unsigned int HID0_DCE_OFFSET = 14;
	static const unsigned int HID0_ILOCK_OFFSET = 13;
	static const unsigned int HID0_DLOCK_OFFSET = 12;
	static const unsigned int HID0_ICFI_OFFSET = 11;
	static const unsigned int HID0_DCFI_OFFSET = 10;
	static const unsigned int HID0_SPD_OFFSET = 9;
	static const unsigned int HID0_IFEM_OFFSET = 8;
	static const unsigned int HID0_IFTT_OFFSET = 8;
	static const unsigned int HID0_XBSEN_OFFSET = 8;
	static const unsigned int HID0_SIED_OFFSET = 7;
	static const unsigned int HID0_SGE_OFFSET = 7;
	static const unsigned int HID0_DCFA_OFFSET = 6;
	static const unsigned int HID0_BTIC_OFFSET = 5;
	static const unsigned int HID0_LRSTK_OFFSET = 4;
	static const unsigned int HID0_ABE_OFFSET = 3;
	static const unsigned int HID0_FOLD_OFFSET = 3;
	static const unsigned int HID0_BHT_OFFSET = 2;
	static const unsigned int HID0_NOPDST_OFFSET = 1;
	static const unsigned int HID0_NOPTI_OFFSET = 0;
	
	// HID0 bits
	static const unsigned int HID0_EMCP_BITSIZE = 1;
	static const unsigned int HID0_DBP_BITSIZE = 1;
	static const unsigned int HID0_EBA_BITSIZE = 1;
	static const unsigned int HID0_EBD_BITSIZE = 1;
	static const unsigned int HID0_SBCLK_BITSIZE = 1;
	static const unsigned int HID0_BCLK_BITSIZE = 1;
	static const unsigned int HID0_TBEN_BITSIZE = 1;
	static const unsigned int HID0_EICE_BITSIZE = 1;
	static const unsigned int HID0_ECLK_BITSIZE = 1;
	static const unsigned int HID0_PAR_BITSIZE = 1;
	static const unsigned int HID0_STEN_BITSIZE = 1;
	static const unsigned int HID0_DOZE_BITSIZE = 1;
	static const unsigned int HID0_HIGH_BAT_EN_BITSIZE = 1;
	static const unsigned int HID0_NAP_BITSIZE = 1;
	static const unsigned int HID0_SLEEP_BITSIZE = 1;
	static const unsigned int HID0_DPM_BITSIZE = 1;
	static const unsigned int HID0_RISEG_BITSIZE = 1;
	static const unsigned int HID0_BHTCLR_BITSIZE = 1;
	static const unsigned int HID0_EIEC_BITSIZE = 1;
	static const unsigned int HID0_XAEN_BITSIZE = 1;
	static const unsigned int HID0_NHR_BITSIZE = 1;
	static const unsigned int HID0_ICE_BITSIZE = 1;
	static const unsigned int HID0_DCE_BITSIZE = 1;
	static const unsigned int HID0_ILOCK_BITSIZE = 1;
	static const unsigned int HID0_DLOCK_BITSIZE = 1;
	static const unsigned int HID0_ICFI_BITSIZE = 1;
	static const unsigned int HID0_DCFI_BITSIZE = 1;
	static const unsigned int HID0_SPD_BITSIZE = 1;
	static const unsigned int HID0_IFEM_BITSIZE = 1;
	static const unsigned int HID0_IFTT_BITSIZE = 1;
	static const unsigned int HID0_XBSEN_BITSIZE = 1;
	static const unsigned int HID0_SIED_BITSIZE = 1;
	static const unsigned int HID0_SGE_BITSIZE = 1;
	static const unsigned int HID0_DCFA_BITSIZE = 1;
	static const unsigned int HID0_BTIC_BITSIZE = 1;
	static const unsigned int HID0_LRSTK_BITSIZE = 1;
	static const unsigned int HID0_ABE_BITSIZE = 1;
	static const unsigned int HID0_FOLD_BITSIZE = 1;
	static const unsigned int HID0_BHT_BITSIZE = 1;
	static const unsigned int HID0_NOPDST_BITSIZE = 1;
	static const unsigned int HID0_NOPTI_BITSIZE = 1;
};

class HID1Layout
{
public:
	// HID1 bits
	static const unsigned int HID1_PC0_OFFSET = 0;
	static const unsigned int HID1_PC1_OFFSET = 0;
	static const unsigned int HID1_PC2_OFFSET = 0;
	static const unsigned int HID1_PC3_OFFSET = 0;
	static const unsigned int HID1_PC4_OFFSET = 0;
	static const unsigned int HID1_PC5_OFFSET = 0;
	static const unsigned int HID1_EMCP_OFFSET = 0;
	static const unsigned int HID1_EBA_OFFSET = 0;
	static const unsigned int HID1_EBD_OFFSET = 0;
	static const unsigned int HID1_BCLK_OFFSET = 0;
	static const unsigned int HID1_ECLK_OFFSET = 0;
	static const unsigned int HID1_PAR_OFFSET = 0;
	static const unsigned int HID1_DFS4_OFFSET = 0;
	static const unsigned int HID1_DFS2_OFFSET = 0;
	static const unsigned int HID1_SYNCBE_OFFSET = 0;
	static const unsigned int HID1_ABE_OFFSET = 0;

	static const uint32_t HID1_PC0_BITSIZE = 0;
	static const uint32_t HID1_PC1_BITSIZE = 0;
	static const uint32_t HID1_PC2_BITSIZE = 0;
	static const uint32_t HID1_PC3_BITSIZE = 0;
	static const uint32_t HID1_PC4_BITSIZE = 0;
	static const uint32_t HID1_PC5_BITSIZE = 0;
	static const uint32_t HID1_EMCP_BITSIZE = 0;
	static const uint32_t HID1_EBA_BITSIZE = 0;
	static const uint32_t HID1_EBD_BITSIZE = 0;
	static const uint32_t HID1_BCLK_BITSIZE = 0;
	static const uint32_t HID1_ECLK_BITSIZE = 0;
	static const uint32_t HID1_PAR_BITSIZE = 0;
	static const uint32_t HID1_DFS4_BITSIZE = 0;
	static const uint32_t HID1_DFS2_BITSIZE = 0;
	static const uint32_t HID1_SYNCBE_BITSIZE = 0;
	static const uint32_t HID1_ABE_BITSIZE = 0;
};

class HID2Layout
{
public:
	static const unsigned int HID2_L2AP_EN_OFFSET = 0;
	static const unsigned int HID2_SWT_EN_OFFSET = 0;
	static const unsigned int HID2_HIGH_BAT_EN_OFFSET = 0;
	static const unsigned int HID2_IWLCK_OFFSET = 0;
	static const unsigned int HID2_DWLCK_OFFSET = 0;

	static const uint32_t HID2_L2AP_EN_BITSIZE = 0;
	static const uint32_t HID2_SWT_EN_BITSIZE = 0;
	static const uint32_t HID2_HIGH_BAT_EN_BITSIZE = 0;
	static const uint32_t HID2_IWLCK_BITSIZE = 0;
	static const uint32_t HID2_DWLCK_BITSIZE = 0;
};

class ICTRLLayout
{
public:
	static const unsigned int ICTRL_CIRQ_OFFSET = 31;
	static const unsigned int ICTRL_EIEC_OFFSET = 27;
	static const unsigned int ICTRL_EDCE_OFFSET = 26;
	static const unsigned int ICTRL_EICP_OFFSET = 8;
	static const unsigned int ICTRL_ICWL_OFFSET = 0;

	static const uint32_t ICTRL_CIRQ_BITSIZE = 1;
	static const uint32_t ICTRL_EIEC_BITSIZE = 1;
	static const uint32_t ICTRL_EDCE_BITSIZE = 1;
	static const uint32_t ICTRL_EICP_BITSIZE = 1;
	static const uint32_t ICTRL_ICWL_BITSIZE = 8;
};

class IABRLayout
{
public:
	static const unsigned int IABR_ADDR_OFFSET = 2;
	static const unsigned int IABR_BE_OFFSET = 1;
	static const unsigned int IABR_TE_OFFSET = 0;

	static const unsigned int IABR_ADDR_BITSIZE = 30;
	static const unsigned int IABR_BE_BITSIZE = 1;
	static const unsigned int IABR_TE_BITSIZE = 1;

	static const uint32_t IABR_ADDR_MASK = ((1UL << IABR_ADDR_BITSIZE) - 1) << IABR_ADDR_OFFSET;
	static const uint32_t IABR_BE_MASK = ((1UL << IABR_BE_BITSIZE) - 1) << IABR_BE_OFFSET;
	static const uint32_t IABR_TE_MASK = ((1UL << IABR_TE_BITSIZE) - 1) << IABR_TE_OFFSET;
	
	static const uint32_t IABR_MASK = IABR_ADDR_MASK | IABR_BE_MASK | IABR_TE_MASK;

};

class BaseConfig : public CRLayout, public XERLayout, public FPSCRLayout
{
public:
	typedef enum { MAT_READ = 0, MAT_WRITE = 1 } MemoryAccessType;
	
	typedef enum { MT_DATA = 0, MT_INSN = 1 } MemoryType;
	
	typedef enum { PR_USER = 0, PR_SUPERVISOR = 1 } PrivilegeLevel;
	
	typedef enum {
		// G1 processors
		MPC601 = 0,
		// G2 processors
		MPC603E,
		MPC604E,
		// G3 processors
		MPC740,
		MPC745,
		MPC750,
		MPC755,
		// G4 processors
		MPC7400,
		MPC7410,
		MPC7441,
		MPC7445,
		MPC7447,
		MPC7447A,
		MPC7448,
		MPC7450,
		MPC7451,
		MPC7455,
		MPC7457,
		NUM_MODELS
	} Model;

	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint64_t virtual_address_t;     // only 52 bits are used, all remaining bits *must* be set to zero
	typedef uint32_t physical_address_t;    // 32-bit physical address
	typedef uint32_t reg_t;                 // register type
	typedef enum { WIMG_DEFAULT = 0, WIMG_GUARDED_MEMORY = 1, WIMG_MEMORY_COHERENCY_ENFORCED = 2, WIMG_CACHE_INHIBITED = 4, WIMG_WRITE_THROUGH = 8 } WIMG;
	static const uint32_t MEMORY_PAGE_SIZE = 4096; // DO NOT MODIFY THIS VALUE !!!!!

	/* A Memory Page Table Entry */
	class PTE
	{
	public:
		physical_address_t base_physical_addr;  /*< 32-bit base physical address */
		uint32_t c;                             /*< 1-bit C field */
		WIMG wimg;                              /*< 4-bit WIMG field */
		uint32_t pp;                            /*< 2-bit page protection field */
	};

	// Debug stuff
	static const bool DEBUG_ENABLE = false; // Debug is disabled by default
	static const bool DEBUG_SETUP_ENABLE = false;
	static const bool DEBUG_STEP_ENABLE = false;
	static const bool DEBUG_DTLB_ENABLE = false;
	static const bool DEBUG_DL1_ENABLE = false;
	static const bool DEBUG_IL1_ENABLE = false;
	static const bool DEBUG_L2_ENABLE = false;
	static const bool DEBUG_LOAD_ENABLE = false;
	static const bool DEBUG_STORE_ENABLE = false;
	static const bool DEBUG_READ_MEMORY_ENABLE = false;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = false;
	static const bool DEBUG_EXCEPTION_ENABLE = false;
	static const bool DEBUG_SET_MSR_ENABLE = false;
	static const bool DEBUG_SET_HID0_ENABLE = false;
	static const bool DEBUG_SET_HID1_ENABLE = false;
	static const bool DEBUG_SET_HID2_ENABLE = false;
	static const bool DEBUG_SET_L2CR_ENABLE = false;

	// performance model
	static const bool PERF_MODEL_ENABLE = false;

	// start address
	static const uint32_t START_ADDR = 0xfff00100UL; // processor starts at system reset interrupt handler

	// Programming model parameters
	static const unsigned int NUM_VRS = 0;
	static const unsigned int NUM_SPRGS = 0;

	// Simulation performance speed-up features
	static const bool PREFETCH_BUFFER_ENABLE = true; // enable faster fetch
	static const unsigned int NUM_PREFETCH_BUFFER_ENTRIES = 8; //!< Maximum number of instruction in the prefetch buffer
	static const bool IABR_ENABLE = false;
	static const bool DABR_ENABLE = false;

	// MMU parameters
	static const unsigned int NUM_BATS = 0;
	static const bool HAS_SOFTWARE_TABLE_SEARCH_SUPPORT = false;
	static const bool HAS_EXTENDED_ADDRESSING_SUPPORT = false;
	
	// Altivec
	static const bool HAS_ALTIVEC = false;
	
	// UISA
	static const bool HAS_MQ = false;
	static const bool HAS_RTCL = false;
	static const bool HAS_RTCU = false;
	static const bool HAS_VRSAVE = false;
	static const bool HAS_VSCR = false;

	// VEA
	static const bool HAS_TBL = false;
	static const bool HAS_TBU = false;

	// OEA
	static const bool HAS_BAMR = false;
	static const bool HAS_DABR = false;
	static const bool HAS_DAR = false;
	static const bool HAS_DEC = false;
	static const bool HAS_DSISR = false;
	static const bool HAS_EAR = false;
	static const bool HAS_HASH1 = false;
	static const bool HAS_HASH2 = false;
	static const bool HAS_HID0 = false;
	static const bool HAS_HID1 = false;
	static const bool HAS_HID2 = false;
	static const bool HAS_IABR = false;
	static const bool HAS_LDSTDB = false; // undocumented !!!
	static const bool HAS_ICMP = false;
	static const bool HAS_DCMP = false;
	static const bool HAS_DMISS = false;
	static const bool HAS_ICTC = false;
	static const bool HAS_ICTRL = false;
	static const bool HAS_IMISS = false;
	static const bool HAS_L2CR = false;
	static const bool HAS_L2ERRINJHI = false;
	static const bool HAS_L2ERRINJLO = false;
	static const bool HAS_L2ERRINJCTL = false;
	static const bool HAS_L2CAPTDATAHI = false;
	static const bool HAS_L2CAPTDATALO = false;
	static const bool HAS_L2CAPTDATAECC = false;
	static const bool HAS_L2ERRDET = false;
	static const bool HAS_L2ERRDIS = false;
	static const bool HAS_L2ERRINTEN = false;
	static const bool HAS_L2ERRATTR = false;
	static const bool HAS_L2ERRADDR = false;
	static const bool HAS_L2ERREADDR = false;
	static const bool HAS_L2ERRCTL = false;
	static const bool HAS_L2PM = false;
	static const bool HAS_L3CR = false;
	static const bool HAS_L3ITCR0 = false;
	static const bool HAS_L3ITCR1 = false;
	static const bool HAS_L3ITCR2 = false;
	static const bool HAS_L3ITCR3 = false;
	static const bool HAS_L3OHCR = false;
	static const bool HAS_L3PM = false;
	static const bool HAS_LDSTCR = false;
	static const bool HAS_MMCR0 = false;
	static const bool HAS_MMCR1 = false;
	static const bool HAS_MMCR2 = false;
	static const bool HAS_MSSCR0 = false;
	static const bool HAS_MSSSR0 = false;
	static const bool HAS_PIR = false;
	static const bool HAS_PMC1 = false;
	static const bool HAS_PMC2 = false;
	static const bool HAS_PMC3 = false;
	static const bool HAS_PMC4 = false;
	static const bool HAS_PMC5 = false;
	static const bool HAS_PMC6 = false;
	static const bool HAS_PTEHI = false;
	static const bool HAS_PTELO = false;
	static const bool HAS_RPA = false;
	static const bool HAS_SDA = false;
	static const bool HAS_SIA = false;
	static const bool HAS_SVR = false;
	static const bool HAS_SDR1 = false;
	static const bool HAS_SRR0 = false;
	static const bool HAS_SRR1 = false;
	static const bool HAS_THRM1 = false;
	static const bool HAS_THRM2 = false;
	static const bool HAS_THRM3 = false;
	static const bool HAS_TLBMISS = false;

	static const bool HAS_DECREMENTER_OVERFLOW = false;
	static const bool HAS_EXTERNAL_INTERRUPT = false;
	static const bool HAS_SOFT_RESET = false;
	static const bool HAS_HARD_RESET = false;
	static const bool HAS_MCP = false;
	static const bool HAS_TEA = false;
	static const bool HAS_SMI = false;
	static const bool HAS_THERMAL_MANAGEMENT_INTERRUPT = false;
	static const bool HAS_PERFORMANCE_MONITOR_INTERRUPT = false;

	static const bool HAS_MSR_VEC = false;
	static const bool HAS_MSR_POW = false;
	static const bool HAS_MSR_ILE = false;
	static const bool HAS_MSR_EE = false;
	static const bool HAS_MSR_PR = false;
	static const bool HAS_MSR_FP = false;
	static const bool HAS_MSR_ME = false;
	static const bool HAS_MSR_FE0 = false;
	static const bool HAS_MSR_SE = false;
	static const bool HAS_MSR_BE = false;
	static const bool HAS_MSR_FE1 = false;
	static const bool HAS_MSR_IP = false;
	static const bool HAS_MSR_IR = false;
	static const bool HAS_MSR_DR = false;
	static const bool HAS_MSR_PM = false;
	static const bool HAS_MSR_RI = false;
	static const bool HAS_MSR_LE = false;
	
	static const bool HAS_HID0_EMCP = false;
	static const bool HAS_HID0_DBP = false;
	static const bool HAS_HID0_EBA = false;
	static const bool HAS_HID0_EBD = false;
	static const bool HAS_HID0_SBCLK = false;
	static const bool HAS_HID0_BCLK = false;
	static const bool HAS_HID0_TBEN = false;
	static const bool HAS_HID0_EICE = false;
	static const bool HAS_HID0_ECLK = false;
	static const bool HAS_HID0_PAR = false;
	static const bool HAS_HID0_STEN = false;
	static const bool HAS_HID0_DOZE = false;
	static const bool HAS_HID0_HIGH_BAT_EN = false;
	static const bool HAS_HID0_NAP = false;
	static const bool HAS_HID0_SLEEP = false;
	static const bool HAS_HID0_DPM = false;
	static const bool HAS_HID0_RISEG = false;
	static const bool HAS_HID0_BHTCLR = false;
	static const bool HAS_HID0_EIEC = false;
	static const bool HAS_HID0_XAEN = false;
	static const bool HAS_HID0_NHR = false;
	static const bool HAS_HID0_ICE = false;
	static const bool HAS_HID0_DCE = false;
	static const bool HAS_HID0_ILOCK = false;
	static const bool HAS_HID0_DLOCK = false;
	static const bool HAS_HID0_ICFI = false;
	static const bool HAS_HID0_DCFI = false;
	static const bool HAS_HID0_SPD = false;
	static const bool HAS_HID0_IFEM = false;
	static const bool HAS_HID0_IFTT = false;
	static const bool HAS_HID0_XBSEN = false;
	static const bool HAS_HID0_SIED = false;
	static const bool HAS_HID0_SGE = false;
	static const bool HAS_HID0_DCFA = false;
	static const bool HAS_HID0_BTIC = false;
	static const bool HAS_HID0_LRSTK = false;
	static const bool HAS_HID0_ABE = false;
	static const bool HAS_HID0_FOLD = false;
	static const bool HAS_HID0_BHT = false;
	static const bool HAS_HID0_NOPDST = false;
	static const bool HAS_HID0_NOPTI = false;

	static const bool HAS_HID1_PC0 = false;
	static const bool HAS_HID1_PC1 = false;
	static const bool HAS_HID1_PC2 = false;
	static const bool HAS_HID1_PC3 = false;
	static const bool HAS_HID1_PC4 = false;
	static const bool HAS_HID1_PC5 = false;
	static const bool HAS_HID1_EMCP = false;
	static const bool HAS_HID1_EBA = false;
	static const bool HAS_HID1_EBD = false;
	static const bool HAS_HID1_BCLK = false;
	static const bool HAS_HID1_ECLK = false;
	static const bool HAS_HID1_PAR = false;
	static const bool HAS_HID1_DFS4 = false;
	static const bool HAS_HID1_DFS2 = false;
	static const bool HAS_HID1_SYNCBE = false;
	static const bool HAS_HID1_ABE = false;

	static const bool HAS_HID2_L2AP_EN = false;
	static const bool HAS_HID2_SWT_EN = false;
	static const bool HAS_HID2_HIGH_BAT_EN = false;
	static const bool HAS_HID2_IWLCK = false;
	static const bool HAS_HID2_DWLCK = false;
	
	static const bool HAS_L2CR_L2E = false;
	static const bool HAS_L2CR_L2PE = false;
	static const bool HAS_L2CR_L2SIZ = false;
	static const bool HAS_L2CR_L2CLK = false;
	static const bool HAS_L2CR_L2RAM = false;
	static const bool HAS_L2CR_L2DR = false;
	static const bool HAS_L2CR_L2DO = false;
	static const bool HAS_L2CR_L2I = false;
	static const bool HAS_L2CR_L2CTL = false;
	static const bool HAS_L2CR_L2WT = false;
	static const bool HAS_L2CR_L2TS = false;
	static const bool HAS_L2CR_L2OH = false;
	static const bool HAS_L2CR_L2SL = false;
	static const bool HAS_L2CR_L2DF = false;
	static const bool HAS_L2CR_L2BYP = false;
	static const bool HAS_L2CR_L2FA = false;
	static const bool HAS_L2CR_L2REP = false;
	static const bool HAS_L2CR_L2HWF = false;
	static const bool HAS_L2CR_L2IO = false;
	static const bool HAS_L2CR_L2CLKSTP = false;
	static const bool HAS_L2CR_L2DRO = false;
	static const bool HAS_L2CR_L2IP = false;
	static const bool HAS_L2CR_LVRAME = false;
	static const bool HAS_L2CR_LVRAMM = false;

	static const bool HAS_ICTRL_CIRQ = false;
	static const bool HAS_ICTRL_EIEC = false;
	static const bool HAS_ICTRL_EDCE = false;
	static const bool HAS_ICTRL_EICP = false;
	static const bool HAS_ICTRL_ICWL = false;

	typedef enum operand_type_t
	{
		GPR_T,    // General Purpose Register
		GPR_ST_T, // General Purpose Register to be stored in memory
		FPR_T,    // Floating Point Register
		FPR_ST_T, // Floating Point Register to be stored in memory
		CR_T,     // Condition Register
		FPSCR_T,  // Floating Point Status and Control Register
		XER_T,    // XER Register
		LR_T,     // Link Register
		CTR_T,    // Count Register
		SPR_T     // Special Purpose Register
	} operand_type_t;

	typedef enum operand_direction_t
	{
		INPUT_T, // Input Operand
		OUTPUT_T // Output Operand
	} operand_direction_t;

	typedef enum execution_unit_type_t
	{
		NO_UNIT_T, // No Unit
		LSU_T,     // Load/Store Unit
		IU1_T,     // Simple Integer Unit
		IU2_T,     // Complex Integer Unit
		FPU_T,     // Floating Point Unit
		BPU_T,     // Branch Processing Unit
		VFPU_T,    // Vector Floating Point Unit
		VPU_T,     // vector Processing Unit
		VIU1_T,    // Simple Vector Integer
		VIU2_T     // Complex Vector Integer
	} execution_unit_type_t;

	typedef enum serialization_t
	{
		NO_SERIALIZATION = 0,
		EXECUTION_SERIALIZATION = 1,
		REFETCH_SERIALIZATION = 2,
		STORE_SERIALIZATION = 4
	} serialization_t;

	typedef struct operand_t
	{
		operand_type_t type;
		operand_direction_t dir;
		unsigned int reg_num; // when type is GPR_T or FPR_T
	} operand_t;

	static const unsigned int MAX_OPERANDS = 8;

	typedef operand_t operands_t[MAX_OPERANDS];
};

//=====================================================================
//=                       MPC7XX configurations                       =
//=====================================================================

class MPC7XXHID0Layout : public HID0Layout
{
public:
	// HID0 bits offsets
	static const unsigned int HID0_EMCP_OFFSET = 31;
	static const unsigned int HID0_DBP_OFFSET = 30;
	static const unsigned int HID0_EBA_OFFSET = 29;
	static const unsigned int HID0_EBD_OFFSET = 28;
	static const unsigned int HID0_BCLK_OFFSET = 27;
	static const unsigned int HID0_ECLK_OFFSET = 25;
	static const unsigned int HID0_PAR_OFFSET = 24;
	static const unsigned int HID0_DOZE_OFFSET = 23;
	static const unsigned int HID0_NAP_OFFSET = 22;
	static const unsigned int HID0_SLEEP_OFFSET = 21;
	static const unsigned int HID0_DPM_OFFSET = 20;
	static const unsigned int HID0_NHR_OFFSET = 16;
	static const unsigned int HID0_ICE_OFFSET = 15;
	static const unsigned int HID0_DCE_OFFSET = 14;
	static const unsigned int HID0_ILOCK_OFFSET = 13;
	static const unsigned int HID0_DLOCK_OFFSET = 12;
	static const unsigned int HID0_ICFI_OFFSET = 11;
	static const unsigned int HID0_DCFI_OFFSET = 10;
	static const unsigned int HID0_SPD_OFFSET = 9;
	static const unsigned int HID0_IFEM_OFFSET = 8;
	static const unsigned int HID0_SGE_OFFSET = 7;
	static const unsigned int HID0_DCFA_OFFSET = 6;
	static const unsigned int HID0_BTIC_OFFSET = 5;
	static const unsigned int HID0_ABE_OFFSET = 3;
	static const unsigned int HID0_BHT_OFFSET = 2;
	
	// HID0 fields sizes
	static const unsigned int HID0_EMCP_BITSIZE = 1;
	static const unsigned int HID0_DBP_BITSIZE = 1;
	static const unsigned int HID0_EBA_BITSIZE = 1;
	static const unsigned int HID0_EBD_BITSIZE = 1;
	static const unsigned int HID0_BCLK_BITSIZE = 1;
	static const unsigned int HID0_ECLK_BITSIZE = 1;
	static const unsigned int HID0_PAR_BITSIZE = 1;
	static const unsigned int HID0_DOZE_BITSIZE = 1;
	static const unsigned int HID0_NAP_BITSIZE = 1;
	static const unsigned int HID0_SLEEP_BITSIZE = 1;
	static const unsigned int HID0_DPM_BITSIZE = 1;
	static const unsigned int HID0_NHR_BITSIZE = 1;
	static const unsigned int HID0_ICE_BITSIZE = 1;
	static const unsigned int HID0_DCE_BITSIZE = 1;
	static const unsigned int HID0_ILOCK_BITSIZE = 1;
	static const unsigned int HID0_DLOCK_BITSIZE = 1;
	static const unsigned int HID0_ICFI_BITSIZE = 1;
	static const unsigned int HID0_DCFI_BITSIZE = 1;
	static const unsigned int HID0_SPD_BITSIZE = 1;
	static const unsigned int HID0_IFEM_BITSIZE = 1;
	static const unsigned int HID0_SGE_BITSIZE = 1;
	static const unsigned int HID0_DCFA_BITSIZE = 1;
	static const unsigned int HID0_BTIC_BITSIZE = 1;
	static const unsigned int HID0_ABE_BITSIZE = 1;
	static const unsigned int HID0_BHT_BITSIZE = 1;
};

class MPC7XXHID1Layout : public HID1Layout
{
public:
	static const unsigned int HID1_PC0_OFFSET = 31;
	static const unsigned int HID1_PC1_OFFSET = 30;
	static const unsigned int HID1_PC2_OFFSET = 29;
	static const unsigned int HID1_PC3_OFFSET = 28;

	static const uint32_t HID1_PC0_BITSIZE = 1;
	static const uint32_t HID1_PC1_BITSIZE = 1;
	static const uint32_t HID1_PC2_BITSIZE = 1;
	static const uint32_t HID1_PC3_BITSIZE = 1;
};

class MPC7XXHID2Layout : public HID2Layout
{
public:
	static const unsigned int HID2_L2AP_EN_OFFSET = 20;
	static const unsigned int HID2_SWT_EN_OFFSET = 19;
	static const unsigned int HID2_HIGH_BAT_EN_OFFSET = 18;
	static const unsigned int HID2_IWLCK_OFFSET = 13;
	static const unsigned int HID2_DWLCK_OFFSET = 5;

	static const uint32_t HID2_L2AP_EN_BITSIZE = 1;
	static const uint32_t HID2_SWT_EN_BITSIZE = 1;
	static const uint32_t HID2_HIGH_BAT_EN_BITSIZE = 1;
	static const uint32_t HID2_IWLCK_BITSIZE = 1;
	static const uint32_t HID2_DWLCK_BITSIZE = 1;
};

class MPC7XXL2CRLayout : public L2CRLayout
{
public:
	static const unsigned int L2CR_L2E_OFFSET = 31;
	static const unsigned int L2CR_L2PE_OFFSET = 30;
	static const unsigned int L2CR_L2SIZ_OFFSET = 28;
	static const unsigned int L2CR_L2CLK_OFFSET = 25;
	static const unsigned int L2CR_L2RAM_OFFSET = 23;
	static const unsigned int L2CR_L2DR_OFFSET = 22;
	static const unsigned int L2CR_L2I_OFFSET = 21;
	static const unsigned int L2CR_L2CTL_OFFSET = 20;
	static const unsigned int L2CR_L2WT_OFFSET = 19;
	static const unsigned int L2CR_L2TS_OFFSET = 18;
	static const unsigned int L2CR_L2OH_OFFSET = 16;
	static const unsigned int L2CR_L2SL_OFFSET = 15;
	static const unsigned int L2CR_L2DF_OFFSET = 14;
	static const unsigned int L2CR_L2BYP_OFFSET = 13;
	static const unsigned int L2CR_L2IP_OFFSET = 0;
	
	static const uint32_t L2CR_L2E_BITSIZE = 1;
	static const uint32_t L2CR_L2PE_BITSIZE = 1;
	static const uint32_t L2CR_L2SIZ_BITSIZE = 2;
	static const uint32_t L2CR_L2CLK_BITSIZE = 2;
	static const uint32_t L2CR_L2RAM_BITSIZE = 2;
	static const uint32_t L2CR_L2DR_BITSIZE = 1;
	static const uint32_t L2CR_L2I_BITSIZE = 1;
	static const uint32_t L2CR_L2CTL_BITSIZE = 1;
	static const uint32_t L2CR_L2WT_BITSIZE = 1;
	static const uint32_t L2CR_L2TS_BITSIZE = 1;
	static const uint32_t L2CR_L2OH_BITSIZE = 2;
	static const uint32_t L2CR_L2SL_BITSIZE = 1;
	static const uint32_t L2CR_L2DF_BITSIZE = 1;
	static const uint32_t L2CR_L2BYP_BITSIZE = 1;
	static const uint32_t L2CR_L2IP_BITSIZE = 1;
};

class MPC7XXConfig :
	public BaseConfig,
	public MSRLayout,
	public MPC7XXHID0Layout,
	public MPC7XXHID1Layout,
	public MPC7XXHID2Layout,
	public MPC7XXL2CRLayout,
	public ICTRLLayout,
	public IABRLayout
{
public:
	// MSR reset value
	static const uint32_t MSR_RESET_VALUE = 0x40; // only MSR[IP] is set
	
	// HID0 value at reset
	static const uint32_t HID0_RESET_VALUE = 0;

	// Programming model parameters
	static const unsigned int NUM_SPRGS = 4;
	
	// MMU parameters
	static const unsigned int NUM_BATS = 4;
	
	// VEA
	static const bool HAS_TBL = true;
	static const bool HAS_TBU = true;

	// OEA
	static const bool HAS_DABR = true;
	static const bool HAS_DAR = true;
	static const bool HAS_DEC = true;
	static const bool HAS_DSISR = true;
	static const bool HAS_EAR = true;
	static const bool HAS_HID0 = true;
	static const bool HAS_HID1 = true;
	static const bool HAS_IABR = true;
	static const bool HAS_ICTC = true;
	static const bool HAS_L2CR = true;
	static const bool HAS_MMCR0 = true;
	static const bool HAS_MMCR1 = true;
	static const bool HAS_PMC1 = true;
	static const bool HAS_PMC2 = true;
	static const bool HAS_PMC3 = true;
	static const bool HAS_PMC4 = true;
	static const bool HAS_SDA = true;
	static const bool HAS_SIA = true;
	static const bool HAS_SDR1 = true;
	static const bool HAS_SRR0 = true;
	static const bool HAS_SRR1 = true;
	static const bool HAS_THRM1 = true;
	static const bool HAS_THRM2 = true;
	static const bool HAS_THRM3 = true;

	static const bool HAS_DECREMENTER_OVERFLOW = true;
	static const bool HAS_EXTERNAL_INTERRUPT = true;
	static const bool HAS_SOFT_RESET = true;
	static const bool HAS_HARD_RESET = true;
	static const bool HAS_L1_INSN_CACHE_PARITY_ERROR = true;
	static const bool HAS_L1_DATA_CACHE_PARITY_ERROR = true;
	static const bool HAS_L2_CACHE_TAG_PARITY_ERROR = true;
 	static const bool HAS_L2_CACHE_DATA_PARITY_ERROR = true;
	static const bool HAS_MCP = true;
	static const bool HAS_TEA = true;
	static const bool HAS_DATA_BUS_PARITY_ERROR = true;
	static const bool HAS_ADDRESS_BUS_PARITY_ERROR = true;
	static const bool HAS_SMI = true;
	static const bool HAS_THERMAL_MANAGEMENT_INTERRUPT = true;
	static const bool HAS_PERFORMANCE_MONITOR_INTERRUPT = true;

	static const bool HAS_MSR_POW = true;
	static const bool HAS_MSR_ILE = true;
	static const bool HAS_MSR_EE = true;
	static const bool HAS_MSR_PR = true;
	static const bool HAS_MSR_FP = true;
	static const bool HAS_MSR_ME = true;
	static const bool HAS_MSR_FE0 = true;
	static const bool HAS_MSR_SE = true;
	static const bool HAS_MSR_BE = true;
	static const bool HAS_MSR_FE1 = true;
	static const bool HAS_MSR_IP = true;
	static const bool HAS_MSR_IR = true;
	static const bool HAS_MSR_DR = true;
	static const bool HAS_MSR_PM = true;
	static const bool HAS_MSR_RI = true;
	static const bool HAS_MSR_LE = true;

	static const bool HAS_HID0_EMCP = true;
	static const bool HAS_HID0_DBP = true;
	static const bool HAS_HID0_EBA = true;
	static const bool HAS_HID0_EBD = true;
	static const bool HAS_HID0_BCLK = true;
	static const bool HAS_HID0_ECLK = true;
	static const bool HAS_HID0_PAR = true;
	static const bool HAS_HID0_DOZE = true;
	static const bool HAS_HID0_NAP = true;
	static const bool HAS_HID0_SLEEP = true;
	static const bool HAS_HID0_DPM = true;
	static const bool HAS_HID0_NHR = true;
	static const bool HAS_HID0_ICE = true;
	static const bool HAS_HID0_DCE = true;
	static const bool HAS_HID0_ILOCK = true;
	static const bool HAS_HID0_DLOCK = true;
	static const bool HAS_HID0_ICFI = true;
	static const bool HAS_HID0_DCFI = true;
	static const bool HAS_HID0_SPD = true;
	static const bool HAS_HID0_SGE = true;
	static const bool HAS_HID0_DCFA = true;
	static const bool HAS_HID0_BTIC = true;
	static const bool HAS_HID0_ABE = true;
	static const bool HAS_HID0_BHT = true;

	static const bool HAS_HID1_PC0 = true;
	static const bool HAS_HID1_PC1 = true;
	static const bool HAS_HID1_PC2 = true;
	static const bool HAS_HID1_PC3 = true;

	static const bool HAS_L2CR_L2E = true;
	static const bool HAS_L2CR_L2PE = true;
	static const bool HAS_L2CR_L2SIZ = true;
	static const bool HAS_L2CR_L2CLK = true;
	static const bool HAS_L2CR_L2RAM = true;
	static const bool HAS_L2CR_L2DR = true;
	static const bool HAS_L2CR_L2I = true;
	static const bool HAS_L2CR_L2CTL = true;
	static const bool HAS_L2CR_L2WT = true;
	static const bool HAS_L2CR_L2TS = true;
	static const bool HAS_L2CR_L2OH = true;
	static const bool HAS_L2CR_L2SL = true;
	static const bool HAS_L2CR_L2DF = true;
	static const bool HAS_L2CR_L2BYP = true;
	static const bool HAS_L2CR_L2IP = true;
};

// class MPC740Config
// {
// 	public:
// 	static const Model MODEL = MPC740;
// 		
// 	static const uint32_t PROCESSOR_VERSION = 0x00088200UL;
// 	
// 	// L1 Data cache parameters
// 	static const bool L1_DATA_ENABLE = true;
// 	static const uint32_t L1_DATA_CACHE_SIZE = 32768;
// 	static const uint32_t L1_DATA_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L1_DATA_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L1_DATA_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 
// 	// L1 Instruction cache parameters
// 	static const bool L1_INSN_ENABLE = true;
// 	static const uint32_t L1_INSN_CACHE_SIZE = 32768;
// 	static const uint32_t L1_INSN_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L1_INSN_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L1_INSN_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 
// 	// L2 cache parameters
// 	static const bool L2_ENABLE = false;
// 	static const uint32_t L2_CACHE_SIZE = 32; // 0 KB
// 	static const uint32_t L2_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L2_CACHE_ASSOCIATIVITY = 1;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L2_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 	
// 	// Front side bus parameters
// 	static const uint32_t FSB_BURST_SIZE = 32;
// 
// 	// DTLB parameters
// 	static const bool DTLB_ENABLE = true;
// 	static const uint32_t NUM_DTLB_ENTRIES = 128; // PowerPC 750 and 755 have a 128-entries DTLB
// 	static const uint32_t DTLB_ASSOCIATIVITY = 2; // PowerPC 750 and 755 have a 2-way set associative DTLB
// 	static const unisim::component::cxx::tlb::ReplacementPolicy DTLB_REPLACEMENT_POLICY = unisim::component::cxx::tlb::RP_LRU;
// 	
// 	// ITLB parameters
// 	static const bool ITLB_ENABLE = true;
// 	static const uint32_t NUM_ITLB_ENTRIES = 128; // PowerPC 750 and 755 have a 128-entries ITLB
// 	static const uint32_t ITLB_ASSOCIATIVITY = 2; // PowerPC 750 and 755 have a 2-way set associative ITLB
// 	static const unisim::component::cxx::tlb::ReplacementPolicy ITLB_REPLACEMENT_POLICY = unisim::component::cxx::tlb::RP_LRU;
// 	
// 	// Programming model parameters
// 	static const unsigned int NUM_SPRGS = 4;
// 	
// 	// MMU parameters
// 	static const unsigned int MMU_NUM_BATS = 4;
// 	static const bool MMU_HAS_SOFTWARE_TABLE_SEARCH_SUPPORT = false;
// 	static const bool MMU_HAS_EXTENDED_ADDRESSING_SUPPORT = false;
// };
// 
// class MPC745Config
// {
// 	public:
// 	static const Model MODEL = MPC745;
// 	
// 	static const uint32_t PROCESSOR_VERSION = 0x00083100UL;
// 	
// 	// L1 Data cache parameters
// 	static const bool L1_DATA_ENABLE = true;
// 	static const uint32_t L1_DATA_CACHE_SIZE = 32768;
// 	static const uint32_t L1_DATA_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L1_DATA_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L1_DATA_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 
// 	// L1 Instruction cache parameters
// 	static const bool L1_INSN_ENABLE = true;
// 	static const uint32_t L1_INSN_CACHE_SIZE = 32768;
// 	static const uint32_t L1_INSN_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L1_INSN_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L1_INSN_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 
// 	// L2 cache parameters
// 	static const bool L2_ENABLE = false;
// 	static const uint32_t L2_CACHE_SIZE = 32; // 0 KB
// 	static const uint32_t L2_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L2_CACHE_ASSOCIATIVITY = 1;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L2_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 	
// 	// Front side bus parameters
// 	static const uint32_t FSB_BURST_SIZE = 32;
// 
// 	// DTLB parameters
// 	static const bool DTLB_ENABLE = true;
// 	static const uint32_t NUM_DTLB_ENTRIES = 128; // PowerPC 750 and 755 have a 128-entries DTLB
// 	static const uint32_t DTLB_ASSOCIATIVITY = 2; // PowerPC 750 and 755 have a 2-way set associative DTLB
// 	static const unisim::component::cxx::tlb::ReplacementPolicy DTLB_REPLACEMENT_POLICY = unisim::component::cxx::tlb::RP_LRU;
// 	
// 	// ITLB parameters
// 	static const bool ITLB_ENABLE = true;
// 	static const uint32_t NUM_ITLB_ENTRIES = 128; // PowerPC 750 and 755 have a 128-entries ITLB
// 	static const uint32_t ITLB_ASSOCIATIVITY = 2; // PowerPC 750 and 755 have a 2-way set associative ITLB
// 	static const unisim::component::cxx::tlb::ReplacementPolicy ITLB_REPLACEMENT_POLICY = unisim::component::cxx::tlb::RP_LRU;
// 	
// 	// Programming model parameters
// 	static const unsigned int NUM_SPRGS = 8;
// 	
// 	// MMU parameters
// 	static const unsigned int MMU_NUM_BATS = 8;
// 	static const bool MMU_HAS_SOFTWARE_TABLE_SEARCH_SUPPORT = true;
// 	static const bool MMU_HAS_EXTENDED_ADDRESSING_SUPPORT = false;
// };
// 
// class MPC750Config
// {
// 	public:
// 	static const Model MODEL = MPC750;
// 	
// 	static const uint32_t PROCESSOR_VERSION = 0x00088300UL;
// 
// 	// L1 Data cache parameters
// 	static const bool L1_DATA_ENABLE = true;
// 	static const uint32_t L1_DATA_CACHE_SIZE = 32768;
// 	static const uint32_t L1_DATA_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L1_DATA_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L1_DATA_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 
// 	// L1 Instruction cache parameters
// 	static const bool L1_INSN_ENABLE = true;
// 	static const uint32_t L1_INSN_CACHE_SIZE = 32768;
// 	static const uint32_t L1_INSN_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L1_INSN_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L1_INSN_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 
// 	// L2 cache parameters
// 	static const bool L2_ENABLE = true;
// 	static const uint32_t L2_CACHE_SIZE = 512 * 1024; // 512 KB
// 	static const uint32_t L2_CACHE_BLOCK_SIZE = 32;
// 	static const uint32_t L2_CACHE_ASSOCIATIVITY = 8;
// 	static const unisim::component::cxx::cache::ReplacementPolicy L2_CACHE_REPLACEMENT_POLICY = unisim::component::cxx::cache::RP_LRU;
// 	
// 	// Front side bus parameters
// 	static const uint32_t FSB_BURST_SIZE = 32;
// 
// 	// DTLB parameters
// 	static const bool DTLB_ENABLE = true;
// 	static const uint32_t NUM_DTLB_ENTRIES = 128; // PowerPC 750 and 755 have a 128-entries DTLB
// 	static const uint32_t DTLB_ASSOCIATIVITY = 2; // PowerPC 750 and 755 have a 2-way set associative DTLB
// 	static const unisim::component::cxx::tlb::ReplacementPolicy DTLB_REPLACEMENT_POLICY = unisim::component::cxx::tlb::RP_LRU;
// 	
// 	// ITLB parameters
// 	static const bool ITLB_ENABLE = true;
// 	static const uint32_t NUM_ITLB_ENTRIES = 128; // PowerPC 750 and 755 have a 128-entries ITLB
// 	static const uint32_t ITLB_ASSOCIATIVITY = 2; // PowerPC 750 and 755 have a 2-way set associative ITLB
// 	static const unisim::component::cxx::tlb::ReplacementPolicy ITLB_REPLACEMENT_POLICY = unisim::component::cxx::tlb::RP_LRU;
// 	
// 	// Programming model parameters
// 	static const unsigned int NUM_SPRGS = 4;
// 	
// 	// MMU parameters
// 	static const unsigned int MMU_NUM_BATS = 4;
// 	static const bool MMU_HAS_SOFTWARE_TABLE_SEARCH_SUPPORT = false;
// 	static const bool MMU_HAS_EXTENDED_ADDRESSING_SUPPORT = false;
// };

class MPC755Config : public MPC7XXConfig
{
public:
	typedef CPU<MPC755Config> STATE;

	static const Model MODEL = MPC755;
	
	static const uint32_t PROCESSOR_VERSION = 0x00083200UL;

	// L1 Data cache parameters
	class DL1_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			bool dirty;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
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
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};


		static const bool ENABLE = true;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// L2 cache parameters
	class L2_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			bool dirty;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 512 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// ITLB Parameters
	class ITLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 128;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 1; // 2-way set associative
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = MPC755Config::MEMORY_PAGE_SIZE;
		typedef MPC755Config::PTE PTE;
	};

	// DTLB Parameters
	class DTLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 128;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 1; // 2-way set associative
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = MPC755Config::MEMORY_PAGE_SIZE;
		typedef MPC755Config::PTE PTE;
	};

	// Front side bus parameters
	static const uint32_t FSB_BURST_SIZE = 32;

	// Programming model parameters
	static const unsigned int NUM_SPRGS = 8;
	
	// MMU parameters
	static const unsigned int NUM_BATS = 8;
	static const bool HAS_SOFTWARE_TABLE_SEARCH_SUPPORT = true;

	// Special purpose registers configuration

	// OEA
	static const bool HAS_HASH1 = true;
	static const bool HAS_HASH2 = true;
	static const bool HAS_HID2 = true;
	static const bool HAS_ICMP = true;
	static const bool HAS_DCMP = true;
	static const bool HAS_DMISS = true;
	static const bool HAS_IMISS = true;
	static const bool HAS_L2PM = true;
	static const bool HAS_RPA = true;

	static const bool HAS_HID2_L2AP_EN = true;
	static const bool HAS_HID2_SWT_EN = true;
	static const bool HAS_HID2_HIGH_BAT_EN = true;
	static const bool HAS_HID2_IWLCK = true;
	static const bool HAS_HID2_DWLCK = true;

	// L2CR bits
	static const unsigned int L2CR_L2E_OFFSET = 31;
	static const unsigned int L2CR_L2PE_OFFSET = 30;
	static const unsigned int L2CR_L2SIZ_OFFSET = 28;
	static const unsigned int L2CR_L2CLK_OFFSET = 25;
	static const unsigned int L2CR_L2RAM_OFFSET = 23;
	static const unsigned int L2CR_L2DR_OFFSET = 22;
	static const unsigned int L2CR_L2DO_OFFSET = 22;
	static const unsigned int L2CR_L2I_OFFSET = 21;
	static const unsigned int L2CR_L2CTL_OFFSET = 20;
	static const unsigned int L2CR_L2WT_OFFSET = 19;
	static const unsigned int L2CR_L2TS_OFFSET = 18;
	static const unsigned int L2CR_L2OH_OFFSET = 16;
	static const unsigned int L2CR_L2SL_OFFSET = 15;
	static const unsigned int L2CR_L2DF_OFFSET = 14;
	static const unsigned int L2CR_L2BYP_OFFSET = 13;
	static const unsigned int L2CR_L2FA_OFFSET = 12;
	static const unsigned int L2CR_L2REP_OFFSET = 12;
	static const unsigned int L2CR_L2HWF_OFFSET = 11;
	static const unsigned int L2CR_L2IO_OFFSET = 10;
	static const unsigned int L2CR_L2CLKSTP_OFFSET = 9;
	static const unsigned int L2CR_L2DRO_OFFSET = 8;
	static const unsigned int L2CR_L2IP_OFFSET = 0;
	
	static const uint32_t L2CR_L2E_BITSIZE = 1;
	static const uint32_t L2CR_L2PE_BITSIZE = 1;
	static const uint32_t L2CR_L2SIZ_BITSIZE = 2;
	static const uint32_t L2CR_L2CLK_BITSIZE = 2;
	static const uint32_t L2CR_L2RAM_BITSIZE = 2;
	static const uint32_t L2CR_L2DR_BITSIZE = 1;
	static const uint32_t L2CR_L2DO_BITSIZE = 0;
	static const uint32_t L2CR_L2I_BITSIZE = 1;
	static const uint32_t L2CR_L2CTL_BITSIZE = 1;
	static const uint32_t L2CR_L2WT_BITSIZE = 1;
	static const uint32_t L2CR_L2TS_BITSIZE = 1;
	static const uint32_t L2CR_L2OH_BITSIZE = 2;
	static const uint32_t L2CR_L2SL_BITSIZE = 1;
	static const uint32_t L2CR_L2DF_BITSIZE = 1;
	static const uint32_t L2CR_L2BYP_BITSIZE = 1;
	static const uint32_t L2CR_L2FA_BITSIZE = 0;
	static const uint32_t L2CR_L2REP_BITSIZE = 0;
	static const uint32_t L2CR_L2HWF_BITSIZE = 0;
	static const uint32_t L2CR_L2IO_BITSIZE = 1;
	static const uint32_t L2CR_L2CLKSTP_BITSIZE = 0;
	static const uint32_t L2CR_L2DRO_BITSIZE = 0;
	static const uint32_t L2CR_L2IP_BITSIZE = 1;
	
	static const uint32_t MSR_MASK = (HAS_MSR_VEC ? MSR_VEC_MASK : 0)
	                               | (HAS_MSR_POW ? MSR_POW_MASK : 0)
	                               | (HAS_MSR_ILE ? MSR_ILE_MASK : 0)
	                               | (HAS_MSR_EE  ? MSR_EE_MASK  : 0)
	                               | (HAS_MSR_PR  ? MSR_PR_MASK  : 0)
	                               | (HAS_MSR_FP  ? MSR_FP_MASK  : 0)
	                               | (HAS_MSR_ME  ? MSR_ME_MASK  : 0)
	                               | (HAS_MSR_FE0 ? MSR_FE0_MASK : 0)
	                               | (HAS_MSR_SE  ? MSR_SE_MASK  : 0)
	                               | (HAS_MSR_BE  ? MSR_BE_MASK  : 0)
	                               | (HAS_MSR_FE1 ? MSR_FE1_MASK : 0)
	                               | (HAS_MSR_IP  ? MSR_IP_MASK  : 0)
	                               | (HAS_MSR_IR  ? MSR_IR_MASK  : 0)
	                               | (HAS_MSR_DR  ? MSR_DR_MASK  : 0)
	                               | (HAS_MSR_PM  ? MSR_PM_MASK  : 0)
	                               | (HAS_MSR_PM  ? MSR_RI_MASK  : 0)
	                               | (HAS_MSR_LE  ? MSR_LE_MASK  : 0);

};

//=====================================================================
//=                    MPC744X-MPC745X configurations                 =
//=====================================================================

class MPC74XXHID0Layout : public HID0Layout
{
public:
	// HID0 bits offsets
	static const unsigned int HID0_TBEN_OFFSET = 26;
	static const unsigned int HID0_STEN_OFFSET = 24;
	static const unsigned int HID0_HIGH_BAT_EN_OFFSET = 23;
	static const unsigned int HID0_NAP_OFFSET = 22;
	static const unsigned int HID0_SLEEP_OFFSET = 21;
	static const unsigned int HID0_DPM_OFFSET = 20;
	static const unsigned int HID0_BHTCLR_OFFSET = 18;
	static const unsigned int HID0_XAEN_OFFSET = 17;
	static const unsigned int HID0_NHR_OFFSET = 16;
	static const unsigned int HID0_ICE_OFFSET = 15;
	static const unsigned int HID0_DCE_OFFSET = 14;
	static const unsigned int HID0_ILOCK_OFFSET = 13;
	static const unsigned int HID0_DLOCK_OFFSET = 12;
	static const unsigned int HID0_ICFI_OFFSET = 11;
	static const unsigned int HID0_DCFI_OFFSET = 10;
	static const unsigned int HID0_SPD_OFFSET = 9;
	static const unsigned int HID0_XBSEN_OFFSET = 8;
	static const unsigned int HID0_SGE_OFFSET = 7;
	static const unsigned int HID0_BTIC_OFFSET = 5;
	static const unsigned int HID0_LRSTK_OFFSET = 4;
	static const unsigned int HID0_FOLD_OFFSET = 3;
	static const unsigned int HID0_BHT_OFFSET = 2;
	static const unsigned int HID0_NOPDST_OFFSET = 1;
	static const unsigned int HID0_NOPTI_OFFSET = 0;
	
	// HID0 bits
	static const unsigned int HID0_TBEN_BITSIZE = 1;
	static const unsigned int HID0_STEN_BITSIZE = 1;
	static const unsigned int HID0_HIGH_BAT_EN_BITSIZE = 1;
	static const unsigned int HID0_NAP_BITSIZE = 1;
	static const unsigned int HID0_SLEEP_BITSIZE = 1;
	static const unsigned int HID0_DPM_BITSIZE = 1;
	static const unsigned int HID0_BHTCLR_BITSIZE = 1;
	static const unsigned int HID0_XAEN_BITSIZE = 1;
	static const unsigned int HID0_NHR_BITSIZE = 1;
	static const unsigned int HID0_ICE_BITSIZE = 1;
	static const unsigned int HID0_DCE_BITSIZE = 1;
	static const unsigned int HID0_ILOCK_BITSIZE = 1;
	static const unsigned int HID0_DLOCK_BITSIZE = 1;
	static const unsigned int HID0_ICFI_BITSIZE = 1;
	static const unsigned int HID0_DCFI_BITSIZE = 1;
	static const unsigned int HID0_SPD_BITSIZE = 1;
	static const unsigned int HID0_XBSEN_BITSIZE = 1;
	static const unsigned int HID0_SGE_BITSIZE = 1;
	static const unsigned int HID0_BTIC_BITSIZE = 1;
	static const unsigned int HID0_LRSTK_BITSIZE = 1;
	static const unsigned int HID0_FOLD_BITSIZE = 1;
	static const unsigned int HID0_BHT_BITSIZE = 1;
	static const unsigned int HID0_NOPDST_BITSIZE = 1;
	static const unsigned int HID0_NOPTI_BITSIZE = 1;
};

class MPC744X_745X_HID1Layout : public HID1Layout
{
public:
	// HID1 bits
	static const unsigned int HID1_PC0_OFFSET = 16;
	static const unsigned int HID1_PC1_OFFSET = 15;
	static const unsigned int HID1_PC2_OFFSET = 14;
	static const unsigned int HID1_PC3_OFFSET = 13;
	static const unsigned int HID1_PC4_OFFSET = 12;
	static const unsigned int HID1_PC5_OFFSET = 17;
	static const unsigned int HID1_EMCP_OFFSET = 31;
	static const unsigned int HID1_EBA_OFFSET = 29;
	static const unsigned int HID1_EBD_OFFSET = 28;
	static const unsigned int HID1_BCLK_OFFSET = 26;
	static const unsigned int HID1_ECLK_OFFSET = 25;
	static const unsigned int HID1_PAR_OFFSET = 24;
	static const unsigned int HID1_DFS4_OFFSET = 23;
	static const unsigned int HID1_DFS2_OFFSET = 22;
	static const unsigned int HID1_SYNCBE_OFFSET = 11;
	static const unsigned int HID1_ABE_OFFSET = 10;

	static const uint32_t HID1_PC0_BITSIZE = 1;
	static const uint32_t HID1_PC1_BITSIZE = 1;
	static const uint32_t HID1_PC2_BITSIZE = 1;
	static const uint32_t HID1_PC3_BITSIZE = 1;
	static const uint32_t HID1_PC4_BITSIZE = 1;
	static const uint32_t HID1_PC5_BITSIZE = 1;
	static const uint32_t HID1_EMCP_BITSIZE = 1;
	static const uint32_t HID1_EBA_BITSIZE = 1;
	static const uint32_t HID1_EBD_BITSIZE = 1;
	static const uint32_t HID1_BCLK_BITSIZE = 1;
	static const uint32_t HID1_ECLK_BITSIZE = 1;
	static const uint32_t HID1_PAR_BITSIZE = 1;
	static const uint32_t HID1_DFS4_BITSIZE = 1;
	static const uint32_t HID1_DFS2_BITSIZE = 1;
	static const uint32_t HID1_SYNCBE_BITSIZE = 1;
	static const uint32_t HID1_ABE_BITSIZE = 1;
};

class MPC744X_745X_L2CRLayout : public L2CRLayout
{
public:
	static const unsigned int L2CR_L2E_OFFSET = 31;
	static const unsigned int L2CR_L2PE_OFFSET = 30;
	static const unsigned int L2CR_L2I_OFFSET = 21;
	static const unsigned int L2CR_L2IO_OFFSET = 20;
	static const unsigned int L2CR_L2DO_OFFSET = 16;
	static const unsigned int L2CR_L2REP_OFFSET = 12;
	static const unsigned int L2CR_L2HWF_OFFSET = 11;
	static const unsigned int L2CR_LVRAME_OFFSET = 7;
	static const unsigned int L2CR_LVRAMM_OFFSET = 4;
	
	static const unsigned int L2CR_L2E_BITSIZE = 1;
	static const unsigned int L2CR_L2PE_BITSIZE = 1;
	static const unsigned int L2CR_L2I_BITSIZE = 1;
	static const unsigned int L2CR_L2IO_BITSIZE = 1;
	static const unsigned int L2CR_L2DO_BITSIZE = 1;
	static const unsigned int L2CR_L2REP_BITSIZE = 1;
	static const unsigned int L2CR_L2HWF_BITSIZE = 1;
	static const unsigned int L2CR_LVRAME_BITSIZE = 1;
	static const unsigned int L2CR_LVRAMM_BITSIZE = 3;
};


class MPC7447AConfig :
	public BaseConfig,
	public MSRLayout,
	public MPC74XXHID0Layout,
	public MPC744X_745X_HID1Layout,
	public HID2Layout,
	public MPC744X_745X_L2CRLayout,
	public ICTRLLayout,
	public IABRLayout
{
public:
	//typedef uint64_t physical_address_t;	// only 36 bits are used, all remaining bits *must* be set to zero
	typedef CPU<MPC7447AConfig> STATE;

	static const Model MODEL = MPC7447A;
	
	static const uint32_t PROCESSOR_VERSION = 0x80030100UL;

	// MSR reset value
	static const uint32_t MSR_RESET_VALUE = 0x40; // only MSR[IP] is set
	
	// HID0 value at reset
	static const uint32_t HID0_RESET_VALUE = 0;
	
	// L1 Data cache parameters
	class DL1_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			bool dirty;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; //32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
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
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// L2 cache parameters
	class L2_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			bool dirty;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = true;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 512 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 1; // 2 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// ITLB Parameters
	class ITLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 128;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 1; // 2-way set associative
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = MPC7447AConfig::MEMORY_PAGE_SIZE;
		typedef MPC7447AConfig::PTE PTE;
	};

	// DTLB Parameters
	class DTLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		static const bool ENABLE = true;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 128;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 1; // 2-way set associative
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = MPC7447AConfig::MEMORY_PAGE_SIZE;
		typedef MPC7447AConfig::PTE PTE;
	};

	// Front side bus parameters
	static const uint32_t FSB_BURST_SIZE = 32;

	// Programming model parameters
	static const unsigned int NUM_VRS = 32;
	static const unsigned int NUM_SPRGS = 8;
	
	// MMU parameters
	static const unsigned int NUM_BATS = 8;
	static const bool HAS_SOFTWARE_TABLE_SEARCH_SUPPORT = true;
	static const bool HAS_EXTENDED_ADDRESSING_SUPPORT = true;
	
	// Altivec
	static const bool HAS_ALTIVEC = true;
	
	// UISA
	static const bool HAS_VRSAVE = true;
	static const bool HAS_VSCR = true;

	// VEA
	static const bool HAS_TBL = true;
	static const bool HAS_TBU = true;

	// OEA
	static const bool HAS_BAMR = true;
	static const bool HAS_DABR = true;
	static const bool HAS_DAR = true;
	static const bool HAS_DEC = true;
	static const bool HAS_DSISR = true;
	static const bool HAS_EAR = true;
	static const bool HAS_HID0 = true;
	static const bool HAS_HID1 = true;
	static const bool HAS_IABR = true;
	static const bool HAS_LDSTDB = true; // undocumented !!!
	static const bool HAS_ICTC = true;
	static const bool HAS_ICTRL = true;
	static const bool HAS_L2CR = true;
	static const bool HAS_L3CR = true;
	static const bool HAS_L3ITCR0 = true;
	static const bool HAS_L3PM = true;
	static const bool HAS_LDSTCR = true;
	static const bool HAS_MMCR0 = true;
	static const bool HAS_MMCR1 = true;
	static const bool HAS_MMCR2 = true;
	static const bool HAS_MSSCR0 = true;
	static const bool HAS_MSSSR0 = true;
	static const bool HAS_PIR = true;
	static const bool HAS_PMC1 = true;
	static const bool HAS_PMC2 = true;
	static const bool HAS_PMC3 = true;
	static const bool HAS_PMC4 = true;
	static const bool HAS_PMC5 = true;
	static const bool HAS_PMC6 = true;
	static const bool HAS_PTEHI = true;
	static const bool HAS_PTELO = true;
	static const bool HAS_SDA = true;
	static const bool HAS_SIA = true;
	static const bool HAS_SVR = true;
	static const bool HAS_SDR1 = true;
	static const bool HAS_SRR0 = true;
	static const bool HAS_SRR1 = true;
	static const bool HAS_TLBMISS = true;

	static const bool HAS_DECREMENTER_OVERFLOW = true;
	static const bool HAS_EXTERNAL_INTERRUPT = true;
	static const bool HAS_SOFT_RESET = true;
	static const bool HAS_HARD_RESET = true;
	static const bool HAS_L1_INSN_CACHE_PARITY_ERROR = true;
	static const bool HAS_L1_DATA_CACHE_PARITY_ERROR = true;
	static const bool HAS_L2_CACHE_TAG_PARITY_ERROR = true;
	static const bool HAS_L2_CACHE_DATA_PARITY_ERROR = true;
	static const bool HAS_L3_CACHE_TAG_PARITY_ERROR = true;
	static const bool HAS_L3_CACHE_DATA_PARITY_ERROR = true;
	static const bool HAS_MCP = true;
	static const bool HAS_TEA = true;
	static const bool HAS_DATA_BUS_PARITY_ERROR = true;
	static const bool HAS_ADDRESS_BUS_PARITY_ERROR = true;
	static const bool HAS_SMI = true;
	static const bool HAS_THERMAL_MANAGEMENT_INTERRUPT = true;
	static const bool HAS_PERFORMANCE_MONITOR_INTERRUPT = true;

	static const bool HAS_MSR_VEC = true;
	static const bool HAS_MSR_POW = true;
	static const bool HAS_MSR_ILE = true;
	static const bool HAS_MSR_EE = true;
	static const bool HAS_MSR_PR = true;
	static const bool HAS_MSR_FP = true;
	static const bool HAS_MSR_ME = true;
	static const bool HAS_MSR_FE0 = true;
	static const bool HAS_MSR_SE = true;
	static const bool HAS_MSR_BE = true;
	static const bool HAS_MSR_FE1 = true;
	static const bool HAS_MSR_IP = true;
	static const bool HAS_MSR_IR = true;
	static const bool HAS_MSR_DR = true;
	static const bool HAS_MSR_PM = true;
	static const bool HAS_MSR_RI = true;
	static const bool HAS_MSR_LE = true;
	
	static const bool HAS_HID0_TBEN = true;
	static const bool HAS_HID0_STEN = true;
	static const bool HAS_HID0_HIGH_BAT_EN = true;
	static const bool HAS_HID0_NAP = true;
	static const bool HAS_HID0_SLEEP = true;
	static const bool HAS_HID0_DPM = true;
	static const bool HAS_HID0_BHTCLR = true;
	static const bool HAS_HID0_XAEN = true;
	static const bool HAS_HID0_NHR = true;
	static const bool HAS_HID0_ICE = true;
	static const bool HAS_HID0_DCE = true;
	static const bool HAS_HID0_ILOCK = true;
	static const bool HAS_HID0_DLOCK = true;
	static const bool HAS_HID0_ICFI = true;
	static const bool HAS_HID0_DCFI = true;
	static const bool HAS_HID0_SPD = true;
	static const bool HAS_HID0_XBSEN = true;
	static const bool HAS_HID0_SGE = true;
	static const bool HAS_HID0_BTIC = true;
	static const bool HAS_HID0_LRSTK = true;
	static const bool HAS_HID0_FOLD = true;
	static const bool HAS_HID0_BHT = true;
	static const bool HAS_HID0_NOPDST = true;
	static const bool HAS_HID0_NOPTI = true;

	static const bool HAS_HID1_EMCP = true;
	static const bool HAS_HID1_EBA = true;
	static const bool HAS_HID1_EBD = true;
	static const bool HAS_HID1_BCLK = true;
	static const bool HAS_HID1_ECLK = true;
	static const bool HAS_HID1_PAR = true;
	static const bool HAS_HID1_DFS2 = true;
	static const bool HAS_HID1_PC0 = true;
	static const bool HAS_HID1_PC1 = true;
	static const bool HAS_HID1_PC2 = true;
	static const bool HAS_HID1_PC3 = true;
	static const bool HAS_HID1_PC4 = true;
	static const bool HAS_HID1_SYNCBE = true;
	static const bool HAS_HID1_ABE = true;

	static const bool HAS_L2CR_L2E = true;
	static const bool HAS_L2CR_L2PE = true;
	static const bool HAS_L2CR_L2I = true;
	static const bool HAS_L2CR_L2IO = true;
	static const bool HAS_L2CR_L2DO = true;
	static const bool HAS_L2CR_L2REP = true;
	static const bool HAS_L2CR_L2HWF = true;

	static const bool HAS_ICTRL_CIRQ = true;
	static const bool HAS_ICTRL_EIEC = true;
	static const bool HAS_ICTRL_EDCE = true;
	static const bool HAS_ICTRL_EICP = true;
	static const bool HAS_ICTRL_ICWL = true;

	static const uint32_t MSR_MASK = (HAS_MSR_VEC ? MSR_VEC_MASK : 0)
	                               | (HAS_MSR_POW ? MSR_POW_MASK : 0)
	                               | (HAS_MSR_ILE ? MSR_ILE_MASK : 0)
	                               | (HAS_MSR_EE  ? MSR_EE_MASK  : 0)
	                               | (HAS_MSR_PR  ? MSR_PR_MASK  : 0)
	                               | (HAS_MSR_FP  ? MSR_FP_MASK  : 0)
	                               | (HAS_MSR_ME  ? MSR_ME_MASK  : 0)
	                               | (HAS_MSR_FE0 ? MSR_FE0_MASK : 0)
	                               | (HAS_MSR_SE  ? MSR_SE_MASK  : 0)
	                               | (HAS_MSR_BE  ? MSR_BE_MASK  : 0)
	                               | (HAS_MSR_FE1 ? MSR_FE1_MASK : 0)
	                               | (HAS_MSR_IP  ? MSR_IP_MASK  : 0)
	                               | (HAS_MSR_IR  ? MSR_IR_MASK  : 0)
	                               | (HAS_MSR_DR  ? MSR_DR_MASK  : 0)
	                               | (HAS_MSR_PM  ? MSR_PM_MASK  : 0)
	                               | (HAS_MSR_PM  ? MSR_RI_MASK  : 0)
	                               | (HAS_MSR_LE  ? MSR_LE_MASK  : 0);

};

class MPC755DebugConfig : public MPC755Config
{
public:
	typedef CPU<MPC755DebugConfig> STATE;

	// Debug stuff
	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_SETUP_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	static const bool DEBUG_DTLB_ENABLE = true;
	static const bool DEBUG_DL1_ENABLE = true;
	static const bool DEBUG_IL1_ENABLE = true;
	static const bool DEBUG_L2_ENABLE = true;
	static const bool DEBUG_LOAD_ENABLE = true;
	static const bool DEBUG_STORE_ENABLE = true;
	static const bool DEBUG_READ_MEMORY_ENABLE = true;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = true;
	static const bool DEBUG_EXCEPTION_ENABLE = true;
	static const bool DEBUG_SET_MSR_ENABLE = true;
	static const bool DEBUG_SET_HID0_ENABLE = true;
	static const bool DEBUG_SET_HID1_ENABLE = true;
	static const bool DEBUG_SET_HID2_ENABLE = true;
	static const bool DEBUG_SET_L2CR_ENABLE = true;
};

class MPC7447ADebugConfig : public MPC7447AConfig
{
public:
	typedef CPU<MPC7447ADebugConfig> STATE;

	// Debug stuff
	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_SETUP_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	static const bool DEBUG_DTLB_ENABLE = true;
	static const bool DEBUG_DL1_ENABLE = true;
	static const bool DEBUG_IL1_ENABLE = true;
	static const bool DEBUG_L2_ENABLE = true;
	static const bool DEBUG_LOAD_ENABLE = true;
	static const bool DEBUG_STORE_ENABLE = true;
	static const bool DEBUG_READ_MEMORY_ENABLE = true;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = true;
	static const bool DEBUG_EXCEPTION_ENABLE = true;
	static const bool DEBUG_SET_MSR_ENABLE = true;
	static const bool DEBUG_SET_HID0_ENABLE = true;
	static const bool DEBUG_SET_HID1_ENABLE = true;
	static const bool DEBUG_SET_HID2_ENABLE = true;
	static const bool DEBUG_SET_L2CR_ENABLE = true;
};

/*
class MPC7447APerfModelConfig
{
public:
	// Fetch
	static const uint32_t FETCH_WIDTH = 4;

	// Decode/Issue
	static const uint32_t DECODE_WIDTH = 3;
	static const uint32_t VR_ISSUE_WIDTH = 2;
	static const uint32_t GPR_ISSUE_WIDTH = 3;
	static const uint32_t FPR_ISSUE_WIDTH = 2;

	class InstructionWindowConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operation ELEMENT;
		static const unsigned int SIZE = 256;
		static const unsigned int BUFFER_SIZE = 256; // BUFFER_SIZE *must* be >= SIZE and a power of two
	};
	
	class InstructionQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef uint32_t ELEMENT;
		static const unsigned int SIZE = 12;
		static const unsigned int BUFFER_SIZE = 16; // BUFFER_SIZE *must* be >= SIZE and a power of two
	};
	
	class VRIssueQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operation *ELEMENT;
		static const unsigned int SIZE = 4;
		static const unsigned int BUFFER_SIZE = 4; // BUFFER_SIZE *must* be >= SIZE and a power of two
	};
	
	class GPRIssueQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operation *ELEMENT;
		static const unsigned int SIZE = 6;
		static const unsigned int BUFFER_SIZE = 8; // BUFFER_SIZE *must* be >= SIZE and a power of two
	};
	
	class FPRIssueQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operation *ELEMENT;
		static const unsigned int SIZE = 2;
		static const unsigned int BUFFER_SIZE = 2; // BUFFER_SIZE *must* be >= SIZE and a power of two
	};
	
	class CompletionQueueConfig
	{
	public:
		static const bool DEBUG = true;
		typedef Operation *ELEMENT;
		static const unsigned int SIZE = 16;
		static const unsigned int BUFFER_SIZE = 16; // BUFFER_SIZE *must* be >= SIZE and a power of two
	};
};
*/

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
