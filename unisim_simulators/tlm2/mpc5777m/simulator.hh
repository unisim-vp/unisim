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

#ifndef __MPC5777M_SIMULATOR_HH__
#define __MPC5777M_SIMULATOR_HH__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Class definition of components
#include <unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z710n3/cpu.hh>
#include <unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh>
#include <unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.hh>
#include <unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.hh>
#include <unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.hh>
#include <unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.hh>
#include <unisim/component/tlm2/com/serial_terminal/serial_terminal.hh>
#include <unisim/component/tlm2/dma/freescale/mpc57xx/dmamux/dmamux.hh>

// Class definition of kernel, services and interfaces
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/util/backtrace/backtrace.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/telnet/telnet.hh>
#include <unisim/service/netcat/netcat.hh>
#include <unisim/service/os/linux_os/powerpc_linux32.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>

// Host machine standard headers
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else
#include <signal.h>
#endif

static const bool DEBUG_ENABLE = true;

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

enum SerialTerminalProtocol
{
	SERIAL_TERMINAL_PROTOCOL_TELNET,
	SERIAL_TERMINAL_PROTOCOL_NETCAT
};

class Simulator : public unisim::kernel::tlm2::Simulator
{
public:
	Simulator(const sc_core::sc_module_name& name, int argc, char **argv);
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

	static const unsigned int NUM_PROCESSORS = 3;
	static const unsigned int MAX_FRONT_ENDS = NUM_PROCESSORS // for gdb-server
	                                         + NUM_PROCESSORS // for inline-debugger
	                                         + NUM_PROCESSORS // for profiler
	                                         ;

	static const unsigned int NUM_DMA_CHANNELS  = 128;
	static const unsigned int NUM_DMA_TRIGGERS  = 8;
	static const unsigned int NUM_DMA_ALWAYS_ON = 64;
	
	// Front Side Bus template parameters
	static const unsigned int FSB_WIDTH = 8;
	static const unsigned int FSB_BURST_SIZE = 32;
	typedef uint32_t CPU_ADDRESS_TYPE;
	typedef uint32_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;
	
	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = Simulator::NUM_PROCESSORS;
		static const unsigned int MAX_FRONT_ENDS = Simulator::MAX_FRONT_ENDS;
	};

	struct XBAR_0_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef FSB_ADDRESS_TYPE ADDRESS;
		static const unsigned int INPUT_SOCKETS = 6;
		static const unsigned int OUTPUT_SOCKETS = 8;
		static const unsigned int MAX_NUM_MAPPINGS = 11;
		static const unsigned int BUSWIDTH = 64;
		static const bool VERBOSE = DEBUG_ENABLE;
	};
	
	struct XBAR_1_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef FSB_ADDRESS_TYPE ADDRESS;
		static const unsigned int INPUT_SOCKETS = 4;
		static const unsigned int OUTPUT_SOCKETS = 4;
		static const unsigned int MAX_NUM_MAPPINGS = 12;
		static const unsigned int BUSWIDTH = 64;
		static const bool VERBOSE = DEBUG_ENABLE;
	};
	
	struct PBRIDGE_A_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef FSB_ADDRESS_TYPE ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 24;
		static const unsigned int MAX_NUM_MAPPINGS = 24;
		static const unsigned int BUSWIDTH = 64;
		static const bool VERBOSE = DEBUG_ENABLE;
	};
	
	struct PBRIDGE_B_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef FSB_ADDRESS_TYPE ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 2;
		static const unsigned int MAX_NUM_MAPPINGS = 2;
		static const unsigned int BUSWIDTH = 64;
		static const bool VERBOSE = DEBUG_ENABLE;
	};
	
	struct INTC_0_CONFIG
	{
		static const unsigned int NUM_PROCESSORS = 3;
		static const unsigned int NUM_HW_IRQS = 965 - 32;
		static const unsigned int BUSWIDTH = 64; // FIXME: INTC will be on PBRIDGE which is 32-bit width
		static const unsigned int VOFFSET_WIDTH = 14;
	};
	
	struct STM_0_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 64; // FIXME: INTC will be on PBRIDGE which is 32-bit width
	};

	struct STM_1_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 64; // FIXME: INTC will be on PBRIDGE which is 32-bit width
	};

	struct STM_2_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 64; // FIXME: INTC will be on PBRIDGE which is 32-bit width
	};

	struct SWT_0_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 64; // FIXME: SWT will be on PBRIDGE which is 32-bit width
	};

	struct SWT_1_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 64; // FIXME: SWT will be on PBRIDGE which is 32-bit width
	};

	struct SWT_2_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 64; // FIXME: SWT will be on PBRIDGE which is 32-bit width
	};
	
	struct SWT_3_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 64; // FIXME: SWT will be on PBRIDGE which is 32-bit width
	};
	
	struct PIT_0_CONFIG
	{
		static const unsigned int MAX_CHANNELS = 8;
		static const unsigned int NUM_CHANNELS = 8;
		static const bool HAS_RTI_SUPPORT = true;
		static const bool HAS_DMA_SUPPORT = true;
		static const bool HAS_64_BIT_TIMER_SUPPORT = false;
		static const unsigned int BUSWIDTH = 64; // FIXME: PIT will be on PBRIDGE which is 32-bit width
	};

	struct PIT_1_CONFIG
	{
		static const unsigned int MAX_CHANNELS = 8;
		static const unsigned int NUM_CHANNELS = 2;
		static const bool HAS_RTI_SUPPORT = false;
		static const bool HAS_DMA_SUPPORT = false;
		static const bool HAS_64_BIT_TIMER_SUPPORT = true;
		static const unsigned int BUSWIDTH = 64; // FIXME: PIT will be on PBRIDGE which is 32-bit width
	};
	
	struct LINFlexD_0_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 16;
		static const bool GENERIC_SLAVE = true;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = true;
		static const unsigned int BUSWIDTH = 64; // FIXME: LINFlexD will be on PBRIDGE which is 32-bit width
	};

	struct LINFlexD_1_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 64; // FIXME: LINFlexD will be on PBRIDGE which is 32-bit width
	};

	struct LINFlexD_2_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 64; // FIXME: LINFlexD will be on PBRIDGE which is 32-bit width
	};

	struct LINFlexD_14_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 64; // FIXME: LINFlexD will be on PBRIDGE which is 32-bit width
	};

	struct LINFlexD_15_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 64; // FIXME: LINFlexD will be on PBRIDGE which is 32-bit width
	};

	struct LINFlexD_16_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 64; // FIXME: LINFlexD will be on PBRIDGE which is 32-bit width
	};

	struct DMAMUX_0_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 0 - 7
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 20
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};
	
	struct DMAMUX_1_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 8 - 15
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 57
		static const unsigned int NUM_DMA_ALWAYS_ON = 5;  // Always 59 - 63
		static const unsigned int NUM_DMA_TRIGGERS  = 5;  // PIT_0 Triggers 0 - 4
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};

	struct DMAMUX_2_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 16 - 23
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 48
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 1;  // PIT_0 Trigger 5
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};

	struct DMAMUX_3_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 24 - 31
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 49
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};
	
	struct DMAMUX_4_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 32 - 47
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 41
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 1;  // PIT_0 Trigger 6
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};

	struct DMAMUX_5_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 48 - 63
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 41
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 1;  // PIT_0 Trigger 7
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};

	struct DMAMUX_6_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 64 - 79
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 47
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};

	struct DMAMUX_7_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 80 - 95
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 50
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};
	
	struct DMAMUX_8_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 96 - 111
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 45
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};
	
	struct DMAMUX_9_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 112 - 127
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 43
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 64; // FIXME: DMAMUX will be on PBRIDGE which is 32-bit width
	};

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> SYSTEM_SRAM;
	typedef unisim::component::tlm2::memory::ram::Memory<FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> FLASH;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z710n3::CPU Main_Core_0;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z710n3::CPU Main_Core_1;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU Peripheral_Core_2;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<XBAR_0_CONFIG> XBAR_0;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<XBAR_1_CONFIG> XBAR_1;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<PBRIDGE_A_CONFIG> PBRIDGE_A;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<PBRIDGE_B_CONFIG> PBRIDGE_B;
	typedef unisim::component::tlm2::interrupt::freescale::mpc57xx::intc::INTC<INTC_0_CONFIG> INTC_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_0_CONFIG> STM_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_1_CONFIG> STM_1;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_2_CONFIG> STM_2;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_0_CONFIG> SWT_0;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_1_CONFIG> SWT_1;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_2_CONFIG> SWT_2;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_3_CONFIG> SWT_3;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::pit::PIT<PIT_0_CONFIG> PIT_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::pit::PIT<PIT_1_CONFIG> PIT_1;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_0_CONFIG> LINFlexD_0;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_1_CONFIG> LINFlexD_1;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_2_CONFIG> LINFlexD_2;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_14_CONFIG> LINFlexD_14;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_15_CONFIG> LINFlexD_15;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_16_CONFIG> LINFlexD_16;
	typedef unisim::component::tlm2::com::serial_terminal::SerialTerminal SERIAL_TERMINAL;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_0_CONFIG> DMAMUX_0;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_1_CONFIG> DMAMUX_1;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_2_CONFIG> DMAMUX_2;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_3_CONFIG> DMAMUX_3;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_4_CONFIG> DMAMUX_4;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_5_CONFIG> DMAMUX_5;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_6_CONFIG> DMAMUX_6;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_7_CONFIG> DMAMUX_7;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_8_CONFIG> DMAMUX_8;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_9_CONFIG> DMAMUX_9;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_0_TX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_0_RX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_1_TX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_1_RX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_2_TX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_2_RX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_14_TX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_14_RX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_15_TX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_15_RX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_16_TX;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_16_RX;
	typedef unisim::kernel::tlm2::TargetStub<64> EBI_STUB;
	typedef unisim::kernel::tlm2::TargetStub<64> FLASH_PORT1_STUB;
	typedef unisim::kernel::tlm2::TargetStub<64> XBAR_0_S6_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<64> XBAR_1_M1_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<64> XBAR_1_M2_STUB;

	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================

	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_ADDRESS_TYPE> INLINE_DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_ADDRESS_TYPE> GDB_SERVER;
	typedef unisim::service::debug::profiler::Profiler<CPU_ADDRESS_TYPE> PROFILER;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_ADDRESS_TYPE> LOADER;
	typedef unisim::service::telnet::Telnet TELNET;
	typedef unisim::service::netcat::Netcat NETCAT;
	
	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - PowerPC cores
	Main_Core_0 *main_core_0;
	Main_Core_1 *main_core_1;
	Peripheral_Core_2 *peripheral_core_2;
	//  - System SRAM
	SYSTEM_SRAM *system_sram;
	//  - FLASH
	FLASH *flash;
	//  - Crossbars
	XBAR_0 *xbar_0;
	XBAR_1 *xbar_1;
	//  - Peripheral Bridges
	PBRIDGE_A *pbridge_a;
	PBRIDGE_B *pbridge_b;
	//  - Interrupt Controller
	INTC_0 *intc_0;
	//  - System Timer Modules
	STM_0 *stm_0;
	STM_1 *stm_1;
	STM_2 *stm_2;
	//  - Software Watchdog Timers
	SWT_0 *swt_0;
	SWT_1 *swt_1;
	SWT_2 *swt_2;
	SWT_3 *swt_3;
	//  - Periodic Interrupt Timers
	PIT_0 *pit_0;
	PIT_1 *pit_1;
	//  - LINFlexD
	LINFlexD_0 *linflexd_0;
	LINFlexD_1 *linflexd_1;
	LINFlexD_2 *linflexd_2;
	LINFlexD_14 *linflexd_14;
	LINFlexD_15 *linflexd_15;
	LINFlexD_16 *linflexd_16;
	//  - LINFlexD serial buses
	LINFlexD_0_TX *linflexd_0_tx;
	LINFlexD_0_RX *linflexd_0_rx;
	LINFlexD_1_TX *linflexd_1_tx;
	LINFlexD_1_RX *linflexd_1_rx;
	LINFlexD_2_TX *linflexd_2_tx;
	LINFlexD_2_RX *linflexd_2_rx;
	LINFlexD_14_TX *linflexd_14_tx;
	LINFlexD_14_RX *linflexd_14_rx;
	LINFlexD_15_TX *linflexd_15_tx;
	LINFlexD_15_RX *linflexd_15_rx;
	LINFlexD_16_TX *linflexd_16_tx;
	LINFlexD_16_RX *linflexd_16_rx;
	//  - Serial Terminal
	SERIAL_TERMINAL *serial_terminal0;
	SERIAL_TERMINAL *serial_terminal1;
	SERIAL_TERMINAL *serial_terminal2;
	SERIAL_TERMINAL *serial_terminal14;
	SERIAL_TERMINAL *serial_terminal15;
	SERIAL_TERMINAL *serial_terminal16;
	//  - DMAMUX
	DMAMUX_0 *dmamux_0;
	DMAMUX_1 *dmamux_1;
	DMAMUX_2 *dmamux_2;
	DMAMUX_3 *dmamux_3;
	DMAMUX_4 *dmamux_4;
	DMAMUX_5 *dmamux_5;
	DMAMUX_6 *dmamux_6;
	DMAMUX_7 *dmamux_7;
	DMAMUX_8 *dmamux_8;
	DMAMUX_9 *dmamux_9;
	//  - Stubs
	EBI_STUB *ebi_stub;
	FLASH_PORT1_STUB *flash_port1_stub;
	XBAR_0_S6_STUB *xbar_0_s6_stub;
	XBAR_1_M1_STUB *xbar_1_m1_stub;
	XBAR_1_M2_STUB *xbar_1_m2_stub;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Loader
	LOADER *loader;
	//  - Debugger
	DEBUGGER *debugger;
	//  - GDB server
	GDB_SERVER *gdb_server[NUM_PROCESSORS];
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger[NUM_PROCESSORS];
	//  - profiler
	PROFILER *profiler[NUM_PROCESSORS];
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - Telnet
	TELNET *telnet0;
	TELNET *telnet1;
	TELNET *telnet2;
	TELNET *telnet14;
	TELNET *telnet15;
	TELNET *telnet16;
	//  - Netcat
	NETCAT *netcat0;
	NETCAT *netcat1;
	NETCAT *netcat2;
	NETCAT *netcat14;
	NETCAT *netcat15;
	NETCAT *netcat16;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool enable_profiler;
	bool enable_serial_terminal0;
	bool enable_serial_terminal1;
	bool enable_serial_terminal2;
	bool enable_serial_terminal14;
	bool enable_serial_terminal15;
	bool enable_serial_terminal16;
	SerialTerminalProtocol serial_terminal_protocol0;
	SerialTerminalProtocol serial_terminal_protocol1;
	SerialTerminalProtocol serial_terminal_protocol2;
	SerialTerminalProtocol serial_terminal_protocol14;
	SerialTerminalProtocol serial_terminal_protocol15;
	SerialTerminalProtocol serial_terminal_protocol16;
	unisim::kernel::service::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::service::Parameter<bool> param_enable_inline_debugger;
	unisim::kernel::service::Parameter<bool> param_enable_profiler;
	unisim::kernel::service::Parameter<bool> param_enable_serial_terminal0;
	unisim::kernel::service::Parameter<bool> param_enable_serial_terminal1;
	unisim::kernel::service::Parameter<bool> param_enable_serial_terminal2;
	unisim::kernel::service::Parameter<bool> param_enable_serial_terminal14;
	unisim::kernel::service::Parameter<bool> param_enable_serial_terminal15;
	unisim::kernel::service::Parameter<bool> param_enable_serial_terminal16;
	unisim::kernel::service::Parameter<SerialTerminalProtocol> param_serial_terminal_protocol0;
	unisim::kernel::service::Parameter<SerialTerminalProtocol> param_serial_terminal_protocol1;
	unisim::kernel::service::Parameter<SerialTerminalProtocol> param_serial_terminal_protocol2;
	unisim::kernel::service::Parameter<SerialTerminalProtocol> param_serial_terminal_protocol14;
	unisim::kernel::service::Parameter<SerialTerminalProtocol> param_serial_terminal_protocol15;
	unisim::kernel::service::Parameter<SerialTerminalProtocol> param_serial_terminal_protocol16;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
	
	void ResetProcess();
	
	void InterruptSource(unsigned int irq_num, const std::string& source = std::string());
	void DMASource(unsigned int dmamux_num, unsigned int dma_source_num, const std::string& source = std::string());
	
	virtual void SigInt();
};

#endif // __MPC5777M_SIMULATOR_HH__
