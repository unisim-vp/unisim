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

#include <unisim/component/cxx/processor/powerpc/mpc7447a/isa.tcc>
#include <unisim/util/simfloat/floating.tcc>
#include <unisim/component/cxx/cache/cache.tcc>
#include <unisim/component/cxx/tlb/tlb.tcc>
#include <unisim/util/queue/queue.tcc>

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
namespace mpc7447a {

using namespace std;

template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
	: Object(name, parent, "PowerPC MPC7447A CPU")
	, unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>()
	, Client<Loader>(name,  parent)
	, Client<SymbolTableLookup<typename CONFIG::address_t> >(name,  parent)
	, Client<DebugYielding>(name,  parent)
	, Client<MemoryAccessReporting<typename CONFIG::address_t> >(name,  parent)
	, Client<TrapReporting>(name,  parent)
	, Service<MemoryAccessReportingControl>(name,  parent)
	, Service<Disassembly<typename CONFIG::address_t> >(name,  parent)
	, Service<unisim::service::interfaces::Registers>(name,  parent)
	, Service<Memory<typename CONFIG::address_t> >(name,  parent)
	, Service<MemoryInjection<typename CONFIG::address_t> >(name,  parent)
	, Client<Memory<typename CONFIG::address_t> >(name,  parent)
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
	, kernel_loader_import("kernel-loader-import",  this)
	, debug_yielding_import("debug-control-import",  this)
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
	, linux_printk_buf_addr(0)
	, linux_printk_buf_size(0)
	, logger(*this)
	, requires_memory_access_reporting(true)
	, requires_fetch_instruction_reporting(true)
	, requires_commit_instruction_reporting(true)
	, cpu_cycle_time(0)
	, voltage(0)
	, cpu_cycle(0)
	, timer_cycle(0)
	, verbose_all(false)
	, verbose_setup(false)
	, verbose_step(false)
	, verbose_dtlb(false)
	, verbose_itlb(false)
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
	, enable_linux_printk_snooping(false)
	, enable_linux_syscall_snooping(false)
	, halt_on_addr((typename CONFIG::address_t) -1)
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, max_inst(0xffffffffffffffffULL)
	, num_interrupts(0)
	, num_system_reset_interrupts(0)
	, num_machine_check_interrupts(0)
	, num_data_storage_interrupts(0)
	, num_instruction_storage_interrupts(0)
	, num_external_interrupts(0)
	, num_alignment_interrupts(0)
	, num_program_interrupts(0)
	, num_floating_point_unavailable_interrupts(0)
	, num_decrementer_interrupts(0)
	, num_system_call_interrupts(0)
	, num_trace_interrupts(0)
	, num_performance_monitor_interrupts(0)
	, num_instruction_address_breakpoint_interrupts(0)
	, num_system_management_interrupts(0)
	, num_itlb_miss_interrupts(0)
	, num_dtlb_miss_on_load_interrupts(0)
	, num_dtlb_miss_on_store_interrupts(0)
	, num_altivec_unavailable_interrupts(0)
	, num_altivec_assist_interrupts(0)
	, registers_registry()
	, instruction_counter(0)
	, fp32_estimate_inv_warning(false)
	, fp64_estimate_inv_sqrt_warning(false)
	, last_ibat_miss_bepi(0)
	, last_ibat_miss_privilege_level(CONFIG::PR_SUPERVISOR)
	, last_dbat_miss_bepi(0)
	, last_dbat_miss_privilege_level(CONFIG::PR_SUPERVISOR)
	, num_ibat_accesses(0)
	, num_ibat_misses(0)
	, num_dbat_accesses(0)
	, num_dbat_misses(0)
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
	, num_dtlb_accesses(0)
	, num_dtlb_misses(0)
	, itlb()
	, num_itlb_accesses(0)
	, num_itlb_misses(0)
	, num_insn_in_prefetch_buffer(0)
	, cur_insn_in_prefetch_buffer(0)
	, prefetch_buffer()
	, reserve(false)
	, reserve_addr(0)
	, exc_flags(0)
	, exc_mask(CONFIG::EXC_MASK_NON_MASKABLE)
	, exc_addr(0)
	, exc_vsid(0)
	, exc_api(0)
	, exc_way(0)
	, exc_key(0)
	, exc_memory_access_type(CONFIG::MAT_READ)
	, exc_memory_type(CONFIG::MT_INSN)
	, enter_isr_table()
	, param_cpu_cycle_time("cpu-cycle-time",  this,  cpu_cycle_time, "CPU cycle time in picoseconds")
	, param_voltage("voltage",  this,  voltage, "CPU voltage in mV")
	, param_max_inst("max-inst",  this,  max_inst, "maximum number of instructions to simulate")
	, param_verbose_all("verbose-all",  this,  verbose_all, "globally enable/disable verbosity")
	, param_verbose_setup("verbose-setup",  this,  verbose_setup, "enable/disable verbosity while setup")
	, param_verbose_step("verbose-step",  this,  verbose_step, "enable/disable verbosity when simulating an instruction")
	, param_verbose_dtlb("verbose-dtlb",  this,  verbose_dtlb, "enable/disable verbosity when accessing data translation lookahead buffer")
	, param_verbose_itlb("verbose-itlb",  this,  verbose_itlb, "enable/disable verbosity when accessing instruction translation lookahead buffer")
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
	, param_enable_linux_printk_snooping("enable-linux-printk-snooping", this, enable_linux_printk_snooping, "enable/disable linux printk buffer snooping")
	, param_enable_linux_syscall_snooping("enable-linux-syscall-snooping", this, enable_linux_syscall_snooping, "enable/disable linux syscall snooping")
	, param_trap_on_instruction_counter("trap-on-instruction-counter",  this,  trap_on_instruction_counter, "number of simulated instruction before traping")
	, param_halt_on("halt-on", this, halt_on, "Symbol or address where to stop simulation")
	, stat_instruction_counter("instruction-counter",  this,  instruction_counter, "number of simulated instructions")
	, stat_timer_cycle("timer-cycle",  this,  timer_cycle, "number of simulated timer cycles")
	, stat_num_il1_accesses("num-il1-accesses", this, num_il1_accesses, "number of accesses to L1 instruction cache")
	, stat_num_il1_misses("num-il1-misses", this, num_il1_misses, "number of misses to L1 instruction cache")
	, formula_il1_miss_rate("il1-miss-rate", this, "/", &stat_num_il1_misses, &stat_num_il1_accesses)
	, stat_num_dl1_accesses("num-dl1-accesses", this, num_dl1_accesses, "number of accesses to L1 data cache")
	, stat_num_dl1_misses("num-dl1-misses", this, num_dl1_misses, "number of misses to L1 data cache")
	, formula_dl1_miss_rate("dl1-miss-rate", this, "/", &stat_num_dl1_misses, &stat_num_dl1_accesses)
	, stat_num_l2_accesses("num-l2-accesses", this, num_l2_accesses, "number of accesses to unified L2 cache")
	, stat_num_l2_misses("num-l2-misses", this, num_l2_misses, "number of misses to unified L2 cache")
	, formula_l2_miss_rate("l2-miss-rate", this, "/", &stat_num_l2_misses, &stat_num_l2_accesses)
	, stat_num_ibat_accesses("num-ibat-accesses", this, num_ibat_accesses, "number of accesses to IBATs")
	, stat_num_ibat_misses("num-ibat-misses", this, num_ibat_misses, "number of misses to IBATs")
	, formula_ibat_miss_rate("ibat-miss-rate", this, "/", &stat_num_ibat_misses, &stat_num_ibat_accesses)
	, stat_num_dbat_accesses("num-dbat-accesses", this, num_dbat_accesses, "number of accesses to DBATs")
	, stat_num_dbat_misses("num-dbat-misses", this, num_dbat_misses, "number of misses to DBATs")
	, formula_dbat_miss_rate("dbat-miss-rate", this, "/", &stat_num_dbat_misses, &stat_num_dbat_accesses)
	, stat_num_itlb_accesses("num-itlb-accesses", this, num_itlb_accesses, "number of accesses to ITLB")
	, stat_num_itlb_misses("num-itlb-misses", this, num_itlb_misses, "number of misses to ITLB")
	, formula_itlb_miss_rate("itlb-miss-rate", this, "/", &stat_num_itlb_misses, &stat_num_itlb_accesses)
	, stat_num_dtlb_accesses("num-dtlb-accesses", this, num_dtlb_accesses, "number of accesses to DTLB")
	, stat_num_dtlb_misses("num-dtlb-misses", this, num_dtlb_misses, "number of misses to DTLB")
	, formula_dtlb_miss_rate("dtlb-miss-rate", this, "/", &stat_num_dtlb_misses, &stat_num_dtlb_accesses)
	, stat_num_interrupts("num-interrupts", this, num_interrupts, "Number of interrupts")
	, stat_num_system_reset_interrupts("num-system-reset-interrupts", this, num_system_reset_interrupts, "Number of system reset interrupts")
	, stat_num_machine_check_interrupts("num-machine-check-interrupts", this, num_machine_check_interrupts, "Number of machine check interrupts")
	, stat_num_data_storage_interrupts("num-data-storage-interrupts", this, num_data_storage_interrupts, "Number of data storage interrupts")
	, stat_num_instruction_storage_interrupts("num-instruction-storage-interrupts", this, num_instruction_storage_interrupts, "Number of instruction storage interrupts")
	, stat_num_external_interrupts("num-external-interrupts", this, num_external_interrupts, "Number of external interrupts")
	, stat_num_alignment_interrupts("num-alignment-interrupts", this, num_alignment_interrupts, "Number of alignment interrupts")
	, stat_num_program_interrupts("num-program-interrupts", this, num_program_interrupts, "Number of program interrupts")
	, stat_num_floating_point_unavailable_interrupts("num-floating-point-unavailable-interrupts", this, num_floating_point_unavailable_interrupts, "Number of floating-point unavailable interrupts")
	, stat_num_decrementer_interrupts("num-decrementer-interrupts", this, num_decrementer_interrupts, "Number of decrementer interrupts")
	, stat_num_system_call_interrupts("num-system-call-interrupts", this, num_system_call_interrupts, "Number of system call interrupts")
	, stat_num_trace_interrupts("num-trace-interrupts", this, num_trace_interrupts, "Number of trace interrupts")
	, stat_num_performance_monitor_interrupts("num-performance-monitor-interrupts", this, num_performance_monitor_interrupts, "Number of performance monitor interrupts")
	, stat_num_instruction_address_breakpoint_interrupts("num-instruction-address-breakpoint-interrupts", this, num_instruction_address_breakpoint_interrupts, "Number of instruction address breakpoint interrupts")
	, stat_num_system_management_interrupts("num-system-management-interrupts", this, num_system_management_interrupts, "Number of system management interrupts")
	, stat_num_itlb_miss_interrupts("num-itlb-miss-interrupts", this, num_itlb_miss_interrupts, "Number of ITLB miss interrupts")
	, stat_num_dtlb_miss_on_load_interrupts("num-dtlb-miss-on-load-interrupts", this, num_dtlb_miss_on_load_interrupts, "Number of DTLB Miss-On-Load interrupts")
	, stat_num_dtlb_miss_on_store_interrupts("num-dtlb-miss-on-store-interrupts", this, num_dtlb_miss_on_store_interrupts, "Number of DTLB Miss-On-Store interrupts")
	, stat_num_altivec_unavailable_interrupts("num-altivec-unavailable-interrupts", this, num_altivec_unavailable_interrupts, "Number of altivec unavailable interrupts")
	, stat_num_altivec_assist_interrupts("num-altivec-assist", this, num_altivec_assist_interrupts, "Number of altivec assist interrupts")
{
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_cpu_cycle_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_voltage.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	unsigned int i;

	for(i = 0; i < 32; i++)
	{
		stringstream sstr;
		sstr << "r" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &gpr[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, gpr[i], "General Purpose Register"));
	}

	registers_registry["fpscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpscr", &fpscr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("fpscr", this, fpscr, "Floating-Point Status and Control Register"));

	for(i = 0; i < 32; i++)
	{
		stringstream sstr;
		sstr << "f" << i;
		registers_registry[sstr.str()] = new FloatingPointRegisterInterface(sstr.str().c_str(), &fpr[i]);
		registers_registry2.push_back(new FloatingPointRegisterView(sstr.str().c_str(), this, fpr[i], "Floating-Point Register"));
	}

	registers_registry["cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("cr", &cr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("cr", this, cr, "Condition Register"));
	registers_registry["xer"] = new unisim::util::debug::SimpleRegister<uint32_t>("xer", &xer);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("xer", this, xer, "Integer Exception Register"));

	registers_registry["lr"] = new unisim::util::debug::SimpleRegister<uint32_t>("lr", &lr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("lr", this, lr, "Link Register"));
	registers_registry["ctr"] = new unisim::util::debug::SimpleRegister<uint32_t>("ctr", &ctr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ctr", this, ctr, "Control Register"));

	registers_registry["cia"] = new unisim::util::debug::SimpleRegister<uint32_t>("cia", &cia);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("cia", this, cia, "Current Instruction Address"));
	registers_registry["pc"] = new unisim::util::debug::SimpleRegister<uint32_t>("pc", &cia);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pc", this, cia, "Program Counter"));

	registers_registry["msr"] = new unisim::util::debug::SimpleRegister<uint32_t>("msr", &msr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("msr", this, msr, "Machine State Register"));
	registers_registry["srr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr0", &srr0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("srr0", this, srr0, "Save/Restore Register"));
	registers_registry["srr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("srr1", &srr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("srr1", this, srr1, "Save/Restore Register"));

	registers_registry["tbl"] = new TimeBaseRegisterInterface("tbl", &tb, TimeBaseRegisterInterface::TB_LOW);
	registers_registry2.push_back(new TimeBaseRegisterView("tbl", this, tb, TimeBaseRegisterView::TB_LOW, "Time Base Lower"));
	registers_registry["tbu"] = new TimeBaseRegisterInterface("tbu", &tb, TimeBaseRegisterInterface::TB_HIGH);
	registers_registry2.push_back(new TimeBaseRegisterView("tbu", this, tb, TimeBaseRegisterView::TB_HIGH, "Time Base Lower"));

	for(i = 0; i < CONFIG::NUM_SPRGS; i++)
	{
		stringstream sstr;
		sstr << "sprg" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sprg[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, sprg[i], "Special Purpose Register General"));
	}

	for(i = 0; i < CONFIG::NUM_VRS; i++)
	{
		stringstream sstr;
		sstr << "vr" << i;
		registers_registry[sstr.str()] = new unisim::util::debug::SimpleRegister<vr_t>(sstr.str().c_str(), &vr[i]);
		registers_registry2.push_back(new VectorRegisterView(sstr.str().c_str(), this, vr[i], "Vector Register"));
	}

	registers_registry["dar"] = new unisim::util::debug::SimpleRegister<uint32_t>("dar", &dar);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dar", this, dar, "Data Address Register"));
	registers_registry["dsisr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dsisr", &dsisr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dsisr", this, dsisr, "Data Storage Interrupt Source Register"));
	registers_registry["ear"] = new unisim::util::debug::SimpleRegister<uint32_t>("ear", &ear);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ear", this, ear, "External Access Register"));
	registers_registry["dec"] = new unisim::util::debug::SimpleRegister<uint32_t>("dec", &dec);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dec", this, dec, "Decrementer"));
	registers_registry["dabr"] = new unisim::util::debug::SimpleRegister<uint32_t>("dabr", &dabr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("dabr", this, dabr, "Data Address Breakpoint Register"));
	registers_registry["pvr"] = new unisim::util::debug::SimpleRegister<uint32_t>("pvr", &pvr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pvr", this, pvr, "Processor Version Register"));

	registers_registry["hid0"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid0", &hid0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("hid0", this, hid0, "Hardware Implementation-Dependent Register 0"));
	registers_registry["hid1"] = new unisim::util::debug::SimpleRegister<uint32_t>("hid1", &hid1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("hid1", this, hid1, "Hardware Implementation-Dependent Register 1"));
	registers_registry["iabr"] = new unisim::util::debug::SimpleRegister<uint32_t>("iabr", &iabr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("iabr", this, iabr, "Instruction Address Breakpoint Register"));
	registers_registry["ldstdb"] = new unisim::util::debug::SimpleRegister<uint32_t>("ldstdb", &ldstdb);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ldstdb", this, ldstdb, "Load/Store Debug"));
	registers_registry["ictc"] = new unisim::util::debug::SimpleRegister<uint32_t>("ictc", &ictc);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("ictc", this, ictc, "Instruction Cache Throttling Control Register"));
	registers_registry["pmc1"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc1", &pmc1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pmc1", this, pmc1, "Performance Monitor Counter Register 1"));
	registers_registry["pmc2"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc2", &pmc2);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pmc2", this, pmc2, "Performance Monitor Counter Register 2"));
	registers_registry["pmc3"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc3", &pmc3);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pmc3", this, pmc3, "Performance Monitor Counter Register 3"));
	registers_registry["pmc4"] = new unisim::util::debug::SimpleRegister<uint32_t>("pmc4", &pmc4);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("pmc4", this, pmc4, "Performance Monitor Counter Register 4"));
	registers_registry["sia"] = new unisim::util::debug::SimpleRegister<uint32_t>("sia", &sia);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("sia", this, sia, "Sampled Instruction Address Register"));
	registers_registry["sda"] = new unisim::util::debug::SimpleRegister<uint32_t>("sda", &sda);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("sda", this, sda, "Sampled Data Address Register"));
	registers_registry["mmcr0"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmcr0", &mmcr0);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("mmcr0", this, mmcr0, "Monitor Mode Control Register 0"));
	registers_registry["mmcr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("mmcr1", &mmcr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("mmcr1", this, mmcr1, "Monitor Mode Control Register 1"));
	registers_registry["l2cr"] = new unisim::util::debug::SimpleRegister<uint32_t>("l2cr", &l2cr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("l2cr", this, l2cr, "L2 Cache Control Register"));
	registers_registry["tlbmiss"] = new unisim::util::debug::SimpleRegister<uint32_t>("tlbmiss", &tlbmiss);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("tlbmiss", this, tlbmiss, "TLB Miss"));

	registers_registry["vscr"] = new unisim::util::debug::SimpleRegister<uint32_t>("vscr", &vscr);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("vscr", this, vscr, "Vector Status and Control Register"));
	registers_registry["vrsave"] = new unisim::util::debug::SimpleRegister<uint32_t>("vrsave", &vrsave);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("vrsave", this, vrsave, "Vector Save/Restore Register"));

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "dbat" << i << "u";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dbat[i].u);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dbat[i].u, "Data Block Address Translation Upper"));
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "dbat" << i << "l";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &dbat[i].l);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, dbat[i].l, "Data Block Address Translation Lower"));
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "ibat" << i << "u";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &ibat[i].u);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, ibat[i].u, "Instruction Block Address Translation Upper"));
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "ibat" << i << "l";
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &ibat[i].l);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, ibat[i].l, "Instruction Block Address Translation Lower"));
	}

	for(i = 0; i < CONFIG::NUM_SRS; i++)
	{
		stringstream sstr;
		sstr << "sr" << i;
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &sr[i]);
		registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>(sstr.str().c_str(), this, sr[i], "Segment Register"));
	}

	registers_registry["sdr1"] = new unisim::util::debug::SimpleRegister<uint32_t>("sdr1", &sdr1);
	registers_registry2.push_back(new unisim::kernel::service::Register<uint32_t>("sdr1", this, sdr1, "SDR1"));

	enter_isr_table[CONFIG::EXC_SYSTEM_RESET_HARD] = &CPU<CONFIG>::EnterSystemResetISR;
	enter_isr_table[CONFIG::EXC_MACHINE_CHECK_MCP] = &CPU<CONFIG>::EnterMachineCheckISR;
	enter_isr_table[CONFIG::EXC_MACHINE_CHECK_TEA] = &CPU<CONFIG>::EnterMachineCheckISR;
	enter_isr_table[CONFIG::EXC_SYSTEM_RESET_SOFT] = &CPU<CONFIG>::EnterSystemResetISR;

	enter_isr_table[CONFIG::EXC_INSTRUCTION_STORAGE_NO_EXECUTE] = &CPU<CONFIG>::EnterInstructionStorageISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_STORAGE_DIRECT_STORE] = &CPU<CONFIG>::EnterInstructionStorageISR;
	enter_isr_table[CONFIG::EXC_TLB_MISS] = &CPU<CONFIG>::EnterTLBMissISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_STORAGE_GUARDED_MEMORY] = &CPU<CONFIG>::EnterInstructionStorageISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_STORAGE_PAGE_FAULT] = &CPU<CONFIG>::EnterInstructionStorageISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_STORAGE_PROTECTION_VIOLATION] = &CPU<CONFIG>::EnterInstructionStorageISR;
	enter_isr_table[CONFIG::EXC_INSTRUCTION_ADDRESS_BREAKPOINT] = &CPU<CONFIG>::EnterInstructionAddressBreakpointISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_TRAP] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_UNIMPLEMENTED_INSTRUCTION] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_SYSTEM_CALL] = &CPU<CONFIG>::EnterSystemCallISR;
	enter_isr_table[CONFIG::EXC_FLOATING_POINT_UNAVAILABLE] = &CPU<CONFIG>::EnterFloatingPointUnavailableISR;
	enter_isr_table[CONFIG::EXC_ALTIVEC_UNAVAILABLE] = &CPU<CONFIG>::EnterAltivecUnavailableISR;
	enter_isr_table[CONFIG::EXC_PROGRAM_FLOATING_POINT] = &CPU<CONFIG>::EnterProgramISR;
	enter_isr_table[CONFIG::EXC_ALIGNMENT] = &CPU<CONFIG>::EnterAlignmentISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_EXTERNAL_ACCESS_DISABLED] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_WRITE_THROUGH_LINKED_LOAD_STORE] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_DIRECT_STORE] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_PROTECTION_VIOLATION] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_PAGE_FAULT] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_DATA_STORAGE_DATA_ADDRESS_BREAKPOINT] = &CPU<CONFIG>::EnterDataStorageISR;
	enter_isr_table[CONFIG::EXC_ALTIVEC_ASSIST] = &CPU<CONFIG>::EnterAltivecAssistISR;
	enter_isr_table[CONFIG::EXC_TRACE] = &CPU<CONFIG>::EnterTraceISR;
	enter_isr_table[CONFIG::EXC_SYSTEM_MANAGEMENT] = &CPU<CONFIG>::EnterSystemManagementISR;
	enter_isr_table[CONFIG::EXC_EXTERNAL] = &CPU<CONFIG>::EnterExternalISR;
	enter_isr_table[CONFIG::EXC_PERFORMANCE_MONITOR] = &CPU<CONFIG>::EnterPerformanceMonitorISR;
	enter_isr_table[CONFIG::EXC_DECREMENTER] = &CPU<CONFIG>::EnterDecrementerISR;
	
	Reset();
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
	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_fetch_instruction_reporting = false;
		requires_commit_instruction_reporting = false;
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

	num_il1_accesses = 0;
	num_il1_misses = 0;
	num_dl1_accesses = 0;
	num_dl1_misses = 0;
	num_l2_accesses = 0;
	num_l2_misses = 0;
	
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
				
				if(!symbol)
				{
					symbol = symbol_table_lookup_import->FindSymbolByName("__log_buf", Symbol<typename CONFIG::address_t>::SYM_OBJECT);
				}
				
				if(symbol)
				{
					linux_printk_buf_addr = symbol->GetAddress();
					linux_printk_buf_size = symbol->GetSize();
					if(IsVerboseSetup())
					{
						logger << DebugInfo << "Found Linux printk buffer at 0x" << std::hex << linux_printk_buf_addr << std::dec << "(" << linux_printk_buf_size << " bytes)" << EndDebugInfo;
					}
				}
				else
				{
					if(IsVerboseSetup())
					{
						logger << DebugInfo << "Linux printk buffer not found" << EndDebugInfo;
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
CPU<CONFIG>::RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report)
{
	switch (type) {
	case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
	case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
	case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
	default: throw 0;
	}
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

	cpu_cycle = 0;
	timer_cycle = 0;
	instruction_counter = 0;

	exc_flags = 0;

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
	tb = 0;

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
		dbat[i].u = 0;
		dbat[i].l = 0;
		ibat[i].u = 0;
		ibat[i].l = 0;
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

	ReconfigureFastBATLookup();
	UpdateExceptionMask();
}

template <class CONFIG>
bool CPU<CONFIG>::GetSPR(unsigned int n, uint32_t& value)
{
	switch(n)
	{
		case 1:
			value = GetXER();
			return true;
		case 8:
			value = GetLR();
			return true;
		case 9:
			value = GetCTR();
			return true;
		case 18:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDSISR();
			return true;
		case 19:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDAR();
			return true;
		case 22:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDEC();
			return true;
		case 25:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetSDR1();
			return true;
		case 26:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetSRR0();
			return true;
		case 27:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetSRR1();
			return true;
		case 256:
			value = GetVRSAVE();
			return true;
		case 268:
			value = GetTBL();
			return true;
		case 269:
			value = GetTBU();
			return true;
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
		case 282:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetEAR();
			return true;
		case 287:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPVR();
			return true;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = l ? GetIBATL(num_ibat) : GetIBATU(num_ibat);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = l ? GetDBATL(num_dbat) : GetDBATU(num_dbat);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = l ? GetIBATL(num_ibat) : GetIBATU(num_ibat);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				value = l ? GetDBATL(num_dbat) : GetDBATU(num_dbat);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 928:
			value = GetMMCR2();
			return true;
		case 929:
			value = GetPMC5();
			return true;
		case 930:
			value = GetPMC6();
			return true;
		case 936:
			value = GetMMCR0();
			return true;
		case 937:
			value = GetPMC1();
			return true;
		case 938:
			value = GetPMC2();
			return true;
		case 940:
			value = GetMMCR1();
			return true;
		case 941:
			value = GetPMC3();
			return true;
		case 942:
			value = GetPMC4();
			return true;
		case 944:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMMCR2();
			return true;
		case 945:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPMC5();
			return true;
		case 946:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPMC6();
			return true;
		case 951:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetBAMR();
			return true;
		case 952:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMMCR0();
			return true;
		case 953:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPMC1();
			return true;
		case 954:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPMC2();
			return true;
		case 955:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetSIA();
			return true;
		case 956:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMMCR1();
			return true;
		case 957:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPMC3();
			return true;
		case 958:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPMC4();
			return true;
		case 980:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetTLBMISS();
			return true;
		case 981:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPTEHI();
			return true;
		case 982:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPTELO();
			return true;
		case 1008:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetHID0();
			return true;
		case 1009:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetHID1();
			return true;
		case 1010:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetIABR();
			return true;
		case 1011:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetICTRL();
			return true;
		case 1012: // undocumented !!!
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetLDSTDB();
			return true;
		case 1013:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetDABR();
			return true;
		case 1014:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMSSCR0();
			return true;
		case 1015:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetMSSSR0();
			return true;
		case 1016:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetLDSTCR();
			return true;
		case 1017:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetL2CR();
			return true;
		case 1019:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetICTC();
			return true;
		case 1023:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			value = GetPIR();
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
		/*case 0:*/
		case 1: SetXER(value); return true;
		case 8: SetLR(value); return true;
		case 9: SetCTR(value); return true;
		case 18:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDSISR(value);
			return true;
		case 19:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDAR(value);
			return true;
		case 22:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			RunInternalTimers();
			SetDEC(value);
			RunInternalTimers();
			return true;
		case 25:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSDR1(value);
			return true;
		case 26:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSRR0(value);
			return true;
		case 27:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSRR1(value);
			return true;
		case 256:
			SetVRSAVE(value);
			return true;
		case 268:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetTBL(value);
			return true;
		case 269:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetTBU(value);
			return true;
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
		case 282:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetEAR(value);
			return true;
		case 284:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetTBL(value);
			return true;
		case 285:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetTBU(value);
			return true;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				if(l) SetIBATL(num_ibat, value); else SetIBATU(num_ibat, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				if(l) SetDBATL(num_dbat, value); else SetDBATU(num_dbat, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				if(l) SetIBATL(num_ibat, value); else SetIBATU(num_ibat, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
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
				if(GetMSR_PR())
				{
					SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
					return false;
				}
				if(l) SetDBATL(num_dbat, value); else SetDBATU(num_dbat, value);
				return true;
			}
			SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
			return false;
		}
		case 944:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMMCR2(value);
			return true;
		case 945:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPMC5(value);
			return true;
		case 946:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPMC6(value);
			return true;
		case 951:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetBAMR(value);
			return true;
		case 952:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMMCR0(value);
			return true;
		case 953:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPMC1(value);
			return true;
		case 954:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPMC2(value);
			return true;
		case 955:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSIA(value);
			return true;
		case 956:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMMCR1(value);
			return true;
		case 957:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPMC3(value);
			return true;
		case 958:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPMC4(value);
			return true;
		case 959:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetSDA(value);
			return true;
		case 980:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetTLBMISS(value);
			return true;
		case 981:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPTEHI(value);
			return true;
		case 982:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPTELO(value);
			return true;
		case 1008:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetHID0(value);
			return true;
		case 1009:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetHID1(value);
			return true;
		case 1010:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetIABR(value);
			return true;
		case 1011:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetICTRL(value);
			return true;
		case 1012: // undocumented !!!
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetLDSTDB(value);
			return true;
		case 1013:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetDABR(value);
			return true;
		case 1014:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMSSCR0(value);
			return true;
		case 1015:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetMSSSR0(value);
			return true;
		case 1016:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetLDSTCR(value);
			return true;
		case 1017:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetL2CR(value);
			return true;
		case 1019:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetICTC(value);
			return true;
		case 1023:
			if(GetMSR_PR())
			{
				SetException(CONFIG::EXC_PROGRAM_PRIVILEGE_VIOLATION);
				return false;
			}
			SetPIR(value);
			return true;
	}
	SetException(CONFIG::EXC_PROGRAM_ILLEGAL_INSTRUCTION);
	return false;
}

template <class CONFIG>
void CPU<CONFIG>::StepOneInstruction()
{
	unisim::component::cxx::processor::powerpc::mpc7447a::Operation<CONFIG> *operation = 0;

	typename CONFIG::address_t addr = GetCIA();
	SetNIA(addr + 4);

	if (unlikely(requires_fetch_instruction_reporting and memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportFetchInstruction(addr);
	}
	if (unlikely(debug_yielding_import))
	{
		debug_yielding_import->DebugYield();
	}

	uint32_t insn;
	if(likely(EmuFetch(addr, insn)))
	{
		/* Check for instruction address breakpoint */
		if(CONFIG::IABR_ENABLE && unlikely(GetIABR_BE() && GetIABR_TE() == GetMSR_IR() && ((GetCIA() >> 2) & 0x3fffffffUL) == GetIABR_ADDR()))
		{
			SetException(CONFIG::EXC_INSTRUCTION_ADDRESS_BREAKPOINT);
		}
		else
		{
			operation = unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>::Decode(addr, insn);

//			stringstream sstr;
//			operation->disasm((CPU<CONFIG> *) this, sstr);
//			std::cerr << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << std::endl;

			if(unlikely(IsVerboseStep()))
			{
				stringstream sstr;
				operation->disasm((CPU<CONFIG> *) this, sstr);
				logger << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << endl << EndDebugInfo;
			}

// 			{
// 				std::cerr << "pc=0x" << std::hex << addr << std::dec;
// 				std::cerr << " cr=0x" << std::hex << cr << std::dec;
// 				std::cerr << " lr=0x" << std::hex << lr << std::dec;
// 				std::cerr << " ctr=0x" << std::hex << ctr << std::dec;
// 				std::cerr << " xer=0x" << std::hex << xer << std::dec;
// 				std::cerr << " fpscr=0x" << std::hex << fpscr << std::dec;
// 				unsigned i;
// 				for(i = 0; i < 32; i++)
// 				{
// 					std::cerr << " r" << i << "=0x" << std::hex << gpr[i] << std::dec;
// 				}
// 				for(i = 0; i < 32; i++)
// 				{
// 					std::cerr << " f" << i << "=0x" << std::hex << fpr[i].queryValue() << std::dec;
// 				}
// 				std::cerr << std::endl;
// 			}
			/* execute the instruction */
			if(likely(operation->execute(this)))
			{
				/* update the instruction counter */
				instruction_counter++;
				
				/* report a finished instruction */
				if(unlikely(requires_commit_instruction_reporting and memory_access_reporting_import != 0))
				{
					memory_access_reporting_import->ReportCommitInstruction(GetCIA(),operation->GetLength()/8);
				}
			}
		}
	}

	ProcessExceptions(operation);

	/* go to the next instruction */
	SetCIA(GetNIA());

	if(unlikely(trap_reporting_import && (instruction_counter == trap_on_instruction_counter)))
	{
		trap_reporting_import->ReportTrap();
	}
	
	if(unlikely((instruction_counter >= max_inst) || (GetCIA() == halt_on_addr))) Stop(0);
}

template <class CONFIG>
void CPU<CONFIG>::SetHID0(uint32_t value)
{
	if(unlikely(IsVerboseSetHID0()))
		logger << DebugInfo << "Writing 0x" << std::hex << value << std::dec << " into HID0" << endl << EndDebugInfo;
	uint32_t old_hid0_ice = GetHID0_ICE();
	uint32_t old_hid0_dce = GetHID0_DCE();
	uint32_t old_hid0_nap = GetHID0_NAP();
	uint32_t old_hid0_high_bat_en = GetHID0_HIGH_BAT_EN();
	
	hid0 = (hid0 & ~CONFIG::HID0_MASK) | (value & CONFIG::HID0_MASK);

	if(!old_hid0_high_bat_en && GetHID0_HIGH_BAT_EN())
	{
		if(unlikely(IsVerboseSetHID0()))
			logger << DebugInfo << "Enabling high BATs" << endl << EndDebugInfo;
		
		ReconfigureFastBATLookup();
	}
	
	if(old_hid0_high_bat_en && !GetHID0_HIGH_BAT_EN())
	{
		if(unlikely(IsVerboseSetHID0()))
			logger << DebugInfo << "Disabling high BATs" << endl << EndDebugInfo;
		
		ReconfigureFastBATLookup();
	}
	
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
		unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>::InvalidateDecodingCache();
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
	
	UpdateExceptionMask();
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
		unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>::InvalidateDecodingCache();
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

template <class CONFIG>
bool CPU<CONFIG>::BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg, bool rwitm)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg)
{
	return memory_import->WriteMemory(physical_addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::BusZeroBlock(typename CONFIG::physical_address_t physical_addr)
{
	uint8_t zero[32];
	memset(zero, 0, sizeof(zero));
	return memory_import->WriteMemory(physical_addr & (~31), zero, sizeof(zero));
}

template <class CONFIG>
bool CPU<CONFIG>::BusFlushBlock(typename CONFIG::physical_address_t physical_addr)
{
	return true;
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
			bool status = EmuLoad<true>(addr, dst, sz);
			if(unlikely(!status)) return false;
			MonitorLoad(addr, sz);
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
			bool status = EmuStore<true>(addr, src, sz);
			if(unlikely(!status)) return false;
			MonitorStore(addr, sz);
			src += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
	return true;
}


/* Endian interface */
template <class CONFIG>
endian_type CPU<CONFIG>::GetEndianess()
{
	return E_BIG_ENDIAN;
}

template <class CONFIG>
void CPU<CONFIG>::SetExceptionFlags(uint32_t _exc_flags)
{
	if(IsVerboseException())
	{
		if(_exc_flags & CONFIG::EXC_MASK_SYSTEM_RESET_HARD)
		{
			logger << DebugInfo << "Got a system reset (hard reset) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_MACHINE_CHECK_MCP)
		{
			logger << DebugInfo << "Got a machine check (assertion of MCP signal) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_MACHINE_CHECK_TEA)
		{
			logger << DebugInfo << "Got a machine check (assertion of TEA signal) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_SYSTEM_RESET_SOFT)
		{
			logger << DebugInfo << "Got a machine check (assertion of MCP signal) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_STORAGE_NO_EXECUTE)
		{
			logger << DebugInfo << "Got an instruction storage (no execute) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_STORAGE_DIRECT_STORE)
		{
			logger << DebugInfo << "Got a instruction storage (direct store) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_TLB_MISS)
		{
			logger << DebugInfo << "Got a TLB miss exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_STORAGE_GUARDED_MEMORY)
		{
			logger << DebugInfo << "Got an instruction storage (guarded memory) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_STORAGE_PAGE_FAULT)
		{
			logger << DebugInfo << "Got an instruction storage (page fault) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_STORAGE_PROTECTION_VIOLATION)
		{
			logger << DebugInfo << "Got an instruction storage (protection violation) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_INSTRUCTION_ADDRESS_BREAKPOINT)
		{
			logger << DebugInfo << "Got an instruction address breakpoint exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_ILLEGAL_INSTRUCTION)
		{
			logger << DebugInfo << "Got a program illegal instruction exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_PRIVILEGE_VIOLATION)
		{
			logger << DebugInfo << "Got a program privilege violation exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_TRAP)
		{
			logger << DebugInfo << "Got a program trap exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_UNIMPLEMENTED_INSTRUCTION)
		{
			logger << DebugInfo << "Got a program unimplemented exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_SYSTEM_CALL)
		{
			logger << DebugInfo << "Got a system call exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_FLOATING_POINT_UNAVAILABLE)
		{
			logger << DebugInfo << "Got a floating point unavailable exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_ALTIVEC_UNAVAILABLE)
		{
			logger << DebugInfo << "Got an altivec unavailable exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_PROGRAM_FLOATING_POINT)
		{
			logger << DebugInfo << "Got a program floating point exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_ALIGNMENT)
		{
			logger << DebugInfo << "Got an alignment exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_DATA_STORAGE_EXTERNAL_ACCESS_DISABLED)
		{
			logger << DebugInfo << "Got a data storage (external access disabled) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_DATA_STORAGE_WRITE_THROUGH_LINKED_LOAD_STORE)
		{
			logger << DebugInfo << "Got a data storage (write through linked load/store) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_DIRECT_STORE)
		{
			logger << DebugInfo << "Got a data storage (direct store) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_PROTECTION_VIOLATION)
		{
			logger << DebugInfo << "Got a data storage (protection violation) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_PAGE_FAULT)
		{
			logger << DebugInfo << "Got a data storage (page fault) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_DATA_STORAGE_DATA_ADDRESS_BREAKPOINT)
		{
			logger << DebugInfo << "Got a data store (data address breakpoint) exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_ALTIVEC_ASSIST)
		{
			logger << DebugInfo << "Got an altivec assist exception" << EndDebugInfo;
		}
		if(_exc_flags & CONFIG::EXC_MASK_TRACE)
		{
			logger << DebugInfo << "Got a trace exception" << EndDebugInfo;
		}
	}
	exc_flags = exc_flags | _exc_flags;
}

template <class CONFIG>
void CPU<CONFIG>::ResetExceptionFlags(unsigned int _exc_flags)
{
	exc_flags = exc_flags & ~_exc_flags;
	if(IsVerboseException())
	{
		logger << DebugInfo << "Resetting exception flags: " << std::hex << exc_flags << " (" << _exc_flags << ")" << std::dec << EndDebugInfo;
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
void CPU<CONFIG>::UpdateExceptionMask()
{
	if(GetHID1_EMCP()) SetExceptionMask(CONFIG::EXC_MASK_MACHINE_CHECK_MCP); else ResetExceptionMask(CONFIG::EXC_MASK_MACHINE_CHECK_MCP);
	if(GetMSR_EE()) SetExceptionMask(CONFIG::EXC_MASK_EXTERNAL | CONFIG::EXC_MASK_DECREMENTER); else ResetExceptionMask(CONFIG::EXC_MASK_EXTERNAL | CONFIG::EXC_MASK_DECREMENTER);
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
}

template <class CONFIG>
void CPU<CONFIG>::RunInternalTimers()
{
}

template <class CONFIG>
bool CPU<CONFIG>::Isync()
{
	FlushSubsequentInstructions();
	
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
	SetNIA(GetSRR0() & 0xfffffffcUL);
	SetMSR((GetMSR() & 0xffff008cUL) | (GetSRR1() & 0x0000ff73UL));
	
	return true;
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
