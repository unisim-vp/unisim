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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CONFIG_HH__

#include <unisim/component/cxx/processor/powerpc/config.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

template <class CONFIG>
class CPU;

class HID0Layout
{
public:
	// HID0 bit fields offsets
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
	
	// HID0 bit fields size
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
	
	// HID0 bit fields AND masks
	static const uint32_t HID0_TBEN_MASK = ((1UL << HID0_TBEN_BITSIZE) - 1) << HID0_TBEN_OFFSET;
	static const uint32_t HID0_STEN_MASK = ((1UL << HID0_STEN_BITSIZE) - 1) << HID0_STEN_OFFSET;
	static const uint32_t HID0_HIGH_BAT_EN_MASK = ((1UL << HID0_HIGH_BAT_EN_BITSIZE) - 1) << HID0_HIGH_BAT_EN_OFFSET;
	static const uint32_t HID0_NAP_MASK = ((1UL << HID0_NAP_BITSIZE) - 1) << HID0_NAP_OFFSET;
	static const uint32_t HID0_SLEEP_MASK = ((1UL << HID0_SLEEP_BITSIZE) - 1) << HID0_SLEEP_OFFSET;
	static const uint32_t HID0_DPM_MASK = ((1UL << HID0_DPM_BITSIZE) - 1) << HID0_DPM_OFFSET;
	static const uint32_t HID0_BHTCLR_MASK = ((1UL << HID0_BHTCLR_BITSIZE) - 1) << HID0_BHTCLR_OFFSET;
	static const uint32_t HID0_XAEN_MASK = ((1UL << HID0_XAEN_BITSIZE) - 1) << HID0_XAEN_OFFSET;
	static const uint32_t HID0_NHR_MASK = ((1UL << HID0_NHR_BITSIZE) - 1) << HID0_NHR_OFFSET;
	static const uint32_t HID0_ICE_MASK = ((1UL << HID0_ICE_BITSIZE) - 1) << HID0_ICE_OFFSET;
	static const uint32_t HID0_DCE_MASK = ((1UL << HID0_DCE_BITSIZE) - 1) << HID0_DCE_OFFSET;
	static const uint32_t HID0_ILOCK_MASK = ((1UL << HID0_ILOCK_BITSIZE) - 1) << HID0_ILOCK_OFFSET;
	static const uint32_t HID0_DLOCK_MASK = ((1UL << HID0_DLOCK_BITSIZE) - 1) << HID0_DLOCK_OFFSET;
	static const uint32_t HID0_ICFI_MASK = ((1UL << HID0_ICFI_BITSIZE) - 1) << HID0_ICFI_OFFSET;
	static const uint32_t HID0_DCFI_MASK = ((1UL << HID0_DCFI_BITSIZE) - 1) << HID0_DCFI_OFFSET;
	static const uint32_t HID0_SPD_MASK = ((1UL << HID0_SPD_BITSIZE) - 1) << HID0_SPD_OFFSET;
	static const uint32_t HID0_XBSEN_MASK = ((1UL << HID0_XBSEN_BITSIZE) - 1) << HID0_XBSEN_OFFSET;
	static const uint32_t HID0_SGE_MASK = ((1UL << HID0_SGE_BITSIZE) - 1) << HID0_SGE_OFFSET;
	static const uint32_t HID0_BTIC_MASK = ((1UL << HID0_BTIC_BITSIZE) - 1) << HID0_BTIC_OFFSET;
	static const uint32_t HID0_LRSTK_MASK = ((1UL << HID0_LRSTK_BITSIZE) - 1) << HID0_LRSTK_OFFSET;
	static const uint32_t HID0_FOLD_MASK = ((1UL << HID0_FOLD_BITSIZE) - 1) << HID0_FOLD_OFFSET;
	static const uint32_t HID0_BHT_MASK = ((1UL << HID0_BHT_BITSIZE) - 1) << HID0_BHT_OFFSET;
	static const uint32_t HID0_NOPDST_MASK = ((1UL << HID0_NOPDST_BITSIZE) - 1) << HID0_NOPDST_OFFSET;
	static const uint32_t HID0_NOPTI_MASK = ((1UL << HID0_NOPTI_BITSIZE) - 1) << HID0_NOPTI_OFFSET;
	
	// HID0 bitwise AND mask
	static const uint32_t HID0_MASK = HID0_TBEN_MASK | HID0_STEN_MASK | HID0_HIGH_BAT_EN_MASK | HID0_NAP_MASK |
	                                  HID0_SLEEP_MASK | HID0_DPM_MASK | HID0_BHTCLR_MASK | HID0_XAEN_MASK |
	                                  HID0_NHR_MASK | HID0_ICE_MASK | HID0_DCE_MASK | HID0_ILOCK_MASK |
	                                  HID0_DLOCK_MASK | HID0_ICFI_MASK | HID0_DCFI_MASK | HID0_SPD_MASK |
	                                  HID0_XBSEN_MASK | HID0_SGE_MASK | HID0_BTIC_MASK | HID0_LRSTK_MASK |
	                                  HID0_FOLD_MASK | HID0_BHT_MASK | HID0_NOPDST_MASK | HID0_NOPTI_MASK;

	// HID0 value at reset
	static const uint32_t HID0_RESET_VALUE = 0x80000000UL;
};

class HID1Layout
{
public:
	// HID1 bit fields offsets
	static const unsigned int HID1_EMCP_OFFSET = 31;
	static const unsigned int HID1_EBA_OFFSET = 29;
	static const unsigned int HID1_EBD_OFFSET = 28;
	static const unsigned int HID1_BCLK_OFFSET = 26;
	static const unsigned int HID1_ECLK_OFFSET = 25;
	static const unsigned int HID1_PAR_OFFSET = 24;
	static const unsigned int HID1_DFS2_OFFSET = 22;
	static const unsigned int HID1_PC0_OFFSET = 16;
	static const unsigned int HID1_PC1_OFFSET = 15;
	static const unsigned int HID1_PC2_OFFSET = 14;
	static const unsigned int HID1_PC3_OFFSET = 13;
	static const unsigned int HID1_PC4_OFFSET = 12;
	static const unsigned int HID1_SYNCBE_OFFSET = 11;
	static const unsigned int HID1_ABE_OFFSET = 10;

	// HID1 bit fields sizes
	static const unsigned int HID1_EMCP_BITSIZE = 1;
	static const unsigned int HID1_EBA_BITSIZE = 1;
	static const unsigned int HID1_EBD_BITSIZE = 1;
	static const unsigned int HID1_BCLK_BITSIZE = 1;
	static const unsigned int HID1_ECLK_BITSIZE = 1;
	static const unsigned int HID1_PAR_BITSIZE = 1;
	static const unsigned int HID1_DFS2_BITSIZE = 1;
	static const unsigned int HID1_PC0_BITSIZE = 1;
	static const unsigned int HID1_PC1_BITSIZE = 1;
	static const unsigned int HID1_PC2_BITSIZE = 1;
	static const unsigned int HID1_PC3_BITSIZE = 1;
	static const unsigned int HID1_PC4_BITSIZE = 1;
	static const unsigned int HID1_SYNCBE_BITSIZE = 1;
	static const unsigned int HID1_ABE_BITSIZE = 1;

	// HID1 bit fields AND masks
	static const uint32_t HID1_EMCP_MASK = ((1UL << HID1_EMCP_BITSIZE) - 1) << HID1_EMCP_OFFSET;
	static const uint32_t HID1_EBA_MASK = ((1UL << HID1_EBA_BITSIZE) - 1) << HID1_EBA_OFFSET;
	static const uint32_t HID1_EBD_MASK = ((1UL << HID1_EBD_BITSIZE) - 1) << HID1_EBD_OFFSET;
	static const uint32_t HID1_BCLK_MASK = ((1UL << HID1_BCLK_BITSIZE) - 1) << HID1_BCLK_OFFSET;
	static const uint32_t HID1_ECLK_MASK = ((1UL << HID1_ECLK_BITSIZE) - 1) << HID1_ECLK_OFFSET;
	static const uint32_t HID1_PAR_MASK = ((1UL << HID1_PAR_BITSIZE) - 1) << HID1_PAR_OFFSET;
	static const uint32_t HID1_DFS2_MASK = ((1UL << HID1_DFS2_BITSIZE) - 1) << HID1_DFS2_OFFSET;
	static const uint32_t HID1_PC0_MASK = ((1UL << HID1_PC0_BITSIZE) - 1) << HID1_PC0_OFFSET;
	static const uint32_t HID1_PC1_MASK = ((1UL << HID1_PC1_BITSIZE) - 1) << HID1_PC1_OFFSET;
	static const uint32_t HID1_PC2_MASK = ((1UL << HID1_PC2_BITSIZE) - 1) << HID1_PC2_OFFSET;
	static const uint32_t HID1_PC3_MASK = ((1UL << HID1_PC3_BITSIZE) - 1) << HID1_PC3_OFFSET;
	static const uint32_t HID1_PC4_MASK = ((1UL << HID1_PC4_BITSIZE) - 1) << HID1_PC4_OFFSET;
	static const uint32_t HID1_SYNCBE_MASK = ((1UL << HID1_SYNCBE_BITSIZE) - 1) << HID1_SYNCBE_OFFSET;
	static const uint32_t HID1_ABE_MASK = ((1UL << HID1_ABE_BITSIZE) - 1) << HID1_ABE_OFFSET;
	
	// HID1 bitwise AND mask
	static const uint32_t HID1_MASK = HID1_EMCP_MASK | HID1_EBA_MASK | HID1_EBD_MASK | HID1_BCLK_MASK |
	                                  HID1_ECLK_MASK | HID1_PAR_MASK | HID1_DFS2_MASK | HID1_PC0_MASK |
	                                  HID1_PC1_MASK | HID1_PC2_MASK | HID1_PC3_MASK | HID1_PC4_MASK |
	                                  HID1_SYNCBE_MASK | HID1_ABE_MASK;
	
	// HID1 value at reset
	static const uint32_t HID1_RESET_VALUE = 0x00000080UL;
};

class MSRLayout : unisim::component::cxx::processor::powerpc::MSRLayout
{
public:
	// MSR bitwise AND mask
	static const uint32_t MSR_MASK = MSR_VEC_MASK | MSR_POW_MASK | MSR_ILE_MASK | MSR_EE_MASK |
	                                 MSR_PR_MASK | MSR_FP_MASK | MSR_ME_MASK | MSR_FE0_MASK |
	                                 MSR_SE_MASK | MSR_BE_MASK | MSR_FE1_MASK | MSR_IP_MASK |
	                                 MSR_IR_MASK | MSR_DR_MASK | MSR_PM_MASK | MSR_RI_MASK |
	                                 MSR_LE_MASK;

	// MSR reset value
	static const uint32_t MSR_RESET_VALUE = MSR_IP_MASK; // only MSR[IP] is set
};

class ICTRLLayout
{
public:
	static const unsigned int ICTRL_CIRQ_OFFSET = 31;
	static const unsigned int ICTRL_EIEC_OFFSET = 27;
	static const unsigned int ICTRL_EDCE_OFFSET = 26;
	static const unsigned int ICTRL_EICP_OFFSET = 8;
	static const unsigned int ICTRL_ICWL_OFFSET = 0;

	static const unsigned int ICTRL_CIRQ_BITSIZE = 1;
	static const unsigned int ICTRL_EIEC_BITSIZE = 1;
	static const unsigned int ICTRL_EDCE_BITSIZE = 1;
	static const unsigned int ICTRL_EICP_BITSIZE = 1;
	static const unsigned int ICTRL_ICWL_BITSIZE = 8;
	
	static const uint32_t ICTRL_CIRQ_MASK = ((1UL << ICTRL_CIRQ_BITSIZE) - 1) << ICTRL_CIRQ_OFFSET;
	static const uint32_t ICTRL_EIEC_MASK = ((1UL << ICTRL_EIEC_BITSIZE) - 1) << ICTRL_EIEC_OFFSET;
	static const uint32_t ICTRL_EDCE_MASK = ((1UL << ICTRL_EDCE_BITSIZE) - 1) << ICTRL_EDCE_OFFSET;
	static const uint32_t ICTRL_EICP_MASK = ((1UL << ICTRL_EICP_BITSIZE) - 1) << ICTRL_EICP_OFFSET;
	static const uint32_t ICTRL_ICWL_MASK = ((1UL << ICTRL_ICWL_BITSIZE) - 1) << ICTRL_ICWL_OFFSET;
	
	static const uint32_t ICTRL_MASK = ICTRL_CIRQ_MASK | ICTRL_EIEC_MASK | ICTRL_EDCE_MASK | ICTRL_EICP_MASK | ICTRL_ICWL_MASK;
	
	static const uint32_t ICTRL_RESET_VALUE = 0x0UL;
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
	
	static const uint32_t IABR_RESET_VALUE = 0x0UL;

};

class L2CRLayout
{
public:
	static const unsigned int L2CR_L2E_OFFSET = 31;
	static const unsigned int L2CR_L2PE_OFFSET = 30;
	static const unsigned int L2CR_L2I_OFFSET = 21;
	static const unsigned int L2CR_L2IO_OFFSET = 20;
	static const unsigned int L2CR_L2DO_OFFSET = 16;
	static const unsigned int L2CR_L2REP_OFFSET = 12;
	static const unsigned int L2CR_L2HWF_OFFSET = 11;
	
	static const uint32_t L2CR_L2E_BITSIZE = 1;
	static const uint32_t L2CR_L2PE_BITSIZE = 1;
	static const uint32_t L2CR_L2I_BITSIZE = 1;
	static const uint32_t L2CR_L2IO_BITSIZE = 1;
	static const uint32_t L2CR_L2REP_BITSIZE = 1;
	static const uint32_t L2CR_L2HWF_BITSIZE = 1;
	
	static const uint32_t L2CR_L2E_MASK = ((1UL << L2CR_L2E_BITSIZE) - 1) << L2CR_L2E_OFFSET;
	static const uint32_t L2CR_L2PE_MASK = ((1UL << L2CR_L2PE_BITSIZE) - 1) << L2CR_L2PE_OFFSET;
	static const uint32_t L2CR_L2I_MASK = ((1UL << L2CR_L2I_BITSIZE) - 1) << L2CR_L2I_OFFSET;
	static const uint32_t L2CR_L2IO_MASK = ((1UL << L2CR_L2IO_BITSIZE) - 1) << L2CR_L2IO_OFFSET;
	static const uint32_t L2CR_L2REP_MASK = ((1UL << L2CR_L2REP_BITSIZE) - 1) << L2CR_L2REP_OFFSET;
	static const uint32_t L2CR_L2HWF_MASK = ((1UL << L2CR_L2HWF_BITSIZE) - 1) << L2CR_L2HWF_OFFSET;
	
	static const uint32_t L2CR_MASK = L2CR_L2E_MASK | L2CR_L2PE_MASK | L2CR_L2I_MASK |
	                                  L2CR_L2IO_MASK | L2CR_L2REP_MASK | L2CR_L2HWF_MASK;
	
	static const uint32_t L2CR_RESET_VALUE = 0x0UL;
};

class Config :
	public unisim::component::cxx::processor::powerpc::Config,
	public MSRLayout,
	public HID0Layout,
	public HID1Layout,
	public L2CRLayout,
	public ICTRLLayout,
	public IABRLayout
{
public:
	typedef CPU<Config> STATE;

	static const Model MODEL = MPC7447A;
	
	static const uint32_t PROCESSOR_VERSION = 0x80030100UL;
	
	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint64_t virtual_address_t;     // only 52 bits are used, all remaining bits *must* be set to zero
	typedef uint32_t physical_address_t;    // 32-bit physical address
	typedef enum { WIMG_DEFAULT = 0, WIMG_GUARDED_MEMORY = 1, WIMG_MEMORY_COHERENCY_ENFORCED = 2, WIMG_CACHE_INHIBITED = 4, WIMG_WRITE_THROUGH = 8 } WIMG;
	static const uint32_t MEMORY_PAGE_SIZE = 4096; // DO NOT MODIFY THIS VALUE !!!!!

	//=====================================================================
	//=                         Exception vectors                         =
	//=====================================================================
	// PowerPC 32 bits common exceptions
	static const physical_address_t EXC_SYSTEM_RESET_VECTOR = 0x100UL;                  //!< system reset vector (all)
	static const physical_address_t EXC_MACHINE_CHECK_VECTOR = 0x200UL;                 //!< machine check exception vector (all)
	static const physical_address_t EXC_DSI_VECTOR = 0x300UL;                           //!< DSI exception vector (all)
	static const physical_address_t EXC_ISI_VECTOR = 0x400UL;                           //!< ISI exception vector (all)
	static const physical_address_t EXC_EXTERNAL_INTERRUPT_VECTOR = 0x500UL;            //!< External interrupt vector (all)
	static const physical_address_t EXC_ALIGNMENT_VECTOR = 0x600UL;                     //!< Alignment exception vector (all)
	static const physical_address_t EXC_PROGRAM_VECTOR = 0x700UL;                       //!< Program exception vector (all)
	static const physical_address_t EXC_FLOATING_POINT_UNAVAILABLE_VECTOR = 0x800UL;    //!< Floating unavailable exception vector (all)
	static const physical_address_t EXC_DECREMENTER_VECTOR = 0x900UL;                   //!< Decrementer exception vector (all)
	static const physical_address_t EXC_SYSTEM_CALL_VECTOR = 0xc00UL;                   //!< system call exception vector (all)
	static const physical_address_t EXC_TRACE_VECTOR = 0xd00UL;                         //!< Trace exception vector (all)

	// Additional exceptions in MPC7xx 
	static const physical_address_t EXC_PERFORMANCE_MONITOR_VECTOR = 0xf00UL;
	static const physical_address_t EXC_INSTRUCTION_ADDRESS_BREAKPOINT_VECTOR = 0x1300UL;
	static const physical_address_t EXC_SYSTEM_MANAGEMENT_INTERRUPT_VECTOR = 0x1400UL;

	// Additional exceptions in MPC7x5
	static const physical_address_t EXC_ITLB_MISS_VECTOR = 0x1000UL;
	static const physical_address_t EXC_DTLB_LOAD_MISS_VECTOR = 0x1100UL;
	static const physical_address_t EXC_DTLB_STORE_MISS_VECTOR = 0x1200UL;
	
	// Additional exceptions in MPC74xx
	static const physical_address_t EXC_ALTIVEC_UNAVAILABLE_VECTOR = 0xf20UL;

	// Asynchronous interrupt signal masks
	static const unsigned int SIG_DECREMENTER_OVERFLOW = 1;
	static const unsigned int SIG_EXTERNAL_INTERRUPT = 2;
	static const unsigned int SIG_SOFT_RESET = 4;
	static const unsigned int SIG_HARD_RESET = 8;
	static const unsigned int SIG_MCP = 16;
	static const unsigned int SIG_TEA = 32;
	static const unsigned int SIG_SMI = 64;
	static const unsigned int SIG_PERFORMANCE_MONITOR_INTERRUPT = 128;

	// start address
	static const uint32_t START_ADDR = EXC_SYSTEM_RESET_VECTOR | ((MSR_RESET_VALUE & MSR_IP_MASK) ? 0xfff00000UL : 0x00000000UL); // processor starts at system reset interrupt handler

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

	static const uint32_t IQ_SIZE = 12;
	static const uint32_t VIQ_SIZE = 4;
	static const uint32_t GIQ_SIZE = 6;
	static const uint32_t FIQ_SIZE = 2;
	static const uint32_t CQ_SIZE = 16;
	static const uint32_t NUM_GPR_RENAME_REGISTERS = 16;
	static const uint32_t NUM_FPR_RENAME_REGISTERS = 16;
	static const uint32_t IU1_RS_SIZE = 1;
	static const uint32_t IU2_RS_SIZE = 2;
	static const uint32_t FPU_RS_SIZE = 2;
	static const uint32_t LSU_RS_SIZE = 2;
	static const uint32_t FSQ_SIZE = 3;
	static const uint32_t CSQ_SIZE = 5;
	static const uint32_t BSQ_SIZE = 10;
	static const uint32_t FETCH_WIDTH = 4;
	static const uint32_t DECODE_WIDTH = 3;
	static const uint32_t MAX_DISPATCHED_LOAD_STORES_PER_CYCLE = 1;
	static const uint32_t MAX_GPR_RENAMES_PER_CYCLE = 4;
	static const uint32_t MAX_FPR_RENAMES_PER_CYCLE = 2;
	static const uint32_t MAX_VPR_RENAMES_PER_CYCLE = 3;
	static const uint32_t VR_ISSUE_WIDTH = 2;
	static const uint32_t GPR_ISSUE_WIDTH = 3;
	static const uint32_t FPR_ISSUE_WIDTH = 1;
	static const uint32_t NUM_IU1 = 3;
	static const uint32_t IU1_PIPELINE_DEPTH = 1;
	static const uint32_t NUM_IU2 = 1;
	static const uint32_t IU2_PIPELINE_DEPTH = 3;
	static const uint32_t NUM_FPU = 1;
	static const uint32_t FPU_PIPELINE_DEPTH = 5;
	static const uint32_t COMPLETE_WIDTH = 3;
	static const uint32_t MAX_GPR_WB_PER_CYCLE = 3;
	static const uint32_t MAX_FPR_WB_PER_CYCLE = 3;
	static const uint32_t MAX_CR_WB_PER_CYCLE = 3;
	static const uint32_t MAX_LR_WB_PER_CYCLE = 1;
	static const uint32_t MAX_CTR_WB_PER_CYCLE = 1;
	static const uint32_t IL1_LATENCY = 2;
	static const uint32_t DL1_LATENCY = 2;
	static const uint32_t L2_LATENCY = 2;
	static const uint32_t MAX_OUTSTANDING_L1_FETCH_MISS = 5;
	static const uint32_t MAX_OUTSTANDING_L1_LOAD_MISS = 5;
	static const uint32_t MAX_OUTSTANDING_L2_LOAD_MISS = 11;
	static const uint32_t MAX_OUTSTANDING_L1_STORE_MISS = 1;
	static const uint32_t MAX_OUTSTANDING_L2_STORE_MISS = 9;
	static const uint32_t STORE_WB_LATENCY = 2;

	// MMU Page Table Entry
	class PTE
	{
	public:
		physical_address_t base_physical_addr;  /*< 32-bit base physical address */
		uint32_t c;                             /*< 1-bit C field */
		WIMG wimg;                              /*< 4-bit WIMG field */
		uint32_t pp;                            /*< 2-bit page protection field */
	};

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
		static const uint32_t PAGE_SIZE = Config::MEMORY_PAGE_SIZE;
		typedef Config::PTE PTE;
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
		static const uint32_t PAGE_SIZE = Config::MEMORY_PAGE_SIZE;
		typedef Config::PTE PTE;
	};

	// Front side bus parameters
	static const uint32_t FSB_BURST_SIZE = 32; // 256-bit burst
	static const uint32_t FSB_WIDTH = 8; // 64-bit front side bus

	// Simulation performance speed-up features
	static const bool PREFETCH_BUFFER_ENABLE = true; // enable faster fetch
	static const unsigned int NUM_PREFETCH_BUFFER_ENTRIES = 8; //!< Maximum number of instruction in the prefetch buffer
	static const bool IABR_ENABLE = false;
	static const bool DABR_ENABLE = false;

	// Programming model parameters
	static const unsigned int NUM_GPRS = 32;
	static const unsigned int NUM_FPRS = 32;
	static const unsigned int NUM_SRS = 16;
	static const unsigned int NUM_VRS = 32;
	static const unsigned int NUM_SPRGS = 8;
	
	// MMU parameters
	static const unsigned int NUM_BATS = 8;
	
	// Altivec
	static const bool HAS_ALTIVEC = true;
	
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


} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
