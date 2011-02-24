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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/ppc440/isa.tcc>
#include <unisim/util/simfloat/floating.tcc>
#include <unisim/component/cxx/processor/powerpc/ppc440/exception.tcc>
#include <unisim/component/cxx/cache/cache.tcc>
#include <unisim/component/cxx/tlb/tlb.tcc>
#include <unisim/util/queue/queue.tcc>
#include <unisim/kernel/debug/debug.hh>

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
namespace ppc440 {

using namespace std;

template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
	: Object(name, parent, "this module implements a PPC440 CPU core")
	, unisim::component::cxx::processor::powerpc::ppc440::Decoder<CONFIG>()
	, Client<Loader<typename CONFIG::address_t> >(name,  parent)
	, Client<SymbolTableLookup<typename CONFIG::address_t> >(name,  parent)
	, Client<DebugControl<typename CONFIG::address_t> >(name,  parent)
	, Client<MemoryAccessReporting<typename CONFIG::address_t> >(name,  parent)
	, Client<TrapReporting>(name,  parent)
	, Service<MemoryAccessReportingControl>(name,  parent)
	, Service<Disassembly<typename CONFIG::address_t> >(name,  parent)
	, Service<unisim::service::interfaces::Registers>(name,  parent)
	, Service<Memory<typename CONFIG::address_t> >(name,  parent)
	, Service<MemoryInjection<typename CONFIG::address_t> >(name,  parent)
	, Client<Memory<typename CONFIG::physical_address_t> >(name,  parent)
	, Client<LinuxOS>(name,  parent)
	, Client<CachePowerEstimator>(name,  parent)
	, Client<PowerMode>(name,  parent)
	, Service<Synchronizable>(name,  parent)
	, disasm_export("disasm-export",  this)
	, registers_export("registers-export",  this)
	, memory_export("memory-export",  this)
	, memory_injection_export("memory-injection-export",  this)
	, synchronizable_export("synchronizable-export",  this)
	, memory_access_reporting_control_export("memory_access_reporting_control_export",  this)
	, loader_import("loader-import",  this)
	, debug_control_import("debug-control-import",  this)
	, memory_access_reporting_import("memory-access-reporting-import",  this)
	, symbol_table_lookup_import("symbol-table-lookup-import",  this)
	, memory_import("memory-import", this)
	, linux_os_import("linux-os-import",  this)
	, trap_reporting_import("trap-reporting-import",  this)
	, il1_power_estimator_import("il1-power-estimator-import",  this)
	, dl1_power_estimator_import("dl1-power-estimator-import",  this)
	, itlb_power_estimator_import("itlb-power-estimator-import",  this)
	, dtlb_power_estimator_import("dtlb-power-estimator-import",  this)
	, utlb_power_estimator_import("utlb-power-estimator-import",  this)
	, il1_power_mode_import("il1-power-mode-import",  this)
	, dl1_power_mode_import("dl1-power-mode-import",  this)
	, itlb_power_mode_import("itlb-power-mode-import",  this)
	, dtlb_power_mode_import("dtlb-power-mode-import",  this)
	, utlb_power_mode_import("utlb-power-mode-import",  this)
    , linux_printk_buf_addr(0)
    , linux_printk_buf_size(0)
	, logger(*this)
	, requires_memory_access_reporting(true)
	, requires_finished_instruction_reporting(true)
	, cpu_cycle_time(0)
	, voltage(0)
	, timer_cycle(0)
	, effective_address(0)
	, verbose_all(false)
	, verbose_setup(false)
	, verbose_step(false)
	, verbose_itlb(false)
	, verbose_dtlb(false)
	, verbose_utlb(false)
	, verbose_dl1(false)
	, verbose_il1(false)
	, verbose_load(false)
	, verbose_store(false)
	, verbose_read_memory(false)
	, verbose_write_memory(false)
	, verbose_exception(false)
	, verbose_set_msr(false)
	, verbose_tlbwe(false)
	, enable_linux_printk_snooping(false)
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, enable_trap_on_exception(false)
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
	, itlb()
	, dtlb()
	, utlb()
	, num_itlb_accesses(0)
	, num_itlb_misses(0)
	, num_dtlb_accesses(0)
	, num_dtlb_misses(0)
	, num_utlb_accesses(0)
	, num_utlb_misses(0)
	, num_insn_in_prefetch_buffer(0)
	, cur_insn_in_prefetch_buffer(0)
	, prefetch_buffer()
	, reserve(false)
	, reserve_addr(0)
	, irq(0)
	, param_cpu_cycle_time("cpu-cycle-time",  this,  cpu_cycle_time, "CPU cycle time in picoseconds")
	, param_voltage("voltage",  this,  voltage, "CPU voltage in mV")
	, param_max_inst("max-inst",  this,  max_inst, "maximum number of instructions to simulate")
	, param_verbose_all("verbose-all",  this,  verbose_all, "globally enable/disable verbosity")
	, param_verbose_setup("verbose-setup",  this,  verbose_setup, "enable/disable verbosity while setup")
	, param_verbose_step("verbose-step",  this,  verbose_step, "enable/disable verbosity when simulating an instruction")
	, param_verbose_itlb("verbose-itlb",  this,  verbose_itlb, "enable/disable verbosity when accessing shadow instruction translation lookahead buffer")
	, param_verbose_dtlb("verbose-dtlb",  this,  verbose_dtlb, "enable/disable verbosity when accessing shadow data translation lookahead buffer")
	, param_verbose_utlb("verbose-utlb",  this,  verbose_utlb, "enable/disable verbosity when accessing unified translation lookahead buffer")
	, param_verbose_dl1("verbose-dl1",  this,  verbose_dl1, "enable/disable verbosity when accessing L1 data cache")
	, param_verbose_il1("verbose-il1",  this,  verbose_il1, "enable/disable verbosity when accessing L1 instruction cache")
	, param_verbose_load("verbose-load",  this,  verbose_load, "enable/disable verbosity when simulating a load")
	, param_verbose_store("verbose-store",  this,  verbose_store, "enable/disable verbosity when simulating a store")
	, param_verbose_read_memory("verbose-read-memory",  this,  verbose_read_memory, "enable/disable verbosity when reading memory for a debug purpose")
	, param_verbose_write_memory("verbose-write-memory",  this,  verbose_write_memory, "enable/disable verbosity when writing memory for a debug purpose")
	, param_verbose_exception("verbose-exception",  this,  verbose_exception, "enable/disable verbosity when handling exceptions")
	, param_verbose_set_msr("verbose-set-msr",  this,  verbose_set_msr, "enable/disable verbosity when setting MSR")
	, param_verbose_tlbwe("verbose-tlbwe",  this,  verbose_tlbwe, "enable/disable verbosity when executing a tlbwe instruction")
	, param_enable_linux_printk_snooping("enable-linux-printk-snooping", this, enable_linux_printk_snooping, "enable/disable linux printk buffer snooping")
	, param_trap_on_instruction_counter("trap-on-instruction-counter",  this,  trap_on_instruction_counter, "number of simulated instruction before traping")
	, param_enable_trap_on_exception("enable-trap-on-exception", this, enable_trap_on_exception, "enable/disable trap reporting on exception")
//	, param_bus_cycle_time("bus-cycle-time",  this,  bus_cycle_time, "bus cycle time in picoseconds")
	, stat_instruction_counter("instruction-counter",  this,  instruction_counter, "number of simulated instructions")
	, stat_timer_cycle("timer-cycle",  this,  timer_cycle, "number of simulated timer cycles")
	, stat_num_il1_accesses("num-il1-accesses", this, num_il1_accesses, "number of accesses to L1 instruction cache")
	, stat_num_il1_misses("num-il1-misses", this, num_il1_misses, "number of misses to L1 instruction cache")
	, formula_il1_miss_rate("il1-miss-rate", this, Formula<double>::OP_DIV, &stat_num_il1_misses, &stat_num_il1_accesses, "L1 instruction cache miss rate")
	, stat_num_dl1_accesses("num-dl1-accesses", this, num_dl1_accesses, "number of accesses to L1 data cache")
	, stat_num_dl1_misses("num-dl1-misses", this, num_dl1_misses, "number of misses to L1 data cache")
	, formula_dl1_miss_rate("dl1-miss-rate", this, Formula<double>::OP_DIV, &stat_num_dl1_misses, &stat_num_dl1_accesses, "L1 data cache miss rate")
	, stat_num_itlb_accesses("num-itlb-accesses", this, num_itlb_accesses, "number of accesses to shadow instruction translation look-aside buffer")
	, stat_num_itlb_misses("num-itlb-misses", this, num_itlb_misses, "number of misses to shadow instruction translation look-aside buffer")
	, formula_itlb_miss_rate("itlb-miss-rate", this, Formula<double>::OP_DIV, &stat_num_itlb_misses, &stat_num_itlb_accesses, "shadow instruction translation look-aside buffer miss rate")
	, stat_num_dtlb_accesses("num-dtlb-accesses", this, num_dtlb_accesses, "number of accesses to shadow data translation look-aside buffer")
	, stat_num_dtlb_misses("num-dtlb-misses", this, num_dtlb_misses, "number of misses to shadow data translation look-aside buffer")
	, formula_dtlb_miss_rate("dtlb-miss-rate", this, Formula<double>::OP_DIV, &stat_num_dtlb_misses, &stat_num_dtlb_accesses, "shadow data translation look-aside buffer miss rate")
	, stat_num_utlb_accesses("num-utlb-accesses", this, num_utlb_accesses, "number of accesses to unified data translation look-aside buffer")
	, stat_num_utlb_misses("num-utlb-misses", this, num_utlb_misses, "number of misses to unified data translation look-aside buffer")
	, formula_utlb_miss_rate("utlb-miss-rate", this, Formula<double>::OP_DIV, &stat_num_utlb_misses, &stat_num_utlb_accesses, "unified data translation look-aside buffer miss rate")
{
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_cpu_cycle_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_voltage.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	unsigned int i;

	registers_registry["cia"] = new unisim::util::debug::SimpleRegister<uint32_t>("cia", &cia);

	registers_registry["cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("cr", &cr);
	registers_registry["ctr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ctr", &ctr);
	registers_registry["lr"] = new unisim::util::debug::SimpleRegister<uint32_t>("lr", &lr);

	for(i = 0; i < CONFIG::NUM_DNVS; i++)
	{
		stringstream sstr;
		sstr << "dnv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dnv[i]);
	}

	for(i = 0; i < CONFIG::NUM_DTVS; i++)
	{
		stringstream sstr;
		sstr << "dtv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dtv[i]);
	}

	registers_registry["dvlim"] = new unisim::util::debug::SimpleRegister<uint32_t>("dvlim", &dvlim);

	for(i = 0; i < CONFIG::NUM_INVS; i++)
	{
		stringstream sstr;
		sstr << "inv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &inv[i]);
	}

	for(i = 0; i < CONFIG::NUM_ITVS; i++)
	{
		stringstream sstr;
		sstr << "itv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &itv[i]);
	}

	registers_registry["ivlim"] = new unisim::util::debug::SimpleRegister<uint32_t>("ivlim", &ivlim);

	registers_registry["dcdbtrh"] = new unisim::util::debug::SimpleRegister<uint32_t>("dcdbtrh", &dcdbtrh);
	registers_registry["dcdbtrl"] = new unisim::util::debug::SimpleRegister<uint32_t>("dcdbtrl", &dcdbtrl);
	registers_registry["icdbdr"] = new unisim::util::debug::SimpleRegister<uint32_t>("icdbdr", &icdbdr);
	registers_registry["icdbtrh"] = new unisim::util::debug::SimpleRegister<uint32_t>("icdbtrh", &icdbtrh);
	registers_registry["icdbtrl"] = new unisim::util::debug::SimpleRegister<uint32_t>("icdbtrl", &icdbtrl);

	for(i = 0; i < CONFIG::NUM_DACS; i++)
	{
		stringstream sstr;
		sstr << "dac" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dac[i]);
	}

	for(i = 0; i < CONFIG::NUM_DBCRS; i++)
	{
		stringstream sstr;
		sstr << "dbcr" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dbcr[i]);
	}

	registers_registry["dbdr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dbdr", &dbdr);
	registers_registry["dbsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dbsr", &dbsr);

	for(i = 0; i < CONFIG::NUM_DVCS; i++)
	{
		stringstream sstr;
		sstr << "dvc" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dvc[i]);
	}

	for(i = 0; i < CONFIG::NUM_IACS; i++)
	{
		stringstream sstr;
		sstr << "iac" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &iac[i]);
	}

	for(i = 0; i < CONFIG::NUM_GPRS; i++)
	{
		stringstream sstr;
		sstr << "r" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &gpr[i]);
	}

	registers_registry["xer"] = new unisim::util::debug::SimpleRegister<uint32_t>("xer", &xer);

	for(i = 0; i < CONFIG::NUM_FPRS; i++)
	{
		stringstream sstr;
		sstr << "f" << i;
		registers_registry[sstr.str()] = new FloatingPointRegisterInterface(sstr.str().c_str(), &fpr[i]);
	}

	registers_registry["fpscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpscr", &fpscr);

	registers_registry["csrr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("csrr0", &csrr0);
	registers_registry["csrr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("csrr1", &csrr1);
	registers_registry["dear"] = new unisim::util::debug::SimpleRegister<uint32_t>("dear", &dear);
	registers_registry["esr"] = new unisim::util::debug::SimpleRegister<uint32_t>("esr", &esr);

	for(i = 0; i < CONFIG::NUM_IVORS; i++)
	{
		stringstream sstr;
		sstr << "ivor" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &ivor[i]);
	}

	registers_registry["ivpr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ivpr", &ivpr);
	registers_registry["mcsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("mcsr", &mcsr);
	registers_registry["mcsrr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("mcsrr0", &mcsrr0);
	registers_registry["mcsrr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("mcsrr1", &mcsrr1);
	registers_registry["srr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr0", &srr0);
	registers_registry["srr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr1", &srr1);

	registers_registry["ccr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("ccr0", &ccr0);
	registers_registry["ccr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("ccr1", &ccr1);
	registers_registry["msr"] = new unisim::util::debug::SimpleRegister<uint32_t>("msr", &msr);
	registers_registry["pir"] = new unisim::util::debug::SimpleRegister<uint32_t>("pir", &pir);
	registers_registry["pvr"] = new unisim::util::debug::SimpleRegister<uint32_t>("pvr", &pvr);
	registers_registry["rstcfg"] = new unisim::util::debug::SimpleRegister<uint32_t>("rstcfg", &rstcfg);
	
	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
	{
		stringstream sstr;
		sstr << "sprg" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sprg[i]);
	}

	registers_registry["usprg0"] = new unisim::util::debug::SimpleRegister<uint32_t>("usprg0", &usprg0);

	registers_registry["mmucr"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmucr", &mmucr);
	registers_registry["pid"] = new unisim::util::debug::SimpleRegister<uint32_t>("pid", &pid);

	registers_registry["dec"] = new unisim::util::debug::SimpleRegister<uint32_t>("dec", &dec);
	registers_registry["decar"] = new unisim::util::debug::SimpleRegister<uint32_t>("decar", &decar);
	registers_registry["tbl"] = new TimeBaseRegisterInterface("tbl", &tb, TimeBaseRegisterInterface::TB_LOW);
	registers_registry["tbu"] = new TimeBaseRegisterInterface("tbu", &tb, TimeBaseRegisterInterface::TB_HIGH);
	registers_registry["tcr"] = new unisim::util::debug::SimpleRegister<uint32_t>("tcr", &tcr);
	registers_registry["tsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("tsr", &tsr);

	Reset();
	
	std::stringstream sstr_description;
	sstr_description << "This module implements a PPC440 CPU core. It has the following characteristics:" << std::endl;
	sstr_description << "Processor version (PVR value): 0x" << std::hex << CONFIG::PROCESSOR_VERSION << std::dec << std::endl;
	sstr_description << "Reset configuration (RSTCFG): U0=" << GetRSTCFG_U0()  << ", U1=" << GetRSTCFG_U1() << ", U2=" << GetRSTCFG_U2() << ", U3=" << GetRSTCFG_U3() << ", E=" << GetRSTCFG_E() << ", ERPN=0x" << std::hex << GetRSTCFG_ERPN() << std::dec << std::endl;
	sstr_description << "Start address: 0x" << std::hex << CONFIG::START_ADDR << std::dec << std::endl;
	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		sstr_description << "L1 data cache: size=" << CONFIG::DL1_CONFIG::CACHE_SIZE << " bytes, block size=" << CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE << " bytes, associativity=" << CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::IL1_CONFIG::ENABLE)
	{
		sstr_description << "L1 instruction cache: size=" << CONFIG::IL1_CONFIG::CACHE_SIZE << " bytes, block size=" << CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE << " bytes, associativity=" << CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::ITLB_CONFIG::ENABLE)
	{
		sstr_description << "shadow instruction TLB: size=" << CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES << " entries, associativity=" << CONFIG::ITLB_CONFIG::TLB_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::DTLB_CONFIG::ENABLE)
	{
		sstr_description << "shadow data TLB: size=" << CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES << " entries, associativity=" << CONFIG::DTLB_CONFIG::TLB_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::UTLB_CONFIG::ENABLE)
	{
		sstr_description << "unified TLB: size=" << CONFIG::UTLB_CONFIG::TLB_NUM_ENTRIES << " entries, associativity=" << CONFIG::UTLB_CONFIG::TLB_ASSOCIATIVITY << std::endl;
	}
	sstr_description << "FSB/PLB burst size:" << (8 * CONFIG::FSB_BURST_SIZE) << " bits" << std::endl;
	sstr_description << "FSB/PLB width:" << (8 * CONFIG::FSB_WIDTH) << " bits" << std::endl;
	sstr_description << "MMU: " << (CONFIG::HAS_MMU ? "yes" : "no") << std::endl;
	sstr_description << "FPU APU: " << (CONFIG::HAS_FPU ? "yes" : "no") << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());
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
bool CPU<CONFIG>::BeginSetup()
{
	if(Cache<typename CONFIG::IL1_CONFIG>::NUM_SETS > 16)
	{
		logger << DebugError << "Instruction cache has too many sets ( " << Cache<typename CONFIG::IL1_CONFIG>::NUM_SETS << " > 16) for the processor architecture" << EndDebugError;
		return false;
	}
	if(Cache<typename CONFIG::DL1_CONFIG>::NUM_SETS > 16)
	{
		logger << DebugError << "Data cache has too many sets ( " << Cache<typename CONFIG::DL1_CONFIG>::NUM_SETS << " > 16) for the processor architecture" << EndDebugError;
		return false;
	}
	if(CONFIG::IL1_CONFIG::CACHE_LOG_ASSOCIATIVITY > 8)
	{
		logger << DebugError << "Instruction cache associativity " << CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY << " is too high (> 256) for the processor architecture" << EndDebugError;
		return false;
	}
	if(CONFIG::DL1_CONFIG::CACHE_LOG_ASSOCIATIVITY > 8)
	{
		logger << DebugError << "Data cache associativity " << CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY << " is too high (> 256) for the processor architecture" << EndDebugError;
		return false;
	}
	
	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}

	if(!FloatingPointSelfTest())
	{
		logger << DebugError;
		logger << "Floating-point self test failed !" << endl;
		logger << EndDebugError;
		return false;
	}
	
	Reset();

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::EndSetup()
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

	if(CONFIG::UTLB_CONFIG::ENABLE && utlb_power_mode_import)
	{
		unsigned int utlb_min_cycle_time = utlb_power_mode_import->GetMinCycleTime();
		if(utlb_min_cycle_time > 0 && utlb_min_cycle_time > min_cycle_time) min_cycle_time = utlb_min_cycle_time;
		unsigned int utlb_default_voltage = utlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = utlb_default_voltage;
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

	if(itlb_power_mode_import)
		itlb_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);

	if(dtlb_power_mode_import)
		dtlb_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);

	if(utlb_power_mode_import)
		utlb_power_mode_import->SetPowerMode(cpu_cycle_time, voltage);

	if(linux_os_import)
	{
		if(CONFIG::HAS_FPU) EnableFPU();
		SetUserPrivilegeLevel();
	}
	
	num_il1_accesses = 0;
	num_il1_misses = 0;
	num_dl1_accesses = 0;
	num_dl1_misses = 0;
	num_itlb_accesses = 0;
	num_itlb_misses = 0;
	num_dtlb_accesses = 0;
	num_dtlb_misses = 0;
	num_utlb_accesses = 0;
	num_utlb_misses = 0;

	if(unlikely(CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_PRINTK_ENABLE && enable_linux_printk_snooping))
	{
		if(IsVerboseSetup())
		{
			logger << DebugInfo << "Linux printk snooping enabled" << EndDebugInfo;
		}
		if(!linux_printk_buf_addr)
		{
			if(symbol_table_lookup_import)
			{
				const Symbol<typename CONFIG::address_t> *symbol;

				symbol = symbol_table_lookup_import->FindSymbolByName("printk_buf", Symbol<typename CONFIG::address_t>::SYM_OBJECT);
				
				if(symbol)
				{
					linux_printk_buf_addr = symbol->GetAddress();
					linux_printk_buf_size = symbol->GetSize();
					if(IsVerboseSetup())
					{
						logger << DebugInfo << "Found Linux printk buffer at 0x" << std::hex << linux_printk_buf_addr << std::dec << "(" << linux_printk_buf_size << " bytes)" << EndDebugInfo;
					}
				}
			}
		}
		else
		{
			logger << DebugWarning << "Linux printk buffer not found. Linux printk snooping will not work properly." << EndDebugWarning;
		}
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

	timer_cycle = 0;
	instruction_counter = 0;

	irq = 0;

	reserve = false;
	reserve_addr = 0;

	for(i = 0; i < CONFIG::NUM_GPRS; i++) gpr[i] = 0;

	fpscr = 0;
	for(i = 0; i < CONFIG::NUM_FPRS; i++) fpr[i] = SoftDouble(0);

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
	tb = 0;

	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
	{
		sprg[i] = 0;
	}

	for(i = 0; i < CONFIG::NUM_DNVS; i++)
	{
		dnv[i] = 0;
	}
	
	for(i = 0; i < CONFIG::NUM_DTVS; i++)
	{
		dtv[i] = 0;
	}
	
	for(i = 0; i < CONFIG::NUM_INVS; i++)
	{
		inv[i] = 0;
	}

	for(i = 0; i < CONFIG::NUM_ITVS; i++)
	{
		itv[i] = 0;
	}
	dvlim = 0;
	ivlim = 0;
	
	dcdbtrh = 0;
	dcdbtrl = 0;
	icdbdr = 0;
	icdbtrh = 0;
	icdbtrl = 0;
	
	for(i = 0; i < CONFIG::NUM_DACS; i++)
	{
		dac[i] = 0;
	}
	for(i = 0; i < CONFIG::NUM_DBCRS; i++)
	{
		dbcr[i] = 0;
	}
	dbdr = 0;
	dbsr = 0;

	for(i = 0; i < CONFIG::NUM_DVCS; i++)
	{
		dvc[i] = 0;
	}
	for(i = 0; i < CONFIG::NUM_IACS; i++)
	{
		iac[i] = 0;
	}

	csrr0 = 0;
	csrr1 = 0;
	dear = 0;
	esr = 0;
	for(i = 0; i < CONFIG::NUM_IVORS; i++)
	{
		ivor[i] = 0;
	}
	ivpr = 0;
	mcsr = 0;
	mcsrr0 = 0;
	mcsrr1 = 0;
	srr0 = 0;
	srr1 = 0;
	ccr0 = 0;
	ccr1 = 0;
	rstcfg = CONFIG::RSTCFG_RESET_VALUE;
	mmucr = CONFIG::MMUCR_RESET_VALUE;
	pid = 0;
	dec = 0;
	decar = 0;
	tcr = 0;
	tsr = 0;

	CPU<CONFIG>::InvalidateDecodingCache();
	
	ResetDTLB();
	ResetITLB();
	ResetUTLB();

	if(CONFIG::IL1_CONFIG::ENABLE)
	{
		InvalidateIL1();
	}
	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		InvalidateDL1();
	}
	
	num_insn_in_prefetch_buffer = 0;
	cur_insn_in_prefetch_buffer = 0;

	effective_address = 0;
}

template <class CONFIG>
uint32_t CPU<CONFIG>::GetSPR(unsigned int n) const
{
	switch(n)
	{
		case 0x001: return GetXER();
		case 0x008: return GetLR();
		case 0x009: return GetCTR();
		case 0x016:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDEC();
		case 0x01a:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetSRR0();
		case 0x01b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetSRR1();
		case 0x030:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPID();
		case 0x03a:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetCSRR0();
		case 0x03b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetCSRR1();
		case 0x03d:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDEAR();
		case 0x03e:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetESR();
		case 0x03f:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetIVPR();
		case 0x100:
			return GetUSPRG0();
		case 0x104:
		case 0x105:
		case 0x106:
		case 0x107:
		{
			unsigned int num_sprg = n - 0x104 + 4;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				return GetSPRG(num_sprg);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x10c:
			return GetTBL();
		case 0x10d:
			return GetTBU();
		case 0x0110:
		case 0x0111:
		case 0x0112:
		case 0x0113:
		{
			unsigned int num_sprg = n - 0x110;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetSPRG(num_sprg);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x11c:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetTBL();
		case 0x11d:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetTBU();
		case 0x11e:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPIR();
		case 0x11f:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetPVR();
		case 0x130:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDBSR();
		case 0x134:
		case 0x135:
		case 0x136:
		{
			unsigned int num_dbcr = n - 0x134;
			if(num_dbcr < CONFIG::NUM_DBCRS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDBCR(num_dbcr);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x138:
		case 0x139:
		case 0x13a:
		case 0x13b:
		{
			unsigned int num_iac = n - 0x138;
			if(num_iac < CONFIG::NUM_IACS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetIAC(num_iac);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x13c:
		case 0x13d:
		{
			unsigned int num_dac = n - 0x13c;
			if(num_dac < CONFIG::NUM_DACS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDAC(num_dac);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x13e:
		case 0x13f:
		{
			unsigned int num_dvc = n - 0x13e;
			if(num_dvc < CONFIG::NUM_DVCS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDVC(num_dvc);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x150:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetTSR();
		case 0x154:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetTCR();
		case 0x190:
		case 0x191:
		case 0x192:
		case 0x193:
		case 0x194:
		case 0x195:
		case 0x196:
		case 0x197:
		case 0x198:
		case 0x199:
		case 0x19a:
		case 0x19b:
		case 0x19c:
		case 0x19d:
		case 0x19e:
		case 0x19f:
		{
			unsigned int num_ivor = n - 0x190;
			if(num_ivor < CONFIG::NUM_IVORS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetIVOR(num_ivor);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x23a:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMCSRR0();
		case 0x23b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMCSRR1();
		case 0x23c:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMCSR();
		case 0x370:
		case 0x371:
		case 0x372:
		case 0x373:
		{
			unsigned int num_inv = n - 0x370;
			if(num_inv < CONFIG::NUM_INVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetINV(num_inv);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x374:
		case 0x375:
		case 0x376:
		case 0x377:
		{
			unsigned int num_itv = n - 0x374;
			if(num_itv < CONFIG::NUM_ITVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetITV(num_itv);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x378:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetCCR1();
		case 0x390:
		case 0x391:
		case 0x392:
		case 0x393:
		{
			unsigned int num_dnv = n - 0x390;
			if(num_dnv < CONFIG::NUM_DNVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDNV(num_dnv);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x394:
		case 0x395:
		case 0x396:
		case 0x397:
		{
			unsigned int num_dtv = n - 0x394;
			if(num_dtv < CONFIG::NUM_DTVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				return GetDTV(num_dtv);
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x398:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDVLIM();
		case 0x399:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetIVLIM();
		case 0x39b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetRSTCFG();
		case 0x39c:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDCDBTRL();
		case 0x39d:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDCDBTRH();
		case 0x39e:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetICDBTRL();
		case 0x39f:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetICDBTRH();
		case 0x3b2:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetMMUCR();
		case 0x3b3:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetCCR0();
		case 0x3d3:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetICDBDR();
		case 0x3f3:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			return GetDBDR();
	}
	throw IllegalInstructionException<CONFIG>();
	return 0;
}

template <class CONFIG>
void CPU<CONFIG>::SetSPR(unsigned int n, uint32_t value)
{
	switch(n)
	{
		case 0x001: SetXER(value); return;
		case 0x008: SetLR(value); return;
		case 0x009: SetCTR(value); return;
		case 0x016:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDEC(value);
			return;
		case 0x01a:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSRR0(value);
			return;
		case 0x01b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetSRR1(value);
			return;
		case 0x030:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetPID(value);
			return;
		case 0x036:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDECAR(value);
			return;
		case 0x03a:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetCSRR0(value);
			return;
		case 0x03b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetCSRR1(value);
			return;
		case 0x03d:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDEAR(value);
			return;
		case 0x03e:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetESR(value);
			return;
		case 0x03f:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetIVPR(value);
			return;
		case 0x100:
			SetUSPRG0(value);
			return;
		case 0x0110:
		case 0x0111:
		case 0x0112:
		case 0x0113:
		case 0x0114:
		case 0x0115:
		case 0x0116:
		case 0x0117:
		{
			unsigned int num_sprg = n - 0x110;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetSPRG(num_sprg, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x11c:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetTBL(value);
			return;
		case 0x11d:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetTBU(value);
			return;
		case 0x130:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDBSR(GetDBSR() & ~value); // A 1 clears the bit, a 0 leaves it unchanged
			return;
		case 0x134:
		case 0x135:
		case 0x136:
		{
			unsigned int num_dbcr = n - 0x134;
			if(num_dbcr < CONFIG::NUM_DBCRS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDBCR(num_dbcr, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x138:
		case 0x139:
		case 0x13a:
		case 0x13b:
		{
			unsigned int num_iac = n - 0x138;
			if(num_iac < CONFIG::NUM_IACS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetIAC(num_iac, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x13c:
		case 0x13d:
		{
			unsigned int num_dac = n - 0x13c;
			if(num_dac < CONFIG::NUM_DACS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDAC(num_dac, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x13e:
		case 0x13f:
		{
			unsigned int num_dvc = n - 0x13e;
			if(num_dvc < CONFIG::NUM_DVCS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDVC(num_dvc, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x150:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
/*			{
				std::stringstream sstr;
				sstr << "At 0x" << std::hex << GetCIA() << std::dec;
				const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
				if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
				sstr << ", mttsr 0x" << std::hex << value << std::dec;
				std::string msg = sstr.str();
				
				if(trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
			}*/
			SetTSR(GetTSR() & ~value); // A 1 clears the bit, a 0 leaves it unchanged
			return;
		case 0x154:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetTCR(value);
			return;
		case 0x190:
		case 0x191:
		case 0x192:
		case 0x193:
		case 0x194:
		case 0x195:
		case 0x196:
		case 0x197:
		case 0x198:
		case 0x199:
		case 0x19a:
		case 0x19b:
		case 0x19c:
		case 0x19d:
		case 0x19e:
		case 0x19f:
		{
			unsigned int num_ivor = n - 0x190;
			if(num_ivor < CONFIG::NUM_IVORS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetIVOR(num_ivor, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x23a:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMCSRR0(value);
			return;
		case 0x23b:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMCSRR1(value);
			return;
		case 0x23c:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMCSR(GetMCSR() & ~value); // A 1 clears the bit, a 0 leaves it unchanged
			return;
		case 0x370:
		case 0x371:
		case 0x372:
		case 0x373:
		{
			unsigned int num_inv = n - 0x370;
			if(num_inv < CONFIG::NUM_INVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetINV(num_inv, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x374:
		case 0x375:
		case 0x376:
		case 0x377:
		{
			unsigned int num_itv = n - 0x374;
			if(num_itv < CONFIG::NUM_ITVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetITV(num_itv, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x378:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetCCR1(value);
			return;
		case 0x390:
		case 0x391:
		case 0x392:
		case 0x393:
		{
			unsigned int num_dnv = n - 0x390;
			if(num_dnv < CONFIG::NUM_DNVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDNV(num_dnv, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x394:
		case 0x395:
		case 0x396:
		case 0x397:
		{
			unsigned int num_dtv = n - 0x394;
			if(num_dtv < CONFIG::NUM_DTVS)
			{
				if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
				SetDTV(num_dtv, value);
				return;
			}
			throw IllegalInstructionException<CONFIG>();
		}
		case 0x398:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDVLIM(value);
			return;
		case 0x399:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetIVLIM(value);
			return;
		case 0x3b2:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetMMUCR(value);
			return;
		case 0x3b3:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetCCR0(value);
			return;
		case 0x3d3:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetICDBDR(value);
			return;
		case 0x3f3:
			if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();
			SetDBDR(value);
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
				if(loader_import)
				{
					loader_import->Load();
				}
			}
		} while(1);
	}

	typename CONFIG::address_t sequential_nia = GetCIA() + 4;
	unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation = 0;

	SetNIA(sequential_nia);

	try
	{
		typename CONFIG::address_t addr = GetCIA();
		uint32_t insn;

		if(CONFIG::PREFETCH_BUFFER_ENABLE)
		{
			if(unlikely(cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer))
			{
				uint32_t size_to_block_boundary = IsInsnCacheEnabled() ? CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (addr & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1)) : CONFIG::FSB_WIDTH - (addr & (CONFIG::FSB_WIDTH - 1));
				uint32_t size_to_prefetch = size_to_block_boundary > (4 * CONFIG::NUM_PREFETCH_BUFFER_ENTRIES) ? CONFIG::NUM_PREFETCH_BUFFER_ENTRIES * 4 : size_to_block_boundary;
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

		if(unlikely(requires_memory_access_reporting)) 
		{
			if(unlikely(memory_access_reporting_import != 0))
			{
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_READ, MemoryAccessReporting<typename CONFIG::address_t>::MT_INSN, addr, 4);
			}
		}

		operation = unisim::component::cxx::processor::powerpc::ppc440::Decoder<CONFIG>::Decode(addr, insn);

//		stringstream sstr;
//		operation->disasm((CPU<CONFIG> *) this, sstr);
//		std::cerr << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << std::endl;

		if(unlikely(IsVerboseStep()))
		{
			stringstream sstr;
			operation->disasm((CPU<CONFIG> *) this, sstr);
			logger << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":0x" << std::hex << operation->GetEncoding() << std::dec << ":" << sstr.str() << endl << EndDebugInfo;
		}

		/* execute the instruction */
		operation->execute(this);

		if(unlikely(HasIRQ()))
		{
			if(GetMSR_CE() && HasCriticalInputInterrupt()) throw CriticalInputInterruptException<CONFIG>();
			if(GetTCR_WIE() && GetMSR_CE() && HasWatchDogTimerInterrupt()) throw WatchDogTimerInterruptException<CONFIG>();
			if(GetMSR_EE() && HasExternalInputInterrupt()) throw ExternalInputInterruptException<CONFIG>();
			if(GetTCR_FIE() && GetMSR_EE() && HasFixedIntervalTimerInterrupt()) throw FixedIntervalTimerInterruptException<CONFIG>();
			if(GetTCR_DIE() && GetMSR_EE() && HasDecrementerInterrupt()) throw DecrementerInterruptException<CONFIG>();
		}
	}
	catch(SystemCallException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(MachineCheckException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(DecrementerInterruptException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(ExternalInputInterruptException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(CriticalInputInterruptException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(DSIException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(ISIException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(DataTLBErrorException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(InstructionTLBErrorException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(AlignmentException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(ProgramException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(FloatingPointUnavailableException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(AuxiliaryProcessorUnavailableException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(FixedIntervalTimerInterruptException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(WatchDogTimerInterruptException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(DebugInterruptException<CONFIG>& exc) { HandleException(exc, operation); }
	catch(Exception& exc)
	{
		if(unlikely(IsVerboseStep()))
			logger << DebugError << "uncaught processor exception :" << exc.what() << endl << EndDebugError;
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
	
	//DL1SanityCheck();
	//IL1SanityCheck();
}

template <class CONFIG>
void CPU<CONFIG>::OnTimerClock()
{
	timer_cycle++;
	
	/* update the time base */
	IncrementTB();

	/* decrement the decrementer each timer cycle */
	DecrementDEC();
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
	return CONFIG::DL1_CONFIG::ENABLE;
}

template <class CONFIG>
inline bool CPU<CONFIG>::IsInsnCacheEnabled()
{
	return CONFIG::IL1_CONFIG::ENABLE;
}

template <class CONFIG>
void CPU<CONFIG>::SetMSR(uint32_t value)
{
	uint32_t old_msr_we = GetMSR_WE();
	uint32_t old_msr_ce = GetMSR_CE();
	uint32_t old_msr_ee = GetMSR_EE();
	uint32_t old_msr_pr = GetMSR_PR();
	uint32_t old_msr_fp = GetMSR_FP();
	uint32_t old_msr_me = GetMSR_ME();
	uint32_t old_msr_fe = GetMSR_FE0() | GetMSR_FE1();
	uint32_t old_msr_dwe = GetMSR_DWE();
	uint32_t old_msr_de = GetMSR_DE();
	uint32_t old_msr_is = GetMSR_IS();
	uint32_t old_msr_ds = GetMSR_DS();

	msr = (msr & ~CONFIG::MSR_MASK) | (value & CONFIG::MSR_MASK);

	if(!old_msr_we && GetMSR_WE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Processor enters in the wait state" << endl << EndDebugInfo;
	}

	if(old_msr_we && !GetMSR_WE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Processor leaves the wait state" << endl << EndDebugInfo;
	}

	if(!old_msr_ce && GetMSR_CE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling critical input and watchdog interrupts" << endl << EndDebugInfo;
	}

	if(old_msr_ce && !GetMSR_CE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling critical input and watchdog interrupts" << endl << EndDebugInfo;
	}

	if(!old_msr_ee && GetMSR_EE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling external input interrupt" << endl << EndDebugInfo;
	}

	if(old_msr_ee && !GetMSR_EE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling external input interrupt" << endl << EndDebugInfo;
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

	if(!old_msr_me && GetMSR_ME())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling machine check interrupts" << endl << EndDebugInfo;
	}

	if(old_msr_me && !GetMSR_ME())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling machine check interrupts" << endl << EndDebugInfo;
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

	if(!old_msr_dwe && GetMSR_DWE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling debug wait mode" << endl << EndDebugInfo;
	}

	if(old_msr_dwe && !GetMSR_DWE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling debug wait mode" << endl << EndDebugInfo;
	}

	if(!old_msr_de && GetMSR_DE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Enabling debug interrupts" << endl << EndDebugInfo;
	}

	if(old_msr_de && !GetMSR_DE())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Disabling debug interrupts" << endl << EndDebugInfo;
	}

	if(!old_msr_is && GetMSR_IS())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching instruction fetch accesses to the application address space" << endl << EndDebugInfo;
	}

	if(old_msr_is && !GetMSR_IS())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching instruction fetch accesses to the system address space" << endl << EndDebugInfo;
	}

	if(!old_msr_ds && GetMSR_DS())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching load/store accesses to the application address space" << endl << EndDebugInfo;
	}

	if(old_msr_ds && !GetMSR_DS())
	{
		if(unlikely(IsVerboseSetMSR()))
			logger << DebugInfo << "Switching load/store accesses to the system address space" << endl << EndDebugInfo;
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
bool CPU<CONFIG>::BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	return memory_import->WriteMemory(physical_addr, buffer, size);
}



/* Endian interface */
template <class CONFIG>
endian_type CPU<CONFIG>::GetEndianess()
{
	return E_BIG_ENDIAN;
}

template <class CONFIG>
void CPU<CONFIG>::SetIRQ(unsigned int _irq)
{
	if(IsVerboseException())
	{
		if(_irq & CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT)
		{
			logger << DebugInfo << "Got an external input interrupt condition" << EndDebugInfo;
		}
		if(_irq & CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT)
		{
			logger << DebugInfo << "Got a critical input interrupt condition" << EndDebugInfo;
		}
		if(_irq & CONFIG::IRQ_DECREMENTER_INTERRUPT)
		{
			logger << DebugInfo << "Got a decrementer interrupt condition" << EndDebugInfo;
		}
		if(_irq & CONFIG::IRQ_FIXED_INTERVAL_TIMER_INTERRUPT)
		{
			logger << DebugInfo << "Got a fixed interval timer interrupt condition" << EndDebugInfo;
		}
		if(_irq & CONFIG::IRQ_WATCHDOG_TIMER_INTERRUPT)
		{
			logger << DebugInfo << "Got a watchdog timer interrupt condition" << EndDebugInfo;
		}
	}
	irq = irq | (_irq & (CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT | CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT));
	if(_irq & CONFIG::IRQ_DECREMENTER_INTERRUPT) SetTSR(GetTSR() | CONFIG::TSR_DIS_MASK);
	if(_irq & CONFIG::IRQ_FIXED_INTERVAL_TIMER_INTERRUPT) SetTSR(GetTSR() | CONFIG::TSR_FIS_MASK);
	if(_irq & CONFIG::IRQ_WATCHDOG_TIMER_INTERRUPT)
	{
		if(GetTSR_ENW())
		{
			if(GetTSR_WIS())
			{
				if(GetTCR_WRC() != 0)
				{
					SetTCR((GetTSR() & ~CONFIG::TSR_WRS_MASK) | ((GetTCR_WRC() << CONFIG::TSR_WRS_OFFSET) & CONFIG::TSR_WRS_MASK)); // TSR[WRS]=TCR[WRC]
					logger << DebugWarning << "A reset because of watchdog timer occured" << EndDebugWarning;
					Stop(0);
				}
				else
				{
					// nothing
				}
			}
			else
			{
				SetTSR(GetTSR() | CONFIG::TSR_WIS_MASK);
			}
		}
		else
		{
			SetTSR(GetTSR() | CONFIG::TSR_ENW_MASK);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::ResetIRQ(unsigned int _irq)
{
	irq = irq & (~_irq & (CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT | CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT));
	if(_irq & CONFIG::IRQ_DECREMENTER_INTERRUPT) SetTSR(GetTSR() & ~CONFIG::TSR_DIS_MASK);
	if(_irq & CONFIG::IRQ_FIXED_INTERVAL_TIMER_INTERRUPT) SetTSR(GetTSR() & ~CONFIG::TSR_FIS_MASK);
	if(_irq & CONFIG::IRQ_WATCHDOG_TIMER_INTERRUPT) SetTSR(GetTSR() & ~CONFIG::TSR_WIS_MASK);
	if(IsVerboseException())
	{
		logger << DebugInfo << "Reseting pending IRQ mask: " << irq << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
}

template <class CONFIG>
void CPU<CONFIG>::Isync()
{
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();
}

template <class CONFIG>
void CPU<CONFIG>::Rfi()
{
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();

	if(unlikely(GetMSR_PR())) throw PrivilegeViolationException<CONFIG>();

	SetNIA(GetSRR0());
	SetMSR(GetSRR1());
}

template <class CONFIG>
void CPU<CONFIG>::Rfci()
{
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();

	if(unlikely(GetMSR_PR())) throw PrivilegeViolationException<CONFIG>();

	SetNIA(GetCSRR0());
	SetMSR(GetCSRR1());
}

template <class CONFIG>
void CPU<CONFIG>::Rfmci()
{
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();

	if(unlikely(GetMSR_PR())) throw PrivilegeViolationException<CONFIG>();

	SetNIA(GetMCSRR0());
	SetMSR(GetMCSRR1());
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

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
