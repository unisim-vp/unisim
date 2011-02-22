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

#include <unisim/component/cxx/processor/powerpc/ppc440/config.hh>
#include <unisim/component/tlm2/processor/powerpc/ppc440/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/cxx/interrupt/xilinx/xps_intc/config.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/cxx/timer/xilinx/xps_timer/config.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/debug/debug.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/elf_loader/elf32_loader.hh>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>

#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/util/random/random.hh>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef WITH_FPU
#ifdef DEBUG_VIRTEX5FXT
static const bool DEBUG_INFORMATION = true;
typedef unisim::component::cxx::processor::powerpc::ppc440::DebugConfig_wFPU CPU_CONFIG;
#else
static const bool DEBUG_INFORMATION = false;
typedef unisim::component::cxx::processor::powerpc::ppc440::Config_wFPU CPU_CONFIG;
#endif
#else
#ifdef DEBUG_VIRTEX5FXT
static const bool DEBUG_INFORMATION = true;
typedef unisim::component::cxx::processor::powerpc::ppc440::DebugConfig CPU_CONFIG;
#else
static const bool DEBUG_INFORMATION = false;
typedef unisim::component::cxx::processor::powerpc::ppc440::Config CPU_CONFIG;
#endif
#endif

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Variable;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Object;

#ifdef DEBUG_VIRTEX5FXT
class RouterDebugConfig : public unisim::component::tlm2::interconnect::generic_router::VerboseConfig
{
public:
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 3;
	static const unsigned int MAX_NUM_MAPPINGS = 5;
	static const unsigned int BUSWIDTH = 128;
};

typedef RouterDebugConfig ROUTER_CONFIG;
#else
class RouterConfig : public unisim::component::tlm2::interconnect::generic_router::Config
{
public:
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 3;
	static const unsigned int MAX_NUM_MAPPINGS = 5;
	static const unsigned int BUSWIDTH = 128;
};

typedef RouterConfig ROUTER_CONFIG;
#endif

typedef unisim::component::cxx::interrupt::xilinx::xps_intc::Config INTC_CONFIG;
typedef unisim::component::cxx::timer::xilinx::xps_timer::Config TIMER_CONFIG;
static const unsigned int TIMER_IRQ = 3;

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

class CaptureTriggerStub
	: public Object
	, public sc_module
	, tlm::tlm_bw_transport_if<unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerProtocolTypes>
{
public:
	tlm::tlm_initiator_socket<0, unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerProtocolTypes> capture_trigger_master_sock;
	
	CaptureTriggerStub(const sc_module_name& name, Object *parent = 0);

	virtual tlm::tlm_sync_enum nb_transport_bw(unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	void Process();
private:
	unisim::kernel::logger::Logger logger;
	unisim::util::random::Random random;
	unisim::kernel::tlm2::PayloadFabric<unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload> capture_trigger_payload_fabric;
	sc_time cycle_time;
	sc_time nice_time;
	sc_time time;
	bool verbose;
	Parameter<sc_time> param_cycle_time;
	Parameter<sc_time> param_nice_time;
	Parameter<bool> param_verbose;
};

CaptureTriggerStub::CaptureTriggerStub(const sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_module(name)
	, capture_trigger_master_sock("capture-trigger-master-sock")
	, logger(*this)
	, random(-34525, +96489, -57430854, +786329085)
	, capture_trigger_payload_fabric()
	, cycle_time(SC_ZERO_TIME)
	, nice_time(sc_time(1, SC_MS))
	, time(SC_ZERO_TIME)
	, verbose(false)
	, param_cycle_time("cycle-time", this, cycle_time, "cycle time")
	, param_nice_time("nice-time", this, nice_time, "nice time")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	capture_trigger_master_sock(*this);
	
	SC_HAS_PROCESS(CaptureTriggerStub);
	
	SC_THREAD(Process);
}

tlm::tlm_sync_enum CaptureTriggerStub::nb_transport_bw(unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::END_REQ:
			return tlm::TLM_ACCEPTED;
		case tlm::BEGIN_RESP:
			return tlm::TLM_COMPLETED;
		default:
			logger << unisim::kernel::logger::DebugError << "protocol error" << unisim::kernel::logger::EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

void CaptureTriggerStub::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

void CaptureTriggerStub::Process()
{
	bool flipflop = false;
	
	while(1)
	{
		int32_t r = random.Generate(500) + 501;
		sc_time delay(cycle_time);
		delay *= r;
		time += delay;
		
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << (sc_time_stamp() + time) << ": Output goes " << (flipflop ? "high" : "low") << unisim::kernel::logger::EndDebugInfo;
		}
		
		unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload *capture_trigger_payload = capture_trigger_payload_fabric.allocate();
		
		capture_trigger_payload->SetValue(flipflop);
		
		flipflop = !flipflop;
		
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		capture_trigger_master_sock->nb_transport_fw(*capture_trigger_payload, phase, time);
		
		capture_trigger_payload->release();
		
		if(time >= nice_time)
		{
			wait(time);
			time = SC_ZERO_TIME;
		}
	}
}

class PWMStub
	: public sc_module
	, tlm::tlm_fw_transport_if<unisim::component::tlm2::timer::xilinx::xps_timer::PWMProtocolTypes>
{
public:
	tlm::tlm_target_socket<0, unisim::component::tlm2::timer::xilinx::xps_timer::PWMProtocolTypes> pwm_slave_sock;
	
	PWMStub(const sc_module_name& name);

	virtual void b_transport(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans, sc_core::sc_time& t);

	virtual tlm::tlm_sync_enum nb_transport_fw(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual unsigned int transport_dbg(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans);
	
	virtual bool get_direct_mem_ptr(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans, tlm::tlm_dmi& dmi_data);
};

PWMStub::PWMStub(const sc_module_name& name)
	: sc_module(name)
	, pwm_slave_sock("pwm-slave-sock")
{
	pwm_slave_sock(*this);
}

void PWMStub::b_transport(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans, sc_core::sc_time& t)
{
}

tlm::tlm_sync_enum PWMStub::nb_transport_fw(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return tlm::TLM_COMPLETED;
}

unsigned int PWMStub::transport_dbg(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans)
{
	return 0;
}
	
bool PWMStub::get_direct_mem_ptr(unisim::component::tlm2::timer::xilinx::xps_timer::PWMPayload& trans, tlm::tlm_dmi& dmi_data)
{
	return false;
}

class GenerateOutStub
	: public sc_module
	, tlm::tlm_fw_transport_if<unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutProtocolTypes>
{
public:
	tlm::tlm_target_socket<0, unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutProtocolTypes> generate_out_slave_sock;
	
	GenerateOutStub(const sc_module_name& name);

	virtual void b_transport(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans, sc_core::sc_time& t);

	virtual tlm::tlm_sync_enum nb_transport_fw(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual unsigned int transport_dbg(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans);
	
	virtual bool get_direct_mem_ptr(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans, tlm::tlm_dmi& dmi_data);
};

GenerateOutStub::GenerateOutStub(const sc_module_name& name)
	: sc_module(name)
	, generate_out_slave_sock("generate-out-slave-sock")
{
	generate_out_slave_sock(*this);
}

void GenerateOutStub::b_transport(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans, sc_core::sc_time& t)
{
}

tlm::tlm_sync_enum GenerateOutStub::nb_transport_fw(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return tlm::TLM_COMPLETED;
}

unsigned int GenerateOutStub::transport_dbg(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans)
{
	return 0;
}
	
bool GenerateOutStub::get_direct_mem_ptr(unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutPayload& trans, tlm::tlm_dmi& dmi_data)
{
	return false;
}

class Simulator : public unisim::kernel::service::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::service::Simulator::SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// Front Side Bus template parameters
	typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	typedef CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> MEMORY;
	typedef unisim::component::tlm2::processor::powerpc::ppc440::CPU<CPU_CONFIG> CPU;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<ROUTER_CONFIG> ROUTER;
	typedef unisim::component::tlm2::interrupt::xilinx::xps_intc::XPS_IntC<INTC_CONFIG> INTC;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::XPS_Timer<TIMER_CONFIG> TIMER;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - RAM
	MEMORY *memory;
	// - IRQ stubs
	IRQStub *input_interrupt_stub[INTC_CONFIG::C_NUM_INTR_INPUTS];
	//IRQStub *external_input_interrupt_stub;
	IRQStub *critical_input_interrupt_stub;
	// - Router
	ROUTER *router;
	// - Interrupt controller
	INTC *intc;
	// - Timer
	TIMER *timer;
	// - Capture trigger stubs
	CaptureTriggerStub *capture_trigger_stub[TIMER_CONFIG::NUM_TIMERS];
	// - GenerateOutStub
	GenerateOutStub *generate_out_stub[TIMER_CONFIG::NUM_TIMERS];
	// - PWM stub
	PWMStub *pwm_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - ELF32 loader
	Elf32Loader<CPU_ADDRESS_TYPE> *elf32_loader;
	//  - ROM loader (ELF32)
	Elf32Loader<CPU_ADDRESS_TYPE> *rom_loader;
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
	CachePowerEstimator *itlb_power_estimator;
	CachePowerEstimator *dtlb_power_estimator;
	CachePowerEstimator *utlb_power_estimator;
	//  - memory address translator from effective address to physical address
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *effective_to_physical_address_translator;

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
	//, external_input_interrupt_stub(0)
	, critical_input_interrupt_stub(0)
	, router(0)
	, intc(0)
	, timer(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, itlb_power_estimator(0)
	, dtlb_power_estimator(0)
	, utlb_power_estimator(0)
	, effective_to_physical_address_translator(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, exit_status(0)
{
	unsigned int irq;
	unsigned int channel;
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - RAM
	memory = new MEMORY("memory");
	//  - IRQ Stubs
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(irq == TIMER_IRQ)
		{
			input_interrupt_stub[irq] = 0;
		}
		else
		{
			std::stringstream input_interrupt_stub_name_sstr;
			input_interrupt_stub_name_sstr << "input-interrupt-stub" << irq;
			input_interrupt_stub[irq] = new IRQStub(input_interrupt_stub_name_sstr.str().c_str());
		}
	}
	//external_input_interrupt_stub = new IRQStub("external-input-interrupt-stub");
	critical_input_interrupt_stub = new IRQStub("critical-input-interrupt-stub");
	// - Router
	router = new ROUTER("router");
	// - Interrupt controller
	intc = new INTC("intc");
	// - Timer
	timer = new TIMER("timer");
	// - Capture trigger stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream capture_trigger_stub_name_sstr;
		capture_trigger_stub_name_sstr << "capture-trigger-stub" << channel;
		capture_trigger_stub[channel] = new CaptureTriggerStub(capture_trigger_stub_name_sstr.str().c_str());
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream generate_out_stub_name_sstr;
		generate_out_stub_name_sstr << "generate-out-stub" << channel;
		generate_out_stub[channel] = new GenerateOutStub(generate_out_stub_name_sstr.str().c_str());
	}
	// - PWM stub
	pwm_stub = new PWMStub("pwm-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - ELF32 loader
	elf32_loader = new Elf32Loader<CPU_ADDRESS_TYPE>("elf32-loader");
	//  - ROM Loader
	rom_loader = new Elf32Loader<CPU_ADDRESS_TYPE>("rom-loader");
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
	itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;
	utlb_power_estimator = estimate_power ? new CachePowerEstimator("utlb-power-estimator") : 0;
	//  - memory address translator from effective address to physical address
	effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("effective-to-physical-address-translator");
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->bus_master_sock(*router->targ_socket[0]); // CPU <-> PLB
	(*router->init_socket[0])(memory->slave_sock); // PLB <-> RAM
	//cpu->bus_master_sock(memory->slave_sock); // CPU <-> RAM
	(*router->init_socket[1])(intc->slave_sock); // PLB <-> INTC
	(*router->init_socket[2])(timer->slave_sock); // PLB <-> TIMER
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(irq == TIMER_IRQ)
		{
			timer->interrupt_master_sock(*intc->irq_slave_sock[irq]); // INTC <-> TIMER
		}
		else
		{
			(input_interrupt_stub[irq]->irq_master_sock)(*intc->irq_slave_sock[irq]); // INTC <-> IRQ stub
		}
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		capture_trigger_stub[channel]->capture_trigger_master_sock(*timer->capture_trigger_slave_sock[channel]); // TIMER <-> Capture trigger stub
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		(*timer->generate_out_master_sock[channel])(generate_out_stub[channel]->generate_out_slave_sock); // TIMER <-> Generate out stub
	}
	timer->pwm_master_sock(pwm_stub->pwm_slave_sock); // TIMER <-> PWM stub
	intc->irq_master_sock(cpu->external_input_interrupt_slave_sock); // INTC <-> CPU
	//external_input_interrupt_stub->irq_master_sock(cpu->external_input_interrupt_slave_sock);
	critical_input_interrupt_stub->irq_master_sock(cpu->critical_input_interrupt_slave_sock); // IRQ Stub <-> CPU

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory->memory_export;
	cpu->loader_import >> elf32_loader->loader_export;
	
	if(enable_inline_debugger)
	{
		// Connect inline-debugger to CPU
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
		inline_debugger->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
		*inline_debugger->loader_import[0] >> elf32_loader->loader_export;
		*inline_debugger->stmt_lookup_import[0] >> elf32_loader->stmt_lookup_export;
		*inline_debugger->symbol_table_lookup_import[0] >> elf32_loader->symbol_table_lookup_export;
	}
	else if(enable_gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
	}

	if(estimate_power)
	{
		// Connect everything related to power estimation
		cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
		cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
		cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
		cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
		cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
		cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
		cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;
		cpu->utlb_power_estimator_import >> utlb_power_estimator->power_estimator_export;
		cpu->utlb_power_mode_import >> utlb_power_estimator->power_mode_export;

		il1_power_estimator->time_import >> sim_time->time_export;
		dl1_power_estimator->time_import >> sim_time->time_export;
		itlb_power_estimator->time_import >> sim_time->time_export;
		dtlb_power_estimator->time_import >> sim_time->time_export;
		utlb_power_estimator->time_import >> sim_time->time_export;
	}

	effective_to_physical_address_translator->memory_import >> memory->memory_export;
	elf32_loader->memory_import >> effective_to_physical_address_translator->memory_export;
	rom_loader->memory_import >> effective_to_physical_address_translator->memory_export;
	cpu->symbol_table_lookup_import >> elf32_loader->symbol_table_lookup_export;
}

Simulator::~Simulator()
{
	unsigned int irq;
	//if(external_input_interrupt_stub) delete external_input_interrupt_stub;
	if(critical_input_interrupt_stub) delete critical_input_interrupt_stub;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(router) delete router;
	if(intc) delete intc;
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(input_interrupt_stub[irq]) delete input_interrupt_stub[irq];
	}
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(utlb_power_estimator) delete utlb_power_estimator;
	if(sim_time) delete sim_time;
	if(elf32_loader) delete elf32_loader;
	if(rom_loader) delete rom_loader;
	if(effective_to_physical_address_translator) delete effective_to_physical_address_translator;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM Virtex 5 FXT");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM Virtex 5 FXT, full system PPC440 simulator with support of ELF32 binaries");

	const char *filename = "";
	const char *rom_filename = "boot.elf";
	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 400.0; // in Mhz
	double cpu_clock_multiplier = 2.0;
	double ext_timer_clock_divisor = 2.0;
	uint32_t tech_node = 130; // in nm
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double ext_timer_cycle_time = ext_timer_clock_divisor * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", sc_time(cpu_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.ext-timer-cycle-time", sc_time(ext_timer_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.3 * 1e3); // mV
	simulator->SetVariable("cpu.max-inst", maxinst);
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.ipc", cpu_ipc);

	//  - Router
	simulator->SetVariable("router.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("router.mapping_0", "range_start=\"0x0\" range_end=\"0x3fffffff\" output_port=\"0\" translation=\"0x0\"");
	simulator->SetVariable("router.mapping_1", "range_start=\"0x41200000\" range_end=\"0x4120ffff\" output_port=\"1\" translation=\"0x41200000\"");
	simulator->SetVariable("router.mapping_2", "range_start=\"0x41210000\" range_end=\"0x83bfffff\" output_port=\"0\" translation=\"0x41210000\"");
	simulator->SetVariable("router.mapping_3", "range_start=\"0x83c00000\" range_end=\"0x83c0ffff\" output_port=\"2\" translation=\"0x83c00000\"");
	simulator->SetVariable("router.mapping_4", "range_start=\"0x83c10000\" range_end=\"0xffffffff\" output_port=\"0\" translation=\"0x83c10000\"");

	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", 0xffffffffffffffffULL);

	//  - Interrupt controller
	simulator->SetVariable("intc.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Timer
	simulator->SetVariable("timer.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Capture Trigger stubs
	simulator->SetVariable("capture-trigger-stub0.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("capture-trigger-stub0.nice-time", "1 ms");
	simulator->SetVariable("capture-trigger-stub1.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("capture-trigger-stub1.nice-time", "1 ms");
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	//  - ELF32 Loader run-time configuration
	simulator->SetVariable("elf32-loader.filename", filename);
	//  - ROM Loader run-time configuration
	simulator->SetVariable("rom-loader.filename", rom_filename);

	//  - Cache/TLB power estimators run-time configuration
	simulator->SetVariable("il1-power-estimator.cache-size", CPU_CONFIG::DL1_CONFIG::CACHE_SIZE);
	simulator->SetVariable("il1-power-estimator.line-size", CPU_CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE);
	simulator->SetVariable("il1-power-estimator.associativity", CPU_CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY);
	simulator->SetVariable("il1-power-estimator.rw-ports", 0);
	simulator->SetVariable("il1-power-estimator.excl-read-ports", 1);
	simulator->SetVariable("il1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("il1-power-estimator.banks", 4);
	simulator->SetVariable("il1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("il1-power-estimator.output-width", 64);
	simulator->SetVariable("il1-power-estimator.tag-width", CPU_CONFIG::ICDBTRL_BITSIZE + CPU_CONFIG::ICDBTRH_BITSIZE);
	simulator->SetVariable("il1-power-estimator.access-mode", "fast");

	simulator->SetVariable("dl1-power-estimator.cache-size", CPU_CONFIG::IL1_CONFIG::CACHE_SIZE);
	simulator->SetVariable("dl1-power-estimator.line-size", CPU_CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE);
	simulator->SetVariable("dl1-power-estimator.associativity", CPU_CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY);
	simulator->SetVariable("dl1-power-estimator.rw-ports", 1);
	simulator->SetVariable("dl1-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.banks", 4);
	simulator->SetVariable("dl1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dl1-power-estimator.output-width", 64);
	simulator->SetVariable("dl1-power-estimator.tag-width", CPU_CONFIG::DCDBTRL_BITSIZE + CPU_CONFIG::DCDBTRH_BITSIZE);
	simulator->SetVariable("dl1-power-estimator.access-mode", "fast");

	simulator->SetVariable("itlb-power-estimator.cache-size", CPU_CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES * (CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("itlb-power-estimator.line-size", (CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("itlb-power-estimator.associativity", CPU_CONFIG::ITLB_CONFIG::TLB_ASSOCIATIVITY);
	simulator->SetVariable("itlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("itlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("itlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.banks", 4);
	simulator->SetVariable("itlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("itlb-power-estimator.output-width", CPU_CONFIG::TLBE_DATA_BITSIZE);
	simulator->SetVariable("itlb-power-estimator.tag-width", CPU_CONFIG::TLBE_TAG_BITSIZE);
	simulator->SetVariable("itlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("dtlb-power-estimator.cache-size", CPU_CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES * (CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("dtlb-power-estimator.line-size", (CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("dtlb-power-estimator.associativity", CPU_CONFIG::DTLB_CONFIG::TLB_ASSOCIATIVITY);
	simulator->SetVariable("dtlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("dtlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.banks", 4);
	simulator->SetVariable("dtlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dtlb-power-estimator.output-width", CPU_CONFIG::TLBE_DATA_BITSIZE);
	simulator->SetVariable("dtlb-power-estimator.tag-width", CPU_CONFIG::TLBE_TAG_BITSIZE);
	simulator->SetVariable("dtlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("utlb-power-estimator.cache-size", CPU_CONFIG::UTLB_CONFIG::TLB_NUM_ENTRIES * (CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("utlb-power-estimator.line-size", (CPU_CONFIG::TLBE_DATA_BITSIZE + 7 / 8));
	simulator->SetVariable("utlb-power-estimator.associativity", CPU_CONFIG::UTLB_CONFIG::TLB_ASSOCIATIVITY);
	simulator->SetVariable("utlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("utlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("utlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("utlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("utlb-power-estimator.banks", 4);
	simulator->SetVariable("utlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("utlb-power-estimator.output-width", CPU_CONFIG::TLBE_DATA_BITSIZE);
	simulator->SetVariable("utlb-power-estimator.tag-width", CPU_CONFIG::TLBE_TAG_BITSIZE);
	simulator->SetVariable("utlb-power-estimator.access-mode", "fast");

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
	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "WARNING! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " should not be used together. Use " << param_enable_inline_debugger.GetName() << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
	}
	
	// Build the Linux OS arguments from the command line arguments
	
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("elf32-loader.filename", ((string)(*cmd_args)[0]).c_str());
/*		SetVariable("linux-loader.argc", cmd_args_length);
		
		unsigned int i;
		for(i = 0; i < cmd_args_length; i++)
		{
			std::stringstream sstr;
			sstr << "linux-loader.argv[" << i << "]";
			SetVariable(sstr.str().c_str(), ((string)(*cmd_args)[i]).c_str());
		}*/
	}

	return unisim::kernel::service::Simulator::Setup();
}

void Simulator::Stop(Object *object, int _exit_status)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_VIRTEX5FXT
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
#endif
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	wait();
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
			cerr << "Starting simulation at supervisor privilege level" << endl;
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
