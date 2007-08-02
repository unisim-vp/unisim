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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__

#include <unisim/component/c++/processor/powerpc/exception.tcc>
#include <unisim/component/c++/cache/mesi/cache.tcc>
#include <unisim/component/c++/cache/insn/cache.tcc>

#include <sstream>
#include <stdexcept>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using namespace std;
using unisim::component::cxx::cache::BC_GLOBAL;
using unisim::component::cxx::cache::BC_NONE;
using unisim::component::cxx::cache::BS_OK;
using unisim::component::cxx::cache::BS_ERROR;
using unisim::component::cxx::cache::CS_MISS;
using unisim::component::cxx::cache::CacheStatus;



template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, BusInterface<physical_address_t> *bus_interface, Object *parent) :
	Object(name, parent),
	Client<Loader<typename CONFIG::physical_address_t> >(name, parent),
	Client<SymbolTableLookup<typename CONFIG::address_t> >(name, parent),
	Client<DebugControl<typename CONFIG::address_t> >(name, parent),
	Client<MemoryAccessReporting<typename CONFIG::address_t> >(name, parent),
	Service<Disassembly<typename CONFIG::address_t> >(name, parent),
	Service<unisim::service::interfaces::Registers>(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Service<CPULinuxOS>(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
	Client<LinuxOS>(name, parent),
	Client<Logger>(name, parent),
	Client<CachePowerEstimator>(name, parent),
	Client<PowerMode>(name, parent),
	disasm_export("disasm-export", this),
	registers_export("registers-export", this),
	memory_export("memory-export", this),
	cpu_linux_os_export("cpu-linux-os-export", this),
	kernel_loader_import("kernel-loader-import", this),
	debug_control_import("debug-control-import", this),
	memory_access_reporting_import("memory-access-reporting-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	memory_import("memory-import", this),
	linux_os_import("linux-os-import", this),
	logger_import("logger-import", this),
	fpu_logger_import("fpu-logger-import", this),
	mmu_logger_import("mmu-logger-import", this),
	il1_power_estimator_import("il1-power-estimator-import", this),
	dl1_power_estimator_import("dl1-power-estimator-import", this),
	l2_power_estimator_import("l2-power-estimator-import", this),
	itlb_power_estimator_import("itlb-power-estimator-import", this),
	dtlb_power_estimator_import("dtlb-power-estimator-import", this),
	il1_power_mode_import("il1-power-mode-import", this),
	dl1_power_mode_import("dl1-power-mode-import", this),
	l2_power_mode_import("l2-power-mode-import", this),
	itlb_power_mode_import("itlb-power-mode-import", this),
	dtlb_power_mode_import("dtlb-power-mode-import", this),
	internal_il1_power_mode_import("internal-il1-power-mode-import", this),
	internal_dl1_power_mode_import("internal-dl1-power-mode-import", this),
	internal_l2_power_mode_import("internal-l2-power-mode-import", this),
	internal_itlb_power_mode_import("internal-itlb-power-mode-import", this),
	internal_dtlb_power_mode_import("internal-dtlb-power-mode-import", this),
	dl1("dl1", CONFIG::L2_ENABLE ? &l2 : 0, bus_interface, this),
	il1("il1", CONFIG::L2_ENABLE ? &l2 : 0, bus_interface, this),
	l2("l2", 0, bus_interface, this),
	mmu("mmu", CONFIG::L1_INSN_ENABLE ? (CacheInterface<physical_address_t> *) &il1 : (CONFIG::L2_ENABLE ? (CacheInterface<physical_address_t> *) &l2 : 0), CONFIG::L1_DATA_ENABLE ? (CacheInterface<physical_address_t> *) &dl1 : (CONFIG::L2_ENABLE ? (CacheInterface<physical_address_t> *) &l2 : 0), bus_interface, this),
	fpu("fpu", this),
	cpu_frequency(0),
	voltage(0),
	bus_frequency(0),
	max_inst(0xffffffffffffffffULL),
	num_insn_in_prefetch_buffer(0),
	cur_insn_in_prefetch_buffer(0),
	icache_enabled(false),
#if 0
	printk_buf_addr(0),
	printk_buf_size(0),
#endif
	param_cpu_frequency("cpu-frequency", this, cpu_frequency),
	param_voltage("voltage", this, voltage),
	param_bus_frequency("bus-frequency", this, bus_frequency),
	param_max_inst("max-inst", this, max_inst)
{
	SetupDependsOn(internal_il1_power_mode_import);
	SetupDependsOn(internal_dl1_power_mode_import);
	SetupDependsOn(internal_l2_power_mode_import);
	SetupDependsOn(internal_itlb_power_mode_import);
	SetupDependsOn(internal_dtlb_power_mode_import);
	SetupDependsOn(logger_import);

	this->bus_interface = bus_interface;

	int i;

	for(i = 0; i < 32; i++)
	{
		stringstream sstr;
		sstr << "r" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &gpr[i]);
	}

	registers_registry["cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("cr", &cr);
	registers_registry["xer"] = new unisim::util::debug::SimpleRegister<uint32_t>("xer", &xer);
	registers_registry["lr"] = new unisim::util::debug::SimpleRegister<uint32_t>("lr", &lr);
	registers_registry["ctr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ctr", &ctr);
	registers_registry["cia"] = new unisim::util::debug::SimpleRegister<uint32_t>("cia", &cia);

	registers_registry["msr"] = new unisim::util::debug::SimpleRegister<uint32_t>("msr", &msr);
	if(CONFIG::HAS_SRR0) registers_registry["srr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr0", &srr0);
	if(CONFIG::HAS_SRR1) registers_registry["srr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr1", &srr1);

	if(CONFIG::HAS_TBL) registers_registry["tbl"] = new unisim::util::debug::SimpleRegister<uint32_t>("tbl", &tbl);
	if(CONFIG::HAS_TBU) registers_registry["tbu"] = new unisim::util::debug::SimpleRegister<uint32_t>("tbu", &tbu);

	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
	{
		stringstream sstr;
		sstr << "sprg" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sprg[i]);
	}

	for(i = 0; i < CONFIG::NUM_VRS; i++)
	{
		stringstream sstr;
		sstr << "vr" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<vr_t>(sstr.str().c_str(), &vr[i]);
	}

	if(CONFIG::HAS_DAR) registers_registry["dar"] = new unisim::util::debug::SimpleRegister<uint32_t>("dar", &dar);
	if(CONFIG::HAS_DSISR) registers_registry["dsisr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dsisr", &dsisr);
	if(CONFIG::HAS_EAR) registers_registry["ear"] = new unisim::util::debug::SimpleRegister<uint32_t>("ear", &ear);
	if(CONFIG::HAS_DEC) registers_registry["dec"] = new unisim::util::debug::SimpleRegister<uint32_t>("dec", &dec);
	if(CONFIG::HAS_DABR) registers_registry["dabr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dabr", &dabr);
	registers_registry["pvr"] = new unisim::util::debug::SimpleRegister<uint32_t>("pvr", &pvr);

	mmu.to_mem >> memory_import;
	
	if(CONFIG::L1_DATA_ENABLE)
	{
		mmu.to_dcache >> dl1.to_cpu;
		dl1.power_estimator_import >> dl1_power_estimator_import;
		dl1.power_mode_import >> dl1_power_mode_import;
		internal_dl1_power_mode_import >> dl1.power_mode_export;
	}
	
	if(CONFIG::L1_INSN_ENABLE)
	{
		mmu.to_icache >> il1.to_cpu;
		il1.power_estimator_import >> il1_power_estimator_import;
		il1.power_mode_import >> il1_power_mode_import;
		internal_il1_power_mode_import >> il1.power_mode_export;
	}
	
	if(CONFIG::L2_ENABLE)
	{
		if(CONFIG::L1_DATA_ENABLE)
		{
			dl1.to_mem >> l2.to_cpu;
		}
		if(CONFIG::L1_INSN_ENABLE)
		{
			il1.to_mem >> l2.to_cpu;
		}
		l2.to_mem >> memory_import;
		l2.power_estimator_import >> l2_power_estimator_import;
		l2.power_mode_import >> l2_power_mode_import;
		internal_l2_power_mode_import >> l2.power_mode_export;
	}

	if(CONFIG::ITLB_ENABLE)
	{
		mmu.itlb_power_estimator_import >> itlb_power_estimator_import;
		mmu.itlb_power_mode_import >> itlb_power_mode_import;
		internal_itlb_power_mode_import >> mmu.itlb_power_mode_export;
	}
	
	if(CONFIG::DTLB_ENABLE)
	{
		mmu.dtlb_power_estimator_import >> dtlb_power_estimator_import;
		mmu.dtlb_power_mode_import >> dtlb_power_mode_import;
		internal_dtlb_power_mode_import >> mmu.dtlb_power_mode_export;
	}

	if(CONFIG::HAS_HID0) registers_registry["hid0"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid0", &hid0);
	if(CONFIG::HAS_HID1) registers_registry["hid1"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid1", &hid1);
	if(CONFIG::HAS_IABR) registers_registry["iabr"] = new unisim::util::debug::SimpleRegister<uint32_t>("iabr", &iabr);
	if(CONFIG::HAS_LDSTDB) registers_registry["ldstdb"] = new unisim::util::debug::SimpleRegister<uint32_t>("ldstdb", &ldstdb);
	if(CONFIG::HAS_THRM1) registers_registry["thrm1"] = new unisim::util::debug::SimpleRegister<uint32_t>("thrm1", &thrm1);
	if(CONFIG::HAS_THRM2) registers_registry["thrm2"] = new unisim::util::debug::SimpleRegister<uint32_t>("thrm2", &thrm2);
	if(CONFIG::HAS_THRM3) registers_registry["thrm3"] = new unisim::util::debug::SimpleRegister<uint32_t>("thrm3", &thrm3);
	if(CONFIG::HAS_ICTC) registers_registry["ictc"] = new unisim::util::debug::SimpleRegister<uint32_t>("ictc", &ictc);
	if(CONFIG::HAS_PMC1) registers_registry["pmc1"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc1", &pmc1);
	if(CONFIG::HAS_PMC2) registers_registry["pmc2"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc2", &pmc1);
	if(CONFIG::HAS_PMC3) registers_registry["pmc3"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc3", &pmc1);
	if(CONFIG::HAS_PMC4) registers_registry["pmc4"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc4", &pmc1);
	if(CONFIG::HAS_SIA) registers_registry["sia"] = new unisim::util::debug::SimpleRegister<uint32_t>("sia", &sia);
	if(CONFIG::HAS_SDA) registers_registry["sda"] = new unisim::util::debug::SimpleRegister<uint32_t>("sda", &sda);
	if(CONFIG::HAS_MMCR0) registers_registry["mmcr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmmcr0", &mmcr0);
	if(CONFIG::HAS_MMCR1) registers_registry["mmcr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmcr1", &mmcr1);
	if(CONFIG::HAS_L2CR) registers_registry["l2cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("l2cr", &l2cr);
	if(CONFIG::HAS_L2PM) registers_registry["l2pm"] = new unisim::util::debug::SimpleRegister<uint32_t>("l2pm", &l2pm);
	if(CONFIG::HAS_HID2) registers_registry["hid2"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid2", &hid2);
	if(CONFIG::HAS_DMISS) registers_registry["dmiss"] = new unisim::util::debug::SimpleRegister<uint32_t>("dmiss", &dmiss);
	if(CONFIG::HAS_IMISS) registers_registry["imiss"] = new unisim::util::debug::SimpleRegister<uint32_t>("imiss", &imiss);
	if(CONFIG::HAS_DCMP) registers_registry["dcmp"] = new unisim::util::debug::SimpleRegister<uint32_t>("dcmp", &dcmp);
	if(CONFIG::HAS_ICMP) registers_registry["icmp"] = new unisim::util::debug::SimpleRegister<uint32_t>("icmp", &icmp);
	if(CONFIG::HAS_HASH1) registers_registry["hash1"] = new unisim::util::debug::SimpleRegister<uint32_t>("hash1", &hash1);
	if(CONFIG::HAS_HASH2) registers_registry["hash2"] = new unisim::util::debug::SimpleRegister<uint32_t>("hash2", &hash2);
	if(CONFIG::HAS_RPA) registers_registry["rpa"] = new unisim::util::debug::SimpleRegister<uint32_t>("rpa", &rpa);

	if(CONFIG::HAS_VSCR) registers_registry["vscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("vscr", &vscr);
	if(CONFIG::HAS_VSCR) registers_registry["vrsave"] = new unisim::util::debug::SimpleRegister<uint32_t>("vrsave", &vrsave);

	fpu.logger_import >> fpu_logger_import;
	mmu.logger_import >> mmu_logger_import;
	
	Reset();
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
}

template <class CONFIG>
bool CPU<CONFIG>::Setup()
{
	if(linux_os_import)
	{
		SetMSR_FP();
		SetMSR_PR();
	}
	
	unsigned int max_frequency = 0xffffffffUL;
	
	if(CONFIG::L1_INSN_ENABLE)
	{
		if(!internal_il1_power_mode_import) return false;
		unsigned int il1_max_frequency = internal_il1_power_mode_import->GetMaxFrequency();
		if(il1_max_frequency > 0 && il1_max_frequency < max_frequency) max_frequency = il1_max_frequency;
		unsigned int il1_default_voltage = internal_il1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = il1_default_voltage;
	}
	
	if(CONFIG::L1_DATA_ENABLE)
	{
		if(!internal_dl1_power_mode_import) return false;
		unsigned int dl1_max_frequency = internal_dl1_power_mode_import->GetMaxFrequency();
		if(dl1_max_frequency > 0 && dl1_max_frequency < max_frequency) max_frequency = dl1_max_frequency;
		unsigned int dl1_default_voltage = internal_dl1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = dl1_default_voltage;
	}
	
	if(CONFIG::L2_ENABLE)
	{
		if(!internal_l2_power_mode_import) return false;
		unsigned int l2_max_frequency = internal_l2_power_mode_import->GetMaxFrequency();
		if(l2_max_frequency > 0 && l2_max_frequency < max_frequency) max_frequency = l2_max_frequency;
		unsigned int l2_default_voltage = internal_l2_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = l2_default_voltage;
	}
	

	if(cpu_frequency > 0)
	{
		if(cpu_frequency > max_frequency)
		{
			if(logger_import)
			{
				(*logger_import) << DebugWarning;
				(*logger_import) << "A frequency of " << cpu_frequency << " Mhz is too high for the simulated hardware !" << Endl;
				(*logger_import) << "cpu frequency should be < " << max_frequency << " Mhz." << Endl;
				(*logger_import) << EndDebugWarning;
			}
		}
	}
	else
	{
		if(max_frequency != 0xffffffffUL)
		{
			cpu_frequency = max_frequency;
		}
		else
		{
			// We can't provide a valid configuration automatically
			if(logger_import)
			{
				(*logger_import) << DebugError;
				(*logger_import) << "user must provide a cpu frequency > 0" << Endl;
				(*logger_import) << EndDebugError;
			}
			return false;
		}
	}

	if(logger_import)
	{
		(*logger_import) << DebugInfo << "cpu frequency of " << cpu_frequency << " Mhz" << Endl << EndDebugInfo;
	}

	if(voltage <= 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError << "user must provide a voltage > 0" << Endl << EndDebugError;
		}
		return false;
	}

	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		
		(*logger_import) << "voltage of " << ((double) voltage / 1e3) << " V" << Endl;

		if(bus_frequency > 0)
			(*logger_import) << "bus frequency of " << bus_frequency << " Mhz" << Endl;
		
		(*logger_import) << EndDebugInfo;
		
		if(bus_frequency <= 0)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "bus frequency must be > 0" << Endl;
			(*logger_import) << EndDebugError;
		}
	}

	if(internal_il1_power_mode_import)
		internal_il1_power_mode_import->SetPowerMode(cpu_frequency, voltage);
	
	if(internal_dl1_power_mode_import)
		internal_dl1_power_mode_import->SetPowerMode(cpu_frequency, voltage);
	
	if(internal_l2_power_mode_import)
		internal_l2_power_mode_import->SetPowerMode(cpu_frequency, voltage);
	
	if(linux_os_import)
	{
		if(CONFIG::HAS_MSR_FP) SetMSR_FP();   // enable floating point unit
		if(CONFIG::HAS_MSR_PR) SetMSR_PR();   // user mode
		if(CONFIG::HAS_HID0_DCE) SetHID0_DCE(); // enable L1 data cache
		if(CONFIG::HAS_HID0_ICE) SetHID0_ICE(); // enable L1 instruction cache
		if(CONFIG::HAS_HID0_ABE) SetHID0_ABE(); // enable address only broadcast

		if(CONFIG::HAS_L2CR) SetL2CR_L2E(); // enable L2 cache
	}
	
	if(kernel_loader_import)
	{
		if(CONFIG::HAS_MSR_FP) SetMSR_FP();   // enable floating point unit
		if(CONFIG::HAS_HID0_DCE) SetHID0_DCE(); // enable L1 data cache
		if(CONFIG::HAS_HID0_ICE) SetHID0_ICE(); // enable L1 instruction cache
		if(CONFIG::HAS_HID0_ABE) SetHID0_ABE(); // enable address only broadcast
		if(CONFIG::HAS_HID1_ABE) SetHID1_ABE(); // enable address only broadcast

		if(CONFIG::HAS_L2CR_L2E) SetL2CR_L2E(); // enable L2 cache
		
#if 0
		if(!printk_buf_addr)
		{
			if(symbol_table)
			{
				const Symbol<address_t> *symbol;
				
				symbol = symbol_table_lookup_import->FindSymbolByName("printk_buf.7", Symbol<address_t>::SYM_OBJECT);
				
				if(!symbol)
				{
					symbol = symbol_table_lookup_import->FindSymbolByName("printk_buf.6", Symbol<address_t>::SYM_OBJECT);
				}
				
				if(!symbol)
				{
					symbol = symbol_table_lookup_import->FindSymbolByName("printk_buf.898", Symbol<address_t>::SYM_OBJECT);
				}
				
				if(symbol)
				{
					printk_buf_addr = symbol->GetAddress();
					printk_buf_size = symbol->GetSize();
					cerr << "Found printk_buf at 0x" << hex << printk_buf_addr << std::dec << "(" << printk_buf_size << " bytes)" << endl;
				}
			}
		}
#endif
	}
	return true;
}

template <class CONFIG>
void CPU<CONFIG>::OnDisconnect()
{
}

template <class CONFIG>
void CPU<CONFIG>::Reset()
{
	int i;

	bus_cycle = 0;
	instruction_counter = 0;

	dec = 0;

	decrementer_overflow = false;
	external_interrupt = false;
	soft_reset = false;
	hard_reset = false;
	asynchronous_interrupt = false;

	reserve = false;
	reserve_addr = 0;

	for(i = 0; i < 32; i++) SetGPR(i, 0);
	SetCR(0);
	SetXER(0);
	SetLR(0);
	SetCTR(0);

	SetCIA(CONFIG::START_ADDR);
	SetNIA(CONFIG::START_ADDR);
	
	msr = 0;
	SetMSR(CONFIG::MSR_RESET_VALUE);
	
	SetPVR(CONFIG::PROCESSOR_VERSION);
	
	SetSRR0(0);
	SetSRR1(0);
	if(CONFIG::HAS_TBL) SetTBL(0);
	if(CONFIG::HAS_TBU) SetTBU(0);

	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
		SetSPRG(i, 0);

	if(CONFIG::HAS_DAR) SetDAR(0);
	if(CONFIG::HAS_DSISR) SetDSISR(0);
	if(CONFIG::HAS_EAR) SetEAR(0);
	if(CONFIG::HAS_DEC) SetDEC(0);
	if(CONFIG::HAS_DABR) SetDABR(0);

	CPU<CONFIG>::InvalidateDecodingCache();
	
	mmu.Reset();
	if(CONFIG::L1_INSN_ENABLE)
	{
		il1.Reset();
	}
	if(CONFIG::L1_DATA_ENABLE)
	{
		dl1.Reset();
	}
	if(CONFIG::L2_ENABLE)
	{
		l2.Reset();
	}
	fpu.Reset();
	
	num_insn_in_prefetch_buffer = 0;
	cur_insn_in_prefetch_buffer = 0;

	if(CONFIG::HAS_DECREMENTER_OVERFLOW) decrementer_overflow = false;
	if(CONFIG::HAS_EXTERNAL_INTERRUPT) external_interrupt = false;
	if(CONFIG::HAS_SOFT_RESET) soft_reset = false;
	if(CONFIG::HAS_HARD_RESET) hard_reset = false;
	if(CONFIG::HAS_MCP) mcp = false;
	if(CONFIG::HAS_TEA) tea = false;
	if(CONFIG::HAS_SMI) smi = false;
	if(CONFIG::HAS_THERMAL_MANAGEMENT_INTERRUPT) thermal_management_interrupt = false;
	if(CONFIG::HAS_PERFORMANCE_MONITOR_INTERRUPT) performance_monitor_interrupt = false;
	asynchronous_interrupt = false;
	
	if(CONFIG::HAS_HID0)
	{
		hid0 = 0;
		SetHID0(CONFIG::HID0_RESET_VALUE);
	}
	if(CONFIG::HAS_HID1)
	{
		hid1 = 0;
		SetHID1(0);
	}

	if(CONFIG::HAS_HID2)
	{
		hid2 = 0;
		SetHID2(0);
	}

	if(CONFIG::HAS_IABR) SetIABR(0);
	if(CONFIG::HAS_LDSTDB) SetLDSTDB(0);
	if(CONFIG::HAS_THRM1) SetTHRM1(0);
	if(CONFIG::HAS_THRM2) SetTHRM2(0);
	if(CONFIG::HAS_THRM3) SetTHRM3(0);
	if(CONFIG::HAS_ICTC) SetICTC(0);
	if(CONFIG::HAS_PMC1) SetPMC1(0);
	if(CONFIG::HAS_PMC2) SetPMC2(0);
	if(CONFIG::HAS_PMC3) SetPMC3(0);
	if(CONFIG::HAS_PMC4) SetPMC4(0);
	if(CONFIG::HAS_PMC5) SetPMC5(0);
	if(CONFIG::HAS_PMC6) SetPMC6(0);
	if(CONFIG::HAS_SIA) SetSIA(0);
	if(CONFIG::HAS_SDA) SetSDA(0);
	if(CONFIG::HAS_MMCR0) SetMMCR0(0);
	if(CONFIG::HAS_MMCR1) SetMMCR1(0);
	if(CONFIG::HAS_MMCR2) SetMMCR2(0);
	
	if(CONFIG::HAS_L2CR)
	{
		l2cr = 0;
		SetL2CR(0);
	}

	if(CONFIG::HAS_DMISS) SetDMISS(0);
	if(CONFIG::HAS_DCMP) SetDCMP(0);
	if(CONFIG::HAS_HASH1) SetHASH1(0);
	if(CONFIG::HAS_HASH2) SetHASH2(0);
	if(CONFIG::HAS_RPA) SetRPA(0);
	if(CONFIG::HAS_L2PM) SetL2PM(0);
}

template <class CONFIG>
uint32_t CPU<CONFIG>::GetSPR(unsigned int n)
{
	switch(n)
	{
		case 0:
			if(CONFIG::HAS_MQ)
			{
				return GetMQ();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1: return GetXER();
		case 4:
			if(CONFIG::HAS_RTCU)
			{
				return GetRTCU();
			}
			throw IllegalInstructionException<CONFIG>();
		case 5:
			if(CONFIG::HAS_RTCL)
			{
				return GetRTCL();
			}
			throw IllegalInstructionException<CONFIG>();
		case 8: return GetLR();
		case 9: return GetCTR();
		case 18:
			if(CONFIG::HAS_DSISR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDSISR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 19:
			if(CONFIG::HAS_DAR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDAR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 22:
			if(CONFIG::HAS_DEC)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDEC();
			}
			throw IllegalInstructionException<CONFIG>();
		case 25:
			if(CONFIG::HAS_SDR1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetSDR1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 26:
			if(CONFIG::HAS_SRR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetSRR0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 27:
			if(CONFIG::HAS_SRR1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetSRR1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 256:
			if(CONFIG::HAS_VRSAVE)
			{
				return GetVRSAVE();
			}
			throw IllegalInstructionException<CONFIG>();
		case 272:
		case 273:
		case 274:
		case 275:
		case 276:
		case 277:
		case 278:
		case 279:
		{
			unsigned int num_sprg = n - 272;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetSPRG(num_sprg);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 282:
			if(CONFIG::HAS_EAR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetEAR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 287:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPVR();
		case 528:
		case 529:
		case 530:
		case 531:
		case 532:
		case 533:
		case 534:
		case 535:
		{
			unsigned int num_ibat = (n - 528) >> 1;
			unsigned int l = (n - 528) & 1;
			if(num_ibat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return l ? GetIBATL(num_ibat) : GetIBATU(num_ibat);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 536:
		case 537:
		case 538:
		case 539:
		case 540:
		case 541:
		case 542:
		case 543:
		{
			unsigned int num_dbat = (n - 536) >> 1;
			unsigned int l = (n - 536) & 1;
			if(num_dbat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return l ? GetDBATL(num_dbat) : GetDBATU(num_dbat);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 560:
		case 561:
		case 562:
		case 563:
		case 564:
		case 565:
		case 566:
		case 567:
		{
			unsigned int num_ibat = 4 + ((n - 560) >> 1);
			unsigned int l = (n - 560) & 1;
			if(num_ibat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return l ? GetIBATL(num_ibat) : GetIBATU(num_ibat);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 568:
		case 569:
		case 570:
		case 571:
		case 572:
		case 573:
		case 574:
		case 575:
		{
			unsigned int num_dbat = 4 + ((n - 568) >> 1);
			unsigned int l = (n - 568) & 1;
			if(num_dbat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return l ? GetDBATL(num_dbat) : GetDBATU(num_dbat);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 928:
			if(CONFIG::HAS_MMCR2)
			{
				return GetMMCR2();
			}
			throw IllegalInstructionException<CONFIG>();
		case 929:
			if(CONFIG::HAS_PMC5)
			{
				return GetPMC5();
			}
			throw IllegalInstructionException<CONFIG>();
		case 930:
			if(CONFIG::HAS_PMC6)
			{
				return GetPMC6();
			}
			throw IllegalInstructionException<CONFIG>();
		case 936:
			if(CONFIG::HAS_MMCR0)
			{
				return GetMMCR0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 937:
			if(CONFIG::HAS_PMC1)
			{
				return GetPMC1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 938:
			if(CONFIG::HAS_PMC2)
			{
				return GetPMC2();
			}
			throw IllegalInstructionException<CONFIG>();
		case 940:
			if(CONFIG::HAS_MMCR1)
			{
				return GetMMCR1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 941:
			if(CONFIG::HAS_PMC3)
			{
				return GetPMC3();
			}
			throw IllegalInstructionException<CONFIG>();
		case 942:
			if(CONFIG::HAS_PMC4)
			{
				return GetPMC4();
			}
			throw IllegalInstructionException<CONFIG>();
		case 944:
			if(CONFIG::HAS_MMCR2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetMMCR2();
			}
			throw IllegalInstructionException<CONFIG>();
		case 945:
			if(CONFIG::HAS_PMC5)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPMC5();
			}
			throw IllegalInstructionException<CONFIG>();
		case 946:
			if(CONFIG::HAS_PMC6)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPMC6();
			}
			throw IllegalInstructionException<CONFIG>();
		case 951:
			if(CONFIG::HAS_BAMR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetBAMR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 952:
			if(CONFIG::HAS_MMCR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetMMCR0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 953:
			if(CONFIG::HAS_PMC1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPMC1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 954:
			if(CONFIG::HAS_PMC2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPMC2();
			}
			throw IllegalInstructionException<CONFIG>();
		case 955:
			if(CONFIG::HAS_SIA)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetSIA();
			}
			throw IllegalInstructionException<CONFIG>();
		case 956:
			if(CONFIG::HAS_MMCR1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetMMCR1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 957:
			if(CONFIG::HAS_PMC3)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPMC3();
			}
			throw IllegalInstructionException<CONFIG>();
		case 958:
			if(CONFIG::HAS_PMC4)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPMC4();
			}
			throw IllegalInstructionException<CONFIG>();
		case 976:
			if(CONFIG::HAS_DMISS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDMISS();
			}
			throw IllegalInstructionException<CONFIG>();
		case 977:
			if(CONFIG::HAS_DCMP)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDCMP();
			}
			throw IllegalInstructionException<CONFIG>();
		case 978:
			if(CONFIG::HAS_HASH1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetHASH1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 979:
			if(CONFIG::HAS_HASH2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetHASH2();
			}
			throw IllegalInstructionException<CONFIG>();
		case 980:
			if(CONFIG::HAS_IMISS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetIMISS();
			}
			if(CONFIG::HAS_TLBMISS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetTLBMISS();
			}
			throw IllegalInstructionException<CONFIG>();
		case 981:
			if(CONFIG::HAS_ICMP)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetICMP();
			}
			if(CONFIG::HAS_PTEHI)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPTEHI();
			}
			throw IllegalInstructionException<CONFIG>();
		case 982:
			if(CONFIG::HAS_RPA)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPTELO();
			}
			if(CONFIG::HAS_PTELO)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPTELO();
			}
			throw IllegalInstructionException<CONFIG>();
		case 983:
			if(CONFIG::HAS_L3PM)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL3PM();
			}
			throw IllegalInstructionException<CONFIG>();
		case 984:
			if(CONFIG::HAS_L3ITCR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL3ITCR0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 985:
			if(CONFIG::HAS_L2ERRINJHI)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRINJHI();
			}
			throw IllegalInstructionException<CONFIG>();
		case 986:
			if(CONFIG::HAS_L2ERRINJLO)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRINJLO();
			}
			throw IllegalInstructionException<CONFIG>();
		case 987:
			if(CONFIG::HAS_L2ERRINJCTL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRINJCTL();
			}
			throw IllegalInstructionException<CONFIG>();
		case 988:
			if(CONFIG::HAS_L2CAPTDATAHI)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2CAPTDATAHI();
			}
			throw IllegalInstructionException<CONFIG>();
		case 989:
			if(CONFIG::HAS_L2CAPTDATALO)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2CAPTDATALO();
			}
			throw IllegalInstructionException<CONFIG>();
		case 990:
			if(CONFIG::HAS_L2CAPTDATAECC)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2CAPTDATAECC();
			}
			throw IllegalInstructionException<CONFIG>();
		case 991:
			if(CONFIG::HAS_L2ERRDET)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRDET();
			}
			throw IllegalInstructionException<CONFIG>();
		case 992:
			if(CONFIG::HAS_L2ERRDIS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRDIS();
			}
			throw IllegalInstructionException<CONFIG>();
		case 993:
			if(CONFIG::HAS_L2ERRINTEN)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRINTEN();
			}
			throw IllegalInstructionException<CONFIG>();
		case 994:
			if(CONFIG::HAS_L2ERRATTR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRATTR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 995:
			if(CONFIG::HAS_L2ERRADDR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRADDR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 996:
			if(CONFIG::HAS_L2ERREADDR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERREADDR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 997:
			if(CONFIG::HAS_L2ERRCTL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2ERRCTL();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1000:
			if(CONFIG::HAS_L3OHCR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL3OHCR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1008:
			if(CONFIG::HAS_HID0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetHID0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1009:
			if(CONFIG::HAS_HID1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetHID1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1010:
			if(CONFIG::HAS_IABR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetIABR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1011:
			if(CONFIG::HAS_HID2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetHID2();
			}
			if(CONFIG::HAS_ICTRL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetICTRL();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1012: // undocumented !!!
			if(CONFIG::HAS_LDSTDB)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetLDSTDB();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1013:
			if(CONFIG::HAS_DABR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDABR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1014:
			if(CONFIG::HAS_MSSCR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetMSSCR0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1015:
			if(CONFIG::HAS_MSSSR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetMSSSR0();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1016:
			if(CONFIG::HAS_L2PM)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2PM();
			}
			if(CONFIG::HAS_LDSTCR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetLDSTCR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1017:
			if(CONFIG::HAS_L2CR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL2CR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1018:
			if(CONFIG::HAS_L3CR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetL3CR();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1019:
			if(CONFIG::HAS_ICTC)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetICTC();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1020:
			if(CONFIG::HAS_THRM1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetTHRM1();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1021:
			if(CONFIG::HAS_THRM2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetTHRM2();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1022:
			if(CONFIG::HAS_THRM3)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetTHRM3();
			}
			throw IllegalInstructionException<CONFIG>();
		case 1023:
			if(CONFIG::HAS_PIR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetPIR();
			}
			throw IllegalInstructionException<CONFIG>();
	}
	throw IllegalInstructionException<CONFIG>();
	return 0;
}

template <class CONFIG>
void CPU<CONFIG>::SetSPR(unsigned int n, uint32_t value)
{
	switch(n)
	{
		case 0:
			if(CONFIG::HAS_MQ)
			{
				SetMQ(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1: SetXER(value); return;
		case 8: SetLR(value); return;
		case 9: SetCTR(value); return;
		case 18:
			if(CONFIG::HAS_DSISR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDSISR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 19:
			if(CONFIG::HAS_DAR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDAR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 20:
			if(CONFIG::HAS_RTCU)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetRTCU(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 21:
			if(CONFIG::HAS_RTCL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetRTCL(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 22:
			if(CONFIG::HAS_DEC)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDEC(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 25:
			if(CONFIG::HAS_SDR1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSDR1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 26:
			if(CONFIG::HAS_SRR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSRR0(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 27:
			if(CONFIG::HAS_SRR1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSRR1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 256:
			if(CONFIG::HAS_VRSAVE)
			{
				SetVRSAVE(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 272:
		case 273:
		case 274:
		case 275:
		case 276:
		case 277:
		case 278:
		case 279:
		{
			unsigned int num_sprg = n - 272;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSPRG(num_sprg, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 282:
			if(CONFIG::HAS_EAR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetEAR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 284:
			if(CONFIG::HAS_TBL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetTBL(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 285:
			if(CONFIG::HAS_TBU)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetTBU(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 528:
		case 529:
		case 530:
		case 531:
		case 532:
		case 533:
		case 534:
		case 535:
		{
			unsigned int num_ibat = (n - 528) >> 1;
			unsigned int l = (n - 528) & 1;
			if(num_ibat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				if(l) SetIBATL(num_ibat, value); else SetIBATU(num_ibat, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 536:
		case 537:
		case 538:
		case 539:
		case 540:
		case 541:
		case 542:
		case 543:
		{
			unsigned int num_dbat = (n - 536) >> 1;
			unsigned int l = (n - 536) & 1;
			if(num_dbat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				if(l) SetDBATL(num_dbat, value); else SetDBATU(num_dbat, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 560:
		case 561:
		case 562:
		case 563:
		case 564:
		case 565:
		case 566:
		case 567:
		{
			unsigned int num_ibat = 4 + ((n - 560) >> 1);
			unsigned int l = (n - 560) & 1;
			if(num_ibat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				if(l) SetIBATL(num_ibat, value); else SetIBATU(num_ibat, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 568:
		case 569:
		case 570:
		case 571:
		case 572:
		case 573:
		case 574:
		case 575:
		{
			unsigned int num_dbat = 4 + ((n - 568) >> 1);
			unsigned int l = (n - 568) & 1;
			if(num_dbat < CONFIG::NUM_BATS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				if(l) SetDBATL(num_dbat, value); else SetDBATU(num_dbat, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 944:
			if(CONFIG::HAS_MMCR2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetMMCR2(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 945:
			if(CONFIG::HAS_PMC5)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPMC5(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 946:
			if(CONFIG::HAS_PMC6)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPMC6(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 951:
			if(CONFIG::HAS_BAMR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetBAMR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 952:
			if(CONFIG::HAS_MMCR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetMMCR0(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 953:
			if(CONFIG::HAS_PMC1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPMC1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 954:
			if(CONFIG::HAS_PMC2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPMC2(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 955:
			if(CONFIG::HAS_SIA)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSIA(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 956:
			if(CONFIG::HAS_MMCR1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetMMCR1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 957:
			if(CONFIG::HAS_PMC3)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPMC3(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 958:
			if(CONFIG::HAS_PMC4)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPMC4(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 959:
			if(CONFIG::HAS_SDA)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSDA(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 976:
			if(CONFIG::HAS_DMISS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDMISS(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 977:
			if(CONFIG::HAS_DCMP)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDCMP(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 978:
			if(CONFIG::HAS_HASH1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetHASH1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 979:
			if(CONFIG::HAS_HASH2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetHASH2(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 980:
			if(CONFIG::HAS_IMISS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetIMISS(value);
				return;
			}
			if(CONFIG::HAS_TLBMISS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetTLBMISS(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 981:
			if(CONFIG::HAS_ICMP)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetICMP(value);
				return;
			}
			if(CONFIG::HAS_PTEHI)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPTEHI(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 982:
			if(CONFIG::HAS_RPA)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetRPA(value);
				return;
			}
			if(CONFIG::HAS_PTELO)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPTELO(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 983:
			if(CONFIG::HAS_L3PM)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL3PM(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 984:
			if(CONFIG::HAS_L3ITCR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL3ITCR0(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 985:
			if(CONFIG::HAS_L2ERRINJHI)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRINJHI(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 986:
			if(CONFIG::HAS_L2ERRINJLO)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRINJLO(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 987:
			if(CONFIG::HAS_L2ERRINJCTL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRINJCTL(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 988:
			if(CONFIG::HAS_L2CAPTDATAHI)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2CAPTDATAHI(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 989:
			if(CONFIG::HAS_L2CAPTDATALO)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2CAPTDATALO(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 990:
			if(CONFIG::HAS_L2CAPTDATAECC)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2CAPTDATAECC(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 991:
			if(CONFIG::HAS_L2ERRDET)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRDET(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 992:
			if(CONFIG::HAS_L2ERRDIS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRDIS(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 993:
			if(CONFIG::HAS_L2ERRINTEN)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRINTEN(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 994:
			if(CONFIG::HAS_L2ERRATTR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRATTR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 995:
			if(CONFIG::HAS_L2ERRADDR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRADDR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 996:
			if(CONFIG::HAS_L2ERREADDR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERREADDR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 997:
			if(CONFIG::HAS_L2ERRCTL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2ERRCTL(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1000:
			if(CONFIG::HAS_L3OHCR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL3OHCR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1008:
			if(CONFIG::HAS_HID0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetHID0(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1009:
			if(CONFIG::HAS_HID1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetHID1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1010:
			if(CONFIG::HAS_IABR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetIABR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1011:
			if(CONFIG::HAS_HID2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetHID2(value);
				return;
			}
			if(CONFIG::HAS_ICTRL)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetICTRL(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1012: // undocumented !!!
			if(CONFIG::HAS_LDSTDB)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetLDSTDB(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1013:
			if(CONFIG::HAS_DABR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDABR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1014:
			if(CONFIG::HAS_MSSCR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetMSSCR0(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1015:
			if(CONFIG::HAS_MSSSR0)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetMSSSR0(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1016:
			if(CONFIG::HAS_L2PM)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2PM(value);
				return;
			}
			if(CONFIG::HAS_LDSTCR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetLDSTCR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1017:
			if(CONFIG::HAS_L2CR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL2CR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1018:
			if(CONFIG::HAS_L3CR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetL3CR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1019:
			if(CONFIG::HAS_ICTC)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetICTC(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1020:
			if(CONFIG::HAS_THRM1)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetTHRM1(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1021:
			if(CONFIG::HAS_THRM2)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetTHRM2(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1022:
			if(CONFIG::HAS_THRM3)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetTHRM3(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		case 1023:
			if(CONFIG::HAS_PIR)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetPIR(value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
	}
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Fetch(void *buffer, typename CONFIG::address_t addr, uint32_t size)
{
	if(CONFIG::L1_INSN_ENABLE)
	{
		il1.PrRead(addr, buffer, size, CC_MEMORY_COHERENCY_ENFORCED);
	}
	else
	{
		CacheStatus cs;
	
		bus_interface->BusRead(addr, buffer, size, BC_NONE, cs);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Step()
{
	if(debug_control_import)
	{
		typename DebugControl<address_t>::DebugCommand dbg_cmd;

		do
		{
			dbg_cmd = debug_control_import->FetchDebugCommand(GetCIA());
	
			if(dbg_cmd == DebugControl<address_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<address_t>::DBG_SYNC)
			{
				Sync();
				continue;
			}

			if(dbg_cmd == DebugControl<address_t>::DBG_KILL) Stop(0);
			if(dbg_cmd == DebugControl<address_t>::DBG_RESET)
			{
				bus_interface->Reset();
				if(kernel_loader_import)
				{
					kernel_loader_import->Reset();
				}
			}
		} while(1);
	}

	address_t sequential_nia = GetCIA() + 4;
	Operation<CONFIG> *operation = 0;

	SetNIA(sequential_nia);

	try
	{
		address_t addr = GetCIA();
		uint32_t insn;
		
		if(cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer)
		{
			uint32_t size_to_block_boundary = icache_enabled ? CONFIG::L1_INSN_CACHE_BLOCK_SIZE - (addr & (CONFIG::L1_INSN_CACHE_BLOCK_SIZE - 1)) : FSB_WIDTH - (addr & (FSB_WIDTH - 1));
			physical_address_t physical_addr;
			// refill the prefetch buffer with up to one cache line, not much
			uint32_t read_size = mmu.ReadInsnMemory(addr, GetPrivilegeLevel(), prefetch_buffer, sizeof(prefetch_buffer) > size_to_block_boundary ? size_to_block_boundary : sizeof(prefetch_buffer), physical_addr);
			num_insn_in_prefetch_buffer = read_size / 4;
			cur_insn_in_prefetch_buffer = 0;
		}
		insn = prefetch_buffer[cur_insn_in_prefetch_buffer];
#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(insn);
#endif

		if(memory_access_reporting_import)
		{
			memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, MemoryAccessReporting<address_t>::MT_INSN, addr, 4);
		}

		/* Check for instruction address breakpoint */
		if(GetIABR_BE() && GetIABR_TE() == GetMSR_IR() && ((GetCIA() >> 2) & 0x3fffffffUL) == GetIABR_ADDR())
		{
			throw InstructionAddressBreakpointException<CONFIG>();
		}
	
		operation = Decoder<CONFIG>::Decode(addr, insn);

		/* execute the instruction */
		operation->execute(this);

		/* update the instruction counter */
		instruction_counter++;
	
		if(HasAsynchronousInterrupt())
		{
			if(CONFIG::HAS_HARD_RESET && HasHardReset()) throw SystemResetException<CONFIG>();
			if(CONFIG::HAS_MCP && HasMCP() && GetHID0_EMCP()) throw MachineCheckException<CONFIG>();
			if(CONFIG::HAS_TEA && HasTEA()) throw MachineCheckException<CONFIG>();
			if(CONFIG::HAS_SOFT_RESET && HasSoftReset()) throw SystemResetException<CONFIG>();
			if(CONFIG::HAS_SMI && HasSMI()) throw SystemManagementInterruptException<CONFIG>();
			if(CONFIG::HAS_EXTERNAL_INTERRUPT && HasExternalInterrupt() && GetMSR_EE()) throw ExternalInterruptException<CONFIG>();
			if(CONFIG::HAS_PERFORMANCE_MONITOR_INTERRUPT && HasPerformanceMonitorInterrupt()) throw PerformanceMonitorInterruptException<CONFIG>();
			if(CONFIG::HAS_DECREMENTER_OVERFLOW && HasDecrementerOverflow() && GetMSR_EE()) throw DecrementerException<CONFIG>();
			if(CONFIG::HAS_THERMAL_MANAGEMENT_INTERRUPT && HasThermalManagementInterrupt()) throw ThermalManagementInterruptException<CONFIG>();
		}
	}
	catch(MachineCheckException<CONFIG>& exc) { HandleException(exc); }
	catch(DecrementerException<CONFIG>& exc) { HandleException(exc); }
	catch(ExternalInterruptException<CONFIG>& exc) { HandleException(exc); }
	catch(SystemResetException<CONFIG>& exc) { HandleException(exc); }
	catch(PerformanceMonitorInterruptException<CONFIG>& exc) { HandleException(exc); }
	catch(SystemManagementInterruptException<CONFIG>& exc) { HandleException(exc); }
	catch(ThermalManagementInterruptException<CONFIG>& exc) { HandleException(exc); }
	catch(ISIProtectionViolationException<CONFIG>& exc) { HandleException(exc); }
	catch(ISINoExecuteException<CONFIG>& exc) { HandleException(exc); }
	catch(ISIDirectStoreException<CONFIG>& exc) { HandleException(exc); }
	catch(ISIPageFaultException<CONFIG>& exc) { HandleException(exc); }
	catch(ISIGuardedMemoryException<CONFIG>& exc) { HandleException(exc); }
	catch(DSIDirectStoreException<CONFIG>& exc) { HandleException(exc); }
	catch(DSIProtectionViolationException<CONFIG>& exc) { HandleException(exc); }
	catch(DSIPageFaultException<CONFIG>& exc) { HandleException(exc); }
	catch(DSIDataAddressBreakpointException<CONFIG>& exc) { HandleException(exc); }
	catch(DSIExternalAccessDisabledException<CONFIG>& exc) { HandleException(exc); }
	catch(DSIWriteThroughLinkedLoadStore<CONFIG>& exc) { HandleException(exc); }
	catch(AlignmentException<CONFIG>& exc) { HandleException(exc, operation->GetEncoding()); }
	catch(IllegalInstructionException<CONFIG>& exc) { HandleException(exc); }
	catch(PrivilegeViolationException<CONFIG>& exc) { HandleException(exc); }
	catch(TrapException<CONFIG>& exc) { HandleException(exc); }
	catch(FloatingPointException<CONFIG>& exc) { HandleException(exc); }
	catch(SystemCallException<CONFIG>& exc) { HandleException(exc); }
	catch(FloatingPointUnavailableException<CONFIG>& exc) { HandleException(exc); }
	catch(TraceException<CONFIG>& exc) { HandleException(exc); }
	catch(InstructionAddressBreakpointException<CONFIG>& exc) { HandleException(exc); }
	catch(TLBMissException<CONFIG>& exc) { HandleException(exc); }
	catch(Exception& e)
	{
		if(logger_import)
			(*logger_import) << DebugError << "uncaught processor exception :" << e.what() << Endl << EndDebugError;
		Stop(1);
	}

	/* go to the next instruction */
	if(GetNIA() != sequential_nia)
	{
		// branch or exception is being taken: flush the prefetch buffer
		num_insn_in_prefetch_buffer = 0;
		cur_insn_in_prefetch_buffer = 0;
	}
	else
	{
		cur_insn_in_prefetch_buffer++;
	}

	SetCIA(GetNIA());
	
	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportFinishedInstruction(GetNIA());
	}

	if(instruction_counter >= max_inst) Stop(0);

}

template <class CONFIG>
void CPU<CONFIG>::OnBusCycle()
{
	/* update the bus cycle counter */
	bus_cycle++;
	
	// Time Base registers and DEC register are updated at one fourth the bus clock rate
	if((bus_cycle % 4) == 0)
	{
		/* Update Time base counters */
/*		SetTBL(GetTBL() + 1);
		if(GetTBL() == 0) SetTBU(GetTBU() + 1);*/
		if(CONFIG::HAS_TBU) SetTBU((bus_cycle >> 34) & 0xffffffffUL);
		if(CONFIG::HAS_TBL) SetTBL((bus_cycle >> 2) & 0xffffffffUL);
		
		/* Decrement the decrementer */
		if(CONFIG::HAS_DEC) SetDEC(GetDEC() - 1);
		//cerr << "DEC = " << GetDEC() << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::FlushSubsequentInstructions()
{
	cur_insn_in_prefetch_buffer = num_insn_in_prefetch_buffer - 1;
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
	do
	{
		Step();
		if(instruction_counter >= max_inst) Stop(0);
	} while(1);
}

template <class CONFIG>
void CPU<CONFIG>::SetHID0(uint32_t value)
{
	uint32_t old_hid0_ice = GetHID0_ICE();
	uint32_t old_hid0_dce = GetHID0_DCE();
	uint32_t old_hid0_abe = GetHID0_ABE();

	hid0 = (hid0 & ~HID0_MASK) | (value & HID0_MASK);

	if(CONFIG::L1_INSN_ENABLE && CONFIG::HAS_HID0_ICE)
	{
		if(!old_hid0_ice && GetHID0_ICE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling L1 Insn Cache" << Endl << EndDebugInfo;
			il1.Enable();
			icache_enabled = true;
		}
		
		if(old_hid0_ice && !GetHID0_ICE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling L1 Insn Cache" << Endl << EndDebugInfo;
			il1.Disable();
			icache_enabled = GetL2CR_L2E() ? true : false;
		}
	}
	
	if(CONFIG::L1_DATA_ENABLE && CONFIG::HAS_HID0_DCE)
	{
		if(!old_hid0_dce && GetHID0_DCE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling L1 Data Cache" << Endl << EndDebugInfo;
			dl1.Enable();
		}
		
		if(old_hid0_dce && !GetHID0_DCE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling L1 Data Cache" << Endl << EndDebugInfo;
			dl1.Disable();
		}
	}
	
	if(CONFIG::HAS_HID0_ICFI)
	{
		if(GetHID0_ICFI())
		{
			if(CONFIG::L1_INSN_ENABLE)
			{
				if(logger_import)
					(*logger_import) << DebugInfo << "Flash Invalidating L1 Insn Cache" << Endl << EndDebugInfo;
				il1.Invalidate();
			}
			Decoder<CONFIG>::InvalidateDecodingCache();
			ResetHID0_ICFI();
		}
	}

	if(CONFIG::HAS_HID0_DCFI)
	{
		if(GetHID0_DCFI())
		{
			if(CONFIG::L1_DATA_ENABLE)
			{
				if(logger_import)
					(*logger_import) << DebugInfo << "Flash Invalidating L1 Data Cache" << Endl << EndDebugInfo;
				dl1.Invalidate();
			}
			ResetHID0_DCFI();
		}
	}

	if(CONFIG::HAS_HID0_ABE)
	{
		if(!old_hid0_abe && GetHID0_ABE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling Address Only broadcast" << Endl << EndDebugInfo;
			mmu.EnableAddressOnlyBroadcast();
			if(CONFIG::L1_DATA_ENABLE)
			{
				dl1.EnableAddressOnlyBroadcast();
			}
			if(CONFIG::L2_ENABLE)
			{
				l2.EnableAddressOnlyBroadcast();
			}
		}

		if(old_hid0_abe && !GetHID0_ABE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling Address Only broadcast" << Endl << EndDebugInfo;
			mmu.DisableAddressOnlyBroadcast();
			if(CONFIG::L1_DATA_ENABLE)
			{
				dl1.DisableAddressOnlyBroadcast();
			}
			if(CONFIG::L2_ENABLE)
			{
				l2.DisableAddressOnlyBroadcast();
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetHID1(uint32_t value)
{
	uint32_t old_hid1_abe = GetHID1_ABE();

	hid1 = (hid1 & ~HID1_MASK) | (value & HID1_MASK);

	if(CONFIG::HAS_HID1_ABE)
	{
		if(!old_hid1_abe && GetHID1_ABE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling Address Only broadcast" << Endl << EndDebugInfo;
			mmu.EnableAddressOnlyBroadcast();
			if(CONFIG::L1_DATA_ENABLE)
			{
				dl1.EnableAddressOnlyBroadcast();
			}
			if(CONFIG::L2_ENABLE)
			{
				l2.EnableAddressOnlyBroadcast();
			}
		}

		if(old_hid1_abe && !GetHID1_ABE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling Address Only broadcast" << Endl << EndDebugInfo;
			mmu.DisableAddressOnlyBroadcast();
			if(CONFIG::L1_DATA_ENABLE)
			{
				dl1.DisableAddressOnlyBroadcast();
			}
			if(CONFIG::L2_ENABLE)
			{
				l2.DisableAddressOnlyBroadcast();
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetHID2(uint32_t value)
{
	uint32_t old_hid2_swt_en = GetHID2_SWT_EN();
	uint32_t old_hid2_high_bat_en = GetHID2_HIGH_BAT_EN();

	hid2 = (hid2 & ~HID2_MASK) | (value & HID2_MASK);

	if(CONFIG::HAS_HID2_SWT_EN)
	{
		if(!old_hid2_swt_en && GetHID2_SWT_EN())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling Software Table Search" << Endl << EndDebugInfo;
			mmu.EnableSoftwareTableSearch();
		}
	
		if(old_hid2_swt_en && !GetHID2_SWT_EN())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling Software Table Search" << Endl << EndDebugInfo;
			mmu.DisableSoftwareTableSearch();
		}
	}

	if(CONFIG::HAS_HID2_HIGH_BAT_EN)
	{
		if(!old_hid2_high_bat_en && GetHID2_HIGH_BAT_EN())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling IBAT[4-7] and DBAT[4-7]" << Endl << EndDebugInfo;
			mmu.EnableHighBAT();
		}
		
		if(old_hid2_high_bat_en && !GetHID2_HIGH_BAT_EN())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling IBAT[4-7] and DBAT[4-7]" << Endl << EndDebugInfo;
			mmu.DisableHighBAT();
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetL2CR(uint32_t value)
{
	uint32_t old_l2cr_l2e = GetL2CR_L2E();

	l2cr = value;

	if(CONFIG::L2_ENABLE && CONFIG::HAS_L2CR_L2E)
	{
		if(!old_l2cr_l2e && GetL2CR_L2E())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling L2 Cache" << Endl << EndDebugInfo;
			l2.Enable();
			icache_enabled = GetHID0_ICE() ? true : false;
		}
		
		if(old_l2cr_l2e && !GetL2CR_L2E())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling L2 Cache" << Endl << EndDebugInfo;
			l2.Disable();
			icache_enabled = GetHID0_ICE() ? true : false;
		}
	}

	if(CONFIG::HAS_L2CR_L2I)
	{
		if(GetL2CR_L2I())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Flash Invalidating L2 Cache" << Endl << EndDebugInfo;
			Decoder<CONFIG>::InvalidateDecodingCache();
			if(CONFIG::L2_ENABLE)
			{
				l2.Invalidate();
			}
			ResetL2CR_L2IP();
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetMSR(uint32_t value)
{
	uint32_t old_msr_ir = GetMSR_IR();
	uint32_t old_msr_dr = GetMSR_DR();
	uint32_t old_msr_fp = GetMSR_FP();
	uint32_t old_msr_pr = GetMSR_PR();
	uint32_t old_msr_le = GetMSR_LE();
	uint32_t old_msr_fe = (CONFIG::HAS_MSR_FE0 ? GetMSR_FE0() : 0) | (CONFIG::HAS_MSR_FE1 ? GetMSR_FE1() : 0);

	msr = (msr & ~MSR_MASK) | (value & MSR_MASK);

	if(CONFIG::HAS_MSR_IR)
	{
		if(!old_msr_ir && GetMSR_IR())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling IMMU" << Endl << EndDebugInfo;
			mmu.EnableIMMU();
		}
	
		if(old_msr_ir && !GetMSR_IR())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling IMMU" << Endl << EndDebugInfo;
			mmu.DisableIMMU();
		}
	}

	if(CONFIG::HAS_MSR_DR)
	{
		if(!old_msr_dr && GetMSR_DR())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling DMMU" << Endl << EndDebugInfo;
			mmu.EnableDMMU();
		}
		
		if(old_msr_dr && !GetMSR_DR())	
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling DMMU" << Endl << EndDebugInfo;
			mmu.DisableDMMU();
		}
	}

	if(CONFIG::HAS_MSR_FP)
	{
		if(!old_msr_fp && GetMSR_FP())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling FPU" << Endl << EndDebugInfo;
		}
	
		if(old_msr_fp && !GetMSR_FP())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling FPU" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_MSR_FE0 || CONFIG::HAS_MSR_FE1)
	{
		uint32_t msr_fe = (CONFIG::HAS_MSR_FE0 ? GetMSR_FE0() : 0) | (CONFIG::HAS_MSR_FE1 ? GetMSR_FE1() : 0);
		if(!old_msr_fe && msr_fe)
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Enabling FPU exception" << Endl << EndDebugInfo;
			fpu.EnableException();
		}
	
		if(old_msr_fe && !msr_fe)
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Disabling FPU exception" << Endl << EndDebugInfo;
			fpu.DisableException();
		}
	}

	if(CONFIG::HAS_MSR_PR)
	{
		if(!old_msr_pr && GetMSR_PR())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Switching to user privilege level" << Endl << EndDebugInfo;
		}
	
		if(old_msr_pr && !GetMSR_PR())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Switching to supervisor privilege level" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_MSR_LE)
	{
		if(!old_msr_le && GetMSR_LE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Switching to little endian" << Endl << EndDebugInfo;
		}
	
		if(old_msr_le && !GetMSR_LE())
		{
			if(logger_import)
				(*logger_import) << DebugInfo << "Switching to big endian" << Endl << EndDebugInfo;
		}
	}
}


template <class CONFIG>
void CPU<CONFIG>::SetDEC(uint32_t value)
{
	bool overflow = (!(dec & 0x80000000UL) && (value & 0x80000000UL));
	dec = value;

	if(overflow)
	{
		ReqDecrementerOverflow();
	}
}

template <class CONFIG>
string CPU<CONFIG>::GetObjectFriendlyName(address_t addr)
{
	stringstream sstr;
	
	const Symbol<address_t> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<address_t>::SYM_OBJECT);
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template <class CONFIG>
string CPU<CONFIG>::GetFunctionFriendlyName(address_t addr)
{
	stringstream sstr;
	
	const Symbol<address_t> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<address_t>::SYM_FUNC);
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template <class CONFIG>
unisim::util::debug::Register *CPU<CONFIG>::GetRegister(const char *name)
{
	map<string, unisim::util::debug::Register *>::iterator reg_iter = registers_registry.find(name);
	if(reg_iter != registers_registry.end())
	{
		return (*reg_iter).second;
	}

	unisim::util::debug::Register *mmu_reg = mmu.GetRegister(name);

	if(mmu_reg) return mmu_reg;

	unisim::util::debug::Register *fpu_reg = fpu.GetRegister(name);

	return fpu_reg;
}

template <class CONFIG>
const char *CPU<CONFIG>::GetArchitectureName() const
{
	return "powerpc";
}

template <class CONFIG>
inline void CPU<CONFIG>::ReadMemoryBuffer(address_t addr, void *buffer, uint32_t size)
{
	mmu.ReadDataMemory(addr, buffer, size, GetPrivilegeLevel());

	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, MemoryAccessReporting<address_t>::MT_DATA, addr, size);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::WriteMemoryBuffer(address_t addr, const void *buffer, uint32_t size)
{
#if 0
	if(printk_buf_addr && addr >= printk_buf_addr && addr < printk_buf_addr + printk_buf_size)
	{
		uint32_t i;
		cout << "\033[31m";
		for(i = 0; i < size; i++)
		{
			char c = ((const char *) buffer)[i];
			if(c != 0) {
				//cerr << c;
				cout << c;
			}
		}
		cout << "\033[37m";
	}
#endif
	mmu.WriteDataMemory(addr, buffer, size, GetPrivilegeLevel());

	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_WRITE, MemoryAccessReporting<address_t>::MT_DATA, addr, size);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ReadMemory8(address_t ea, uint8_t& value)
{
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x7 : ea;
	ReadMemoryBuffer(munged_ea, &value, 1);
}

template <class CONFIG>
void CPU<CONFIG>::ReadMemory16(address_t ea, uint16_t& value)
{
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x6 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 2)
	{
		ReadMemoryBuffer(munged_ea, &value, 2);
#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(value);
#endif
		return;
	}

	// memory access crosses a 64-bit boundary
	ReadMemoryBuffer(munged_ea, (uint8_t *)(&value), 1);
	ReadMemoryBuffer(munged_ea + 1, (uint8_t *)(&value) + 1, 1);
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(value);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::ReadMemoryByteReverse16(address_t ea, uint16_t& value)
{
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x6 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 2)
	{
		ReadMemoryBuffer(munged_ea, &value, 2);
#if BYTE_ORDER == BIG_ENDIAN
		BSwap(value);
#endif
		return;
	}

	// memory access crosses a 64-bit boundary
	ReadMemoryBuffer(munged_ea, (uint8_t *)(&value), 1);
	ReadMemoryBuffer(munged_ea + 1, (uint8_t *)(&value) + 1, 1);
#if BYTE_ORDER == BIG_ENDIAN
	BSwap(value);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::ReadMemory32(address_t ea, uint32_t& value)
{
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x4 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 4)
	{
		ReadMemoryBuffer(munged_ea, &value, 4);
#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(value);
#endif
		return;
	}

	// memory access crosses a 64-bit boundary
	ReadMemoryBuffer(munged_ea, (uint8_t *)(&value), size_to_boundary);
	ReadMemoryBuffer(munged_ea + size_to_boundary, (uint8_t *)(&value) + size_to_boundary, 4 - size_to_boundary);
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(value);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::ReadMemoryByteReverse32(address_t ea, uint32_t& value)
{
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x4 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 4)
	{
		ReadMemoryBuffer(munged_ea, &value, 4);
#if BYTE_ORDER == BIG_ENDIAN
		BSwap(value);
#endif
		return;
	}

	// memory access crosses a 64-bit boundary
	ReadMemoryBuffer(munged_ea, (uint8_t *)(&value), size_to_boundary);
	ReadMemoryBuffer(munged_ea + size_to_boundary, (uint8_t *)(&value) + size_to_boundary, 4 - size_to_boundary);
#if BYTE_ORDER == BIG_ENDIAN
	BSwap(value);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::ReadMemory64(address_t ea, uint64_t& value)
{
	address_t munged_ea = ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 8)
	{
		ReadMemoryBuffer(ea, &value, 8);
#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(value);
#endif
		return;
	}

	// memory access crosses a 64-bit boundary
	ReadMemoryBuffer(munged_ea, (uint8_t *)(&value), size_to_boundary);
	ReadMemoryBuffer(munged_ea + size_to_boundary, (uint8_t *)(&value) + size_to_boundary, 8 - size_to_boundary);
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(value);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::WriteMemory8(address_t ea, uint8_t value)
{
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x7 : ea;
	WriteMemoryBuffer(munged_ea, &value, 1);
}

template <class CONFIG>
void CPU<CONFIG>::WriteMemory16(address_t ea, uint16_t value)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(value);
#endif
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x6 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 2)
	{
		WriteMemoryBuffer(munged_ea, &value, 2);
		return;
	}

	// memory access crosses a 64-bit boundary
	WriteMemoryBuffer(munged_ea, (uint8_t *)(&value), 1);
	WriteMemoryBuffer(munged_ea + 1, (uint8_t *)(&value) + 1, 1);
}

template <class CONFIG>
void CPU<CONFIG>::WriteMemoryByteReverse16(address_t ea, uint16_t value)
{
#if BYTE_ORDER == BIG_ENDIAN
	BSwap(value);
#endif
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x6 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 2)
	{
		WriteMemoryBuffer(munged_ea, &value, 2);
		return;
	}

	// memory access crosses a 64-bit boundary
	WriteMemoryBuffer(munged_ea, (uint8_t *)(&value), 1);
	WriteMemoryBuffer(munged_ea + 1, (uint8_t *)(&value) + 1, 1);
}

template <class CONFIG>
void CPU<CONFIG>::WriteMemory32(address_t ea, uint32_t value)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(value);
#endif
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x4 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 4)
	{
		WriteMemoryBuffer(munged_ea, &value, 4);
		return;
	}

	// memory access crosses a 64-bit boundary
	WriteMemoryBuffer(munged_ea, (uint8_t *)(&value), size_to_boundary);
	WriteMemoryBuffer(munged_ea + size_to_boundary, (uint8_t *)(&value) + size_to_boundary, 4 - size_to_boundary);
}

template <class CONFIG>
void CPU<CONFIG>::WriteMemoryByteReverse32(address_t ea, uint32_t value)
{
#if BYTE_ORDER == BIG_ENDIAN
	BSwap(value);
#endif
	address_t munged_ea = GetMSR_LE() ? ea ^ 0x4 : ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 4)
	{
		WriteMemoryBuffer(munged_ea, &value, 4);
		return;
	}

	// memory access crosses a 64-bit boundary
	WriteMemoryBuffer(munged_ea, (uint8_t *)(&value), size_to_boundary);
	WriteMemoryBuffer(munged_ea + size_to_boundary, (uint8_t *)(&value) + size_to_boundary, 4 - size_to_boundary);
}

template <class CONFIG>
void CPU<CONFIG>::WriteMemory64(address_t ea, uint64_t value)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(value);
#endif
	address_t munged_ea = ea;
	uint32_t size_to_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	if(size_to_boundary >= 8)
	{
		WriteMemoryBuffer(ea, &value, 8);
		return;
	}

	// memory access crosses a 64-bit boundary
	WriteMemoryBuffer(munged_ea, (uint8_t *)(&value), size_to_boundary);
	WriteMemoryBuffer(munged_ea + size_to_boundary, (uint8_t *)(&value) + size_to_boundary, 8 - size_to_boundary);
}

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(address_t addr, void *buffer, uint32_t size)
{
	return mmu.ReadMemory(addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	return mmu.WriteMemory(addr, buffer, size);
}

template <class CONFIG>
string CPU<CONFIG>::DebugDisasm(address_t addr, address_t& next_addr)
{
	stringstream sstr;
	Operation<CONFIG> *operation;

	physical_address_t physical_addr;
	uint32_t insn;

	MMUControl prev_mmc = mmu.GetControl();
	mmu.EnableDebug();

	insn = mmu.ReadInsnMemory(addr, GetPrivilegeLevel(), physical_addr);

	mmu.SetControl(prev_mmc);

	operation = Decoder<CONFIG>::Decode(addr, insn);

	/* disassemble the instruction */
	sstr << "0x" << hex;
	sstr.fill('0'); sstr.width(8); 
	sstr << operation->GetEncoding() << std::dec << " ";
	operation->disasm(this, sstr);

	next_addr = addr + 4;
	return sstr.str();
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevRead(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc)
{
	dl1.BusRead(physical_addr, buffer, size);
	return BS_OK;
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevReadX(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc)
{
	dl1.BusReadX(physical_addr, buffer, size);
	return BS_OK;
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, BusControl bc)
{
	return BS_OK;
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevInvalidateBlock(physical_address_t physical_addr, BusControl bc)
{
	dl1.BusInvalidateBlock(physical_addr);
	return BS_OK;
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevFlushBlock(physical_address_t physical_addr, BusControl bc)
{
	dl1.BusFlushBlock(physical_addr);
	return BS_OK;
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevZeroBlock(physical_address_t physical_addr, BusControl bc)
{
	return BS_OK;
}

template <class CONFIG>
BusStatus CPU<CONFIG>::DevInvalidateTLBEntry(physical_address_t addr, BusControl bc)
{
	mmu.InvalidateITLBSet(addr);
	mmu.InvalidateDTLBSet(addr);
	return BS_OK;
}

template <class CONFIG>
void CPU<CONFIG>::DevOnBusError(BusStatus bs)
{
}

/* PowerPC Linux OS Interface */
template <class CONFIG>
void CPU<CONFIG>::ARMLinuxOSExit(int ret)
{
	if(logger_import)
		(*logger_import) << DebugInfo << "Program exited with code " << ret << Endl << EndDebugInfo;
	Stop(ret);
}

/* Endian interface */
template <class CONFIG>
endian_type CPU<CONFIG>::GetEndianess()
{
	return E_BIG_ENDIAN;
}

/* Data Cache management */
template <class CONFIG>
void CPU<CONFIG>::Dcba(address_t addr)
{
	mmu.ZeroDataCacheBlock(addr, GetPrivilegeLevel());
}

template <class CONFIG>
void CPU<CONFIG>::Dcbf(address_t addr)
{
	mmu.FlushDataCacheBlock(addr, GetPrivilegeLevel());
}

template <class CONFIG>
void CPU<CONFIG>::Dcbi(address_t addr)
{
	mmu.InvalidateDataCacheBlock(addr, GetPrivilegeLevel());
}

template <class CONFIG>
void CPU<CONFIG>::Dcbst(address_t addr)
{
	mmu.FlushDataCacheBlock(addr, GetPrivilegeLevel());
}

template <class CONFIG>
void CPU<CONFIG>::Dcbz(address_t addr)
{
	mmu.ZeroDataCacheBlock(addr, GetPrivilegeLevel());
}

/* Instruction Cache Management */
template <class CONFIG>
void CPU<CONFIG>::Icbi(address_t addr)
{
	mmu.InvalidateInsnCacheBlock(addr, GetPrivilegeLevel());
	Decoder<CONFIG>::InvalidateDecodingCacheEntry(addr);
}

/* TLB management */
template <class CONFIG>
void CPU<CONFIG>::Tlbia()
{
	if(CONFIG::MODEL == MPC740 || CONFIG::MODEL == MPC745 || CONFIG::MODEL == MPC750 || CONFIG::MODEL == MPC755)
	{
		throw IllegalInstructionException<CONFIG>();
	}

	mmu.InvalidateDTLB();
	mmu.InvalidateITLB();
}

template <class CONFIG>
void CPU<CONFIG>::Tlbie(address_t addr)
{
	mmu.InvalidateITLBSet(addr);
	mmu.InvalidateDTLBSet(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Tlbld(address_t addr)
{
	if(CONFIG::MODEL == MPC745 || CONFIG::MODEL == MPC755)
	{
		mmu.LoadDTLBEntry(addr, GetSRR1_WAY(), GetDCMP(), GetRPA());
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbli(address_t addr)
{
	if(CONFIG::MODEL == MPC745 || CONFIG::MODEL == MPC755)
	{
		mmu.LoadITLBEntry(addr, GetSRR1_WAY(), GetICMP(), GetRPA());
	}
}


/* Linked Load-Store */
template <class CONFIG>
void CPU<CONFIG>::Lwarx(address_t addr, uint32_t& value)
{
	ReadMemory32(addr, value);

	reserve = true;
	reserve_addr = addr;
}

template <class CONFIG>
void CPU<CONFIG>::Stwcx(address_t addr, uint32_t value)
{
	if(reserve)
	{
		if(reserve_addr == addr)
		{
			WriteMemory32(addr, value);

			/* clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO] */
			SetCR((GetCR() & 0x2fffffffUL) | (GetXER_SO() ? 0x10000000UL : 0) | 0x20000000UL);
			reserve = false;
			return;
		}
	}

	/* clear CR0 and copy XER[SO] to CR0[SO] */
	SetCR((GetCR() & 0x0fffffffUL) | (GetXER_SO() ? 0x10000000UL : 0));
}

template <class CONFIG>
void CPU<CONFIG>::ReqAsynchronousInterrupt()
{
	asynchronous_interrupt = true;
}

template <class CONFIG>
void CPU<CONFIG>::AckAsynchronousInterrupt()
{
	asynchronous_interrupt = false;
	
	if(CONFIG::HAS_DECREMENTER_OVERFLOW) asynchronous_interrupt |= decrementer_overflow;
	if(CONFIG::HAS_EXTERNAL_INTERRUPT) asynchronous_interrupt |= external_interrupt;
	if(CONFIG::HAS_SOFT_RESET) asynchronous_interrupt |= soft_reset;
	if(CONFIG::HAS_HARD_RESET) asynchronous_interrupt |= hard_reset;
	if(CONFIG::HAS_MCP) asynchronous_interrupt |= mcp;
	if(CONFIG::HAS_TEA) asynchronous_interrupt |= tea;
	if(CONFIG::HAS_SMI) asynchronous_interrupt |= smi;
	if(CONFIG::HAS_THERMAL_MANAGEMENT_INTERRUPT) asynchronous_interrupt |= thermal_management_interrupt;
	if(CONFIG::HAS_PERFORMANCE_MONITOR_INTERRUPT) asynchronous_interrupt |= performance_monitor_interrupt;
}

template <class CONFIG>
void CPU<CONFIG>::ReqDecrementerOverflow()
{
	decrementer_overflow = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckDecrementerOverflow()
{
	decrementer_overflow = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqExternalInterrupt()
{
	external_interrupt = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckExternalInterrupt()
{
	external_interrupt = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqHardReset()
{
	hard_reset = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckHardReset()
{
	hard_reset = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqSoftReset()
{
	soft_reset = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckSoftReset()
{
	soft_reset = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::Stop(int ret)
{
	exit(ret);
}

template <class CONFIG>
void CPU<CONFIG>::Sync()
{
}

template <class CONFIG>
void CPU<CONFIG>::ReqMCP()
{
	mcp = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckMCP()
{
	mcp = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqTEA()
{
	tea = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckTEA()
{
	tea = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqSMI()
{
	smi = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckSMI()
{
	smi = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqThermalManagementInterrupt()
{
	thermal_management_interrupt = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckThermalManagementInterrupt()
{
	thermal_management_interrupt = false;
	AckAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::ReqPerformanceMonitorInterrupt()
{
	performance_monitor_interrupt = true;
	ReqAsynchronousInterrupt();
}

template <class CONFIG>
void CPU<CONFIG>::AckPerformanceMonitorInterrupt()
{
	performance_monitor_interrupt = false;
	AckAsynchronousInterrupt();
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
