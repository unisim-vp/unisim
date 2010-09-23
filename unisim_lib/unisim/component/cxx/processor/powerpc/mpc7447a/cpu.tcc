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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/isa/powerpc.tcc>
#include <unisim/util/simfloat/floating.tcc>
#include <unisim/component/cxx/processor/powerpc/mpc7447a/exception.tcc>
#include <unisim/component/cxx/cache/cache.tcc>
#include <unisim/component/cxx/tlb/tlb.tcc>
#include <unisim/util/queue/queue.tcc>

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

using namespace std;

template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
	: Object(name, parent, "PowerPC MPC7447A CPU")
	, unisim::component::cxx::processor::powerpc::isa::Decoder<CONFIG>()
	, Client<Loader<typename CONFIG::physical_address_t> >(name,  parent)
	, Client<SymbolTableLookup<typename CONFIG::address_t> >(name,  parent)
	, Client<DebugControl<typename CONFIG::address_t> >(name,  parent)
	, Client<MemoryAccessReporting<typename CONFIG::address_t> >(name,  parent)
	, Client<TrapReporting>(name,  parent)
	, Service<MemoryAccessReportingControl>(name,  parent)
	, Service<Disassembly<typename CONFIG::address_t> >(name,  parent)
	, Service<unisim::service::interfaces::Registers>(name,  parent)
	, Service<Memory<typename CONFIG::address_t> >(name,  parent)
	, Service<MemoryInjection<typename CONFIG::address_t> >(name,  parent)
	, Service<CPULinuxOS>(name,  parent)
	, Client<Memory<typename CONFIG::address_t> >(name,  parent)
	, Client<LinuxOS>(name,  parent)
	, Client<CachePowerEstimator>(name,  parent)
	, Client<PowerMode>(name,  parent)
	, Service<Synchronizable>(name,  parent)
	, disasm_export("disasm-export",  this)
	, registers_export("registers-export",  this)
	, memory_export("memory-export",  this)
	, memory_injection_export("memory-injection-export",  this)
	, cpu_linux_os_export("cpu-linux-os-export",  this)
	, synchronizable_export("synchronizable-export",  this)
	, memory_access_reporting_control_export("memory_access_reporting_control_export",  this)
	, kernel_loader_import("kernel-loader-import",  this)
	, debug_control_import("debug-control-import",  this)
	, memory_access_reporting_import("memory-access-reporting-import",  this)
	, symbol_table_lookup_import("symbol-table-lookup-import",  this)
	, memory_import("memory-import",  this)
	, linux_os_import("linux-os-import",  this)
	, trap_reporting_import("trap-reporting-import",  this)
	, il1_power_estimator_import("il1-power-estimator-import",  this)
	, dl1_power_estimator_import("dl1-power-estimator-import",  this)
	, l2_power_estimator_import("l2-power-estimator-import",  this)
	, itlb_power_estimator_import("itlb-power-estimator-import",  this)
	, dtlb_power_estimator_import("dtlb-power-estimator-import",  this)
	, il1_power_mode_import("il1-power-mode-import",  this)
	, dl1_power_mode_import("dl1-power-mode-import",  this)
	, l2_power_mode_import("l2-power-mode-import",  this)
	, dtlb_power_mode_import("dtlb-power-mode-import",  this)
	, itlb_power_mode_import("itlb-power-mode-import",  this)
	, logger(*this)
	, requires_memory_access_reporting(true)
	, requires_finished_instruction_reporting(true)
	, cpu_cycle_time(0)
	, voltage(0)
	, bus_cycle_time(0)
	, cpu_cycle(0)
	, bus_cycle(0)
	, effective_address(0)
	, verbose_all(false)
	, verbose_setup(false)
	, verbose_step(false)
	, verbose_dtlb(false)
	, verbose_dl1(false)
	, verbose_il1(false)
	, verbose_l2(false)
	, verbose_load(false)
	, verbose_store(false)
	, verbose_read_memory(false)
	, verbose_write_memory(false)
	, verbose_exception(false)
	, verbose_set_msr(false)
	, verbose_set_hid0(false)
	, verbose_set_hid1(false)
	, verbose_set_hid2(false)
	, verbose_set_l2cr(false)
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, max_inst(0xffffffffffffffffULL)
	, registers_registry()
	, instruction_counter(0)
	, fp32_estimate_inv_warning(false)
	, fp64_estimate_inv_sqrt_warning(false)
	//, formula_insn_per_bus_cycle("insn-per-bus-cycle", this, Formula<double>::OP_DIV, &stat_instruction_counter, &stat_bus_cycle)
	, il1()
	, num_il1_accesses(0)
	, num_il1_misses(0)
	, dl1()
	, num_dl1_accesses(0)
	, num_dl1_misses(0)
	, l2()
	, num_l2_accesses(0)
	, num_l2_misses(0)
	, dtlb()
	, itlb()
	, num_insn_in_prefetch_buffer(0)
	, cur_insn_in_prefetch_buffer(0)
	, prefetch_buffer()
	, reserve(false)
	, reserve_addr(0)
	, asynchronous_interrupt(0)
	, param_cpu_cycle_time("cpu-cycle-time",  this,  cpu_cycle_time, "CPU cycle time in picoseconds")
	, param_voltage("voltage",  this,  voltage, "CPU voltage in mV")
	, param_max_inst("max-inst",  this,  max_inst, "maximum number of instructions to simulate")
	, param_verbose_all("verbose-all",  this,  verbose_all, "globally enable/disable verbosity")
	, param_verbose_setup("verbose-setup",  this,  verbose_setup, "enable/disable verbosity while setup")
	, param_verbose_step("verbose-step",  this,  verbose_step, "enable/disable verbosity when simulating an instruction")
	, param_verbose_dtlb("verbose-dtlb",  this,  verbose_dtlb, "enable/disable verbosity when accessing data translation lookahead buffer")
	, param_verbose_dl1("verbose-dl1",  this,  verbose_dl1, "enable/disable verbosity when accessing L1 data cache")
	, param_verbose_il1("verbose-il1",  this,  verbose_il1, "enable/disable verbosity when accessing L1 instruction cache")
	, param_verbose_l2("verbose-l2",  this,  verbose_l2, "enable/disable verbosity when accessing L2 unified cache")
	, param_verbose_load("verbose-load",  this,  verbose_load, "enable/disable verbosity when simulating a load")
	, param_verbose_store("verbose-store",  this,  verbose_store, "enable/disable verbosity when simulating a store")
	, param_verbose_read_memory("verbose-read-memory",  this,  verbose_read_memory, "enable/disable verbosity when reading memory for a debug purpose")
	, param_verbose_write_memory("verbose-write-memory",  this,  verbose_write_memory, "enable/disable verbosity when writing memory for a debug purpose")
	, param_verbose_exception("verbose-exception",  this,  verbose_exception, "enable/disable verbosity when handling exceptions")
	, param_verbose_set_msr("verbose-set-msr",  this,  verbose_set_msr, "enable/disable verbosity when setting MSR")
	, param_verbose_set_hid0("verbose-set-hid0",  this,  verbose_set_hid0, "enable/disable verbosity when setting HID0")
	, param_verbose_set_hid1("verbose-set-hid1",  this,  verbose_set_hid1, "enable/disable verbosity when setting HID1")
	, param_verbose_set_hid2("verbose-set-hid2",  this,  verbose_set_hid2, "enable/disable verbosity when setting HID2")
	, param_verbose_set_l2cr("verbose-set-l2cr",  this,  verbose_set_l2cr, "enable/disable verbosity when setting L2CR")
	, param_trap_on_instruction_counter("trap-on-instruction-counter",  this,  trap_on_instruction_counter, "number of simulated instruction before traping")
//	, param_bus_cycle_time("bus-cycle-time",  this,  bus_cycle_time, "bus cycle time in picoseconds")
	, stat_instruction_counter("instruction-counter",  this,  instruction_counter, "number of simulated instructions")
	, stat_cpu_cycle("cpu-cycle",  this,  cpu_cycle, "number of simulated CPU cycles")
	, stat_bus_cycle("bus-cycle",  this,  bus_cycle, "number of simulated bus cycles")
#if 0
	, formula_insn_per_bus_cycle("insn-per-bus-cycle", this, unisim::kernel::service::Formula<double>::OP_DIV, &stat_instruction_counter, &stat_bus_cycle)
#endif
	, event_free_list(0)
	, insn_free_list(0)
	, operand_free_list(0)
	, bus_access_free_list(0)
	, load_store_access_free_list(0)
	, fetch_access_free_list(0)
	, num_outstanding_l1_fetch_miss(0)
	, num_outstanding_l1_load_miss(0)
	, num_outstanding_l1_store_miss(0)
	, num_outstanding_l2_load_miss(0)
	, num_outstanding_l2_store_miss(0)
	, dispatch_uop_num(0)
	, load_store_access_num(0)
{
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_cpu_cycle_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_voltage.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	//param_bus_cycle_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_bus_cycle.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_cpu_cycle.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	unsigned int i;

	for(i = 0; i < 32; i++)
	{
		stringstream sstr;
		sstr << "r" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &gpr[i]);
	}

	registers_registry["fpscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpscr", &fpscr);

	for(i = 0; i < 32; i++)
	{
		stringstream sstr;
		sstr << "f" << i;
		registers_registry[sstr.str()] = new FloatingPointRegisterInterface(sstr.str().c_str(), &fpr[i]);
	}

	registers_registry["cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("cr", &cr);
	registers_registry["xer"] = new unisim::util::debug::SimpleRegister<uint32_t>("xer", &xer);

	registers_registry["lr"] = new unisim::util::debug::SimpleRegister<uint32_t>("lr", &lr);
	registers_registry["ctr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ctr", &ctr);

	registers_registry["cia"] = new unisim::util::debug::SimpleRegister<uint32_t>("cia", &cia);

	registers_registry["msr"] = new unisim::util::debug::SimpleRegister<uint32_t>("msr", &msr);
	registers_registry["srr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr0", &srr0);
	registers_registry["srr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr1", &srr1);

	registers_registry["tbl"] = new unisim::util::debug::SimpleRegister<uint32_t>("tbl", &tbl);
	registers_registry["tbu"] = new unisim::util::debug::SimpleRegister<uint32_t>("tbu", &tbu);

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

	registers_registry["dar"] = new unisim::util::debug::SimpleRegister<uint32_t>("dar", &dar);
	registers_registry["dsisr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dsisr", &dsisr);
	registers_registry["ear"] = new unisim::util::debug::SimpleRegister<uint32_t>("ear", &ear);
	registers_registry["dec"] = new unisim::util::debug::SimpleRegister<uint32_t>("dec", &dec);
	registers_registry["dabr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dabr", &dabr);
	registers_registry["pvr"] = new unisim::util::debug::SimpleRegister<uint32_t>("pvr", &pvr);

	registers_registry["hid0"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid0", &hid0);
	registers_registry["hid1"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid1", &hid1);
	registers_registry["iabr"] = new unisim::util::debug::SimpleRegister<uint32_t>("iabr", &iabr);
	registers_registry["ldstdb"] = new unisim::util::debug::SimpleRegister<uint32_t>("ldstdb", &ldstdb);
	registers_registry["ictc"] = new unisim::util::debug::SimpleRegister<uint32_t>("ictc", &ictc);
	registers_registry["pmc1"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc1", &pmc1);
	registers_registry["pmc2"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc2", &pmc1);
	registers_registry["pmc3"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc3", &pmc1);
	registers_registry["pmc4"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc4", &pmc1);
	registers_registry["sia"] = new unisim::util::debug::SimpleRegister<uint32_t>("sia", &sia);
	registers_registry["sda"] = new unisim::util::debug::SimpleRegister<uint32_t>("sda", &sda);
	registers_registry["mmcr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmmcr0", &mmcr0);
	registers_registry["mmcr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmcr1", &mmcr1);
	registers_registry["l2cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("l2cr", &l2cr);
	registers_registry["tlbmiss"] = new unisim::util::debug::SimpleRegister<uint32_t>("tlbmiss", &tlbmiss);

	registers_registry["vscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("vscr", &vscr);
	registers_registry["vrsave"] = new unisim::util::debug::SimpleRegister<uint32_t>("vrsave", &vrsave);

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

	for(i = 0; i < CONFIG::NUM_SRS; i++)
	{
		stringstream sstr;
		sstr << "sr" << i;
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sr[i]);
	}

	registers_registry["sdr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("sdr1", &sdr1);

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
bool CPU<CONFIG>::FloatingPointSelfTest()
{
	// Compute 1.0/sqrt(3.0)
	SoftDouble b(0x4008000000000000ULL); // 3.0
	
	Flags flags;
	flags.setRoundingMode(RN_NEAREST);
	// First estimation of 1/sqrt(b), seed of Newton-Raphson algorithm 
	// see http://www.mceniry.net/papers/Fast%20Inverse%20Square%20Root.pdf
	SoftDouble u((0xbfcdd6a18f6a6f55ULL - b.queryValue()) >> 1);

	// Newton-Raphson method
	SoftDouble half(0x3fe0000000000000ULL); // 0.5
	SoftDouble three(0x4008000000000000ULL); // 3.0

	unsigned int i;
	for(i = 0; i < 5; i++)
	{
		// Newton-Raphson iteration
		// w = b * u * u
		SoftDouble w(b);
		w.multAssign(u, flags);
		w.multAssign(u, flags);
		// v = 3.0 - w
		SoftDouble v(three);
		v.minusAssign(w, flags);
		// u = 0.5 * u * v
		u.multAssign(half, flags);
		u.multAssign(v, flags);
	}
	
	const uint64_t reciprocal_sqrt_3 = 0x3fe279a74590331cULL;
	
	return u.queryValue() == reciprocal_sqrt_3;
}

template <class CONFIG>
bool CPU<CONFIG>::Setup()
{
	unsigned int min_cycle_time = 0;

	if(CONFIG::ITLB_CONFIG::ENABLE && itlb_power_mode_import)
	{
		unsigned int itlb_min_cycle_time = itlb_power_mode_import->GetMinCycleTime();
		if(itlb_min_cycle_time > 0 && itlb_min_cycle_time > min_cycle_time) min_cycle_time = itlb_min_cycle_time;
		unsigned int itlb_default_voltage = itlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = itlb_default_voltage;
	}

	if(CONFIG::DTLB_CONFIG::ENABLE && dtlb_power_mode_import)
	{
		unsigned int dtlb_min_cycle_time = dtlb_power_mode_import->GetMinCycleTime();
		if(dtlb_min_cycle_time > 0 && dtlb_min_cycle_time > min_cycle_time) min_cycle_time = dtlb_min_cycle_time;
		unsigned int dtlb_default_voltage = dtlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = dtlb_default_voltage;
	}
	
	if(CONFIG::IL1_CONFIG::ENABLE && il1_power_mode_import)
	{
		unsigned int il1_min_cycle_time = il1_power_mode_import->GetMinCycleTime();
		if(il1_min_cycle_time > 0 && il1_min_cycle_time > min_cycle_time) min_cycle_time = il1_min_cycle_time;
		unsigned int il1_default_voltage = il1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = il1_default_voltage;
	}
	
	if(CONFIG::DL1_CONFIG::ENABLE && dl1_power_mode_import)
	{
		unsigned int dl1_min_cycle_time = dl1_power_mode_import->GetMinCycleTime();
		if(dl1_min_cycle_time > 0 && dl1_min_cycle_time > min_cycle_time) min_cycle_time = dl1_min_cycle_time;
		unsigned int dl1_default_voltage = dl1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = dl1_default_voltage;
	}
	
	if(CONFIG::L2_CONFIG::ENABLE && l2_power_mode_import)
	{
		unsigned int l2_min_cycle_time = l2_power_mode_import->GetMinCycleTime();
		if(l2_min_cycle_time > 0 && l2_min_cycle_time > min_cycle_time) min_cycle_time = l2_min_cycle_time;
		unsigned int l2_default_voltage = l2_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = l2_default_voltage;
	}
	
	if(min_cycle_time > 0)
	{
		if(cpu_cycle_time > 0)
		{
			if(cpu_cycle_time < min_cycle_time)
			{
				if(unlikely(IsVerboseSetup()))
				{
					logger << DebugWarning;
					logger << "A cycle time of " << cpu_cycle_time << " ps is too low for the simulated hardware !" << endl;
					logger << "cpu cycle time should be >= " << min_cycle_time << " ps." << endl;
					logger << EndDebugWarning;
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
			logger << DebugError;
			logger << "user must provide a cpu cycle time > 0" << endl;
			logger << EndDebugError;
			return false;
		}
	}

	if(unlikely(IsVerboseSetup()))
	{
		logger << DebugInfo << "cpu cycle time of " << cpu_cycle_time << " ps" << endl << EndDebugInfo;
	}

	if(voltage <= 0)
	{
		logger << DebugError << "user must provide a voltage > 0" << endl << EndDebugError;
		return false;
	}

	if(unlikely(IsVerboseSetup()))
	{
		logger << DebugInfo;
		
		logger << "voltage of " << ((double) voltage / 1e3) << " V" << endl;

/*		if(bus_cycle_time > 0)
			logger << "bus cycle time of " << bus_cycle_time << " ps" << endl;*/
		
		logger << EndDebugInfo;
		
/*		if(bus_cycle_time <= 0)
		{
			logger << DebugError;
			logger << "bus cycle time must be > 0" << endl;
			logger << EndDebugError;
			return false;
		}*/
	}

	if(il1_power_mode_import)
		il1_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	
	if(dl1_power_mode_import)
		dl1_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	
	if(l2_power_mode_import)
		l2_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	
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
	
	if(!FloatingPointSelfTest())
	{
		logger << DebugError;
		logger << "Floating-point self test failed !" << endl;
		logger << EndDebugError;
		return false;
	}

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
	unsigned int i;

	bus_cycle = 0;
	cpu_cycle = 0;
	instruction_counter = 0;

	asynchronous_interrupt = 0;

	reserve = false;
	reserve_addr = 0;

	for(i = 0; i < CONFIG::NUM_GPRS; i++) gpr[i] = 0;

	fpscr = 0;
	for(i = 0; i < CONFIG::NUM_FPRS; i++) fpr[i] = SoftDouble(0);

	for(i = 0; i < CONFIG::NUM_VRS; i++)
	{
		memset(&vr[i], 0, sizeof(vr[i]));
	}
	vrsave = 0;
	vscr = 0;

	cr = 0;
	xer = 0;
	lr = 0;
	ctr = 0;

	cia = CONFIG::START_ADDR;
	nia = CONFIG::START_ADDR;
	
	msr = CONFIG::MSR_RESET_VALUE;
	
	pvr = CONFIG::PROCESSOR_VERSION;
	
	srr0 = 0;
	srr1 = 0;
	tbl = 0;
	tbu = 0;

	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
	{
		sprg[i] = 0;
	}

	bamr = 0;
	dar = 0;
	dsisr = 0;
	ear = 0;
	dec = 0;
	SetDABR(0);

	CPU<CONFIG>::InvalidateDecodingCache();
	
	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		dbatu[i] = 0;
		dbatl[i] = 0;
		ibatu[i] = 0;
		ibatl[i] = 0;
	}
	for(i = 0; i < 16; i++)
	{
		sr[i] = 0;
	}
	sdr1 = 0;
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
	
	num_insn_in_prefetch_buffer = 0;
	cur_insn_in_prefetch_buffer = 0;

	asynchronous_interrupt = 0;
	
	hid0 = CONFIG::HID0_RESET_VALUE;
	hid1 = CONFIG::HID1_RESET_VALUE;

	iabr = 0;
	dabr = 0;
	ldstcr = 0;
	ldstdb = 0;
	ictc = 0;
	ictrl = 0;
	pir = 0;
	ptehi = 0;
	ptelo = 0;
	tlbmiss = 0;
	pmc1 = 0;
	pmc2 = 0;
	pmc3 = 0;
	pmc4 = 0;
	pmc5 = 0;
	pmc6 = 0;
	sia = 0;
	sda = 0;
	mmcr0 = 0;
	mmcr1 = 0;
	mmcr2 = 0;
	
	l2cr = 0;

	effective_address = 0;
}

template <class CONFIG>
uint32_t CPU<CONFIG>::GetSPR(unsigned int n)
{
	switch(n)
	{
		case 1: return GetXER();
		case 8: return GetLR();
		case 9: return GetCTR();
		case 18:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDSISR();
		case 19:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDAR();
		case 22:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDEC();
		case 25:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetSDR1();
		case 26:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetSRR0();
		case 27:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetSRR1();
		case 256:
			return GetVRSAVE();
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
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetEAR();
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
			return GetMMCR2();
		case 929:
			return GetPMC5();
		case 930:
			return GetPMC6();
		case 936:
			return GetMMCR0();
		case 937:
			return GetPMC1();
		case 938:
			return GetPMC2();
		case 940:
			return GetMMCR1();
		case 941:
			return GetPMC3();
		case 942:
			return GetPMC4();
		case 944:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMMCR2();
		case 945:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPMC5();
		case 946:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPMC6();
		case 951:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetBAMR();
		case 952:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMMCR0();
		case 953:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPMC1();
		case 954:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPMC2();
		case 955:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetSIA();
		case 956:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMMCR1();
		case 957:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPMC3();
		case 958:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPMC4();
		case 980:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetTLBMISS();
		case 981:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPTEHI();
		case 982:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPTELO();
		case 1008:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetHID0();
		case 1009:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetHID1();
		case 1010:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetIABR();
		case 1011:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetICTRL();
		case 1012: // undocumented !!!
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetLDSTDB();
		case 1013:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDABR();
		case 1014:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMSSCR0();
		case 1015:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMSSSR0();
		case 1016:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetLDSTCR();
		case 1017:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetL2CR();
		case 1019:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetICTC();
		case 1023:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPIR();
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
		case 1: SetXER(value); return;
		case 8: SetLR(value); return;
		case 9: SetCTR(value); return;
		case 18:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDSISR(value);
			return;
		case 19:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDAR(value);
			return;
		case 22:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDEC(value);
			return;
		case 25:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSDR1(value);
			return;
		case 26:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSRR0(value);
			return;
		case 27:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSRR1(value);
			return;
		case 256:
			SetVRSAVE(value);
			return;
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
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetEAR(value);
			return;
		case 284:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetTBL(value);
			return;
		case 285:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetTBU(value);
			return;
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
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMMCR2(value);
			return;
		case 945:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPMC5(value);
			return;
		case 946:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPMC6(value);
			return;
		case 951:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetBAMR(value);
			return;
		case 952:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMMCR0(value);
			return;
		case 953:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPMC1(value);
			return;
		case 954:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPMC2(value);
			return;
		case 955:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSIA(value);
			return;
		case 956:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMMCR1(value);
			return;
		case 957:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPMC3(value);
			return;
		case 958:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPMC4(value);
			return;
		case 959:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSDA(value);
			return;
		case 980:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetTLBMISS(value);
			return;
		case 981:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPTEHI(value);
			return;
		case 982:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPTELO(value);
			return;
		case 1008:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetHID0(value);
			return;
		case 1009:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetHID1(value);
			return;
		case 1010:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetIABR(value);
			return;
		case 1011:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetICTRL(value);
			return;
		case 1012: // undocumented !!!
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetLDSTDB(value);
			return;
		case 1013:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDABR(value);
			return;
		case 1014:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMSSCR0(value);
			return;
		case 1015:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMSSSR0(value);
			return;
		case 1016:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetLDSTCR(value);
			return;
		case 1017:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetL2CR(value);
			return;
		case 1019:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetICTC(value);
			return;
		case 1023:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPIR(value);
			return;
	}
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::StepOneInstruction()
{
	if(unlikely(debug_control_import != 0))
	{
		do
		{
			typename DebugControl<typename CONFIG::address_t>::DebugCommand dbg_cmd;

			dbg_cmd = debug_control_import->FetchDebugCommand(GetCIA());
	
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_SYNC)
			{
				Synchronize();
				continue;
			}

			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_KILL) Stop(0);
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_RESET)
			{
				if(kernel_loader_import)
				{
					kernel_loader_import->Reset();
				}
			}
		} while(1);
	}

	typename CONFIG::address_t sequential_nia = GetCIA() + 4;
	unisim::component::cxx::processor::powerpc::isa::Operation<CONFIG> *operation = 0;

	SetNIA(sequential_nia);

	try
	{
		typename CONFIG::address_t addr = GetCIA();
		uint32_t insn;

#ifdef SOCLIB
		if(unlikely(cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer))
		{
			std::cerr << "No instructions in prefetch buffer" << std::endl;
			return;
		}
		insn = prefetch_buffer[cur_insn_in_prefetch_buffer];
#else
		if(CONFIG::PREFETCH_BUFFER_ENABLE)
		{
			if(unlikely(cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer))
			{
				uint32_t size_to_block_boundary = IsInsnCacheEnabled() ? CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (addr & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1)) : CONFIG::FSB_WIDTH - (addr & (CONFIG::FSB_WIDTH - 1));
				uint32_t size_to_prefetch = size_to_block_boundary > (4 * CONFIG::NUM_PREFETCH_BUFFER_ENTRIES) ? CONFIG::NUM_PREFETCH_BUFFER_ENTRIES / 4 : size_to_block_boundary;
				// refill the prefetch buffer with up to one cache line, not much
				EmuFetch(addr, prefetch_buffer, size_to_prefetch);
				num_insn_in_prefetch_buffer = size_to_prefetch / 4;
				cur_insn_in_prefetch_buffer = 0;
			}
			insn = prefetch_buffer[cur_insn_in_prefetch_buffer];
		}
		else
		{
			EmuFetch(addr, &insn, 4);
		}
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(insn);
#endif

		if(unlikely(requires_memory_access_reporting)) 
		{
			if(unlikely(memory_access_reporting_import != 0))
			{
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_READ, MemoryAccessReporting<typename CONFIG::address_t>::MT_INSN, addr, 4);
			}
		}
		if(CONFIG::IABR_ENABLE)
		{
			/* Check for instruction address breakpoint */
			if(unlikely(GetIABR_BE() && GetIABR_TE() == GetMSR_IR() && ((GetCIA() >> 2) & 0x3fffffffUL) == GetIABR_ADDR()))
			{
				throw InstructionAddressBreakpointException<CONFIG>();
			}
		}

		operation = unisim::component::cxx::processor::powerpc::isa::Decoder<CONFIG>::Decode(addr, insn);

//		stringstream sstr;
//		operation->disasm((CPU<CONFIG> *) this, sstr);
//		std::cerr << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << std::endl;

		if(unlikely(IsVerboseStep()))
		{
			stringstream sstr;
			operation->disasm((CPU<CONFIG> *) this, sstr);
			logger << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << endl << EndDebugInfo;
		}

		/* execute the instruction */
		operation->execute(this);

		if(unlikely(HasAsynchronousInterrupt()))
		{
			if(HasHardReset()) throw SystemResetException<CONFIG>();
			if(HasMCP() && GetHID1_EMCP()) throw MachineCheckException<CONFIG>();
			if(HasTEA()) throw MachineCheckException<CONFIG>();
			if(HasSoftReset()) throw SystemResetException<CONFIG>();
			if(HasSMI()) throw SystemManagementInterruptException<CONFIG>();
			if(HasExternalInterrupt() && GetMSR_EE()) throw ExternalInterruptException<CONFIG>();
			if(HasPerformanceMonitorInterrupt()) throw PerformanceMonitorInterruptException<CONFIG>();
			if(HasDecrementerOverflow() && GetMSR_EE()) throw DecrementerException<CONFIG>();
		}
	}
	catch(MachineCheckException<CONFIG>& exc) { HandleException(exc); }
	catch(DecrementerException<CONFIG>& exc) { HandleException(exc); }
	catch(ExternalInterruptException<CONFIG>& exc) { HandleException(exc); }
	catch(SystemResetException<CONFIG>& exc) { HandleException(exc); }
	catch(PerformanceMonitorInterruptException<CONFIG>& exc) { HandleException(exc); }
	catch(SystemManagementInterruptException<CONFIG>& exc) { HandleException(exc); }
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
		if(unlikely(IsVerboseStep()))
			logger << DebugError << "uncaught processor exception :" << e.what() << endl << EndDebugError;
		Stop(1);
	}

	/* go to the next instruction */
	if(CONFIG::PREFETCH_BUFFER_ENABLE)
	{
		if(unlikely(GetNIA() != sequential_nia))
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

	if(unlikely(trap_reporting_import && instruction_counter == trap_on_instruction_counter))
	{
		trap_reporting_import->ReportTrap();
	}
	
	if(unlikely(requires_finished_instruction_reporting))
	{
		if(unlikely(memory_access_reporting_import != 0))
		{
			memory_access_reporting_import->ReportFinishedInstruction(GetNIA());
		}
	}

	if(unlikely(instruction_counter >= max_inst)) Stop(0);
}

template <class CONFIG>
void CPU<CONFIG>::StepOneCycle()
{
	DumpPipeline();
	DumpSchedule();
	//logger << DebugInfo << "Cycle #" << cpu_cycle << EndDebugInfo;
	ScheduleEvents();

	// Complete
	Complete();
	// Execute
	IU1Execute();
	IU2Execute();
	FPUExecute();
	BIU();
	LSUExecute2();
	LSUExecute1();
	// Issue
	GPRIssue();
	VRIssue();
	FPRIssue();
	// Decode/Dispatch
	DecodeDispatch();
	// Fetch
	Fetch();
	cpu_cycle++;
}

template <class CONFIG>
void CPU<CONFIG>::OnBusCycle()
{
	/* update the bus cycle counter */
	bus_cycle++;
	
	// Time Base registers and DEC register are updated at one fourth the bus clock rate
	if(unlikely((bus_cycle % 4) == 0))
	{
		/* Update Time base counters */
		SetTBU((bus_cycle >> 34) & 0xffffffffUL);
		SetTBL((bus_cycle >> 2) & 0xffffffffUL);
		
		/* std::decrement the decrementer */
		SetDEC(GetDEC() - 1);
	}
}

template <class CONFIG>
void CPU<CONFIG>::FlushSubsequentInstructions()
{
	cur_insn_in_prefetch_buffer = num_insn_in_prefetch_buffer - 1;
}

template <class CONFIG>
void CPU<CONFIG>::FillPrefetchBuffer(uint32_t insn)
{
	prefetch_buffer[0] = insn;
	num_insn_in_prefetch_buffer = 1;
	cur_insn_in_prefetch_buffer = 0;
}

template <class CONFIG>
bool CPU<CONFIG>::NeedFillingPrefetchBuffer() const
{
	return cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer;
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
	do
	{
		StepOneCycle();
		if(instruction_counter >= max_inst) Stop(0);
	} while(1);
}

template <class CONFIG>
void CPU<CONFIG>::SetHID0(uint32_t value)
{
	if(unlikely(IsVerboseSetHID0()))
		logger << DebugInfo << "Writing 0x" << std::hex << value << std::dec << " into HID0" << endl << EndDebugInfo;
	uint32_t old_hid0_ice = GetHID0_ICE();
	uint32_t old_hid0_dce = GetHID0_DCE();
	uint32_t old_hid0_nap = GetHID0_NAP();

	hid0 = (hid0 & ~CONFIG::HID0_MASK) | (value & CONFIG::HID0_MASK);

	if(CONFIG::IL1_CONFIG::ENABLE)
	{
		if(!old_hid0_ice && GetHID0_ICE())
		{
			if(unlikely(IsVerboseSetHID0()))
				logger << DebugInfo << "Enabling L1 Insn Cache" << endl << EndDebugInfo;
		}
		
		if(old_hid0_ice && !GetHID0_ICE())
		{
			if(unlikely(IsVerboseSetHID0()))
				logger << DebugInfo << "Disabling L1 Insn Cache" << endl << EndDebugInfo;
		}
	}
	
	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		if(!old_hid0_dce && GetHID0_DCE())
		{
			if(unlikely(IsVerboseSetHID0()))
				logger << DebugInfo << "Enabling L1 Data Cache" << endl << EndDebugInfo;
		}
		
		if(old_hid0_dce && !GetHID0_DCE())
		{
			if(unlikely(IsVerboseSetHID0()))
				logger << DebugInfo << "Disabling L1 Data Cache" << endl << EndDebugInfo;
		}
	}
	
	if(GetHID0_ICFI())
	{
		if(CONFIG::IL1_CONFIG::ENABLE)
		{
			if(unlikely(IsVerboseSetHID0()))
				logger << DebugInfo << "Flash Invalidating L1 Insn Cache" << endl << EndDebugInfo;
			InvalidateIL1();
		}
		unisim::component::cxx::processor::powerpc::isa::Decoder<CONFIG>::InvalidateDecodingCache();
		ResetHID0_ICFI();
	}

	if(GetHID0_DCFI())
	{
		if(CONFIG::DL1_CONFIG::ENABLE)
		{
			if(unlikely(IsVerboseSetHID0()))
				logger << DebugInfo << "Flash Invalidating L1 Data Cache" << endl << EndDebugInfo;
			InvalidateDL1();
		}
		ResetHID0_DCFI();
	}

	if(!old_hid0_nap && GetHID0_NAP())
	{
		if(unlikely(IsVerboseSetHID0()))
			logger << DebugInfo << "Enabling NAP mode" << endl << EndDebugInfo;
		if(GetMSR_POW())
		{
			if(unlikely(IsVerboseSetHID0()))
			{
				logger << DebugInfo << "Entering NAP mode" << endl << EndDebugInfo;
			}
			if(GetMSR_EE())
			{
				Idle();
			}
			//CPU<CONFIG>::ResetMSR_POW();
		}
	}

	if(old_hid0_nap && !GetHID0_NAP())
	{
		if(unlikely(IsVerboseSetHID0()))
			logger << DebugInfo << "Disabling NAP mode" << endl << EndDebugInfo;
		if(GetMSR_POW())
		{
			if(unlikely(IsVerboseSetHID0()))
			{
				logger << DebugInfo << "Leaving NAP mode" << endl << EndDebugInfo;
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetHID1(uint32_t value)
{
	uint32_t old_hid1_abe = GetHID1_ABE();

	hid1 = (hid1 & ~CONFIG::HID1_MASK) | (value & CONFIG::HID1_MASK);

	if(!old_hid1_abe && GetHID1_ABE())
	{
		if(unlikely(IsVerboseSetHID1()))
			logger << DebugInfo << "Enabling Address Only broadcast" << endl << EndDebugInfo;
	}

	if(old_hid1_abe && !GetHID1_ABE())
	{
		if(unlikely(IsVerboseSetHID1()))
			logger << DebugInfo << "Disabling Address Only broadcast" << endl << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetSupervisorPrivilegeLevel()
{
	ResetMSR_PR();
}

template <class CONFIG>
void CPU<CONFIG>::SetUserPrivilegeLevel()
{
	SetMSR_PR();
}

template <class CONFIG>
void CPU<CONFIG>::EnableFPU()
{
	SetMSR_FP();   // enable floating point unit
}

template <class CONFIG>
void CPU<CONFIG>::DisableFPU()
{
	ResetMSR_FP();   // disable floating point unit
}

template <class CONFIG>
void CPU<CONFIG>::EnableDataCache()
{
	SetHID0_DCE();
}

template <class CONFIG>
void CPU<CONFIG>::DisableDataCache()
{
	ResetHID0_DCE();
}

template <class CONFIG>
void CPU<CONFIG>::EnableInsnCache()
{
	SetHID0_ICE();
}

template <class CONFIG>
void CPU<CONFIG>::DisableInsnCache()
{
	SetHID0_ICE();
}

template <class CONFIG>
void CPU<CONFIG>::EnableL2Cache()
{
	SetL2CR_L2E();
}

template <class CONFIG>
void CPU<CONFIG>::DisableL2Cache()
{
	ResetL2CR_L2E();
}

template <class CONFIG>
void CPU<CONFIG>::EnableAddressBroadcast()
{
	// enable address only broadcast
	SetHID1_ABE();
}

template <class CONFIG>
void CPU<CONFIG>::DisableAddressBroadcast()
{
	// enable address only broadcast
	ResetHID1_ABE();
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsFPUEnabled()
{
	return GetMSR_FP() != 0;
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsFPUExceptionEnabled()
{
	return GetMSR_FE0() || GetMSR_FE1();
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsDataCacheEnabled()
{
	return CONFIG::DL1_CONFIG::ENABLE && GetHID0_DCE();
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsInsnCacheEnabled()
{
	return CONFIG::IL1_CONFIG::ENABLE && GetHID0_ICE();
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsL2CacheEnabled()
{
	return CONFIG::L2_CONFIG::ENABLE && GetL2CR_L2E();
}


template <class CONFIG>
inline bool CPU<CONFIG>::IsAddressBroadcastEnabled()
{
	return GetHID1_ABE() != 0;
}

template <class CONFIG>
void CPU<CONFIG>::SetL2CR(uint32_t value)
{
	uint32_t old_l2cr_l2e = GetL2CR_L2E();

	l2cr = value;

	if(CONFIG::L2_CONFIG::ENABLE)
	{
		if(!old_l2cr_l2e && GetL2CR_L2E())
		{
			if(unlikely(IsVerboseSetL2CR()))
				logger << DebugInfo << "Enabling L2 Cache" << endl << EndDebugInfo;
		}
		
		if(old_l2cr_l2e && !GetL2CR_L2E())
		{
			if(unlikely(IsVerboseSetL2CR()))
				logger << DebugInfo << "Disabling L2 Cache" << endl << EndDebugInfo;
		}
	}

	if(GetL2CR_L2I())
	{
		if(old_l2cr_l2e)
		{
			if(unlikely(IsVerboseSetL2CR()))
				logger << DebugWarning << "L2 Cache should be disabled prior a flash invalidation" << endl << EndDebugWarning;
		}
		
		if(unlikely(IsVerboseSetL2CR()))
			logger << DebugInfo << "Flash Invalidating L2 Cache" << endl << EndDebugInfo;
		unisim::component::cxx::processor::powerpc::isa::Decoder<CONFIG>::InvalidateDecodingCache();
		if(CONFIG::L2_CONFIG::ENABLE)
		{
			InvalidateL2();
		}
		ResetL2CR_L2I();
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
	uint32_t old_msr_fe = GetMSR_FE0() | GetMSR_FE1();
	uint32_t old_msr_pow = GetMSR_POW();

	msr = (msr & ~CONFIG::MSR_MASK) | (value & CONFIG::MSR_MASK);

	if(!old_msr_ir && GetMSR_IR())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling IMMU" << endl << EndDebugInfo;
	}

	if(old_msr_ir && !GetMSR_IR())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling IMMU" << endl << EndDebugInfo;
	}

	if(!old_msr_dr && GetMSR_DR())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling DMMU" << endl << EndDebugInfo;
	}
	
	if(old_msr_dr && !GetMSR_DR())	
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling DMMU" << endl << EndDebugInfo;
	}

	if(!old_msr_fp && GetMSR_FP())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling FPU" << endl << EndDebugInfo;
	}

	if(old_msr_fp && !GetMSR_FP())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling FPU" << endl << EndDebugInfo;
	}

	uint32_t msr_fe = GetMSR_FE0() | GetMSR_FE1();
	if(!old_msr_fe && msr_fe)
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling FPU exception" << endl << EndDebugInfo;
	}

	if(old_msr_fe && !msr_fe)
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling FPU exception" << endl << EndDebugInfo;
	}

	if(!old_msr_pr && GetMSR_PR())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching to user privilege level" << endl << EndDebugInfo;
	}

	if(old_msr_pr && !GetMSR_PR())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching to supervisor privilege level" << endl << EndDebugInfo;
	}

	if(!old_msr_le && GetMSR_LE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching to little endian" << endl << EndDebugInfo;
	}

	if(old_msr_le && !GetMSR_LE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching to big endian" << endl << EndDebugInfo;
	}
	
	if(!old_msr_pow && GetMSR_POW())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling power management" << endl << EndDebugInfo;
		if(GetHID0_NAP())
		{
			if(unlikely(IsVerboseSetMSR()))
			{
				logger << DebugInfo << "Entering NAP mode" << endl << EndDebugInfo;
			}
			if(GetMSR_EE())
			{
				Idle();
			}
			//CPU<CONFIG>::ResetMSR_POW();
		}
	}

	if(old_msr_pow && !GetMSR_POW())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling power management" << endl << EndDebugInfo;

		if(GetHID0_NAP())
		{
			if(unlikely(IsVerboseSetMSR()))
			{
				logger << DebugInfo << "Leaving NAP mode" << endl << EndDebugInfo;
			}
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
string CPU<CONFIG>::GetObjectFriendlyName(typename CONFIG::address_t addr)
{
	stringstream sstr;
	
	const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template <class CONFIG>
string CPU<CONFIG>::GetFunctionFriendlyName(typename CONFIG::address_t addr)
{
	stringstream sstr;
	
	const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
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

	return 0;
}

template <class CONFIG>
const char *CPU<CONFIG>::GetArchitectureName() const
{
	return "powerpc";
}

template <class CONFIG>
void CPU<CONFIG>::BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg, bool rwitm)
{
	memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
void CPU<CONFIG>::BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg)
{
	memory_import->WriteMemory(physical_addr, buffer, size);
}

template <class CONFIG>
void CPU<CONFIG>::BusZeroBlock(typename CONFIG::physical_address_t physical_addr)
{
	uint8_t zero[32];
	memset(zero, 0, sizeof(zero));
	memory_import->WriteMemory(physical_addr & (~31), zero, sizeof(zero));
}

template <class CONFIG>
void CPU<CONFIG>::BusFlushBlock(typename CONFIG::physical_address_t physical_addr)
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
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "Invalidating DL1" << endl << EndDebugInfo;
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
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "Invalidating IL1" << endl << EndDebugInfo;
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
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "Invalidating L2" << endl << EndDebugInfo;
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
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1 Lookup at 0x";
		logger << std::hex << addr << std::dec << " : ";
		logger << "line_base_addr=0x" << std::hex << line_base_addr << std::dec << ",";
		logger << "index=0x" << std::hex << index << std::dec << ",";
		logger << "sector=0x" << std::hex << sector << std::dec << ",";
		logger << "offset=0x" << std::hex << offset << std::dec << ",";
		logger << "size_to_block_boundary=0x" << std::hex << size_to_block_boundary << std::dec;
		logger << endl << EndDebugInfo;
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
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 Line hit: way=" << way << endl << EndDebugInfo;
			}
			// line hit: block may need a fill if not yet present in the line
			l1_access.line = line;
			l1_access.line_to_evict = 0;
			l1_access.way = way;
			block = &(*line)[sector];
			l1_access.block = block->status.valid ? block : 0;
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
			}
			return;
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
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
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1 Lookup at 0x";
		logger << std::hex << addr << std::dec << " : ";
		logger << "line_base_addr=0x" << std::hex << line_base_addr << std::dec << ",";
		logger << "index=0x" << std::hex << index << std::dec << ",";
		logger << "sector=0x" << std::hex << sector << std::dec << ",";
		logger << "offset=0x" << std::hex << offset << std::dec << ",";
		logger << "size_to_block_boundary=0x" << std::hex << size_to_block_boundary << std::dec;
		logger << endl << EndDebugInfo;
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
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1 Line hit: way=" << way << endl << EndDebugInfo;
			}
			// line hit: block may need a fill if not yet present in the line
			l1_access.line = line;
			l1_access.line_to_evict = 0;
			l1_access.way = way;
			block = &(*line)[sector];
			l1_access.block = block->status.valid ? block : 0;
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
			}
			return;
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
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
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2 Lookup at 0x";
		logger << std::hex << addr << std::dec << " : ";
		logger << "line_base_addr=0x" << std::hex << line_base_addr << std::dec << ",";
		logger << "index=0x" << std::hex << index << std::dec << ",";
		logger << "sector=0x" << std::hex << sector << std::dec << ",";
		logger << "offset=0x" << std::hex << offset << std::dec << ",";
		logger << "size_to_block_boundary=0x" << std::hex << size_to_block_boundary << std::dec;
		logger << endl << EndDebugInfo;
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
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2 Line hit: way=" << way << endl << EndDebugInfo;
			}
			// line hit: block may need a fill if not yet present in the line
			l2_access.line = line;
			l2_access.line_to_evict = 0;
			l2_access.way = way;
			block = &(*line)[sector];
			l2_access.block = block->status.valid ? block : 0;
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
			}
			return;
		}
	}

	// line miss
	l2_access.line = 0;
	l2_access.block = 0;
	num_l2_misses++;
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1: evicting line at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
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
					// DL1 block eviction into L2
					CacheAccess<class CONFIG::L2_CONFIG> l2_access;

					l2_access.addr = l1_block_to_evict.GetBaseAddr();
					LookupL2(l2_access);

					if(l2_access.block)
					{
						if(unlikely(IsVerboseDL1()))
						{
							logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into L2" << endl << EndDebugInfo;
						}
						memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_evict[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
						l2_access.block->status.dirty = true;
	
						UpdateReplacementPolicyL2(l2_access);
						
						if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportWriteAccess();
					}
					else
					{
						if(unlikely(IsVerboseDL1()))
						{
							logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
						}
						// dirty DL1 block eviction into memory
						// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
						BusWrite(l1_block_to_evict.GetBaseAddr(), &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
					}
				}
				else
				{
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
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
inline void CPU<CONFIG>::EmuEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1: evicting line at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
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
void CPU<CONFIG>::EmuEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2: evicting line at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
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
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2: evicting dirty block at 0x" << std::hex << l2_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
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

	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1 PLRU bits:";
		for(i = 0; i < CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			logger << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		logger << endl << EndDebugInfo;
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

	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1 PLRU bits:";
		for(i = 0; i < CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			logger << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		logger << endl << EndDebugInfo;
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

	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2 PLRU bits:";
		for(i = 0; i < CONFIG::L2_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			logger << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		logger << endl << EndDebugInfo;
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
inline void CPU<CONFIG>::EmuFillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg, bool rwitm)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(unlikely(unlikely(IsVerboseDL1())))
	{
		logger << DebugInfo << "DL1: filling block at 0x" << std::hex << l1_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}
	// DL1 block fill
	if(likely(IsL2CacheEnabled()))
	{
		// DL1 block fill from L2
		CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
		l2_access.addr = l1_access.block_base_addr;
	
		LookupL2(l2_access);
	
		if(unlikely(!l2_access.line))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: line miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}
			
			ChooseLineToEvictL2(l2_access);
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << endl << EndDebugInfo;
			}
			
			EmuEvictL2(l2_access);
		}
	
		if(unlikely(!l2_access.block))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: block miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillL2(l2_access, wimg, rwitm);
		}

		memcpy(&(*l1_access.block)[0], &(*l2_access.block)[l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
		UpdateReplacementPolicyL2(l2_access);

		if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportReadAccess();
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
inline void CPU<CONFIG>::EmuFillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1: filling block at 0x" << std::hex << l1_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}

	// DL1 block fill
	if(likely(IsL2CacheEnabled()))
	{
		// DL1 block fill from L2
		CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
		l2_access.addr = l1_access.block_base_addr;
	
		LookupL2(l2_access);
	
		if(unlikely(!l2_access.line))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: line miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictL2(l2_access);
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << endl << EndDebugInfo;
			}

			EmuEvictL2(l2_access);
		}
	
		if(unlikely(!l2_access.block))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: block miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillL2(l2_access, wimg, false);
		}
		memcpy(&(*l1_access.block)[0], &(*l2_access.block)[l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
		UpdateReplacementPolicyL2(l2_access);

		if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportReadAccess();
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
void CPU<CONFIG>::EmuFillL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access, typename CONFIG::WIMG wimg, bool rwitm)
{
	l2_access.block = &(*l2_access.line)[l2_access.sector];
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2: filling block at 0x" << std::hex << l2_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}
	BusRead(l2_access.block_base_addr, &(*l2_access.block)[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE, wimg, rwitm);
	l2_access.line->status.valid = true;
	l2_access.line->SetBaseAddr(l2_access.line_base_addr);
	l2_access.block->status.valid = true;
	l2_access.block->status.dirty = false;
}

template <class CONFIG>
void CPU<CONFIG>::EmuFetch(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;

	if(GetMSR_IR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_INSN;
	
		EmuTranslateAddress<false>(mmu_access);
	
		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	// Is IL1 enabled ?
	if(likely(IsInsnCacheEnabled()))
	{
		// IL1 Access
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupIL1(l1_access);

		if(unlikely(!l1_access.line))
		{
			// IL1 line miss
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictIL1(l1_access);
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}

			EmuEvictIL1(l1_access);
		}

		if(unlikely(!l1_access.block))
		{
			// IL1 block miss
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillIL1(l1_access, wimg);
		}
	
		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);
		UpdateReplacementPolicyIL1(l1_access);

		if(unlikely(il1_power_estimator_import != 0)) il1_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 disabled
		BusRead(physical_addr, buffer, size, wimg);
	}
}

template <class CONFIG>
template <bool TRANSLATE_ADDR>
void CPU<CONFIG>::EmuLoad(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;

	memset(buffer, 0, size);
	
	if(TRANSLATE_ADDR && GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}
	
	if(likely(!(wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled()))
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);

		if(unlikely(!l1_access.line))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}
			
			EmuEvictDL1(l1_access);
		}

		if(unlikely(!l1_access.block))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillDL1(l1_access, wimg, false /* not a rwitm */);
		}

		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);
		UpdateReplacementPolicyDL1(l1_access);

		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 disabled
		BusRead(physical_addr, buffer, size, wimg);
	}
	if(unlikely(IsVerboseLoad()))
	{
		uint32_t i;
		logger << DebugInfo << "instruction #" << instruction_counter << " (at 0x" << std::hex << GetCIA() << std::dec << "): Loading ";
		for(i = 0; i < size; i++)
		{
			uint8_t value = ((uint8_t *) buffer)[i];
			uint8_t l = value & 15;
			uint8_t h = value >> 4;
			logger << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10) << " ";
		}
		logger << "(" << size << " bytes) at 0x" << std::hex << addr << std::dec << endl << EndDebugInfo;
	}
}

template <class CONFIG>
template <bool TRANSLATE_ADDR>
void CPU<CONFIG>::EmuStore(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	if(unlikely(IsVerboseStore()))
	{
		uint32_t i;
		logger << DebugInfo << "insn #" << instruction_counter << ":Storing ";
		for(i = 0; i < size; i++)
		{
			uint8_t value = ((uint8_t *) buffer)[i];
			uint8_t l = value & 15;
			uint8_t h = value >> 4;
			logger << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10) << " ";
		}
		logger << "(" << size << " bytes) at 0x" << std::hex << addr << std::dec << endl << EndDebugInfo;
	}
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(TRANSLATE_ADDR && GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE;
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	if(likely(!(wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled()))
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);

		if(unlikely(!l1_access.line))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}

			EmuEvictDL1(l1_access);
		}
		
		if(unlikely(!l1_access.block))
		{
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillDL1(l1_access, wimg, true);
		}
	
		// DL1 hit
		memcpy(&(*l1_access.block)[l1_access.offset], buffer, size);
		l1_access.block->status.dirty = true;
		UpdateReplacementPolicyDL1(l1_access);

		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportWriteAccess();

		if(unlikely(wimg & CONFIG::WIMG_WRITE_THROUGH))
		{
			if(likely(IsL2CacheEnabled()))
			{
				// DL1 block fill from L2
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = physical_addr;

				LookupL2(l2_access);

				memcpy(&(*l2_access.block)[l2_access.offset], buffer, size);
				l2_access.block->status.dirty = true;
				UpdateReplacementPolicyL2(l2_access);
				if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportWriteAccess();
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
inline void CPU<CONFIG>::EmuLoad(T& value, typename CONFIG::address_t ea)
{
	// Data Address Breakpoint handling
	if(CONFIG::DABR_ENABLE)
	{
		if(unlikely(GetDABR_DR() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT()))
		{
			throw DSIDataAddressBreakpointException<CONFIG>(ea, CONFIG::MAT_READ);
		}
	}

	// Address munging
	typename CONFIG::address_t munged_ea = MungEffectiveAddress(ea, sizeof(T));

	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (munged_ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory load does not cross a FSB boundary
		EmuLoad<true>(munged_ea, &value, sizeof(T));
	}
	else
	{
		// Memory load crosses a FSB boundary
		EmuLoad<true>(munged_ea, &value, size_to_fsb_boundary);
		EmuLoad<true>(munged_ea + size_to_fsb_boundary, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary);
	}
}

template <class CONFIG>
template <class T>
inline void CPU<CONFIG>::EmuStore(T value, typename CONFIG::address_t ea)
{
	// Data Address	Breakpoint handling
	if(CONFIG::DABR_ENABLE)
	{
		if(unlikely(GetDABR_DW() && ((ea >> 3) & 0x1fffffffUL) == GetDABR_DAB() && GetMSR_DR() == GetDABR_BT()))
		{
			throw DSIDataAddressBreakpointException<CONFIG>(ea, CONFIG::MAT_WRITE);
		}
	}

	// Address munging
	typename CONFIG::address_t munged_ea = MungEffectiveAddress(ea, sizeof(T));

	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (munged_ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory store does not cross a FSB boundary
		EmuStore<true>(munged_ea, &value, sizeof(T));
	}
	else
	{
		// Memory store crosses a FSB boundary
		EmuStore<true>(munged_ea, &value, size_to_fsb_boundary);
		EmuStore<true>(munged_ea + size_to_fsb_boundary, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorLoad(typename CONFIG::address_t ea, uint32_t size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_READ, MemoryAccessReporting<typename CONFIG::address_t>::MT_DATA, ea, size);
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::MonitorStore(typename CONFIG::address_t ea, uint32_t size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_WRITE, MemoryAccessReporting<typename CONFIG::address_t>::MT_DATA, ea, size);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Int8Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT8_LOAD, rd, MungEffectiveAddress(ea, 1), 1);
#else
	uint8_t value;
	EmuLoad<uint8_t>(value, ea);
	gpr[rd] = (uint32_t) value; // 8-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_LOAD, rd, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value;
	EmuLoad<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) BigEndian2Host(value); // 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::SInt16Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::SINT16_LOAD, rd, ea, 2);
#else
	uint16_t value;
	EmuLoad<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) (int16_t) BigEndian2Host(value); // 16-bit to 32-bit sign extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_LOAD, rd, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value;
	EmuLoad<uint32_t>(value, ea);
	gpr[rd] = BigEndian2Host(value);
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp32Load(unsigned int fd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP32_LOAD, fd, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value;
	EmuLoad<uint32_t>(value, ea);
	Flags flags;
	flags.setRoundingMode((fpscr & CONFIG::FPSCR_RN_MASK) >> CONFIG::FPSCR_RN_OFFSET);
	fpr[fd].assign(SoftFloat(BigEndian2Host(value)), flags);
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp64Load(unsigned int fd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP64_LOAD, fd, MungEffectiveAddress(ea, 8), 8);
#else
	uint64_t value;
	EmuLoad<uint64_t>(value, ea);
	fpr[fd] = SoftDouble(BigEndian2Host(value));
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE, rd, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value;
	EmuLoad<uint16_t>(value, ea);
	gpr[rd] = (uint32_t) LittleEndian2Host(value); // reverse bytes and 16-bit to 32-bit zero extension
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE, rd, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value;
	EmuLoad<uint32_t>(value, ea);
	gpr[rd] = LittleEndian2Host(value); // reverse bytes
	MonitorLoad(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT_LOAD_MSB, rd, ea, size);
#else
	switch(size)
	{
		case 1:
		{
			uint8_t value;
			EmuLoad<uint8_t>(value, ea);
			gpr[rd] = (uint32_t) value << 24;
			break;
		}

		case 2:
		{
			uint16_t value;
			EmuLoad<uint16_t>(value, ea);
			gpr[rd] = (uint32_t) BigEndian2Host(value) << 16;
			break;
		}

		case 3:
		{
			uint8_t buffer[3];
			EmuLoad<typeof(buffer)>(buffer, ea);
			uint32_t value = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8);
			gpr[rd] = value;
			break;
		}

		case 4:
		{
			uint32_t value;
			EmuLoad<uint32_t>(value, ea);
			gpr[rd] = BigEndian2Host(value);
			break;
		}
	}
	MonitorLoad(ea, size);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int8Store(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT8_STORE, rs, MungEffectiveAddress(ea, 1), 1);
#else
	uint8_t value = gpr[rs];
	EmuStore<uint8_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16Store(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_STORE, rs, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value = Host2BigEndian((uint16_t) gpr[rs]);
	EmuStore<uint16_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32Store(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_STORE, rs, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value = Host2BigEndian(gpr[rs]);
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp32Store(unsigned int fs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP32_STORE, fs, MungEffectiveAddress(ea, 4), 4);
#else
	Flags flags;
	flags.setRoundingMode(RN_ZERO);
	uint32_t value = Host2BigEndian(SoftFloat(fpr[fs], flags).queryValue());
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Fp64Store(unsigned int fs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP64_STORE, fs, MungEffectiveAddress(ea, 8), 8);
#else
	uint64_t value = Host2BigEndian(fpr[fs].queryValue());
	EmuStore<uint64_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::FP_STORE_LSW, fs, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value = Host2BigEndian((uint32_t) fpr[fs].queryValue());
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE, rs, MungEffectiveAddress(ea, 2), 2);
#else
	uint16_t value = Host2LittleEndian((uint16_t) gpr[rs]);
	EmuStore<uint16_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE, rs, MungEffectiveAddress(ea, 4), 4);
#else
	uint32_t value = Host2LittleEndian(gpr[rs]);
	EmuStore<uint32_t>(value, ea);
	MonitorStore(ea, sizeof(value));
	effective_address = ea;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size)
{
#ifdef SOCLIB
	GenLoadStore(LoadStoreAccess<CONFIG>::INT_STORE_MSB, rs, ea, size);
#else
	switch(size)
	{
		case 1:
			{
				uint8_t value = gpr[rs] >> 24;
				EmuStore<uint8_t>(value, ea);
				break;
			}

		case 2:
			{
				uint16_t value = Host2BigEndian((uint16_t)(gpr[rs] >> 16));
				EmuStore<uint16_t>(value, ea);
				break;
			}

		case 3:
			{
				uint32_t value = gpr[rs];
				uint8_t buffer[3];
				buffer[0] = value >> 24;
				buffer[1] = value >> 16;
				buffer[2] = value >> 8;
				EmuStore<typeof(buffer)>(buffer, ea);
				break;
			}

		case 4:
			{
				uint32_t value = Host2BigEndian(gpr[rs]);
				EmuStore<uint32_t>(value, ea);
				break;
			}
	}

	MonitorStore(ea, size);
#endif
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::LookupBAT(MMUAccess<CONFIG>& mmu_access)
{
	typename CONFIG::address_t addr = mmu_access.addr;
	typename CONFIG::PrivilegeLevel privilege_level = mmu_access.privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	typename CONFIG::MemoryType memory_type = mmu_access.memory_type;
	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field
/*	uint32_t sr_num;						// Segment register number
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
	bool key;*/
	typename CONFIG::WIMG wimg;
	
	//------------------------------
	//-  Block Address Translation -
	//------------------------------

	// Compute BEPI from the effective address
	mmu_access.bepi = bepi = (addr >> 17) & 0x7fff;

	if(memory_type == CONFIG::MT_INSN)
	{
		// Memory access is an instruction fetch
		uint32_t num_ibats = (GetHID0_HIGH_BAT_EN()) ? CONFIG::NUM_BATS : 4;

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
				if((GetIBATU_VP(bat_num) && (privilege_level & CONFIG::PR_USER)) || (GetIBATU_VS(bat_num) && (privilege_level & CONFIG::PR_SUPERVISOR)))
				{
					//cerr << "IBAT match for 0x" << hex << addr << endl;
					// A BAT match occurs
					mmu_access.protection_boundary = ((GetIBATU_BEPI(bat_num) & 0x7ffUL) + GetIBATU_BL(bat_num) + 1) << 17;
					mmu_access.wimg = wimg = (typename CONFIG::WIMG)(GetIBATL_WIMG(bat_num) & 0x6); // W and G are not defined in IBATs;

					// Check if access is guarded
					if(unlikely(wimg & CONFIG::WIMG_GUARDED_MEMORY))
					{
						if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
					}

					// Check access rights
					uint32_t pp = GetIBATL_PP(bat_num);
					if(unlikely(!pp)) // PP=00: No access
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
		uint32_t num_dbats = (GetHID0_HIGH_BAT_EN()) ? CONFIG::NUM_BATS : 4;
		// Scan each DBAT register to find a translation
		for(bat_num = 0; bat_num < num_dbats; bat_num++)
		{
			// Check if DBAT register match
			if((bepi & 0x7800) == (GetDBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
				// the 15 most significative bits in EA and 15-bit DBAT BEPI field
						 && (bepi & ~GetDBATU_BL(bat_num) & 0x7ff) == (GetDBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
				// bits of the 15 most significative bits in EA and DBAT BEPI field according to the DBAT block size mask (BL)
			{
// 				if((GetDBATU_VP(bat_num) && (privilege_level & CONFIG::PR_USER)) || (GetDBATU_VS(bat_num) && (privilege_level & CONFIG::PR_SUPERVISOR)))
				{
					// A DBAT match occurs
					mmu_access.protection_boundary = ((GetDBATU_BEPI(bat_num) & 0x7ffUL) + GetDBATU_BL(bat_num)) << 17;
					mmu_access.wimg = wimg = (typename CONFIG::WIMG) GetDBATL_WIMG(bat_num);
	
					// Check access rights
					uint32_t pp = GetDBATL_PP(bat_num);
					if(unlikely(!pp || // PP=00: No access
					            ((pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) // PP=x1: Read-only
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
			// return typename CONFIG::WIMG bits
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
	if(unlikely(IsVerboseDTLB()))
	{
		logger << DebugInfo << "DTLB Lookup at 0x";
		logger << std::hex << virtual_addr << std::dec << " : ";
		logger << "base_virtual_addr=0x" << std::hex << base_virtual_addr << std::dec << ",";
		logger << "tlb_index=0x" << std::hex << tlb_index << std::dec << ",";
		logger << endl << EndDebugInfo;
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
			if(unlikely(IsVerboseDTLB()))
			{
				logger << DebugInfo << "DTLB hit: tlb_way=" << tlb_way << ", base_physical_addr=0x" << std::hex << tlb_entry->pte.base_physical_addr << std::dec << endl << EndDebugInfo;
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
	if(unlikely(IsVerboseDTLB()))
	{
		logger << DebugInfo << "DTLB miss: choosen TLB way=" << mmu_access.tlb_way << endl << EndDebugInfo;
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
	uint32_t i = 0;
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
	uint32_t i = 0;
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
	typename CONFIG::address_t addr = mmu_access.addr;
	typename CONFIG::PrivilegeLevel privilege_level = mmu_access.privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	typename CONFIG::MemoryType memory_type = mmu_access.memory_type;
/*	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field*/
	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	typename CONFIG::virtual_address_t virtual_addr;			// Virtual address
	typename CONFIG::virtual_address_t base_virtual_addr;	// Base virtual address
/*	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t h;*/
/*	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index*/
/*	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;*/
	bool key;
	typename CONFIG::WIMG wimg;
// 	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set;
// 	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;

	// Compute the protection boundary
	mmu_access.protection_boundary = (addr + (CONFIG::MEMORY_PAGE_SIZE)) & ~(CONFIG::MEMORY_PAGE_SIZE - 1);

	//-------------------------------
	//- Compute the virtual address -
	//-------------------------------

	// Get the segment register number from the most significative bits in the effective address
	mmu_access.sr_num = sr_num = (addr >> 28) & 0xf;
	
	// Get the No-execute bit from the segment register
	mmu_access.sr_noexecute = sr_noexecute = GetSR_N(sr_num);

	if(unlikely((memory_type == CONFIG::MT_INSN) && sr_noexecute))
	{
		// Raise an exception because the program is trying to execute instructions within a No-execute segment
		if(!DEBUG) throw ISINoExecuteException<CONFIG>(addr);
	}
	
	if(unlikely(GetSR_T(sr_num) == 1))
	{
		// Raise an exception because the program is trying to access to a direct-store segment (not supported by PPC750)
		if(memory_type == CONFIG::MT_INSN)
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
	mmu_access.key = key = (sr_kp && (privilege_level & CONFIG::PR_USER)) || (sr_ks && (privilege_level & CONFIG::PR_SUPERVISOR));
	
	// Get the virtual segment id from the segment register
	mmu_access.virtual_segment_id = virtual_segment_id = GetSR_VSID(sr_num);
	
	// Compute the virtual address from the virtual segment id and the least significant bits of the effective address
	mmu_access.virtual_addr = virtual_addr = ((typename CONFIG::virtual_address_t) virtual_segment_id << 28) | (typename CONFIG::virtual_address_t)(addr & 0x0fffffffUL);
	
	mmu_access.force_page_table_walk = false;
	//------------------------------
	//-    Search in ITLB/DTLB     -
	//------------------------------
	if(memory_type == CONFIG::MT_INSN)
	{
		if(CONFIG::ITLB_CONFIG::ENABLE)
		{
			// Memory access is an instruction fetch
			LookupITLB(mmu_access);
			if(unlikely(itlb_power_estimator_import != 0)) itlb_power_estimator_import->ReportReadAccess();

			if(likely(mmu_access.tlb_hit))
			{
				// ITLB hit
				mmu_access.wimg = wimg = mmu_access.itlb_entry->pte.wimg;
						
				if(unlikely(wimg & CONFIG::WIMG_GUARDED_MEMORY))
				{
					if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
				}
			
				// Check access rights
				if(unlikely(key && !mmu_access.itlb_entry->pte.pp))
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
			if(unlikely(dtlb_power_estimator_import != 0)) dtlb_power_estimator_import->ReportReadAccess();

			if(likely(mmu_access.tlb_hit))
			{
				// DTLB Hit
				// return typename CONFIG::WIMG bits
				mmu_access.wimg = wimg = mmu_access.dtlb_entry->pte.wimg;
	
				// Check access rights
				if(unlikely((key && (!mmu_access.dtlb_entry->pte.pp ||
				            ((mmu_access.dtlb_entry->pte.pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) ||
                             (!key && (mmu_access.dtlb_entry->pte.pp==3 && (memory_access_type == CONFIG::MAT_WRITE)))))
				{
					// Raise a protection violation
					if(!DEBUG)
					{
						throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				mmu_access.force_page_table_walk = memory_access_type == CONFIG::MAT_WRITE && !mmu_access.dtlb_entry->pte.c;

				// Update the replacement policy
				if(likely(!DEBUG && !mmu_access.force_page_table_walk)) UpdateDTLBReplacementPolicy(mmu_access);
					
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
	mmu_access.base_virtual_addr = base_virtual_addr = virtual_addr & ~((typename CONFIG::virtual_address_t) CONFIG::MEMORY_PAGE_SIZE - 1);
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::EmuHardwarePageTableSearch(MMUAccess<CONFIG>& mmu_access)
{
	typename CONFIG::address_t addr = mmu_access.addr;
	typename CONFIG::PrivilegeLevel privilege_level = mmu_access.privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	typename CONFIG::MemoryType memory_type = mmu_access.memory_type;
	uint32_t sr_num = mmu_access.sr_num;						// Segment register number
	uint32_t virtual_segment_id = mmu_access.virtual_segment_id;			// Virtual segment id
//	virtual_address_t virtual_addr = mmu_access.virtual_addr;			// Virtual address
	typename CONFIG::virtual_address_t base_virtual_addr = mmu_access.base_virtual_addr;	// Base virtual address
//	uint32_t tlb_index = mmu_access.tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	typename CONFIG::physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key = mmu_access.key;
	typename CONFIG::WIMG wimg;

	// Compute the page index from the virtual address
	mmu_access.page_index = page_index = (base_virtual_addr >> 12) & 0xffff;

	// Compute API from the page index
	mmu_access.api = api = page_index >> 10;

	if(unlikely(!DEBUG && GetHID0_STEN()))
	{
		// Software page table search is enabled
		
		/* primary PTEG address */
		hash = (virtual_segment_id & 0x7ffff) ^ page_index;
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		typename CONFIG::physical_address_t primary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
			
		/* secondary PTEG address */
		hash = (~hash) & 0x7ffff;
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		typename CONFIG::physical_address_t secondary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
		
		// raise a TLB miss
		throw TLBMissException<CONFIG>(
			memory_access_type,
			memory_type,
			addr,
			mmu_access.tlb_way,
			(privilege_level & CONFIG::PR_USER) ? GetSR_KP(sr_num) : GetSR_KS(sr_num),
			virtual_segment_id,
			api,
			primary_pteg,
			secondary_pteg);
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
			typename CONFIG::WIMG pte_wimg;
			uint32_t pte_c;
			typename CONFIG::physical_address_t base_physical_addr;


//			cerr << "pte_addr = 0x" << std::hex << pte_addr << std::dec << endl;
			// Read the page table entry from memory
			if(DEBUG)
			{
				ReadMemory(pte_addr, &pte_value, 8, CONFIG::MT_DATA, false);
			}
			else
			{
				EmuLoad<false>(pte_addr, &pte_value, 8);
			}

			pte_value = BigEndian2Host(pte_value);
//			cerr << "pte_value = 0x" << std::hex << pte_value << std::dec << endl;

			pte_valid = (pte_value >> 63) & 1;
//			cerr << "pte_valid = " << pte_valid << endl;
			if(likely(!pte_valid)) // check valid bit
				continue;
			pte_h = (pte_value >> 38) & 1;
//			cerr << "pte_h = " << pte_h << endl;
			if(likely(pte_h != h)) // good hash key ?
				continue;

			pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
//			cerr << "pte_virtual_segment_id = 0x" << std::hex << pte_virtual_segment_id << std::dec << endl;
			pte_api = (pte_value >> 32) & 0x3f;
			
			if(unlikely(pte_virtual_segment_id == virtual_segment_id && // compare VSIDs
						pte_api == api)) // compare APIs
			{
				// Hit in page table
				pte_wimg = (typename CONFIG::WIMG)((pte_value >> 3) & 0xf);
				mmu_access.wimg = wimg = pte_wimg;
				
				// Check if access is guarded
				if(unlikely((memory_type == CONFIG::MT_INSN) && (wimg & CONFIG::WIMG_GUARDED_MEMORY)))
				{
					if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
				}

				pte_pp = (pte_value & 3);
				
				// Check access rights
				if(unlikely((key && (!pte_pp || ((pte_pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) || (!key && (pte_pp==3 && (memory_access_type == CONFIG::MAT_WRITE)))))
				{
					// Raise a protection violation
					if(memory_type == CONFIG::MT_INSN)
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
				if((memory_access_type == CONFIG::MAT_WRITE)) pte_value |= 0x80ULL; // set change bit (C)

				base_physical_addr = pte_value & 0xfffff000UL;
//				cerr << "base_physical_addr = 0x" << std::hex << base_physical_addr << std::dec << endl;
				
				// Build an access attribute for the page	
				pte_c = (pte_value >> 7) & 1;

				// Update the page table entry into memory
				pte_value = Host2BigEndian(pte_value);

				if(!DEBUG)
				{
					EmuStore<false>(pte_addr, &pte_value, 8);
				}

				if(memory_type == CONFIG::MT_INSN)
				{
					if(CONFIG::ITLB_CONFIG::ENABLE)
					{
						// ITLB Refill
						if(unlikely(itlb_power_estimator_import != 0)) itlb_power_estimator_import->ReportWriteAccess();
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
						// DTLB Refill
						if(unlikely(dtlb_power_estimator_import != 0)) dtlb_power_estimator_import->ReportWriteAccess();
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
	
	if(memory_type == CONFIG::MT_INSN)
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
	typename CONFIG::physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	uint32_t mask = GetSDR1_HTABMASK();
	uint32_t hash_lo, hash_hi;
	typename CONFIG::physical_address_t pteg_select;
	
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
					typename CONFIG::virtual_address_t base_virtual_addr;
					uint32_t pte_valid, pte_h, pte_virtual_segment_id, pte_api, pte_pp, pte_r, pte_c, pte_wimg;
					typename CONFIG::address_t base_paddr;

					ReadMemory(pte_addr, &pte_value, 8, CONFIG::MT_DATA, false);
		
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
void CPU<CONFIG>::EmuTranslateAddress(MMUAccess<CONFIG>& mmu_access)
{
	LookupBAT<DEBUG>(mmu_access);
	if(likely(!mmu_access.bat_hit))
	{
		AccessTLB<DEBUG>(mmu_access);

		if(unlikely(!mmu_access.tlb_hit || mmu_access.force_page_table_walk))
		{
			// BAT/TLB Miss
			EmuHardwarePageTableSearch<DEBUG>(mmu_access);
		}
	}
}

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr)
{
	uint32_t read_offset = 0;

	do
	{
		typename CONFIG::WIMG wimg;
		typename CONFIG::physical_address_t physical_addr;
		typename CONFIG::address_t protection_boundary;
		if(translate_addr && GetMSR_DR())
		{
			// Address translation
			MMUAccess<CONFIG> mmu_access;
		
			mmu_access.addr = addr;
			mmu_access.privilege_level = GetPrivilegeLevel();
			mmu_access.memory_access_type = CONFIG::MAT_READ;
			mmu_access.memory_type = mt;
	
			EmuTranslateAddress<true>(mmu_access); // debug is enabled
	
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
			wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
			physical_addr = addr;
			protection_boundary = addr + size;
		}

		uint32_t size_to_protection_boundary = protection_boundary - addr;

		do
		{
			uint32_t sz = 0;

			if(unlikely(IsVerboseReadMemory()))
			{
				logger << DebugInfo << "0x" << std::hex << addr << std::dec << ":";
			}

			switch(mt)
			{
				case CONFIG::MT_DATA:
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
							if(unlikely(IsVerboseReadMemory()))
							{
								logger << "DL1: ";
							}
						}
					}
					break;
				case CONFIG::MT_INSN:
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
							if(unlikely(IsVerboseReadMemory()))
							{
								logger << "IL1: ";
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
					if(unlikely(IsVerboseReadMemory()))
					{
						logger << "L2: ";
					}
				}
			}

			if(!sz)
			{
				sz = (size_to_protection_boundary > size) ? size : size_to_protection_boundary;
				if(!memory_import->ReadMemory(physical_addr, (uint8_t *) buffer + read_offset, sz)) return false;
				if(unlikely(IsVerboseReadMemory()))
				{
					logger << "MEM: ";
				}
			}

			if(unlikely(IsVerboseReadMemory()))
			{
				uint32_t i;
				for(i = 0; i < sz; i++)
				{
					logger << std::hex << (unsigned int)((((uint8_t *) buffer)[read_offset + i]) >> 4) << (unsigned int)((((uint8_t *) buffer)[read_offset + i]) & 15) << std::dec << " ";
				}
				logger << endl << EndDebugInfo;
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
bool CPU<CONFIG>::WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size, typename CONFIG::MemoryType mt, bool translate_addr)
{
	uint32_t write_offset = 0;

	do
	{
		typename CONFIG::WIMG wimg;
		typename CONFIG::physical_address_t physical_addr;
		typename CONFIG::address_t protection_boundary;
		if(translate_addr && GetMSR_DR())
		{
			// Address translation
			MMUAccess<CONFIG> mmu_access;
		
			mmu_access.addr = addr;
			mmu_access.privilege_level = CONFIG::PR_SUPERVISOR;
			mmu_access.memory_access_type = CONFIG::MAT_WRITE;
			mmu_access.memory_type = mt;
	
			EmuTranslateAddress<true>(mmu_access); // debug is enabled
	
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
			wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
			physical_addr = addr;
			protection_boundary = addr + size;
		}

		uint32_t size_to_protection_boundary = protection_boundary - addr;

		do
		{
			uint32_t sz = 0;

			if(unlikely(IsVerboseWriteMemory()))
			{
				logger << DebugInfo << "0x" << std::hex << addr << std::dec << ":";
			}

			switch(mt)
			{
				case CONFIG::MT_DATA:
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
							if(unlikely(IsVerboseWriteMemory()))
							{
								logger << "DL1: ";
							}
						}
					}
					break;
				case CONFIG::MT_INSN:
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
							if(unlikely(IsVerboseWriteMemory()))
							{
								logger << "IL1: ";
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
					if(unlikely(IsVerboseWriteMemory()))
					{
						logger << "L2: ";
					}
				}
			}

			if(!sz)
			{
				sz = (size_to_protection_boundary > size) ? size : size_to_protection_boundary;
				if(!memory_import->WriteMemory(physical_addr, (uint8_t *) buffer + write_offset, sz)) return false;
				if(unlikely(IsVerboseWriteMemory()))
				{
					logger << "MEM: ";
				}
			}

			if(unlikely(IsVerboseWriteMemory()))
			{
				uint32_t i;
				for(i = 0; i < sz; i++)
				{
					logger << std::hex << (unsigned int)((((uint8_t *) buffer)[write_offset + i]) >> 4) << (unsigned int)((((uint8_t *) buffer)[write_offset + i]) & 15) << std::dec << " ";
				}
				logger << endl << EndDebugInfo;
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
bool CPU<CONFIG>::ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	return ReadMemory(addr, buffer, size, CONFIG::MT_DATA, true);
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	return WriteMemory(addr, buffer, size, CONFIG::MT_DATA, true);
}

template <class CONFIG>
bool CPU<CONFIG>::InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		uint32_t sz;
		uint8_t *dst = (uint8_t *) buffer;
		do
		{
			uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (addr & (CONFIG::FSB_WIDTH - 1));
			sz = size > size_to_fsb_boundary ? size_to_fsb_boundary : size;
			EmuLoad<true>(addr, dst, sz);
			dst += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		uint32_t sz;
		const uint8_t *src = (const uint8_t *) buffer;
		do
		{
			uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (addr & (CONFIG::FSB_WIDTH - 1));
			sz = size > size_to_fsb_boundary ? size_to_fsb_boundary : size;
			EmuStore<true>(addr, src, sz);
			src += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}

template <class CONFIG>
string CPU<CONFIG>::Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr)
{
	stringstream sstr;
	unisim::component::cxx::processor::powerpc::isa::Operation<CONFIG> *operation;
	uint32_t insn;

	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;

	if(GetMSR_IR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_INSN;
	
		EmuTranslateAddress<true>(mmu_access);
	
		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
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

	operation = unisim::component::cxx::processor::powerpc::isa::Decoder<CONFIG>::Decode(addr, insn);

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
	if(unlikely(IsVerboseStep()))
		logger << DebugInfo << "Program exited with code " << ret << endl << EndDebugInfo;
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
void CPU<CONFIG>::Dcba(typename CONFIG::address_t addr)
{
	Dcbz(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dcbf(typename CONFIG::address_t addr)
{
	Dcbst(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dcbi(typename CONFIG::address_t addr)
{
	if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();

	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE;
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
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
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: flushing/invalidating dirty block at 0x" << std::hex << l1_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
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
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2: flushing/invalidating dirty block at 0x" << std::hex << l2_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
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
void CPU<CONFIG>::Dcbst(typename CONFIG::address_t addr)
{
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ; // 3.4.4.4 Data Cache Block Store (dcbst): This instruction is treated as a load with respect to address translation and memory protection
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
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
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: flushing/invalidating dirty block at 0x" << std::hex << l1_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
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
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2: flushing/invalidating dirty block at 0x" << std::hex << l2_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
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
void CPU<CONFIG>::Dcbz(typename CONFIG::address_t addr)
{
	if(CONFIG::DL1_CONFIG::ENABLE && !IsDataCacheEnabled())
	{
		throw AlignmentException<CONFIG>(addr);
	}

	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE; // 3.4.4.3 Data Cache Block Zero (dcbz): The dcbz instruction is treated as a store to the addressed byte with respect to address translation, protection, and pipelining.
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
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
			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}
			
			EmuEvictDL1(l1_access);
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
		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportWriteAccess();

		if(IsL2CacheEnabled())
		{
			// L2 Access
			CacheAccess<class CONFIG::L2_CONFIG> l2_access;
		
			l2_access.addr = physical_addr;
			LookupL2(l2_access);
		
			if(!l2_access.line)
			{
				ChooseLineToEvictL2(l2_access);
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << endl << EndDebugInfo;
				}

				EmuEvictL2(l2_access);
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
			if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportWriteAccess();
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
void CPU<CONFIG>::Icbi(typename CONFIG::address_t addr)
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

	unisim::component::cxx::processor::powerpc::isa::Decoder<CONFIG>::InvalidateDecodingCacheEntry(addr);
}

/* TLB management */
template <class CONFIG>
void CPU<CONFIG>::Tlbia()
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Tlbie(typename CONFIG::address_t addr)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}
	InvalidateDTLBSet((addr / CONFIG::MEMORY_PAGE_SIZE) & ((CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES / CONFIG::DTLB_CONFIG::TLB_ASSOCIATIVITY) - 1));
	InvalidateITLBSet((addr / CONFIG::MEMORY_PAGE_SIZE) & ((CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES / CONFIG::ITLB_CONFIG::TLB_ASSOCIATIVITY) - 1));
}

template <class CONFIG>
void CPU<CONFIG>::LoadITLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::ITLB_CONFIG::ENABLE)
	{
		uint32_t index = addr / CONFIG::MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		typename CONFIG::address_t base_physical_addr = pte_lo & ~(CONFIG::MEMORY_PAGE_SIZE - 1);
		typename CONFIG::virtual_address_t base_virtual_addr = ((typename CONFIG::virtual_address_t) virtual_segment_id << 28) | (typename CONFIG::virtual_address_t)(addr & 0x0fffffffUL & ~(CONFIG::MEMORY_PAGE_SIZE - 1));
		
		TLBEntry<typename CONFIG::ITLB_CONFIG>& tlb_entry = itlb[index][way];
	
		tlb_entry.status.valid = ((pte_hi >> 31) & 1) ? true : false;
		tlb_entry.SetBaseVirtualAddr(base_virtual_addr);
		tlb_entry.pte.c = (pte_lo >> 7) & 1;
		tlb_entry.pte.wimg = (typename CONFIG::WIMG)((pte_lo >> 3) & 0xf);
		tlb_entry.pte.pp = (pte_lo & 3);
		tlb_entry.pte.base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void CPU<CONFIG>::LoadDTLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::DTLB_CONFIG::ENABLE)
	{
		uint32_t index = addr / CONFIG::MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		typename CONFIG::address_t base_physical_addr = pte_lo & ~(CONFIG::MEMORY_PAGE_SIZE - 1);
		typename CONFIG::virtual_address_t base_virtual_addr = ((typename CONFIG::virtual_address_t) virtual_segment_id << 28) | (typename CONFIG::virtual_address_t)(addr & 0x0fffffffUL & ~(CONFIG::MEMORY_PAGE_SIZE - 1));
		
		TLBEntry<typename CONFIG::DTLB_CONFIG>& tlb_entry = dtlb[index][way];
	
		tlb_entry.SetBaseVirtualAddr(base_virtual_addr);
		tlb_entry.pte.c = (pte_lo >> 7) & 1;
		tlb_entry.pte.wimg = (typename CONFIG::WIMG)((pte_lo >> 3) & 0xf);
		tlb_entry.pte.pp = (pte_lo & 3);
		tlb_entry.pte.base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbld(typename CONFIG::address_t addr)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}

	LoadDTLBEntry(addr, addr & 1, GetPTEHI(), GetPTEHI());
}

template <class CONFIG>
void CPU<CONFIG>::Tlbli(typename CONFIG::address_t addr)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}

	LoadITLBEntry(addr, addr & 1, GetPTEHI(), GetPTEHI());
}


/* Linked Load-Store */
template <class CONFIG>
void CPU<CONFIG>::Lwarx(unsigned int rd, typename CONFIG::address_t addr)
{
	Int32Load(rd, addr);

	reserve = true;
	reserve_addr = addr;
}

template <class CONFIG>
void CPU<CONFIG>::Stwcx(unsigned int rs, typename CONFIG::address_t addr)
{
	// TBD
	if(reserve)
	{
		if(reserve_addr == addr)
		{
			Int32Store(rs, addr);

			/* clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO] */
			SetCR((GetCR() & ~CONFIG::CR0_MASK) | CONFIG::CR0_EQ_MASK | ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
			reserve = false;
			return;
		}
	}

	/* clear CR0 and copy XER[SO] to CR0[SO] */
	SetCR((GetCR() & ~CONFIG::CR0_MASK) | ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
}

template <class CONFIG>
void CPU<CONFIG>::ReqDecrementerOverflow()
{
	asynchronous_interrupt |= CONFIG::SIG_DECREMENTER_OVERFLOW;
}

template <class CONFIG>
void CPU<CONFIG>::AckDecrementerOverflow()
{
	asynchronous_interrupt &= ~CONFIG::SIG_DECREMENTER_OVERFLOW;
}

template <class CONFIG>
void CPU<CONFIG>::ReqExternalInterrupt()
{
	if(unlikely(IsVerboseException())) {
		logger << DebugInfo << "Received external interrupt" << EndDebugInfo;
	}
	asynchronous_interrupt |= CONFIG::SIG_EXTERNAL_INTERRUPT;
}

template <class CONFIG>
void CPU<CONFIG>::AckExternalInterrupt()
{
	asynchronous_interrupt &= ~CONFIG::SIG_EXTERNAL_INTERRUPT;
}

template <class CONFIG>
void CPU<CONFIG>::ReqHardReset()
{
	asynchronous_interrupt |= CONFIG::SIG_HARD_RESET;
}

template <class CONFIG>
void CPU<CONFIG>::AckHardReset()
{
	asynchronous_interrupt &= ~CONFIG::SIG_HARD_RESET;
}

template <class CONFIG>
void CPU<CONFIG>::ReqSoftReset()
{
	asynchronous_interrupt |= CONFIG::SIG_SOFT_RESET;
}

template <class CONFIG>
void CPU<CONFIG>::AckSoftReset()
{
	asynchronous_interrupt &= ~CONFIG::SIG_SOFT_RESET;
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
}

template <class CONFIG>
void CPU<CONFIG>::ReqMCP()
{
	asynchronous_interrupt |= CONFIG::SIG_MCP;
}

template <class CONFIG>
void CPU<CONFIG>::AckMCP()
{
	asynchronous_interrupt &= ~CONFIG::SIG_MCP;
}

template <class CONFIG>
void CPU<CONFIG>::ReqTEA()
{
	asynchronous_interrupt |= CONFIG::SIG_TEA;
}

template <class CONFIG>
void CPU<CONFIG>::AckTEA()
{
	asynchronous_interrupt &= ~CONFIG::SIG_TEA;
}

template <class CONFIG>
void CPU<CONFIG>::ReqSMI()
{
	asynchronous_interrupt |= CONFIG::SIG_SMI;
}

template <class CONFIG>
void CPU<CONFIG>::AckSMI()
{
	asynchronous_interrupt &= ~CONFIG::SIG_SMI;
}


template <class CONFIG>
void CPU<CONFIG>::ReqPerformanceMonitorInterrupt()
{
	asynchronous_interrupt |= CONFIG::SIG_PERFORMANCE_MONITOR_INTERRUPT;
}

template <class CONFIG>
void CPU<CONFIG>::AckPerformanceMonitorInterrupt()
{
	asynchronous_interrupt &= ~CONFIG::SIG_PERFORMANCE_MONITOR_INTERRUPT;
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
Event<CONFIG> *CPU<CONFIG>::AllocateEvent()
{
	if(!event_free_list)
	{
		event_free_list = new Event<CONFIG>();
		event_free_list->next_free = 0;
	}

	Event<CONFIG> *ev = event_free_list;
	event_free_list = ev->next_free;
	ev->type = Event<CONFIG>::EV_NULL;
	ev->object.null = 0;
	ev->next_free = 0;
	return ev;
}

template <class CONFIG>
Instruction<CONFIG> *CPU<CONFIG>::AllocateInstruction()
{
	if(!insn_free_list)
	{
		insn_free_list = new Instruction<CONFIG>();
		insn_free_list->next_free = 0;
	}

	Instruction<CONFIG> *instruction = insn_free_list;
	insn_free_list = instruction->next_free;
	instruction->next_free = 0;
	return instruction;
}

template <class CONFIG>
Operand<CONFIG> *CPU<CONFIG>::AllocateOperand()
{
	if(!operand_free_list)
	{
		operand_free_list = new Operand<CONFIG>();
		operand_free_list->next_free = 0;
	}

	Operand<CONFIG> *operand = operand_free_list;
	operand_free_list = operand->next_free;
	operand->ref_count = 1;
	operand->next_free = 0;
	return operand;
}

template <class CONFIG>
void CPU<CONFIG>::AcquireOperand(Operand<CONFIG> *operand)
{
	operand->ref_count++;
}

template <class CONFIG>
LoadStoreAccess<CONFIG> *CPU<CONFIG>::AllocateLoadStoreAccess()
{
	if(!load_store_access_free_list)
	{
		load_store_access_free_list = new LoadStoreAccess<CONFIG>();
		load_store_access_free_list->next_free = 0;
	}

	LoadStoreAccess<CONFIG> *load_store_access = load_store_access_free_list;
	load_store_access_free_list = load_store_access->next_free;
	load_store_access->next_free = 0;	
	return load_store_access;
}

template <class CONFIG>
void CPU<CONFIG>::GenLoadStoreAccess(typename LoadStoreAccess<CONFIG>::Type type, unsigned int reg_num, typename CONFIG::address_t munged_ea, uint32_t size, Instruction<CONFIG> *instruction)
{
	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (munged_ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(size_to_fsb_boundary >= size)
	{
		// Memory load does not cross a FSB boundary
		LoadStoreAccess<CONFIG> *load_store_access = AllocateLoadStoreAccess();
		load_store_access->type = type;
		load_store_access->reg_num = reg_num;
		load_store_access->munged_ea = munged_ea;
		load_store_access->offset = 0;
		load_store_access->size = size;
		load_store_access->instruction = instruction;
		instruction->load_store_access_queue.Push(load_store_access);
	}
	else
	{
		// Memory load crosses a FSB boundary
		LoadStoreAccess<CONFIG> *load_store_access1 = AllocateLoadStoreAccess();
		load_store_access1->type = type;
		load_store_access1->reg_num = reg_num;
		load_store_access1->munged_ea = munged_ea;
		load_store_access1->offset = 0;
		load_store_access1->size = size_to_fsb_boundary;
		load_store_access1->instruction = instruction;
		instruction->load_store_access_queue.Push(load_store_access1);

		LoadStoreAccess<CONFIG> *load_store_access2 = AllocateLoadStoreAccess();
		load_store_access2->type = type;
		load_store_access2->reg_num = reg_num;
		load_store_access2->munged_ea = munged_ea + size_to_fsb_boundary;
		load_store_access2->offset = size_to_fsb_boundary;
		load_store_access2->size = size - size_to_fsb_boundary;
		load_store_access2->instruction = instruction;
		instruction->load_store_access_queue.Push(load_store_access2);
	}
}

template <class CONFIG>
BusAccess<CONFIG> *CPU<CONFIG>::AllocateBusAccess()
{
	if(!bus_access_free_list)
	{
		bus_access_free_list = new BusAccess<CONFIG>();
		bus_access_free_list->next_free = 0;
	}

	BusAccess<CONFIG> *bus_access = bus_access_free_list;
	bus_access_free_list = bus_access->next_free;
	bus_access->next_free = 0;	
	return bus_access;
}

template <class CONFIG>
FetchAccess<CONFIG> *CPU<CONFIG>::AllocateFetchAccess()
{
	if(!fetch_access_free_list)
	{
		fetch_access_free_list = new FetchAccess<CONFIG>();
		fetch_access_free_list->next_free = 0;
	}

	FetchAccess<CONFIG> *fetch_access = fetch_access_free_list;
	fetch_access_free_list = fetch_access->next_free;
	fetch_access->next_free = 0;	
	return fetch_access;
}

template <class CONFIG>
void CPU<CONFIG>::FreeEvent(Event<CONFIG> *ev)
{
	ev->next_free = event_free_list;
	event_free_list = ev;
}

template <class CONFIG>
void CPU<CONFIG>::FreeInstruction(Instruction<CONFIG> *instruction)
{
	abort();
	unsigned int num_operands = instruction->input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_operands; i++)
	{
		Operand<CONFIG> *operand = instruction->input_operands[i];
		FreeOperand(operand);
	}

	instruction->next_free = insn_free_list;
	insn_free_list = instruction;
}

template <class CONFIG>
void CPU<CONFIG>::FreeOperand(Operand<CONFIG> *operand)
{
	if(--operand->ref_count == 0)
	{
		operand->next_free = operand_free_list;
		operand_free_list = operand;
	}
}

template <class CONFIG>
void CPU<CONFIG>::FreeLoadStoreAccess(LoadStoreAccess<CONFIG> *load_store_access)
{
	load_store_access->next_free = load_store_access_free_list;
	load_store_access_free_list = load_store_access;
}

template <class CONFIG>
void CPU<CONFIG>::FreeBusAccess(BusAccess<CONFIG> *bus_access)
{
	bus_access->next_free = bus_access_free_list;
	bus_access_free_list = bus_access;
}

template <class CONFIG>
void CPU<CONFIG>::FreeFetchAccess(FetchAccess<CONFIG> *fetch_access)
{
	fetch_access->next_free = fetch_access_free_list;
	fetch_access_free_list = fetch_access;
}


template <class CONFIG>
void CPU<CONFIG>::Fetch()
{
	// Stall if IQ is full
	if(iq.Full()) return;
	
	FetchAccess<CONFIG> *fetch_access = AllocateFetchAccess();
	unsigned int lat = 0;
	
	uint32_t size_to_block_boundary = CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (cia & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	uint32_t fetch_width = Max(iq.Size(), CONFIG::FETCH_WIDTH);
	fetch_access->cia = seq_cia;
	fetch_access->size = Max(fetch_width * sizeof(uint32_t), size_to_block_boundary);
	fetch_access->mmu_access.addr = seq_cia;
	fetch_access->mmu_access.privilege_level = GetPrivilegeLevel();
	fetch_access->mmu_access.memory_type = CONFIG::MT_INSN;
	fetch_access->mmu_access.memory_access_type = CONFIG::MAT_READ;

	// Check wether IMMU is enabled
	if(GetMSR_IR())
	{
		// BATs and TLB Lookup
		LookupBAT<false>(fetch_access->mmu_access);
		if(!fetch_access->mmu_access.bat_hit)
		{
			AccessTLB<false>(fetch_access->mmu_access);
				
			if(!fetch_access->mmu_access.tlb_hit)
			{
				// BAT/TLB Miss
				abort(); // Hardware page table search should start at commit
			}
		}
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		fetch_access->mmu_access.wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		fetch_access->mmu_access.physical_addr = fetch_access->mmu_access.addr;
	}

	logger << DebugInfo << *fetch_access << EndDebugInfo;

	if(!(fetch_access->mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsInsnCacheEnabled())
	{
		ClearAccessIL1(fetch_access->l1_access); // TO BE REMOVED
		// IL1 Access
		fetch_access->l1_access.addr = fetch_access->mmu_access.physical_addr;
		LookupIL1(fetch_access->l1_access);
		lat += CONFIG::IL1_LATENCY;

		if(fetch_access->l1_access.block)
		{
			// Fetch hit in IL1
			memcpy(fetch_access->data, &fetch_access->l1_access.block[fetch_access->l1_access.offset], fetch_access->size);
			UpdateReplacementPolicyIL1(fetch_access->l1_access);
			NotifyFinishedFetch(fetch_access, lat);
		}
		else
		{
			// Fetch miss in IL1
			if(num_outstanding_l1_fetch_miss >= CONFIG::MAX_OUTSTANDING_L1_FETCH_MISS) return;
			num_outstanding_l1_fetch_miss++;

			if(IsL2CacheEnabled())
			{
				ClearAccessL2(fetch_access->l2_access); // TO BE REMOVED
				// DL1 block fill from L2
				fetch_access->l2_access.addr = fetch_access->l1_access.block_base_addr;
			
				LookupL2(fetch_access->l2_access);
				lat += CONFIG::L2_LATENCY;

				if(fetch_access->l2_access.block)
				{
					// IL1 block fill hit in L2
					memcpy(&(*fetch_access->l1_access.block)[0], &(*fetch_access->l2_access.block)[fetch_access->l2_access.offset], CacheBlock<class CONFIG::IL1_CONFIG>::SIZE);
					UpdateReplacementPolicyL2(fetch_access->l2_access);
					memcpy(fetch_access->data, &(*fetch_access->l1_access.block)[fetch_access->l1_access.offset], fetch_access->size);
					UpdateReplacementPolicyIL1(fetch_access->l1_access);
					NotifyFinishedFetch(fetch_access, lat);
				}
				else
				{
					// Load miss in L2
					if(num_outstanding_l2_load_miss >= CONFIG::MAX_OUTSTANDING_L2_LOAD_MISS) return;

					// L2 block fill from bus
					fetch_access->l2_access.block = &(*fetch_access->l2_access.line)[fetch_access->l2_access.sector];

					BusAccess<CONFIG> *bus_access = AllocateBusAccess();
					bus_access->issued = false;
					bus_access->type = BusAccess<CONFIG>::REFILL;
					bus_access->addr = fetch_access->l2_access.block_base_addr;
					bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
					bus_access->load_store_access = 0;
					bus_access->fetch_access = fetch_access;
					bus_access->wimg = fetch_access->mmu_access.wimg;
					bus_access->rwitm = false;
					NotifyBusAccess(bus_access, lat);
				}
			}
			else
			{
				// DL1 block fill from bus
				BusAccess<CONFIG> *bus_access = AllocateBusAccess();
				bus_access->issued = false;
				bus_access->type = BusAccess<CONFIG>::REFILL;
				bus_access->addr = fetch_access->l1_access.block_base_addr;
				bus_access->size = CacheBlock<class CONFIG::IL1_CONFIG>::SIZE;
				bus_access->load_store_access = 0;
				bus_access->fetch_access = fetch_access;
				bus_access->wimg = fetch_access->mmu_access.wimg;
				bus_access->rwitm = false;
				NotifyBusAccess(bus_access, lat);
			}
		}
	}
	else
	{
		// Caching is either disabled or inhibited
		BusAccess<CONFIG> *bus_access = AllocateBusAccess();
		bus_access->issued = false;
		bus_access->type = BusAccess<CONFIG>::LOAD;
		bus_access->addr = fetch_access->mmu_access.physical_addr;
		bus_access->size = fetch_access->size;
		bus_access->load_store_access = 0;
		bus_access->fetch_access = fetch_access;
		bus_access->wimg = fetch_access->mmu_access.wimg;
		bus_access->rwitm = false;
		NotifyBusAccess(bus_access, lat);
	}

	seq_cia += fetch_access->size;
	// Send a request to IMMU
/*	uint32_t size_to_block_boundary = CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (cia & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	uint32_t fetch_width = Max(iq.Size(), FETCH_WIDTH);
	uint32_t fetch_size = Max(fetch_width * sizeof(uint32_t), size_to_block_boundary);
	uint8_t prefetch_buffer[fetch_size];
	uint32_t read_size = 0; //ReadInsnMemory(cia, prefetch_buffer, sizeof(prefetch_buffer) > size_to_block_boundary ? size_to_block_boundary : sizeof(prefetch_buffer));
	uint32_t offset;
	for(offset = 0; offset < read_size; offset += sizeof(uint32_t), cia += sizeof(uint32_t))
	{
		uint32_t insn = prefetch_buffer[offset];
		Instruction<CONFIG> *instruction = AllocateInstruction();
		instruction->Initialize(this, cia, insn);
		iq.Push(instruction);
	}*/
}

template <class CONFIG>
void CPU<CONFIG>::DecodeDispatch()
{
	if(iq.Empty()) return;

	unsigned int num_load_stores = 0;
	unsigned int i, j;
	for(i = 0; i < CONFIG::DECODE_WIDTH; i++)
	{
		// Check for completion queue availability
		if(cq.Full()) return;

		// Lookup the last instruction from the instruction queue
		Instruction<CONFIG> *instruction = iq.Front();

		const typename CONFIG::execution_unit_type_t execution_unit = instruction->GetExecutionUnit();

		// Check for specific dispatch constraints
		switch(execution_unit)
		{
			case CONFIG::NO_UNIT_T: // No Unit
				break;
			case CONFIG::LSU_T:     // Load/Store Unit
				if(num_load_stores >= CONFIG::MAX_DISPATCHED_LOAD_STORES_PER_CYCLE) return;
			case CONFIG::IU1_T:     // Simple Integer Unit
			case CONFIG::IU2_T:     // Complex Integer Unit
				break;
			case CONFIG::FPU_T:     // Floating Point Unit
				break;
			case CONFIG::BPU_T:     // Branch Processing Unit
				// Branches don't go to an issue queue
				break;
			case CONFIG::VFPU_T:    // Vector Floating Point Unit
			case CONFIG::VPU_T:     // vector Processing Unit
			case CONFIG::VIU1_T:    // Simple Vector Integer
			case CONFIG::VIU2_T:    // Complex Vector Integer
				break;
		}

		// Check for issue queue availability
		switch(execution_unit)
		{
			case CONFIG::NO_UNIT_T: // No Unit
				break;
			case CONFIG::LSU_T:     // Load/Store Unit
			case CONFIG::IU1_T:     // Simple Integer Unit
			case CONFIG::IU2_T:     // Complex Integer Unit
				if(giq.Full()) return;
				break;
			case CONFIG::FPU_T:     // Floating Point Unit
				if(fiq.Full()) return;
				break;
			case CONFIG::BPU_T:     // Branch Processing Unit
				// Branches don't go to an issue queue
				break;
			case CONFIG::VFPU_T:    // Vector Floating Point Unit
			case CONFIG::VPU_T:     // vector Processing Unit
			case CONFIG::VIU1_T:    // Simple Vector Integer
			case CONFIG::VIU2_T:    // Complex Vector Integer
				if(viq.Full()) return;
				break;
		}

		// Check for rename registers availability
		unsigned int num_output_fpr = instruction->GetNumOutputFPR();
		if(num_output_fpr && num_output_fpr > fpr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_gpr = instruction->GetNumOutputGPR();
		if(num_output_gpr && num_output_gpr > gpr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_cr = instruction->GetNumOutputCR();
		if(num_output_cr && num_output_cr > cr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_lr = instruction->GetNumOutputLR();
		if(num_output_lr && num_output_lr > lr_mapping_table.GetNumFreeRenameRegisters()) return;

		unsigned int num_output_ctr = instruction->GetNumOutputCTR();
		if(num_output_ctr && num_output_ctr > ctr_mapping_table.GetNumFreeRenameRegisters()) return;

		// Locate input operands
		unisim::component::cxx::processor::powerpc::isa::Operation<CONFIG> *operation = instruction->operation;
		unsigned int num_operands = operation->num_insn_operands;

		for(j = 0; j < num_operands; j++)
		{
			typename CONFIG::operand_t *insn_operand = &operation->insn_operands[i];
	
			if(insn_operand->dir == CONFIG::INPUT_T)
			{
				Operand<CONFIG> *operand = 0;
				bool forwarding = true;

				switch(insn_operand->type)
				{
					case CONFIG::GPR_T:
						{
							unsigned int gpr_reg_num = insn_operand->reg_num;
							int gpr_tag = gpr_mapping_table.Lookup(gpr_reg_num);
							operand = (gpr_tag >= 0) ? arch_gpr[gpr_reg_num] : rename_gpr[~gpr_tag];
						}
						break;

					case CONFIG::GPR_ST_T:
						{
							unsigned int gpr_reg_num = insn_operand->reg_num;
							int gpr_tag = gpr_mapping_table.Lookup(gpr_reg_num);
							operand = (gpr_tag >= 0) ? arch_gpr[gpr_reg_num] : rename_gpr[~gpr_tag];
							forwarding = false; // store value is not forwarded to L/S reservation station
						}
						break;

					case CONFIG::FPR_T:
						{
							unsigned int fpr_reg_num = insn_operand->reg_num;
							int fpr_tag = fpr_mapping_table.Lookup(fpr_reg_num);
							operand = (fpr_tag >= 0) ? arch_fpr[fpr_reg_num] : rename_fpr[~fpr_tag];
						}
						break;
		
					case CONFIG::CR_T:
						{
							int cr_tag = cr_mapping_table.Lookup(0);
							operand = (cr_tag >= 0) ? arch_cr : rename_cr[~cr_tag];
						}
						break;
		
					case CONFIG::LR_T:
						{
							int lr_tag = lr_mapping_table.Lookup(0);
							operand = (lr_tag >= 0) ? arch_lr : rename_lr[~lr_tag];
						}
						break;
		
					case CONFIG::CTR_T:
						{
							int ctr_tag = ctr_mapping_table.Lookup(0);
							operand = (ctr_tag >= 0) ? arch_ctr : rename_ctr[~ctr_tag];
						}
						break;

					default:
						logger << DebugError << "Unhandled operand type" << EndDebugError;
						Stop(-1);
				}

				AcquireOperand(operand);
				instruction->input_operands.Push(operand);
				instruction->forwarding.Push(forwarding);
			}
		}

		// Allocate rename registers for output operands
		for(j = 0; j < num_operands; j++)
		{
			typename CONFIG::operand_t *insn_operand = &operation->insn_operands[i];
	
			if(insn_operand->dir == CONFIG::OUTPUT_T)
			{
				Operand<CONFIG> *operand = AllocateOperand();
				operand->valid = false;

				switch(insn_operand->type)
				{
					case CONFIG::GPR_T:
						{
							operand->type = Operand<CONFIG>::GPR;
							operand->reg_num = insn_operand->reg_num;
							gpr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_gpr[~operand->tag] = operand;
						}
						break;

					case CONFIG::FPR_T:
						{
							operand->type = Operand<CONFIG>::FPR;
							operand->reg_num = insn_operand->reg_num;
							fpr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_fpr[~operand->tag] = operand;
						}
						break;
		
					case CONFIG::CR_T:
						{
							operand->type = Operand<CONFIG>::CR;
							operand->reg_num = 0;
							cr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_cr[~operand->tag] = operand;
						}
						break;
		
					case CONFIG::LR_T:
						{
							operand->type = Operand<CONFIG>::LR;
							operand->reg_num = 0;
							lr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_lr[~operand->tag] = operand;
						}
						break;
		
					case CONFIG::CTR_T:
						{
							operand->type = Operand<CONFIG>::CTR;
							operand->reg_num = 0;
							ctr_mapping_table.Allocate(operand->reg_num, operand->tag);
							rename_ctr[~operand->tag] = operand;
						}
						break;

					case CONFIG::XER_T:
						{
							operand->type = Operand<CONFIG>::XER;
							operand->reg_num = 0;
						}
						break;

					case CONFIG::FPSCR_T:
						{
							operand->type = Operand<CONFIG>::FPSCR;
							operand->reg_num = 0;
						}
						break;

					default:
						logger << DebugError << "Unhandled operand type" << EndDebugError;
						Stop(-1);
				}

				instruction->output_operands.Push(operand);
			}
		}

		// Move the instruction from the instruction queue to completion queue and an issue queue
		switch(execution_unit)
		{
			case CONFIG::NO_UNIT_T: // No Unit
				break;
			case CONFIG::LSU_T:     // Load/Store Unit
				num_load_stores++;
			case CONFIG::IU1_T:     // Simple Integer Unit
			case CONFIG::IU2_T:     // Complex Integer Unit
				giq.Push(instruction);
				break;
			case CONFIG::FPU_T:     // Floating Point Unit
				fiq.Push(instruction);
				break;
			case CONFIG::BPU_T:     // Branch Processing Unit
				// Branches don't go to an issue queue
				break;
			case CONFIG::VFPU_T:    // Vector Floating Point Unit
			case CONFIG::VPU_T:     // vector Processing Unit
			case CONFIG::VIU1_T:    // Simple Vector Integer
			case CONFIG::VIU2_T:    // Complex Vector Integer
				viq.Push(instruction);
				break;
		}

		cq.Push(instruction);

		if(++dispatch_uop_num < instruction->GetNumUOps())
		{
			iq[0] = AllocateInstruction();
			iq[0]->Initialize(this, *instruction, dispatch_uop_num);
		}
		else
		{
			dispatch_uop_num = 0;
			iq.Pop();
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::GPRIssue()
{
	// Issue is Out-of-order
	unsigned int i, j, k;
	for(i = k = 0; i < CONFIG::GPR_ISSUE_WIDTH; i++)
	{
		if(k >= giq.Size()) return;
		Instruction<CONFIG> *instruction = giq[k];

		const typename CONFIG::execution_unit_type_t execution_unit = instruction->GetExecutionUnit();

		switch(execution_unit)
		{
			case CONFIG::LSU_T:     // Load/Store Unit
				if(!lsu_reservation_station.Full())
				{
					lsu_reservation_station.Push(instruction);
					giq.Remove(k);
				}
				else
				{
					k++;
				}
				break;
			case CONFIG::IU1_T:     // Simple Integer Unit
				{
					bool found_iu1 = false;
					for(j = 0; j < CONFIG::NUM_IU1; j++)
					{
						if(!iu1_reservation_station[j].Full())
						{
							iu1_reservation_station[j].Push(instruction);
							found_iu1 = true;
							break;
						}
					}

					if(found_iu1)
					{
						giq.Remove(k);
					}
					else
					{
						k++;
					}
				}
				break;
			case CONFIG::IU2_T:     // Complex Integer Unit
				{
					bool found_iu2 = false;
					for(j = 0; j < CONFIG::NUM_IU2; j++)
					{
						if(!iu2_reservation_station[j].Full())
						{
							iu2_reservation_station[j].Push(instruction);
							found_iu2 = true;
							break;
						}
					}

					if(found_iu2)
					{
						giq.Remove(k);
					}
					else
					{
						k++;
					}
				}
				break;

			default:
				logger << DebugError << "Unhandled execution unit type" << EndDebugError;
				Stop(-1);

		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::FPRIssue()
{
}

template <class CONFIG>
void CPU<CONFIG>::VRIssue()
{
}

template <class CONFIG>
void CPU<CONFIG>::IU1Execute()
{
	unsigned int i;
	for(i = 0; i < CONFIG::NUM_IU1; i++)
	{
		if(!iu1_reservation_station[i].Empty())
		{
			Instruction<CONFIG> *instruction = iu1_reservation_station[i].Front();

			if(instruction->IsAllowedToExecute() && instruction->IsReadyToExecute())
			{
				instruction->Execute(this);

				NotifyFinishedInstruction(instruction, instruction->GetLatency());
				iu1_reservation_station[i].Pop();
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::IU2Execute()
{
	unsigned int i;
	for(i = 0; i < CONFIG::NUM_IU2; i++)
	{
		if(!iu2_reservation_station[i].Empty())
		{
			Instruction<CONFIG> *instruction = iu2_reservation_station[i].Front();

			if(instruction->IsAllowedToExecute() && instruction->IsReadyToExecute())
			{
				instruction->Execute(this);
				NotifyFinishedInstruction(instruction, instruction->GetLatency());
				iu2_reservation_station[i].Pop();
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::FPUExecute()
{
}

template <class CONFIG>
void CPU<CONFIG>::LSUExecute1()
{
	if(!lsu_reservation_station.Empty())
	{
		Instruction<CONFIG> *instruction = lsu_reservation_station.Front();

		if(!instruction->IsAllowedToExecute()) return;
		if(!instruction->IsReadyToExecute()) return;
		if(required_ea.Full()) return;

		instruction->Execute(this);
		required_ea.Push(instruction);
		lsu_reservation_station.Pop();
	}
}

// 		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
// 		{
// 			unsigned int num_load_store_accesses = instruction->load_store_access_queue.Size();
// 			NotifyFinishedInstruction(instruction, num_load_store_accesses);
// 		}
// 		else
// 		{


template <class CONFIG>
void CPU<CONFIG>::LSUExecute2()
{
	unsigned int lat = 0;

	if(!required_ea.Empty())
	{
		Instruction<CONFIG> *instruction = required_ea.Front();

		LoadStoreAccess<CONFIG> *load_store_access = instruction->load_store_access_queue[instruction->load_store_access_index];
	
		load_store_access->mmu_access.addr = load_store_access->munged_ea;
		load_store_access->mmu_access.privilege_level = GetPrivilegeLevel();
		load_store_access->mmu_access.memory_type = CONFIG::MT_DATA;
		
		if(GetMSR_DR())
		{
			// BATs and TLB Lookup
			load_store_access->mmu_access.memory_access_type = (load_store_access->type & LoadStoreAccess<CONFIG>::STORE) ? CONFIG::MAT_WRITE : CONFIG::MAT_READ;
	
			LookupBAT<false>(load_store_access->mmu_access);
			if(!load_store_access->mmu_access.bat_hit)
			{
				AccessTLB<false>(load_store_access->mmu_access);
					
				if(!load_store_access->mmu_access.tlb_hit || load_store_access->mmu_access.force_page_table_walk)
				{
					// BAT/TLB Miss
					abort(); // Hardware page table search should start at commit
				}
			}
		}
		else
		{
			// W=0: Write through disabled
			// I=0: Cache not inhibited
			// M=1: Memory coherency enforced
			// G=1: Guarded memory access (speculative fetch forbidden)
			load_store_access->mmu_access.wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
			load_store_access->mmu_access.physical_addr = load_store_access->munged_ea;
		}

		if(load_store_access->type & LoadStoreAccess<CONFIG>::LOAD)
		{
			// Load
			if(!(load_store_access->mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled())
			{
				// DL1 Access
				load_store_access->l1_access.addr = load_store_access->mmu_access.physical_addr;
				LookupDL1(load_store_access->l1_access);
				lat += CONFIG::DL1_LATENCY;
	
				if(load_store_access->l1_access.block)
				{
					// Load hit in DL1
					memcpy(load_store_access->data, &(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->size);
					UpdateReplacementPolicyDL1(load_store_access->l1_access);
					NotifyLoadResultAvailability(load_store_access, lat);
				}
				else
				{
					// Load miss in DL1
					if(num_outstanding_l1_load_miss >= CONFIG::MAX_OUTSTANDING_L1_LOAD_MISS) return;
		
					if(IsL2CacheEnabled())
					{
						// DL1 block fill from L2
						load_store_access->l2_access.addr = load_store_access->l1_access.block_base_addr;
					
						LookupL2(load_store_access->l2_access);
						lat += CONFIG::L2_LATENCY;
	
						if(load_store_access->l2_access.block)
						{
							// DL1 block fill hit in L2
							memcpy(&(*load_store_access->l1_access.block)[0], &(*load_store_access->l2_access.block)[load_store_access->l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
							UpdateReplacementPolicyL2(load_store_access->l2_access);
							memcpy(load_store_access->data, &(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->size);
							UpdateReplacementPolicyDL1(load_store_access->l1_access);
							NotifyLoadResultAvailability(load_store_access, lat);
						}
						else
						{
							// Load miss in L2
							if(num_outstanding_l2_load_miss >= CONFIG::MAX_OUTSTANDING_L2_LOAD_MISS) return;
	
							// L2 block fill from bus
							load_store_access->l2_access.block = &(*load_store_access->l2_access.line)[load_store_access->l2_access.sector];
		
							BusAccess<CONFIG> *bus_access = AllocateBusAccess();
							bus_access->issued = false;
							bus_access->type = BusAccess<CONFIG>::REFILL;
							bus_access->addr = load_store_access->l2_access.block_base_addr;
							bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
							bus_access->load_store_access = load_store_access;
							bus_access->fetch_access = 0;
							bus_access->wimg = load_store_access->mmu_access.wimg;
							bus_access->rwitm = false;
							NotifyBusAccess(bus_access, lat);
						}
					}
					else
					{
						// DL1 block fill from bus
						BusAccess<CONFIG> *bus_access = AllocateBusAccess();
						bus_access->issued = false;
						bus_access->type = BusAccess<CONFIG>::REFILL;
						bus_access->addr = load_store_access->l1_access.block_base_addr;
						bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
						bus_access->load_store_access = load_store_access;
						bus_access->fetch_access = 0;
						bus_access->wimg = load_store_access->mmu_access.wimg;
						bus_access->rwitm = false;
						NotifyBusAccess(bus_access, lat);
					}
				}
			}
			else
			{
				// Caching is either disabled or inhibited
				BusAccess<CONFIG> *bus_access = AllocateBusAccess();
				bus_access->issued = false;
				bus_access->type = BusAccess<CONFIG>::LOAD;
				bus_access->addr = load_store_access->mmu_access.physical_addr;
				bus_access->size = load_store_access->size;
				bus_access->load_store_access = load_store_access;
				bus_access->fetch_access = 0;
				bus_access->wimg = load_store_access->mmu_access.wimg;
				bus_access->rwitm = false;
				NotifyBusAccess(bus_access, lat);
			}
		}

		if(++instruction->load_store_access_index >= instruction->load_store_access_queue.Size())
		{
			required_ea.Pop();

			if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
			{
				instruction->load_store_access_index = 0;
				NotifyFinishedInstruction(instruction, 1);
			}
			else
			{
				return;
			}
		}
	}

	if(!csq.Empty())
	{
		Instruction<CONFIG> *instruction = csq.Front();

		LoadStoreAccess<CONFIG> *load_store_access = instruction->load_store_access_queue[instruction->load_store_access_index];

		switch(load_store_access->type)
		{
			case LoadStoreAccess<CONFIG>::INT8_STORE:
				{
					uint8_t value = instruction->GetGPR(load_store_access->reg_num);
					memcpy(load_store_access->data, &value, sizeof(value));
				}
				break;
			case LoadStoreAccess<CONFIG>::INT16_STORE:
				{
					uint16_t value = Host2BigEndian((uint16_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT32_STORE:
				{
					uint32_t value = Host2BigEndian((uint32_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::FP32_STORE:
				{
					Flags flags;
					flags.setRoundingMode((fpscr & CONFIG::FPSCR_RN_MASK) >> CONFIG::FPSCR_RN_OFFSET);
					uint32_t value = Host2BigEndian(SoftFloat(instruction->GetFPR(load_store_access->reg_num), flags).queryValue());
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::FP64_STORE:
				{
					uint64_t value = Host2BigEndian(instruction->GetFPR(load_store_access->reg_num).queryValue());
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::FP_STORE_LSW:
				{
					uint32_t value = Host2BigEndian((uint32_t) instruction->GetFPR(load_store_access->reg_num).queryValue());
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE:
				{
					uint16_t value = Host2LittleEndian((uint16_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE:
				{
					uint32_t value = Host2LittleEndian((uint32_t) instruction->GetGPR(load_store_access->reg_num));
					memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
				}
				break;
			case LoadStoreAccess<CONFIG>::INT_STORE_MSB:
				switch(load_store_access->size)
				{
					case 1:
						{
							uint8_t value = instruction->GetGPR(load_store_access->reg_num) >> 24;
							memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
						}
						break;
					case 2:
						{
							uint16_t value = Host2BigEndian((uint16_t)(instruction->GetGPR(load_store_access->reg_num) >> 16));
							memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
							break;
						}
			
					case 3:
						{
							uint32_t value = instruction->GetGPR(load_store_access->reg_num);
							uint8_t buffer[3];
							buffer[0] = value >> 24;
							buffer[1] = value >> 16;
							buffer[2] = value >> 8;
							memcpy(load_store_access->data, buffer + load_store_access->offset, load_store_access->size);
							break;
						}
			
					case 4:
						{
							uint32_t value = Host2BigEndian(instruction->GetGPR(load_store_access->reg_num));
							memcpy(load_store_access->data, (uint8_t *) &value + load_store_access->offset, load_store_access->size);
							break;
						}
				}
				break;

			default:
				logger << DebugError << "Unhandled load/store access type" << EndDebugError;
				Stop(-1);
		}

		if(!(load_store_access->mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) && IsDataCacheEnabled())
		{
			// DL1 Access
			load_store_access->l1_access.addr = load_store_access->mmu_access.physical_addr;
			LookupDL1(load_store_access->l1_access);
			lat += CONFIG::DL1_LATENCY;

			if(load_store_access->l1_access.block)
			{
				// Store hit in DL1
				memcpy(&(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->data, load_store_access->size);
				load_store_access->l1_access.block->status.dirty = true;
				UpdateReplacementPolicyDL1(load_store_access->l1_access);
			}
			else
			{
				// Store miss in DL1
				if(num_outstanding_l1_store_miss >= CONFIG::MAX_OUTSTANDING_L1_STORE_MISS) return;
	
				if(IsL2CacheEnabled())
				{
					// DL1 block fill from L2
					load_store_access->l2_access.addr = load_store_access->l1_access.block_base_addr;
				
					LookupL2(load_store_access->l2_access);
					lat += CONFIG::L2_LATENCY;
	
					if(load_store_access->l2_access.block)
					{
						// DL1 block fill hit in L2
						memcpy(&(*load_store_access->l1_access.block)[0], &(*load_store_access->l2_access.block)[load_store_access->l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
						UpdateReplacementPolicyL2(load_store_access->l2_access);
						memcpy(&(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->data, load_store_access->size);
						UpdateReplacementPolicyDL1(load_store_access->l1_access);
					}
					else
					{
						// Store miss in L2
						if(num_outstanding_l2_store_miss >= CONFIG::MAX_OUTSTANDING_L2_STORE_MISS) return;
	
						// L2 block fill from bus
						load_store_access->l2_access.block = &(*load_store_access->l2_access.line)[load_store_access->l2_access.sector];
	
						BusAccess<CONFIG> *bus_access = AllocateBusAccess();
						bus_access->issued = false;
						bus_access->type = BusAccess<CONFIG>::REFILLX;
						bus_access->addr = load_store_access->l2_access.block_base_addr;
						bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
						bus_access->load_store_access = load_store_access;
						bus_access->fetch_access = 0;
						bus_access->wimg = load_store_access->mmu_access.wimg;
						bus_access->rwitm = false;
						NotifyBusAccess(bus_access, lat);
					}
				}
				else
				{
					// DL1 block fill from bus
					BusAccess<CONFIG> *bus_access = AllocateBusAccess();
					bus_access->issued = false;
					bus_access->type = BusAccess<CONFIG>::REFILLX;
					bus_access->addr = load_store_access->l1_access.block_base_addr;
					bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
					bus_access->load_store_access = load_store_access;
					bus_access->fetch_access = 0;
					bus_access->wimg = load_store_access->mmu_access.wimg;
					bus_access->rwitm = false;
					NotifyBusAccess(bus_access, lat);
				}
			}
		}
		else
		{
			// Caching is either disabled or inhibited
			BusAccess<CONFIG> *bus_access = AllocateBusAccess();
			bus_access->issued = false;
			bus_access->type = BusAccess<CONFIG>::STORE;
			bus_access->addr = load_store_access->mmu_access.physical_addr;
			bus_access->size = load_store_access->size;
			bus_access->load_store_access = load_store_access;
			bus_access->fetch_access = 0;
			bus_access->wimg = load_store_access->mmu_access.wimg;
			bus_access->rwitm = false;
			NotifyBusAccess(bus_access, lat);
		}
		csq.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::ScheduleEvents()
{
	while(!schedule.empty())
	{
		typename std::multimap<uint64_t, Event<CONFIG> *>::iterator it = schedule.begin();
		if(it->first > cpu_cycle) return;

		Event<CONFIG> *ev = it->second;

		switch(ev->type)
		{
			case Event<CONFIG>::EV_NULL:
				break;
			case Event<CONFIG>::EV_FINISHED_INSN:     // an instruction is finished
				{
					Instruction<CONFIG> *instruction = ev->object.instruction;

					instruction->Finish();
					if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
					{
						if(!fsq.Full())
						{
							fsq.Push(instruction);
							required_ea.Pop();
						}
					}
				}
				break;
			case Event<CONFIG>::EV_AVAILABLE_OPERAND: // an operand is available
				{
					Operand<CONFIG> *operand = ev->object.operand;

					operand->valid = true;
					
					unsigned int num_pending_instructions = operand->pending_instructions.Size();
					unsigned int i, j;
			
					for(i = j = 0; i < num_pending_instructions; i++)
					{
						Instruction<CONFIG> *instruction = operand->pending_instructions[j];
						
						if(instruction->CheckInputOperandsAvailability())
						{
							operand->pending_instructions.Remove(j);
						}
						else
						{
							j++;
						}
					}
				}
				break;
			case Event<CONFIG>::EV_BUS_ACCESS:        // a cache miss causes a bus access
				{
					BusAccess<CONFIG> *bus_access = ev->object.bus_access;

					switch(bus_access->type)
					{
						case BusAccess<CONFIG>::LOAD:     // cache inhibited load
						case BusAccess<CONFIG>::REFILL:   // cache refill
						case BusAccess<CONFIG>::REFILLX:  // cache refill with intent to modify
							if(!blq.Full())
								blq.Push(bus_access);
							else
								NotifyEvent(ev, 1); // Reschedule event on next cycle
							break;
						case BusAccess<CONFIG>::STORE:    // write through/cache inhibited store
						case BusAccess<CONFIG>::EVICTION: // block eviction
							if(!bsq.Full())
								bsq.Push(bus_access);
							else
								NotifyEvent(ev, 1); // Reschedule event on next cycle
							break;
					}
				}
				break;
				
			case Event<CONFIG>::EV_FINISHED_FETCH:     // a fetch has finished
				{
					FetchAccess<CONFIG> *fetch_access = ev->object.fetch_access;
					unsigned int offset;
					unsigned int i;
					
					for(offset = 0, i = 0; !iq.Full() && offset < fetch_access->size; offset += 4, i++)
					{
						uint32_t insn = Host2BigEndian(*(uint32_t *) &fetch_access->data[offset]);
						Instruction<CONFIG> *instruction = AllocateInstruction();
						instruction->Initialize(this, fetch_access->cia + i, insn);
						iq.Push(instruction);
					}
					FreeFetchAccess(fetch_access);
				}
				break;
		}

		schedule.erase(it);
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpSchedule()
{
	stringstream sstr;
	typename std::multimap<uint64_t, Event<CONFIG> *>::iterator it;
	
	for(it = schedule.begin(); it != schedule.end(); it++)
	{
		Event<CONFIG> *ev = it->second;
		
		sstr << "EVENT(time=" << it->first << ",";
		
		switch(ev->type)
		{
			case Event<CONFIG>::EV_NULL:
				sstr << "null";
				break;
			case Event<CONFIG>::EV_FINISHED_INSN:     // an instruction is finished
				{
					Instruction<CONFIG> *instruction = ev->object.instruction;

					sstr << *instruction;
				}
				break;
			case Event<CONFIG>::EV_AVAILABLE_OPERAND: // an operand is available
				{
					Operand<CONFIG> *operand = ev->object.operand;

					sstr << *operand;
				}
				break;
			case Event<CONFIG>::EV_BUS_ACCESS:        // a cache miss causes a bus access
				{
					BusAccess<CONFIG> *bus_access = ev->object.bus_access;
					sstr << *bus_access;
				}
				break;
				
			case Event<CONFIG>::EV_FINISHED_FETCH:     // a fetch has finished
				{
					FetchAccess<CONFIG> *fetch_access = ev->object.fetch_access;
					sstr << *fetch_access;
				}
				break;
		}
		sstr << ")" << endl;
	}
	
	logger << sstr.str();
}

template <class CONFIG>
bool CPU<CONFIG>::HasPipelineCollision(BusAccess<CONFIG> *bus_load_access)
{
	unsigned int i;
	unsigned int bsq_size = bsq.Size();
	for(i = 0; i < bsq_size; i++)
	{
		BusAccess<CONFIG> *bus_store_access = bsq[i];
		if(bus_load_access->addr == bus_store_access->addr) return true;
	}

	return false;
}

template <class CONFIG>
void CPU<CONFIG>::BIU()
{
	if(!blq.Empty())
	{
		BusAccess<CONFIG> *bus_access = blq.Front();
		if(!bus_access->issued && !HasPipelineCollision(bus_access))
		{
			bus_access->issued = true;
			DoBusAccess(bus_access);
			return;
		}
	}

	if(!bsq.Empty())
	{
		BusAccess<CONFIG> *bus_access = bsq.Front();
		if(!bus_access->issued)
		{
			bus_access->issued = true;
			DoBusAccess(bus_access);
		}
	}
}


template <class CONFIG>
void CPU<CONFIG>::OnFinishedBusAccess(BusAccess<CONFIG> *bus_access)
{
	if(bus_access->fetch_access)
	{
		FetchAccess<CONFIG> *fetch_access = bus_access->fetch_access;
		unsigned int lat = 0;
		
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
				if(likely(IsInsnCacheEnabled()) && !fetch_access->l1_access.line)
				{
					// IL1 Line miss
					ChooseLineToEvictIL1(fetch_access->l1_access);
					
					uint32_t l1_sector;
					
					for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::IL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
					{
						CacheBlock<class CONFIG::IL1_CONFIG>& l1_block_to_evict = (*fetch_access->l1_access.line_to_evict)[l1_sector];
						l1_block_to_evict.status.valid = false;
					}
					
					fetch_access->l1_access.line_to_evict->status.valid = false;
					fetch_access->l1_access.line = fetch_access->l1_access.line_to_evict;
					fetch_access->l1_access.line_to_evict = 0;
					fetch_access->l1_access.block = &(*fetch_access->l1_access.line)[fetch_access->l1_access.sector];				
				}

				if(likely(IsL2CacheEnabled()) && !fetch_access->l2_access.line)
				{
					// IL1 Line miss
					ChooseLineToEvictL2(fetch_access->l2_access);
					// L2 Line eviction
					uint32_t l2_sector;
				
					for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
					{
						CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_evict = (*fetch_access->l2_access.line_to_evict)[l2_sector];
				
						l2_block_to_evict.status.valid = false;
						l2_block_to_evict.status.dirty = false;
					}
					
					fetch_access->l2_access.line_to_evict->status.valid = false;
					fetch_access->l2_access.line = fetch_access->l2_access.line_to_evict;
					fetch_access->l2_access.line_to_evict = 0;
					fetch_access->l2_access.block = &(*fetch_access->l2_access.line)[fetch_access->l2_access.sector];				
				}
				
				num_outstanding_l1_fetch_miss--;
				break;

			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize cache refills
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
				if(likely(IsL2CacheEnabled()) && fetch_access->l2_access.line)
				{
					if(!fetch_access->l2_access.block)
					{
						logger << DebugError << "PANIC ! null block" << EndDebugError;
						Stop(-1);
					}
					logger << DebugInfo << "block=0x" << std::hex << fetch_access->l2_access.block << std::dec << EndDebugInfo;
					// Finalize L2 Refill
					memcpy(&(*fetch_access->l2_access.block)[0], bus_access->storage, CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
					fetch_access->l2_access.line->status.valid = true;
					fetch_access->l2_access.line->SetBaseAddr(fetch_access->l2_access.line_base_addr);
					fetch_access->l2_access.block->status.valid = true;
					fetch_access->l2_access.block->status.dirty = false;
				}

				if(likely(IsInsnCacheEnabled()) && fetch_access->l1_access.line)
				{
					// Finalize IL1 refill
					memcpy(&(*fetch_access->l1_access.block)[0], &(*fetch_access->l2_access.block)[fetch_access->l2_access.offset], CacheBlock<class CONFIG::IL1_CONFIG>::SIZE);
					UpdateReplacementPolicyL2(fetch_access->l2_access);
					fetch_access->l1_access.line->status.valid = true;
					fetch_access->l1_access.line->SetBaseAddr(fetch_access->l1_access.line_base_addr);
					fetch_access->l1_access.block->status.valid = true;
				}
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize Fetch access
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::LOAD:
				// Finalize fetch from bus and forward raw data to IQ
				memcpy(fetch_access->data, bus_access->storage, fetch_access->size);
				NotifyFinishedFetch(fetch_access, lat);
				break;
			case BusAccess<CONFIG>::REFILL:
				if(likely(IsInsnCacheEnabled()) && fetch_access->l1_access.line)
				{
					// Finalize fetch from IL1 and forward raw data to IQ
					memcpy(fetch_access->data, &(*fetch_access->l1_access.block)[fetch_access->l1_access.offset], fetch_access->size);
					UpdateReplacementPolicyIL1(fetch_access->l1_access);
					NotifyFinishedFetch(fetch_access, lat);
				}
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}
	}
	
	if(bus_access->load_store_access)
	{
		LoadStoreAccess<CONFIG> *load_store_access = bus_access->load_store_access;
		unsigned int lat = 0;

		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
			case BusAccess<CONFIG>::REFILLX:
				if(!load_store_access->l1_access.line)
				{
					// DL1 Line miss
					ChooseLineToEvictDL1(load_store_access->l1_access);
					
					if(load_store_access->l1_access.line_to_evict->status.valid)
					{
						// DL1 Line eviction
						uint32_t l1_sector;
					
						for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
						{
							CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_evict = (*load_store_access->l1_access.line_to_evict)[l1_sector];
					
							if(l1_block_to_evict.status.valid && l1_block_to_evict.status.dirty)
							{
								// dirty DL1 block eviction
								if(IsL2CacheEnabled())
								{
									// DL1 block eviction into L2
									CacheAccess<class CONFIG::L2_CONFIG> l2_access;
									l2_access.addr = l1_block_to_evict.GetBaseAddr();
									LookupL2(l2_access);
									lat += CONFIG::L2_LATENCY;
				
									if(l2_access.block)
									{
										// DL1 block eviction hit in L2
										memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_evict[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
										l2_access.block->status.dirty = true;
					
										UpdateReplacementPolicyL2(l2_access);
									}
									else
									{
										// dirty DL1 block eviction miss in L2
										// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
										BusAccess<CONFIG> *bus_access = AllocateBusAccess();
										bus_access->issued = false;
										bus_access->type = BusAccess<CONFIG>::EVICTION;
										bus_access->addr = l1_block_to_evict.GetBaseAddr();
										bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
										memcpy(bus_access->storage, &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
										bus_access->load_store_access = load_store_access;
										bus_access->fetch_access = 0;
										NotifyBusAccess(bus_access, lat);
									}
								}
								else
								{
									// dirty DL1 block eviction into memory
									// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
									BusAccess<CONFIG> *bus_access = AllocateBusAccess();
									bus_access->issued = false;
									bus_access->type = BusAccess<CONFIG>::EVICTION;
									bus_access->addr = l1_block_to_evict.GetBaseAddr();
									bus_access->size = CacheBlock<class CONFIG::DL1_CONFIG>::SIZE;
									memcpy(bus_access->storage, &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
									bus_access->load_store_access = load_store_access;
									bus_access->fetch_access = 0;
									NotifyBusAccess(bus_access, lat);
								}
							}
							l1_block_to_evict.status.valid = false;
							l1_block_to_evict.status.dirty = false;
						}
						load_store_access->l1_access.line_to_evict->status.valid = false;
					}
					load_store_access->l1_access.line = load_store_access->l1_access.line_to_evict;
					load_store_access->l1_access.line_to_evict = 0;
				}

				if(!load_store_access->l2_access.line)
				{
					if(load_store_access->l2_access.line_to_evict->status.valid)
					{
						// L2 Line eviction
						uint32_t l2_sector;
					
						for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
						{
							CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_evict = (*load_store_access->l2_access.line_to_evict)[l2_sector];
					
							if(l2_block_to_evict.status.valid && l2_block_to_evict.status.dirty)
							{
								// dirty L2 block eviction into memory
								// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
								BusAccess<CONFIG> *bus_access = AllocateBusAccess();
								bus_access->issued = false;
								bus_access->type = BusAccess<CONFIG>::EVICTION;
								bus_access->addr = l2_block_to_evict.GetBaseAddr();
								bus_access->size = CacheBlock<class CONFIG::L2_CONFIG>::SIZE;
								memcpy(bus_access->storage, &l2_block_to_evict[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
								bus_access->load_store_access = load_store_access;
								bus_access->fetch_access = 0;
								NotifyBusAccess(bus_access, lat);
							}
							l2_block_to_evict.status.valid = false;
							l2_block_to_evict.status.dirty = false;
						}
					
						load_store_access->l2_access.line_to_evict->status.valid = false;
					}
					load_store_access->l2_access.line = load_store_access->l2_access.line_to_evict;
					load_store_access->l2_access.line_to_evict = 0;
				}

				break;
			case BusAccess<CONFIG>::STORE:
			case BusAccess<CONFIG>::EVICTION:
				FreeInstruction(load_store_access->instruction);
				bsq.Pop();
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize cache refills
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::REFILL:
			case BusAccess<CONFIG>::REFILLX:
				// Finalize L2 Refill
				memcpy(&(*load_store_access->l2_access.block)[0], bus_access->storage, CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
				load_store_access->l2_access.line->status.valid = true;
				load_store_access->l2_access.line->SetBaseAddr(load_store_access->l2_access.line_base_addr);
				load_store_access->l2_access.block->status.valid = true;
				load_store_access->l2_access.block->status.dirty = false;

				// Finalize DL1 refill
				memcpy(&(*load_store_access->l1_access.block)[0], &(*load_store_access->l2_access.block)[load_store_access->l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				UpdateReplacementPolicyL2(load_store_access->l2_access);
				load_store_access->l1_access.line->status.valid = true;
				load_store_access->l1_access.line->SetBaseAddr(load_store_access->l1_access.line_base_addr);
				load_store_access->l1_access.block->status.valid = true;
				load_store_access->l1_access.block->status.dirty = false;
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}

		// Finalize Load/Store access
		switch(bus_access->type)
		{
			case BusAccess<CONFIG>::LOAD:
			case BusAccess<CONFIG>::REFILL:
				// Finalize load in DL1 and forward load result
				memcpy(load_store_access->data, &(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->size);
				UpdateReplacementPolicyDL1(load_store_access->l1_access);
				NotifyLoadResultAvailability(load_store_access, lat);
				NotifyFinishedInstruction(load_store_access->instruction, lat);
				blq.Pop();
				break;
			case BusAccess<CONFIG>::REFILLX:
			case BusAccess<CONFIG>::STORE:
				// Finalize store in DL1
				memcpy(&(*load_store_access->l1_access.block)[load_store_access->l1_access.offset], load_store_access->data, load_store_access->size);
				UpdateReplacementPolicyDL1(load_store_access->l1_access);
				bsq.Pop();
				break;
			default:
				logger << DebugError << "Unhandled bus access type" << EndDebugError;
				Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::NotifyEvent(Event<CONFIG> *ev, uint64_t latency)
{
	schedule.insert(std::pair<uint64_t, Event<CONFIG> *>(cpu_cycle + latency, ev));
}

template <class CONFIG>
void CPU<CONFIG>::NotifyBusAccess(BusAccess<CONFIG> *bus_access, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_BUS_ACCESS;
	ev->object.bus_access = bus_access;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyFinishedInstruction(Instruction<CONFIG> *instruction, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_FINISHED_INSN;
	ev->object.instruction = instruction;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyOperandAvailability(Operand<CONFIG> *operand, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_AVAILABLE_OPERAND;
	ev->object.operand = operand;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyFinishedFetch(FetchAccess<CONFIG> *fetch_access, uint64_t latency)
{
	Event<CONFIG> *ev = AllocateEvent();
	ev->type = Event<CONFIG>::EV_FINISHED_FETCH;
	ev->object.fetch_access = fetch_access;
	NotifyEvent(ev, latency);
}

template <class CONFIG>
void CPU<CONFIG>::NotifyLoadResultAvailability(LoadStoreAccess<CONFIG> *load_store_access, uint64_t latency)
{
	Instruction<CONFIG> *instruction = load_store_access->instruction;

	unsigned int i;
	unsigned int num_load_store_accesses = instruction->load_store_access_queue.Size();
	uint8_t data[8];

	for(i = 0; i < num_load_store_accesses; i++)
	{
		if(!load_store_access->valid) return;
		memcpy(data + load_store_access->offset, load_store_access->data, load_store_access->size);
	}

	switch(load_store_access->type)
	{
		case LoadStoreAccess<CONFIG>::INT8_LOAD:
			{
				uint8_t value = *(uint8_t *) data;
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) value, latency); // 8-bit to 32-bit zero extension
			}
			break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD:
			{
				uint16_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) BigEndian2Host(value), latency); // 16-bit to 32-bit zero extension
			}
			break;
		case LoadStoreAccess<CONFIG>::SINT16_LOAD:
			{
				uint16_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) (int16_t) BigEndian2Host(value), latency); // 16-bit to 32-bit sign extension
			}
			break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD:
			{
				uint32_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, BigEndian2Host(value), latency);
			}
			break;
		case LoadStoreAccess<CONFIG>::FP32_LOAD:
			{
				uint32_t value;
				memcpy(&value, data, sizeof(value));
				Flags flags;
				flags.setRoundingMode(RN_ZERO);
				load_store_access->instruction->SetFPR(load_store_access->reg_num, SoftDouble(SoftFloat(BigEndian2Host(value)), flags), latency);
			}
			break;
		case LoadStoreAccess<CONFIG>::FP64_LOAD:
			{
				uint64_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetFPR(load_store_access->reg_num, SoftDouble(BigEndian2Host(value)), latency);
			}
			break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE:
			{
				uint16_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) LittleEndian2Host(value), latency); // reverse bytes and 16-bit to 32-bit zero extension
			}
			break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE:
			{
				uint32_t value;
				memcpy(&value, data, sizeof(value));
				load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) LittleEndian2Host(value), latency); // reverse bytes
			}
			break;
		case LoadStoreAccess<CONFIG>::INT_LOAD_MSB:
			switch(load_store_access->size)
			{
				case 1:
				{
					uint8_t value = *data;
					load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) value << 24, latency);
					break;
				}
		
				case 2:
				{
					uint16_t value;
					memcpy(&value, data, sizeof(value));
					load_store_access->instruction->SetGPR(load_store_access->reg_num, (uint32_t) BigEndian2Host(value) << 16, latency);
					break;
				}
		
				case 3:
				{
					uint8_t *buffer = data;
					load_store_access->instruction->SetGPR(load_store_access->reg_num, ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8), latency);
					break;
				}
		
				case 4:
				{
					uint32_t value;
					memcpy(&value, data, sizeof(value));
					load_store_access->instruction->SetGPR(load_store_access->reg_num, BigEndian2Host(value), latency);
					break;
				}
			}
			break;
		default:
			logger << DebugError << "Unhandled load/store access type" << EndDebugError;
			Stop(-1);
	}

	NotifyFinishedInstruction(load_store_access->instruction, 1);
}

template <class CONFIG>
void CPU<CONFIG>::Complete()
{
	if(cq.Empty()) return;

	unsigned int i;
	unsigned int num_gpr_wb_ports = 0;
	unsigned int num_fpr_wb_ports = 0;
	unsigned int num_cr_wb_ports = 0;
	unsigned int num_lr_wb_ports = 0;
	unsigned int num_ctr_wb_ports = 0;
	for(i = 0; i < CONFIG::COMPLETE_WIDTH; i++)
	{
		// Lookup the last instruction from the instruction queue
		Instruction<CONFIG> *instruction = cq.Front();

		if(!instruction->IsFinished())
		{
			if(!instruction->IsAllowedToExecute() && (instruction->GetSerialization() & CONFIG::EXECUTION_SERIALIZATION))
			{
				instruction->AllowExecution();
			}
			return;
		}

		// Check exception flags

		// check for store
		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
		{
			
		}

		// Check write back constraints
		if(wb0.Full()) return;

		unsigned int num_output_gpr = instruction->GetNumOutputGPR();
		num_gpr_wb_ports += num_output_gpr;
		if(num_gpr_wb_ports > CONFIG::MAX_GPR_WB_PER_CYCLE) return;

		unsigned int num_output_fpr = instruction->GetNumOutputFPR();
		num_fpr_wb_ports += num_output_fpr;
		if(num_fpr_wb_ports > CONFIG::MAX_FPR_WB_PER_CYCLE) return;

		unsigned int num_output_cr = instruction->GetNumOutputCR();
		num_cr_wb_ports += num_output_cr;
		if(num_cr_wb_ports > CONFIG::MAX_CR_WB_PER_CYCLE) return;

		unsigned int num_output_lr = instruction->GetNumOutputLR();
		num_lr_wb_ports += num_output_lr;
		if(num_lr_wb_ports > CONFIG::MAX_LR_WB_PER_CYCLE) return;

		unsigned int num_output_ctr = instruction->GetNumOutputCTR();
		num_ctr_wb_ports += num_output_ctr;
		if(num_ctr_wb_ports > CONFIG::MAX_CTR_WB_PER_CYCLE) return;
	
		// Instruction Retire
		wb0.Push(instruction);
		cq.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::WriteBack0()
{
	while(!wb0.Empty())
	{
		Instruction<CONFIG> *instruction = wb0.Front();

		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
		{
			if(wb1.Full()) return;
		}

		unsigned int num_output_operands = instruction->output_operands.Size();
		unsigned int j;

		for(j = 0; j < num_output_operands; j++)
		{
			Operand<CONFIG> *operand = instruction->output_operands[j];

			switch(operand->type)
			{
				case Operand<CONFIG>::GPR:
					FreeOperand(arch_gpr[operand->reg_num]);
					arch_gpr[operand->reg_num] = operand;
					rename_gpr[~operand->tag] = 0;
					gpr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = operand->reg_num;
					break;
				case Operand<CONFIG>::FPR:
					FreeOperand(arch_fpr[operand->reg_num]);
					arch_fpr[operand->reg_num] = operand;
					rename_fpr[~operand->tag] = 0;
					fpr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = operand->reg_num;
					break;
				case Operand<CONFIG>::CR:
					FreeOperand(arch_cr);
					arch_cr = operand;
					rename_cr[~operand->tag] = 0;
					cr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = 0;
					break;
				case Operand<CONFIG>::LR:
					FreeOperand(arch_lr);
					arch_lr = operand;
					rename_lr[~operand->tag] = 0;
					lr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = 0;
					break;
				case Operand<CONFIG>::CTR:
					FreeOperand(arch_ctr);
					arch_ctr = operand;
					rename_ctr[~operand->tag] = 0;
					ctr_mapping_table.Free(operand->reg_num, operand->tag);
					operand->tag = 0;
					break;
				case Operand<CONFIG>::XER:
					FreeOperand(arch_xer);
					arch_xer = operand;
					break;
				case Operand<CONFIG>::FPSCR:
					FreeOperand(arch_fpscr);
					arch_fpscr = operand;
					break;
			}
		}
		
		if(instruction->GetSerialization() & CONFIG::STORE_SERIALIZATION)
		{
			wb1.Push(instruction);
		}
		else
		{
			FreeInstruction(instruction);
		}
		wb0.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::WriteBack1()
{
	while(!wb1.Empty())
	{
		Instruction<CONFIG> *instruction = wb1.Front();

		if(csq.Full()) return;

		csq.Push(instruction);
		fsq.Pop();
		wb1.Pop();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Flush()
{
	unsigned int i;

	if(!iq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = iq.Front();
			FreeInstruction(instruction);
			iq.Pop();
		} while(!iq.Empty());
	}

	if(!giq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = giq.Front();
			FreeInstruction(instruction);
			giq.Pop();
		} while(!giq.Empty());
	}

	if(!fiq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = fiq.Front();
			FreeInstruction(instruction);
			fiq.Pop();
		} while(!fiq.Empty());
	}

	if(!viq.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = viq.Front();
			FreeInstruction(instruction);
			viq.Pop();
		} while(!viq.Empty());
	}

	for(i = 0; i < CONFIG::NUM_IU1; i++)
	{
		if(!iu1_reservation_station[i].Empty())
		{
			do
			{
				Instruction<CONFIG> *instruction = iu1_reservation_station[i].Front();
				FreeInstruction(instruction);
				iu1_reservation_station[i].Pop();
			} while(!iu1_reservation_station[i].Empty());
		}
	}

	for(i = 0; i < CONFIG::NUM_IU2; i++)
	{
		if(!iu2_reservation_station[i].Empty())
		{
			do
			{
				Instruction<CONFIG> *instruction = iu2_reservation_station[i].Front();
				FreeInstruction(instruction);
				iu2_reservation_station[i].Pop();
			} while(!iu2_reservation_station[i].Empty());
		}
	}

	for(i = 0; i < CONFIG::NUM_FPU; i++)
	{
		if(!fpu_reservation_station[i].Empty())
		{
			do
			{
				Instruction<CONFIG> *instruction = fpu_reservation_station[i].Front();
				FreeInstruction(instruction);
				fpu_reservation_station[i].Pop();
			} while(!fpu_reservation_station[i].Empty());
		}
	}

	if(!lsu_reservation_station.Empty())
	{
		do
		{
			Instruction<CONFIG> *instruction = lsu_reservation_station.Front();
			FreeInstruction(instruction);
			lsu_reservation_station.Pop();
		} while(!lsu_reservation_station.Empty());
	}

	if(!cq.Empty())
	{
		do
		{
			//Instruction<CONFIG> *instruction = cq.Front();

			cq.Pop();
		} while(!cq.Empty());
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpPipeline()
{
	stringstream sstr;
	
	sstr << iq;
	
	logger << DebugInfo;
	logger << sstr.str();
	logger << EndDebugInfo;
}


template <class CONFIG>
void Instruction<CONFIG>::Initialize(CPU<CONFIG> *cpu, typename CONFIG::address_t cia, uint32_t encoding)
{
	this->operation = cpu->Decode(cia, encoding);
	operation->resolve(cpu, 0);
	execution_unit = operation->execution_unit;
	serialization = operation->serialization;
	num_uops = operation->num_uops;
	this->uop_num = 0;
	//Instruction<CONFIG> *macro_op;
	num_input_gpr = 0;
	num_input_fpr = 0;
	num_input_cr = 0;
	num_input_lr = 0;
	num_input_ctr = 0;
	input_operands.Clear();
	forwarding.Clear();
	output_operands.Clear();
}

template <class CONFIG>
void Instruction<CONFIG>::Initialize(CPU<CONFIG> *cpu, const Instruction<CONFIG>& instruction, unsigned int uop_num)
{
	this->operation = instruction.operation;
	operation->resolve(cpu, uop_num);
	execution_unit = operation->execution_unit;
	serialization = operation->serialization;
	num_uops = operation->num_uops;
	this->uop_num = uop_num;
	input_operands.Clear();
	forwarding.Clear();
	output_operands.Clear();
}

template <class CONFIG>
bool Instruction<CONFIG>::CheckInputOperandsAvailability()
{
	if(ready_to_execute) return true;

	unsigned int num_input_operands = input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_input_operands; i++)
	{
		if(forwarding[i] && !input_operands[i]->valid) return false;
	}

	ready_to_execute = true;
	return true;
}

/*
template <class CONFIG>
void Instruction<CONFIG>::Initialize(CPU<CONFIG> *cpu, unsigned int uop_num)
{
	unsigned int i;

	operation->resolve(cpu, uop_num);
	execution_unit = operation->execution_unit;
	serialization = operation->serialization;
	num_uops = operation->num_uops;
	this->uop_num = uop_num;
	num_input_operands = 0;
	num_ready_input_operands = 0;

	for(i = 0; i < operation->num_insn_operands; i++)
	{
		typename CONFIG::operand_t *operand = &operation->insn_operands[i];

		switch(operand->dir)
		{
			case CONFIG::INPUT_T:
				switch(operand->type)
				{
					case CONFIG::GPR_T:
						input_gpr[num_input_gpr].reg_num = operand->extra.reg_num;
						num_input_gpr++;
						num_input_operands++;
						break;

					case CONFIG::FPR_T:
						input_fpr[num_input_fpr].reg_num = operand->extra.reg_num;
						num_input_fpr++;
						num_input_operands++;
						break;
		
					case CONFIG::CRF_T:
						has_input_cr = true;
						num_input_operands++;
						break;
		
					case CONFIG::LR_T:
						has_input_lr = true;
						num_input_operands++;
						break;
		
					case CONFIG::CTR_T:
						has_input_ctr = true;
						num_input_operands++;
						break;
				}
				break;

			case CONFIG::OUTPUT_T:
				switch(operand->type)
				{
					case CONFIG::GPR_T:
						output_gpr[num_output_gpr].reg_num = operand->extra.reg_num;
						num_output_gpr++;
						break;
		
					case CONFIG::FPR_T:
						output_fpr[num_output_fpr].reg_num = operand->extra.reg_num;
						num_output_fpr++;
						break;
		
					case CONFIG::CRF_T:
						has_output_cr = true;
						break;
		
					case CONFIG::LR_T:
						has_output_lr = true;
						break;
		
					case CONFIG::CTR_T:
						has_output_ctr = true;
						break;
				}
				break;
		}
	}

	cycle = 0;
	stage = FETCH;
	finished = false;
	allowed_to_execute = (serialization == CONFIG::EXECUTION_SERIALIZATION) ? false : true;
	ready_to_execute = false;
}

template <class CONFIG>
void Instruction<CONFIG>::SetInputGPRValue(unsigned int i, uint32_t value)
{
	input_gpr[i].value = value;
	input_gpr[i].ready = true;
	num_ready_input_operands++;
	ready_to_execute = (num_ready_input_operands >= num_input_operands);
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetGPR(unsigned int n)
{
	unsigned int i;
	for(i = 0; i < num_input_gpr; i++)
	{
		if(input_gpr[i].reg_num == n) return input_gpr[i].value;
	}
	assert(false);
}

*/

template <class CONFIG>
const Operand<CONFIG> *Instruction<CONFIG>::SearchInputOperand(typename Operand<CONFIG>::Type type, unsigned int reg_num) const
{
	unsigned int num_input_operands = input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_input_operands; i++)
	{
		const Operand<CONFIG> *operand = input_operands[i];
		if(operand->type == type && operand->reg_num == reg_num)
		{
			return operand;
		}
	}
	return 0;
}

template <class CONFIG>
Operand<CONFIG> *Instruction<CONFIG>::SearchOutputOperand(typename Operand<CONFIG>::Type type, unsigned int reg_num)
{
	unsigned int num_output_operands = input_operands.Size();
	unsigned int i;

	for(i = 0; i < num_output_operands; i++)
	{
		Operand<CONFIG> *operand = input_operands[i];
		if(operand->type == type && operand->reg_num == reg_num)
		{
			return operand;
		}
	}
	return 0;
}

template <class CONFIG>
void Instruction<CONFIG>::SetGPR(unsigned int n, uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::GPR, n);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetFPR(unsigned int n, const SoftDouble& value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::FPR, n);
	assert(operand != 0);
	operand->float_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetCR(uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::CR);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetLR(uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::LR);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetCTR(uint32_t value, uint64_t latency)
{
	Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::CTR);
	assert(operand != 0);
	operand->int_value = value;
	cpu->NotifyOperandAvailability(operand, latency);
}

template <class CONFIG>
void Instruction<CONFIG>::SetXER(uint32_t value)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
	{
		Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::XER);
		assert(operand != 0);
		operand->int_value = value;
	}
	else
	{
		cpu->SetXER(value);
	}
}

template <class CONFIG>
void Instruction<CONFIG>::SetFPSCR(uint32_t value)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
	{
		Operand<CONFIG> *operand = SearchOutputOperand(Operand<CONFIG>::FPSCR);
		assert(operand != 0);
		operand->int_value = value;
	}
	else
	{
		cpu->SetFPSCR(value);
	}
}

template <class CONFIG>
void Instruction<CONFIG>::SetNIA(typename CONFIG::address_t value)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		nia = value;
	else
		cpu->SetNIA(value);
}


template <class CONFIG>
uint32_t Instruction<CONFIG>::GetGPR(unsigned int n) const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::GPR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
const SoftDouble& Instruction<CONFIG>::GetFPR(unsigned int n) const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::FPR);
	assert(operand != 0);
	return operand->float_value;
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetCR() const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::CR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetLR() const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::LR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
uint32_t Instruction<CONFIG>::GetCTR() const
{
	const Operand<CONFIG> *operand = SearchInputOperand(Operand<CONFIG>::CTR);
	assert(operand != 0);
	return operand->int_value;
}

template <class CONFIG>
void Instruction<CONFIG>::Int8Load(unsigned int rd, typename CONFIG::address_t ea)
{
#ifdef SOCLIB
	GenSocLibLoadStore(LoadStoreAccess<CONFIG>::INT8_LOAD, rd, ea, 1);
#else
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT8_LOAD, rd, ea, 1, this);
	else
		cpu->Int8Load(rd, ea);
#endif
}

template <class CONFIG>
void Instruction<CONFIG>::Int16Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_LOAD, rd, ea, 2, this);
	else
		cpu->Int16Load(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::SInt16Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::SINT16_LOAD, rd, ea, 2, this);
	else
		cpu->SInt16Load(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32Load(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_LOAD, rd, ea, 4, this);
	else
		cpu->Int32Load(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp32Load(unsigned int fd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP32_LOAD, fd, ea, 4, this);
	else
		cpu->Fp32Load(fd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp64Load(unsigned int fd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP64_LOAD, fd, ea, 8, this);
	else
		cpu->Fp64Load(fd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int16LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE, rd, ea, 2, this);
	else
		cpu->Int16LoadByteReverse(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32LoadByteReverse(unsigned int rd, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE, rd, ea, 4, this);
	else
		cpu->Int32LoadByteReverse(rd, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::IntLoadMSBFirst(unsigned int rd, typename CONFIG::address_t ea, uint32_t size)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT_LOAD_MSB, rd, ea, size, this);
	else
		cpu->IntLoadMSBFirst(rd, ea, size);
}

template <class CONFIG>
void Instruction<CONFIG>::Int8Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT8_STORE, rs, ea, 1, this);
	else
		cpu->Int8Store(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int16Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_STORE, rs, ea, 2, this);
	else
		cpu->Int16Store(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32Store(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_STORE, rs, ea, 4, this);
	else
		cpu->Int32Store(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp32Store(unsigned int fs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP32_STORE, fs, ea, 4, this);
	else
		cpu->Fp32Store(fs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Fp64Store(unsigned int fs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP64_STORE, fs, ea, 8, this);
	else
		cpu->Fp64Store(fs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::FpStoreLSW(unsigned int fs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::FP_STORE_LSW, fs, ea, 4, this);
	else
		cpu->FpStoreLSW(fs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int16StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE, rs, ea, 2, this);
	else
		cpu->Int16StoreByteReverse(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::Int32StoreByteReverse(unsigned int rs, typename CONFIG::address_t ea)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE, rs, ea, 4, this);
	else
		cpu->Int32StoreByteReverse(rs, ea);
}

template <class CONFIG>
void Instruction<CONFIG>::IntStoreMSBFirst(unsigned int rs, typename CONFIG::address_t ea, uint32_t size)
{
	if(sim_mode == PERFORMANCE_SIMULATION_MODE)
		cpu->GenLoadStoreAccess(LoadStoreAccess<CONFIG>::INT_STORE_MSB, rs, ea, size, this);
	else
		cpu->IntStoreMSBFirst(rs, ea, size);
}

template <class CONFIG>
void Instruction<CONFIG>::Execute(CPU<CONFIG> *cpu)
{
	operation->uop_execute(cpu, uop_num);
}

template <class CONFIG>
MappingTable<CONFIG>::MappingTable()
{
	unsigned int reg_num;

	for(reg_num = 0; reg_num < CONFIG::NUM_LOGICAL_REGISTERS; reg_num++)
	{
		mapping_table[reg_num] = reg_num;
	}

	int tag;

	// WARNING! tags are negative integers (2's complement)
	// WARNING! tag 0 is unused as it refers to logical register 0
	// WARNING! 'tag' refers to rename register number '~tag'
	for(tag = -1; tag >= -((int) CONFIG::NUM_RENAME_REGISTERS); tag--)
	{
		free_list.Push(tag);
	}
}

template <class CONFIG>
MappingTable<CONFIG>::~MappingTable()
{
}

template <class CONFIG>
bool MappingTable<CONFIG>::Allocate(unsigned int reg_num, int& tag)
{
	if(free_list.Empty()) return false;

	tag = mapping_table[reg_num] = free_list.Front();
	free_list.Pop();

	return true;
}

template <class CONFIG>
int MappingTable<CONFIG>::Lookup(unsigned int reg_num)
{
	return mapping_table[reg_num];
}

template <class CONFIG>
void MappingTable<CONFIG>::Free(unsigned int reg_num, int tag)
{
	if(mapping_table[reg_num] == tag)
	{
		mapping_table[reg_num] = reg_num;
	}

	free_list.Push(tag);
}

template <class CONFIG>
unsigned int MappingTable<CONFIG>::GetNumFreeRenameRegisters() const
{
	return free_list.Size();
}

template <class CONFIG>
ostream& operator << (ostream& os, const FetchAccess<CONFIG>& fetch_access)
{
	os << "FETCH_ACCESS(";
	os << "cia=0x" << std::hex << fetch_access.cia << std::dec;
	os << ", size=" << fetch_access.size;
	os << ", " << fetch_access.mmu_access;
	os << ", L1_" << fetch_access.l1_access;
	os << ", L2_" << fetch_access.l2_access;
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const MMUAccess<CONFIG>& mmu_access)
{
	os << "MMU_ACCESS(";
	os << "addr=0x" << std::hex << mmu_access.addr << std::dec;
	os << ", privilege_level=";
	switch(mmu_access.privilege_level)
	{
		case CONFIG::PR_SUPERVISOR:
			os << "supervisor";
			break;
		case CONFIG::PR_USER:
			os << "user";
			break;
	}
	os << ", memory_access_type=";
	switch(mmu_access.memory_access_type)
	{
		case CONFIG::MAT_READ:
			os << "read";
			break;
		case CONFIG::MAT_WRITE:
			os << "write";
			break;
	}
	os << ", memory_type=";
	switch(mmu_access.memory_type)
	{
		case CONFIG::MT_INSN:
			os << "insn";
			break;
		case CONFIG::MT_DATA:
			os << "data";
			break;
	}
	os << ", bepi=" << std::hex << mmu_access.bepi << std::dec;
	os << ", sr_num=" << mmu_access.sr_num;
	os << ", virtual_segment_id=" << mmu_access.virtual_segment_id;
	os << ", sr_ks=" << mmu_access.sr_ks;
	os << ", sr_kp=" << mmu_access.sr_kp;
	os << ", sr_noexecute=" << mmu_access.sr_noexecute;
	os << ", virtual_addr=0x" << std::hex << mmu_access.virtual_addr << std::dec;
	os << ", base_virtual_addr=0x" << std::hex << mmu_access.base_virtual_addr << std::dec;
	os << ", tlb_index=" << mmu_access.tlb_index;
	os << ", tlb_way=" << mmu_access.tlb_way;
	os << ", key=" << mmu_access.key;
	os << ", force_page_table_walk=" << mmu_access.force_page_table_walk;
	os << ", page_index=0x" << std::hex << mmu_access.page_index << std::dec;
	os << ", api=0x" << std::hex << mmu_access.api << std::dec;
	os << ", bat_hit=" << mmu_access.bat_hit;
	os << ", tlb_hit=" << mmu_access.tlb_hit;
	os << ", physical_addr=0x" << std::hex << mmu_access.physical_addr << std::dec;
	os << ", protection_boundary=0x" << std::hex << mmu_access.protection_boundary << std::dec;
	os << ", wimg=";
	os << ((mmu_access.wimg & CONFIG::WIMG_WRITE_THROUGH) ? "W" : "x");
	os << ((mmu_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) ? "I" : "x");
	os << ((mmu_access.wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? "M" : "x");
	os << ((mmu_access.wimg & CONFIG::WIMG_GUARDED_MEMORY) ? "G" : "x");
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const CacheAccess<CONFIG>& cache_access)
{
	os << "CACHE_ACCESS(";
	os << "addr=0x" << std::hex << cache_access.addr << std::dec;
	os << ",line_base_addr=0x" << std::hex << cache_access.line_base_addr << std::dec;
	os << ",block_base_addr=0x" << std::hex << cache_access.block_base_addr << std::dec;
	os << ", way=" << cache_access.way;
	os << ", sector=" << cache_access.sector;
	os << ", offset=" << cache_access.offset;
	os << ", size_to_block_boundary=" << cache_access.size_to_block_boundary;
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const LoadStoreAccess<CONFIG>& load_store_access)
{
	os << "LOAD_STORE_ACCESS(";
	os << "type=";
	switch(load_store_access.type)
	{
		case LoadStoreAccess<CONFIG>::INT8_LOAD: os << "int8_load"; break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD: os << "int16_load"; break;
		case LoadStoreAccess<CONFIG>::SINT16_LOAD: os << "sint16_load"; break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD: os << "int32_load"; break;
		case LoadStoreAccess<CONFIG>::FP32_LOAD: os << "fp32_load"; break;
		case LoadStoreAccess<CONFIG>::FP64_LOAD: os << "fp64_load"; break;
		case LoadStoreAccess<CONFIG>::INT16_LOAD_BYTE_REVERSE: os << "int16_load_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT32_LOAD_BYTE_REVERSE: os << "int32_load_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT_LOAD_MSB: os << "int_load_msb"; break;
		case LoadStoreAccess<CONFIG>::INT8_STORE: os << "int8_store"; break;
		case LoadStoreAccess<CONFIG>::INT16_STORE: os << "int16_store"; break;
		case LoadStoreAccess<CONFIG>::INT32_STORE: os << "int32_store"; break;
		case LoadStoreAccess<CONFIG>::FP32_STORE: os << "fp32_store"; break;
		case LoadStoreAccess<CONFIG>::FP64_STORE: os << "fp64_store"; break;
		case LoadStoreAccess<CONFIG>::FP_STORE_LSW: os << "fp_store_lsw"; break;
		case LoadStoreAccess<CONFIG>::INT16_STORE_BYTE_REVERSE: os << "int16_store_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT32_STORE_BYTE_REVERSE: os << "int32_store_byte_reverse"; break;
		case LoadStoreAccess<CONFIG>::INT_STORE_MSB: os << "int_store_msb"; break;
	}
	os << ", reg_num=" << load_store_access.reg_num;
	os << ", munged_ea=0x" << std::hex << load_store_access.munged_ea << std::dec;
	os << ", offset=" << load_store_access.offset;
	os << ", size=" << load_store_access.size;
	os << ", valid=" << load_store_access.valid;
	os << ", data=[" << std::hex;
	int i;
	for(i = 0; i < sizeof(load_store_access.data); i++)
	{
		if(i != 0) os << " ";
		os << "0x" << (unsigned int) load_store_access.data[i];
	}
	os << "]" << std::dec;
	os << ", " << load_store_access.mmu_access;
	os << ", L1_" << load_store_access.l1_access;
	os << ", L2_" << load_store_access.l2_access;
	os << ")";
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const BusAccess<CONFIG>& bus_access)
{
	os << "BUS_ACCESS(";
	os << "issued=" << bus_access.issued;
	os << ", type=";
	switch(bus_access.type)
	{
		case BusAccess<CONFIG>::LOAD: os << "load"; break;
		case BusAccess<CONFIG>::REFILL: os << "refill"; break;
		case BusAccess<CONFIG>::REFILLX: os << "refillx"; break;
		case BusAccess<CONFIG>::STORE: os << "store"; break;
		case BusAccess<CONFIG>::EVICTION: os << "eviction"; break;
	}
	os << ", addr=0x" << std::hex << bus_access.addr << std::dec;
	os << ", size=" << bus_access.size;
	os << ", storage=[";
	unsigned int i;
	for(i = 0; i < bus_access.size; i++)
	{
		if(i != 0) os << " ";
		os << "0x" << std::hex << (unsigned int) bus_access.storage[i] << std::dec;
	}
	os << "]";
	os << ", wimg=";
	os << ((bus_access.wimg & CONFIG::WIMG_WRITE_THROUGH) ? "W" : "x");
	os << ((bus_access.wimg & CONFIG::WIMG_CACHE_INHIBITED) ? "I" : "x");
	os << ((bus_access.wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? "M" : "x");
	os << ((bus_access.wimg & CONFIG::WIMG_GUARDED_MEMORY) ? "G" : "x");
	os << ", rwitm=" << bus_access.rwitm;
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const Operand<CONFIG>& operand)
{
	os << "OPERAND(";
	os << "type=";
	switch(operand.type)
	{
		case Operand<CONFIG>::GPR: os << "GPR"; break;
		case Operand<CONFIG>::FPR: os << "FPR"; break;
		case Operand<CONFIG>::CR: os << "CR"; break;
		case Operand<CONFIG>::LR: os << "LR"; break;
		case Operand<CONFIG>::CTR: os << "CTR"; break;
		case Operand<CONFIG>::XER: os << "XER"; break;
		case Operand<CONFIG>::FPSCR: os << "FPSCR"; break;
	}
	os << ", valid=" << operand.valid;
	os << ", reg_num=" << operand.reg_num;
	os << ", tag=" << operand.tag;
	os << ", int_value=0x" << std::hex << operand.int_value << std::dec;
	os << ", float_value=0x" << operand.float_value;
	os << ", ref_count=" << operand.ref_count;
	return os;
}

template <class CONFIG>
ostream& operator << (ostream& os, const Instruction<CONFIG>& instruction)
{
	os << "INSTRUCTION(";
	os << ")";
	return os;
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
