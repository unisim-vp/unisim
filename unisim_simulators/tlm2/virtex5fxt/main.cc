/*
 *  Copyright (c) 2007-2011,
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

// Class definition of components
#include <unisim/component/tlm2/processor/powerpc/ppc440/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/capture_trigger_stub.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/memory/flash/am29/am29.hh>
#include <unisim/component/tlm2/interconnect/xilinx/dcr_controller/dcr_controller.hh>
#include <unisim/component/tlm2/interconnect/xilinx/crossbar/crossbar.hh>
#include <unisim/component/tlm2/interconnect/xilinx/mci/mci.hh>
#include <unisim/component/tlm2/com/xilinx/xps_uart_lite/xps_uart_lite.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/xps_gpio.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/gpio_leds.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/gpio_switches.hh>

// Simulator compile time configuration
#include <config.hh>

// Template class implementation of components
// #include <unisim/component/tlm2/interconnect/generic_router/router.tcc>
// #include <unisim/component/tlm2/com/xilinx/xps_gpio/xps_gpio.tcc>
// #include <unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.tcc>
// #include <unisim/component/tlm2/com/xilinx/xps_gpio/gpio_leds.tcc>
// #include <unisim/component/tlm2/com/xilinx/xps_gpio/gpio_switches.tcc>

// Class definition of kernel, services and interfaces
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/debug/debug.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>
#include <unisim/service/tee/loader/tee.hh>
#include <unisim/service/tee/symbol_table_lookup/tee.hh>
#include <unisim/service/telnet/telnet.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>

// Host machine standard headers
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#else
#include <signal.h>
#endif

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::loader::multiformat_loader::MultiFormatLoader;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::service::telnet::Telnet;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Variable;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Object;

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

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
	//===                  Template classes configuration                   ===
	//=========================================================================
#ifdef DEBUG_VIRTEX5FXT
	static const bool DEBUG_INFORMATION = true;
#else
	static const bool DEBUG_INFORMATION = false;
#endif

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

	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> BRAM;
	typedef unisim::component::tlm2::processor::powerpc::ppc440::CPU<CPU_CONFIG> CPU;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<MPLB_CONFIG> MPLB;
	typedef unisim::component::tlm2::interrupt::xilinx::xps_intc::XPS_IntC<INTC_CONFIG> INTC;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::XPS_Timer<TIMER_CONFIG> TIMER;
	typedef unisim::component::tlm2::memory::flash::am29::AM29<AM29_CONFIG, 32 * unisim::component::cxx::memory::flash::am29::M, 2, CPU_CONFIG::FSB_WIDTH * 8> FLASH;
	typedef unisim::component::tlm2::interconnect::xilinx::dcr_controller::DCRController<DCR_CONTROLLER_CONFIG> DCR_CONTROLLER;
	typedef unisim::component::tlm2::interconnect::xilinx::crossbar::Crossbar<CROSSBAR_CONFIG> CROSSBAR;
	typedef unisim::component::tlm2::interconnect::xilinx::mci::MCI<MCI_CONFIG> MCI;
	typedef unisim::component::tlm2::com::xilinx::xps_uart_lite::XPS_UARTLite<UART_LITE_CONFIG> UART_LITE;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_DIP_SWITCHES_8BIT_CONFIG> GPIO_DIP_SWITCHES_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_LEDS_8BIT_CONFIG> GPIO_LEDS_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_5_LEDS_POSITIONS_CONFIG> GPIO_5_LEDS_POSITIONS;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_PUSH_BUTTONS_5BIT_CONFIG> GPIO_PUSH_BUTTONS_5BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_Switches<8> DIP_SWITCHES_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_LEDs<8> LEDS_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_LEDs<5> _5_LEDS_POSITIONS;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_Switches<5> PUSH_BUTTONS_5BIT;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::PWMProtocolTypes> PWM_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutProtocolTypes> GENERATE_OUT_STUB;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerStub CAPTURE_TRIGGER_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<CPU_CONFIG::FSB_WIDTH * 8> SPLB0_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<CPU_CONFIG::FSB_WIDTH * 8> SPLB1_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<4> MASTER1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> APU_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC0_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC2_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC3_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> EXTERNAL_SLAVE_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::com::xilinx::xps_gpio::GPIOProtocolTypes> GPIO_OUTPUT_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::com::xilinx::xps_gpio::GPIOProtocolTypes> GPIO_INPUT_STUB;

	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - RAM
	RAM *ram;
	//  - BRAM
	BRAM *bram;
	// - IRQ stubs
	IRQ_STUB *input_interrupt_stub[INTC_CONFIG::C_NUM_INTR_INPUTS];
	IRQ_STUB *critical_input_interrupt_stub;
	// - MPLB
	MPLB *mplb;
	// - SPLB0
	SPLB0_STUB *splb0_stub;
	// - SPLB1
	SPLB1_STUB *splb1_stub;
	// - Interrupt controller
	INTC *intc;
	// - Timer
	TIMER *timer;
	// - Flash memory
	FLASH *flash;
	// - Capture trigger stubs
	CAPTURE_TRIGGER_STUB *capture_trigger_stub[TIMER_CONFIG::NUM_TIMERS];
	// - GenerateOutStub
	GENERATE_OUT_STUB *generate_out_stub[TIMER_CONFIG::NUM_TIMERS];
	// - PWM stub
	PWM_STUB *pwm_stub;
	// - DCR controller
	DCR_CONTROLLER *dcr_controller;
	// - Crossbar
	CROSSBAR *crossbar;
	// - MCI
	MCI *mci;
	// - UART Lite
	UART_LITE *uart_lite;
	// - GPIO DIP switches 8 Bit
	GPIO_DIP_SWITCHES_8BIT *gpio_dip_switches_8bit;
	// - GPIO LEDs 8 Bit
	GPIO_LEDS_8BIT *gpio_leds_8bit;
	// - GPIO 5 LEDs Positions
	GPIO_5_LEDS_POSITIONS *gpio_5_leds_positions;
	// - GPIO Push Buttons 5 bit
	GPIO_PUSH_BUTTONS_5BIT *gpio_push_buttons_5bit;
	// - DIP Switches 8 bit
	DIP_SWITCHES_8BIT *dip_switches_8bit;
	// - LEDs 8 bit
	LEDS_8BIT *leds_8bit;
	// - 5 LEDs Positions
	_5_LEDS_POSITIONS *_5_leds_positions;
	// - Push buttons 5 bit
	PUSH_BUTTONS_5BIT *push_buttons_5bit;
	// - DCR stubs
	MASTER1_DCR_STUB *master1_dcr_stub;
	APU_DCR_STUB *apu_dcr_stub;
	DMAC0_DCR_STUB *dmac0_dcr_stub;
	DMAC1_DCR_STUB *dmac1_dcr_stub;
	DMAC2_DCR_STUB *dmac2_dcr_stub;
	DMAC3_DCR_STUB *dmac3_dcr_stub;
	EXTERNAL_SLAVE_DCR_STUB *external_slave_dcr_stub;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Multiformat loader
	MultiFormatLoader<CPU_ADDRESS_TYPE> *loader;
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
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *ram_effective_to_physical_address_translator;
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *flash_effective_to_physical_address_translator;
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *bram_effective_to_physical_address_translator;
	// - telnet
	unisim::service::telnet::Telnet *telnet;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool estimate_power;
	bool enable_telnet;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_estimate_power;
	Parameter<bool> param_enable_telnet;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
#ifdef WIN32
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
	static void SigIntHandler(int signum);
#endif
};

const unsigned int GPIO_DIP_SWITCHES_8BIT_CONFIG::C_GPIO_WIDTH;
const unsigned int GPIO_LEDS_8BIT_CONFIG::C_GPIO_WIDTH;
const unsigned int GPIO_5_LEDS_POSITIONS_CONFIG::C_GPIO_WIDTH;
const unsigned int GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_GPIO_WIDTH;

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, ram(0)
	, bram(0)
	, critical_input_interrupt_stub(0)
	, mplb(0)
	, splb0_stub(0)
	, splb1_stub(0)
	, intc(0)
	, timer(0)
	, flash(0)
	, pwm_stub(0)
	, dcr_controller(0)
	, crossbar(0)
	, mci(0)
	, uart_lite(0)
	, gpio_dip_switches_8bit(0)
	, gpio_leds_8bit(0)
	, gpio_5_leds_positions(0)
	, gpio_push_buttons_5bit(0)
	, dip_switches_8bit(0)
	, leds_8bit(0)
	, _5_leds_positions(0)
	, push_buttons_5bit(0)
	, master1_dcr_stub(0)
	, apu_dcr_stub(0)
	, dmac0_dcr_stub(0)
	, dmac1_dcr_stub(0)
	, dmac2_dcr_stub(0)
	, dmac3_dcr_stub(0)
	, external_slave_dcr_stub(0)
	, loader(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, itlb_power_estimator(0)
	, dtlb_power_estimator(0)
	, utlb_power_estimator(0)
	, ram_effective_to_physical_address_translator(0)
	, flash_effective_to_physical_address_translator(0)
	, bram_effective_to_physical_address_translator(0)
	, telnet(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, enable_telnet(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, param_enable_telnet("enable-telnet", 0, enable_telnet, "Enable/Disable telnet instantiation")
	, exit_status(0)
{
	unsigned int irq;
	unsigned int channel;
	unsigned int i;
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - RAM
	ram = new RAM("ram");
	//  - RAM
	bram = new BRAM("bram");
	//  - IRQ Stubs
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		switch(irq)
		{
			case TIMER_IRQ:
			case UART_LITE_IRQ:
			case GPIO_DIP_SWITCHES_8BIT_IRQ:
			case GPIO_PUSH_BUTTONS_5BIT_IRQ:
				input_interrupt_stub[irq] = 0;
				break;
			default:
				{
					std::stringstream input_interrupt_stub_name_sstr;
					input_interrupt_stub_name_sstr << "input-interrupt-stub" << irq;
					input_interrupt_stub[irq] = new IRQ_STUB(input_interrupt_stub_name_sstr.str().c_str());
				}
				break;
		}
	}
	critical_input_interrupt_stub = new IRQ_STUB("critical-input-interrupt-stub");
	// - MPLB
	mplb = new MPLB("mplb");
	// - SPLB0
	splb0_stub = new SPLB0_STUB("splb0-stub");
	// - SPLB1
	splb1_stub = new SPLB0_STUB("splb1-stub");
	// - Interrupt controller
	intc = new INTC("intc");
	// - Timer
	timer = new TIMER("timer");
	// - Flash memory
	flash = new FLASH("flash");
	// - Capture trigger stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream capture_trigger_stub_name_sstr;
		capture_trigger_stub_name_sstr << "capture-trigger-stub" << channel;
		capture_trigger_stub[channel] = new CAPTURE_TRIGGER_STUB(capture_trigger_stub_name_sstr.str().c_str());
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream generate_out_stub_name_sstr;
		generate_out_stub_name_sstr << "generate-out-stub" << channel;
		generate_out_stub[channel] = new GENERATE_OUT_STUB(generate_out_stub_name_sstr.str().c_str());
	}
	// - PWM stub
	pwm_stub = new PWM_STUB("pwm-stub");
	// - DCR controller
	dcr_controller = new DCR_CONTROLLER("dcr-controller");
	// - Crossbar
	crossbar = new CROSSBAR("crossbar");
	// - MCI
	mci = new MCI("mci");
	// - UART Lite
	uart_lite = new UART_LITE("uart-lite");
	// - GPIO DIP switches 8 Bit
	gpio_dip_switches_8bit = new GPIO_DIP_SWITCHES_8BIT("gpio-dip-switches-8bit");
	// - GPIO LEDs 8 Bit
	gpio_leds_8bit = new GPIO_LEDS_8BIT("gpio-leds-8bit");
	// - GPIO 5 LEDs Positions
	gpio_5_leds_positions = new GPIO_5_LEDS_POSITIONS("gpio-5-leds-positions");
	// - GPIO Push Buttons 5 bit
	gpio_push_buttons_5bit = new GPIO_PUSH_BUTTONS_5BIT("gpio-push-buttons-5bit");
	// - DIP Switches 8 bit
	dip_switches_8bit = new DIP_SWITCHES_8BIT("dip-switches-8bit");
	// - LEDs 8 bit
	leds_8bit = new LEDS_8BIT("leds-8bit");
	// - 5 LEDs Positions
	_5_leds_positions = new _5_LEDS_POSITIONS("5-leds-positions");
	// - Push buttons 5 bit
	push_buttons_5bit = new PUSH_BUTTONS_5BIT("push-buttons-5bit");
	// - DCR stubs
	master1_dcr_stub = new MASTER1_DCR_STUB("master1-dcr-stub");
	apu_dcr_stub = new APU_DCR_STUB("apu-dcr-stub");
	dmac0_dcr_stub = new DMAC0_DCR_STUB("dma0-dcr-stub");
	dmac1_dcr_stub = new DMAC1_DCR_STUB("dma1-dcr-stub");
	dmac2_dcr_stub = new DMAC2_DCR_STUB("dma2-dcr-stub");
	dmac3_dcr_stub = new DMAC3_DCR_STUB("dma3-dcr-stub");
	external_slave_dcr_stub = new EXTERNAL_SLAVE_DCR_STUB("external-slave-dcr-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");
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
	ram_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("ram-effective-to-physical-address-translator");
	flash_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("flash-effective-to-physical-address-translator");
	bram_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("bram-effective-to-physical-address-translator");
	// - telnet
	telnet = enable_telnet ? new unisim::service::telnet::Telnet("telnet") : 0;
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->icurd_plb_master_sock(crossbar->icurd_plb_slave_sock); // CPU>ICURD <-> ICURD<Crossbar
	cpu->dcuwr_plb_master_sock(crossbar->dcuwr_plb_slave_sock); // CPU>DCUWR <-> DCUWR<Crossbar
	cpu->dcurd_plb_master_sock(crossbar->dcurd_plb_slave_sock); // CPU>DCURD <-> DCURD<Crossbar
	cpu->dcr_master_sock(*dcr_controller->dcr_slave_sock[0]); // (master 0) CPU>DCR <-> DCR controller
	master1_dcr_stub->master_sock(*dcr_controller->dcr_slave_sock[1]); // master 1>DCR <-> DCR controller
	
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::APU_SLAVE_NUM])(apu_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::MCI_SLAVE_NUM])(mci->dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::CROSSBAR_SLAVE_NUM])(crossbar->crossbar_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBS0_SLAVE_NUM])(crossbar->plbs0_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBS1_SLAVE_NUM])(crossbar->plbs1_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBM_SLAVE_NUM])(crossbar->plbm_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC0_SLAVE_NUM])(dmac0_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC1_SLAVE_NUM])(dmac1_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC2_SLAVE_NUM])(dmac2_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC3_SLAVE_NUM])(dmac3_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::EXTERNAL_SLAVE_NUM])(external_slave_dcr_stub->slave_sock);
	
	crossbar->mplb_master_sock(*mplb->targ_socket[0]);   // crossbar>MPLB <-> MPLB
	crossbar->mci_master_sock(mci->mci_slave_sock);      // crossbar>MCI <-> MCI
	mci->mci_master_sock(ram->slave_sock);               // MCI <-> RAM 
	
	splb0_stub->master_sock(crossbar->splb0_slave_sock);  // SPLB0 stub <-> SPLB0<Crossbar
	splb1_stub->master_sock(crossbar->splb1_slave_sock);  // SPLB1 stub <-> SPLB1<Crossbar
	
	(*mplb->init_socket[0])(intc->slave_sock);      // MPLB <-> INTC
	(*mplb->init_socket[1])(timer->slave_sock);     // MPLB <-> TIMER
	(*mplb->init_socket[2])(flash->slave_sock);     // MPLB <-> FLASH
	(*mplb->init_socket[3])(bram->slave_sock);      // MPLB <-> BRAM
	(*mplb->init_socket[4])(uart_lite->slave_sock); // MPLB <-> UART Lite
	(*mplb->init_socket[5])(gpio_dip_switches_8bit->slave_sock);      // MPLB <-> GPIO DIP switches 8 Bit
	(*mplb->init_socket[6])(gpio_leds_8bit->slave_sock);              // MPLB <-> GPIO LEDs 8 Bit
	(*mplb->init_socket[7])(gpio_5_leds_positions->slave_sock);       // MPLB <-> GPIO 5 LEDs Positions
	(*mplb->init_socket[8])(gpio_push_buttons_5bit->slave_sock);      // MPLB <-> GPIO Push Buttons 5 bit
	
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		switch(irq)
		{
			case TIMER_IRQ:
				timer->interrupt_master_sock(*intc->irq_slave_sock[irq]); // TIMER>IRQ <-> INTR<INTC
				break;
			case UART_LITE_IRQ:
				uart_lite->interrupt_master_sock(*intc->irq_slave_sock[irq]); // UART Lite>IRQ <-> INTR<INTC
				break;
			case GPIO_DIP_SWITCHES_8BIT_IRQ:
				(*gpio_dip_switches_8bit->interrupt_master_sock)(*intc->irq_slave_sock[irq]); // GPIO DIP SWITCHES 8BIT>IRQ <-> INTR<INTC
				break;
			case GPIO_PUSH_BUTTONS_5BIT_IRQ:
				(*gpio_push_buttons_5bit->interrupt_master_sock)(*intc->irq_slave_sock[irq]); // GPIO PUSH BUTTONS 5BIT>IRQ <-> INTR<INTC
				break;
			default:
				(input_interrupt_stub[irq]->master_sock)(*intc->irq_slave_sock[irq]); // IRQ stub>IRQ <-> INTR<INTC
				break;
		}
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		capture_trigger_stub[channel]->master_sock(*timer->capture_trigger_slave_sock[channel]); // Capture trigger stub <-> TIMER
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		(*timer->generate_out_master_sock[channel])(generate_out_stub[channel]->slave_sock); // TIMER <-> Generate out stub
	}
	timer->pwm_master_sock(pwm_stub->slave_sock); // TIMER <-> PWM stub
	intc->irq_master_sock(cpu->external_input_interrupt_slave_sock); // INTC>IRQ <-> External Input<CPU
	critical_input_interrupt_stub->master_sock(cpu->critical_input_interrupt_slave_sock); // IRQ Stub <-> CPU
	
	for(i = 0; i < GPIO_DIP_SWITCHES_8BIT_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*dip_switches_8bit->gpio_master_sock[i])(*gpio_dip_switches_8bit->gpio_slave_sock[i]);
		(*gpio_dip_switches_8bit->gpio_master_sock[i])(*dip_switches_8bit->gpio_slave_sock[i]);
	}
	for(i = 0; i < GPIO_LEDS_8BIT_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*gpio_leds_8bit->gpio_master_sock[i])(*leds_8bit->gpio_slave_sock[i]);
		(*leds_8bit->gpio_master_sock[i])(*gpio_leds_8bit->gpio_slave_sock[i]);
	}
	for(i = 0; i < GPIO_5_LEDS_POSITIONS_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*gpio_5_leds_positions->gpio_master_sock[i])(*_5_leds_positions->gpio_slave_sock[i]);
		(*_5_leds_positions->gpio_master_sock[i])(*gpio_5_leds_positions->gpio_slave_sock[i]);
	}
	for(i = 0; i < GPIO_PUSH_BUTTONS_5BIT_CONFIG::C_GPIO_WIDTH; i++)
	{
		(*push_buttons_5bit->gpio_master_sock[i])(*gpio_push_buttons_5bit->gpio_slave_sock[i]);
		(*gpio_push_buttons_5bit->gpio_master_sock[i])(*push_buttons_5bit->gpio_slave_sock[i]);
	}

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> crossbar->memory_export;
	
	crossbar->mci_memory_import >> mci->memory_export;
	crossbar->mplb_memory_import >> mplb->memory_export;
	mci->memory_import >> ram->memory_export;
	(*mplb->memory_import[0]) >> intc->memory_export;
	(*mplb->memory_import[1]) >> timer->memory_export;
	(*mplb->memory_import[2]) >> flash->memory_export;
	(*mplb->memory_import[3]) >> bram->memory_export;
	(*mplb->memory_import[4]) >> uart_lite->memory_export;
	(*mplb->memory_import[5]) >> gpio_dip_switches_8bit->memory_export;
	(*mplb->memory_import[6]) >> gpio_leds_8bit->memory_export;
	(*mplb->memory_import[7]) >> gpio_5_leds_positions->memory_export;
	(*mplb->memory_import[8]) >> gpio_push_buttons_5bit->memory_export;
	cpu->loader_import >> loader->loader_export;
	
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
		*inline_debugger->loader_import[0] >> loader->loader_export;
		*inline_debugger->stmt_lookup_import[0] >> loader->stmt_lookup_export;
		*inline_debugger->symbol_table_lookup_import[0] >> loader->symbol_table_lookup_export;
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

	ram_effective_to_physical_address_translator->memory_import >> ram->memory_export;
	flash_effective_to_physical_address_translator->memory_import >> flash->memory_export;
	bram_effective_to_physical_address_translator->memory_import >> bram->memory_export;
	*loader->memory_import[0] >> ram_effective_to_physical_address_translator->memory_export;
	*loader->memory_import[1] >> bram_effective_to_physical_address_translator->memory_export;
	*loader->memory_import[2] >> flash_effective_to_physical_address_translator->memory_export;
	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

	if(enable_telnet)
	{
		uart_lite->char_io_import >> telnet->char_io_export;
	}
}

Simulator::~Simulator()
{
	unsigned int irq;
	unsigned int channel;
	if(critical_input_interrupt_stub) delete critical_input_interrupt_stub;
	if(ram) delete ram;
	if(bram) delete bram;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(mplb) delete mplb;
	if(splb0_stub) delete splb0_stub;
	if(splb1_stub) delete splb1_stub;
	if(intc) delete intc;
	if(timer) delete timer;
	if(flash) delete flash;
	if(crossbar) delete crossbar;
	if(mci) delete mci;
	if(uart_lite) delete uart_lite;
	if(gpio_dip_switches_8bit) delete gpio_dip_switches_8bit;
	if(gpio_leds_8bit) delete gpio_leds_8bit;
	if(gpio_5_leds_positions) delete gpio_5_leds_positions;
	if(gpio_push_buttons_5bit) delete gpio_push_buttons_5bit;
	if(dip_switches_8bit) delete dip_switches_8bit;
	if(leds_8bit) delete leds_8bit;
	if(_5_leds_positions) delete _5_leds_positions;
	if(push_buttons_5bit) delete push_buttons_5bit;
	if(master1_dcr_stub) delete master1_dcr_stub;
	if(apu_dcr_stub) delete apu_dcr_stub;
	if(dmac0_dcr_stub) delete dmac0_dcr_stub;
	if(dmac1_dcr_stub) delete dmac1_dcr_stub;
	if(dmac2_dcr_stub) delete dmac2_dcr_stub;
	if(dmac3_dcr_stub) delete dmac3_dcr_stub;
	if(external_slave_dcr_stub) delete external_slave_dcr_stub;
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(input_interrupt_stub[irq]) delete input_interrupt_stub[irq];
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(capture_trigger_stub[channel]) delete capture_trigger_stub[channel];
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(generate_out_stub[channel]) delete generate_out_stub[channel];
	}
	if(pwm_stub) delete pwm_stub;
	if(dcr_controller) delete dcr_controller;
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(utlb_power_estimator) delete utlb_power_estimator;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
	if(ram_effective_to_physical_address_translator) delete ram_effective_to_physical_address_translator;
	if(bram_effective_to_physical_address_translator) delete bram_effective_to_physical_address_translator;
	if(flash_effective_to_physical_address_translator) delete flash_effective_to_physical_address_translator;
	if(telnet) delete telnet;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM Virtex 5 FXT");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2011, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM Virtex 5 FXT, full system PPC440x5 based simulator including some Virtex 5 IPs");
	simulator->SetVariable("schematic", "virtex5fxt/fig_schematic.pdf");

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
	simulator->SetVariable("cpu.nice-time", "200 ns"); // 200 ns (currently geared to the minimum interval between capture trigger samples)
	simulator->SetVariable("cpu.ipc", cpu_ipc);

	//  - DCR controller
	simulator->SetVariable("dcr-controller.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Crossbar
	simulator->SetVariable("crossbar.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - MCI
	simulator->SetVariable("mci.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - MPLB
	simulator->SetVariable("mplb.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("mplb.mapping_0", "range_start=\"0x81800000\" range_end=\"0x8180ffff\" output_port=\"0\" translation=\"0x81800000\""); // XPS IntC
	simulator->SetVariable("mplb.mapping_1", "range_start=\"0x83c00000\" range_end=\"0x83c0ffff\" output_port=\"1\" translation=\"0x83c00000\""); // XPS Timer/Counter
	simulator->SetVariable("mplb.mapping_2", "range_start=\"0xfc000000\" range_end=\"0xfdffffff\" output_port=\"2\" translation=\"0xfc000000\""); // 32 MB Flash memory (i.e. 1 * 256 Mbits S29GL256P flash memory chips)
	simulator->SetVariable("mplb.mapping_3", "range_start=\"0xfffc0000\" range_end=\"0xffffffff\" output_port=\"3\" translation=\"0xfffc0000\""); // 256 KB XPS BRAM
	simulator->SetVariable("mplb.mapping_4", "range_start=\"0x84000000\" range_end=\"0x8400ffff\" output_port=\"4\" translation=\"0x84000000\""); // XPS UART Lite
	simulator->SetVariable("mplb.mapping_5", "range_start=\"0x81460000\" range_end=\"0x8146ffff\" output_port=\"5\" translation=\"0x81460000\""); // GPIO DIP SWITCHES 8BIT
	simulator->SetVariable("mplb.mapping_6", "range_start=\"0x81400000\" range_end=\"0x8140ffff\" output_port=\"6\" translation=\"0x81400000\""); // GPIO LEDS 8BIT
	simulator->SetVariable("mplb.mapping_7", "range_start=\"0x81420000\" range_end=\"0x8142ffff\" output_port=\"7\" translation=\"0x81420000\""); // GPIO 5 LEDS POSITIONS
	simulator->SetVariable("mplb.mapping_8", "range_start=\"0x81440000\" range_end=\"0x8144ffff\" output_port=\"8\" translation=\"0x81440000\""); // GPIO PUSH BUTTONS 5BIT
	
	// - Loader memory router
	simulator->SetVariable("loader.memory-mapper.mapping", "ram-effective-to-physical-address-translator:0x00000000-0x0fffffff,bram-effective-to-physical-address-translator:0xfffc0000-0xffffffff,flash-effective-to-physical-address-translator:0xfc000000-0xfdffffff"); // 256 MB RAM / 256 KB BRAM / 32 MB Flash memory

	//  - RAM
	simulator->SetVariable("ram.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("ram.org", 0x00000000UL);
	simulator->SetVariable("ram.bytesize", 256 * 1024 * 1024); // 256 MB

	//  - BRAM
	simulator->SetVariable("bram.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("bram.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("bram.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("bram.org", 0xfffc0000UL);
	simulator->SetVariable("bram.bytesize", 256 * 1024); // 256 KB
	
	//  - Interrupt controller
	simulator->SetVariable("intc.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Timer
	simulator->SetVariable("timer.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - UART Lite
	simulator->SetVariable("uart-lite.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - GPIOs
	simulator->SetVariable("gpio-dip-switches-8bit.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("gpio-leds-8bit.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("gpio-5-leds-positions.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("gpio-push-buttons-5bit.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Flash
	simulator->SetVariable("flash.org", 0xfc000000UL);
	simulator->SetVariable("flash.bytesize", 32 * 1024 * 1024); // 32 MB
	simulator->SetVariable("flash.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	
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
	simulator->SetVariable("inline-debugger.num-loaders", 4);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

#ifndef WIN32
	void (*prev_sig_int_handler)(int) = 0;
#endif

	if(!inline_debugger)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, TRUE);
#else
		prev_sig_int_handler = signal(SIGINT, &Simulator::SigIntHandler);
#endif
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
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, FALSE);
#else
		signal(SIGINT, prev_sig_int_handler);
#endif
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
	cerr << "target speed: " << ((double) (*cpu)["instruction-counter"] / ((double) (*cpu)["run-time"] - (double) (*cpu)["idle-time"]) / 1000000.0) << " MIPS" << endl;
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
	
	// Optionally get the program to load from the command line arguments
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("elf32-loader.filename", ((string)(*cmd_args)[0]).c_str());
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

int Simulator::GetExitStatus() const
{
	return exit_status;
}

#ifdef WIN32
BOOL WINAPI Simulator::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	bool stop = false;
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
			cerr << "Interrupted by Ctrl-C" << endl;
			stop = true;
			break;
		case CTRL_BREAK_EVENT:
			cerr << "Interrupted by Ctrl-Break" << endl;
			stop = true;
			break;
		case CTRL_CLOSE_EVENT:
			cerr << "Interrupted by a console close" << endl;
			stop = true;
			break;
		case CTRL_LOGOFF_EVENT:
			cerr << "Interrupted because of logoff" << endl;
			stop = true;
			break;
		case CTRL_SHUTDOWN_EVENT:
			cerr << "Interrupted because of shutdown" << endl;
			stop = true;
			break;
	}
	if(stop) sc_stop();
	return stop ? TRUE : FALSE;
}
#else
void Simulator::SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}
#endif

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
