/*
 *  Copyright (c) 2012,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/cxx/processor/hcs12x/s12xgate.hh>
#include <unisim/component/cxx/processor/hcs12x/xgate.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace s12xgate {


#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

using unisim::util::debug::SimpleRegister;

XGATE::XGATE(const char *name, Object *parent):
	Object(name, parent)
	, unisim::kernel::service::Service<Registers>(name, parent)
	, unisim::kernel::service::Service<MemoryAccessReportingControl>(name, parent)
	, unisim::kernel::service::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::service::Client<DebugControl<physical_address_t> >(name, parent)
	, unisim::kernel::service::Client<MemoryAccessReporting<physical_address_t> >(name, parent)
	, unisim::kernel::service::Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::service::Client<SymbolTableLookup<physical_address_t> >(name, parent)
	, unisim::kernel::service::Client<TrapReporting>(name, parent)

	, registers_export("registers_export", this)
	, memory_access_reporting_control_export("memory_access_reporting_control_export", this)
	, memory_export("memory_export", this)
	, debug_control_import("debug_control_import", this)
	, memory_access_reporting_import("memory_access_reporting_import", this)
	, memory_import("memory_import", this)
	, trap_reporting_import("trap_reporting_import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import",  this)
	, logger(0)

	, cpu_cycle_time()
	, voltage()
	, bus_cycle_time()
	, cpu_cycle()
	, bus_cycle()
	, xgate_enabled(false)
	, stop_current_thread(false)
	, state(IDLE)
	, mode(NORMAL)
	, currentThreadTerminated(true)

	, verbose_all(false)
	, param_verbose_all("verbose-all", this, verbose_all)
	, verbose_setup(false)
	, param_verbose_setup("verbose-setup", this, verbose_setup)
	, verbose_step(false)
	, param_verbose_step("verbose-step", this, verbose_step)
	, verbose_dump_regs_start(false)
	, param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start)
	, verbose_dump_regs_end(false)
	, param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end)
	, verbose_exception(false)
	, param_verbose_exception("verbose-exception", this, verbose_exception)

	, requires_memory_access_reporting(true)
	, param_requires_memory_access_reporting("requires-memory-access-reporting", this, requires_memory_access_reporting)
	, requires_finished_instruction_reporting(true)
	, param_requires_finished_instruction_reporting("requires-finished-instruction-reporting", this, requires_finished_instruction_reporting)

	, trace_enable(false)
	, param_trace_enable("trace-enable", this, trace_enable)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled, "")

	, baseAddress(0x0380)
	, param_baseAddress("base-address", this, baseAddress, "XGATE base Address")

	, param_software_channel_id("software_channel_id", this, sofwtare_channel_id, XGATE_SIZE, "XGATE channel ID associated to software trigger. Determined on chip integration level (see Interrupts section of the SoC Guide.")

	, interrupt_software_error(0x62)
	, param_interrupt_software_error("software-error-interrupt", this, interrupt_software_error, "XGATE Software error interrupt")

	, max_inst((uint64_t) -1)
	, param_max_inst("max-inst", this, max_inst)
	, periodic_trap(-1)
	, param_periodic_trap("periodic-trap", this, periodic_trap)

	, instruction_counter(0)
	, stat_instruction_counter("instruction-counter", this, instruction_counter)
	, cycles_counter(0)
	, stat_cycles_counter("cycles-counter", this, cycles_counter)
	, data_load_counter(0)
	, stat_load_counter("data-load-counter", this, data_load_counter)
	, data_store_counter(0)
	, stat_store_counter("data-store-counter", this, data_store_counter)

{
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_periodic_trap.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_cycles_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_load_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	stat_store_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	for (uint8_t i=0; i<XGATE_SIZE; i++) {
		param_software_channel_id.SetFormat(unisim::kernel::service::VariableBase::FMT_HEX);
		param_software_channel_id.SetMutable(true);
	}

	logger = new unisim::kernel::logger::Logger(*this);

	ccr = new XGCCR_t(&xgccr_register);
}

XGATE::~XGATE()
{
	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	if (logger) { delete logger; logger = NULL;}

	if (ccr) { delete ccr; ccr = NULL; }

}

/**************************************************************/
/* Verbose methods (protected)                          START */
/**************************************************************/

inline INLINE
bool XGATE::VerboseSetup() {
	return (debug_enabled && verbose_setup);
}

inline INLINE
bool XGATE::VerboseStep() {
	return (debug_enabled && verbose_step);
}

inline INLINE
void XGATE::VerboseDumpRegs() {

	*logger << "\t- XGMCTL" << " = 0x" << std::hex << xgmctl_register << std::dec; //2-bytes
	*logger << "\t- XGCHID" << " = 0x" << std::hex << (unsigned int) xgchid_register << std::dec; // 1-byte
	*logger << "\t- XGVBR" << " = 0x" << std::hex << xgvbr_register << std::dec; // 2-bytes
	*logger << "\t- XGIF_7F_70" << " = 0x" << std::hex << xgif_register[0] << std::dec; // 2-bytes
	*logger << "\t- XGIF_6F_60" << " = 0x" << std::hex << xgif_register[1] << std::dec; // 2-bytes
	*logger << "\t- XGIF_5F_50" << " = 0x" << std::hex << xgif_register[2] << std::dec; // 2-bytes
	*logger << "\t- XGIF_4F_40" << " = 0x" << std::hex << xgif_register[3] << std::dec; // 2-bytes
	*logger << "\t- XGIF_3F_30" << " = 0x" << std::hex << xgif_register[4] << std::dec; // 2-bytes
	*logger << "\t- XGIF_2F_20" << " = 0x" << std::hex << xgif_register[5] << std::dec; // 2-bytes
	*logger << "\t- XGIF_1F_10" << " = 0x" << std::hex << xgif_register[6] << std::dec; // 2-bytes
	*logger << "\t- XGIF_0F_00" << " = 0x" << std::hex << xgif_register[7] << std::dec; // 2-bytes
	*logger << "\t- XGSWT" << " = 0x" << std::hex << xgswt_register << std::dec; // 2-bytes
	*logger << "\t- XGSEMM" << " = 0x" << std::hex << xgsem_register << std::dec; // 2-bytes
	*logger << "\t- XGCCR" << " = 0x" << std::hex << (unsigned int) xgccr_register << std::dec; // 1-byte
	*logger << "\t- XGPC" << " = 0x" << std::hex << xgpc_register << std::dec; // 2-bytes
	*logger << "\t- XGR1" << " = 0x" << std::hex << xgr_register[1] << std::dec; // 2-bytes
	*logger << "\t- XGR2" << " = 0x" << std::hex << xgr_register[2] << std::dec; // 2-bytes
	*logger << "\t- XGR3" << " = 0x" << std::hex << xgr_register[3] << std::dec; // 2-bytes
	*logger << "\t- XGR4" << " = 0x" << std::hex << xgr_register[4] << std::dec; // 2-bytes
	*logger << "\t- XGR5" << " = 0x" << std::hex << xgr_register[5] << std::dec; // 2-bytes
	*logger << "\t- XGR6" << " = 0x" << std::hex << xgr_register[6] << std::dec; // 2-bytes
	*logger << "\t- XGR7" << " = 0x" << std::hex << xgr_register[7] << std::dec; // 2-bytes
	*logger << std::endl;

}

inline INLINE
void XGATE::VerboseDumpRegsStart() {
	if(debug_enabled && verbose_dump_regs_start) {
		*logger << DebugInfo
			<< "Register dump before starting instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

inline INLINE
void XGATE::VerboseDumpRegsEnd() {
	if(debug_enabled && verbose_dump_regs_end) {
		*logger << DebugInfo
			<< "Register dump at the end of instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

bool XGATE::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.XGMCTL", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgmctl_register);

	unisim::kernel::service::Register<uint16_t> *xgmctl_var = new unisim::kernel::service::Register<uint16_t>("XGMCTL", this, xgmctl_register, "XGATE Control Register (XGMCTL)");
	extended_registers_registry.push_back(xgmctl_var);
	xgmctl_var->setCallBack(this, XGMCTL, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGCHID", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &xgchid_register);

	unisim::kernel::service::Register<uint8_t> *xgchid_var = new unisim::kernel::service::Register<uint8_t>("XGCHID", this, xgchid_register, "XGATE Channel ID Register (XGCHID)");
	extended_registers_registry.push_back(xgchid_var);
	xgchid_var->setCallBack(this, XGCHID, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGVBR", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgvbr_register);

	unisim::kernel::service::Register<uint16_t> *xgvbr_var = new unisim::kernel::service::Register<uint16_t>("XGVBR", this, xgvbr_register, "XGATE Vector Base Address Register (XGVBR)");
	extended_registers_registry.push_back(xgvbr_var);
	xgvbr_var->setCallBack(this, XGVBR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_7F_70", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[0]);

	unisim::kernel::service::Register<uint16_t> *xgif_7f_70_var = new unisim::kernel::service::Register<uint16_t>("XGIF_7F_70", this, xgif_register[0], "XGATE Channel (7F-70) Interrupt Flag Register (XGIF_7F_70)");
	extended_registers_registry.push_back(xgif_7f_70_var);
	xgif_7f_70_var->setCallBack(this, XGIF_7F_70, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_6F_60", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[1]);

	unisim::kernel::service::Register<uint16_t> *xgif_6f_60_var = new unisim::kernel::service::Register<uint16_t>("XGIF_6F_60", this, xgif_register[1], "XGATE Channel (6F-60) Interrupt Flag Register (XGIF_6F_60)");
	extended_registers_registry.push_back(xgif_6f_60_var);
	xgvbr_var->setCallBack(this, XGIF_6F_60, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_5F_50", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[2]);

	unisim::kernel::service::Register<uint16_t> *xgif_5f_50_var = new unisim::kernel::service::Register<uint16_t>("XGIF_5F_50", this, xgif_register[2], "XGATE Channel (5F-60) Interrupt Flag Register (XGIF_5F_50)");
	extended_registers_registry.push_back(xgif_5f_50_var);
	xgvbr_var->setCallBack(this, XGIF_5F_50, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_4F_40", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[3]);

	unisim::kernel::service::Register<uint16_t> *xgif_4F_40_var = new unisim::kernel::service::Register<uint16_t>("XGIF_4F_40", this, xgif_register[3], "XGATE Channel (4F_40) Register (XGIF_4F_40)");
	extended_registers_registry.push_back(xgif_4F_40_var);
	xgvbr_var->setCallBack(this, XGIF_4F_40, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_3F_30", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[4]);

	unisim::kernel::service::Register<uint16_t> *xgif_3f_30_var = new unisim::kernel::service::Register<uint16_t>("XGIF_3F_30", this, xgif_register[4], "XGATE channel (3F-30) Register (XGIF_3F_30)");
	extended_registers_registry.push_back(xgif_3f_30_var);
	xgvbr_var->setCallBack(this, XGIF_3F_30, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_2F_20", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[5]);

	unisim::kernel::service::Register<uint16_t> *xgif_2f_20_var = new unisim::kernel::service::Register<uint16_t>("XGIF_2F_20", this, xgif_register[5], "XGATE Channel (2F-20) Register (XGIF_2F_20)");
	extended_registers_registry.push_back(xgif_2f_20_var);
	xgvbr_var->setCallBack(this, XGIF_2F_20, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_1F_10", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[6]);

	unisim::kernel::service::Register<uint16_t> *xgif_1f_10_var = new unisim::kernel::service::Register<uint16_t>("XGIF_1F_10", this, xgif_register[6], "XGATE Channel (1F-10) Register (XGIF_1F_10)");
	extended_registers_registry.push_back(xgif_1f_10_var);
	xgvbr_var->setCallBack(this, XGIF_1F_10, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGIF_0F_00", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgif_register[7]);

	unisim::kernel::service::Register<uint16_t> *xgif_0f_00_var = new unisim::kernel::service::Register<uint16_t>("XGIF_0F_00", this, xgif_register[7], "XGATE Channel (0F-00) Register (XGIF_0F_00)");
	extended_registers_registry.push_back(xgif_0f_00_var);
	xgvbr_var->setCallBack(this, XGIF_0F_00, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGSWT", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgswt_register);

	unisim::kernel::service::Register<uint16_t> *xgswt_var = new unisim::kernel::service::Register<uint16_t>("XGSWT", this, xgswt_register, "XGATE Software Trigger Register (XGSWT)");
	extended_registers_registry.push_back(xgswt_var);
	xgvbr_var->setCallBack(this, XGSWT, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGSEMM", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgsem_register);

	unisim::kernel::service::Register<uint16_t> *xgsemm_var = new unisim::kernel::service::Register<uint16_t>("XGSEMM", this, xgsem_register, "XGATE Semaphore Register (XGSEMM)");
	extended_registers_registry.push_back(xgsemm_var);
	xgvbr_var->setCallBack(this, XGSEM, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGCCR", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &xgccr_register);

	unisim::kernel::service::Register<uint8_t> *xgccr_var = new unisim::kernel::service::Register<uint8_t>("XGCCR", this, xgccr_register, "XGATE Condition Code Register (XGCCR)");
	extended_registers_registry.push_back(xgccr_var);
	xgvbr_var->setCallBack(this, XGCCR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGPC", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgpc_register);

	unisim::kernel::service::Register<uint16_t> *xgpc_var = new unisim::kernel::service::Register<uint16_t>("XGPC", this, xgpc_register, "XGATE Program Counter Register (XGPC)");
	extended_registers_registry.push_back(xgpc_var);
	xgvbr_var->setCallBack(this, XGPC, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR1", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[1]);

	unisim::kernel::service::Register<uint16_t> *xgr1_var = new unisim::kernel::service::Register<uint16_t>("XGR1", this, xgr_register[1], "XGATE Register 1 (XGR1)");
	extended_registers_registry.push_back(xgr1_var);
	xgvbr_var->setCallBack(this, XGR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR2", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[2]);

	unisim::kernel::service::Register<uint16_t> *xgr2_var = new unisim::kernel::service::Register<uint16_t>("XGR2", this, xgr_register[2], "XGATE Register 2 (XGR2)");
	extended_registers_registry.push_back(xgr2_var);
	xgvbr_var->setCallBack(this, XGR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR3", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[3]);

	unisim::kernel::service::Register<uint16_t> *xgr3_var = new unisim::kernel::service::Register<uint16_t>("XGR3", this, xgr_register[3], "XGATE Register 3 (XGR3)");
	extended_registers_registry.push_back(xgr3_var);
	xgvbr_var->setCallBack(this, XGR3, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR4", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[4]);

	unisim::kernel::service::Register<uint16_t> *xgr4_var = new unisim::kernel::service::Register<uint16_t>("XGR4", this, xgr_register[4], "XGATE Register 4 (XGR4)");
	extended_registers_registry.push_back(xgr4_var);
	xgvbr_var->setCallBack(this, XGR4, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR5", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[5]);

	unisim::kernel::service::Register<uint16_t> *xgr5_var = new unisim::kernel::service::Register<uint16_t>("XGR5", this, xgr_register[5], "XGATE Register 5 (XGR5)");
	extended_registers_registry.push_back(xgr5_var);
	xgvbr_var->setCallBack(this, XGR5, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR6", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[6]);

	unisim::kernel::service::Register<uint16_t> *xgr6_var = new unisim::kernel::service::Register<uint16_t>("XGR6", this, xgr_register[6], "XGATE Register 6 (XGR6)");
	extended_registers_registry.push_back(xgr6_var);
	xgvbr_var->setCallBack(this, XGR6, &CallBackObject::write, NULL);

	sprintf(buf, "%s.XGR7", GetName());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &xgr_register[7]);

	unisim::kernel::service::Register<uint16_t> *xgr7_var = new unisim::kernel::service::Register<uint16_t>("XGR7", this, xgr_register[7], "XGATE Register 7 (XGR7)");
	extended_registers_registry.push_back(xgr7_var);
	xgvbr_var->setCallBack(this, XGR7, &CallBackObject::write, NULL);

	Reset();

	return (true);
}

bool XGATE::Setup(ServiceExportBase *srv_export) {

	if (!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}

	return (true);

}

bool XGATE::EndSetup() {

	return (true);
}

void XGATE::OnDisconnect() {

}

void XGATE::Reset() {

	xgmctl_register = 0x0000;
	xgchid_register = 0x0000;
	reserverd_register[0] = 0x00;
	reserverd_register[1] = 0x00;
	reserverd_register[2] = 0x00;
	xgvbr_register = 0x0000;
	xgif_register[0] = 0x0000;
	xgif_register[1] = 0x0000;
	xgif_register[2] = 0x0000;
	xgif_register[3] = 0x0000;
	xgif_register[4] = 0x0000;
	xgif_register[5] = 0x0000;
	xgif_register[6] = 0x0000;
	xgif_register[7] = 0x0000;
	xgswt_register = 0x0000;
	xgsem_register = 0x0000;
	reserverd_register[3] = 0x00;
	xgccr_register = 0x0000;
	xgpc_register = 0x0000;
	reserverd_register[4] = 0x00;
	reserverd_register[5] = 0x00;
	xgr_register[1] = 0x0000;
	xgr_register[2] = 0x0000;
	xgr_register[3] = 0x0000;
	xgr_register[4] = 0x0000;
	xgr_register[5] = 0x0000;
	xgr_register[6] = 0x0000;
	xgr_register[7] = 0x0000;


}

void XGATE::Stop(int ret)
{
	exit(ret);
}

void XGATE::Sync()
{

}

uint8_t XGATE::step()
{

	uint8_t 	buffer[MAX_INS_SIZE];

	Decoder decoder;
	Operation 	*op;
	uint8_t	opCycles = 0;

	try
	{

		VerboseDumpRegsStart();

		if(debug_enabled && verbose_step)
			*logger << DebugInfo << "Starting step at PC = 0x" << std::hex << getXGPC() << std::dec << std::endl << EndDebugInfo;

		if(debug_control_import) {

			DebugControl<physical_address_t>::DebugCommand dbg_cmd;

			do {
				if(debug_enabled && verbose_step)
					*logger << DebugInfo << "Fetching debug command (PC = 0x" << std::hex << getXGPC() << std::dec << ")"
						<< std::endl << EndDebugInfo;

				dbg_cmd = debug_control_import->FetchDebugCommand(MMC::getXGATEPagedAddress(getXGPC()));

				if(dbg_cmd == DebugControl<physical_address_t>::DBG_STEP) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_STEP command (PC = 0x"
							<< std::hex << getXGPC() << std::dec << ")"
							<< std::endl << EndDebugInfo;
					break;
				}
				if(dbg_cmd == DebugControl<physical_address_t>::DBG_SYNC) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_SYNC command (PC = 0x"
							<< std::hex << getXGPC() << std::dec << ")"
							<< std::endl << EndDebugInfo;
					Sync();
					continue;
				}

				if(dbg_cmd == DebugControl<physical_address_t>::DBG_KILL) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_KILL command (PC = 0x"
							<< std::hex << getXGPC() << std::dec << ")"
							<< std::endl << EndDebugInfo;
					Stop(0);
				}
				if(dbg_cmd == DebugControl<physical_address_t>::DBG_RESET) {
					if(debug_enabled && verbose_step)
						*logger << DebugInfo
							<< "Received debug DBG_RESET command (PC = 0x"
							<< std::hex << getXGPC() << std::dec << ")"
							<< std::endl << EndDebugInfo;
				}
			} while(1);
		}

		if(requires_memory_access_reporting) {
			if(memory_access_reporting_import) {
				if(debug_enabled && verbose_step)
					*logger << DebugInfo
						<< "Reporting memory access for fetch at address 0x"
						<< std::hex << getXGPC() << std::dec
						<< std::endl << EndDebugInfo;

				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<physical_address_t>::MAT_READ, MemoryAccessReporting<physical_address_t>::MT_INSN, getXGPC(), MAX_INS_SIZE);
			}
		}


		if(debug_enabled && verbose_step)
		{
			*logger << DebugInfo
				<< "Fetching (reading) instruction at address 0x"
				<< std::hex << getXGPC() << std::dec
				<< std::endl << EndDebugInfo;
		}

		fetchInstruction(getXGPC(), buffer, MAX_INS_SIZE);
		CodeType 	insn( buffer, MAX_INS_SIZE*8);

		/* Decode current PC */
		if(debug_enabled && verbose_step)
		{
			stringstream ctstr;
			ctstr << insn;
			*logger << DebugInfo
				<< "Decoding instruction at 0x"
				<< std::hex << getXGPC() << std::dec
				<< " (0x" << std::hex << ctstr.str() << std::dec << ")"
				<< std::endl << EndDebugInfo;
		}

//		op = this->Decode(getRegPC(), insn);
		op = decoder.Decode(getXGPC(), insn);

		lastPC = getXGPC();
        unsigned int insn_length = op->GetLength();
        if (insn_length % 8) throw "InternalError";

		/* Execute instruction */

		if (trace_enable) {
			stringstream disasm_str;
			stringstream ctstr;

			op->disasm(disasm_str);

			ctstr << op->GetEncoding();

			*logger << DebugInfo << GetSimulatedTime() << " ms: "
				<< "PC = 0x" << std::hex << getXGPC() << std::dec << " : "
				<< getFunctionFriendlyName(getXGPC()) << " : "
				<< disasm_str.str()
				<< " : (0x" << std::hex << ctstr.str() << std::dec << " ) " << EndDebugInfo	<< std::endl;

		}

		if (debug_enabled && verbose_step) {
			stringstream disasm_str;
			stringstream ctstr;

			op->disasm(disasm_str);

			ctstr << op->GetEncoding();
			*logger << DebugInfo << GetSimulatedTime() << "ms: "
				<< "Executing instruction "
				<< disasm_str.str()
				<< " at PC = 0x" << std::hex << getXGPC() << std::dec
				<< " (0x" << std::hex << ctstr.str() << std::dec << ") , Instruction Counter = " << instruction_counter
				<< "  " << EndDebugInfo	<< std::endl;
		}

		setXGPC(getXGPC() + (insn_length/8));

		op->execute(this);

		opCycles = op->getCycles();

		cycles_counter += opCycles;

		VerboseDumpRegsEnd();

		instruction_counter++;

		if ((trap_reporting_import) && ((instruction_counter % periodic_trap) == 0)) {
			trap_reporting_import->ReportTrap();
		}

		if(requires_finished_instruction_reporting) {
			if(memory_access_reporting_import) {
				memory_access_reporting_import->ReportFinishedInstruction(MMC::getXGATEPagedAddress(lastPC), MMC::getXGATEPagedAddress(getXGPC()));

			}
		}

	}
	catch(Exception& e)
	{
		if(debug_enabled && verbose_step)
			*logger << DebugError << "uncaught processor exception :" << e.what() << std::endl << EndDebugError;
		Stop(1);
	}

	if (instruction_counter >= max_inst) Stop(0);

	return (opCycles);

}

void XGATE::fetchInstruction(address_t addr, uint8_t* ins, uint8_t nByte)
{

	MMC_DATA mmc_data;

	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = ins;
	mmc_data.data_size = nByte;

	busRead(addr, &mmc_data, sizeof(MMC_DATA));

}

void XGATE::
terminateCurrentThread() {

	currentThreadTerminated = true;
}

void XGATE::riseErrorCondition() {
	/**
	 * XGMCTL::XGSWEIF
	 *  XGATE Software Error Interrupt Flag — This bit signals a pending Software Error Interrupt.
	 *  It is set if the RISC core detects an error condition (see Section 6.4.5, “Software Error Detection”).
	 *  The RISC core is stopped while this bit is set.
	 *  Clearing this bit will terminate the current thread and cause the XGATE to become idle.
	 */

	xgmctl_register = xgmctl_register | 0x0002;
	state = STOP;
}

//=====================================================================
//=             memory access reporting control interface methods     =
//=====================================================================

void XGATE::RequiresMemoryAccessReporting(bool report)
{
	requires_memory_access_reporting = report;
}

void XGATE::RequiresFinishedInstructionReporting(bool report)
{
	requires_finished_instruction_reporting = report;
}


bool XGATE::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+64))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;
		switch (offset) {
			case XGMCTL: *((uint16_t *) buffer) = xgmctl_register; break;
			case XGCHID: *((uint8_t *) buffer) = xgchid_register; break;
			case RESERVED1: *((uint16_t *) buffer) = 0; break;
			case RESERVED2: *((uint16_t *) buffer) = 0; break;
			case RESERVED3: *((uint16_t *) buffer) = 0; break;
			case XGVBR: *((uint16_t *) buffer) = xgvbr_register; break;
			case XGSWT: *((uint16_t *) buffer) = xgswt_register; break;
			case XGSEM: *((uint16_t *) buffer) = xgsem_register; break;
			case RESERVED4: *((uint16_t *) buffer) = 0; break;
			case XGCCR: *((uint8_t *) buffer) = xgccr_register; break;
			case XGPC: *((uint16_t *) buffer) = xgpc_register; break;
			case RESERVED5: *((uint16_t *) buffer) = 0; break;
			case RESERVED6: *((uint16_t *) buffer) = 0; break;
			case XGR1: *((uint16_t *) buffer) = xgr_register[1]; break;
			case XGR2: *((uint16_t *) buffer) = xgr_register[2]; break;
			case XGR3: *((uint16_t *) buffer) = xgr_register[3]; break;
			case XGR4: *((uint16_t *) buffer) = xgr_register[4]; break;
			case XGR5: *((uint16_t *) buffer) = xgr_register[5]; break;
			case XGR6: *((uint16_t *) buffer) = xgr_register[6]; break;
			case XGR7: *((uint16_t *) buffer) = xgr_register[7]; break;

			default: {
				if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
					*((uint16_t *) buffer) = xgif_register[(offset - XGIF_7F_70)/2];

				} else {
					return (false);
				}
				break;
			}
		}

		return (true);
	}

	return (false);
}

bool XGATE::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+64))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case XGMCTL: {
				uint16_t val = *((uint16_t *) buffer);
				xgmctl_register = val;
			} break;
			case XGCHID: {
				uint8_t val = *((uint8_t *) buffer);
				xgchid_register = val;
			} break;
			case RESERVED1: break;
			case RESERVED2: break;
			case RESERVED3: break;
			case XGVBR: {
				uint16_t val = *((uint16_t *) buffer);
				xgvbr_register = val;
			} break;
			case XGSWT: {
				uint16_t val = *((uint16_t *) buffer);
				xgswt_register = val;
			} break;
			case XGSEM: {
				uint16_t val = *((uint16_t *) buffer);
				xgsem_register = val;
			} break;
			case RESERVED4: break;
			case XGCCR: {
				uint8_t val = *((uint8_t *) buffer);
				xgccr_register = val;
			} break;
			case XGPC: {
				uint16_t val = *((uint16_t *) buffer);
				xgpc_register = val;
			} break;
			case RESERVED5: break;
			case RESERVED6: break;
			case XGR1: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[1] = val;
			} break;
			case XGR2: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[2] = val;
			} break;
			case XGR3: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[3] = val;
			} break;
			case XGR4: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[4] = val;
			} break;
			case XGR5: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[5] = val;
			} break;
			case XGR6: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[6] = val;
			} break;
			case XGR7: {
				uint16_t val = *((uint16_t *) buffer);
				xgr_register[7] = val;
			} break;

			default: {
				if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
					uint16_t val = *((uint16_t *) buffer);
					xgif_register[(offset - XGIF_7F_70)/2] = val;
				}

				break;
			}
		}

		return (true);
	}

	return (false);
}

Register *XGATE::GetRegister(const char *name)
{
	if(registers_registry.find(std::string(name)) != registers_registry.end())
		return (registers_registry[std::string(name)]);
	else
		return (NULL);
}

bool XGATE::read(unsigned int offset, const void *buffer, unsigned int data_length)
{

	switch (offset) {
		case XGMCTL: *((uint16_t *) buffer) = Host2BigEndian(xgmctl_register & 0x00FB); break;
		case XGCHID: {
			// The XGCHID register read 0x00 if the XGATE module is idle
			*((uint8_t *) buffer) = (state == IDLE)? 0 : xgchid_register & 0x7F;
		} break;
		case RESERVED1: *((uint16_t *) buffer) = 0; break;
		case RESERVED2: *((uint16_t *) buffer) = 0; break;
		case RESERVED3: *((uint16_t *) buffer) = 0; break;
		case XGVBR: *((uint16_t *) buffer) = Host2BigEndian(xgvbr_register & 0xFFFE); break;
		case XGSWT: *((uint16_t *) buffer) = Host2BigEndian(xgswt_register & 0x00FF); break;
		case XGSEM: *((uint16_t *) buffer) = Host2BigEndian(xgsem_register & 0x00FF); break;
		case RESERVED4: *((uint16_t *) buffer) = 0; break;
		case XGCCR: *((uint8_t *) buffer) = xgccr_register & 0x0F; break;
		case XGPC: *((uint16_t *) buffer) = Host2BigEndian(xgpc_register); break;
		case RESERVED5: *((uint16_t *) buffer) = 0; break;
		case RESERVED6: *((uint16_t *) buffer) = 0; break;
		case XGR1: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[1]); break;
		case XGR2: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[2]); break;
		case XGR3: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[3]); break;
		case XGR4: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[4]); break;
		case XGR5: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[5]); break;
		case XGR6: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[6]); break;
		case XGR7: *((uint16_t *) buffer) = Host2BigEndian(xgr_register[7]); break;

		default: {
			if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
				uint8_t index = (offset - XGIF_7F_70)/2;
				if (index < 2) {
					*((uint16_t *) buffer) = Host2BigEndian(xgif_register[index] & 0x01FF);
				}
				else if (index >= XGIF_0F_00) {
					*((uint16_t *) buffer) = Host2BigEndian(xgif_register[index] & 0xFE00);
				}
				else {
					*((uint16_t *) buffer) = Host2BigEndian(xgif_register[index]);
				}
			}

			break;
		}

	}

	return (true);

}

bool XGATE::write(unsigned int offset, const void *buffer, unsigned int data_length)
{
	switch (offset) {
		case XGMCTL: {
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer)) & 0xFBFB;
			// control bits can only be set or cleared if a "1" is written to its mask bit in the same register access.
			uint16_t mask = val >> 8;

			for (uint8_t i=0,j=1; i<8; i++,j=j*2) {
				if ((mask & j) != 0) {
					if ((val & j) != 0) {
						xgmctl_register = xgmctl_register | j;
					} else {
						xgmctl_register = xgmctl_register & ~j;
					}
				}
			}
			// Freeze and debug modes aren't emulated, hence we don't take care about XGFRZ and XGDBG bits
			if ((xgmctl_register & 0x0080) == 0x0080) {
				enbale_xgate();
			} else {
				/**
				 *  All XGATE interrupts (pending requests) can be disabled by the XGIE bit in the XGATE module control register (XGMCTL)
				 *  The pending request are canceled by clearing XGSWT register
				 */
				disable_xgate();
			}

			// check clearing of XGSWEIF bit
			if (((val & mask) & 0x02) != 0) {
				/**
				 * XGMCTL::XGSWEIF
				 *  XGATE Software Error Interrupt Flag — This bit signals a pending Software Error Interrupt.
				 *  It is set if the RISC core detects an error condition (see Section 6.4.5, “Software Error Detection”).
				 *  The RISC core is stopped while this bit is set.
				 *  Clearing this bit will terminate the current thread and cause the XGATE to become idle.
				 */

				xgmctl_register = xgmctl_register & 0xFFFD;
				terminateCurrentThread();
			}

			if ((xgmctl_register & 0x04) != 0) {
				fakeXGATEActivity();
			}
		} break;
		case XGCHID: {
			// The XGCHID register shows the identifier of the currently active channel. It is only writable in debug mode.
			if (mode == DEBUG) {
				uint8_t val = *((uint8_t *) buffer);
				xgchid_register = val & 0x7F;
			}

		} break;
		case RESERVED1: break;
		case RESERVED2: break;
		case RESERVED3: break;
		case XGVBR: {
			// Writable if the module is disabled (XGMCTL::XGE = 0) and idle (XGCHID = 0x00)
			// I use "RUNNING state and not IDLE because it includes "IDLE" and "STOP" modes

			if (((xgmctl_register & 0x0080) == 0) && (xgchid_register == 0x00)) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgvbr_register = val & 0xFFFE;
			}
		} break;
		case XGSWT: {
			/**
			 * NOTE:
			 * The XGATE channel IDs that are associated with the eight software triggers
			 * are determined on chip integration level. (see Section “Interrupts” of the Soc Guide)
			 * XGATE software triggers work like any peripheral interrupt.
			 * They can be used as XGATE requests as well as S12X_CPU interrupts.
			 * The target of the software trigger must be selected in the S12X_INT module
			 */
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
			uint16_t mask = val >> 8;
			uint16_t old_xgswt_register = xgswt_register;

			for (uint8_t i=0,j=1; i<8; i++,j=j*2) {
				if ((mask & j) != 0) {
					if ((val & j) != 0) {
						xgswt_register = xgswt_register | j;

						// Software trigger pending if XGIE bit is set
						if ((xgmctl_register & 0x0080) == 0x0080) {
							triggerChannelThread();
						}

					} else {
						xgswt_register = xgswt_register & ~j;
					}
				}
			}

		} break;
		case XGSEM: {

			uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
			uint16_t mask = val >> 8;

			for (uint8_t i=0,j=1; i<8; i++,j=j*2) {
				if ((mask & j) != 0) {
					if ((val & j) != 0) {
						if (semphore[i].lock(TSemaphore::CPU12X)) {
							xgsem_register = xgsem_register | j;
						}
					} else {
						if (semphore[i].unlock(TSemaphore::CPU12X)) {
							xgsem_register = xgsem_register & ~j;
						}
					}
				}
			}

		} break;
		case RESERVED4: break;
		case XGCCR: {
			if (mode == DEBUG) {
				uint8_t val = BigEndian2Host(*((uint8_t *) buffer));
				xgccr_register = val & 0x0F;
			}
		} break;
		case XGPC: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgpc_register = val;
			}
		} break;
		case RESERVED5: break;
		case RESERVED6: break;
		case XGR1: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[1] = val;
			}
		} break;
		case XGR2: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[2] = val;
			}
		} break;
		case XGR3: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[3] = val;
			}
		} break;
		case XGR4: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[4] = val;
			}
		} break;
		case XGR5: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[5] = val;
			}
		} break;
		case XGR6: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[6] = val;
			}
		} break;
		case XGR7: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgr_register[7] = val;
			}
		} break;

		default: {
			if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgif_register[(offset - XGIF_7F_70)/2] = (xgif_register[(offset - XGIF_7F_70)/2] & ~val);
			} else {
				return (false);
			}
			break;
		}
	}

	return (true);

}


//=====================================================================
//=                   Debugging methods                               =
//=====================================================================

string XGATE::getObjectFriendlyName(physical_address_t addr)
{
	stringstream sstr;

	const Symbol<physical_address_t> *symbol = NULL;

	if (symbol_table_lookup_import) {
		symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<physical_address_t>::SYM_OBJECT);
	}

	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return (sstr.str());
}

string XGATE::getFunctionFriendlyName(physical_address_t addr)
{
	stringstream sstr;

	const Symbol<physical_address_t> *symbol = NULL;

	if (symbol_table_lookup_import) {
		symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<physical_address_t>::SYM_FUNC);
	}

	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return (sstr.str());
}



} // end of namespace s12xgate
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim



