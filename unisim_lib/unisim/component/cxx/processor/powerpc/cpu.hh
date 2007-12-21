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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/powerpc_types.hh>
#include <unisim/component/cxx/processor/powerpc/fpu.hh>
#include <unisim/component/cxx/cache/mesi/cache.hh>
#include <unisim/component/cxx/cache/insn/cache.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/component/cxx/cache/cache_interface.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
//#include <memories/endian_interface.hh>
#include <unisim/component/cxx/processor/powerpc/mmu.hh>
#include <unisim/component/cxx/processor/powerpc/exception.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/cpu_linux_os.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/service/interfaces/logger.hh>
#include <map>
#include <iostream>
#include <stdio.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using unisim::util::arithmetic::Add32;
using unisim::util::arithmetic::RotateLeft;

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::component::cxx::cache::BusInterface;
using unisim::component::cxx::cache::BusStatus;
using unisim::component::cxx::cache::BusControl;
using namespace unisim::util::endian;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::LinuxOS;
using unisim::service::interfaces::CPULinuxOS;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Synchronizable;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ParameterArray;
using unisim::service::interfaces::Logger;
//using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using namespace std;

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



typedef union
{
	uint8_t b[16];
	uint16_t h[8];
	uint32_t w[4];
} vr_t;

template <class CONFIG>
class CPU :
	public Decoder<CONFIG>,
	public Client<Loader<typename CONFIG::physical_address_t> >,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<unisim::service::interfaces::Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Service<CPULinuxOS>,
	public Client<Memory<typename CONFIG::address_t> >,
	public Client<LinuxOS>,
	public Client<Logger>,
	public Client<CachePowerEstimator>,
	public Client<PowerMode>,
	public Service<Synchronizable>
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::virtual_address_t virtual_address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;

	static const uint32_t FSB_WIDTH = 8; // 64-bit front side bus

	static const uint32_t MSR_VEC_MASK = CONFIG::HAS_MSR_VEC ? ((1UL << CONFIG::MSR_VEC_BITSIZE) - 1) << CONFIG::MSR_VEC_OFFSET : 0;
	static const uint32_t MSR_POW_MASK = CONFIG::HAS_MSR_POW ? ((1UL << CONFIG::MSR_POW_BITSIZE) - 1) << CONFIG::MSR_POW_OFFSET : 0;
	static const uint32_t MSR_ILE_MASK = CONFIG::HAS_MSR_ILE ? ((1UL << CONFIG::MSR_ILE_BITSIZE) - 1) << CONFIG::MSR_ILE_OFFSET : 0;
	static const uint32_t MSR_EE_MASK = CONFIG::HAS_MSR_EE ? ((1UL << CONFIG::MSR_EE_BITSIZE) - 1) << CONFIG::MSR_EE_OFFSET : 0;
	static const uint32_t MSR_PR_MASK = CONFIG::HAS_MSR_PR ? ((1UL << CONFIG::MSR_PR_BITSIZE) - 1) << CONFIG::MSR_PR_OFFSET : 0;
	static const uint32_t MSR_FP_MASK = CONFIG::HAS_MSR_FP ? ((1UL << CONFIG::MSR_FP_BITSIZE) - 1) << CONFIG::MSR_FP_OFFSET : 0;
	static const uint32_t MSR_ME_MASK = CONFIG::HAS_MSR_ME ? ((1UL << CONFIG::MSR_ME_BITSIZE) - 1) << CONFIG::MSR_ME_OFFSET : 0;
	static const uint32_t MSR_FE0_MASK = CONFIG::HAS_MSR_FE0 ? ((1UL << CONFIG::MSR_FE0_BITSIZE) - 1) << CONFIG::MSR_FE0_OFFSET : 0;
	static const uint32_t MSR_SE_MASK = CONFIG::HAS_MSR_SE ? ((1UL << CONFIG::MSR_SE_BITSIZE) - 1) << CONFIG::MSR_SE_OFFSET: 0;
	static const uint32_t MSR_BE_MASK = CONFIG::HAS_MSR_BE ? ((1UL << CONFIG::MSR_BE_BITSIZE) - 1) << CONFIG::MSR_BE_OFFSET: 0;
	static const uint32_t MSR_FE1_MASK = CONFIG::HAS_MSR_FE1 ? ((1UL << CONFIG::MSR_FE1_BITSIZE) - 1) << CONFIG::MSR_FE1_OFFSET : 0;
	static const uint32_t MSR_IP_MASK = CONFIG::HAS_MSR_IP ? ((1UL << CONFIG::MSR_IP_BITSIZE) - 1) << CONFIG::MSR_IP_OFFSET : 0;
	static const uint32_t MSR_IR_MASK = CONFIG::HAS_MSR_IR ? ((1UL << CONFIG::MSR_IR_BITSIZE) - 1) << CONFIG::MSR_IR_OFFSET : 0;
	static const uint32_t MSR_DR_MASK = CONFIG::HAS_MSR_DR ? ((1UL << CONFIG::MSR_DR_BITSIZE) - 1) << CONFIG::MSR_DR_OFFSET : 0;
	static const uint32_t MSR_PM_MASK = CONFIG::HAS_MSR_PM ? ((1UL << CONFIG::MSR_PM_BITSIZE) - 1) << CONFIG::MSR_PM_OFFSET : 0;
	static const uint32_t MSR_RI_MASK = CONFIG::HAS_MSR_RI ? ((1UL << CONFIG::MSR_RI_BITSIZE) - 1) << CONFIG::MSR_RI_OFFSET : 0;
	static const uint32_t MSR_LE_MASK = CONFIG::HAS_MSR_LE ? ((1UL << CONFIG::MSR_RI_BITSIZE) - 1) << CONFIG::MSR_LE_OFFSET : 0;
	static const uint32_t MSR_MASK = MSR_VEC_MASK | MSR_POW_MASK | MSR_ILE_MASK | MSR_EE_MASK | MSR_PR_MASK | MSR_FP_MASK | MSR_ME_MASK | MSR_FE0_MASK |
	                                 MSR_SE_MASK | MSR_BE_MASK | MSR_FE1_MASK | MSR_IP_MASK | MSR_IR_MASK | MSR_DR_MASK | MSR_PM_MASK | MSR_RI_MASK | MSR_LE_MASK;


	static const uint32_t HID0_EMCP_MASK = CONFIG::HAS_HID0_EMCP ? ((1UL << CONFIG::HID0_EMCP_BITSIZE) - 1) << CONFIG::HID0_EMCP_OFFSET : 0;
	static const uint32_t HID0_DBP_MASK = CONFIG::HAS_HID0_DBP ? ((1UL << CONFIG::HID0_DBP_BITSIZE) - 1) << CONFIG::HID0_DBP_OFFSET : 0;
	static const uint32_t HID0_EBA_MASK = CONFIG::HAS_HID0_EBA ? ((1UL << CONFIG::HID0_EBA_BITSIZE) - 1) << CONFIG::HID0_EBA_OFFSET : 0;
	static const uint32_t HID0_EBD_MASK = CONFIG::HAS_HID0_EBD ? ((1UL << CONFIG::HID0_EBD_BITSIZE) - 1) << CONFIG::HID0_EBD_OFFSET : 0;
	static const uint32_t HID0_SBCLK_MASK = CONFIG::HAS_HID0_SBCLK ? ((1UL << CONFIG::HID0_SBCLK_BITSIZE) - 1) << CONFIG::HID0_SBCLK_OFFSET : 0;
	static const uint32_t HID0_BCLK_MASK = CONFIG::HAS_HID0_BCLK ? ((1UL << CONFIG::HID0_BCLK_BITSIZE) - 1) << CONFIG::HID0_BCLK_OFFSET : 0;
	static const uint32_t HID0_TBEN_MASK = CONFIG::HAS_HID0_TBEN ? ((1UL << CONFIG::HID0_TBEN_BITSIZE) - 1) << CONFIG::HID0_TBEN_OFFSET : 0;
	static const uint32_t HID0_EICE_MASK = CONFIG::HAS_HID0_EICE ? ((1UL << CONFIG::HID0_EICE_BITSIZE) - 1) << CONFIG::HID0_EICE_OFFSET : 0;
	static const uint32_t HID0_ECLK_MASK = CONFIG::HAS_HID0_ECLK ? ((1UL << CONFIG::HID0_ECLK_BITSIZE) - 1) << CONFIG::HID0_ECLK_OFFSET : 0;
	static const uint32_t HID0_PAR_MASK = CONFIG::HAS_HID0_PAR ? ((1UL << CONFIG::HID0_PAR_BITSIZE) - 1) << CONFIG::HID0_PAR_OFFSET : 0;
	static const uint32_t HID0_STEN_MASK = CONFIG::HAS_HID0_STEN ? ((1UL << CONFIG::HID0_STEN_BITSIZE) - 1) << CONFIG::HID0_STEN_OFFSET : 0;
	static const uint32_t HID0_DOZE_MASK = CONFIG::HAS_HID0_DOZE ? ((1UL << CONFIG::HID0_DOZE_BITSIZE) - 1) << CONFIG::HID0_DOZE_OFFSET : 0;
	static const uint32_t HID0_NAP_MASK = CONFIG::HAS_HID0_NAP ? ((1UL << CONFIG::HID0_NAP_BITSIZE) - 1) << CONFIG::HID0_NAP_OFFSET : 0;
	static const uint32_t HID0_SLEEP_MASK = CONFIG::HAS_HID0_SLEEP ? ((1UL << CONFIG::HID0_SLEEP_BITSIZE) - 1) << CONFIG::HID0_SLEEP_OFFSET : 0;
	static const uint32_t HID0_DPM_MASK = CONFIG::HAS_HID0_DPM ? ((1UL << CONFIG::HID0_DPM_BITSIZE) - 1) << CONFIG::HID0_DPM_OFFSET : 0;
	static const uint32_t HID0_RISEG_MASK = CONFIG::HAS_HID0_RISEG ? ((1UL << CONFIG::HID0_RISEG_BITSIZE) - 1) << CONFIG::HID0_RISEG_OFFSET : 0;
	static const uint32_t HID0_BHTCLR_MASK = CONFIG::HAS_HID0_BHTCLR ? ((1UL << CONFIG::HID0_BHTCLR_BITSIZE) - 1) << CONFIG::HID0_BHTCLR_OFFSET : 0;
	static const uint32_t HID0_EIEC_MASK = CONFIG::HAS_HID0_EIEC ? ((1UL << CONFIG::HID0_EIEC_BITSIZE) - 1) << CONFIG::HID0_EIEC_OFFSET : 0;
	static const uint32_t HID0_XAEN_MASK = CONFIG::HAS_HID0_XAEN ? ((1UL << CONFIG::HID0_XAEN_BITSIZE) - 1) << CONFIG::HID0_XAEN_OFFSET : 0;
	static const uint32_t HID0_NHR_MASK = CONFIG::HAS_HID0_NHR ? ((1UL << CONFIG::HID0_NHR_BITSIZE) - 1) << CONFIG::HID0_NHR_OFFSET : 0;
	static const uint32_t HID0_ICE_MASK = CONFIG::HAS_HID0_ICE ? ((1UL << CONFIG::HID0_ICE_BITSIZE) - 1) << CONFIG::HID0_ICE_OFFSET : 0;
	static const uint32_t HID0_DCE_MASK = CONFIG::HAS_HID0_DCE ? ((1UL << CONFIG::HID0_DCE_BITSIZE) - 1) << CONFIG::HID0_DCE_OFFSET : 0;
	static const uint32_t HID0_ILOCK_MASK = CONFIG::HAS_HID0_ILOCK ? ((1UL << CONFIG::HID0_ILOCK_BITSIZE) - 1) << CONFIG::HID0_ILOCK_OFFSET : 0;
	static const uint32_t HID0_DLOCK_MASK = CONFIG::HAS_HID0_DLOCK ? ((1UL << CONFIG::HID0_DLOCK_BITSIZE) - 1) << CONFIG::HID0_DLOCK_OFFSET : 0;
	static const uint32_t HID0_ICFI_MASK = CONFIG::HAS_HID0_ICFI ? ((1UL << CONFIG::HID0_ICFI_BITSIZE) - 1) << CONFIG::HID0_ICFI_OFFSET : 0;
	static const uint32_t HID0_DCFI_MASK = CONFIG::HAS_HID0_DCFI ? ((1UL << CONFIG::HID0_DCFI_BITSIZE) - 1) << CONFIG::HID0_DCFI_OFFSET : 0;
	static const uint32_t HID0_SPD_MASK = CONFIG::HAS_HID0_SPD ? ((1UL << CONFIG::HID0_SPD_BITSIZE) - 1) << CONFIG::HID0_SPD_OFFSET : 0;
	static const uint32_t HID0_IFEM_MASK = CONFIG::HAS_HID0_IFEM ? ((1UL << CONFIG::HID0_IFEM_BITSIZE) - 1) << CONFIG::HID0_IFEM_OFFSET : 0;
	static const uint32_t HID0_IFTT_MASK = CONFIG::HAS_HID0_IFTT ? ((1UL << CONFIG::HID0_IFTT_BITSIZE) - 1) << CONFIG::HID0_IFTT_OFFSET : 0;
	static const uint32_t HID0_XBSEN_MASK = CONFIG::HAS_HID0_XBSEN ? ((1UL << CONFIG::HID0_XBSEN_BITSIZE) - 1) << CONFIG::HID0_XBSEN_OFFSET : 0;
	static const uint32_t HID0_SIED_MASK = CONFIG::HAS_HID0_SIED ? ((1UL << CONFIG::HID0_SIED_BITSIZE) - 1) << CONFIG::HID0_SIED_OFFSET : 0;
	static const uint32_t HID0_SGE_MASK = CONFIG::HAS_HID0_SGE ? ((1UL << CONFIG::HID0_SGE_BITSIZE) - 1) << CONFIG::HID0_SGE_OFFSET : 0;
	static const uint32_t HID0_DCFA_MASK = CONFIG::HAS_HID0_DCFA ? ((1UL << CONFIG::HID0_DCFA_BITSIZE) - 1) << CONFIG::HID0_DCFA_OFFSET : 0;
	static const uint32_t HID0_BTIC_MASK = CONFIG::HAS_HID0_BTIC ? ((1UL << CONFIG::HID0_BTIC_BITSIZE) - 1) << CONFIG::HID0_BTIC_OFFSET : 0;
	static const uint32_t HID0_LRSTK_MASK = CONFIG::HAS_HID0_LRSTK ? ((1UL << CONFIG::HID0_LRSTK_BITSIZE) - 1) << CONFIG::HID0_LRSTK_OFFSET : 0;
	static const uint32_t HID0_ABE_MASK = CONFIG::HAS_HID0_ABE ? ((1UL << CONFIG::HID0_ABE_BITSIZE) - 1) << CONFIG::HID0_ABE_OFFSET : 0;
	static const uint32_t HID0_FOLD_MASK = CONFIG::HAS_HID0_FOLD ? ((1UL << CONFIG::HID0_FOLD_BITSIZE) - 1) << CONFIG::HID0_FOLD_OFFSET : 0;
	static const uint32_t HID0_BHT_MASK = CONFIG::HAS_HID0_BHT ? ((1UL << CONFIG::HID0_BHT_BITSIZE) - 1) << CONFIG::HID0_BHT_OFFSET : 0;
	static const uint32_t HID0_NOPDST_MASK = CONFIG::HAS_HID0_NOPDST ? ((1UL << CONFIG::HID0_NOPDST_BITSIZE) - 1) << CONFIG::HID0_NOPDST_OFFSET : 0;
	static const uint32_t HID0_NOPTI_MASK = CONFIG::HAS_HID0_NOPTI ? ((1UL << CONFIG::HID0_NOPTI_BITSIZE) - 1) << CONFIG::HID0_NOPTI_OFFSET : 0;

	static const uint32_t HID0_MASK = HID0_EMCP_MASK | HID0_DBP_MASK | HID0_EBA_MASK | HID0_EBD_MASK | HID0_SBCLK_MASK | HID0_BCLK_MASK | HID0_TBEN_MASK |
	                                  HID0_EICE_MASK | HID0_ECLK_MASK | HID0_PAR_MASK | HID0_STEN_MASK | HID0_DOZE_MASK | HID0_NAP_MASK | HID0_SLEEP_MASK |
	                                  HID0_DPM_MASK | HID0_RISEG_MASK | HID0_BHTCLR_MASK | HID0_EIEC_MASK | HID0_XAEN_MASK | HID0_NHR_MASK | HID0_ICE_MASK |
	                                  HID0_DCE_MASK | HID0_ILOCK_MASK | HID0_DLOCK_MASK | HID0_ICFI_MASK | HID0_DCFI_MASK | HID0_SPD_MASK | HID0_IFEM_MASK |
	                                  HID0_IFTT_MASK | HID0_XBSEN_MASK | HID0_SIED_MASK | HID0_SGE_MASK | HID0_DCFA_MASK | HID0_BTIC_MASK | HID0_LRSTK_MASK |
	                                  HID0_ABE_MASK | HID0_FOLD_MASK | HID0_BHT_MASK | HID0_NOPDST_MASK | HID0_NOPTI_MASK;

	static const uint32_t HID1_PC0_MASK = CONFIG::HAS_HID1_PC0 ? ((1UL << CONFIG::HID1_PC0_BITSIZE) - 1) << CONFIG::HID1_PC0_OFFSET : 0;
	static const uint32_t HID1_PC1_MASK = CONFIG::HAS_HID1_PC1 ? ((1UL << CONFIG::HID1_PC1_BITSIZE) - 1) << CONFIG::HID1_PC1_OFFSET : 0;
	static const uint32_t HID1_PC2_MASK = CONFIG::HAS_HID1_PC2 ? ((1UL << CONFIG::HID1_PC2_BITSIZE) - 1) << CONFIG::HID1_PC2_OFFSET : 0;
	static const uint32_t HID1_PC3_MASK = CONFIG::HAS_HID1_PC3 ? ((1UL << CONFIG::HID1_PC3_BITSIZE) - 1) << CONFIG::HID1_PC3_OFFSET : 0;
	static const uint32_t HID1_PC4_MASK = CONFIG::HAS_HID1_PC4 ? ((1UL << CONFIG::HID1_PC4_BITSIZE) - 1) << CONFIG::HID1_PC4_OFFSET : 0;
	static const uint32_t HID1_PC5_MASK = CONFIG::HAS_HID1_PC5 ? ((1UL << CONFIG::HID1_PC5_BITSIZE) - 1) << CONFIG::HID1_PC5_OFFSET : 0;
	static const uint32_t HID1_EMCP_MASK = CONFIG::HAS_HID1_EMCP ? ((1UL << CONFIG::HID1_EMCP_BITSIZE) - 1) << CONFIG::HID1_EMCP_OFFSET : 0;
	static const uint32_t HID1_EBA_MASK = CONFIG::HAS_HID1_EBA ? ((1UL << CONFIG::HID1_EBA_BITSIZE) - 1) << CONFIG::HID1_EBA_OFFSET : 0;
	static const uint32_t HID1_EBD_MASK = CONFIG::HAS_HID1_EBD ? ((1UL << CONFIG::HID1_EBD_BITSIZE) - 1) << CONFIG::HID1_EBD_OFFSET : 0;
	static const uint32_t HID1_BCLK_MASK = CONFIG::HAS_HID1_BCLK ? ((1UL << CONFIG::HID1_BCLK_BITSIZE) - 1) << CONFIG::HID1_BCLK_OFFSET : 0;
	static const uint32_t HID1_ECLK_MASK = CONFIG::HAS_HID1_ECLK ? ((1UL << CONFIG::HID1_ECLK_BITSIZE) - 1) << CONFIG::HID1_ECLK_OFFSET : 0;
	static const uint32_t HID1_PAR_MASK = CONFIG::HAS_HID1_PAR ? ((1UL << CONFIG::HID1_PAR_BITSIZE) - 1) << CONFIG::HID1_PAR_OFFSET : 0;
	static const uint32_t HID1_DFS4_MASK = CONFIG::HAS_HID1_DFS4 ? ((1UL << CONFIG::HID1_DFS4_BITSIZE) - 1) << CONFIG::HID1_DFS4_OFFSET : 0;
	static const uint32_t HID1_DFS2_MASK = CONFIG::HAS_HID1_DFS2 ? ((1UL << CONFIG::HID1_DFS2_BITSIZE) - 1) << CONFIG::HID1_DFS2_OFFSET : 0;
	static const uint32_t HID1_SYNCBE_MASK = CONFIG::HAS_HID1_SYNCBE ? ((1UL << CONFIG::HID1_SYNCBE_BITSIZE) - 1) << CONFIG::HID1_SYNCBE_OFFSET : 0;
	static const uint32_t HID1_ABE_MASK = CONFIG::HAS_HID1_ABE ? ((1UL << CONFIG::HID1_ABE_BITSIZE) - 1) << CONFIG::HID1_ABE_OFFSET : 0;

	static const uint32_t HID1_MASK = HID1_PC0_MASK | HID1_PC1_MASK | HID1_PC2_MASK | HID1_PC3_MASK | HID1_PC4_MASK | HID1_PC5_MASK | HID1_EMCP_MASK |
	                                  HID1_EBA_MASK | HID1_EBD_MASK | HID1_BCLK_MASK | HID1_ECLK_MASK | HID1_PAR_MASK | HID1_DFS4_MASK | HID1_DFS2_MASK |
	                                  HID1_SYNCBE_MASK | HID1_ABE_MASK;

	static const uint32_t HID2_L2AP_EN_MASK = CONFIG::HAS_HID2_L2AP_EN ? ((1UL << CONFIG::HID2_L2AP_EN_BITSIZE) - 1) << CONFIG::HID2_L2AP_EN_OFFSET : 0;
	static const uint32_t HID2_SWT_EN_MASK = CONFIG::HAS_HID2_SWT_EN ? ((1UL << CONFIG::HID2_SWT_EN_BITSIZE) - 1) << CONFIG::HID2_SWT_EN_OFFSET : 0;
	static const uint32_t HID2_HIGH_BAT_EN_MASK = CONFIG::HAS_HID2_HIGH_BAT_EN ? ((1UL << CONFIG::HID2_HIGH_BAT_EN_BITSIZE) - 1) << CONFIG::HID2_HIGH_BAT_EN_OFFSET : 0;
	static const uint32_t HID2_IWLCK_MASK = CONFIG::HAS_HID2_IWLCK ? ((1UL << CONFIG::HID2_IWLCK_BITSIZE) - 1) << CONFIG::HID2_IWLCK_OFFSET : 0;
	static const uint32_t HID2_DWLCK_MASK = CONFIG::HAS_HID2_DWLCK ? ((1UL << CONFIG::HID2_DWLCK_BITSIZE) - 1) << CONFIG::HID2_DWLCK_OFFSET : 0;

	static const uint32_t HID2_MASK = HID2_L2AP_EN_MASK | HID2_SWT_EN_MASK | HID2_HIGH_BAT_EN_MASK | HID2_IWLCK_MASK | HID2_DWLCK_MASK;

	static const uint32_t L2CR_L2E_MASK = CONFIG::HAS_L2CR_L2E ? ((1UL << CONFIG::L2CR_L2E_BITSIZE) - 1) << CONFIG::L2CR_L2E_OFFSET : 0;
	static const uint32_t L2CR_L2PE_MASK = CONFIG::HAS_L2CR_L2PE ? ((1UL << CONFIG::L2CR_L2PE_BITSIZE) - 1) << CONFIG::L2CR_L2PE_OFFSET : 0;
	static const uint32_t L2CR_L2SIZ_MASK = CONFIG::HAS_L2CR_L2SIZ ? ((1UL << CONFIG::L2CR_L2SIZ_BITSIZE) - 1) << CONFIG::L2CR_L2SIZ_OFFSET : 0;
	static const uint32_t L2CR_L2CLK_MASK = CONFIG::HAS_L2CR_L2CLK ? ((1UL << CONFIG::L2CR_L2CLK_BITSIZE) - 1) << CONFIG::L2CR_L2CLK_OFFSET : 0;
	static const uint32_t L2CR_L2RAM_MASK = CONFIG::HAS_L2CR_L2RAM ? ((1UL << CONFIG::L2CR_L2RAM_BITSIZE) - 1) << CONFIG::L2CR_L2RAM_OFFSET : 0;
	static const uint32_t L2CR_L2DR_MASK = CONFIG::HAS_L2CR_L2DR ? ((1UL << CONFIG::L2CR_L2DR_BITSIZE) - 1) << CONFIG::L2CR_L2DR_OFFSET : 0;
	static const uint32_t L2CR_L2DO_MASK = CONFIG::HAS_L2CR_L2DO ? ((1UL << CONFIG::L2CR_L2DO_BITSIZE) - 1) << CONFIG::L2CR_L2DO_OFFSET : 0;
	static const uint32_t L2CR_L2I_MASK = CONFIG::HAS_L2CR_L2I ? ((1UL << CONFIG::L2CR_L2I_BITSIZE) - 1) << CONFIG::L2CR_L2I_OFFSET : 0;
	static const uint32_t L2CR_L2CTL_MASK = CONFIG::HAS_L2CR_L2CTL ? ((1UL << CONFIG::L2CR_L2CTL_BITSIZE) - 1) << CONFIG::L2CR_L2CTL_OFFSET : 0;
	static const uint32_t L2CR_L2WT_MASK = CONFIG::HAS_L2CR_L2WT ? ((1UL << CONFIG::L2CR_L2WT_BITSIZE) - 1) << CONFIG::L2CR_L2WT_OFFSET : 0;
	static const uint32_t L2CR_L2TS_MASK = CONFIG::HAS_L2CR_L2TS ? ((1UL << CONFIG::L2CR_L2TS_BITSIZE) - 1) << CONFIG::L2CR_L2TS_OFFSET : 0;
	static const uint32_t L2CR_L2OH_MASK = CONFIG::HAS_L2CR_L2OH ? ((1UL << CONFIG::L2CR_L2OH_BITSIZE) - 1) << CONFIG::L2CR_L2OH_OFFSET : 0;
	static const uint32_t L2CR_L2SL_MASK = CONFIG::HAS_L2CR_L2SL ? ((1UL << CONFIG::L2CR_L2SL_BITSIZE) - 1) << CONFIG::L2CR_L2SL_OFFSET : 0;
	static const uint32_t L2CR_L2DF_MASK = CONFIG::HAS_L2CR_L2DF ? ((1UL << CONFIG::L2CR_L2DF_BITSIZE) - 1) << CONFIG::L2CR_L2DF_OFFSET : 0;
	static const uint32_t L2CR_L2BYP_MASK = CONFIG::HAS_L2CR_L2BYP ? ((1UL << CONFIG::L2CR_L2BYP_BITSIZE) - 1) << CONFIG::L2CR_L2BYP_OFFSET : 0;
	static const uint32_t L2CR_L2FA_MASK = CONFIG::HAS_L2CR_L2FA ? ((1UL << CONFIG::L2CR_L2FA_BITSIZE) - 1) << CONFIG::L2CR_L2FA_OFFSET : 0;
	static const uint32_t L2CR_L2REP_MASK = CONFIG::HAS_L2CR_L2REP ? ((1UL << CONFIG::L2CR_L2REP_BITSIZE) - 1) << CONFIG::L2CR_L2REP_OFFSET : 0;
	static const uint32_t L2CR_L2HWF_MASK = CONFIG::HAS_L2CR_L2HWF ? ((1UL << CONFIG::L2CR_L2HWF_BITSIZE) - 1) << CONFIG::L2CR_L2HWF_OFFSET : 0;
	static const uint32_t L2CR_L2IO_MASK = CONFIG::HAS_L2CR_L2IO ? ((1UL << CONFIG::L2CR_L2IO_BITSIZE) - 1) << CONFIG::L2CR_L2IO_OFFSET : 0;
	static const uint32_t L2CR_L2CLKSTP_MASK = CONFIG::HAS_L2CR_L2CLKSTP ? ((1UL << CONFIG::L2CR_L2CLKSTP_BITSIZE) - 1) << CONFIG::L2CR_L2CLKSTP_OFFSET : 0;
	static const uint32_t L2CR_L2DRO_MASK = CONFIG::HAS_L2CR_L2DRO ? ((1UL << CONFIG::L2CR_L2DRO_BITSIZE) - 1) << CONFIG::L2CR_L2DRO_OFFSET : 0;
	static const uint32_t L2CR_L2IP_MASK = CONFIG::HAS_L2CR_L2IP ? ((1UL << CONFIG::L2CR_L2IP_BITSIZE) - 1) << CONFIG::L2CR_L2IP_OFFSET : 0;
	static const uint32_t L2CR_LVRAME_MASK = CONFIG::HAS_L2CR_LVRAME? ((1UL << CONFIG::L2CR_LVRAME_BITSIZE) - 1) << CONFIG::L2CR_LVRAME_OFFSET : 0;
	static const uint32_t L2CR_LVRAMM_MASK = CONFIG::HAS_L2CR_LVRAMM ? ((1UL << CONFIG::L2CR_LVRAMM_BITSIZE) - 1) << CONFIG::L2CR_LVRAMM_OFFSET : 0;


	static const uint32_t ICTRL_CIRQ_MASK = CONFIG::HAS_ICTRL_CIRQ ? ((1UL << CONFIG::ICTRL_CIRQ_BITSIZE) - 1) << CONFIG::ICTRL_CIRQ_OFFSET : 0;
	static const uint32_t ICTRL_EIEC_MASK = CONFIG::HAS_ICTRL_EIEC ? ((1UL << CONFIG::ICTRL_EIEC_BITSIZE) - 1) << CONFIG::ICTRL_EIEC_OFFSET : 0;
	static const uint32_t ICTRL_EDCE_MASK = CONFIG::HAS_ICTRL_EDCE ? ((1UL << CONFIG::ICTRL_EDCE_BITSIZE) - 1) << CONFIG::ICTRL_EDCE_OFFSET : 0;
	static const uint32_t ICTRL_EICP_MASK = CONFIG::HAS_ICTRL_EICP ? ((1UL << CONFIG::ICTRL_EICP_BITSIZE) - 1) << CONFIG::ICTRL_EICP_OFFSET : 0;
	static const uint32_t ICTRL_ICWL_MASK = CONFIG::HAS_ICTRL_ICWL ? ((1UL << CONFIG::ICTRL_ICWL_BITSIZE) - 1) << CONFIG::ICTRL_ICWL_OFFSET : 0;

	static const uint32_t ICTRL_MASK = ICTRL_CIRQ_MASK | ICTRL_EIEC_MASK | ICTRL_EDCE_MASK | ICTRL_EICP_MASK | ICTRL_ICWL_MASK;

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<address_t> > disasm_export;
	ServiceExport<unisim::service::interfaces::Registers> registers_export;
	ServiceExport<Memory<address_t> > memory_export;
	ServiceExport<MemoryInjection<address_t> > memory_injection_export;
	ServiceExport<CPULinuxOS> cpu_linux_os_export;
	ServiceExport<Synchronizable> synchronizable_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<Loader<physical_address_t> > kernel_loader_import;
	ServiceImport<DebugControl<address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceImport<LinuxOS> linux_os_import;
	ServiceImport<Logger> logger_import;
	ServiceImport<Logger> fpu_logger_import;
	ServiceImport<Logger> mmu_logger_import;
	ServiceImport<CachePowerEstimator> il1_power_estimator_import;
	ServiceImport<CachePowerEstimator> dl1_power_estimator_import;
	ServiceImport<CachePowerEstimator> l2_power_estimator_import;
	ServiceImport<CachePowerEstimator> itlb_power_estimator_import;
	ServiceImport<CachePowerEstimator> dtlb_power_estimator_import;
	ServiceImport<PowerMode> il1_power_mode_import;
	ServiceImport<PowerMode> dl1_power_mode_import;
	ServiceImport<PowerMode> l2_power_mode_import;
	ServiceImport<PowerMode> dtlb_power_mode_import;
	ServiceImport<PowerMode> itlb_power_mode_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, BusInterface<physical_address_t> *bus_interface, Object *parent = 0);
	virtual ~CPU();
	
	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool Setup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================
	
	void Step();
	void Run();
	virtual void Stop(int ret);
	virtual void Synchronize();
	virtual void Reset();

	//=====================================================================
	//=             instruction fetch (unused, but mandatory)             =
	//=====================================================================
	
	virtual void Fetch(void *buffer, address_t addr, uint32_t size); // called by inherited class Decoder

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report) ;

	//=====================================================================
	//=                        Interface with outside                     =
	//=====================================================================
	
	virtual BusStatus DevRead(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc);
	virtual BusStatus DevReadX(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc);
	virtual BusStatus DevWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, BusControl bc);
	virtual BusStatus DevInvalidateBlock(physical_address_t physical_addr, BusControl bc);
	virtual BusStatus DevFlushBlock(physical_address_t physical_addr, BusControl bc);
	virtual BusStatus DevZeroBlock(physical_address_t physical_addr, BusControl bc);
	virtual BusStatus DevInvalidateTLBEntry(physical_address_t addr, BusControl bc);
	virtual void DevOnBusError(BusStatus bs);

	// PowerPC Linux OS Interface
	virtual void PerformExit(int ret);

	// Endian interface
    virtual endian_type GetEndianess();
	
	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	void ReadMemory8(address_t addr, uint8_t& value);
	void ReadMemory16(address_t addr, uint16_t& value);
	void ReadMemoryByteReverse16(address_t addr, uint16_t& value);
	void ReadMemory32(address_t addr, uint32_t& value);
	void ReadMemoryByteReverse32(address_t addr, uint32_t& value);
	void ReadMemory64(address_t addr, uint64_t& value);
	void WriteMemory8(address_t addr, uint8_t value);
	void WriteMemory16(address_t addr, uint16_t value);
	void WriteMemoryByteReverse16(address_t addr, uint16_t value);
	void WriteMemory32(address_t addr, uint32_t value);
	void WriteMemoryByteReverse32(address_t addr, uint32_t value);
	void WriteMemory64(address_t addr, uint64_t value);
	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size);
	void ReadMemoryBuffer(address_t addr, void *buffer, uint32_t size);
	void WriteMemoryBuffer(address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=                         utility methods                           =
	//=====================================================================
	
	static uint32_t Mask(uint32_t mb, uint32_t me)
	{
		return (mb > me) ? ~(((uint32_t) -1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1))) : (((uint32_t)-1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1)));
	}
	
	static uint32_t CRFieldMask(uint32_t crfD)
	{
		return (crfD != 7 ? 0xFFFFFFFFul >> ((crfD + 1) * 4) : 0) | (crfD != 0 ? (0xFFFFFFFFul << (8 - crfD) * 4) : 0);
	}

	//=====================================================================
	//=           processor version register set/get methods              =
	//=====================================================================
	
	inline void SetPVR(uint32_t value) { pvr = value; }
	inline uint32_t GetPVR() { return pvr; }
	
	//=====================================================================
	//=                    user level registers set/get methods           =
	//=====================================================================
	
	inline uint32_t GetGPR(unsigned int n) { return gpr[n]; }
	inline void SetGPR(unsigned int n, uint32_t value) { gpr[n] = value; }
	inline uint32_t GetLR() { return lr; }
	inline void SetLR(uint32_t value) { lr =  value; }
	inline uint32_t GetCTR() { return ctr; }
	inline void SetCTR(uint32_t value) { ctr = value; }
	inline uint32_t GetMQ() { return mq; }
	inline void SetMQ(uint32_t value) { mq = value; }	
	inline uint32_t GetRTCU() { return rtcu; }
	inline void SetRTCU(uint32_t value) { rtcu = value; }	
	inline uint32_t GetRTCL() { return rtcl; }
	inline void SetRTCL(uint32_t value) { rtcl = value; }	
	inline uint32_t GetVRSAVE() { return vrsave; }
	inline void SetVRSAVE(uint32_t value) { vrsave = value; }	
	
	//=====================================================================
	//=                  instruction address set/get methods              =
	//=====================================================================
	
	inline uint32_t GetCIA() { return cia; }
	inline void SetCIA(uint32_t value) { cia = value; }
	inline uint32_t GetNIA() { return nia; }
	inline void SetNIA(uint32_t value) { nia = value; }

	//=====================================================================
	//=            external address register set/get methods              =
	//=====================================================================
	
	inline void SetEAR(uint32_t value) { ear = value; }
	inline uint32_t GetEAR() { return ear; }
	
	//=====================================================================
	//=           special purpose registers set/get methods               =
	//=====================================================================

	uint32_t GetSPR(unsigned int n);
	void SetSPR(unsigned int n, uint32_t value);
	
	//=====================================================================
	//=          exception handling registers set/get methods             =
	//=====================================================================
	
	inline uint32_t GetSPRG(unsigned int n) { return sprg[n]; }
	inline void SetSPRG(unsigned int n, uint32_t value) { sprg[n] = value; }
	inline uint32_t GetSRR0() { return srr0; }
	inline void SetSRR0(uint32_t value) { srr0 = value; }
	inline uint32_t GetSRR1() { return srr1; }
	inline void SetSRR1(uint32_t value) { srr1 = value; }
	inline uint32_t GetSRR1_WAY() { return (GetSRR1() >> 17) & 1; }
	inline void SetDAR(uint32_t value) { dar = value; }
	inline uint32_t GetDAR() { return dar; }
	inline void SetDSISR(uint32_t value) { dsisr = value; }
	uint32_t GetDSISR() { return dsisr; }
	
	//=====================================================================
	//=              DEC/TBL/TBU registers set/get methods                =
	//=====================================================================
	
	void SetDEC(uint32_t value);
	inline uint32_t GetDEC() { return dec; }
	inline uint32_t GetTBL() { return tbl; }
	inline void SetTBL(uint32_t value) { tbl = value; }
	inline uint32_t GetTBU() { return tbu; }
	inline void SetTBU(uint32_t value) { tbu = value; }

	//=====================================================================
	//=                        XER set/get methods                        =
	//=====================================================================

	inline void SetXER(uint32_t value) { xer = value & 0xe000ff7fUL; }  // reserved bits are set to zero but bits 16-23
	inline uint32_t GetXER() { return xer; }
	inline uint32_t GetXER_SO() { return (xer >> 31) & 1; }
	inline uint32_t GetXER_OV() { return (xer >> 30) & 1; }
	inline uint32_t GetXER_CA() { return (xer >> 29) & 1; }
	inline uint32_t GetXER_BYTE_COUNT() { return xer & 0x7f; }
	inline void SetXER_CA() { xer = xer | (1 << 29); }
	inline void SetXER_OV() { xer = xer | (1 << 30); }
	inline void SetXER_SO() { xer = xer | (1 << 31); }
	inline void ResetXER_CA() { xer = xer & (~(1 << 29)); }
	inline void ResetXER_OV() { xer = xer & (~(1 << 30)); }
	inline void ResetXER_SO() { xer = xer & (~(1 << 31)); }
	
	//=====================================================================
	//=                         CR set/get methods                        =
	//=====================================================================

	inline uint32_t GetCR() { return cr; }
	inline uint32_t GetCRF(unsigned int crf) { return cr >> ((7 - crf) * 4); }
	inline void SetCR(uint32_t value) { cr = value; }
	inline void SetCRF(unsigned int crfd, uint32_t value)
	{
		uint32_t _c = value << ((7 - crfd) * 4) ; // positionning the crX field
		uint32_t _d = CRFieldMask(crfd) ;
		cr = cr & _d ;	//Deleting old sub-registrer value
		cr = cr | _c ;  //Writing the new one
	}
	inline void SetCR0_LT() { cr = cr | (1 << 31); }
	inline void SetCR0_GT() { cr = cr | (1 << 30); }
	inline void SetCR0_EQ() { cr = cr | (1 << 29); }
	inline void SetCR0_SO() { cr = cr | (1 << 28); }
	inline void ResetCR0_LT() { cr = cr & (~(1 << 31)); }
	inline void ResetCR0_GT() { cr = cr & (~(1 << 30)); }
	inline void ResetCR0_EQ() { cr = cr & (~(1 << 29)); }
	inline void ResetCR0_SO() { cr = cr & (~(1 << 28)); }
	inline void SetCR_LT(unsigned int n) { cr = cr | (1 << (3 + 4 * (7 - n))); }
	inline void SetCR_GT(unsigned int n) { cr = cr | (1 << (2 + 4 * (7 - n))); }
	inline void SetCR_EQ(unsigned int n) { cr = cr | (1 << (1 + 4 * (7 - n))); }
	inline void SetCR_SO(unsigned int n) { cr = cr | (1 << (4 * (7 - n))); }
	inline void ResetCR_LT(unsigned int n) { cr = cr & (~(1 << (3 + 4 * (7 - n)))); }
	inline void ResetCR_GT(unsigned int n) { cr = cr & (~(1 << (2 + 4 * (7 - n)))); }
	inline void ResetCR_EQ(unsigned int n) { cr = cr & (~(1 << (1 + 4 * (7 - n)))); }
	inline void ResetCR_SO(unsigned int n) { cr = cr & (~(1 << (4 * (7 - n)))); }
	
	//=====================================================================
	//=                 CR0/CR1 generation methods                        =
	//=====================================================================
	
	inline void ComputeCR0(uint32_t result) { cr = (cr & 0xfffffffUL) | (GetXER_SO() ? 0x10000000UL : 0) | (((int32_t)(result) < 0) ? 0x80000000UL : (((int32_t)(result) > 0) ? 0x40000000UL : 0x20000000UL)); }
	inline void ComputeCR1() { cr = (cr & 0xf0ffffffUL) | ((GetFPSCR() >> 4) & 0x0f000000UL); }
	
	//=====================================================================
	//=                        MSR set/get methods                        =
	//=====================================================================
	
	uint32_t GetMSR() { return msr; }
	void SetMSR(uint32_t value);
	inline uint32_t GetMSR_VEC() { return (GetMSR() & MSR_VEC_MASK) >> CONFIG::MSR_VEC_OFFSET; }
	inline uint32_t GetMSR_POW() { return (GetMSR() & MSR_POW_MASK) >> CONFIG::MSR_POW_OFFSET; }
	inline uint32_t GetMSR_ILE() { return (GetMSR() & MSR_ILE_MASK) >> CONFIG::MSR_ILE_OFFSET; }
	inline uint32_t GetMSR_EE() { return (GetMSR() & MSR_EE_MASK) >> CONFIG::MSR_EE_OFFSET; }
	inline uint32_t GetMSR_PR() { return (GetMSR() & MSR_PR_MASK) >> CONFIG::MSR_PR_OFFSET; }
	inline PrivilegeLevel GetPrivilegeLevel() { return GetMSR_PR() ? PR_USER : PR_SUPERVISOR; }
	inline uint32_t GetMSR_FP() { return (GetMSR() & MSR_FP_MASK) >> CONFIG::MSR_FP_OFFSET; }
	inline uint32_t GetMSR_ME() { return (GetMSR() & MSR_ME_MASK) >> CONFIG::MSR_ME_OFFSET; }
	inline uint32_t GetMSR_FE0() { return (GetMSR() & MSR_FE0_MASK) >> CONFIG::MSR_FE0_OFFSET; }
	inline uint32_t GetMSR_SE() { return (GetMSR() & MSR_SE_MASK) >> CONFIG::MSR_SE_OFFSET; }
	inline uint32_t GetMSR_BE() { return (GetMSR() & MSR_BE_MASK) >> CONFIG::MSR_BE_OFFSET; }
	inline uint32_t GetMSR_FE1() { return (GetMSR() & MSR_FE1_MASK) >> CONFIG::MSR_FE1_OFFSET; }
	inline uint32_t GetMSR_IP() { return (GetMSR() & MSR_IP_MASK) >> CONFIG::MSR_IP_OFFSET; }
	inline uint32_t GetMSR_IR() { return (GetMSR() & MSR_IR_MASK) >> CONFIG::MSR_IR_OFFSET; }
	inline uint32_t GetMSR_DR() { return (GetMSR() & MSR_DR_MASK) >> CONFIG::MSR_DR_OFFSET; }
	inline uint32_t GetMSR_PM() { return (GetMSR() & MSR_PM_MASK) >> CONFIG::MSR_PM_OFFSET; }
	inline uint32_t GetMSR_PMM() { return GetMSR_PM(); }
	inline uint32_t GetMSR_RI() { return (GetMSR() & MSR_RI_MASK) >> CONFIG::MSR_RI_OFFSET; }
	inline uint32_t GetMSR_LE() { return (GetMSR() & MSR_LE_MASK) >> CONFIG::MSR_LE_OFFSET; }
	inline void SetMSR_VEC() { SetMSR(GetMSR() | MSR_VEC_MASK); }
	inline void SetMSR_POW() { SetMSR(GetMSR() | MSR_POW_MASK); }
	inline void SetMSR_ILE() { SetMSR(GetMSR() | MSR_ILE_MASK); }
	inline void SetMSR_EE() { SetMSR(GetMSR() | MSR_EE_MASK); }
	inline void SetMSR_PR() { SetMSR(GetMSR() | MSR_PR_MASK); }
	inline void SetMSR_FP() { SetMSR(GetMSR() | MSR_FP_MASK); }
	inline void SetMSR_ME() { SetMSR(GetMSR() | MSR_ME_MASK); }
	inline void SetMSR_FE0() { SetMSR(GetMSR() | MSR_FE0_MASK); }
	inline void SetMSR_SE() { SetMSR(GetMSR() | MSR_SE_MASK); }
	inline void SetMSR_BE() { SetMSR(GetMSR() | MSR_BE_MASK); }
	inline void SetMSR_FE1() { SetMSR(GetMSR() | MSR_FE1_MASK); }
	inline void SetMSR_IP() { SetMSR(GetMSR() | MSR_IP_MASK); }
	inline void SetMSR_IR() { SetMSR(GetMSR() | MSR_IR_MASK); }
	inline void SetMSR_DR() { SetMSR(GetMSR() | MSR_DR_MASK); }
	inline void SetMSR_PM() { SetMSR(GetMSR() | MSR_PM_MASK); }
	inline void SetMSR_PMM() { SetMSR_PM(); }
	inline void SetMSR_RI() { SetMSR(GetMSR() | MSR_RI_MASK); }
	inline void SetMSR_LE() { SetMSR(GetMSR() | MSR_LE_MASK); }
	inline void ResetMSR_VEC() { SetMSR(GetMSR() & ~MSR_VEC_MASK); }
	inline void ResetMSR_POW() { SetMSR(GetMSR() & ~MSR_POW_MASK); }
	inline void ResetMSR_ILE() { SetMSR(GetMSR() & ~MSR_ILE_MASK); }
	inline void ResetMSR_EE() { SetMSR(GetMSR() & ~MSR_EE_MASK); }
	inline void ResetMSR_PR() { SetMSR(GetMSR() & ~MSR_PR_MASK); }
	inline void ResetMSR_FP() { SetMSR(GetMSR() & ~MSR_FP_MASK); }
	inline void ResetMSR_ME() { SetMSR(GetMSR() & ~MSR_ME_MASK); }
	inline void ResetMSR_FE0() { SetMSR(GetMSR() & ~MSR_FE0_MASK); }
	inline void ResetMSR_SE() { SetMSR(GetMSR() & ~MSR_SE_MASK); }
	inline void ResetMSR_BE() { SetMSR(GetMSR() & ~MSR_BE_MASK); }
	inline void ResetMSR_FE1() { SetMSR(GetMSR() & ~MSR_FE1_MASK); }
	inline void ResetMSR_IP() { SetMSR(GetMSR() & ~MSR_IP_MASK); }
	inline void ResetMSR_IR() { SetMSR(GetMSR() & ~MSR_IR_MASK); }
	inline void ResetMSR_DR() { SetMSR(GetMSR() & ~MSR_DR_MASK); }
	inline void ResetMSR_PM() { SetMSR(GetMSR() & ~MSR_PM_MASK); }
	inline void ResetMSR_PMM() { ResetMSR_PM(); }
	inline void ResetMSR_RI() { SetMSR(GetMSR() & ~MSR_RI_MASK); }
	inline void ResetMSR_LE() { SetMSR(GetMSR() & ~MSR_LE_MASK); }

	//=====================================================================
	//=                       DABR set/get methods                        =
	//=====================================================================
	
	inline void SetDABR(uint32_t value) { dabr = value; }
	inline uint32_t GetDABR() { return dabr; }
	inline uint32_t GetDABR_DAB() { return (dabr >> 3) & 0x1fffffffUL; }
	inline uint32_t GetDABR_BT() { return (dabr >> 2) & 1; }inline uint32_t GetDABR_DW() { return (dabr >> 1) & 1; }
	inline uint32_t GetDABR_DR() { return dabr & 1; }

	//=====================================================================
	//=                   FPU registers set/get methods                   =
	//=====================================================================
	
	inline uint64_t GetFp64(unsigned int n) { return fpu.GetFp64(n); }
	inline void SetFp64(unsigned int n, uint64_t value) { fpu.SetFp64(n, value); }
	inline uint32_t GetFp32(unsigned int n) { return fpu.GetFp32(n); }
	inline void SetFp32(unsigned int n, uint32_t value) { fpu.SetFp32(n, value); }
	inline void SetFPSCR(uint32_t value) { fpu.SetFPSCR(value); }
	inline uint32_t GetFPSCR() { return fpu.GetFPSCR(); }
	inline uint32_t GetFPSCR_FX() { return fpu.GetFPSCR_FX(); }
	inline uint32_t GetFPSCR_FEX() { return  fpu.GetFPSCR_FEX(); }
	inline uint32_t GetFPSCR_VX() { return fpu.GetFPSCR_VX(); }
	inline uint32_t GetFPSCR_OX() { return fpu.GetFPSCR_OX(); }
	inline uint32_t GetFPSCR_UX() { return fpu.GetFPSCR_UX(); }
	inline uint32_t GetFPSCR_ZX() { return  fpu.GetFPSCR_ZX(); }
	inline uint32_t GetFPSCR_XX() { return fpu.GetFPSCR_XX(); }
	inline uint32_t GetFPSCR_VXSNAN() { return fpu.GetFPSCR_VXSNAN(); }
	inline uint32_t GetFPSCR_VXISI() { return fpu.GetFPSCR_VXISI(); }
	inline uint32_t GetFPSCR_VXIDI() { return fpu.GetFPSCR_VXIDI(); }
	inline uint32_t GetFPSCR_VXZDZ() { return fpu.GetFPSCR_VXZDZ(); }
	inline uint32_t GetFPSCR_VXIMZ() { return fpu.GetFPSCR_VXIMZ(); }
	inline uint32_t GetFPSCR_VXVC() { return fpu.GetFPSCR_VXVC(); }
	inline uint32_t GetFPSCR_FR() { return fpu.GetFPSCR_FR(); }
	inline uint32_t GetFPSCR_FI() { return fpu.GetFPSCR_FI(); }
	inline uint32_t GetFPSCR_FPRF() { return fpu.GetFPSCR_FPRF(); }
	inline uint32_t GetFPSCR_FPCC() { return fpu.GetFPSCR_FPCC(); }
	inline uint32_t GetFPSCR_VXSOFT() { return fpu.GetFPSCR_VXSOFT(); }
	inline uint32_t GetFPSCR_VXSQRT() { return fpu.GetFPSCR_VXSQRT(); }
	inline uint32_t GetFPSCR_VXCVI() { return fpu.GetFPSCR_VXCVI(); }
	inline uint32_t GetFPSCR_VE() { return fpu.GetFPSCR_VE(); }
	inline uint32_t GetFPSCR_OE() { return fpu.GetFPSCR_OE(); }
	inline uint32_t GetFPSCR_UE() { return fpu.GetFPSCR_UE(); }
	inline uint32_t GetFPSCR_ZE() { return fpu.GetFPSCR_ZE(); }
	inline uint32_t GetFPSCR_XE() { return fpu.GetFPSCR_XE(); }
	inline uint32_t GetFPSCR_NI() { return fpu.GetFPSCR_NI(); }
	inline uint32_t GetFPSCR_RN() { return fpu.GetFPSCR_RN(); }
	inline void SetFPSCR_FPRF(uint32_t value) { fpu.SetFPSCR_FPRF(value); }
	inline void SetFPSCR_FX() { fpu.SetFPSCR_FX(); }
	inline void SetFPSCR_FEX() { fpu.SetFPSCR_FEX(); }
	inline void SetFPSCR_VX() { fpu.SetFPSCR_VX(); }
	inline void SetFPSCR_OX() { fpu.SetFPSCR_OX(); }
	inline void SetFPSCR_UX() { fpu.SetFPSCR_UX(); }
	inline void SetFPSCR_ZX() { fpu.SetFPSCR_ZX(); }
	inline void SetFPSCR_XX() { fpu.SetFPSCR_XX(); }
	inline void SetFPSCR_VXSNAN() { fpu.SetFPSCR_VXSNAN(); }
	inline void SetFPSCR_VXISI() { fpu.SetFPSCR_VXISI(); }
	inline void SetFPSCR_VXIDI() { fpu.SetFPSCR_VXIDI(); }
	inline void SetFPSCR_VXZDZ() { fpu.SetFPSCR_VXZDZ(); }
	inline void SetFPSCR_VXIMZ() { fpu.SetFPSCR_VXIMZ(); }
	inline void SetFPSCR_VXVC() { fpu.SetFPSCR_VXVC(); }
	inline void SetFPSCR_FR() { fpu.SetFPSCR_FR();; }
	inline void SetFPSCR_FI() { fpu.SetFPSCR_FI();; }
	inline void SetFPSCR_FPCC(uint32_t c) { fpu.SetFPSCR_FPCC(c); }
	inline void SetFPSCR_VXSOFT() { fpu.SetFPSCR_VXSOFT(); }
	inline void SetFPSCR_VXSQRT() { fpu.SetFPSCR_VXSQRT(); }
	inline void SetFPSCR_VXCVI() { fpu.SetFPSCR_VXCVI(); }
	inline void SetFPSCR_VE() { fpu.SetFPSCR_VE(); }
	inline void SetFPSCR_OE() { fpu.SetFPSCR_OE(); }
	inline void SetFPSCR_UE() { fpu.SetFPSCR_UE(); }
	inline void SetFPSCR_ZE() { fpu.SetFPSCR_ZE(); }
	inline void SetFPSCR_XE() { fpu.SetFPSCR_XE(); }
	inline void SetFPSCR_NI() { fpu.SetFPSCR_NI(); }
	inline void SetFPSCR_RN(uint32_t mode) { fpu.SetFPSCR_RN(mode); }
	inline void ResetFPSCR_FX() { fpu.ResetFPSCR_FX(); }
	inline void ResetFPSCR_FEX() { fpu.ResetFPSCR_FEX(); }
	inline void ResetFPSCR_VX() { fpu.ResetFPSCR_VX(); }
	inline void ResetFPSCR_OX() { fpu.ResetFPSCR_OX(); }
	inline void ResetFPSCR_UX() { fpu.ResetFPSCR_UX(); }
	inline void ResetFPSCR_ZX() { fpu.ResetFPSCR_ZX(); }
	inline void ResetFPSCR_XX() { fpu.ResetFPSCR_XX(); }
	inline void ResetFPSCR_VXSNAN() { fpu.ResetFPSCR_VXSNAN(); }
	inline void ResetFPSCR_VXISI() { fpu.ResetFPSCR_VXISI(); }
	inline void ResetFPSCR_VXIDI() { fpu.ResetFPSCR_VXIDI(); }
	inline void ResetFPSCR_VXZDZ() { fpu.ResetFPSCR_VXZDZ(); }
	inline void ResetFPSCR_VXIMZ() { fpu.ResetFPSCR_VXIMZ(); }
	inline void ResetFPSCR_VXVC() { fpu.ResetFPSCR_VXVC(); }
	inline void ResetFPSCR_FR() { fpu.ResetFPSCR_FR(); }
	inline void ResetFPSCR_FI() { fpu.ResetFPSCR_FI(); }
	inline void ResetFPSCR_VXSOFT() { fpu.ResetFPSCR_VXSOFT(); }
	inline void ResetFPSCR_VXSQRT() { fpu.ResetFPSCR_VXSQRT(); }
	inline void ResetFPSCR_VXCVI() { fpu.ResetFPSCR_VXCVI(); }
	inline void ResetFPSCR_VE() { fpu.ResetFPSCR_VE(); }
	inline void ResetFPSCR_OE() { fpu.ResetFPSCR_OE(); }
	inline void ResetFPSCR_UE() { fpu.ResetFPSCR_UE(); }
	inline void ResetFPSCR_ZE() { fpu.ResetFPSCR_ZE(); }
	inline void ResetFPSCR_XE() { fpu.ResetFPSCR_XE(); }
	inline void ResetFPSCR_NI() { fpu.ResetFPSCR_NI(); }
	
	//=====================================================================
	//=                     FPU instructions handling                     =
	//=====================================================================
	
	inline void Fp32Add(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp32Add(fd, fa, fb); }
	inline void Fp32Sub(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp32Sub(fd, fa, fb); }
	inline void Fp32Mul(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp32Mul(fd, fa, fb); }
	inline void Fp32Div(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp32Div(fd, fa, fb); }
	inline void Fp32EstimateInv(unsigned int fd, unsigned int fa) { fpu.Fp32EstimateInv(fd, fa); }
	inline void Fp32MulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp32MulAdd(fd, fa, fb, fc); }
	inline void Fp32MulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp32MulSub(fd, fa, fb, fc); }
	inline void Fp32NegMulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp32NegMulAdd(fd, fa, fb, fc); }
	inline void Fp32NegMulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp32NegMulSub(fd, fa, fb, fc); }
	inline void Fp64Add(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp64Add(fd, fa, fb); }
	inline void Fp64Sub(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp64Sub(fd, fa, fb); }
	inline void Fp64Mul(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp64Mul(fd, fa, fb); }
	inline void Fp64Div(unsigned int fd, unsigned int fa, unsigned int fb) { fpu.Fp64Div(fd, fa, fb); }
	inline void Fp64EstimateInvSqrt(unsigned int fd, unsigned int fa) { fpu.Fp64EstimateInvSqrt(fd, fa); }
	inline void Fp64MulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp64MulAdd(fd, fa, fb, fc); }
	inline void Fp64MulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp64MulSub(fd, fa, fb, fc); }
	inline void Fp64NegMulAdd(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp64NegMulAdd(fd, fa, fb, fc); }
	inline void Fp64NegMulSub(unsigned int fd, unsigned fa, unsigned fb, unsigned fc) { fpu.Fp64NegMulSub(fd, fa, fb, fc); }
	inline void Fp64Neg(unsigned int fd, unsigned int fa) { fpu.Fp64Neg(fd, fa); }
	inline void Fp64Abs(unsigned int fd, unsigned int fa) { fpu.Fp64Abs(fd, fa); }
	inline void Fp64NegAbs(unsigned int fd, unsigned int fa) { fpu.Fp64NegAbs(fd, fa); }
	inline void Fp64CompareOrdered(unsigned int crfD, unsigned int fa, unsigned int fb) { SetCRF(crfD, fpu.Fp64CompareOrdered(fa, fb)); }
	inline void Fp64CompareUnordered(unsigned int crfD, unsigned int fa, unsigned int fb) { SetCRF(crfD, fpu.Fp64CompareUnordered(fa, fb)); }
	inline void Fp64ToInt32(unsigned int fd, unsigned int fa) { fpu.Fp64ToInt32(fd, fa); }
	inline void Fp64ToInt32TowardZero(unsigned int fd, unsigned int fa) { fpu.Fp64ToInt32TowardZero(fd, fa); }
	inline void Fp64Select(unsigned int fd, unsigned int fa, unsigned int fb, unsigned int fc) { fpu.Fp64Select(fd, fa, fb, fc); }
	inline void Fp64ToFp32(unsigned int fd, unsigned int fa) { fpu.Fp64ToFp32(fd, fa); }
	inline void Fp64Move(unsigned int fd, unsigned int fa) { fpu.Fp64Move(fd, fa); }

	//=====================================================================
	//=                      L2CR set/get methods                         =
	//=====================================================================
	
	inline uint32_t GetL2CR() { return l2cr; }
	void SetL2CR(uint32_t value);
	inline uint32_t GetL2CR_L2E() { return (GetL2CR() & L2CR_L2E_MASK) >> CONFIG::L2CR_L2E_OFFSET; }
	inline uint32_t GetL2CR_L2I() { return (GetL2CR() & L2CR_L2I_MASK) >> CONFIG::L2CR_L2I_OFFSET; }
	inline uint32_t GetL2CR_L2IP() { return (GetL2CR() & L2CR_L2IP_MASK) >> CONFIG::L2CR_L2IP_OFFSET; }
	inline void SetL2CR_L2E() { SetL2CR(GetL2CR() | L2CR_L2E_MASK); }
	inline void SetL2CR_L2I() { SetL2CR(GetL2CR() | L2CR_L2I_MASK); }
	inline void ResetL2CR_L2E() { SetL2CR(GetL2CR() & ~L2CR_L2E_MASK); }
	inline void ResetL2CR_L2I() { SetL2CR(GetL2CR() & ~L2CR_L2I_MASK); }
	inline void ResetL2CR_L2IP() { SetL2CR(GetL2CR() & ~L2CR_L2IP_MASK); }

	//=====================================================================
	//=                      IABR set/get methods                         =
	//=====================================================================
	inline uint32_t GetIABR() { return iabr; }
	inline void SetIABR(uint32_t value) { iabr = value; }
	inline uint32_t GetIABR_ADDR() { return (iabr >> 2) & 0x3fffffffUL; }
	inline uint32_t GetIABR_BE() { return (iabr >> 1) & 1; }
	inline uint32_t GetIABR_TE() { return iabr & 1; }

	inline uint32_t GetLDSTDB() { return ldstdb; }
	inline void SetLDSTDB(uint32_t value) { ldstdb = value; }
	
	//=====================================================================
	//=                THRM1/THRM2/THRM3 set/get methods                  =
	//=====================================================================
	
	inline uint32_t GetTHRM1() { return thrm1; }
	inline void SetTHRM1(uint32_t value) { thrm1 = value; }
	inline uint32_t GetTHRM2() { return thrm2; }
	inline void SetTHRM2(uint32_t value) { thrm2 = value; }
	inline uint32_t GetTHRM3() { return thrm3; }
	inline void SetTHRM3(uint32_t value) { thrm3 = value; }
	
	
	inline uint32_t GetICTC() { return ictc; }
	inline void SetICTC(uint32_t value) { ictc = value; }

	//=====================================================================
	//=         performance monitor registers set/get methods             =
	//=====================================================================
	
	inline uint32_t GetPMC1() { return pmc1; }
	inline void SetPMC1(uint32_t value) { pmc1 = value; }
	inline uint32_t GetPMC2() { return pmc2; }
	inline void SetPMC2(uint32_t value) { pmc2 = value; }
	inline uint32_t GetPMC3() { return pmc3; }
	inline void SetPMC3(uint32_t value) { pmc3 = value; }
	inline uint32_t GetPMC4() { return pmc4; }
	inline void SetPMC4(uint32_t value) { pmc4 = value; }
	inline uint32_t GetPMC5() { return pmc5; }
	inline void SetPMC5(uint32_t value) { pmc5 = value; }
	inline uint32_t GetPMC6() { return pmc6; }
	inline void SetPMC6(uint32_t value) { pmc6 = value; }
	inline uint32_t GetSIA() { return sia; }
	inline void SetSIA(uint32_t value) { sia = value; }
	inline uint32_t GetSDA() { return sda; }
	inline void SetSDA(uint32_t value) { sda = value; }
	inline uint32_t GetMMCR0() { return mmcr0; }
	inline void SetMMCR0(uint32_t value) { mmcr0 = value; }
	inline uint32_t GetMMCR1() { return mmcr1; }
	inline void SetMMCR1(uint32_t value) { mmcr1 = value; }
	inline uint32_t GetMMCR2() { return mmcr2; }
	inline void SetMMCR2(uint32_t value) { mmcr2 = value; }


	//=====================================================================
	//=                  HID0 register set/get methods                    =
	//=====================================================================

	inline uint32_t GetHID0() { return hid0; }
	void SetHID0(uint32_t value);
	inline uint32_t GetHID0_EMCP() { return (GetHID0() & HID0_EMCP_MASK) >> CONFIG::HID0_EMCP_OFFSET; }
	inline uint32_t GetHID0_DBP() { return (GetHID0() & HID0_DBP_MASK) >> CONFIG::HID0_DBP_OFFSET; }
	inline uint32_t GetHID0_EBA() { return (GetHID0() & HID0_EBA_MASK) >> CONFIG::HID0_EBA_OFFSET; }
	inline uint32_t GetHID0_EBD() { return (GetHID0() & HID0_EBD_MASK) >> CONFIG::HID0_EBD_OFFSET; }
	inline uint32_t GetHID0_SBCLK() { return (GetHID0() & HID0_SBCLK_MASK) >> CONFIG::HID0_SBCLK_OFFSET; }
	inline uint32_t GetHID0_EICE() { return (GetHID0() & HID0_EICE_MASK) >> CONFIG::HID0_EICE_OFFSET; }
	inline uint32_t GetHID0_ECLK() { return (GetHID0() & HID0_ECLK_MASK) >> CONFIG::HID0_ECLK_OFFSET; }
	inline uint32_t GetHID0_PAR() { return (GetHID0() & HID0_PAR_MASK) >> CONFIG::HID0_PAR_OFFSET; }
	inline uint32_t GetHID0_DOZE() { return (GetHID0() & HID0_DOZE_MASK) >> CONFIG::HID0_DOZE_OFFSET; }
	inline uint32_t GetHID0_NAP() { return (GetHID0() & HID0_NAP_MASK) >> CONFIG::HID0_NAP_OFFSET; }
	inline uint32_t GetHID0_SLEEP() { return (GetHID0() & HID0_SLEEP_MASK) >> CONFIG::HID0_SLEEP_OFFSET; }
	inline uint32_t GetHID0_DPM() { return (GetHID0() & HID0_DPM_MASK) >> CONFIG::HID0_DPM_OFFSET; }
	inline uint32_t GetHID0_NHR() { return (GetHID0() & HID0_NHR_MASK) >> CONFIG::HID0_NHR_OFFSET; }
	inline uint32_t GetHID0_ICE() { return (GetHID0() & HID0_ICE_MASK) >> CONFIG::HID0_ICE_OFFSET; }
	inline uint32_t GetHID0_DCE() { return (GetHID0() & HID0_DCE_MASK) >> CONFIG::HID0_DCE_OFFSET; }
	inline uint32_t GetHID0_ILOCK() { return (GetHID0() & HID0_ILOCK_MASK) >> CONFIG::HID0_ILOCK_OFFSET; }
	inline uint32_t GetHID0_DLOCK() { return (GetHID0() & HID0_DLOCK_MASK) >> CONFIG::HID0_DLOCK_OFFSET; }
	inline uint32_t GetHID0_ICFI() { return (GetHID0() & HID0_ICFI_MASK) >> CONFIG::HID0_ICFI_OFFSET; }
	inline uint32_t GetHID0_DCFI() { return (GetHID0() & HID0_DCFI_MASK) >> CONFIG::HID0_DCFI_OFFSET; }
	inline uint32_t GetHID0_SPD() { return (GetHID0() & HID0_SPD_MASK) >> CONFIG::HID0_SPD_OFFSET; }
	inline uint32_t GetHID0_SIED() { return (GetHID0() & HID0_SIED_MASK) >> CONFIG::HID0_SIED_OFFSET; }
	inline uint32_t GetHID0_SGE() { return (GetHID0() & HID0_SGE_MASK) >> CONFIG::HID0_SGE_OFFSET; }
	inline uint32_t GetHID0_BTIC() { return (GetHID0() & HID0_BTIC_MASK) >> CONFIG::HID0_BTIC_OFFSET; }
	inline uint32_t GetHID0_ABE() { return (GetHID0() & HID0_ABE_MASK) >> CONFIG::HID0_ABE_OFFSET; }
	inline void SetHID0_DCE() { SetHID0(GetHID0() | HID0_DCE_MASK); }
	inline void ResetHID0_DCE() { SetHID0(GetHID0() & ~HID0_DCE_MASK); }
	inline void SetHID0_ICE() { SetHID0(GetHID0() | HID0_ICE_MASK); }
	inline void ResetHID0_ICE() { SetHID0(GetHID0() & ~HID0_ICE_MASK); }
	inline void SetHID0_ABE() { SetHID0(GetHID0() | HID0_ABE_MASK); }
	inline void ResetHID0_ABE() { SetHID0(GetHID0() & ~HID0_ABE_MASK); }
	inline void ResetHID0_ICFI() { SetHID0(GetHID0() & ~HID0_ICFI_MASK); }
	inline void ResetHID0_DCFI() { SetHID0(GetHID0() & ~HID0_DCFI_MASK); }
	inline void SetHID0_NHR() { SetHID0(GetHID0() | HID0_NHR_MASK); }
	inline void ResetHID0_NHR() { SetHID0(GetHID0() & ~HID0_NHR_MASK); }

	//=====================================================================
	//=                  HID1 register set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetHID1() { return hid1; }
	void SetHID1(uint32_t value);
	inline uint32_t GetHID1_ABE() { return (GetHID1() & HID1_ABE_MASK) >> CONFIG::HID1_ABE_OFFSET; }
	inline uint32_t SetHID1_ABE() { SetHID1(GetHID1() | HID1_ABE_MASK); }
	inline uint32_t ResetHID1_ABE() { SetHID1(GetHID1() & ~HID1_ABE_MASK); }
	
	//=====================================================================
	//=                  HID2 register set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetHID2() { return hid2; }
	void SetHID2(uint32_t value);
	inline uint32_t GetHID2_SWT_EN() { return (GetHID2() & HID2_SWT_EN_MASK) >> CONFIG::HID2_SWT_EN_OFFSET; }
	inline uint32_t GetHID2_HIGH_BAT_EN() { return (GetHID2() & HID2_HIGH_BAT_EN_MASK) >> CONFIG::HID2_HIGH_BAT_EN_OFFSET; }
	inline void SetDMISS(uint32_t value) { dmiss = value; }
	inline uint32_t GetDMISS() { return dmiss; }
	inline void SetDCMP(uint32_t value) { dcmp = value; }
	inline uint32_t GetDCMP() { return dcmp; }
	inline void SetIMISS(uint32_t value) { imiss = value; }
	inline uint32_t GetIMISS() { return imiss; }
	inline void SetICMP(uint32_t value) { icmp = value; }
	inline uint32_t GetICMP() { return icmp; }
	inline void SetHASH1(uint32_t value) { hash1 = value; }
	inline uint32_t GetHASH1() { return hash1; }
	inline void SetHASH2(uint32_t value) { hash2 = value; }
	inline uint32_t GetHASH2() { return hash2; }
	inline void SetRPA(uint32_t value) { rpa = value; }
	inline uint32_t GetRPA() { return rpa; }
	inline void SetL2PM(uint32_t value) { l2pm = value; }
	inline uint32_t GetL2PM() { return l2pm; }
	inline void SetPTEHI(uint32_t value) { ptehi = value; }
	inline uint32_t GetPTEHI() { return ptehi; }
	inline void SetPTELO(uint32_t value) { ptelo = value; }
	inline uint32_t GetPTELO() { return ptelo; }

	//=====================================================================
	//=                  MMU registers set/get methods                    =
	//=====================================================================
	
	inline uint32_t GetSR(unsigned int n) { return mmu.GetSR(n); }
	inline void SetSR(unsigned int n, uint32_t value) { mmu.SetSR(n, value); }
	inline uint32_t GetSDR1() { return mmu.GetSDR1(); }
	inline void SetSDR1(uint32_t value) { mmu.SetSDR1(value); }
	inline uint32_t GetIBATU(unsigned int n) { return mmu.GetIBATU(n); }
	inline void SetIBATU(unsigned int n, uint32_t value) { mmu.SetIBATU(n, value); }
	inline uint32_t GetIBATL(unsigned int n) { return mmu.GetIBATL(n); }
	inline void SetIBATL(unsigned int n, uint32_t value) { mmu.SetIBATL(n, value); }
	inline uint32_t GetDBATU(unsigned int n) { return mmu.GetDBATU(n); }
	inline void SetDBATU(unsigned int n, uint32_t value) { mmu.SetDBATU(n, value); }
	inline uint32_t GetDBATL(unsigned int n) { return mmu.GetDBATL(n); }
	inline void SetDBATL(unsigned int n, uint32_t value) { mmu.SetDBATL(n, value); }

	//=====================================================================
	//=                    registers set/get methods                      =
	//=====================================================================

	inline uint32_t GetBAMR() { return bamr; }
	inline void SetBAMR(uint32_t value) { bamr = value; }

	inline uint32_t GetICTRL() { return ictrl; }
	inline void SetICTRL(uint32_t value) { ictrl = (ictrl & ~ICTRL_MASK) | (value & ICTRL_MASK); }
	inline uint32_t GetICTRL_CIRQ() { return (GetICTRL() & ICTRL_CIRQ_MASK) >> CONFIG::ICTRL_CIRQ_OFFSET; }
	inline uint32_t GetICTRL_EIEC() { return (GetICTRL() & ICTRL_EIEC_MASK) >> CONFIG::ICTRL_EIEC_OFFSET; }
	inline uint32_t GetICTRL_EDCE() { return (GetICTRL() & ICTRL_EDCE_MASK) >> CONFIG::ICTRL_EDCE_OFFSET; }
	inline uint32_t GetICTRL_EICP() { return (GetICTRL() & ICTRL_EICP_MASK) >> CONFIG::ICTRL_EICP_OFFSET; }
	inline uint32_t GetICTRL_ICWL() { return (GetICTRL() & ICTRL_ICWL_MASK) >> CONFIG::ICTRL_ICWL_OFFSET; }
	inline void SetICTRL_CIRQ() { SetICTRL(GetICTRL() | ICTRL_CIRQ_MASK); }
	inline void ResetICTRL_CIRQ() { SetICTRL(GetICTRL() & ~ICTRL_CIRQ_MASK); }
	inline void SetICTRL_EIEC() { SetICTRL(GetICTRL() | ICTRL_EIEC_MASK); }
	inline void ResetICTRL_EIEC() { SetICTRL(GetICTRL() & ~ICTRL_EIEC_MASK); }
	inline void SetICTRL_EDCE() { SetICTRL(GetICTRL() | ICTRL_EDCE_MASK); }
	inline void ResetICTRL_EDCE() { SetICTRL(GetICTRL() & ~ICTRL_EDCE_MASK); }
	inline void SetICTRL_EICP() { SetICTRL(GetICTRL() | ICTRL_EICP_MASK); }
	inline void ResetICTRL_EICP() { SetICTRL(GetICTRL() & ~ICTRL_EICP_MASK); }
	inline void SetICTRL_ICWL() { SetICTRL(GetICTRL() | ICTRL_ICWL_MASK); }
	inline void ResetICTRL_ICWL() { SetICTRL(GetICTRL() & ~ICTRL_ICWL_MASK); }

	inline uint32_t GetL2ERRINJHI() { return l2errinjhi; }
	inline void SetL2ERRINJHI(uint32_t value) { l2errinjhi = value; }
	inline uint32_t GetL2ERRINJLO() { return l2errinjlo; }
	inline void SetL2ERRINJLO(uint32_t value) { l2errinjlo = value; }
	inline uint32_t GetL2ERRINJCTL() { return l2errinjctl; }
	inline void SetL2ERRINJCTL(uint32_t value) { l2errinjctl = value; }
	inline uint32_t GetL2CAPTDATAHI() { return l2captdatahi; }
	inline void SetL2CAPTDATAHI(uint32_t value) { l2captdatahi = value; }
	inline uint32_t GetL2CAPTDATALO() { return l2captdatalo; }
	inline void SetL2CAPTDATALO(uint32_t value) { l2captdatalo = value; }
	inline uint32_t GetL2CAPTDATAECC() { return l2captdataecc; }
	inline void SetL2CAPTDATAECC(uint32_t value) { l2captdataecc = value; }
	inline uint32_t GetL2ERRDET() { return l2errdet; }
	inline void SetL2ERRDET(uint32_t value) { l2errdet = value; }
	inline uint32_t GetL2ERRDIS() { return l2errdis; }
	inline void SetL2ERRDIS(uint32_t value) { l2errdis = value; }
	inline uint32_t GetL2ERRINTEN() { return l2errinten; }
	inline void SetL2ERRINTEN(uint32_t value) { l2errinten = value; }
	inline uint32_t GetL2ERRATTR() { return l2errattr; }
	inline void SetL2ERRATTR(uint32_t value) { l2errattr = value; }
	inline uint32_t GetL2ERRADDR() { return l2erraddr; }
	inline void SetL2ERRADDR(uint32_t value) { l2erraddr = value; }
	inline uint32_t GetL2ERREADDR() { return l2erreaddr; }
	inline void SetL2ERREADDR(uint32_t value) { l2erreaddr = value; }
	inline uint32_t GetL2ERRCTL() { return l2errctl; }
	inline void SetL2ERRCTL(uint32_t value) { l2errctl = value; }
	inline uint32_t GetL3CR() { return l3cr; }
	inline void SetL3CR(uint32_t value) { l3cr = value; }
	inline uint32_t GetL3ITCR0() { return l3itcr0; }
	inline void SetL3ITCR0(uint32_t value) { l3itcr0 = value; }
	inline uint32_t GetL3ITCR1() { return l3itcr1; }
	inline void SetL3ITCR1(uint32_t value) { l3itcr1 = value; }
	inline uint32_t GetL3ITCR2() { return l3itcr2; }
	inline void SetL3ITCR2(uint32_t value) { l3itcr2 = value; }
	inline uint32_t GetL3ITCR3() { return l3itcr3; }
	inline void SetL3ITCR3(uint32_t value) { l3itcr3 = value; }
	inline uint32_t GetL3OHCR() { return l3ohcr; }
	inline void SetL3OHCR(uint32_t value) { l3ohcr = value; }
	inline uint32_t GetL3PM() { return l3pm; }
	inline void SetL3PM(uint32_t value) { l3pm = value; }
	inline uint32_t GetLDSTCR() { return ldstcr; }
	inline void SetLDSTCR(uint32_t value) { ldstcr = value; }
	inline uint32_t GetMSSCR0() { return msscr0; }
	inline void SetMSSCR0(uint32_t value) { msscr0 = value; }
	inline uint32_t GetMSSSR0() { return msssr0; }
	inline void SetMSSSR0(uint32_t value) { msssr0 = value; }
	inline uint32_t GetPIR() { return pir; }
	inline void SetPIR(uint32_t value) { pir = value; }
	inline uint32_t GetSVR() { return svr; }
	inline void SetSVR(uint32_t value) { svr = value; }
	inline uint32_t GetTLBMISS() { return tlbmiss; }
	inline void SetTLBMISS(uint32_t value) { tlbmiss = value; }

	
	//=====================================================================
	//=                   Vector instructions handling                    =
	//=====================================================================
	

	//=====================================================================
	//=               Cache management instructions handling              =
	//=====================================================================
	
	void Dcba(address_t addr);
	void Dcbf(address_t addr);
	void Dcbi(address_t addr);
	void Dcbst(address_t addr);
	void Dcbz(address_t addr);
	void Icbi(address_t addr);
	
	//=====================================================================
	//=  Translation look-aside buffers management instructions handling  =
	//=====================================================================
	
	void Tlbia();
	void Tlbie(address_t addr);
	void Tlbld(address_t addr);
	void Tlbli(address_t addr);

	//=====================================================================
	//=               Linked Load-Store instructions handling             =
	//=====================================================================
	
	void Lwarx(address_t addr, uint32_t& value);
	void Stwcx(address_t addr, uint32_t value);

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================
	
	virtual unisim::util::debug::Register *GetRegister(const char *name);
	virtual string Disasm(address_t addr, address_t& next_addr);
	virtual const char *GetArchitectureName() const;
	inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	string GetObjectFriendlyName(address_t addr);
	string GetFunctionFriendlyName(address_t addr);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================
	
	void AckDecrementerOverflow();
	void AckExternalInterrupt();
	void AckHardReset();
	void AckSoftReset();
	void AckMCP();
	void AckTEA();
	void AckSMI();
	void AckThermalManagementInterrupt();
	void AckPerformanceMonitorInterrupt();
	void AckAsynchronousInterrupt();

	//=====================================================================
	//=                    Hardware interrupt request                     =
	//=====================================================================
	
	void ReqDecrementerOverflow();
	void ReqExternalInterrupt();
	void ReqHardReset();
	void ReqSoftReset();
	void ReqMCP();
	void ReqTEA();
	void ReqSMI();
	void ReqThermalManagementInterrupt();
	void ReqPerformanceMonitorInterrupt();
	void ReqAsynchronousInterrupt();
	
	//=====================================================================
	//=          Instruction prefetch buffer handling methods             =
	//=====================================================================
	
	void FlushSubsequentInstructions();
	
	//=====================================================================
	//=          DEC/TBL/TBU bus-time based update methods                =
	//=====================================================================
	
	void OnBusCycle();

private:
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
	static const physical_address_t EXC_FLOATING_POINT_ASSIST_VECTOR = 0xe00UL;         //!< Floating point assist exception vector (none ?)

	// Additional exceptions in MPC7xx 
	static const physical_address_t EXC_PERFORMANCE_MONITOR_VECTOR = 0xf00UL;
	static const physical_address_t EXC_INSTRUCTION_ADDRESS_BREAKPOINT_VECTOR = 0x1300UL;
	static const physical_address_t EXC_SYSTEM_MANAGEMENT_INTERRUPT_VECTOR = 0x1400UL;
	static const physical_address_t EXC_THERMAL_MANAGEMENT_INTERRUPT_VECTOR = 0x1700UL; // (not in 74xx)

	// Additional exceptions in MPC7x5
	static const physical_address_t EXC_ITLB_MISS_VECTOR = 0x1000UL;
	static const physical_address_t EXC_DTLB_LOAD_MISS_VECTOR = 0x1100UL;
	static const physical_address_t EXC_DTLB_STORE_MISS_VECTOR = 0x1200UL;
	
	// Additional exceptions in MPC74xx
	static const physical_address_t EXC_ALTIVEC_UNAVAILABLE_VECTOR = 0xf20UL;

	//=====================================================================
	//=                        PowerPC registers                          =
	//=====================================================================

	uint32_t cia;            //!< Current instruction address (all)
	uint32_t nia;            //!< Next instruction address (all)
	
	// UISA
	uint32_t cr;            //!< condition register (all)
	uint32_t ctr;           //!< count register (all)
	SoftDouble fpr[32];     //!< floating point registers (C++ objects implementing IEEE 754 floating point numbers) (604e, 7xx, 7xxx)
	uint32_t fpscr;         //!< floating point status and control register (604e, 7xx, 7xxx)
	uint32_t gpr[32];       //!< general purpose registers (all)
	uint32_t lr;            //!< link register (all)
	uint32_t mq;            //!< MQ register (601)
	uint32_t rtcl;          //!< RTCL register (601)
	uint32_t rtcu;          //!< RTCU register (601)
	vr_t vr[CONFIG::NUM_VRS ? CONFIG::NUM_VRS : 1]; //!< vector registers (7xxx)
	uint32_t vrsave;        //!< vector save/restore register (7xxx)
	uint32_t vscr;          //!< vector status and control register (7xxx)
	uint32_t xer;           //!< XER register (all)

	// VEA
	uint32_t tbl;           //!< Time base register (lower 32-bits) (603e, 604e, 7xx, 7xxx)
	uint32_t tbu;           //!< Time base register (upper 32-bits) (603e, 604e, 7xx, 7xxx)

	// OEA
	uint32_t bamr;          //!< breakpoint address mask register (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t dabr;          //!< data address breakpoint register (all)
	uint32_t dar;           //!< data address register (all)
	uint32_t dec;           //!< decrementer (all)
	uint32_t dsisr;         //!< DSISR register (all)
	uint32_t ear;           //!< external access register (all)
	uint32_t hash1;         //!< Primary hash address register (603e, 7x5)
	uint32_t hash2;         //!< Secondary hash address register (603e, 7x5)
	uint32_t hid0;          //!< hardware implementation dependent register 0 (all)
	uint32_t hid1;          //!< hardware implementation dependent register 1 (all)
	uint32_t hid2;          //!< hardware implementation dependent register 2 (7x5)
	uint32_t iabr;          //!< instruction address breakpoint register (603e, 604e, 7xx, 7xxx)
	uint32_t ibatl[CONFIG::NUM_BATS ? CONFIG::NUM_BATS : 1];	//!< instruction block address translation registers (lower 32 bits) (all)
	uint32_t ibatu[CONFIG::NUM_BATS ? CONFIG::NUM_BATS : 1];	//!< instruction block address translation registers (upper 32 bits) (all)
	uint32_t icmp;          //!< ITLB Compare register (603e, 7x5)
	uint32_t dbatl[CONFIG::NUM_BATS ? CONFIG::NUM_BATS : 1];	//!< data block address translation registers (lower 32 bits)  (603e, 604e, 7xx, 7xxx)
	uint32_t dbatu[CONFIG::NUM_BATS ? CONFIG::NUM_BATS : 1];	//!< data block address translation registers (upper 32 bits) (603e, 604e, 7xx, 7xxx)
	uint32_t dcmp;          //!< DTLB Compare register (603e, 7x5)
	uint32_t dmiss;         //!< DTLB Miss register (603e, 7x5)
	uint32_t ictc;          //!< instruction cache throttling control register (7xx, 7xxx)
	uint32_t ictrl;         //!< instruction cache and interrupt control register (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t imiss;         //!< ITLB Miss register (603e, 7x5)
	uint32_t l2cr;          //!< L2 Control Register (7xx, 7xxx)
	uint32_t l2errinjhi;    //!< L2 error registers (7448)
	uint32_t l2errinjlo;    //!< L2 error registers (7448)
	uint32_t l2errinjctl;   //!< L2 error registers (7448)
	uint32_t l2captdatahi;  //!< L2 error registers (7448)
	uint32_t l2captdatalo;  //!< L2 error registers (7448)
	uint32_t l2captdataecc; //!< L2 error registers (7448)
	uint32_t l2errdet;      //!< L2 error registers (7448)
	uint32_t l2errdis;      //!< L2 error registers (7448)
	uint32_t l2errinten;    //!< L2 error registers (7448)
	uint32_t l2errattr;     //!< L2 error registers (7448)
	uint32_t l2erraddr;     //!< L2 error registers (7448)
	uint32_t l2erreaddr;    //!< L2 error registers (7448)
	uint32_t l2errctl;      //!< L2 error registers (7448)
	uint32_t l2pm;          //!< L2 private memory control register (755, 7410)
	uint32_t l3cr;          //!< L3 cache output control register (7455, 7457)
	uint32_t l3itcr0;       //!< L3 cache input timing control register 0 (7451, 7455, 7457)
	uint32_t l3itcr1;       //!< L3 cache input timing control register 1 (7457)
	uint32_t l3itcr2;       //!< L3 cache input timing control register 1 (7457)
	uint32_t l3itcr3;       //!< L3 cache input timing control register 1 (7457)
	uint32_t l3ohcr;        //!< L3 cache output hold control register (7457)
	uint32_t l3pm;          //!< L3 private memory register (7451, 7455, 7457)
	uint32_t ldstcr;        //!< load/store control register (7441, 7445, 7447, 7447A, 7448, 7451, 7455, 7457)
	uint32_t ldstdb;		//!< load/store debug register (factory-use only register)
	uint32_t mmcr0;         //!< monitor control (604e, 7xx, 7xxx)
	uint32_t mmcr1;         //!< monitor control (604e, 7xx, 7xxx)
	uint32_t mmcr2;         //!< monitor control (744x, 745x)
	uint32_t msr;           //!< Machine status register (all)
	uint32_t msscr0;        //!< Memory subsystem control register (7441, 7445, 7447, 7447A, 7448, 7451, 7455, 7457)
	uint32_t msssr0;        //!< Memory subsystem status register (7441, 7445, 7447, 7447A, 7448, 7451, 7455, 7457)
	uint32_t pir;           //!< Processor identification register (604e, 7xxx)
	uint32_t pmc1;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc2;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc3;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc4;          //!< performance counter (604e, 7xx, 7xxx)
	uint32_t pmc5;          //!< performance counter (744x, 745x)
	uint32_t pmc6;          //!< performance counter (744x, 745x)
	uint32_t ptehi;         //!< PTEHI (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t ptelo;         //!< PTELO (7445, 7447, 7447A, 7448, 7455, 7457)
	uint32_t pvr;           //!< processor version register (all)
	uint32_t rpa;           //!< Required Physical Address register (603e, 7x5)
	uint32_t sda;           //!< Sampled data address (604e, 7xx, 7xxx)
	uint32_t sdr1;          //!< SDR1 register (all)
	uint32_t sia;           //!< Sampled instruction address (604e, 7xx, 7xxx)
	uint32_t sprg[CONFIG::NUM_SPRGS ? CONFIG::NUM_SPRGS : 1];   //!< SPRGs registers (all)
	uint32_t sr[16];        //!< segment registers (all)
	uint32_t srr0;          //!< Save and restore register 0 (all)
	uint32_t srr1;          //!< Save and restore register 1 (all)
	uint32_t svr;           //!< System version register (7448)
	uint32_t thrm1;         //!< thermal assist unit register (7xx, 7400, 7410)
	uint32_t thrm2;         //!< thermal assist unit register (7xx, 7400, 7410)
	uint32_t thrm3;         //!< thermal assist unit register (7xx, 7400, 7410)
	uint32_t tlbmiss;       //!< TLBMISS register (7441, 7445, 7447, 7447A, 7448, 7451, 7455)

	//=====================================================================
	//=              PowerPC hardware interrupt signals                   =
	//=====================================================================
	
	bool decrementer_overflow;            //!< Decrementer overflow signal
	bool external_interrupt;              //!< External interrupt signal
	bool soft_reset;                      //!< Soft reset signal
	bool hard_reset;                      //!< Hard reset signal
	bool mcp;                             //!< Machine check exception signal
	bool tea;                             //!< Transfer error acknowledge signal
	bool smi;                             //!< System management interrupt signal
	bool thermal_management_interrupt;    //!< Thermal management interrupt signal
	bool performance_monitor_interrupt;   //!< Performance monitor interrupt signal
	
	bool asynchronous_interrupt;          //!< summary of all hardware interrupt signals
	

	//=====================================================================
	//=                     L1 Instruction Cache                          =
	//=====================================================================
	
	unisim::component::cxx::cache::insn::Cache<
		physical_address_t,
		CONFIG::L1_INSN_CACHE_SIZE,
		CONFIG::L1_INSN_CACHE_BLOCK_SIZE,
		CONFIG::L1_INSN_CACHE_ASSOCIATIVITY,
		CONFIG::L1_INSN_CACHE_REPLACEMENT_POLICY> il1;
		
	//=====================================================================
	//=                       L2 Unified Cache                            =
	//=====================================================================
		
	unisim::component::cxx::cache::mesi::Cache<
		physical_address_t,
		CONFIG::L2_CACHE_SIZE,
		CONFIG::L2_CACHE_BLOCK_SIZE,
		CONFIG::L2_CACHE_ASSOCIATIVITY,
		CONFIG::L2_CACHE_REPLACEMENT_POLICY> l2;
	
	
	//=====================================================================
	//=                  Floating point Unit (MMU)                        =
	//=====================================================================
	
	FPU<CONFIG> fpu;
	
	//=====================================================================
	//=    Internal service imports to adjust power mode of caches/TLBs   =
	//=====================================================================
	
	ServiceImport<PowerMode> internal_il1_power_mode_import;
	ServiceImport<PowerMode> internal_dl1_power_mode_import;
	ServiceImport<PowerMode> internal_l2_power_mode_import;
	ServiceImport<PowerMode> internal_dtlb_power_mode_import;
	ServiceImport<PowerMode> internal_itlb_power_mode_import;

	//=====================================================================
	//=               Instruction prefetch buffer                         =
	//=====================================================================
	
	unsigned int num_insn_in_prefetch_buffer;                  //!< Number of instructions currently in the prefetch buffer
	unsigned int cur_insn_in_prefetch_buffer;                  //!< Prefetch buffer index of the current instruction to be executed
	uint32_t prefetch_buffer[CONFIG::NUM_PREFETCH_BUFFER_ENTRIES];     //!< The instruction prefetch buffer
	bool icache_enabled;
	bool dcache_enabled;
	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================

	map<string, unisim::util::debug::Register *> registers_registry;       //!< Every CPU register interfaces excluding MMU/FPU registers
	string architecture_name;                                  //!< Architecture name (i.e. "powerpc")
	uint64_t instruction_counter;                              //!< Number of executed instructions
	uint64_t max_inst;                                         //!< Maximum number of instructions to execute
	int StringLength(address_t addr);                          //!< Something to compute the length of a null-terminated string at an effective address

	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_cpu_cycle_time;             //!< linked to member cpu_cycle_time
	Parameter<uint64_t> param_voltage;                    //!< linked to member voltage
	Parameter<uint64_t> param_bus_cycle_time;             //!< linked to member bus_cycle_time
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst

	//=====================================================================
	//=                   lwarx/stwcx. reservation                        =
	//=====================================================================
	
	bool reserve;                                             //!< true if a reservation has been set (by lwarx), otherwise false
	address_t reserve_addr;                                   //!< address of the reservation
	
	//=====================================================================
	//=                    Interface with outside                         =
	//=====================================================================
	
	BusInterface<physical_address_t> *bus_interface;          //!< the bus interface to get access to the outside world (bus, memory, I/O, PCI...)

	inline bool HasDecrementerOverflow() const { return decrementer_overflow; }
	inline bool HasExternalInterrupt() const { return external_interrupt; }
	inline bool HasHardReset() const { return hard_reset; }
	inline bool HasSoftReset() const { return soft_reset; }
	inline bool HasMCP() const { return mcp; }
	inline bool HasTEA() const { return tea; }
	inline bool HasThermalManagementInterrupt() const { return thermal_management_interrupt; }
	inline bool HasSMI() const { return smi; }
	inline bool HasPerformanceMonitorInterrupt() const { return performance_monitor_interrupt; }
	inline bool HasAsynchronousInterrupt() const { return asynchronous_interrupt; }

	//=====================================================================
	//=                    Exception handling methods                     =
	//=====================================================================
	
	// System reset exception
	void HandleException(const SystemResetException<CONFIG>& exc);

	// Machine check exception
	void HandleException(const MachineCheckException<CONFIG>& exc);

	// Decrementer exception
	void HandleException(const DecrementerException<CONFIG>& exc);

	// External interrupt exception
	void HandleException(const ExternalInterruptException<CONFIG>& exc);

	// Performance monitor interrupt exception
	void HandleException(const PerformanceMonitorInterruptException<CONFIG>& exc);

	// System management interrupt exception
	void HandleException(const SystemManagementInterruptException<CONFIG>& exc);

	// Thermal management interrupt exception
	void HandleException(const ThermalManagementInterruptException<CONFIG>& exc);

	// Instruction address breakpoint exception
	void HandleException(const InstructionAddressBreakpointException<CONFIG>& exc);

	// ISI exception
	void HandleException(const ISIProtectionViolationException<CONFIG>& exc);
	void HandleException(const ISINoExecuteException<CONFIG>& exc);
	void HandleException(const ISIDirectStoreException<CONFIG>& exc);
	void HandleException(const ISIPageFaultException<CONFIG>& exc);
	void HandleException(const ISIGuardedMemoryException<CONFIG>& exc);

	// DSI exception
	void HandleException(const DSIDirectStoreException<CONFIG>& exc);
	void HandleException(const DSIProtectionViolationException<CONFIG>& exc);
	void HandleException(const DSIPageFaultException<CONFIG>& exc);
	void HandleException(const DSIDataAddressBreakpointException<CONFIG>& exc);
	void HandleException(const DSIExternalAccessDisabledException<CONFIG>& exc);
	void HandleException(const DSIWriteThroughLinkedLoadStore<CONFIG>& exc);
	
	// Alignment exception
	void HandleException(const AlignmentException<CONFIG>& exc, uint32_t instruction_encoding);

	// Floating point assist exception
	void HandleFloatingPointAssistException(const FloatingPointAssistException<CONFIG>& exc);

	// Program exceptions
	void HandleException(const IllegalInstructionException<CONFIG>& exc);
	void HandleException(const PrivilegeViolationException<CONFIG>& exc);
	void HandleException(const TrapException<CONFIG>& exc);
	void HandleException(const FloatingPointException<CONFIG>& exc);

	// Floating point unavailable exception
	void HandleException(const FloatingPointUnavailableException<CONFIG>& exc);

	// System call exception
	void HandleException(const SystemCallException<CONFIG>& exc);

	// Trace exception
	void HandleException(const TraceException<CONFIG>& exc);

	// TLB exception
	void HandleException(const TLBMissException<CONFIG>& exc);

	inline bool IsMPC7XX() { return CONFIG::MODEL == MPC740 || CONFIG::MODEL == MPC745 || CONFIG::MODEL == MPC750 || CONFIG::MODEL == MPC755; }
	inline bool IsMPC74X() { return CONFIG::MODEL == MPC740 || CONFIG::MODEL == MPC745; }
	inline bool IsMPC75X() { return CONFIG::MODEL == MPC750 || CONFIG::MODEL == MPC755; }
	inline bool IsMPC7X5() { return CONFIG::MODEL == MPC745 || CONFIG::MODEL == MPC755; }

    /** indicates if the memory accesses require to be reported */
    bool requires_memory_access_reporting;
    /** indicates if the finished instructions require to be reported */
    bool requires_finished_instruction_reporting;

protected:
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================
	
	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
	uint64_t voltage;        //!< CPU voltage in mV
	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
	uint64_t bus_cycle;      //!< Number of front side bus cycles
	
	//=====================================================================
	//=                 Memory Management Unit (MMU)                      =
	//=====================================================================
		
	MMU<CONFIG> mmu;

	//=====================================================================
	//=                         L1 Data Cache                             =
	//=====================================================================
	
	unisim::component::cxx::cache::mesi::Cache<
		physical_address_t,
		CONFIG::L1_DATA_CACHE_SIZE,
		CONFIG::L1_DATA_CACHE_BLOCK_SIZE,
		CONFIG::L1_DATA_CACHE_ASSOCIATIVITY,
		CONFIG::L1_DATA_CACHE_REPLACEMENT_POLICY> dl1;
		
	
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
