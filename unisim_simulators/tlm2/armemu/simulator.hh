/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>

#include "unisim/kernel/service/service.hh"

#include "unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.hh"
#include "unisim/component/tlm2/memory/ram/memory.hh"
#include "unisim/component/tlm2/interrupt/master_stub.hh"

#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/loader/elf_loader/elf_loader.hh"
#include "unisim/service/loader/elf_loader/elf_loader.tcc"
#include "unisim/service/loader/linux_loader/linux_loader.hh"
#include "unisim/service/os/linux_os/linux_os_32/linux_os_32.hh"
#include "unisim/service/trap_handler/trap_handler.hh"
#include "config.hh"
#ifdef SIM_GDB_SERVER_SUPPORT
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
#include "unisim/service/debug/sim_debugger/sim_debugger.hh"
#endif // SIM_SIM_DEBUGGER_SUPPORT
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
#include "unisim/service/power/cache_power_estimator.hh"
#endif // SIM_POWER_ESTIMATOR_SUPPORT

class Simulator
	: public unisim::kernel::service::Simulator
#ifdef SIM_LIBRARY
	, public unisim::kernel::service::VariableBase::Notifiable
	, public unisim::service::trap_handler::ExternalTrapHandlerInterface
#endif // SIM_LIBRARY
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	int Run();
	int Run(double time, sc_time_unit unit);
	bool IsRunning() const;
	bool SimulationStarted() const;
	bool SimulationFinished() const;
#ifdef SIM_LIBRARY
	bool AddNotifiable(void *(*notif_function)(const char *), const char *var_name);
	bool SetTrapHandler(void (*function)(void *, unsigned int), void *context);
	unisim::service::debug::DebuggerHandler *GetDebugger(const char *name);
#endif // SIM_LIBRARY
	void Stop();

protected:
private:
	static void DefaultConfiguration(unisim::kernel::service::Simulator *sim);
	typedef unisim::component::tlm2::processor::arm::arm926ejs::ARM926EJS CPU;
	typedef unisim::component::tlm2::interrupt::InterruptMasterStub IRQ_MASTER_STUB;
	typedef unisim::component::tlm2::interrupt::InterruptMasterStub FIQ_MASTER_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, true> MEMORY;
	typedef unisim::service::loader::linux_loader::LinuxLoader<uint64_t> LINUX_LOADER;
	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> ELF32_LOADER;
	typedef unisim::service::os::linux_os::linux_os_32::LinuxOS32 LINUX_OS;
	typedef unisim::service::trap_handler::TrapHandler TRAP_HANDLER;
#ifdef SIM_GDB_SERVER_SUPPORT
	typedef unisim::service::debug::gdb_server::GDBServer<uint64_t> GDB_SERVER;
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	typedef unisim::service::debug::inline_debugger::InlineDebugger<uint64_t> INLINE_DEBUGGER;
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
	typedef unisim::service::debug::sim_debugger::SimDebugger<uint64_t> SIM_DEBUGGER;
#endif // SIM_SIM_DEBUGGER_SUPPORT
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	typedef unisim::service::power::CachePowerEstimator POWER_ESTIMATOR;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	CPU *cpu;
	IRQ_MASTER_STUB *irq_master_stub;
	FIQ_MASTER_STUB *fiq_master_stub;
	MEMORY *memory;
	unisim::service::time::sc_time::ScTime *time;
	unisim::service::time::host_time::HostTime *host_time;
	ELF32_LOADER *elf32_loader;
	LINUX_LOADER *linux_loader;
	LINUX_OS *linux_os;
	TRAP_HANDLER *trap_handler;

	double simulation_spent_time;

#ifdef SIM_GDB_SERVER_SUPPORT
	GDB_SERVER *gdb_server;
	bool enable_gdb_server;
	void EnableGdbServer();
	unisim::kernel::service::Parameter<bool> *param_enable_gdb_server;
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	INLINE_DEBUGGER *inline_debugger;
	bool enable_inline_debugger;
	void EnableInlineDebugger();
	unisim::kernel::service::Parameter<bool> *param_enable_inline_debugger;
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
	SIM_DEBUGGER *sim_debugger;
	bool enable_sim_debugger;
	void EnableSimDebugger();
	unisim::kernel::service::Parameter<bool> *param_enable_sim_debugger;
#endif // SIM_SIM_DEBUGGER_SUPPORT
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	POWER_ESTIMATOR *il1_power_estimator;
	POWER_ESTIMATOR *dl1_power_estimator;
	bool enable_power_estimation;
	unisim::kernel::service::Parameter<bool> param_enable_power_estimation;
	unisim::kernel::service::Formula<double> *formula_caches_total_dynamic_energy;
	unisim::kernel::service::Formula<double> *formula_caches_total_dynamic_power;
	unisim::kernel::service::Formula<double> *formula_caches_total_leakage_power;
	unisim::kernel::service::Formula<double> *formula_caches_total_power;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

#ifdef SIM_LIBRARY
	std::map < std::string, std::vector<void * (*)(const char *)> * > notif_list;
	virtual void VariableNotify(const char *name);
	void *trap_handler_context;
	void (*trap_handler_function)(void *, unsigned int);
	virtual void ExternalTrap(unsigned int id);
#endif // SIM_LIBRARY
};

#endif /* SIMULATOR_HH_ */
