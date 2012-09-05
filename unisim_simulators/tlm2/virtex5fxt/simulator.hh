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

#ifndef __VIRTEX5FXT_SIMULATOR_HH__
#define __VIRTEX5FXT_SIMULATOR_HH__

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

// Class definition of kernel, services and interfaces
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/debug/debug.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>
#include <unisim/service/tee/memory_access_reporting/tee.hh>
#include <unisim/service/telnet/telnet.hh>
#include <unisim/service/os/linux_os/linux.hh>
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
using unisim::service::debug::debugger::Debugger;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::profiling::addr_profiler::Profiler;
using unisim::service::power::CachePowerEstimator;
using unisim::service::telnet::Telnet;
using unisim::service::os::linux_os::Linux;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Variable;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Object;

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

template <class CONFIG>
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
	typedef typename CONFIG::CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	typedef typename CONFIG::CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<CONFIG::CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CONFIG::CPU_CONFIG::FSB_BURST_SIZE / CONFIG::CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, CONFIG::DEBUG_INFORMATION> RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<CONFIG::CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CONFIG::CPU_CONFIG::FSB_BURST_SIZE / CONFIG::CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, CONFIG::DEBUG_INFORMATION> BRAM;
	typedef unisim::component::tlm2::processor::powerpc::ppc440::CPU<typename CONFIG::CPU_CONFIG> CPU;
	typedef unisim::component::tlm2::processor::powerpc::ppc440::CPU<typename CONFIG::LINUX_OS_CPU_CONFIG> LINUX_OS_CPU;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<typename CONFIG::MPLB_CONFIG> MPLB;
	typedef unisim::component::tlm2::interrupt::xilinx::xps_intc::XPS_IntC<typename CONFIG::INTC_CONFIG> INTC;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::XPS_Timer<typename CONFIG::TIMER_CONFIG> TIMER;
	typedef unisim::component::tlm2::memory::flash::am29::AM29<typename CONFIG::AM29_CONFIG, 32 * unisim::component::cxx::memory::flash::am29::M, 2, CONFIG::CPU_CONFIG::FSB_WIDTH * 8> FLASH;
	typedef unisim::component::tlm2::interconnect::xilinx::dcr_controller::DCRController<typename CONFIG::DCR_CONTROLLER_CONFIG> DCR_CONTROLLER;
	typedef unisim::component::tlm2::interconnect::xilinx::crossbar::Crossbar<typename CONFIG::CROSSBAR_CONFIG> CROSSBAR;
	typedef unisim::component::tlm2::interconnect::xilinx::mci::MCI<typename CONFIG::MCI_CONFIG> MCI;
	typedef unisim::component::tlm2::com::xilinx::xps_uart_lite::XPS_UARTLite<typename CONFIG::UART_LITE_CONFIG> UART_LITE;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<typename CONFIG::GPIO_DIP_SWITCHES_8BIT_CONFIG> GPIO_DIP_SWITCHES_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<typename CONFIG::GPIO_LEDS_8BIT_CONFIG> GPIO_LEDS_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<typename CONFIG::GPIO_5_LEDS_POSITIONS_CONFIG> GPIO_5_LEDS_POSITIONS;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<typename CONFIG::GPIO_PUSH_BUTTONS_5BIT_CONFIG> GPIO_PUSH_BUTTONS_5BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_Switches<8> DIP_SWITCHES_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_LEDs<8> LEDS_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_LEDs<5> _5_LEDS_POSITIONS;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_Switches<5> PUSH_BUTTONS_5BIT;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::PWMProtocolTypes> PWM_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutProtocolTypes> GENERATE_OUT_STUB;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerStub CAPTURE_TRIGGER_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<CONFIG::CPU_CONFIG::FSB_WIDTH * 8> SPLB0_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<CONFIG::CPU_CONFIG::FSB_WIDTH * 8> SPLB1_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<4> MASTER1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> APU_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC0_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC2_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC3_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> EXTERNAL_SLAVE_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::com::xilinx::xps_gpio::GPIOProtocolTypes> GPIO_OUTPUT_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::com::xilinx::xps_gpio::GPIOProtocolTypes> GPIO_INPUT_STUB;

	typedef unisim::kernel::tlm2::TargetStub<4> DCR_SLAVE_STUB;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<typename CONFIG::MEMORY_ROUTER_CONFIG> MEMORY_ROUTER;

	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	LINUX_OS_CPU *linux_os_cpu;
	//  - RAM
	RAM *ram;
	//  - BRAM
	BRAM *bram;
	// - IRQ stubs
	IRQ_STUB *input_interrupt_stub[CONFIG::INTC_CONFIG::C_NUM_INTR_INPUTS];
	IRQ_STUB *critical_input_interrupt_stub;
	IRQ_STUB *external_input_interrupt_stub;
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
	CAPTURE_TRIGGER_STUB *capture_trigger_stub[CONFIG::TIMER_CONFIG::NUM_TIMERS];
	// - GenerateOutStub
	GENERATE_OUT_STUB *generate_out_stub[CONFIG::TIMER_CONFIG::NUM_TIMERS];
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
	DCR_SLAVE_STUB *dcr_slave_stub; // used in combination with Linux loader and ABI translator
	// - Memory router (used in combination with Linux loader and ABI translator)
	MEMORY_ROUTER *memory_router;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Multiformat loader
	MultiFormatLoader<CPU_ADDRESS_TYPE> *loader;
	//  - Linux loader and Linux ABI translator
	Linux<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE> *linux_os;
	//  - Debugger
	Debugger<CPU_ADDRESS_TYPE> *debugger;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;
	//  - profiler
	Profiler<CPU_ADDRESS_TYPE> *profiler;
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
	//  - Tee Memory Access Reporting
	unisim::service::tee::memory_access_reporting::Tee<CPU_ADDRESS_TYPE> *tee_memory_access_reporting;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool estimate_power;
	bool enable_telnet;
	bool enable_linux_os;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_estimate_power;
	Parameter<bool> param_enable_telnet;
	Parameter<bool> param_enable_linux_os;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
#ifdef WIN32
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
	static void SigIntHandler(int signum);
#endif
};

#endif // __VIRTEX5FXT_SIMULATOR_HH__
