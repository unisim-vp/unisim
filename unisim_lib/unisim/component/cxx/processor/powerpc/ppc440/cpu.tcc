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
#include <unisim/component/cxx/cache/cache.tcc>
#include <unisim/component/cxx/tlb/tlb.tcc>
#include <unisim/util/queue/queue.tcc>
#include <unisim/kernel/debug/debug.hh>

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifdef powerpc
#undef powerpc
#endif

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
	, Client<Loader>(name,  parent)
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
	, enable_linux_syscall_snooping(false)
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, enable_trap_on_exception(false)
	, halt_on_addr((typename CONFIG::address_t) -1)
	, halt_on()
	, max_inst(0xffffffffffffffffULL)
	, num_interrupts(0)
	, num_decrementer_interrupts(0)
	, num_fixed_interval_timer_interrupts(0)
	, num_watchdog_timer_interrupts(0)
	, num_debug_interrupts(0)
	, num_external_input_interrupts(0)
	, num_critical_input_interrupts(0)
	, num_machine_check_interrupts(0)
	, num_data_storage_interrupts(0)
	, num_instruction_storage_interrupts(0)
	, num_data_tlb_error_interrupts(0)
	, num_instruction_tlb_error_interrupts(0)
	, num_alignment_interrupts(0)
	, num_program_interrupts(0)
	, num_system_call_interrupts(0)
	, num_floating_point_unavailable_interrupts(0)
	, num_auxiliary_processor_unavailable_interrupts(0)
	, registers_registry()
	, instruction_counter(0)
	, fp32_estimate_inv_warning(false)
	, fp64_estimate_inv_sqrt_warning(false)
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
	, exc_flags(0)
	, exc_mask(CONFIG::EXC_MASK_NON_MASKABLE)
	, exc_addr(0)
	, exc_memory_access_type(CONFIG::MAT_READ)
	, enter_isr_table()
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
	, param_enable_linux_syscall_snooping("enable-linux-syscall-snooping", this, enable_linux_syscall_snooping, "enable/disable linux syscall snooping")
	, param_trap_on_instruction_counter("trap-on-instruction-counter",  this,  trap_on_instruction_counter, "number of simulated instruction before traping")
	, param_enable_trap_on_exception("enable-trap-on-exception", this, enable_trap_on_exception, "enable/disable trap reporting on exception")
	, param_halt_on("halt-on", this, halt_on, "Symbol or address where to stop simulation")
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
	, stat_num_interrupts("num-interrupts", this, num_interrupts, "Number of interrupts")
	, stat_num_decrementer_interrupts("num-decrementer-interrupts", this, num_decrementer_interrupts, "Number decrementer interrupts")
	, stat_num_fixed_interval_timer_interrupts("num-fixed-interval-timer-interrupts", this, num_fixed_interval_timer_interrupts, "Number of fixed interval timer interrupts")
	, stat_num_watchdog_timer_interrupts("num-watchdog-timer-interrupts", this, num_watchdog_timer_interrupts, "Number of watchdog timer interrupts")
	, stat_num_debug_interrupts("num-debug-interrupts", this, num_debug_interrupts, "Number of debug interrupts")
	, stat_num_external_input_interrupts("num-external-input-interrupts", this, num_external_input_interrupts, "Number of external input interrupts")
	, stat_num_critical_input_interrupts("num-critical-input-interrupts", this, num_critical_input_interrupts, "Number of critical input interrupts")
	, stat_num_machine_check_interrupts("num-machine-check-interrupts", this, num_machine_check_interrupts, "Number of machine check interrupts")
	, stat_num_data_storage_interrupts("num-data-storage-interrupts", this, num_data_storage_interrupts, "Number of data storage interrupts")
	, stat_num_instruction_storage_interrupts("num-instruction-storage-interrupts", this, num_instruction_storage_interrupts, "Number of instruction storage interrupts")
	, stat_num_data_tlb_error_interrupts("num-data-tlb-error-interrupts", this, num_data_tlb_error_interrupts, "Number of data TLB error interrupts")
	, stat_num_instruction_tlb_error_interrupts("num-instruction-tlb-error-interrupts", this, num_instruction_tlb_error_interrupts, "Number of instruction TLB error interrupts")
	, stat_num_alignment_interrupts("num-alignment-interrupts", this, num_alignment_interrupts, "Number of alignment interrupts")
	, stat_num_program_interrupts("num-program-interrupts", this, num_program_interrupts, "Number of program interrupts")
	, stat_num_system_call_interrupts("num-system-call-interrupts", this, num_system_call_interrupts, "Number of system call interrupts")
	, stat_num_floating_point_unavailable_interrupts("num-floating-point-unavailable-interrupts", this, num_floating_point_unavailable_interrupts, "Number of floating point unavailable interrupts")
	, stat_num_auxiliary_processor_unavailable_interrupts("num-auxiliary-processor-unavailable-interrupts", this, num_auxiliary_processor_unavailable_interrupts, "Number of auxiliary processor unavailable interrupts")
{
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_cpu_cycle_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_voltage.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	unsigned int i;

	registers_registry["cia"] = new unisim::util::debug::SimpleRegister<uint32_t>("cia", &cia);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("cia", this, cia, "Current Instruction Address"));

	registers_registry["cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("cr", &cr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("cr", this, cr, "Condition Register"));
	registers_registry["ctr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ctr", &ctr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ctr", this, ctr, "Control Register"));
	registers_registry["lr"] = new unisim::util::debug::SimpleRegister<uint32_t>("lr", &lr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("lr", this, lr, "Link Register"));

	for(i = 0; i < CONFIG::NUM_DNVS; i++)
	{
		stringstream sstr;
		sstr << "dnv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dnv[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dnv[i], "Data Cache Normal Victim"));
	}

	for(i = 0; i < CONFIG::NUM_DTVS; i++)
	{
		stringstream sstr;
		sstr << "dtv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dtv[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dtv[i], "Data Cache Transient Victim"));
	}

	registers_registry["dvlim"] = new unisim::util::debug::SimpleRegister<uint32_t>("dvlim", &dvlim);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dvlim", this, dvlim, "Data Cache Victim Limit"));

	for(i = 0; i < CONFIG::NUM_INVS; i++)
	{
		stringstream sstr;
		sstr << "inv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &inv[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, inv[i], "Instruction Cache Normal Victim"));
	}

	for(i = 0; i < CONFIG::NUM_ITVS; i++)
	{
		stringstream sstr;
		sstr << "itv" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &itv[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, itv[i], "Instruction Cache Transient Victim"));
	}

	registers_registry["ivlim"] = new unisim::util::debug::SimpleRegister<uint32_t>("ivlim", &ivlim);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ivlim", this, dvlim, "Instruction Cache Victim Limit"));

	registers_registry["dcdbtrh"] = new unisim::util::debug::SimpleRegister<uint32_t>("dcdbtrh", &dcdbtrh);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dcdbtrh", this, dcdbtrh, "Data Cache Debug Tag Register High"));
	registers_registry["dcdbtrl"] = new unisim::util::debug::SimpleRegister<uint32_t>("dcdbtrl", &dcdbtrl);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dcdbtrl", this, dcdbtrl, "Data Cache Debug Tag Register Low"));
	registers_registry["icdbdr"] = new unisim::util::debug::SimpleRegister<uint32_t>("icdbdr", &icdbdr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("icdbdr", this, icdbdr, "Instruction Cache Debug Data Register"));
	registers_registry["icdbtrh"] = new unisim::util::debug::SimpleRegister<uint32_t>("icdbtrh", &icdbtrh);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("icdbtrh", this, icdbtrh, "Instruction Cache Debug Tag Register High"));
	registers_registry["icdbtrl"] = new unisim::util::debug::SimpleRegister<uint32_t>("icdbtrl", &icdbtrl);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("icdbtrl", this, icdbtrl, "Instruction Cache Debug Tag Register Low"));

	for(i = 0; i < CONFIG::NUM_DACS; i++)
	{
		stringstream sstr;
		sstr << "dac" << (i + 1);
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dac[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dac[i], "Data Address Compare"));
	}

	for(i = 0; i < CONFIG::NUM_DBCRS; i++)
	{
		stringstream sstr;
		sstr << "dbcr" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dbcr[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dbcr[i], "Debug Control Register"));
	}

	registers_registry["dbdr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dbdr", &dbdr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dbdr", this, dbdr, "Debug Data Register"));
	registers_registry["dbsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dbsr", &dbsr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dbsr", this, dbsr, "Debug Status Register"));

	for(i = 0; i < CONFIG::NUM_DVCS; i++)
	{
		stringstream sstr;
		sstr << "dvc" << (i + 1);
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dvc[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dvc[i], "Data Value Compare"));
	}

	for(i = 0; i < CONFIG::NUM_IACS; i++)
	{
		stringstream sstr;
		sstr << "iac" << (i + 1);
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &iac[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, iac[i], "Instruction Address Compare"));
	}

	for(i = 0; i < CONFIG::NUM_GPRS; i++)
	{
		stringstream sstr;
		sstr << "r" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &gpr[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, gpr[i], "General Purpose Register"));
	}

	registers_registry["xer"] = new unisim::util::debug::SimpleRegister<uint32_t>("xer", &xer);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("xer", this, xer, "Integer Exception Register"));

	for(i = 0; i < CONFIG::NUM_FPRS; i++)
	{
		stringstream sstr;
		sstr << "f" << i;
		registers_registry[sstr.str()] = new FloatingPointRegisterInterface(sstr.str().c_str(), &fpr[i]);
		registers_registry2.push_back(new FloatingPointRegisterView(sstr.str().c_str(), this, fpr[i], "Floating-Point Register"));
	}

	registers_registry["fpscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpscr", &fpscr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("fpscr", this, fpscr, "Floating-Point Status and Control Register"));

	registers_registry["csrr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("csrr0", &csrr0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("csrr0", this, csrr0, "Critical Save/Restore Register"));
	registers_registry["csrr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("csrr1", &csrr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("csrr1", this, csrr1, "Critical Save/Restore Register"));
	registers_registry["dear"] = new unisim::util::debug::SimpleRegister<uint32_t>("dear", &dear);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dear", this, dear, "Data Exception Address Register"));
	registers_registry["esr"] = new unisim::util::debug::SimpleRegister<uint32_t>("esr", &esr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("esr", this, esr, "Exception Syndrome Register"));

	for(i = 0; i < CONFIG::NUM_IVORS; i++)
	{
		stringstream sstr;
		sstr << "ivor" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &ivor[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, ivor[i], "Interrupt Vector Offset Register"));
	}

	registers_registry["ivpr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ivpr", &ivpr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ivpr", this, ivpr, "Interrupt Vector Prefix Register"));
	registers_registry["mcsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("mcsr", &mcsr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("mcsr", this, mcsr, "Machine Check Status Register"));
	registers_registry["mcsrr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("mcsrr0", &mcsrr0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("mcsrr0", this, mcsrr0, "Machine Check Save/Restore Register"));
	registers_registry["mcsrr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("mcsrr1", &mcsrr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("mcsrr1", this, mcsrr1, "Machine Check Save/Restore Register"));
	registers_registry["srr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr0", &srr0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("srr0", this, srr0, "Save/Restore Register"));
	registers_registry["srr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr1", &srr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("srr1", this, srr1, "Save/Restore Register"));

	registers_registry["ccr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("ccr0", &ccr0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ccr0", this, ccr0, "Core Configuration Register"));
	registers_registry["ccr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("ccr1", &ccr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ccr1", this, ccr1, "Core Configuration Register"));
	registers_registry["msr"] = new unisim::util::debug::SimpleRegister<uint32_t>("msr", &msr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("msr", this, msr, "Machine State Register"));
	registers_registry["pir"] = new unisim::util::debug::SimpleRegister<uint32_t>("pir", &pir);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pir", this, pir, "Processor Identification Register"));
	registers_registry["pvr"] = new unisim::util::debug::SimpleRegister<uint32_t>("pvr", &pvr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pvr", this, pvr, "Processor Version Register"));
	registers_registry["rstcfg"] = new unisim::util::debug::SimpleRegister<uint32_t>("rstcfg", &rstcfg);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("rstcfg", this, rstcfg, "Reset Configuration"));
	
	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
	{
		stringstream sstr;
		sstr << "sprg" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sprg[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, sprg[i], "Special Purpose Register General"));
	}

	registers_registry["usprg0"] = new unisim::util::debug::SimpleRegister<uint32_t>("usprg0", &usprg0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("usprg0", this, usprg0, "Special Purpose Register General"));

	registers_registry["mmucr"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmucr", &mmucr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("mmucr", this, mmucr, "Memory Management Unit Control Register"));
	registers_registry["pid"] = new unisim::util::debug::SimpleRegister<uint32_t>("pid", &pid);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pid", this, pid, "Process ID"));

	registers_registry["dec"] = new unisim::util::debug::SimpleRegister<uint32_t>("dec", &dec);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dec", this, dec, "Decrementer"));
	registers_registry["decar"] = new unisim::util::debug::SimpleRegister<uint32_t>("decar", &decar);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("decar", this, decar, "Decrementer Auto-Reload"));
	registers_registry["tbl"] = new TimeBaseRegisterInterface("tbl", &tb, TimeBaseRegisterInterface::TB_LOW);
	registers_registry2.push_back(new TimeBaseRegisterView("tbl", this, tb, TimeBaseRegisterView::TB_LOW, "Time Base Lower"));
	registers_registry["tbu"] = new TimeBaseRegisterInterface("tbu", &tb, TimeBaseRegisterInterface::TB_HIGH);
	registers_registry2.push_back(new TimeBaseRegisterView("tbu", this, tb, TimeBaseRegisterView::TB_HIGH, "Time Base Lower"));
	registers_registry["tcr"] = new unisim::util::debug::SimpleRegister<uint32_t>("tcr", &tcr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("tcr", this, tcr, "Timer Control Register"));
	registers_registry["tsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("tsr", &tsr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("tsr", this, tsr, "Timer Status Register"));

	enter_isr_table[CONFIG::EXC_DATA_STORAGE_READ_ACCESS_CONTROL] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_WRITE_ACCESS_CONTROL] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_BYTE_ORDERING] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_CACHE_LOCKING] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_STORAGE_EXECUTE_ACCESS_CONTROL] = &CPU<CONFIG>::EnterInstructionStorageISR;
	enter_isr_table[CONFIG::EXC_ALIGNMENT] = &CPU<CONFIG>::EnterAlignmentISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_TRAP] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_FLOATING_POINT] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_UNIMPLEMENTED_INSTRUCTION] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_FLOATING_POINT_UNAVAILABLE] = &CPU<CONFIG>::EnterFloatingPointUnavailableISR;
	enter_isr_table[CONFIG::EXC_SYSTEM_CALL] = &CPU<CONFIG>::EnterSystemCallISR;
	enter_isr_table[CONFIG::EXC_AUXILIARY_PROCESSOR_UNAVAILABLE] = &CPU<CONFIG>::EnterAuxiliaryProcessorUnavailableISR;
	enter_isr_table[CONFIG::EXC_DATA_TLB_ERROR] = &CPU<CONFIG>::EnterDataTLBErrorISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_TLB_ERROR] = &CPU<CONFIG>::EnterInstructionTLBErrorISR;
	enter_isr_table[CONFIG::EXC_MACHINE_CHECK_INSTRUCTION_SYNCHRONOUS] = &CPU<CONFIG>::EnterMachineCheckISR;
	enter_isr_table[CONFIG::EXC_MACHINE_CHECK_INSTRUCTION_ASYNCHRONOUS] = &CPU<CONFIG>::EnterMachineCheckISR;
	enter_isr_table[CONFIG::EXC_MACHINE_CHECK_DATA_ASYNCHRONOUS] = &CPU<CONFIG>::EnterMachineCheckISR;
	enter_isr_table[CONFIG::EXC_MACHINE_CHECK_TLB_ASYNCHRONOUS] = &CPU<CONFIG>::EnterMachineCheckISR;
	enter_isr_table[CONFIG::EXC_DEBUG] = &CPU<CONFIG>::EnterDebugISR;
	enter_isr_table[CONFIG::EXC_CRITICAL_INPUT] = &CPU<CONFIG>::EnterCriticalInputISR;
	enter_isr_table[CONFIG::EXC_WATCHDOG_TIMER] = &CPU<CONFIG>::EnterWatchDogTimerISR;
	enter_isr_table[CONFIG::EXC_EXTERNAL_INPUT] = &CPU<CONFIG>::EnterExternalInputISR;
	enter_isr_table[CONFIG::EXC_FIXED_INTERVAL_TIMER] = &CPU<CONFIG>::EnterFixedIntervalTimerISR;
	enter_isr_table[CONFIG::EXC_DECREMENTER] = &CPU<CONFIG>::EnterDecrementerISR;
	
	Reset();
	
	std::stringstream sstr_description;
	sstr_description << "This module implements a PPC440 CPU core. It has the following characteristics:" << std::endl;
	sstr_description << "Processor version (PVR value): 0x" << std::hex << CONFIG::PROCESSOR_VERSION << std::dec << std::endl;
	sstr_description << "Reset configuration (RSTCFG): U0=" << GetRSTCFG_U0()  << ", U1=" << GetRSTCFG_U1() << ", U2=" << GetRSTCFG_U2() << ", U3=" << GetRSTCFG_U3() << ", E=" << GetRSTCFG_E() << ", ERPN=0x" << std::hex << GetRSTCFG_ERPN() << std::dec << std::endl;
	sstr_description << "Start address: 0x" << std::hex << CONFIG::START_ADDR << std::dec << std::endl;
	if(CONFIG::HAS_DCACHE)
	{
		sstr_description << "L1 data cache: size=" << CONFIG::DL1_CONFIG::CACHE_SIZE << " bytes, block size=" << CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE << " bytes, associativity=" << CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::HAS_ICACHE)
	{
		sstr_description << "L1 instruction cache: size=" << CONFIG::IL1_CONFIG::CACHE_SIZE << " bytes, block size=" << CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE << " bytes, associativity=" << CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::HAS_MMU)
	{
		sstr_description << "shadow instruction TLB: size=" << CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES << " entries, associativity=" << CONFIG::ITLB_CONFIG::TLB_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::HAS_MMU)
	{
		sstr_description << "shadow data TLB: size=" << CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES << " entries, associativity=" << CONFIG::DTLB_CONFIG::TLB_ASSOCIATIVITY << std::endl;
	}
	if(CONFIG::HAS_MMU)
	{
		sstr_description << "unified TLB: size=" << CONFIG::UTLB_CONFIG::TLB_NUM_ENTRIES << " entries, associativity=" << CONFIG::UTLB_CONFIG::TLB_ASSOCIATIVITY << std::endl;
	}
	sstr_description << "FSB/PLB burst size:" << (8 * CONFIG::FSB_BURST_SIZE) << " bits" << std::endl;
	sstr_description << "FSB/PLB width:" << (8 * CONFIG::FSB_WIDTH) << " bits" << std::endl;
	sstr_description << "MMU: " << (CONFIG::HAS_MMU ? "yes" : "no") << std::endl;
	sstr_description << "FPU APU: " << (CONFIG::HAS_FPU ? "yes" : "no");
	
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
	map<string, unisim::service::interfaces::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}

	unsigned int i;
	unsigned int n = registers_registry2.size();
	for(i = 0; i < n; i++)
	{
		delete registers_registry2[i];
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
	
	if(IsVerboseSetup())
	{
		logger << DebugInfo << Object::GetDescription() << EndDebugInfo;
	}
	
	Reset();

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::EndSetup()
{
	unsigned int min_cycle_time = 0;

	if(CONFIG::HAS_MMU && itlb_power_mode_import)
	{
		unsigned int itlb_min_cycle_time = itlb_power_mode_import->GetMinCycleTime();
		if(itlb_min_cycle_time > 0 && itlb_min_cycle_time > min_cycle_time) min_cycle_time = itlb_min_cycle_time;
		unsigned int itlb_default_voltage = itlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = itlb_default_voltage;
	}

	if(CONFIG::HAS_MMU && dtlb_power_mode_import)
	{
		unsigned int dtlb_min_cycle_time = dtlb_power_mode_import->GetMinCycleTime();
		if(dtlb_min_cycle_time > 0 && dtlb_min_cycle_time > min_cycle_time) min_cycle_time = dtlb_min_cycle_time;
		unsigned int dtlb_default_voltage = dtlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = dtlb_default_voltage;
	}

	if(CONFIG::HAS_MMU && utlb_power_mode_import)
	{
		unsigned int utlb_min_cycle_time = utlb_power_mode_import->GetMinCycleTime();
		if(utlb_min_cycle_time > 0 && utlb_min_cycle_time > min_cycle_time) min_cycle_time = utlb_min_cycle_time;
		unsigned int utlb_default_voltage = utlb_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = utlb_default_voltage;
	}

	if(CONFIG::HAS_ICACHE && il1_power_mode_import)
	{
		unsigned int il1_min_cycle_time = il1_power_mode_import->GetMinCycleTime();
		if(il1_min_cycle_time > 0 && il1_min_cycle_time > min_cycle_time) min_cycle_time = il1_min_cycle_time;
		unsigned int il1_default_voltage = il1_power_mode_import->GetDefaultVoltage();
		if(voltage <= 0) voltage = il1_default_voltage;
	}
	
	if(CONFIG::HAS_DCACHE && dl1_power_mode_import)
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
					logger << "cpu cycle time should be >= " << min_cycle_time << " ps.";
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
			logger << "user must provide a cpu cycle time > 0";
			logger << EndDebugError;
			return false;
		}
	}

	if(unlikely(IsVerboseSetup()))
	{
		logger << DebugInfo << "cpu cycle time of " << cpu_cycle_time << " ps" << EndDebugInfo;
	}

	if(voltage <= 0)
	{
		logger << DebugError << "user must provide a voltage > 0" << EndDebugError;
		return false;
	}

	if(unlikely(IsVerboseSetup()))
	{
		logger << DebugInfo;
		
		logger << "voltage of " << ((double) voltage / 1e3) << " V";

		logger << EndDebugInfo;
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

	if(!halt_on.empty())
	{
		const Symbol<typename CONFIG::address_t> *halt_on_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByName(halt_on.c_str(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		
		if(halt_on_symbol)
		{
			halt_on_addr = halt_on_symbol->GetAddress();
			if(IsVerboseSetup())
			{
				logger << DebugInfo << "Simulation will halt at '" << halt_on_symbol->GetName() << "' (0x" << std::hex << halt_on_addr << std::dec << ")" << EndDebugInfo;
			}
		}
		else
		{
			std::stringstream sstr(halt_on);
			sstr >> std::hex;
			if(sstr >> halt_on_addr)
			{
				if(IsVerboseSetup())
				{
					logger << DebugInfo <<  "Simulation will halt at 0x" << std::hex << halt_on_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				logger << DebugWarning << "Invalid address (" << halt_on << ") in Parameter " << param_halt_on.GetName() << EndDebugWarning;
				halt_on_addr = (typename CONFIG::address_t) -1;
			}
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
	num_interrupts = 0;
	num_decrementer_interrupts = 0;
	num_fixed_interval_timer_interrupts = 0;
	num_watchdog_timer_interrupts = 0;
	num_external_input_interrupts = 0;
	num_critical_input_interrupts = 0;
	num_machine_check_interrupts = 0;
	num_data_storage_interrupts = 0;
	num_instruction_storage_interrupts = 0;
	num_data_tlb_error_interrupts = 0;
	num_instruction_tlb_error_interrupts = 0;
	num_alignment_interrupts = 0;
	num_program_interrupts = 0;
	num_system_call_interrupts = 0;
	num_floating_point_unavailable_interrupts = 0;
	num_auxiliary_processor_unavailable_interrupts = 0;
	num_debug_interrupts = 0;
	instruction_counter = 0;

	exc_flags = 0;
	exc_mask = CONFIG::EXC_MASK_NON_MASKABLE;
	exc_addr = 0;
	exc_memory_access_type = CONFIG::MAT_READ;

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

	usprg0 = 0;
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
	pir = 0;
	dec = 0;
	decar = 0;
	tcr = 0;
	tsr = 0;

	CPU<CONFIG>::InvalidateDecodingCache();
	
	ResetDTLB();
	ResetITLB();
	ResetUTLB();

	if(CONFIG::HAS_ICACHE)
	{
		InvalidateIL1();
	}
	if(CONFIG::HAS_DCACHE)
	{
		InvalidateDL1();
	}

	FlushSubsequentInstructions();
}

template <class CONFIG>
bool CPU<CONFIG>::GetSPR(unsigned int n, uint32_t& value)
{
	switch(n)
	{
		case 0x001:
			value = GetXER();
			return true;
		case 0x008:
			value = GetLR();
			return true;
		case 0x009:
			value = GetCTR();
			return true;
		case 0x016:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			RunInternalTimers();
			value = GetDEC();
			return true;
		case 0x01a:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetSRR0();
			return true;
		case 0x01b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetSRR1();
			return true;
		case 0x030:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPID();
			return true;
		case 0x03a:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetCSRR0();
			return true;
		case 0x03b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetCSRR1();
			return true;
		case 0x03d:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDEAR();
			return true;
		case 0x03e:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetESR();
			return true;
		case 0x03f:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetIVPR();
			return true;
		case 0x100:
			value = GetUSPRG0();
			return true;
		case 0x104:
		case 0x105:
		case 0x106:
		case 0x107:
		{
			unsigned int num_sprg = n - 0x104 + 4;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				value = GetSPRG(num_sprg);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x10c:
			if(!linux_os_import) RunInternalTimers();
			value = GetTBL();
			return true;
		case 0x10d:
			if(!linux_os_import) RunInternalTimers();
			value = GetTBU();
			return true;
		case 0x0110:
		case 0x0111:
		case 0x0112:
		case 0x0113:
		{
			unsigned int num_sprg = n - 0x110;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetSPRG(num_sprg);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x11c:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetTBL();
			return true;
		case 0x11d:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetTBU();
			return true;
		case 0x11e:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPIR();
			return true;
		case 0x11f:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPVR();
			return true;
		case 0x130:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDBSR();
			return true;
		case 0x134:
		case 0x135:
		case 0x136:
		{
			unsigned int num_dbcr = n - 0x134;
			if(num_dbcr < CONFIG::NUM_DBCRS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetDBCR(num_dbcr);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x138:
		case 0x139:
		case 0x13a:
		case 0x13b:
		{
			unsigned int num_iac = n - 0x138;
			if(num_iac < CONFIG::NUM_IACS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetIAC(num_iac);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x13c:
		case 0x13d:
		{
			unsigned int num_dac = n - 0x13c;
			if(num_dac < CONFIG::NUM_DACS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetDAC(num_dac);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x13e:
		case 0x13f:
		{
			unsigned int num_dvc = n - 0x13e;
			if(num_dvc < CONFIG::NUM_DVCS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetDVC(num_dvc);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x150:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetTSR();
			return true;
		case 0x154:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetTCR();
			return true;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetIVOR(num_ivor);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x23a:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMCSRR0();
			return true;
		case 0x23b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMCSRR1();
			return true;
		case 0x23c:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMCSR();
			return true;
		case 0x370:
		case 0x371:
		case 0x372:
		case 0x373:
		{
			unsigned int num_inv = n - 0x370;
			if(num_inv < CONFIG::NUM_INVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetINV(num_inv);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x374:
		case 0x375:
		case 0x376:
		case 0x377:
		{
			unsigned int num_itv = n - 0x374;
			if(num_itv < CONFIG::NUM_ITVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetITV(num_itv);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x378:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetCCR1();
			return true;
		case 0x390:
		case 0x391:
		case 0x392:
		case 0x393:
		{
			unsigned int num_dnv = n - 0x390;
			if(num_dnv < CONFIG::NUM_DNVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetDNV(num_dnv);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x394:
		case 0x395:
		case 0x396:
		case 0x397:
		{
			unsigned int num_dtv = n - 0x394;
			if(num_dtv < CONFIG::NUM_DTVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = GetDTV(num_dtv);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x398:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDVLIM();
			return true;
		case 0x399:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetIVLIM();
			return true;
		case 0x39b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetRSTCFG();
			return true;
		case 0x39c:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDCDBTRL();
			return true;
		case 0x39d:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDCDBTRH();
			return true;
		case 0x39e:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetICDBTRL();
			return true;
		case 0x39f:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetICDBTRH();
			return true;
		case 0x3b2:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMMUCR();
			return true;
		case 0x3b3:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetCCR0();
			return true;
		case 0x3d3:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetICDBDR();
			return value;
		case 0x3f3:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDBDR();
			return true;
	}
	SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
	return false;
}

template <class CONFIG>
bool CPU<CONFIG>::SetSPR(unsigned int n, uint32_t value)
{
	switch(n)
	{
		case 0x001: SetXER(value); return true;
		case 0x008: SetLR(value); return true;
		case 0x009: SetCTR(value); return true;
		case 0x016:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			RunInternalTimers();
			SetDEC(value);
			RunInternalTimers();
			return true;
		case 0x01a:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSRR0(value);
			return true;
		case 0x01b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSRR1(value);
			return true;
		case 0x030:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPID(value);
			return true;
		case 0x036:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDECAR(value);
			return true;
		case 0x03a:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetCSRR0(value);
			return true;
		case 0x03b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetCSRR1(value);
			return true;
		case 0x03d:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDEAR(value);
			return true;
		case 0x03e:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetESR(value);
			return true;
		case 0x03f:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetIVPR(value);
			return true;
		case 0x100:
			SetUSPRG0(value);
			return true;
		case 0x104:
		case 0x105:
		case 0x106:
		case 0x107:
		{
			unsigned int num_sprg = n - 0x104 + 4;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				SetSPRG(num_sprg, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x110:
		case 0x111:
		case 0x112:
		case 0x113:
		case 0x114:
		case 0x115:
		case 0x116:
		case 0x117:
		{
			unsigned int num_sprg = n - 0x110;
			if(num_sprg < CONFIG::NUM_SPRGS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetSPRG(num_sprg, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x11c:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			RunInternalTimers();
			SetTBL(value);
			RunInternalTimers();
			return true;
		case 0x11d:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			RunInternalTimers();
			SetTBU(value);
			RunInternalTimers();
			return true;
		case 0x130:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDBSR(GetDBSR() & ~value); // A 1 clears the bit, a 0 leaves it unchanged
			return true;
		case 0x134:
		case 0x135:
		case 0x136:
		{
			unsigned int num_dbcr = n - 0x134;
			if(num_dbcr < CONFIG::NUM_DBCRS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetDBCR(num_dbcr, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x138:
		case 0x139:
		case 0x13a:
		case 0x13b:
		{
			unsigned int num_iac = n - 0x138;
			if(num_iac < CONFIG::NUM_IACS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetIAC(num_iac, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x13c:
		case 0x13d:
		{
			unsigned int num_dac = n - 0x13c;
			if(num_dac < CONFIG::NUM_DACS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetDAC(num_dac, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x13e:
		case 0x13f:
		{
			unsigned int num_dvc = n - 0x13e;
			if(num_dvc < CONFIG::NUM_DVCS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetDVC(num_dvc, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x150:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetTSR(GetTSR() & ~value); // A 1 clears the bit, a 0 leaves it unchanged
			UpdateExceptionFlags(); // Mirror TSR[DIS], TSR[FIS] and TSR[WIS]
			return true;
		case 0x154:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			RunInternalTimers();
			SetTCR(value);
			RunInternalTimers();
			return true;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetIVOR(num_ivor, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x23a:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMCSRR0(value);
			return true;
		case 0x23b:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMCSRR1(value);
			return true;
		case 0x23c:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMCSR(GetMCSR() & ~value); // A 1 clears the bit, a 0 leaves it unchanged
			return true;
		case 0x370:
		case 0x371:
		case 0x372:
		case 0x373:
		{
			unsigned int num_inv = n - 0x370;
			if(num_inv < CONFIG::NUM_INVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetINV(num_inv, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x374:
		case 0x375:
		case 0x376:
		case 0x377:
		{
			unsigned int num_itv = n - 0x374;
			if(num_itv < CONFIG::NUM_ITVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetITV(num_itv, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x378:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetCCR1(value);
			RunInternalTimers();
			return true;
		case 0x390:
		case 0x391:
		case 0x392:
		case 0x393:
		{
			unsigned int num_dnv = n - 0x390;
			if(num_dnv < CONFIG::NUM_DNVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetDNV(num_dnv, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x394:
		case 0x395:
		case 0x396:
		case 0x397:
		{
			unsigned int num_dtv = n - 0x394;
			if(num_dtv < CONFIG::NUM_DTVS)
			{
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				SetDTV(num_dtv, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 0x398:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDVLIM(value);
			return true;
		case 0x399:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetIVLIM(value);
			return true;
		case 0x3b2:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMMUCR(value);
			return true;
		case 0x3b3:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetCCR0(value);
			return true;
		case 0x3d3:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetICDBDR(value);
			return true;
		case 0x3f3:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDBDR(value);
			return true;
	}
	SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
	return false;
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

	unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation = 0;

	typename CONFIG::address_t addr = GetCIA();
	SetNIA(addr + 4);
	uint32_t insn = 0;
	if(likely(EmuFetch(addr, insn)))
	{
		operation = unisim::component::cxx::processor::powerpc::ppc440::Decoder<CONFIG>::Decode(addr, insn);

	//	stringstream sstr;
	//	operation->disasm((CPU<CONFIG> *) this, sstr);
	//	std::cerr << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << std::endl;

		if(unlikely(IsVerboseStep()))
		{
			stringstream sstr;
			operation->disasm((CPU<CONFIG> *) this, sstr);
			logger << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":0x" << std::hex << operation->GetEncoding() << std::dec << ":" << sstr.str() << endl << EndDebugInfo;
		}

// 		{
// 			std::cerr << "pc=0x" << std::hex << addr << std::dec;
// 			std::cerr << " cr=0x" << std::hex << cr << std::dec;
// 			std::cerr << " lr=0x" << std::hex << lr << std::dec;
// 			std::cerr << " ctr=0x" << std::hex << ctr << std::dec;
// 			std::cerr << " xer=0x" << std::hex << xer << std::dec;
// 			std::cerr << " fpscr=0x" << std::hex << fpscr << std::dec;
// 			unsigned i;
// 			for(i = 0; i < 32; i++)
// 			{
// 				std::cerr << " r" << i << "=0x" << std::hex << gpr[i] << std::dec;
// 			}
// 			for(i = 0; i < 32; i++)
// 			{
// 				std::cerr << " f" << i << "=0x" << std::hex << fpr[i].queryValue() << std::dec;
// 			}
// 			std::cerr << std::endl;
// 		}
		/* execute the instruction */
		if(likely(operation->execute(this)))
		{
			/* update the instruction counter */
			instruction_counter++;
		}
	}

	ProcessExceptions(operation);

	/* report a finished instruction */
	if(unlikely(requires_finished_instruction_reporting))
	{
		if(unlikely(memory_access_reporting_import != 0))
		{
			memory_access_reporting_import->ReportFinishedInstruction(GetCIA(), GetNIA());
		}
	}

	/* go to the next instruction */
	SetCIA(GetNIA());

	if(unlikely(trap_reporting_import && (instruction_counter == trap_on_instruction_counter)))
	{
		trap_reporting_import->ReportTrap();
	}
	
	if(unlikely((instruction_counter >= max_inst) || (GetCIA() == halt_on_addr))) Stop(0);
	
	//DL1SanityCheck();
	//IL1SanityCheck();
	
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
		Idle();
		ResetMSR_WE();
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
	
	UpdateExceptionMask();
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
unisim::service::interfaces::Register *CPU<CONFIG>::GetRegister(const char *name)
{
	map<string, unisim::service::interfaces::Register *>::iterator reg_iter = registers_registry.find(name);
	if(reg_iter != registers_registry.end())
	{
		return (*reg_iter).second;
	}

	return 0;
}

template <class CONFIG>
void CPU<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
  scanner.Append(this->GetRegister("r0"));
  scanner.Append(this->GetRegister("r1"));
  scanner.Append(this->GetRegister("r2"));
  scanner.Append(this->GetRegister("r3"));
  scanner.Append(this->GetRegister("r4"));
  scanner.Append(this->GetRegister("r5"));
  scanner.Append(this->GetRegister("r6"));
  scanner.Append(this->GetRegister("r7"));
  scanner.Append(this->GetRegister("r8"));
  scanner.Append(this->GetRegister("r9"));
  scanner.Append(this->GetRegister("r10"));
  scanner.Append(this->GetRegister("r11"));
  scanner.Append(this->GetRegister("r12"));
  scanner.Append(this->GetRegister("r13"));
  scanner.Append(this->GetRegister("r14"));
  scanner.Append(this->GetRegister("r15"));
  scanner.Append(this->GetRegister("r16"));
  scanner.Append(this->GetRegister("r17"));
  scanner.Append(this->GetRegister("r18"));
  scanner.Append(this->GetRegister("r19"));
  scanner.Append(this->GetRegister("r20"));
  scanner.Append(this->GetRegister("r21"));
  scanner.Append(this->GetRegister("r22"));
  scanner.Append(this->GetRegister("r23"));
  scanner.Append(this->GetRegister("r24"));
  scanner.Append(this->GetRegister("r25"));
  scanner.Append(this->GetRegister("r26"));
  scanner.Append(this->GetRegister("r27"));
  scanner.Append(this->GetRegister("r28"));
  scanner.Append(this->GetRegister("r29"));
  scanner.Append(this->GetRegister("r30"));
  scanner.Append(this->GetRegister("r31"));
  scanner.Append(this->GetRegister("cr"));
  scanner.Append(this->GetRegister("cia"));
}

template <class CONFIG>
const char *CPU<CONFIG>::GetArchitectureName() const
{
	return "powerpc";
}

/* Endian interface */
template <class CONFIG>
endian_type CPU<CONFIG>::GetEndianess()
{
	return E_BIG_ENDIAN;
}

template <class CONFIG>
void CPU<CONFIG>::SetExceptionFlags(uint32_t _exc_flags, typename CONFIG::address_t addr, typename CONFIG::MemoryAccessType memory_access_type)
{
	if(IsVerboseException())
	{
		if(_exc_flags & CONFIG::EXC_MASK_MACHINE_CHECK_INSTRUCTION_SYNCHRONOUS)
		{
			logger << DebugInfo << "Got an instruction synchronous machine check exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_MACHINE_CHECK_INSTRUCTION_ASYNCHRONOUS)
		{
			logger << DebugInfo << "Got an instruction asynchronous machine check exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_MACHINE_CHECK_DATA_ASYNCHRONOUS)
		{
			logger << DebugInfo << "Got a data asynchronous machine check exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_MACHINE_CHECK_TLB_ASYNCHRONOUS)
		{
			logger << DebugInfo << "Got a TLB asynchronous machine check exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_READ_ACCESS_CONTROL)
		{
			logger << DebugInfo << "Got a read access control data storage exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_WRITE_ACCESS_CONTROL)
		{
			logger << DebugInfo << "Got a write access control data storage exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_BYTE_ORDERING)
		{
			logger << DebugInfo << "Got a cache locking data storage exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_CACHE_LOCKING)
		{
			logger << DebugInfo << "Got a cache locking data storage exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_STORAGE_EXECUTE_ACCESS_CONTROL)
		{
			logger << DebugInfo << "Got an execute access control instruction storage exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_TLB_ERROR)
		{
			logger << DebugInfo << "Got a data TLB error exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_TLB_ERROR)
		{
			logger << DebugInfo << "Got an instruction TLB error exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_ALIGNMENT)
		{
			logger << DebugInfo << "Got an alignment exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_ILLEGAL_INSTRUCTION)
		{
			logger << DebugInfo << "Got an illegal instruction program exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_PRIVILEGE_VIOLATION)
		{
			logger << DebugInfo << "Got a privilege violation program exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_TRAP)
		{
			logger << DebugInfo << "Got a trap program exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_FLOATING_POINT)
		{
			logger << DebugInfo << "Got a floating point program exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_UNIMPLEMENTED_INSTRUCTION)
		{
			logger << DebugInfo << "Got an unimplemented instruction program exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_FLOATING_POINT_UNAVAILABLE)
		{
			logger << DebugInfo << "Got a floating point unavailable exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_AUXILIARY_PROCESSOR_UNAVAILABLE)
		{
			logger << DebugInfo << "Got an auxiliary processor unavailable exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_EXTERNAL_INPUT)
		{
			logger << DebugInfo << "Got an external input exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_CRITICAL_INPUT)
		{
			logger << DebugInfo << "Got a critical input exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DECREMENTER)
		{
			logger << DebugInfo << "Got a decrementer exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DEBUG)
		{
			logger << DebugInfo << "Got a debug exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER)
		{
			logger << DebugInfo << "Got a fixed interval timer exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_WATCHDOG_TIMER)
		{
			logger << DebugInfo << "Got a watchdog timer exception" << EndDebugInfo;
		}
	}
	
	exc_flags = exc_flags | _exc_flags;
	// Mirror TSR[DIS], TSR[FIS] and TSR[WIS]
	if(_exc_flags & CONFIG::EXC_MASK_DECREMENTER)
	{
		SetTSR(GetTSR() | CONFIG::TSR_DIS_MASK);
	}
	if(_exc_flags & CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER)
	{
		SetTSR(GetTSR() | CONFIG::TSR_FIS_MASK);
	}
	if(_exc_flags & CONFIG::EXC_MASK_WATCHDOG_TIMER)
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
	exc_addr = addr;
	exc_memory_access_type = memory_access_type;
	
	/*
	
	//exc = exc | (_exc & (CONFIG::EXC_MASK_EXTERNAL_INPUT_INTERRUPT | CONFIG::EXC_MASK_CRITICAL_INPUT_INTERRUPT | CONFIG::EXC_MASK_DEBUG_INTERRUPT));
	exc = exc | (_exc & ~(CONFIG::EXC_MASK_DECREMENTER | CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER | CONFIG::EXC_MASK_WATCHDOG_TIMER));
	if(_exc & CONFIG::EXC_MASK_DECREMENTER) SetTSR(GetTSR() | CONFIG::TSR_DIS_MASK);
	if(_exc & CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER) SetTSR(GetTSR() | CONFIG::TSR_FIS_MASK);
	if(_exc & CONFIG::EXC_MASK_WATCHDOG_TIMER)
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
	exc_addr = addr;
	exc_memory_access_type = memory_access_type;
	*/
}

template <class CONFIG>
void CPU<CONFIG>::ResetExceptionFlags(unsigned int _exc_flags)
{
	// Note: HW is not supposed to clear TSR bits
	_exc_flags = _exc_flags & ~(CONFIG::EXC_MASK_DECREMENTER | CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER | CONFIG::EXC_MASK_WATCHDOG_TIMER);
	exc_flags = exc_flags & ~_exc_flags;
	if(IsVerboseException())
	{
		logger << DebugInfo << "Resetting exception flags (" << std::hex << _exc_flags << std::dec << "): " << std::hex << exc_flags << std::dec << EndDebugInfo;
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetExceptionMask(unsigned int _exc_mask)
{
	exc_mask = exc_mask | _exc_mask;
}

template <class CONFIG>
void CPU<CONFIG>::ResetExceptionMask(unsigned int _exc_mask)
{
	exc_mask = exc_mask & ~_exc_mask;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateExceptionFlags()
{
	uint32_t tsr_dis = GetTSR_DIS();
	uint32_t tsr_fis = GetTSR_FIS();
	uint32_t tsr_wis = GetTSR_WIS();
	exc_flags = (exc_flags & ~(CONFIG::EXC_MASK_DECREMENTER | CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER | CONFIG::EXC_MASK_WATCHDOG_TIMER))
	          | (tsr_dis << CONFIG::EXC_DECREMENTER) | (tsr_fis << CONFIG::EXC_FIXED_INTERVAL_TIMER) | (tsr_wis << CONFIG::EXC_WATCHDOG_TIMER);
}

template <class CONFIG>
void CPU<CONFIG>::UpdateExceptionMask()
{
	uint32_t dbcr0_idm = GetDBCR0_IDM();
	uint32_t msr_de = GetMSR_DE();
	uint32_t tcr_wie = GetTCR_WIE();
	uint32_t msr_ce = GetMSR_CE();
	uint32_t msr_ee = GetMSR_EE();
	uint32_t tcr_fie = GetTCR_FIE();
	uint32_t tcr_die = GetTCR_DIE();
	exc_mask = (exc_mask & ~(CONFIG::EXC_MASK_DEBUG | CONFIG::EXC_MASK_CRITICAL_INPUT | CONFIG::EXC_MASK_WATCHDOG_TIMER | CONFIG::EXC_MASK_EXTERNAL_INPUT | CONFIG::EXC_MASK_FIXED_INTERVAL_TIMER | CONFIG::EXC_MASK_DECREMENTER))
	         | ((dbcr0_idm & msr_de) << CONFIG::EXC_DEBUG)
	         | (msr_ce << CONFIG::EXC_CRITICAL_INPUT)
	         | ((tcr_wie & msr_ce) << CONFIG::EXC_WATCHDOG_TIMER)
	         | (msr_ee << CONFIG::EXC_EXTERNAL_INPUT)
	         | ((tcr_fie & msr_ee) << CONFIG::EXC_FIXED_INTERVAL_TIMER)
	         | ((tcr_die & msr_ee) << CONFIG::EXC_DECREMENTER);
}

template <class CONFIG>
void CPU<CONFIG>::RunInternalTimers()
{
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
}

template <class CONFIG>
bool CPU<CONFIG>::Isync()
{
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();
	
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Rfi()
{
	if(unlikely(GetMSR_PR()))
	{
		SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
		return false;
	}
	
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();
	SetNIA(GetSRR0());
	SetMSR(GetSRR1());
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo << "Returning from interrupt to 0x" << std::hex << GetNIA() << std::dec << EndDebugInfo;
	}

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Rfci()
{
	if(unlikely(GetMSR_PR()))
	{
		SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
		return false;
	}
	
	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();

	SetNIA(GetCSRR0());
	SetMSR(GetCSRR1());
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo << "Returning from critical interrupt to 0x" << std::hex << GetNIA() << std::dec << EndDebugInfo;
	}

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::Rfmci()
{
	if(unlikely(GetMSR_PR()))
	{
		SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
		return false;
	}

	FlushSubsequentInstructions();
	InvalidateITLB();
	InvalidateDTLB();

	SetNIA(GetMCSRR0());
	SetMSR(GetMCSRR1());
	
	if(unlikely(IsVerboseException()))
	{
		logger << DebugInfo << "Returning from machine check interrupt to 0x" << std::hex << GetNIA() << std::dec << EndDebugInfo;
	}

	return true;
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
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
	os << ", lower_protection_boundary=0x" << std::hex << mmu_access.lower_protection_boundary << std::dec;
	os << ", upper_protection_boundary=0x" << std::hex << mmu_access.upper_protection_boundary << std::dec;
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
