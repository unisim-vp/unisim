/*
 *  Copyright (c) 2007-2010,
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/kernel/service/service.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/os/linux_os/powerpc_linux32.hh>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>


#include <unisim/component/cxx/processor/powerpc/mpc7447a/config.hh>
#include <unisim/component/tlm2/processor/powerpc/mpc7447a/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef DEBUG_PPCEMU
typedef unisim::component::cxx::processor::powerpc::mpc7447a::DebugConfig CPU_CONFIG;
static const bool DEBUG_INFORMATION = true;
#else
typedef unisim::component::cxx::processor::powerpc::mpc7447a::Config CPU_CONFIG;
static const bool DEBUG_INFORMATION = false;
#endif

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::os::linux_os::Linux;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Variable;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Object;

class IRQStub
	: public sc_module
	, tlm::tlm_bw_transport_if<unisim::component::tlm2::interrupt::InterruptProtocolTypes>
{
public:
	tlm::tlm_initiator_socket<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> irq_master_sock;
	
	IRQStub(const sc_module_name& name);

	virtual tlm::tlm_sync_enum nb_transport_bw(unisim::component::tlm2::interrupt::InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
};

IRQStub::IRQStub(const sc_module_name& name)
	: sc_module(name)
	, irq_master_sock("irq-master-sock")
{
	irq_master_sock(*this);
}

tlm::tlm_sync_enum IRQStub::nb_transport_bw(unisim::component::tlm2::interrupt::InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return tlm::TLM_COMPLETED;
}

void IRQStub::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}


class Simulator : public unisim::kernel::service::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::service::Simulator::SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// Front Side Bus template parameters
	typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	typedef CPU_ADDRESS_TYPE FSB_ADDRESS_TYPE;
	typedef CPU_ADDRESS_TYPE MEMORY_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> MEMORY;
	typedef unisim::component::tlm2::processor::powerpc::mpc7447a::CPU<CPU_CONFIG> CPU;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - RAM
	MEMORY *memory;
	// - IRQ stubs
	IRQStub *external_interrupt_stub;
	IRQStub *hard_reset_stub;
	IRQStub *soft_reset_stub;
	IRQStub *mcp_slave_stub;
//	IRQStub *tea_slave_stub;
	IRQStub *smi_slave_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Linux loader and Linux ABI translator
	Linux<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE> *linux_os;
	
	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger and/or monitor */
		static const unsigned int MAX_FRONT_ENDS = 2;
	};
	
	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> Debugger;
	//  - debugger back-end
	Debugger *debugger;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - the optional power estimators
	CachePowerEstimator *il1_power_estimator;
	CachePowerEstimator *dl1_power_estimator;
	CachePowerEstimator *l2_power_estimator;
	CachePowerEstimator *itlb_power_estimator;
	CachePowerEstimator *dtlb_power_estimator;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool estimate_power;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_estimate_power;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};



Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, external_interrupt_stub(0)
	, hard_reset_stub(0)
	, soft_reset_stub(0)
	, mcp_slave_stub(0)
//	, tea_slave_stub(0)
	, smi_slave_stub(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, l2_power_estimator(0)
	, itlb_power_estimator(0)
	, dtlb_power_estimator(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, exit_status(0)
{
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - RAM
	memory = new MEMORY("memory");
	//  - IRQ Stubs
	external_interrupt_stub = new IRQStub("external-interrupt-stub");
	hard_reset_stub = new IRQStub("hard-reset-stub");
	soft_reset_stub = new IRQStub("soft-reset-stub");
	mcp_slave_stub = new IRQStub("mcp-stub");
	//	tea_slave_stub = new IRQStub("tea-stub");
	smi_slave_stub = new IRQStub("smi-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Linux loader and Linux ABI translator
	linux_os = new unisim::service::os::linux_os::PowerPCLinux32<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE>("linux-os");
	//  - Debugger
	debugger = (enable_inline_debugger or enable_gdb_server) ? new Debugger("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - the optional power estimators
	il1_power_estimator = estimate_power ? new CachePowerEstimator("il1-power-estimator") : 0;
	dl1_power_estimator = estimate_power ? new CachePowerEstimator("dl1-power-estimator") : 0;
	l2_power_estimator = estimate_power ? new CachePowerEstimator("l2-power-estimator") : 0;
	itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->bus_master_sock(memory->slave_sock); // CPU <-> RAM
	external_interrupt_stub->irq_master_sock(cpu->external_interrupt_slave_sock);
	hard_reset_stub->irq_master_sock(cpu->hard_reset_slave_sock);
	soft_reset_stub->irq_master_sock(cpu->soft_reset_slave_sock);
	mcp_slave_stub->irq_master_sock(cpu->mcp_slave_sock);
	//	tea_slave_stub->irq_master_sock(cpu->tea_slave_sock);
	smi_slave_stub->irq_master_sock(cpu->smi_slave_sock);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory->memory_export;
	
	if (enable_inline_debugger or enable_gdb_server)
	{
		// Debugger <-> CPU Connections
		// Debugger <-> CPU connections
		cpu->debug_yielding_import                            >> *debugger->debug_yielding_export[0];
		cpu->trap_reporting_import                            >> *debugger->trap_reporting_export[0];
		cpu->memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import[0]                          >> cpu->disasm_export;
		*debugger->memory_import[0]                          >> cpu->memory_export;
		*debugger->registers_import[0]                       >> cpu->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> cpu->memory_access_reporting_control_export;
			
		// Debugger <-> Loader connections
		debugger->blob_import >> linux_os->blob_export_;
		
		if (enable_inline_debugger)
		{
			// inline-debugger <-> debugger connections
			*debugger->debug_event_listener_import[0]      >> inline_debugger->debug_event_listener_export;
			*debugger->debug_yielding_import[0]            >> inline_debugger->debug_yielding_export;
			inline_debugger->debug_yielding_request_import >> *debugger->debug_yielding_request_export[0];
			inline_debugger->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[0];
			inline_debugger->disasm_import                 >> *debugger->disasm_export[0];
			inline_debugger->memory_import                 >> *debugger->memory_export[0];
			inline_debugger->registers_import              >> *debugger->registers_export[0];
			inline_debugger->stmt_lookup_import            >> *debugger->stmt_lookup_export[0];
			inline_debugger->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[0];
			inline_debugger->backtrace_import              >> *debugger->backtrace_export[0];
			inline_debugger->debug_info_loading_import     >> *debugger->debug_info_loading_export[0];
			inline_debugger->data_object_lookup_import     >> *debugger->data_object_lookup_export[0];
			inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[0];
		}
		
		if (enable_gdb_server)
		{
			// gdb-server <-> debugger connections
			*debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
			*debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
			gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
			gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
			gdb_server->memory_import                 >> *debugger->memory_export[1];
			gdb_server->registers_import              >> *debugger->registers_export[1];
		}
	}

	if(estimate_power)
	{
		// Connect everything related to power estimation
		cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
		cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
		cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
		cpu->l2_power_estimator_import >> l2_power_estimator->power_estimator_export;
		cpu->l2_power_mode_import >> l2_power_estimator->power_mode_export;
		cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
		cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
		cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
		cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;

		il1_power_estimator->time_import >> sim_time->time_export;
		dl1_power_estimator->time_import >> sim_time->time_export;
		l2_power_estimator->time_import >> sim_time->time_export;
		itlb_power_estimator->time_import >> sim_time->time_export;
		dtlb_power_estimator->time_import >> sim_time->time_export;
	}

	cpu->linux_os_import >> linux_os->linux_os_export_;
	linux_os->memory_import_ >> cpu->memory_export;
	linux_os->memory_injection_import_ >> cpu->memory_injection_export;
	linux_os->registers_import_ >> cpu->registers_export;
}

Simulator::~Simulator()
{
	delete external_interrupt_stub;
	delete hard_reset_stub;
	delete soft_reset_stub;
	delete mcp_slave_stub;
//	delete tea_slave_stub;
	delete smi_slave_stub;
	delete memory;
	delete debugger;
	delete gdb_server;
	delete inline_debugger;
	delete cpu;
	delete il1_power_estimator;
	delete dl1_power_estimator;
	delete l2_power_estimator;
	delete itlb_power_estimator;
	delete dtlb_power_estimator;
	delete sim_time;
	delete linux_os;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM ppcemu");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM ppcemu, user level PowerPC simulator with support of ELF32 binaries and Linux system call translation");
	simulator->SetVariable("schematic", "ppcemu/fig_schematic.pdf");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc_32.xml";
	const char *dwarf_register_number_mapping_filename = "powerpc_eabi_gcc_dwarf_register_number_mapping.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	uint32_t tech_node = 130; // in nm
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", cpu_cycle_time);
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.3 * 1e3); // mV
	simulator->SetVariable("cpu.max-inst", maxinst);
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.ipc", cpu_ipc);
	simulator->SetVariable("cpu.enable-dmi", true); // Allow CPU to use SystemC TLM 2.0 DMI

	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", (uint32_t) -1);

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", dwarf_register_number_mapping_filename);

	//  - Linux OS run-time configuration
	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.stack-base", 0xc0000000);
	simulator->SetVariable("linux-os.envc", 0);
	simulator->SetVariable("linux-os.system", "ppc");
	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.utsname-sysname", "Linux");
 	simulator->SetVariable("linux-os.utsname-nodename", "(none)");
 	simulator->SetVariable("linux-os.utsname-release", "3.0.4");
 	simulator->SetVariable("linux-os.utsname-version", "#1 PREEMPT Thu Jan 1 00:00:00 CEST 1970");
	simulator->SetVariable("linux-os.utsname-machine", "ppc");
	simulator->SetVariable("linux-os.utsname-domainname", "(none)");
	simulator->SetVariable("linux-os.apply-host-environment", false);

	//  - Cache/TLB power estimators run-time configuration
	simulator->SetVariable("il1-power-estimator.cache-size", 32 * 1024);
	simulator->SetVariable("il1-power-estimator.line-size", 32);
	simulator->SetVariable("il1-power-estimator.associativity", 8);
	simulator->SetVariable("il1-power-estimator.rw-ports", 0);
	simulator->SetVariable("il1-power-estimator.excl-read-ports", 1);
	simulator->SetVariable("il1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("il1-power-estimator.banks", 4);
	simulator->SetVariable("il1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("il1-power-estimator.output-width", 128);
	simulator->SetVariable("il1-power-estimator.tag-width", 64);
	simulator->SetVariable("il1-power-estimator.access-mode", "fast");

	simulator->SetVariable("dl1-power-estimator.cache-size", 32 * 1024);
	simulator->SetVariable("dl1-power-estimator.line-size", 32);
	simulator->SetVariable("dl1-power-estimator.associativity", 8);
	simulator->SetVariable("dl1-power-estimator.rw-ports", 1);
	simulator->SetVariable("dl1-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.banks", 4);
	simulator->SetVariable("dl1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dl1-power-estimator.output-width", 64);
	simulator->SetVariable("dl1-power-estimator.tag-width", 64);
	simulator->SetVariable("dl1-power-estimator.access-mode", "fast");

	simulator->SetVariable("l2-power-estimator.cache-size", 512 * 1024);
	simulator->SetVariable("l2-power-estimator.line-size", 32);
	simulator->SetVariable("l2-power-estimator.associativity", 8);
	simulator->SetVariable("l2-power-estimator.rw-ports", 1);
	simulator->SetVariable("l2-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("l2-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("l2-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("l2-power-estimator.banks", 4);
	simulator->SetVariable("l2-power-estimator.tech-node", tech_node);
	simulator->SetVariable("l2-power-estimator.output-width", 256);
	simulator->SetVariable("l2-power-estimator.tag-width", 64);
	simulator->SetVariable("l2-power-estimator.access-mode", "fast");

	simulator->SetVariable("itlb-power-estimator.cache-size", 128 * 2 * 4);
	simulator->SetVariable("itlb-power-estimator.line-size", 4);
	simulator->SetVariable("itlb-power-estimator.associativity", 2);
	simulator->SetVariable("itlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("itlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("itlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.banks", 4);
	simulator->SetVariable("itlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("itlb-power-estimator.output-width", 32);
	simulator->SetVariable("itlb-power-estimator.tag-width", 64);
	simulator->SetVariable("itlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("dtlb-power-estimator.cache-size", 128 * 2 * 4);
	simulator->SetVariable("dtlb-power-estimator.line-size", 4);
	simulator->SetVariable("dtlb-power-estimator.associativity", 2);
	simulator->SetVariable("dtlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("dtlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.banks", 4);
	simulator->SetVariable("dtlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dtlb-power-estimator.output-width", 32);
	simulator->SetVariable("dtlb-power-estimator.tag-width", 64);
	simulator->SetVariable("dtlb-power-estimator.access-mode", "fast");
	
	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	void (*prev_sig_int_handler)(int) = 0;

	if(!inline_debugger)
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}

	sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages
	
	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	if(!inline_debugger)
	{
		signal(SIGINT, prev_sig_int_handler);
	}

	cerr << "Simulation finished" << endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(cerr);
	cerr << endl;
	cerr << "Simulation formulas:" << endl;
	DumpFormulas(cerr);
	cerr << endl;
	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
}

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	// Build the Linux OS arguments from the command line arguments
	
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("linux-os.binary", ((string)(*cmd_args)[0]).c_str());
		SetVariable("linux-os.argc", cmd_args_length);
		
		unsigned int i;
		for(i = 0; i < cmd_args_length; i++)
		{
			std::stringstream sstr;
			sstr << "linux-os.argv[" << i << "]";
			SetVariable(sstr.str().c_str(), ((string)(*cmd_args)[i]).c_str());
		}
	}

	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();

	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " shall not be used together. Use " << param_enable_inline_debugger.GetName() << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
		if(setup_status != unisim::kernel::service::Simulator::ST_OK_DONT_START)
		{
			setup_status = unisim::kernel::service::Simulator::ST_ERROR;
		}
	}
	
	return setup_status;
}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_PPCEMU
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
#endif
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	if(!asynchronous)
	{
		switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
		{
			case SC_THREAD_PROC_: 
			case SC_CTHREAD_PROC_:
				wait();
				break;
			default:
				break;
		}
	}
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

int sc_main(int argc, char *argv[])
{
#ifdef WIN32
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cerr << "WSAStartup failed" << endl;
		return -1;
	}
#endif
	Simulator *simulator = new Simulator(argc, argv);

	switch(simulator->Setup())
	{
		case unisim::kernel::service::Simulator::ST_OK_DONT_START:
			break;
		case unisim::kernel::service::Simulator::ST_WARNING:
			cerr << "Some warnings occurred during setup" << endl;
		case unisim::kernel::service::Simulator::ST_OK_TO_START:
			cerr << "Starting simulation at user privilege level (Linux system call translation mode)" << endl;
			simulator->Run();
			break;
		case unisim::kernel::service::Simulator::ST_ERROR:
			cerr << "Can't start simulation because of previous errors" << endl;
			break;
	}

	int exit_status = simulator->GetExitStatus();
	if(simulator) delete simulator;
#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return exit_status;
}
