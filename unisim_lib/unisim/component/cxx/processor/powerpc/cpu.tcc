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

#include <unisim/component/cxx/processor/powerpc/exception.tcc>
#include <unisim/component/cxx/cache/cache.tcc>
#include <unisim/component/cxx/tlb2/tlb.tcc>
#include <unisim/util/queue/queue.tcc>

#include <sstream>
#include <stdexcept>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using namespace std;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;



template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent) :
	Object(name, parent),
	Client<Loader<typename CONFIG::physical_address_t> >(name, parent),
	Client<SymbolTableLookup<typename CONFIG::address_t> >(name, parent),
	Client<DebugControl<typename CONFIG::address_t> >(name, parent),
	Client<MemoryAccessReporting<typename CONFIG::address_t> >(name, parent),
	Client<TrapReporting>(name, parent),
	Service<MemoryAccessReportingControl>(name, parent),
	Service<Disassembly<typename CONFIG::address_t> >(name, parent),
	Service<unisim::service::interfaces::Registers>(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Service<MemoryInjection<typename CONFIG::address_t> >(name, parent),
	Service<CPULinuxOS>(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
	Client<LinuxOS>(name, parent),
	Client<Logger>(name, parent),
	Client<CachePowerEstimator>(name, parent),
	Client<PowerMode>(name, parent),
	Service<Synchronizable>(name, parent),
	disasm_export("disasm-export", this),
	registers_export("registers-export", this),
	memory_export("memory-export", this),
	memory_injection_export("memory-injection-export", this),
	cpu_linux_os_export("cpu-linux-os-export", this),
	memory_access_reporting_control_export("memory_access_reporting_control_export", this),
	kernel_loader_import("kernel-loader-import", this),
	debug_control_import("debug-control-import", this),
	memory_access_reporting_import("memory-access-reporting-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	memory_import("memory-import", this),
	linux_os_import("linux-os-import", this),
	trap_reporting_import("trap-reporting-import", this),
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
	synchronizable_export("synchronizable-export", this),
	requires_memory_access_reporting(true),
	requires_finished_instruction_reporting(true),
	dl1(),
	il1(),
	l2(),
	itlb(),
	dtlb(),
	fpu("fpu", this),
	cpu_cycle_time(0),
	voltage(0),
	bus_cycle_time(0),
	max_inst(0xffffffffffffffffULL),
	num_insn_in_prefetch_buffer(0),
	cur_insn_in_prefetch_buffer(0),
	verbose_all(false),
	verbose_step(false),
	verbose_dtlb(false),
	verbose_dl1(false),
	verbose_il1(false),
	verbose_l2(false),
	verbose_load(false),
	verbose_store(false),
	verbose_read_memory(false),
	verbose_write_memory(false),
	verbose_exception(false),
	verbose_set_msr(false),
	verbose_set_hid0(false),
	verbose_set_hid1(false),
	verbose_set_hid2(false),
	verbose_set_l2cr(false),
	trap_on_instruction_counter(0xffffffffffffffffULL),
	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time),
	param_voltage("voltage", this, voltage),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time),
	param_max_inst("max-inst", this, max_inst),
	param_verbose_all("verbose-all", this, verbose_all),
	param_verbose_step("verbose-step", this, verbose_step),
	param_verbose_dtlb("verbose-dtlb", this, verbose_dtlb),
	param_verbose_dl1("verbose-dl1", this, verbose_dl1),
	param_verbose_il1("verbose-il1", this, verbose_il1),
	param_verbose_l2("verbose-l2", this, verbose_l2),
	param_verbose_load("verbose-load", this, verbose_load),
	param_verbose_store("verbose-store", this, verbose_store),
	param_verbose_read_memory("verbose-read-memory", this, verbose_read_memory),
	param_verbose_write_memory("verbose-write-memory", this, verbose_write_memory),
	param_verbose_exception("verbose-exception", this, verbose_exception),
	param_verbose_set_msr("verbose-set-msr", this, verbose_set_msr),
	param_verbose_set_hid0("verbose-set-hid0", this, verbose_set_hid0),
	param_verbose_set_hid1("verbose-set-hid1", this, verbose_set_hid1),
	param_verbose_set_hid2("verbose-set-hid2", this, verbose_set_hid2),
	param_verbose_set_l2cr("verbose-set-l2cr", this, verbose_set_l2cr),
	param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter),
	stat_instruction_counter("instruction-counter", this, instruction_counter),
	stat_bus_cycle("bus-cycle", this, bus_cycle),
	stat_cpu_cycle("cpu-cycle", this, cpu_cycle)
{
	Object::SetupDependsOn(internal_il1_power_mode_import);
	Object::SetupDependsOn(internal_dl1_power_mode_import);
	Object::SetupDependsOn(internal_l2_power_mode_import);
	Object::SetupDependsOn(internal_itlb_power_mode_import);
	Object::SetupDependsOn(internal_dtlb_power_mode_import);
	Object::SetupDependsOn(logger_import);


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

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "dbat" << i << "u";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dbatu[i]);
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "dbat" << i << "l";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dbatl[i]);
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "ibat" << i << "u";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &ibatu[i]);
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "ibat" << i << "l";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &ibatl[i]);
	}

	for(i = 0; i < 16; i++)
	{
		stringstream sstr;
		sstr << "sr" << i;
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sr[i]);
	}

	registers_registry["sdr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("sdr1", &sdr1);

	fpu.logger_import >> fpu_logger_import;
	
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

/*	cerr << "num_il1_accesses = " << num_il1_accesses << endl;
	cerr << "num_il1_misses = " << num_il1_misses << endl;
	cerr << "il1 miss rate = " << (double) num_il1_misses / num_il1_accesses << endl;
	cerr << "num_dl1_accesses = " << num_dl1_accesses << endl;
	cerr << "num_dl1_misses = " << num_dl1_misses << endl;
	cerr << "dl1 miss rate = " << (double) num_dl1_misses / num_dl1_accesses << endl;
	cerr << "num_l2_accesses = " << num_l2_accesses << endl;
	cerr << "num_l2_misses = " << num_l2_misses << endl;
	cerr << "l2 miss rate = " << (double) num_l2_misses / num_l2_accesses << endl;*/
}

template <class CONFIG>
bool CPU<CONFIG>::Setup()
{
/*	
	unsigned int min_cycle_time = 0;

	if(CONFIG::ITLB_ENABLE)
	{
		if(!internal_itlb_power_mode_import) return false;
		unsigned int itlb_min_cycle_time = internal_itlb_power_mode_import->GetMinCycleTime();
		if(itlb_min_cycle_time > 0 && itlb_min_cycle_time > min_cycle_time) min_cycle_time = itlb_min_cycle_time;
		unsigned int itlb_default_voltage = internal_itlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = itlb_default_voltage;
	}

	if(CONFIG::DTLB_ENABLE)
	{
		if(!internal_dtlb_power_mode_import) return false;
		unsigned int dtlb_min_cycle_time = internal_dtlb_power_mode_import->GetMinCycleTime();
		if(dtlb_min_cycle_time > 0 && dtlb_min_cycle_time > min_cycle_time) min_cycle_time = dtlb_min_cycle_time;
		unsigned int dtlb_default_voltage = internal_dtlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = dtlb_default_voltage;
	}
	
	if(CONFIG::L1_INSN_ENABLE)
	{
		if(!internal_il1_power_mode_import) return false;
		unsigned int il1_min_cycle_time = internal_il1_power_mode_import->GetMinCycleTime();
		if(il1_min_cycle_time > 0 && il1_min_cycle_time > min_cycle_time) min_cycle_time = il1_min_cycle_time;
		unsigned int il1_default_voltage = internal_il1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = il1_default_voltage;
	}
	
	if(CONFIG::L1_DATA_ENABLE)
	{
		if(!internal_dl1_power_mode_import) return false;
		unsigned int dl1_min_cycle_time = internal_dl1_power_mode_import->GetMinCycleTime();
		if(dl1_min_cycle_time > 0 && dl1_min_cycle_time > min_cycle_time) min_cycle_time = dl1_min_cycle_time;
		unsigned int dl1_default_voltage = internal_dl1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = dl1_default_voltage;
	}
	
	if(CONFIG::L2_ENABLE)
	{
		if(!internal_l2_power_mode_import) return false;
		unsigned int l2_min_cycle_time = internal_l2_power_mode_import->GetMinCycleTime();
		if(l2_min_cycle_time > 0 && l2_min_cycle_time > min_cycle_time) min_cycle_time = l2_min_cycle_time;
		unsigned int l2_default_voltage = internal_l2_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = l2_default_voltage;
	}
	
	if(min_cycle_time > 0)
	{
		if(cpu_cycle_time > 0)
		{
			if(cpu_cycle_time < min_cycle_time)
			{
				if(logger_import)
				{
					(*logger_import) << DebugWarning;
					(*logger_import) << "A cycle time of " << cpu_cycle_time << " ps is too low for the simulated hardware !" << Endl;
					(*logger_import) << "cpu cycle time should be >= " << min_cycle_time << " ps." << Endl;
					(*logger_import) << EndDebugWarning;
				}
			}
		}
		else
		{
			cpu_cycle_time = min_cycle_time;
		}
	}
	else
	{
		if(cpu_cycle_time <= 0)
		{
			// We can't provide a valid configuration automatically
			if(logger_import)
			{
				(*logger_import) << DebugError;
				(*logger_import) << "user must provide a cpu cycle time > 0" << Endl;
				(*logger_import) << EndDebugError;
			}
			return false;
		}
	}

	if(logger_import)
	{
		(*logger_import) << DebugInfo << "cpu cycle time of " << cpu_cycle_time << " ps" << Endl << EndDebugInfo;
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

		if(bus_cycle_time > 0)
			(*logger_import) << "bus cycle time of " << bus_cycle_time << " ps" << Endl;
		
		(*logger_import) << EndDebugInfo;
		
		if(bus_cycle_time <= 0)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "bus cycle time must be > 0" << Endl;
			(*logger_import) << EndDebugError;
		}
	}

	if(internal_il1_power_mode_import)
		internal_il1_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	
	if(internal_dl1_power_mode_import)
		internal_dl1_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	
	if(internal_l2_power_mode_import)
		internal_l2_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	
	*/
	if(linux_os_import)
	{
		EnableFPU();
		SetUserPrivilegeLevel();
		EnableDataCache();
		EnableInsnCache();
		EnableAddressBroadcast();
		EnableL2Cache();
	}
	
	if(kernel_loader_import)
	{
		EnableFPU();
		EnableDataCache();
		EnableInsnCache();
		EnableAddressBroadcast();
		EnableL2Cache();
	}

	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}
	
	num_il1_accesses = 0;
	num_il1_misses = 0;
	num_dl1_accesses = 0;
	num_dl1_misses = 0;
	num_l2_accesses = 0;
	num_l2_misses = 0;

	return true;
}

//=====================================================================
//=         memory access reporting control interface methods   START =
//=====================================================================

template<class CONFIG>
void 
CPU<CONFIG>::RequiresMemoryAccessReporting(bool report) {
	requires_memory_access_reporting = report;
}

template<class CONFIG>
void 
CPU<CONFIG>::RequiresFinishedInstructionReporting(bool report) {
	requires_finished_instruction_reporting = report;
}

//=====================================================================
//=         memory access reporting control interface methods   END   =
//=====================================================================

template <class CONFIG>
void CPU<CONFIG>::OnDisconnect()
{
}

template <class CONFIG>
void CPU<CONFIG>::Reset()
{
	int i;

	bus_cycle = 0;
	cpu_cycle = 0;
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
	
	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		SetDBATU(i, 0);
		SetDBATL(i, 0);
		SetIBATU(i, 0);
		SetIBATL(i, 0);
	}
	for(i = 0; i < 16; i++)
	{
		SetSR(i, 0);
	}
	SetSDR1(0);
	InvalidateDTLB();
	InvalidateITLB();

	if(CONFIG::IL1_CONFIG::ENABLE)
	{
		InvalidateIL1();
	}
	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		InvalidateDL1();
	}
	if(CONFIG::L2_CONFIG::ENABLE)
	{
		InvalidateL2();
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
void CPU<CONFIG>::Step()
{
	if(debug_control_import)
	{
		do
		{
			typename DebugControl<address_t>::DebugCommand dbg_cmd;

			dbg_cmd = debug_control_import->FetchDebugCommand(GetCIA());
	
			if(dbg_cmd == DebugControl<address_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<address_t>::DBG_SYNC)
			{
				Synchronize();
				continue;
			}

			if(dbg_cmd == DebugControl<address_t>::DBG_KILL) Stop(0);
			if(dbg_cmd == DebugControl<address_t>::DBG_RESET)
			{
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
		
		if(CONFIG::PREFETCH_BUFFER_ENABLE)
		{
			if(cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer)
			{
				uint32_t size_to_block_boundary = IsInsnCacheEnabled() ? CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (addr & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1)) : FSB_WIDTH - (addr & (FSB_WIDTH - 1));
				uint32_t size_to_prefetch = size_to_block_boundary > (4 * CONFIG::NUM_PREFETCH_BUFFER_ENTRIES) ? CONFIG::NUM_PREFETCH_BUFFER_ENTRIES / 4 : size_to_block_boundary;
				// refill the prefetch buffer with up to one cache line, not much
				Fetch(addr, prefetch_buffer, size_to_prefetch);
				num_insn_in_prefetch_buffer = size_to_prefetch / 4;
				cur_insn_in_prefetch_buffer = 0;
			}
			insn = prefetch_buffer[cur_insn_in_prefetch_buffer];
		}
		else
		{
			Fetch(addr, &insn, 4);
		}

#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(insn);
#endif

		if(requires_memory_access_reporting) 
		{
			if(memory_access_reporting_import)
			{
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, MemoryAccessReporting<address_t>::MT_INSN, addr, 4);
			}
		}
		
		if(CONFIG::IABR_ENABLE && CONFIG::HAS_IABR)
		{
			/* Check for instruction address breakpoint */
			if(GetIABR_BE() && GetIABR_TE() == GetMSR_IR() && ((GetCIA() >> 2) & 0x3fffffffUL) == GetIABR_ADDR())
			{
				throw InstructionAddressBreakpointException<CONFIG>();
			}
		}

		operation = Decoder<CONFIG>::Decode(addr, insn);

		if(IsVerboseStep())
		{
			stringstream sstr;
			operation->disasm(this, sstr);
			(*logger_import) << DebugInfo << "#" << instruction_counter << ":0x" << Hex << addr << Dec << ":" << sstr.str() << Endl << EndDebugInfo;
		}

		/* execute the instruction */
		operation->execute(this);

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
	if(CONFIG::PREFETCH_BUFFER_ENABLE)
	{
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
	}

	SetCIA(GetNIA());

	/* update the instruction counter */
	instruction_counter++;
	if(trap_reporting_import && instruction_counter == trap_on_instruction_counter)
	{
		trap_reporting_import->ReportTrap();
	}
	
	if(requires_finished_instruction_reporting)
	{
		if(memory_access_reporting_import)
		{
			memory_access_reporting_import->ReportFinishedInstruction(GetNIA());
		}
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

	if(CONFIG::IL1_CONFIG::ENABLE && CONFIG::HAS_HID0_ICE)
	{
		if(!old_hid0_ice && GetHID0_ICE())
		{
			if(IsVerboseSetHID0())
				(*logger_import) << DebugInfo << "Enabling L1 Insn Cache" << Endl << EndDebugInfo;
		}
		
		if(old_hid0_ice && !GetHID0_ICE())
		{
			if(IsVerboseSetHID0())
				(*logger_import) << DebugInfo << "Disabling L1 Insn Cache" << Endl << EndDebugInfo;
		}
	}
	
	if(CONFIG::DL1_CONFIG::ENABLE && CONFIG::HAS_HID0_DCE)
	{
		if(!old_hid0_dce && GetHID0_DCE())
		{
			if(IsVerboseSetHID0())
				(*logger_import) << DebugInfo << "Enabling L1 Data Cache" << Endl << EndDebugInfo;
		}
		
		if(old_hid0_dce && !GetHID0_DCE())
		{
			if(IsVerboseSetHID0())
				(*logger_import) << DebugInfo << "Disabling L1 Data Cache" << Endl << EndDebugInfo;
		}
	}
	
	if(CONFIG::HAS_HID0_ICFI)
	{
		if(GetHID0_ICFI())
		{
			if(CONFIG::IL1_CONFIG::ENABLE)
			{
				if(IsVerboseSetHID0())
					(*logger_import) << DebugInfo << "Flash Invalidating L1 Insn Cache" << Endl << EndDebugInfo;
				InvalidateIL1();
			}
			Decoder<CONFIG>::InvalidateDecodingCache();
			ResetHID0_ICFI();
		}
	}

	if(CONFIG::HAS_HID0_DCFI)
	{
		if(GetHID0_DCFI())
		{
			if(CONFIG::DL1_CONFIG::ENABLE)
			{
				if(IsVerboseSetHID0())
					(*logger_import) << DebugInfo << "Flash Invalidating L1 Data Cache" << Endl << EndDebugInfo;
				InvalidateDL1();
			}
			ResetHID0_DCFI();
		}
	}

	if(CONFIG::HAS_HID0_ABE)
	{
		if(!old_hid0_abe && GetHID0_ABE())
		{
			if(IsVerboseSetHID0())
				(*logger_import) << DebugInfo << "Enabling Address Only broadcast" << Endl << EndDebugInfo;
		}

		if(old_hid0_abe && !GetHID0_ABE())
		{
			if(IsVerboseSetHID0())
				(*logger_import) << DebugInfo << "Disabling Address Only broadcast" << Endl << EndDebugInfo;
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
			if(IsVerboseSetHID1())
				(*logger_import) << DebugInfo << "Enabling Address Only broadcast" << Endl << EndDebugInfo;
		}

		if(old_hid1_abe && !GetHID1_ABE())
		{
			if(IsVerboseSetHID1())
				(*logger_import) << DebugInfo << "Disabling Address Only broadcast" << Endl << EndDebugInfo;
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
			if(IsVerboseSetHID2())
				(*logger_import) << DebugInfo << "Enabling Software Table Search" << Endl << EndDebugInfo;
		}
	
		if(old_hid2_swt_en && !GetHID2_SWT_EN())
		{
			if(IsVerboseSetHID2())
				(*logger_import) << DebugInfo << "Disabling Software Table Search" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_HID2_HIGH_BAT_EN)
	{
		if(!old_hid2_high_bat_en && GetHID2_HIGH_BAT_EN())
		{
			if(IsVerboseSetHID2())
				(*logger_import) << DebugInfo << "Enabling IBAT[4-7] and DBAT[4-7]" << Endl << EndDebugInfo;
		}
		
		if(old_hid2_high_bat_en && !GetHID2_HIGH_BAT_EN())
		{
			if(IsVerboseSetHID2())
				(*logger_import) << DebugInfo << "Disabling IBAT[4-7] and DBAT[4-7]" << Endl << EndDebugInfo;
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetSupervisorPrivilegeLevel()
{
	if(CONFIG::HAS_MSR_PR) ResetMSR_PR();
}

template <class CONFIG>
void CPU<CONFIG>::SetUserPrivilegeLevel()
{
	if(CONFIG::HAS_MSR_PR) SetMSR_PR();
}

template <class CONFIG>
void CPU<CONFIG>::EnableFPU()
{
	if(CONFIG::HAS_MSR_FP) SetMSR_FP();   // enable floating point unit
}

template <class CONFIG>
void CPU<CONFIG>::DisableFPU()
{
	if(CONFIG::HAS_MSR_FP) ResetMSR_FP();   // disable floating point unit
}

template <class CONFIG>
void CPU<CONFIG>::EnableDataCache()
{
	if(CONFIG::HAS_HID0_DCE) SetHID0_DCE();
}

template <class CONFIG>
void CPU<CONFIG>::DisableDataCache()
{
	if(CONFIG::HAS_HID0_DCE) ResetHID0_DCE();
}

template <class CONFIG>
void CPU<CONFIG>::EnableInsnCache()
{
	if(CONFIG::HAS_HID0_ICE) SetHID0_ICE();
}

template <class CONFIG>
void CPU<CONFIG>::DisableInsnCache()
{
	if(CONFIG::HAS_HID0_ICE) SetHID0_ICE();
}

template <class CONFIG>
void CPU<CONFIG>::EnableL2Cache()
{
	if(CONFIG::HAS_L2CR_L2E) SetL2CR_L2E();
}

template <class CONFIG>
void CPU<CONFIG>::DisableL2Cache()
{
	if(CONFIG::HAS_L2CR_L2E) ResetL2CR_L2E();
}

template <class CONFIG>
void CPU<CONFIG>::EnableAddressBroadcast()
{
	// enable address only broadcast
	if(CONFIG::HAS_HID0_ABE) SetHID0_ABE();
	if(CONFIG::HAS_HID1_ABE) SetHID1_ABE();
}

template <class CONFIG>
void CPU<CONFIG>::DisableAddressBroadcast()
{
	// enable address only broadcast
	if(CONFIG::HAS_HID0_ABE) ResetHID0_ABE();
	if(CONFIG::HAS_HID1_ABE) ResetHID1_ABE();
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsDataCacheEnabled()
{
	return CONFIG::DL1_CONFIG::ENABLE && (!CONFIG::HAS_HID0_DCE || (CONFIG::HAS_HID0_DCE && GetHID0_DCE()));
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsInsnCacheEnabled()
{
	return CONFIG::IL1_CONFIG::ENABLE && (!CONFIG::HAS_HID0_ICE || (CONFIG::HAS_HID0_ICE && GetHID0_ICE()));
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsL2CacheEnabled()
{
	return CONFIG::L2_CONFIG::ENABLE && (!CONFIG::HAS_L2CR_L2E || (CONFIG::HAS_L2CR_L2E && GetL2CR_L2E()));
}


template <class CONFIG>
inline bool CPU<CONFIG>::IsAddressBroadcastEnabled()
{
	return (!CONFIG::HAS_HID0_ABE || (CONFIG::HAS_HID0_ABE &&  GetHID0_ABE())) && (!CONFIG::HAS_HID1_ABE || (CONFIG::HAS_HID1_ABE &&  GetHID1_ABE()));
}

template <class CONFIG>
void CPU<CONFIG>::SetL2CR(uint32_t value)
{
	uint32_t old_l2cr_l2e = GetL2CR_L2E();

	l2cr = value;

	if(CONFIG::L2_CONFIG::ENABLE && CONFIG::HAS_L2CR_L2E)
	{
		if(!old_l2cr_l2e && GetL2CR_L2E())
		{
			if(IsVerboseSetL2CR())
				(*logger_import) << DebugInfo << "Enabling L2 Cache" << Endl << EndDebugInfo;
		}
		
		if(old_l2cr_l2e && !GetL2CR_L2E())
		{
			if(IsVerboseSetL2CR())
				(*logger_import) << DebugInfo << "Disabling L2 Cache" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_L2CR_L2I)
	{
		if(GetL2CR_L2I())
		{
			if(old_l2cr_l2e)
			{
				if(IsVerboseSetL2CR())
					(*logger_import) << DebugWarning << "L2 Cache should be disabled prior a flash invalidation" << Endl << EndDebugWarning;
			}
			
			if(IsVerboseSetL2CR())
				(*logger_import) << DebugInfo << "Flash Invalidating L2 Cache" << Endl << EndDebugInfo;
			Decoder<CONFIG>::InvalidateDecodingCache();
			if(CONFIG::L2_CONFIG::ENABLE)
			{
				InvalidateL2();
			}
			if(CONFIG::HAS_L2CR_L2IP)
			{
				ResetL2CR_L2IP();
			}
			if(IsMPC7XXX())
			{
				ResetL2CR_L2I();
			}
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
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Enabling IMMU" << Endl << EndDebugInfo;
		}
	
		if(old_msr_ir && !GetMSR_IR())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Disabling IMMU" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_MSR_DR)
	{
		if(!old_msr_dr && GetMSR_DR())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Enabling DMMU" << Endl << EndDebugInfo;
		}
		
		if(old_msr_dr && !GetMSR_DR())	
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Disabling DMMU" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_MSR_FP)
	{
		if(!old_msr_fp && GetMSR_FP())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Enabling FPU" << Endl << EndDebugInfo;
		}
	
		if(old_msr_fp && !GetMSR_FP())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Disabling FPU" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_MSR_FE0 || CONFIG::HAS_MSR_FE1)
	{
		uint32_t msr_fe = (CONFIG::HAS_MSR_FE0 ? GetMSR_FE0() : 0) | (CONFIG::HAS_MSR_FE1 ? GetMSR_FE1() : 0);
		if(!old_msr_fe && msr_fe)
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Enabling FPU exception" << Endl << EndDebugInfo;
			fpu.EnableException();
		}
	
		if(old_msr_fe && !msr_fe)
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Disabling FPU exception" << Endl << EndDebugInfo;
			fpu.DisableException();
		}
	}

	if(CONFIG::HAS_MSR_PR)
	{
		if(!old_msr_pr && GetMSR_PR())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Switching to user privilege level" << Endl << EndDebugInfo;
		}
	
		if(old_msr_pr && !GetMSR_PR())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Switching to supervisor privilege level" << Endl << EndDebugInfo;
		}
	}

	if(CONFIG::HAS_MSR_LE)
	{
		if(!old_msr_le && GetMSR_LE())
		{
			if(IsVerboseSetMSR())
				(*logger_import) << DebugInfo << "Switching to little endian" << Endl << EndDebugInfo;
		}
	
		if(old_msr_le && !GetMSR_LE())
		{
			if(IsVerboseSetMSR())
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

	unisim::util::debug::Register *fpu_reg = fpu.GetRegister(name);

	return fpu_reg;
}

template <class CONFIG>
const char *CPU<CONFIG>::GetArchitectureName() const
{
	return "powerpc";
}

template <class CONFIG>
void CPU<CONFIG>::BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, WIMG wimg, bool rwitm)
{
	memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
void CPU<CONFIG>::BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, WIMG wimg)
{
	memory_import->WriteMemory(physical_addr, buffer, size);
}

template <class CONFIG>
void CPU<CONFIG>::BusZeroBlock(physical_address_t physical_addr)
{
	uint8_t zero[32];
	memset(zero, 0, sizeof(zero));
	memory_import->WriteMemory(physical_addr & (~31), zero, sizeof(zero));
}

template <class CONFIG>
void CPU<CONFIG>::BusFlushBlock(physical_address_t physical_addr)
{
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDL1Set(uint32_t index)
{
	uint32_t way;

	CacheSet<typename CONFIG::DL1_CONFIG>& l1_set = dl1[index];

	l1_set.status.plru_bits = 0;
	
	for(way = 0; way < CacheSet<typename CONFIG::DL1_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::DL1_CONFIG>& l1_line = l1_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::DL1_CONFIG>& l1_block = l1_line[sector];

			l1_block.status.valid = false;
			l1_block.status.dirty = false;
		}
		l1_line.status.valid = false;
		l1_line.SetBaseAddr(0);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDL1()
{
	if(IsVerboseDL1())
	{
		(*logger_import) << DebugInfo << "Invalidating DL1" << Endl << EndDebugInfo;
	}
	uint32_t index;
	
	for(index = 0; index < Cache<typename CONFIG::DL1_CONFIG>::NUM_SETS; index++)
	{
		InvalidateDL1Set(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateIL1Set(uint32_t index)
{
	uint32_t way;
	
	CacheSet<typename CONFIG::IL1_CONFIG>& l1_set = il1[index];

	l1_set.status.plru_bits = 0;
	
	for(way = 0; way < CacheSet<typename CONFIG::IL1_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::IL1_CONFIG>& l1_line = l1_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::IL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::IL1_CONFIG>& l1_block = l1_line[sector];

			l1_block.status.valid = false;
		}
		l1_line.status.valid = false;
		l1_line.SetBaseAddr(0);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateIL1()
{
	if(IsVerboseIL1())
	{
		(*logger_import) << DebugInfo << "Invalidating IL1" << Endl << EndDebugInfo;
	}
	uint32_t index;
	
	for(index = 0; index < Cache<typename CONFIG::IL1_CONFIG>::NUM_SETS; index++)
	{
		InvalidateIL1Set(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateL2Set(uint32_t index)
{
	uint32_t way;
	
	CacheSet<typename CONFIG::L2_CONFIG>& l2_set = l2[index];

	l2_set.status.plru_bits = 0;
	
	for(way = 0; way < CacheSet<typename CONFIG::L2_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::L2_CONFIG>& l2_line = l2_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::L2_CONFIG>& l2_block = l2_line[sector];

			l2_block.status.valid = false;
			l2_block.status.dirty = false;
		}
		l2_line.status.valid = false;
		l2_line.SetBaseAddr(0);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateL2()
{
	if(IsVerboseL2())
	{
		(*logger_import) << DebugInfo << "Invalidating L2" << Endl << EndDebugInfo;
	}
	uint32_t index;
	
	for(index = 0; index < Cache<typename CONFIG::L2_CONFIG>::NUM_SETS; index++)
	{
		InvalidateL2Set(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ClearAccessDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	l1_access.addr = 0;
	l1_access.line_base_addr = 0;
	l1_access.block_base_addr = 0;
	//uint32_t index;
	l1_access.way = 0;
	l1_access.sector = 0;
	l1_access.offset = 0;
	l1_access.size_to_block_boundary = 0;
	l1_access.set = 0;
	l1_access.line = 0;
	l1_access.line_to_evict = 0;
	l1_access.block = 0;
}

template <class CONFIG>
void CPU<CONFIG>::ClearAccessIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	l1_access.addr = 0;
	l1_access.line_base_addr = 0;
	l1_access.block_base_addr = 0;
	//uint32_t index;
	l1_access.way = 0;
	l1_access.sector = 0;
	l1_access.offset = 0;
	l1_access.size_to_block_boundary = 0;
	l1_access.set = 0;
	l1_access.line = 0;
	l1_access.line_to_evict = 0;
	l1_access.block = 0;
}

template <class CONFIG>
void CPU<CONFIG>::ClearAccessL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	l2_access.addr = 0;
	l2_access.line_base_addr = 0;
	l2_access.block_base_addr = 0;
	//uint32_t index;
	l2_access.way = 0;
	l2_access.sector = 0;
	l2_access.offset = 0;
	l2_access.size_to_block_boundary = 0;
	l2_access.set = 0;
	l2_access.line = 0;
	l2_access.line_to_evict = 0;
	l2_access.block = 0;
}

template <class CONFIG>
inline void CPU<CONFIG>::LookupDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	typename CONFIG::DL1_CONFIG::ADDRESS addr = l1_access.addr;
	typename CONFIG::DL1_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::DL1_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	uint32_t way;
	CacheSet<typename CONFIG::DL1_CONFIG> *set;
	CacheLine<typename CONFIG::DL1_CONFIG> *line;
	CacheBlock<typename CONFIG::DL1_CONFIG> *block;
	
	num_dl1_accesses++;
	// Decode the address
	dl1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(IsVerboseDL1())
	{
		(*logger_import) << DebugInfo << "DL1 Lookup at 0x";
		(*logger_import) << Hex << addr << Dec << " : ";
		(*logger_import) << "line_base_addr=0x" << Hex << line_base_addr << Dec << ",";
		(*logger_import) << "index=0x" << Hex << index << Dec << ",";
		(*logger_import) << "sector=0x" << Hex << sector << Dec << ",";
		(*logger_import) << "offset=0x" << Hex << offset << Dec << ",";
		(*logger_import) << "size_to_block_boundary=0x" << Hex << size_to_block_boundary << Dec;
		(*logger_import) << Endl << EndDebugInfo;
	}
	l1_access.line_base_addr = line_base_addr;
	l1_access.block_base_addr = block_base_addr;
	//l1_access.index = index;
	l1_access.sector = sector;
	l1_access.offset = offset;
	l1_access.size_to_block_boundary = size_to_block_boundary;
	l1_access.set = set = &dl1[index];

	// Associative search
	for(way = 0; way < CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY; way++)
	{
		line = &(*set)[way];
		if(line->status.valid && line->GetBaseAddr() == line_base_addr)
		{
			if(IsVerboseDL1())
			{
				(*logger_import) << DebugInfo << "DL1 Line hit: way=" << way << Endl << EndDebugInfo;
			}
			// line hit: block may need a fill if not yet present in the line
			l1_access.line = line;
			l1_access.line_to_evict = 0;
			l1_access.way = way;
			block = &(*line)[sector];
			l1_access.block = block->status.valid ? block : 0;
			if(IsVerboseDL1())
			{
				(*logger_import) << DebugInfo << "DL1 block " << (block->status.valid ? "hit" : "miss") << Endl << EndDebugInfo;
			}
			return;
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
	ChooseLineToEvictDL1(l1_access);
	if(IsVerboseDL1())
	{
		(*logger_import) << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << Endl << EndDebugInfo;
	}
	num_dl1_misses++;
}

template <class CONFIG>
inline void CPU<CONFIG>::LookupIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	typename CONFIG::IL1_CONFIG::ADDRESS addr = l1_access.addr;
	typename CONFIG::IL1_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::IL1_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	uint32_t way;
	CacheSet<typename CONFIG::IL1_CONFIG> *set;
	CacheLine<typename CONFIG::IL1_CONFIG> *line;
	CacheBlock<typename CONFIG::IL1_CONFIG> *block;
	
	num_il1_accesses++;
	// Decode the address
	il1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(IsVerboseIL1())
	{
		(*logger_import) << DebugInfo << "IL1 Lookup at 0x";
		(*logger_import) << Hex << addr << Dec << " : ";
		(*logger_import) << "line_base_addr=0x" << Hex << line_base_addr << Dec << ",";
		(*logger_import) << "index=0x" << Hex << index << Dec << ",";
		(*logger_import) << "sector=0x" << Hex << sector << Dec << ",";
		(*logger_import) << "offset=0x" << Hex << offset << Dec << ",";
		(*logger_import) << "size_to_block_boundary=0x" << Hex << size_to_block_boundary << Dec;
		(*logger_import) << Endl << EndDebugInfo;
	}
	l1_access.line_base_addr = line_base_addr;
	l1_access.block_base_addr = block_base_addr;
	//l1_access.index = index;
	l1_access.sector = sector;
	l1_access.offset = offset;
	l1_access.size_to_block_boundary = size_to_block_boundary;
	l1_access.set = set = &il1[index];

	// Associative search
	for(way = 0; way < CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY; way++)
	{
		line = &(*set)[way];
		if(line->status.valid && line->GetBaseAddr() == line_base_addr)
		{
			if(IsVerboseIL1())
			{
				(*logger_import) << DebugInfo << "IL1 Line hit: way=" << way << Endl << EndDebugInfo;
			}
			// line hit: block may need a fill if not yet present in the line
			l1_access.line = line;
			l1_access.line_to_evict = 0;
			l1_access.way = way;
			block = &(*line)[sector];
			l1_access.block = block->status.valid ? block : 0;
			if(IsVerboseIL1())
			{
				(*logger_import) << DebugInfo << "IL1 block " << (block->status.valid ? "hit" : "miss") << Endl << EndDebugInfo;
			}
			return;
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
	ChooseLineToEvictIL1(l1_access);
	if(IsVerboseIL1())
	{
		(*logger_import) << DebugInfo << "IL1 line miss: choosen way=" << l1_access.way << Endl << EndDebugInfo;
	}
	num_il1_misses++;
}

template <class CONFIG>
void CPU<CONFIG>::LookupL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	typename CONFIG::L2_CONFIG::ADDRESS addr = l2_access.addr;
	typename CONFIG::L2_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::L2_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	uint32_t way;
	CacheSet<typename CONFIG::L2_CONFIG> *set;
	CacheLine<typename CONFIG::L2_CONFIG> *line;
	CacheBlock<typename CONFIG::L2_CONFIG> *block;
	
	num_l2_accesses++;
	// Decode the address
	l2.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(IsVerboseL2())
	{
		(*logger_import) << DebugInfo << "L2 Lookup at 0x";
		(*logger_import) << Hex << addr << Dec << " : ";
		(*logger_import) << "line_base_addr=0x" << Hex << line_base_addr << Dec << ",";
		(*logger_import) << "index=0x" << Hex << index << Dec << ",";
		(*logger_import) << "sector=0x" << Hex << sector << Dec << ",";
		(*logger_import) << "offset=0x" << Hex << offset << Dec << ",";
		(*logger_import) << "size_to_block_boundary=0x" << Hex << size_to_block_boundary << Dec;
		(*logger_import) << Endl << EndDebugInfo;
	}
	l2_access.line_base_addr = line_base_addr;
	l2_access.block_base_addr = block_base_addr;
	//l2_access.index = index;
	l2_access.sector = sector;
	l2_access.offset = offset;
	l2_access.size_to_block_boundary = size_to_block_boundary;
	l2_access.set = set = &l2[index];

	// Associative search
	for(way = 0; way < CONFIG::L2_CONFIG::CACHE_ASSOCIATIVITY; way++)
	{
		line = &(*set)[way];
		if(line->status.valid && line->GetBaseAddr() == line_base_addr)
		{
			if(IsVerboseL2())
			{
				(*logger_import) << DebugInfo << "L2 Line hit: way=" << way << Endl << EndDebugInfo;
			}
			// line hit: block may need a fill if not yet present in the line
			l2_access.line = line;
			l2_access.line_to_evict = 0;
			l2_access.way = way;
			block = &(*line)[sector];
			l2_access.block = block->status.valid ? block : 0;
			if(IsVerboseL2())
			{
				(*logger_import) << DebugInfo << "L2 block " << (block->status.valid ? "hit" : "miss") << Endl << EndDebugInfo;
			}
			return;
		}
	}

	// line miss
	l2_access.line = 0;
	l2_access.block = 0;
	ChooseLineToEvictL2(l2_access);
	if(IsVerboseL2())
	{
		(*logger_import) << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << Endl << EndDebugInfo;
	}
	num_l2_misses++;
}

template <class CONFIG>
inline void CPU<CONFIG>::EvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	if(IsVerboseDL1())
	{
		(*logger_import) << DebugInfo << "DL1: evicting line at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
	}
	if(l1_access.line_to_evict->status.valid)
	{
		uint32_t sector;
	
		for(sector = 0; sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_evict = (*l1_access.line_to_evict)[sector];
	
			if(l1_block_to_evict.status.valid && l1_block_to_evict.status.dirty)
			{
				// dirty DL1 block eviction
				if(IsL2CacheEnabled())
				{
					if(IsVerboseDL1())
					{
						(*logger_import) << DebugInfo << "DL1: evicting dirty block at 0x" << Hex << l1_block_to_evict.GetBaseAddr() << Dec << " into L2" << Endl << EndDebugInfo;
					}
					// DL1 block eviction into L2
					CacheAccess<class CONFIG::L2_CONFIG> l2_access;

					l2_access.addr = l1_block_to_evict.GetBaseAddr();
					LookupL2(l2_access);

					if(!l2_access.line)
					{
						EvictL2(l2_access);
					}

					if(!l2_access.block)
					{
						FillL2(l2_access, CONFIG::WIMG_DEFAULT, true); // rwitm
					}

					memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_evict[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
					l2_access.block->status.valid = true;
					l2_access.block->status.dirty = true;

					UpdateReplacementPolicyL2(l2_access);
				}
				else
				{
					if(IsVerboseDL1())
					{
						(*logger_import) << DebugInfo << "DL1: evicting dirty block at 0x" << Hex << l1_block_to_evict.GetBaseAddr() << Dec << " into memory" << Endl << EndDebugInfo;
					}
					// dirty DL1 block eviction into memory
					// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l1_block_to_evict.GetBaseAddr(), &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				}
			}
			l1_block_to_evict.status.valid = false;
			l1_block_to_evict.status.dirty = false;
		}
	
		l1_access.line_to_evict->status.valid = false;
	}
	l1_access.line = l1_access.line_to_evict;
	l1_access.line_to_evict = 0;
}

template <class CONFIG>
inline void CPU<CONFIG>::EvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	if(IsVerboseIL1())
	{
		(*logger_import) << DebugInfo << "IL1: evicting line at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
	}
	if(l1_access.line_to_evict->status.valid)
	{
		uint32_t sector;
	
		for(sector = 0; sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<class CONFIG::IL1_CONFIG>& l1_block_to_evict = (*l1_access.line_to_evict)[sector];
	
			l1_block_to_evict.status.valid = false;
		}
	
		l1_access.line_to_evict->status.valid = false;
	}
	l1_access.line = l1_access.line_to_evict;
	l1_access.line_to_evict = 0;
}

template <class CONFIG>
void CPU<CONFIG>::EvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	if(IsVerboseL2())
	{
		(*logger_import) << DebugInfo << "L2: evicting line at 0x" << Hex << l2_access.addr << Dec << Endl << EndDebugInfo;
	}
	if(l2_access.line_to_evict->status.valid)
	{
		uint32_t l2_sector;
	
		for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
		{
			CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_evict = (*l2_access.line_to_evict)[l2_sector];
	
			if(l2_block_to_evict.status.valid && l2_block_to_evict.status.dirty)
			{
				// dirty L2 block eviction into memory
				// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
				if(IsVerboseL2())
				{
					(*logger_import) << DebugInfo << "L2: evicting dirty block at 0x" << Hex << l2_block_to_evict.GetBaseAddr() << Dec << " into memory" << Endl << EndDebugInfo;
				}
				BusWrite(l2_block_to_evict.GetBaseAddr(), &l2_block_to_evict[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
			}
			l2_block_to_evict.status.valid = false;
			l2_block_to_evict.status.dirty = false;
		}
	
		l2_access.line_to_evict->status.valid = false;
	}
	l2_access.line = l2_access.line_to_evict;
	l2_access.line_to_evict = 0;
}

template <class CONFIG>
inline void CPU<CONFIG>::ChooseLineToEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	uint32_t way;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	if(IsVerboseDL1())
	{
		(*logger_import) << DebugInfo << "DL1 PLRU bits:";
		for(i = 0; i < CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			(*logger_import) << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		(*logger_import) << Endl << EndDebugInfo;
	}
	for(i = 0, way = 0, n = 0; n < CONFIG::DL1_CONFIG::CACHE_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}
//  	cerr << "way=" << way << endl;

	l1_access.way = way;
	l1_access.line_to_evict = &(*l1_access.set)[way];
}

template <class CONFIG>
inline void CPU<CONFIG>::ChooseLineToEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	uint32_t way;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	if(IsVerboseIL1())
	{
		(*logger_import) << DebugInfo << "IL1 PLRU bits:";
		for(i = 0; i < CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			(*logger_import) << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		(*logger_import) << Endl << EndDebugInfo;
	}
	for(i = 0, way = 0, n = 0; n < CONFIG::IL1_CONFIG::CACHE_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}
//  	cerr << "way=" << way << endl;

	l1_access.way = way;
	l1_access.line_to_evict = &(*l1_access.set)[way];
}

template <class CONFIG>
void CPU<CONFIG>::ChooseLineToEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	uint32_t way;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = l2_access.set->status.plru_bits;

	if(IsVerboseL2())
	{
		(*logger_import) << DebugInfo << "L2 PLRU bits:";
		for(i = 0; i < CONFIG::L2_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			(*logger_import) << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		(*logger_import) << Endl << EndDebugInfo;
	}
	for(i = 0, way = 0, n = 0; n < CONFIG::L2_CONFIG::CACHE_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}
//  	cerr << "way=" << way << endl;

	l2_access.way = way;
	l2_access.line_to_evict = &(*l2_access.set)[way];
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateReplacementPolicyDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = l1_access.way;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	for(n = CONFIG::DL1_CONFIG::CACHE_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	l1_access.set->status.plru_bits = plru_bits;
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateReplacementPolicyIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = l1_access.way;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	for(n = CONFIG::IL1_CONFIG::CACHE_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	l1_access.set->status.plru_bits = plru_bits;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateReplacementPolicyL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = l2_access.way;
	uint32_t plru_bits = l2_access.set->status.plru_bits;

	for(n = CONFIG::L2_CONFIG::CACHE_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	l2_access.set->status.plru_bits = plru_bits;
}

template <class CONFIG>
inline void CPU<CONFIG>::FillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access, WIMG wimg, bool rwitm)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(IsVerboseDL1())
	{
		(*logger_import) << DebugInfo << "DL1: filling block at 0x" << Hex << l1_access.block_base_addr << Dec << Endl << EndDebugInfo;
	}
	// DL1 block fill
	if(IsL2CacheEnabled())
	{
		// DL1 block fill from L2
		CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
		l2_access.addr = l1_access.block_base_addr;
	
		LookupL2(l2_access);
	
		if(!l2_access.line)
		{
			if(IsVerboseL2())
			{
				(*logger_import) << DebugInfo << "L2: line miss at 0x" << Hex << l2_access.addr << Dec << Endl << EndDebugInfo;
			}
			EvictL2(l2_access);
		}
	
		if(!l2_access.block)
		{
			if(IsVerboseL2())
			{
				(*logger_import) << DebugInfo << "L2: block miss at 0x" << Hex << l2_access.addr << Dec << Endl << EndDebugInfo;
			}
			FillL2(l2_access, wimg, rwitm);
		}

		memcpy(&(*l1_access.block)[0], &(*l2_access.block)[l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
		UpdateReplacementPolicyL2(l2_access);
	}
	else
	{
		// DL1 block fill from memory
		BusRead(l1_access.block_base_addr, &(*l1_access.block)[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE, wimg, rwitm);
	}

	l1_access.line->status.valid = true;
	l1_access.line->SetBaseAddr(l1_access.line_base_addr);
	l1_access.block->status.valid = true;
	l1_access.block->status.dirty = false;
}

template <class CONFIG>
inline void CPU<CONFIG>::FillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, WIMG wimg)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(IsVerboseIL1())
	{
		(*logger_import) << DebugInfo << "IL1: filling block at 0x" << Hex << l1_access.block_base_addr << Dec << Endl << EndDebugInfo;
	}

	// DL1 block fill
	if(IsL2CacheEnabled())
	{
		// DL1 block fill from L2
		CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
		l2_access.addr = l1_access.block_base_addr;
	
		LookupL2(l2_access);
	
		if(!l2_access.line)
		{
			if(IsVerboseL2())
			{
				(*logger_import) << DebugInfo << "L2: line miss at 0x" << Hex << l2_access.addr << Dec << Endl << EndDebugInfo;
			}
			EvictL2(l2_access);
		}
	
		if(!l2_access.block)
		{
			if(IsVerboseL2())
			{
				(*logger_import) << DebugInfo << "L2: block miss at 0x" << Hex << l2_access.addr << Dec << Endl << EndDebugInfo;
			}
			FillL2(l2_access, wimg, false);
		}
		memcpy(&(*l1_access.block)[0], &(*l2_access.block)[l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
		UpdateReplacementPolicyL2(l2_access);
	}
	else
	{
		// DL1 block fill from memory
		BusRead(l1_access.block_base_addr, &(*l1_access.block)[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE, wimg, false);
	}

	l1_access.line->status.valid = true;
	l1_access.line->SetBaseAddr(l1_access.line_base_addr);
	l1_access.block->status.valid = true;
}

template <class CONFIG>
void CPU<CONFIG>::FillL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access, WIMG wimg, bool rwitm)
{
	l2_access.block = &(*l2_access.line)[l2_access.sector];
	if(IsVerboseL2())
	{
		(*logger_import) << DebugInfo << "L2: filling block at 0x" << Hex << l2_access.block_base_addr << Dec << Endl << EndDebugInfo;
	}
	BusRead(l2_access.block_base_addr, &(*l2_access.block)[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE, wimg, rwitm);
	l2_access.line->status.valid = true;
	l2_access.line->SetBaseAddr(l2_access.line_base_addr);
	l2_access.block->status.valid = true;
	l2_access.block->status.dirty = false;
}

template <class CONFIG>
void CPU<CONFIG>::Fetch(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	WIMG wimg;
	physical_address_t physical_addr;

	if(GetMSR_IR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_READ;
		mmu_access.memory_type = MT_INSN;
	
		TranslateAddress<false>(mmu_access);
	
		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	// Is IL1 enabled ?
	if(IsInsnCacheEnabled())
	{
		// IL1 Access
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupIL1(l1_access);

		if(!l1_access.line)
		{
			// IL1 line miss
			if(IsVerboseIL1())
			{
				(*logger_import) << DebugInfo << "IL1: line miss at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
			}
			EvictIL1(l1_access);
		}

		if(!l1_access.block)
		{
			// IL1 block miss
			if(IsVerboseIL1())
			{
				(*logger_import) << DebugInfo << "IL1: block miss at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
			}
			FillIL1(l1_access, wimg);
		}
	
		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);
		UpdateReplacementPolicyIL1(l1_access);
	}
	else
	{
		// DL1 disabled
		BusRead(physical_addr, buffer, size, wimg);
	}
}

template <class CONFIG>
template <bool TRANSLATE_ADDR>
void CPU<CONFIG>::Load(address_t addr, void *buffer, uint32_t size)
{
	WIMG wimg;
	physical_address_t physical_addr;

	memset(buffer, 0, size);
	
	if(TRANSLATE_ADDR && GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_READ;
		mmu_access.memory_type = MT_DATA;

		TranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}
	
	if(!(wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled())
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);

		if(!l1_access.line)
		{
			if(IsVerboseDL1())
			{
				(*logger_import) << DebugInfo << "DL1: line miss at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
			}
			EvictDL1(l1_access);
		}

		if(!l1_access.block)
		{
			if(IsVerboseDL1())
			{
				(*logger_import) << DebugInfo << "DL1: block miss at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
			}
			FillDL1(l1_access, wimg, false /* not a rwitm */);
		}

		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);
		UpdateReplacementPolicyDL1(l1_access);
	}
	else
	{
		// DL1 disabled
		BusRead(physical_addr, buffer, size, wimg);
	}
	if(IsVerboseLoad())
	{
		uint32_t i;
		(*logger_import) << DebugInfo << "instruction #" << instruction_counter << ":Loading ";
		for(i = 0; i < size; i++)
		{
			uint8_t value = ((uint8_t *) buffer)[i];
			uint8_t l = value & 15;
			uint8_t h = value >> 4;
			(*logger_import) << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10) << " ";
		}
		(*logger_import) << "(" << size << " bytes) at 0x" << Hex << addr << Dec << Endl << EndDebugInfo;
	}
}

template <class CONFIG>
template <bool TRANSLATE_ADDR>
void CPU<CONFIG>::Store(address_t addr, const void *buffer, uint32_t size)
{
	if(IsVerboseStore())
	{
		uint32_t i;
		(*logger_import) << DebugInfo << "insn #" << instruction_counter << ":Storing ";
		for(i = 0; i < size; i++)
		{
			uint8_t value = ((uint8_t *) buffer)[i];
			uint8_t l = value & 15;
			uint8_t h = value >> 4;
			(*logger_import) << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10) << " ";
		}
		(*logger_import) << "(" << size << " bytes) at 0x" << Hex << addr << Dec << Endl << EndDebugInfo;
	}
	WIMG wimg;
	physical_address_t physical_addr;
	
	if(TRANSLATE_ADDR && GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_WRITE;
		mmu_access.memory_type = MT_DATA;

		TranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	if(!(wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled())
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);

		if(!l1_access.line)
		{
			if(IsVerboseDL1())
			{
				(*logger_import) << DebugInfo << "DL1: line miss at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
			}
			EvictDL1(l1_access);
		}
		
		if(!l1_access.block)
		{
			if(IsVerboseDL1())
			{
				(*logger_import) << DebugInfo << "DL1: block miss at 0x" << Hex << l1_access.addr << Dec << Endl << EndDebugInfo;
			}
			FillDL1(l1_access, wimg, true);
		}
	
		// DL1 hit
		memcpy(&(*l1_access.block)[l1_access.offset], buffer, size);
		l1_access.block->status.dirty = true;
		UpdateReplacementPolicyDL1(l1_access);
	
		if(wimg & CONFIG::WIMG_WRITE_THROUGH)
		{
			if(IsL2CacheEnabled())
			{
				// DL1 block fill from L2
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = physical_addr;

				LookupL2(l2_access);

				memcpy(&(*l2_access.block)[l2_access.offset], buffer, size);
				l2_access.block->status.dirty = true;
				UpdateReplacementPolicyL2(l2_access);
			}
			BusWrite(physical_addr, buffer, size, wimg);
		}
	}
	else
	{
		// DL1 disabled
		BusWrite(physical_addr, buffer, size, wimg);
	}
}

template <class CONFIG>
template <class T>
inline void CPU<CONFIG>::Load(T& value, address_t ea)
{
	// Data Address Breakpoint handling
	if(CONFIG::DABR_ENABLE && CONFIG::HAS_DABR)
	{
		if(GetDABR_DR() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT())
		{
			throw DSIDataAddressBreakpointException<CONFIG>(ea, MAT_READ);
		}
	}

	// Address munging
	address_t munged_ea = GetMSR_LE() ? ea ^ (8 - sizeof(T)) : ea;

	uint32_t size_to_fsb_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(size_to_fsb_boundary >= sizeof(T))
	{
		// Memory load does not cross a FSB boundary
		Load<true>(munged_ea, &value, sizeof(T));
	}
	else
	{
		// Memory load crosses a FSB boundary
		Load<true>(munged_ea, &value, size_to_fsb_boundary);
		Load<true>(munged_ea + size_to_fsb_boundary, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary);
	}
}

template <class CONFIG>
template <class T>
inline void CPU<CONFIG>::Store(T value, address_t ea)
{
	// Data Address	Breakpoint handling
	if(CONFIG::DABR_ENABLE && CONFIG::HAS_DABR)
	{
		if(GetDABR_DW() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT())
		{
			throw DSIDataAddressBreakpointException<CONFIG>(ea, MAT_WRITE);
		}
	}

	// Address munging
	address_t munged_ea = GetMSR_LE() ? ea ^ (8 - sizeof(T)) : ea;

	uint32_t size_to_fsb_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(size_to_fsb_boundary >= sizeof(T))
	{
		// Memory store does not cross a FSB boundary
		Store<true>(munged_ea, &value, sizeof(T));
	}
	else
	{
		// Memory store crosses a FSB boundary
		Store<true>(munged_ea, &value, size_to_fsb_boundary);
		Store<true>(munged_ea + size_to_fsb_boundary, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorLoad(address_t ea, uint32_t size)
{
	// Memory access reporting
	if(requires_memory_access_reporting && memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, MemoryAccessReporting<address_t>::MT_DATA, ea, size);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorStore(address_t ea, uint32_t size)
{
	// Memory access reporting
	if(requires_memory_access_reporting && memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_WRITE, MemoryAccessReporting<address_t>::MT_DATA, ea, size);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Int8Load(unsigned int rd, address_t ea)
{
	uint8_t value;
	Load<uint8_t>(value, ea);
	gpr[rd] = (uint32_t) value; // 8-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int16Load(unsigned int rd, address_t ea)
{
	uint16_t value;
	Load<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) BigEndian2Host(value); // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::SInt16Load(unsigned int rd, address_t ea)
{
	uint16_t value;
	Load<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) (int16_t) BigEndian2Host(value); // 16-bit to 32-bit sign extension
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int32Load(unsigned int rd, address_t ea)
{
	uint32_t value;
	Load<uint32_t>(value, ea);
	gpr[rd] = BigEndian2Host(value);
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Fp32Load(unsigned int fd, address_t ea)
{
	uint32_t value;
	Load<uint32_t>(value, ea);
	fpu.SetFp32(fd, BigEndian2Host(value));
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Fp64Load(unsigned int fd, address_t ea)
{
	uint64_t value;
	Load<uint64_t>(value, ea);
	fpu.SetFp64(fd, BigEndian2Host(value));
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int16LoadByteReverse(unsigned int rd, address_t ea)
{
	uint16_t value;
	Load<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) LittleEndian2Host(value); // reverse bytes and 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int32LoadByteReverse(unsigned int rd, address_t ea)
{
	uint32_t value;
	Load<uint32_t>(value, ea);
	gpr[rd] = LittleEndian2Host(value); // reverse bytes
	MonitorLoad(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::IntLoadMSBFirst(unsigned int rd, address_t ea, uint32_t size)
{
	switch(size)
	{
		case 1:
		{
			uint8_t value;
			Load<uint8_t>(value, ea);
			gpr[rd] = (uint32_t) value << 24;
			break;
		}

		case 2:
		{
			uint16_t value;
			Load<uint16_t>(value, ea);
			gpr[rd] = (uint32_t) BigEndian2Host(value) << 16;
			break;
		}

		case 3:
		{
			uint8_t buffer[3];
			Load<typeof(buffer)>(buffer, ea);
			uint32_t value = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8);
			gpr[rd] = value;
			break;
		}

		case 4:
		{
			uint32_t value;
			Load<uint32_t>(value, ea);
			gpr[rd] = BigEndian2Host(value);
			break;
		}
	}
	MonitorLoad(ea, size);
}

template <class CONFIG>
void CPU<CONFIG>::Int8Store(unsigned int rs, address_t ea)
{
	uint8_t value = gpr[rs];
	Store<uint8_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int16Store(unsigned int rs, address_t ea)
{
	uint16_t value = Host2BigEndian((uint16_t) gpr[rs]);
	Store<uint16_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int32Store(unsigned int rs, address_t ea)
{
	uint32_t value = Host2BigEndian(gpr[rs]);
	Store<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Fp32Store(unsigned int fs, address_t ea)
{
	uint32_t value = Host2BigEndian(fpu.GetFp32(fs));
	Store<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Fp64Store(unsigned int fs, address_t ea)
{
	uint64_t value = Host2BigEndian(fpu.GetFp64(fs));
	Store<uint64_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::FpStoreLSW(unsigned int fs, address_t ea)
{
	uint32_t value = Host2BigEndian((uint32_t) fpu.GetFp64(fs));
	Store<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int16StoreByteReverse(unsigned int rs, address_t ea)
{
	uint16_t value = Host2LittleEndian((uint16_t) gpr[rs]);
	Store<uint16_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::Int32StoreByteReverse(unsigned int rs, address_t ea)
{
	uint32_t value = Host2LittleEndian(gpr[rs]);
	Store<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
}

template <class CONFIG>
void CPU<CONFIG>::IntStoreMSBFirst(unsigned int rs, address_t ea, uint32_t size)
{
	switch(size)
	{
		case 1:
			{
				uint8_t value = gpr[rs] >> 24;
				Store<uint8_t>(value, ea);
				break;
			}

		case 2:
			{
				uint16_t value = Host2BigEndian((uint16_t)(gpr[rs] >> 16));
				Store<uint16_t>(value, ea);
				break;
			}

		case 3:
			{
				uint32_t value = gpr[rs];
				uint8_t buffer[3];
				buffer[0] = value >> 24;
				buffer[1] = value >> 16;
				buffer[2] = value >> 8;
				Store<typeof(buffer)>(buffer, ea);
				break;
			}

		case 4:
			{
				uint32_t value = Host2BigEndian(gpr[rs]);
				Store<uint32_t>(value, ea);
				break;
			}
	}

	MonitorStore(ea, size);
}

// template <class CONFIG>
// void CPU<CONFIG>::LoadRegister(unsigned int i, address_t ea, uint32_t size, LoadStoreMode lsm)
// {
// 	// Address munging
// 	address_t munged_ea = GetMSR_LE() ? ea ^ (8 - size) : ea;
// 
// 	uint8_t buffer[size];
// 	uint32_t size_to_fsb_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));
// 
// 	// Ensure that memory access does not cross a FSB boundary
// 	if(size_to_fsb_boundary >= size)
// 	{
// 		// Memory load does not cross a FSB boundary
// 		Load<true>(munged_ea, buffer, size);
// 	}
// 	else
// 	{
// 		// Memory load crosses a FSB boundary
// 		Load<true>(munged_ea, buffer, size_to_fsb_boundary);
// 		Load<true>(munged_ea + size_to_fsb_boundary, buffer + size_to_fsb_boundary, size - size_to_fsb_boundary);
// 	}
// 
// 	// Reverse bytes, sign extend, and write back value into general purpose register
// 	if(lsm & LSM_FLOATING_POINT)
// 	{
// 		switch(size)
// 		{
// 			case 4:
// 			{
// 				uint32_t value = BigEndian2Host(*(uint32_t *) buffer);
// 				fpu.SetFp32(i, value);
// 				break;
// 			}
// 	
// 			case 8:
// 			{
// 				uint64_t value = BigEndian2Host(*(uint64_t *) buffer);
// 				fpu.SetFp64(i, value);
// 				break;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		switch(size)
// 		{
// 			case 1:
// 			{
// 				uint8_t read_value = *buffer;
// 				uint32_t value = (lsm & LSM_MSB_FIRST) ? (uint32_t) read_value << 24 : (uint32_t) ((lsm & LSM_SIGN_EXTEND) ? (uint32_t) (int8_t) read_value : read_value);
// 				gpr[i] = value;
// 				break;
// 			}
// 	
// 			case 2:
// 			{
// 				uint16_t read_value = (lsm & LSM_BYTE_REVERSE) ? LittleEndian2Host(*(uint16_t *) buffer) : BigEndian2Host(*(uint16_t *) buffer);
// 				uint32_t value = (lsm & LSM_MSB_FIRST) ? (uint32_t) read_value << 16 : (uint32_t) ((lsm & LSM_SIGN_EXTEND) ? (uint32_t) (int16_t) read_value : read_value);
// 				gpr[i] = value;
// 				break;
// 			}
// 	
// 			case 3:
// 			{
// 				uint32_t read_value = (lsm & LSM_BYTE_REVERSE) ?  ((uint32_t) buffer[0] << 16) | ((uint32_t) buffer[1] << 8) | ((uint32_t) buffer[2]) :
// 																((uint32_t) buffer[0]) | ((uint32_t) buffer[1] << 8) | ((uint32_t) buffer[2] << 16);
// 				uint32_t value = (lsm & LSM_MSB_FIRST) ? (uint32_t) read_value << 8 : (uint32_t) ((lsm & LSM_SIGN_EXTEND) ? ((int32_t) read_value << 8) >> 8: read_value);
// 				gpr[i] = value;
// 				break;
// 			}
// 	
// 			case 4:
// 			{
// 				uint32_t read_value = (lsm & LSM_BYTE_REVERSE) ? LittleEndian2Host(*(uint32_t *) buffer) : BigEndian2Host(*(uint32_t *) buffer);
// 				uint32_t value = read_value;
// 				gpr[i] = value;
// 				break;
// 			}
// 		}
// 	}
// 
// 	// Memory access reporting
// 	if(requires_memory_access_reporting)
// 	{
// 		if(memory_access_reporting_import)
// 		{
// 			memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ, MemoryAccessReporting<address_t>::MT_DATA, ea, size);
// 		}
// 	}
// 	
// 	// Data Address Breakpoint handling
// 	if(CONFIG::DABR_ENABLE && CONFIG::HAS_DABR)
// 	{
// 		if(GetDABR_DR() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT())
// 		{
// 			throw DSIDataAddressBreakpointException<CONFIG>(ea, MAT_READ);
// 		}
// 	}
// }
// 
// template <class CONFIG>
// void CPU<CONFIG>::StoreRegister(unsigned int i, address_t ea, uint32_t size, LoadStoreMode lsm)
// {
// 	// Address munging
// 	address_t munged_ea = GetMSR_LE() ? ea ^ (8 - size) : ea;
// 
// 	// Read value from general purpose register
// 	uint8_t buffer[size];
// 
// 	if(lsm & LSM_FLOATING_POINT)
// 	{
// 		switch(size)
// 		{
// 			case 4:
// 				if(lsm & LSM_RAW_FLOAT)
// 					*(uint32_t *) buffer = (uint32_t) Host2BigEndian((uint32_t) fpu.GetFp64(i));
// 				else
// 					*(uint32_t *) buffer = (uint32_t) Host2BigEndian(fpu.GetFp32(i));
// 				break;
// 			case 8:
// 				*(uint64_t *) buffer = (uint64_t) Host2BigEndian(fpu.GetFp64(i));
// 				break;
// 		}
// 	}
// 	else
// 	{
// 		switch(size)
// 		{
// 			case 1:
// 				{
// 					uint8_t value = (lsm & LSM_MSB_FIRST) ? gpr[i] >> 24 : gpr[i];
// 					*buffer = value;
// 					break;
// 				}
// 	
// 			case 2:
// 				{
// 					uint16_t value = (lsm & LSM_MSB_FIRST) ? gpr[i] >> 16 : gpr[i];
// 					*(uint16_t *) buffer = (lsm & LSM_BYTE_REVERSE) ? Host2LittleEndian(value) : Host2BigEndian(value);
// 					break;
// 				}
// 	
// 			case 3:
// 				{
// 					uint32_t value = (lsm & LSM_MSB_FIRST) ? gpr[i] >> 8 : gpr[i];
// 					buffer[0] = (lsm & LSM_BYTE_REVERSE) ? value : value >> 16;
// 					buffer[1] = value >> 8;
// 					buffer[2] = (lsm & LSM_BYTE_REVERSE) ? value >> 16 : value;
// 					break;
// 				}
// 	
// 			case 4:
// 				{
// 					uint32_t value = gpr[i];
// 					*(uint32_t *) buffer = (lsm & LSM_BYTE_REVERSE) ? Host2LittleEndian(value) : Host2BigEndian(value);
// 					break;
// 				}
// 		}
// 	}
// 
// 	uint32_t size_to_fsb_boundary = FSB_WIDTH - (munged_ea & (FSB_WIDTH - 1));
// 
// 	// Ensure that memory access does not cross a FSB boundary
// 	if(size_to_fsb_boundary >= size)
// 	{
// 		// Memory store does not cross a FSB boundary
// 		Store<true>(munged_ea, buffer, size);
// 	}
// 	else
// 	{
// 		// Memory store crosses a FSB boundary
// 		Store<true>(munged_ea, buffer, size_to_fsb_boundary);
// 		Store<true>(munged_ea + size_to_fsb_boundary, buffer + size_to_fsb_boundary, size - size_to_fsb_boundary);
// 	}
// 
// 	// Memory access reporting
// 	if(requires_memory_access_reporting) 
// 	{
// 		if(memory_access_reporting_import)
// 		{
// 			memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_WRITE, MemoryAccessReporting<address_t>::MT_DATA, ea, size);
// 		}
// 	}
// 
// 	// Data Address	Breakpoint handling
// 	if(CONFIG::DABR_ENABLE && CONFIG::HAS_DABR)
// 	{
// 		if(GetDABR_DW() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT())
// 		{
// 			throw DSIDataAddressBreakpointException<CONFIG>(ea, MAT_WRITE);
// 		}
// 	}
// }

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::LookupBAT(MMUAccess<CONFIG>& mmu_access)
{
	address_t addr = mmu_access.addr;
	PrivilegeLevel privilege_level = mmu_access.privilege_level;
	MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	MemoryType memory_type = mmu_access.memory_type;
	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field
	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	virtual_address_t virtual_addr;			// Virtual address
	virtual_address_t base_virtual_addr;	// Base virtual address
	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key;
	WIMG wimg;
	
	//------------------------------
	//-  Block Address Translation -
	//------------------------------

	// Compute BEPI from the effective address
	mmu_access.bepi = bepi = (addr >> 17) & 0x7fff;

	if(memory_type == MT_INSN)
	{
		// Memory access is an instruction fetch
		uint32_t num_ibats = (CONFIG::HAS_HID2 && GetHID2_SWT_EN()) ? CONFIG::NUM_BATS : 4;

		// Scan each IBAT register to find a translation
		for(bat_num = 0; bat_num < num_ibats; bat_num++)
		{
			// Check if IBAT register match
			if((bepi & 0x7800) == (GetIBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
				// the 15 most significative bits in EA and 15-bit IBAT BEPI field
						 && (bepi & ~GetIBATU_BL(bat_num) & 0x7ff) == (GetIBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
				// bits of the 15 most significative bits in EA and IBAT BEPI field according to the IBAT block size mask (BL)
			{
				//cerr << "privilege level = " << ((privilege_level == PR_SUPERVISOR) ? "supervisor" : "user") << endl;
				//cerr << "IBATU_VP = " << GetIBATU_VP(bat_num) << endl;
				//cerr << "IBATU_VS = " << GetIBATU_VS(bat_num) << endl;
				if((GetIBATU_VP(bat_num) && (privilege_level & PR_USER)) || (GetIBATU_VS(bat_num) && (privilege_level & PR_SUPERVISOR)))
				{
					//cerr << "IBAT match for 0x" << hex << addr << endl;
					// A BAT match occurs
					mmu_access.protection_boundary = ((GetIBATU_BEPI(bat_num) & 0x7ffUL) + GetIBATU_BL(bat_num) + 1) << 17;
					mmu_access.wimg = wimg = (WIMG)(GetIBATL_WIMG(bat_num) & 0x6); // W and G are not defined in IBATs;

					// Check if access is guarded
					if(wimg & CONFIG::WIMG_GUARDED_MEMORY)
					{
						if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
					}

					// Check access rights
					uint32_t pp = GetIBATL_PP(bat_num);
					if(!pp) // PP=00: No access
					{
						// Raise a protection violation
						if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
					}
					// Access is allowed
				
					// Compute the physical address
					mmu_access.physical_addr = ((((bepi & 0x7ff) & GetIBATU_BL(bat_num)) | (GetIBATL_BRPN(bat_num) & 0x7ff) | (GetIBATL_BRPN(bat_num) & 0x7800)) << 17) | (addr & 0x1ffff);
					mmu_access.bat_hit = true;
//					cerr << "IBAT hit at 0x" << std::hex << addr << std::hex << endl;
					return;
				}
			}
		}
		//cerr << "IBAT miss at 0x" << std::hex << addr << std::hex << endl;
	}
	else
	{
		// Memory access is a data memory access
		uint32_t num_dbats = (CONFIG::HAS_HID2 && GetHID2_SWT_EN()) ? CONFIG::NUM_BATS : 4;
		// Scan each DBAT register to find a translation
		for(bat_num = 0; bat_num < num_dbats; bat_num++)
		{
			// Check if DBAT register match
			if((bepi & 0x7800) == (GetDBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
				// the 15 most significative bits in EA and 15-bit DBAT BEPI field
						 && (bepi & ~GetDBATU_BL(bat_num) & 0x7ff) == (GetDBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
				// bits of the 15 most significative bits in EA and DBAT BEPI field according to the DBAT block size mask (BL)
			{
				if((GetDBATU_VP(bat_num) && (privilege_level & PR_USER)) || (GetDBATU_VS(bat_num) && (privilege_level & PR_SUPERVISOR)))
				{
					// A DBAT match occurs
					mmu_access.protection_boundary = ((GetDBATU_BEPI(bat_num) & 0x7ffUL) + GetDBATU_BL(bat_num)) << 17;
					mmu_access.wimg = wimg = (WIMG) GetDBATL_WIMG(bat_num);
	
					// Check access rights
					uint32_t pp = GetDBATL_PP(bat_num);
					if(!pp || // PP=00: No access
					  ((pp & 1) && (memory_access_type == MAT_WRITE))) // PP=x1: Read-only
					{
						if(!DEBUG)
						{
							throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
						}
					}
					// Access is allowed
					
					// Compute the physical address
					mmu_access.physical_addr = ((((bepi & 0x7ff) & GetDBATU_BL(bat_num)) | (GetDBATL_BRPN(bat_num) & 0x7ff) | (GetDBATL_BRPN(bat_num) & 0x7800)) << 17) | (addr & 0x1ffff);
					mmu_access.bat_hit = true;
//					cerr << "DBAT hit at 0x" << std::hex << addr << std::hex << endl;
					return;
				}
			}
		}
		//cerr << "DBAT miss at 0x" << std::hex << addr << std::hex << endl;
	}
	// Miss in BAT registers
	mmu_access.bat_hit = false;
}



template <class CONFIG>
void CPU<CONFIG>::InvalidateITLBSet(uint32_t index)
{
	uint32_t way;

	TLBSet<typename CONFIG::ITLB_CONFIG>& itlb_set = itlb[index];

	itlb_set.status.plru_bits = 0;
	
	for(way = 0; way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::ITLB_CONFIG>& itlb_entry = itlb_set[way];

		itlb_entry.status.valid = false;
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateITLB()
{
	uint32_t index;
	
	for(index = 0; index < TLB<typename CONFIG::ITLB_CONFIG>::NUM_SETS; index++)
	{
		InvalidateITLBSet(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDTLBSet(uint32_t index)
{
	uint32_t way;

	TLBSet<typename CONFIG::DTLB_CONFIG>& dtlb_set = dtlb[index];

	dtlb_set.status.plru_bits = 0;
	
	for(way = 0; way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::DTLB_CONFIG>& dtlb_entry = dtlb_set[way];

		dtlb_entry.status.valid = false;
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDTLB()
{
	uint32_t index;
	
	for(index = 0; index < TLB<typename CONFIG::DTLB_CONFIG>::NUM_SETS; index++)
	{
		InvalidateDTLBSet(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::LookupITLB(MMUAccess<CONFIG>& mmu_access)
{
	typename TLB<typename CONFIG::ITLB_CONFIG>::VIRTUAL_ADDRESS virtual_addr = mmu_access.virtual_addr;
	typename TLB<typename CONFIG::ITLB_CONFIG>::VIRTUAL_ADDRESS base_virtual_addr;
	uint32_t tlb_index;
	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;
	uint32_t tlb_way;

	itlb.DecodeAddress(virtual_addr, base_virtual_addr, tlb_index);
	mmu_access.base_virtual_addr = base_virtual_addr;
	mmu_access.tlb_index = tlb_index;
	mmu_access.itlb_set = tlb_set = &itlb[tlb_index];

	for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
	{
		tlb_entry = &(*tlb_set)[tlb_way];
		if(tlb_entry->status.valid && tlb_entry->GetBaseVirtualAddr() == base_virtual_addr)
		{
			// ITLB Hit
			// return WIMG bits
			mmu_access.tlb_way = tlb_way;
			mmu_access.itlb_entry = tlb_entry;
			mmu_access.tlb_hit = true;
			return;
		}
	}

	// ITLB Miss
	mmu_access.tlb_hit = false;
	ChooseEntryToEvictITLB(mmu_access);
}

template <class CONFIG>
void CPU<CONFIG>::LookupDTLB(MMUAccess<CONFIG>& mmu_access)
{
	typename TLB<typename CONFIG::DTLB_CONFIG>::VIRTUAL_ADDRESS virtual_addr = mmu_access.virtual_addr;
	typename TLB<typename CONFIG::DTLB_CONFIG>::VIRTUAL_ADDRESS base_virtual_addr;
	uint32_t tlb_index;
	TLBSet<typename CONFIG::DTLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::DTLB_CONFIG> *tlb_entry;
	uint32_t tlb_way;

	dtlb.DecodeAddress(virtual_addr, base_virtual_addr, tlb_index);
	if(IsVerboseDTLB())
	{
		(*logger_import) << DebugInfo << "DTLB Lookup at 0x";
		(*logger_import) << Hex << virtual_addr << Dec << " : ";
		(*logger_import) << "base_virtual_addr=0x" << Hex << base_virtual_addr << Dec << ",";
		(*logger_import) << "tlb_index=0x" << Hex << tlb_index << Dec << ",";
		(*logger_import) << Endl << EndDebugInfo;
	}
	mmu_access.base_virtual_addr = base_virtual_addr;
	mmu_access.tlb_index = tlb_index;
	mmu_access.dtlb_set = tlb_set = &dtlb[tlb_index];

	for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
	{
		tlb_entry = &(*tlb_set)[tlb_way];
		if(tlb_entry->status.valid && tlb_entry->GetBaseVirtualAddr() == base_virtual_addr)
		{
			// DTLB Hit
			if(IsVerboseDTLB())
			{
				(*logger_import) << DebugInfo << "DTLB hit: tlb_way=" << tlb_way << ", base_physical_addr=0x" << Hex << tlb_entry->pte.base_physical_addr << Dec << Endl << EndDebugInfo;
			}
			mmu_access.tlb_way = tlb_way;
			mmu_access.dtlb_entry = tlb_entry;
			mmu_access.tlb_hit = true;
			return;
		}
	}

	// DTLB Miss
	mmu_access.tlb_hit = false;
	ChooseEntryToEvictDTLB(mmu_access);
	if(IsVerboseDTLB())
	{
		(*logger_import) << DebugInfo << "DTLB miss: choosen TLB way=" << mmu_access.tlb_way << Endl << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::UpdateITLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = mmu_access.tlb_way;
	uint32_t plru_bits = mmu_access.itlb_set->status.plru_bits;

	for(n = CONFIG::ITLB_CONFIG::TLB_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	mmu_access.itlb_set->status.plru_bits = plru_bits;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateDTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = mmu_access.tlb_way;
	uint32_t plru_bits = mmu_access.dtlb_set->status.plru_bits;

	for(n = CONFIG::DTLB_CONFIG::TLB_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	mmu_access.dtlb_set->status.plru_bits = plru_bits;
}

template <class CONFIG>
void CPU<CONFIG>::ChooseEntryToEvictITLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t way = 0;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = mmu_access.itlb_set->status.plru_bits;

	for(n = 0; n < CONFIG::ITLB_CONFIG::TLB_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}

	mmu_access.tlb_way = way;
	mmu_access.itlb_entry = &(*mmu_access.itlb_set)[way];
}

template <class CONFIG>
void CPU<CONFIG>::ChooseEntryToEvictDTLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t way = 0;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = mmu_access.dtlb_set->status.plru_bits;

	for(n = 0; n < CONFIG::DTLB_CONFIG::TLB_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}

	mmu_access.tlb_way = way;
	mmu_access.dtlb_entry = &(*mmu_access.dtlb_set)[way];
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::AccessTLB(MMUAccess<CONFIG>& mmu_access)
{
	address_t addr = mmu_access.addr;
	PrivilegeLevel privilege_level = mmu_access.privilege_level;
	MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	MemoryType memory_type = mmu_access.memory_type;
	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field
	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	virtual_address_t virtual_addr;			// Virtual address
	virtual_address_t base_virtual_addr;	// Base virtual address
	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key;
	WIMG wimg;
	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;

	// Compute the protection boundary
	mmu_access.protection_boundary = (addr + (MEMORY_PAGE_SIZE)) & ~(MEMORY_PAGE_SIZE - 1);

	//-------------------------------
	//- Compute the virtual address -
	//-------------------------------

	// Get the segment register number from the most significative bits in the effective address
	mmu_access.sr_num = sr_num = (addr >> 28) & 0xf;
	
	// Get the No-execute bit from the segment register
	mmu_access.sr_noexecute = sr_noexecute = GetSR_N(sr_num);

	if((memory_type == MT_INSN) && sr_noexecute)
	{
		// Raise an exception because the program is trying to execute instructions within a No-execute segment
		if(!DEBUG) throw ISINoExecuteException<CONFIG>(addr);
	}
	
	if(GetSR_T(sr_num) == 1)
	{
		// Raise an exception because the program is trying to access to a direct-store segment (not supported by PPC750)
		if(memory_type == MT_INSN)
		{
			if(!DEBUG) throw ISIDirectStoreException<CONFIG>(addr);
		}
		else
		{
			if(!DEBUG) throw DSIDirectStoreException<CONFIG>(addr, memory_access_type);
		}
	}
	
	// Get the key bits from the segment register
	mmu_access.sr_ks = sr_ks = GetSR_KS(sr_num);
	mmu_access.sr_kp = sr_kp = GetSR_KP(sr_num);
	mmu_access.key = key = (sr_kp && (privilege_level & PR_USER)) || (sr_ks && (privilege_level & PR_SUPERVISOR));
	
	// Get the virtual segment id from the segment register
	mmu_access.virtual_segment_id = virtual_segment_id = GetSR_VSID(sr_num);
	
	// Compute the virtual address from the virtual segment id and the least significant bits of the effective address
	mmu_access.virtual_addr = virtual_addr = ((virtual_address_t) virtual_segment_id << 28) | (virtual_address_t)(addr & 0x0fffffffUL);
	
	mmu_access.force_page_table_walk = false;
	//------------------------------
	//-    Search in ITLB/DTLB     -
	//------------------------------
	if(memory_type == MT_INSN)
	{
		if(CONFIG::ITLB_CONFIG::ENABLE)
		{
			// Memory access is an instruction fetch
			LookupITLB(mmu_access);
			if(mmu_access.tlb_hit)
			{
				// ITLB hit
				mmu_access.wimg = wimg = mmu_access.itlb_entry->pte.wimg;
						
				if(wimg & CONFIG::WIMG_GUARDED_MEMORY)
				{
					if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
				}
			
				// Check access rights
				if(key && !mmu_access.itlb_entry->pte.pp)
				{
					if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
				}
			
				// Update the replacement policy
				if(!DEBUG) UpdateITLBReplacementPolicy(mmu_access);
				
				// compute the physical address
				mmu_access.physical_addr = mmu_access.itlb_entry->pte.base_physical_addr | (addr & 0x00000fffUL);
			}
		
			// ITLB Miss
			return;
		}

		mmu_access.itlb_set = 0;
		mmu_access.itlb_entry = 0;
	}
	else
	{
		if(CONFIG::DTLB_CONFIG::ENABLE)
		{
			// Memory access is a data memory access
			LookupDTLB(mmu_access);

			if(mmu_access.tlb_hit)
			{
				// DTLB Hit
				// return WIMG bits
				mmu_access.wimg = wimg = mmu_access.dtlb_entry->pte.wimg;
	
				// Check access rights
				if((key && (!mmu_access.dtlb_entry->pte.pp ||
				   ((mmu_access.dtlb_entry->pte.pp & 1) && (memory_access_type == MAT_WRITE)))) ||
                   (!key && (mmu_access.dtlb_entry->pte.pp==3 && (memory_access_type == MAT_WRITE))))
				{
					// Raise a protection violation
					if(!DEBUG)
					{
						throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				mmu_access.force_page_table_walk = memory_access_type == MAT_WRITE && !mmu_access.dtlb_entry->pte.c;

				// Update the replacement policy
				if(!DEBUG && !mmu_access.force_page_table_walk) UpdateDTLBReplacementPolicy(mmu_access);
					
				// compute the physical address
				mmu_access.physical_addr = mmu_access.dtlb_entry->pte.base_physical_addr | (addr & 0x00000fffUL);
				//cerr << "DTLB hit at 0x" << std::hex << addr << std::dec << endl;
			}
			// DTLB Miss
			return;
		}

		mmu_access.dtlb_set = 0;
		mmu_access.dtlb_entry = 0;
	}
	// Miss in ITLB/DTLB
	mmu_access.tlb_hit = false;
	mmu_access.base_virtual_addr = base_virtual_addr = virtual_addr & ~((virtual_address_t) MEMORY_PAGE_SIZE - 1);
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::HardwarePageTableSearch(MMUAccess<CONFIG>& mmu_access)
{
	address_t addr = mmu_access.addr;
	PrivilegeLevel privilege_level = mmu_access.privilege_level;
	MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	MemoryType memory_type = mmu_access.memory_type;
	uint32_t sr_num = mmu_access.sr_num;						// Segment register number
	uint32_t virtual_segment_id = mmu_access.virtual_segment_id;			// Virtual segment id
	virtual_address_t virtual_addr = mmu_access.virtual_addr;			// Virtual address
	virtual_address_t base_virtual_addr = mmu_access.base_virtual_addr;	// Base virtual address
	uint32_t tlb_index = mmu_access.tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key = mmu_access.key;
	WIMG wimg;

	// Compute the page index from the virtual address
	mmu_access.page_index = page_index = (base_virtual_addr >> 12) & 0xffff;

	// Compute API from the page index
	mmu_access.api = api = page_index >> 10;

	if(CONFIG::HAS_SOFTWARE_TABLE_SEARCH_SUPPORT)
	{
		if(!DEBUG && CONFIG::HAS_HID2 && GetHID2_SWT_EN())
		{
			// Software page table search is enabled
			
			/* primary PTEG address */
			hash = (virtual_segment_id & 0x7ffff) ^ page_index;
			pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
			physical_address_t primary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
				
			/* secondary PTEG address */
			hash = (~hash) & 0x7ffff;
			pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
			physical_address_t secondary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
			
			// raise a TLB miss
			throw TLBMissException<CONFIG>(
				memory_access_type,
				memory_type,
				addr,
				mmu_access.tlb_way,
				(privilege_level & PR_USER) ? GetSR_KP(sr_num) : GetSR_KS(sr_num),
				virtual_segment_id,
				api,
				primary_pteg,
				secondary_pteg);
		}
	}
	
	//-------------------------------------
	//- Hardware search in the page table -
	//-------------------------------------

	// search with the primary hash key and if needed with the secondary hash key
	for(h = 0, hash = (virtual_segment_id & 0x7ffff) ^ page_index; h < 2; h++, hash = (~hash) & 0x7ffff)
	{
		// Compute the base address of a group of 8 page table entries
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);

		// Scan each page table entry in the group
		for(pte_num = 0; pte_num < 8; pte_num++, pte_addr += 8)
		{
			uint64_t pte_value;
			uint32_t pte_virtual_segment_id;
			uint32_t pte_api;
			uint32_t pte_valid;
			uint32_t pte_h;
			uint32_t pte_pp;
			WIMG pte_wimg;
			uint32_t pte_c;
			physical_address_t base_physical_addr;


//			cerr << "pte_addr = 0x" << std::hex << pte_addr << std::dec << endl;
			// Read the page table entry from memory
			if(DEBUG)
			{
				ReadMemory(pte_addr, &pte_value, 8, MT_DATA, false);
			}
			else
			{
				Load<false>(pte_addr, &pte_value, 8);
			}

			pte_value = BigEndian2Host(pte_value);
//			cerr << "pte_value = 0x" << std::hex << pte_value << std::dec << endl;

			pte_valid = (pte_value >> 63) & 1;
//			cerr << "pte_valid = " << pte_valid << endl;
			if(!pte_valid) // check valid bit
				continue;
			pte_h = (pte_value >> 38) & 1;
//			cerr << "pte_h = " << pte_h << endl;
			if(pte_h != h) // good hash key ?
				continue;

			pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
//			cerr << "pte_virtual_segment_id = 0x" << std::hex << pte_virtual_segment_id << std::dec << endl;
			pte_api = (pte_value >> 32) & 0x3f;
			
			if(pte_virtual_segment_id == virtual_segment_id && // compare VSIDs
						pte_api == api) // compare APIs
			{
				// Hit in page table
				pte_wimg = (WIMG)((pte_value >> 3) & 0xf);
				mmu_access.wimg = wimg = pte_wimg;
				
				// Check if access is guarded
				if((memory_type == MT_INSN) && (wimg & CONFIG::WIMG_GUARDED_MEMORY))
				{
					if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
				}

				pte_pp = (pte_value & 3);
				
				// Check access rights
				if((key && (!pte_pp || ((pte_pp & 1) && (memory_access_type == MAT_WRITE)))) || (!key && (pte_pp==3 && (memory_access_type == MAT_WRITE))))
				{
					// Raise a protection violation
					if(memory_type == MT_INSN)
					{
						if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
					}
					else
					{
						if(!DEBUG) throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				// Update bit R and C in PTE
				pte_value |= 0x100ULL; // set accessed bit (R)
				if((memory_access_type == MAT_WRITE)) pte_value |= 0x80ULL; // set change bit (C)

				base_physical_addr = pte_value & 0xfffff000UL;
//				cerr << "base_physical_addr = 0x" << std::hex << base_physical_addr << std::dec << endl;
				
				// Build an access attribute for the page	
				pte_c = (pte_value >> 7) & 1;

				// Update the page table entry into memory
				pte_value = Host2BigEndian(pte_value);

				if(!DEBUG)
				{
					Store<false>(pte_addr, &pte_value, 8);
				}

				if(memory_type == MT_INSN)
				{
					if(CONFIG::ITLB_CONFIG::ENABLE)
					{
						mmu_access.itlb_entry->status.valid = true;
						mmu_access.itlb_entry->SetBaseVirtualAddr(base_virtual_addr);
						mmu_access.itlb_entry->pte.c = pte_c;
						mmu_access.itlb_entry->pte.wimg = pte_wimg;
						mmu_access.itlb_entry->pte.pp = pte_pp;
						mmu_access.itlb_entry->pte.base_physical_addr = base_physical_addr;
	
						if(!DEBUG)
						{
							UpdateITLBReplacementPolicy(mmu_access);
						}
					}
				}
				else
				{
					if(CONFIG::DTLB_CONFIG::ENABLE)
					{
						mmu_access.dtlb_entry->status.valid = true;
						mmu_access.dtlb_entry->SetBaseVirtualAddr(base_virtual_addr);
						mmu_access.dtlb_entry->pte.c = pte_c;
						mmu_access.dtlb_entry->pte.wimg = pte_wimg;
						mmu_access.dtlb_entry->pte.pp = pte_pp;
						mmu_access.dtlb_entry->pte.base_physical_addr = base_physical_addr;
	
						if(!DEBUG)
						{
							UpdateDTLBReplacementPolicy(mmu_access);
						}
					}
				}
			
				// compute the physical address
				mmu_access.physical_addr = base_physical_addr | (addr & 0x00000fffUL);
				return;
			}
		}
	}

	// Miss in the page table
	// Raise a page fault
	//DumpPageTable(cerr);
	
	if(memory_type == MT_INSN)
	{
		if(!DEBUG)
		{
			throw ISIPageFaultException<CONFIG>(addr);
		}
	}
	else
	{
		if(!DEBUG)
		{
			throw DSIPageFaultException<CONFIG>(addr, memory_access_type);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpPageTable(ostream& os)
{
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash[2];						// hash keys
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	uint32_t mask = GetSDR1_HTABMASK();
	uint32_t hash_lo, hash_hi;
	physical_address_t pteg_select;
	
	os << "HTABORG = 0x" << std::hex << GetSDR1_HTABORG() << std::dec << endl;
	os << "HTABMASK = 0x" << std::hex << GetSDR1_HTABMASK() << std::dec << endl;
	
	
	for(hash_hi = 0; hash_hi <= mask; hash_hi++)
	{
		for(hash_lo = 0; hash_lo < 1024; hash_lo++)
		{
			hash[0] = (hash_hi << 10) | hash_lo;
			hash[1] = (~hash[0]) & 0x7ffff;
			
			for(h = 0; h < 2; h++)
			{
				pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash[h] >> 10) & 0x1ff))) << 10) | (hash[h] & 0x3ff);
				pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
		
				for(pte_num = 0; pte_num < 8; pte_num++, pte_addr += 8)
				{
					uint64_t pte_value;
					virtual_address_t base_virtual_addr;
					uint32_t pte_valid, pte_h, pte_virtual_segment_id, pte_api, pte_pp, pte_r, pte_c, pte_wimg;
					uint32_t page_index, api;
					address_t base_paddr;

					ReadMemory(pte_addr, &pte_value, 8, MT_DATA, false);
		
					pte_value = BigEndian2Host(pte_value);
		
					pte_valid = (pte_value >> 63) & 1;
					if(!pte_valid) continue;
					pte_h = (pte_value >> 38) & 1;
					if(pte_h != h) continue;
					
					pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
					pte_api = (pte_value >> 32) & 0x3f;
					
					page_index = (pte_virtual_segment_id & 0x7ffff) ^ hash[0];
					api = page_index >> 10; 
					base_virtual_addr = (pte_virtual_segment_id << 28) | (page_index << 12);
					if(pte_valid && pte_api == api)
					{
						pte_r = (pte_value >> 8) & 1;
						pte_c = (pte_value >> 7) & 1;
						pte_pp = (pte_value & 3);
						pte_wimg = (pte_value >> 3) & 0xfUL;
						base_paddr = pte_value & 0xfffff000UL;
						
						os << "0x" << std::hex << pte_addr << ": base_virt_addr=0x" << base_virtual_addr << " hash=0x" << hash << " V=" << pte_valid << " VSID=0x" << pte_virtual_segment_id << " H=" << pte_h << " API=0x" << pte_api << " pp=" << pte_pp << " r=" << pte_r << " c=" << pte_c << " base_paddr=0x" << base_paddr << " wimg=" << pte_wimg << std::dec << endl;
					}
				}
			}
		}
	}
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::TranslateAddress(MMUAccess<CONFIG>& mmu_access)
{
	LookupBAT<DEBUG>(mmu_access);
	if(!mmu_access.bat_hit)
	{
		AccessTLB<DEBUG>(mmu_access);

		if(!mmu_access.tlb_hit || mmu_access.force_page_table_walk)
		{
			// BAT/TLB Miss
			HardwarePageTableSearch<DEBUG>(mmu_access);
		}
	}
}

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(address_t addr, void *buffer, uint32_t size, MemoryType mt, bool translate_addr)
{
	uint32_t read_offset = 0;

	do
	{
		WIMG wimg;
		physical_address_t physical_addr;
		address_t protection_boundary;
		if(translate_addr && GetMSR_DR())
		{
			// Address translation
			MMUAccess<CONFIG> mmu_access;
		
			mmu_access.addr = addr;
			mmu_access.privilege_level = GetPrivilegeLevel();
			mmu_access.memory_access_type = MAT_READ;
			mmu_access.memory_type = mt;
	
			TranslateAddress<true>(mmu_access); // debug is enabled
	
			wimg = mmu_access.wimg;
			physical_addr = mmu_access.physical_addr;
			protection_boundary = mmu_access.protection_boundary;
		}
		else
		{
			// W=0: Write through disabled
			// I=0: Cache not inhibited
			// M=1: Memory coherency enforced
			// G=1: Guarded memory access (speculative fetch forbidden)
			wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
			physical_addr = addr;
			protection_boundary = addr + size;
		}

		uint32_t size_to_protection_boundary = protection_boundary - addr;

		do
		{
			uint32_t sz = 0;

			if(IsVerboseReadMemory())
			{
				(*logger_import) << DebugInfo << "0x" << Hex << addr << Dec << ":";
			}

			switch(mt)
			{
				case MT_DATA:
					if(IsDataCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupDL1(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy((uint8_t *) buffer + read_offset, &(*l1_access.block)[l1_access.offset], l1_sz);
							sz = l1_sz;
							if(IsVerboseReadMemory())
							{
								(*logger_import) << "DL1: ";
							}
						}
					}
					break;
				case MT_INSN:
					if(IsInsnCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupIL1(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy((uint8_t *) buffer + read_offset, &(*l1_access.block)[l1_access.offset], l1_sz);
							sz = l1_sz;
							if(IsVerboseReadMemory())
							{
								(*logger_import) << "IL1: ";
							}
						}
					}
					break;
			}

			if(!sz && IsL2CacheEnabled())
			{
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
				l2_access.addr = addr;
				LookupL2(l2_access);

				if(l2_access.block)
				{
					// L2 hit
					uint32_t l2_sz = (l2_access.size_to_block_boundary > size) ? size : l2_access.size_to_block_boundary;
					memcpy((uint8_t *) buffer + read_offset, &(*l2_access.block)[l2_access.offset], l2_sz);
					sz = l2_sz;
					if(IsVerboseReadMemory())
					{
						(*logger_import) << "L2: ";
					}
				}
			}

			if(!sz)
			{
				sz = (size_to_protection_boundary > size) ? size : size_to_protection_boundary;
				if(!memory_import->ReadMemory(physical_addr, (uint8_t *) buffer + read_offset, sz)) return false;
				if(IsVerboseReadMemory())
				{
					(*logger_import) << "MEM: ";
				}
			}

			if(IsVerboseReadMemory())
			{
				uint32_t i;
				for(i = 0; i < sz; i++)
				{
					(*logger_import) << Hex << (unsigned int)((((uint8_t *) buffer)[read_offset + i]) >> 4) << (unsigned int)((((uint8_t *) buffer)[read_offset + i]) & 15) << Dec << " ";
				}
				(*logger_import) << Endl << EndDebugInfo;
			}

			size -= sz;
			read_offset += sz;
			addr += sz;
			size_to_protection_boundary -= sz;
		} while(size > 0 && size_to_protection_boundary > 0);
	} while(size > 0);

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size, MemoryType mt, bool translate_addr)
{
	uint32_t write_offset = 0;

	do
	{
		WIMG wimg;
		physical_address_t physical_addr;
		address_t protection_boundary;
		if(translate_addr && GetMSR_DR())
		{
			// Address translation
			MMUAccess<CONFIG> mmu_access;
		
			mmu_access.addr = addr;
			mmu_access.privilege_level = PR_SUPERVISOR;
			mmu_access.memory_access_type = MAT_WRITE;
			mmu_access.memory_type = mt;
	
			TranslateAddress<true>(mmu_access); // debug is enabled
	
			wimg = mmu_access.wimg;
			physical_addr = mmu_access.physical_addr;
			protection_boundary = mmu_access.protection_boundary;
		}
		else
		{
			// W=0: Write through disabled
			// I=0: Cache not inhibited
			// M=1: Memory coherency enforced
			// G=1: Guarded memory access (speculative fetch forbidden)
			wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
			physical_addr = addr;
			protection_boundary = addr + size;
		}

		uint32_t size_to_protection_boundary = protection_boundary - addr;

		do
		{
			uint32_t sz = 0;

			if(IsVerboseWriteMemory())
			{
				(*logger_import) << DebugInfo << "0x" << Hex << addr << Dec << ":";
			}

			switch(mt)
			{
				case MT_DATA:
					if(IsDataCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupDL1(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy(&(*l1_access.block)[l1_access.offset], (uint8_t *) buffer + write_offset, l1_sz);
							sz = l1_sz;
							if(IsVerboseWriteMemory())
							{
								(*logger_import) << "DL1: ";
							}
						}
					}
					break;
				case MT_INSN:
					if(IsInsnCacheEnabled())
					{
						// DL1 Access
						CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
					
						l1_access.addr = physical_addr;
						LookupIL1(l1_access);
		
						if(l1_access.block)
						{
							// L1 Hit
							uint32_t l1_sz = (l1_access.size_to_block_boundary > size) ? size : l1_access.size_to_block_boundary;
							memcpy(&l1_access.block[l1_access.offset], (uint8_t *) buffer + write_offset, l1_sz);
							sz = l1_sz;
							if(IsVerboseWriteMemory())
							{
								(*logger_import) << "IL1: ";
							}
						}
					}
					break;
			}

			if(!sz && IsL2CacheEnabled())
			{
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
				l2_access.addr = addr;
				LookupL2(l2_access);

				if(l2_access.block)
				{
					// L2 hit
					uint32_t l2_sz = (l2_access.size_to_block_boundary > size) ? size : l2_access.size_to_block_boundary;
					memcpy(&(*l2_access.block)[l2_access.offset], (uint8_t *) buffer + write_offset, l2_sz);
					sz = l2_sz;
					if(IsVerboseWriteMemory())
					{
						(*logger_import) << "L2: ";
					}
				}
			}

			if(!sz)
			{
				sz = (size_to_protection_boundary > size) ? size : size_to_protection_boundary;
				if(!memory_import->WriteMemory(physical_addr, (uint8_t *) buffer + write_offset, sz)) return false;
				if(IsVerboseWriteMemory())
				{
					(*logger_import) << "MEM: ";
				}
			}

			if(IsVerboseWriteMemory())
			{
				uint32_t i;
				for(i = 0; i < sz; i++)
				{
					(*logger_import) << Hex << (unsigned int)((((uint8_t *) buffer)[write_offset + i]) >> 4) << (unsigned int)((((uint8_t *) buffer)[write_offset + i]) & 15) << Dec << " ";
				}
				(*logger_import) << Endl << EndDebugInfo;
			}

			size -= sz;
			write_offset += sz;
			addr += sz;
			size_to_protection_boundary -= sz;
		} while(size > 0 && size_to_protection_boundary > 0);
	} while(size > 0);

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(address_t addr, void *buffer, uint32_t size)
{
	return ReadMemory(addr, buffer, size, MT_DATA, true);
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	return WriteMemory(addr, buffer, size, MT_DATA, true);
}

template <class CONFIG>
bool CPU<CONFIG>::InjectReadMemory(address_t addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		uint32_t sz;
		uint8_t *dst = (uint8_t *) buffer;
		do
		{
			uint32_t size_to_fsb_boundary = FSB_WIDTH - (addr & (FSB_WIDTH - 1));
			sz = size > size_to_fsb_boundary ? size_to_fsb_boundary : size;
			Load<true>(addr, dst, sz);
			dst += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		uint32_t sz;
		const uint8_t *src = (const uint8_t *) buffer;
		do
		{
			uint32_t size_to_fsb_boundary = FSB_WIDTH - (addr & (FSB_WIDTH - 1));
			sz = size > size_to_fsb_boundary ? size_to_fsb_boundary : size;
			Store<true>(addr, src, sz);
			src += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}

template <class CONFIG>
string CPU<CONFIG>::Disasm(address_t addr, address_t& next_addr)
{
	stringstream sstr;
	Operation<CONFIG> *operation;
	uint32_t insn;

	WIMG wimg;
	physical_address_t physical_addr;

	if(GetMSR_IR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_READ;
		mmu_access.memory_type = MT_INSN;
	
		TranslateAddress<true>(mmu_access);
	
		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	bool hit = false;

	// Is IL1 enabled ?
	if(IsInsnCacheEnabled())
	{
		// IL1 Access
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupIL1(l1_access);

		if(l1_access.block)
		{
			// L1 hit
			memcpy(&insn, &(*l1_access.block)[l1_access.offset], 4);
			hit = true;
		} else if(IsL2CacheEnabled())
		{
			CacheAccess<class CONFIG::L2_CONFIG> l2_access;
		
			l2_access.addr = physical_addr;

			LookupL2(l2_access);

			if(l2_access.block)
			{
				// L2 hit
				memcpy(&insn, &(*l2_access.block)[l2_access.offset], 4);
				hit = true;
			}
		}
	}

	if(!hit)
	{
		if(!memory_import->ReadMemory(physical_addr, &insn, 4)) return string("unreadable ?");
	}

#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(insn);
#endif

	operation = Decoder<CONFIG>::Decode(addr, insn);

	// disassemble the instruction
	sstr << "0x" << std::hex;
	sstr.fill('0'); sstr.width(8); 
	sstr << operation->GetEncoding() << std::dec << " ";
	operation->disasm(this, sstr);

	next_addr = addr + 4;
	return sstr.str();
}

/* PowerPC Linux OS Interface */
template <class CONFIG>
void CPU<CONFIG>::PerformExit(int ret)
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
	Dcbz(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dcbf(address_t addr)
{
	Dcbst(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dcbi(address_t addr)
{
	if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();

	WIMG wimg;
	physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_WRITE;
		mmu_access.memory_type = MT_DATA;

		TranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	bool dirty = false;

	// DL1 Access
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = physical_addr;

	LookupDL1(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_flush = (*l1_access.line)[l1_sector];
	
			if(l1_block_to_flush.status.valid && l1_block_to_flush.status.dirty)
			{
				// DL1 block is dirty
				// Write back to L2
				dirty = true;
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = l1_block_to_flush.GetBaseAddr();
	
				LookupL2(l2_access);
	
				if(l2_access.block)
				{
					memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_flush[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
					l2_access.block->status.dirty = true;
				}
				else
				{
					if(IsVerboseDL1())
					{
						(*logger_import) << DebugInfo << "DL1: flushing/invalidating dirty block at 0x" << Hex << l1_block_to_flush.GetBaseAddr() << Dec << " into memory" << Endl;
					}
					// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l1_block_to_flush.GetBaseAddr(), &l1_block_to_flush[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				}
			}
			l1_block_to_flush.status.valid = false;
			l1_block_to_flush.status.dirty = false;
		}
		l1_access.line->status.valid = false;
	}

	CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	l2_access.addr = physical_addr;

	LookupL2(l2_access);

	if(l2_access.line)
	{
		uint32_t l2_sector;
	
		for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
		{
			CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_flush = (*l2_access.line)[l2_sector];

			if(l2_block_to_flush.status.valid && l2_block_to_flush.status.dirty)
			{
				dirty = true;
				if(IsVerboseL2())
				{
					(*logger_import) << DebugInfo << "L2: flushing/invalidating dirty block at 0x" << Hex << l2_block_to_flush.GetBaseAddr() << Dec << " into memory" << Endl;
				}
				// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l2_block_to_flush.GetBaseAddr(), &l2_block_to_flush[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
			}
			l2_block_to_flush.status.valid = false;
			l2_block_to_flush.status.dirty = false;
		}
		l2_access.line->status.valid = false;
	}

	if(dirty || (IsAddressBroadcastEnabled() && (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED)))
	{
		BusFlushBlock(physical_addr);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbst(address_t addr)
{
	WIMG wimg;
	physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_READ; // 3.4.4.4 Data Cache Block Store (dcbst): This instruction is treated as a load with respect to address translation and memory protection
		mmu_access.memory_type = MT_DATA;

		TranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	bool dirty = false;

	// DL1 Access
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = physical_addr;

	LookupDL1(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_flush = (*l1_access.line)[l1_sector];
	
			if(l1_block_to_flush.status.valid && l1_block_to_flush.status.dirty)
			{
				// DL1 block is dirty
				// Write back to L2
				dirty = true;
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = l1_block_to_flush.GetBaseAddr();
	
				LookupL2(l2_access);
	
				if(l2_access.block)
				{
					memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_flush[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
					l2_access.block->status.dirty = true;
				}
				else
				{
					if(IsVerboseDL1())
					{
						(*logger_import) << DebugInfo << "DL1: flushing/invalidating dirty block at 0x" << Hex << l1_block_to_flush.GetBaseAddr() << Dec << " into memory" << Endl;
					}
					// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l1_block_to_flush.GetBaseAddr(), &l1_block_to_flush[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				}
			}
			l1_block_to_flush.status.valid = false;
			l1_block_to_flush.status.dirty = false;
		}
		l1_access.line->status.valid = false;
	}

	CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	l2_access.addr = physical_addr;

	LookupL2(l2_access);

	if(l2_access.line)
	{
		uint32_t l2_sector;
	
		for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
		{
			CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_flush = (*l2_access.line)[l2_sector];

			if(l2_block_to_flush.status.valid && l2_block_to_flush.status.dirty)
			{
				dirty = true;
				if(IsVerboseL2())
				{
					(*logger_import) << DebugInfo << "L2: flushing/invalidating dirty block at 0x" << Hex << l2_block_to_flush.GetBaseAddr() << Dec << " into memory" << Endl;
				}
				// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l2_block_to_flush.GetBaseAddr(), &l2_block_to_flush[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
			}
			l2_block_to_flush.status.valid = false;
			l2_block_to_flush.status.dirty = false;
		}
		l2_access.line->status.valid = false;
	}

	if(dirty || (IsAddressBroadcastEnabled() && (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED)))
	{
		BusFlushBlock(physical_addr);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbz(address_t addr)
{
	if(CONFIG::DL1_CONFIG::ENABLE && !IsDataCacheEnabled())
	{
		throw AlignmentException<CONFIG>(addr);
	}

	WIMG wimg;
	physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = MAT_WRITE; // 3.4.4.3 Data Cache Block Zero (dcbz): The dcbz instruction is treated as a store to the addressed byte with respect to address translation, protection, and pipelining.
		mmu_access.memory_type = MT_DATA;

		TranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	if((wimg & CONFIG::WIMG_CACHE_INHIBITED) || (wimg & CONFIG::WIMG_WRITE_THROUGH))
	{
		throw AlignmentException<CONFIG>(addr);
	}

	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);
	
		if(!l1_access.line)
		{
			EvictDL1(l1_access);
		}
	
		if(!l1_access.block)
		{
			l1_access.block = &(*l1_access.line)[l1_access.sector];
		}
		
		// DL1 block zero
		l1_access.line->status.valid = true;
		l1_access.line->SetBaseAddr(l1_access.line_base_addr);
		l1_access.block->Zero();
		l1_access.block->status.valid = true;
		l1_access.block->status.dirty = true;
		UpdateReplacementPolicyDL1(l1_access);

		if(IsL2CacheEnabled())
		{
			// L2 Access
			CacheAccess<class CONFIG::L2_CONFIG> l2_access;
		
			l2_access.addr = physical_addr;
			LookupL2(l2_access);
		
			if(!l2_access.line)
			{
				EvictL2(l2_access);
			}
		
			if(!l2_access.block)
			{
				l2_access.block = &(*l2_access.line)[l2_access.sector];
			}
			
			// L2 block zero
			l2_access.line->status.valid = true;
			l2_access.line->SetBaseAddr(l2_access.line_base_addr);
			l2_access.block->Zero();
			l2_access.block->status.valid = true;
			l2_access.block->status.dirty = true;
			UpdateReplacementPolicyL2(l2_access);
		}
	}
	else
	{
		// just in case we don't have a data cache
		uint8_t zero[32];
		memset(zero, 0, sizeof(zero));
		BusWrite(physical_addr & (~31), zero, sizeof(zero));
	}
	
	// broadcast zero block on the bus
	BusZeroBlock(physical_addr);
}

/* Instruction Cache Management */
template <class CONFIG>
void CPU<CONFIG>::Icbi(address_t addr)
{
//	mmu.InvalidateInsnCacheBlock(addr, GetPrivilegeLevel());

	typename CONFIG::IL1_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::IL1_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;

	// Decode the address
	il1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);

	InvalidateIL1Set(index);

	Decoder<CONFIG>::InvalidateDecodingCacheEntry(addr);
}

/* TLB management */
template <class CONFIG>
void CPU<CONFIG>::Tlbia()
{
	if(IsMPC7XX() || IsMPC7XXX())
	{
		throw IllegalInstructionException<CONFIG>();
	}

	InvalidateDTLB();
	InvalidateITLB();
}

template <class CONFIG>
void CPU<CONFIG>::Tlbie(address_t addr)
{
	InvalidateDTLBSet((addr / MEMORY_PAGE_SIZE) & ((CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES / CONFIG::DTLB_CONFIG::TLB_ASSOCIATIVITY) - 1));
	InvalidateITLBSet((addr / MEMORY_PAGE_SIZE) & ((CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES / CONFIG::ITLB_CONFIG::TLB_ASSOCIATIVITY) - 1));
}

template <class CONFIG>
void CPU<CONFIG>::LoadITLBEntry(address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::ITLB_CONFIG::ENABLE)
	{
		PTE *pte;
		uint32_t index = addr / MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		address_t base_physical_addr = pte_lo & ~(MEMORY_PAGE_SIZE - 1);
		virtual_address_t base_virtual_addr = ((virtual_address_t) virtual_segment_id << 28) | (virtual_address_t)(addr & 0x0fffffffUL & ~(MEMORY_PAGE_SIZE - 1));
		
		TLBEntry<typename CONFIG::ITLB_CONFIG>& tlb_entry = itlb[index][way];
	
		tlb_entry.status.valid = ((pte_hi >> 31) & 1) ? true : false;
		tlb_entry.SetBaseVirtualAddr(base_virtual_addr);
		tlb_entry.pte.c = (pte_lo >> 7) & 1;
		tlb_entry.pte.wimg = (WIMG)((pte_lo >> 3) & 0xf);
		tlb_entry.pte.pp = (pte_lo & 3);
		tlb_entry.pte.base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void CPU<CONFIG>::LoadDTLBEntry(address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::DTLB_CONFIG::ENABLE)
	{
		PTE *pte;
		uint32_t index = addr / MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		address_t base_physical_addr = pte_lo & ~(MEMORY_PAGE_SIZE - 1);
		virtual_address_t base_virtual_addr = ((virtual_address_t) virtual_segment_id << 28) | (virtual_address_t)(addr & 0x0fffffffUL & ~(MEMORY_PAGE_SIZE - 1));
		
		TLBEntry<typename CONFIG::DTLB_CONFIG>& tlb_entry = dtlb[index][way];
	
		tlb_entry.SetBaseVirtualAddr(base_virtual_addr);
		tlb_entry.pte.c = (pte_lo >> 7) & 1;
		tlb_entry.pte.wimg = (WIMG)((pte_lo >> 3) & 0xf);
		tlb_entry.pte.pp = (pte_lo & 3);
		tlb_entry.pte.base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbld(address_t addr)
{
	Stop(-1);
	throw IllegalInstructionException<CONFIG>();
	if(IsMPC7XXX())
	{
		LoadDTLBEntry(addr, GetSRR1_WAY(), GetDCMP(), GetRPA());
	}
	else if(IsMPC7XXX())
	{
		LoadDTLBEntry(addr, addr & 1, GetPTEHI(), GetPTEHI());
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbli(address_t addr)
{
	throw IllegalInstructionException<CONFIG>();
	if(IsMPC7X5())
	{
		LoadITLBEntry(addr, GetSRR1_WAY(), GetICMP(), GetRPA());
	}
	else if(IsMPC7XXX())
	{
		LoadITLBEntry(addr, addr & 1, GetPTEHI(), GetPTEHI());
	}
}


/* Linked Load-Store */
template <class CONFIG>
void CPU<CONFIG>::Lwarx(unsigned int rd, address_t addr)
{
	Int32Load(rd, addr);

	reserve = true;
	reserve_addr = addr;
}

template <class CONFIG>
void CPU<CONFIG>::Stwcx(unsigned int rs, address_t addr)
{
	// TBD
	if(reserve)
	{
		if(reserve_addr == addr)
		{
			Int32Store(rs, addr);

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
	if(logger_import) {
		(*logger_import) << DebugInfo << File << __FILE__ << Function << __FUNCTION__ << Line << __LINE__
			<< "Received external interrupt"
			<< Endl << EndDebugInfo;
	}
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
void CPU<CONFIG>::Synchronize()
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

template <class CONFIG>
Instruction<CONFIG>::Instruction()
{
}

template <class CONFIG>
Instruction<CONFIG>::~Instruction()
{
}

inline uint32_t Max(uint32_t a, uint32_t b)
{
	return a > b ? a : b;
}

inline uint32_t Min(uint32_t a, uint32_t b)
{
	return a < b ? a : b;
}

template <class CONFIG>
void CPU<CONFIG>::Fetch()
{
	// Stall if IQ is full
	if(iq.Full()) return;
	// Send a request to IMMU
	uint32_t size_to_block_boundary = CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (cia & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	uint32_t fetch_width = Max(iq.Size(), FETCH_WIDTH);
	uint32_t fetch_size = Max(fetch_width * sizeof(uint32_t), size_to_block_boundary);
	uint8_t prefetch_buffer[fetch_size];
	uint32_t read_size = 0; //ReadInsnMemory(cia, prefetch_buffer, sizeof(prefetch_buffer) > size_to_block_boundary ? size_to_block_boundary : sizeof(prefetch_buffer));
	uint32_t offset;
	for(offset = 0; offset < read_size; offset += sizeof(uint32_t), cia += sizeof(uint32_t))
	{
		uint32_t insn = prefetch_buffer[offset];
		Operation<CONFIG> *operation = Decoder<CONFIG>::Decode(cia, insn);
		Instruction<CONFIG> *instruction = iw.Allocate();
		instruction->operation = operation;
		iq.Push(instruction);
	}
}

template <class CONFIG>
void CPU<CONFIG>::DecodeDispatch()
{
	unsigned int i;
	for(i = 0; i < DECODE_WIDTH; i++)
	{
		if(cq.Full()) return;

		Instruction<CONFIG> *instruction = iq.Front();

/*
		InsnType insn_type = operation->GetType();
		switch(insn_type)
		{
			case VEC_INSN:
				if(viq.Full()) return;

				
				viq.Push(operation);
				break;
			case GP_INSN:
				if(giq.Full()) return;
				giq.Push(operation);
				break;
			case FP_INSN:
				if(fiq.Full()) return;
				fiq.Push(operation);
				break;
		}
*/
		cq.Push(instruction);
		iq.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::GPRIssue()
{
}

template <class CONFIG>
void CPU<CONFIG>::FPRIssue()
{
}

template <class CONFIG>
void CPU<CONFIG>::VRIssue()
{
/*
	unsigned int i;
	for(i = 0; i < CONFIG::VR_ISSUE_WIDTH; i++)
	{
		Operation *operation = viq.Front();
		
		VecInsnType vec_insn_type = operation->GetVecInsnType();
		switch(vec_insn_type)
		{
			case VEC_PERM_INSN:
				break;
			case VEC_INT_INSN:
				break;
			case VEC_FP_INSN:
				break;
		}
	}
*/
}


} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
