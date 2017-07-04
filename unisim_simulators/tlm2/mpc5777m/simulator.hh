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

// Class definition of kernel, services and interfaces
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/util/backtrace/backtrace.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/tee/memory_access_reporting/tee.hh>
#include <unisim/service/telnet/telnet.hh>
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

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::loader::multiformat_loader::MultiFormatLoader;
using unisim::service::debug::debugger::Debugger;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::profiling::addr_profiler::Profiler;
using unisim::service::telnet::Telnet;
using unisim::service::os::linux_os::Linux;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Variable;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Object;

static const bool DEBUG_ENABLE = true;

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

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

	// Front Side Bus template parameters
	static const unsigned int FSB_WIDTH = 8;
	static const unsigned int FSB_BURST_SIZE = 32;
	typedef uint32_t CPU_ADDRESS_TYPE;
	typedef uint32_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;

	struct INTERCONNECT_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef FSB_ADDRESS_TYPE ADDRESS;
		static const unsigned int INPUT_SOCKETS = 2;
		static const unsigned int OUTPUT_SOCKETS = 8;
		static const unsigned int MAX_NUM_MAPPINGS = 13;
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
	
	struct STM_2_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 64; // FIXME: INTC will be on PBRIDGE which is 32-bit width
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

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

//	typedef unisim::component::tlm2::memory::ram::Memory<FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> STANDBY_RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> SYSTEM_RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> FLASH;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z710n3::CPU Main_Core_0;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z710n3::CPU Main_Core_1;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU Peripheral_Core_2;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<INTERCONNECT_CONFIG> INTERCONNECT;
	typedef unisim::component::tlm2::interrupt::freescale::mpc57xx::intc::INTC<INTC_0_CONFIG> INTC_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_2_CONFIG> STM_2;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_2_CONFIG> SWT_2;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_3_CONFIG> SWT_3;

	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - PowerPC processor
	Peripheral_Core_2 *peripheral_core_2;
	//  - Standby RAM
	STANDBY_RAM *standby_ram;
	//  - System RAM
	SYSTEM_RAM *system_ram;
	//  - FLASH
	FLASH *flash;
	//  - Interconnect
	INTERCONNECT *interconnect;
	//  - Interrupt Controller
	INTC_0 *intc_0;
	//  - System Timer Module
	STM_2 *stm_2;
	//  - Software Watchdog Timers
	SWT_2 *swt_2;
	SWT_3 *swt_3;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Multiformat loader
	MultiFormatLoader<CPU_ADDRESS_TYPE> *loader;
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
	//  - Tee Memory Access Reporting
	unisim::service::tee::memory_access_reporting::Tee<CPU_ADDRESS_TYPE> *tee_memory_access_reporting;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
#ifdef WIN32
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
	static void SigIntHandler(int signum);
#endif
	
	void ResetProcess();
	
	void InterruptSource(unsigned int irq_num, const std::string& source = std::string());
};

#endif // __MPC5777M_SIMULATOR_HH__
