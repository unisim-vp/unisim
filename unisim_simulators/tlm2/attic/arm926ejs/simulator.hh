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
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.hh"
#include "unisim/component/tlm2/memory/ram/memory.hh"

#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
// #include "unisim/service/loader/raw_loader/raw_loader.hh"
// #include "unisim/service/loader/elf_loader/elf_loader.hh"
#include "unisim/service/loader/multiformat_loader/multiformat_loader.hh"
#include "unisim/service/trap_handler/trap_handler.hh"
#include "config.h"
#if defined(SIM_GDB_SERVER_SUPPORT) || defined(SIM_INLINE_DEBUGGER_SUPPORT) || defined(SIM_SIM_DEBUGGER_SUPPORT)
#include "unisim/service/debug/debugger/debugger.hh"
#include "unisim/service/profiling/addr_profiler/profiler.hh"
#include "unisim/service/tee/memory_access_reporting/tee.hh"
#endif
#ifdef SIM_GDB_SERVER_SUPPORT
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
#include "unisim/service/debug/sim_debugger/sim_debugger.hh"
#include "unisim/api/debug/debug_api.hh"
#endif // SIM_SIM_DEBUGGER_SUPPORT
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
#include "unisim/service/power/cache_power_estimator.hh"
#endif // SIM_POWER_ESTIMATOR_SUPPORT

class Simulator
	: public unisim::kernel::service::Simulator
#ifdef SIM_LIBRARY
	, public unisim::kernel::service::VariableBaseListener
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
	bool AddVariableBaseListener(void *(*notif_function)(const char *), const char *var_name);
	bool SetTrapHandler(void (*function)(void *, unsigned int), void *context);
	template <typename T> 
		bool 
		HasAPI(const T *api = 0) const
		{
			bool found = false;

			std::list<unisim::kernel::service::Object *> objs;

			GetRootObjects(objs);

			for ( std::list<unisim::kernel::service::Object *>::iterator it = objs.begin();
					it != objs.end();
					it++ )
			{
				found |= ObjectHasAPI<T>(*it);
			}

			return found;
		};

	template <typename T>
		std::list<T *>
		GetAPI(const T *api = 0) const
	{
		std::list<T *> ret;
		bool found = HasAPI<T>();
		if ( !found )
		{
			return ret; // returning empty list
		}

		std::list<unisim::kernel::service::Object *> objs;

		GetRootObjects(objs);

		for ( std::list<unisim::kernel::service::Object *>::iterator it = objs.begin();
				it != objs.end();
				it++ )
		{
			ObjectHasAPI<T>(*it, &ret);
		}

		return ret;
	}

private:
	template <typename T> 
		bool 
		ObjectHasAPI(unisim::kernel::service::Object *obj,
				std::list<T *> *api_obj_list = 0,
				const T *api = 0) const
		{
			bool found = false;

			// std::cerr << "- " << obj->GetName() << " ";
			if ( dynamic_cast<T *>(obj) )
			{
				// std::cerr << "YES";
				found = true;
				if ( api_obj_list )
					api_obj_list->push_back(dynamic_cast<T *>(obj));
			}
			else
			{
				// std::cerr << "NO";
			}
			// std::cerr << std::endl;

			std::list<unisim::kernel::service::Object *> objs = obj->GetLeafs();
			for ( std::list<unisim::kernel::service::Object *>::iterator it = 
					objs.begin();
					it != objs.end();
					it++ )
			{
				found |= ObjectHasAPI<T>(*it);
			}

			return found;
		};
public:
#endif // SIM_LIBRARY
	virtual void Stop(unisim::kernel::service::Object *object, int exit_status, bool asynchronous);

protected:
private:
	static void DefaultConfiguration(unisim::kernel::service::Simulator *sim);
	typedef unisim::component::tlm2::processor::arm::arm926ejs::ARM926EJS CPU;
	typedef unisim::component::tlm2::chipset::arm926ejs_pxp::PXP DEVCHIP;
	typedef unisim::component::tlm2::memory::ram::Memory<32, uint64_t, 8, 1024 * 1024, true> MEMORY;
	typedef unisim::component::tlm2::memory::ram::Memory<32, uint64_t, 8, 1024 * 1024, true> FLASH;
// 	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> ELF32_LOADER;
// 	typedef unisim::service::loader::raw_loader::RawLoader64 RAW_LOADER;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<uint64_t> LOADER;
	typedef unisim::service::trap_handler::TrapHandler TRAP_HANDLER;
#if defined(SIM_GDB_SERVER_SUPPORT) || defined(SIM_INLINE_DEBUGGER_SUPPORT) || defined(SIM_SIM_DEBUGGER_SUPPORT)
	typedef unisim::service::debug::debugger::Debugger<uint64_t> DEBUGGER;
	typedef unisim::service::profiling::addr_profiler::Profiler<uint64_t> PROFILER;
	typedef unisim::service::tee::memory_access_reporting::Tee<uint64_t> TEE_MEMORY_ACCESS_REPORTING;
#endif
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
	DEVCHIP *devchip;
	MEMORY *memory;
	FLASH *nor_flash_2;
	FLASH *nor_flash_1;
	unisim::service::time::sc_time::ScTime *time;
	unisim::service::time::host_time::HostTime *host_time;
// 	ELF32_LOADER *elf32_loader;
// 	RAW_LOADER *raw_kernel_loader;
// 	RAW_LOADER *raw_fs_loader;
	LOADER *loader;
	TRAP_HANDLER *trap_handler;

	double simulation_spent_time;

#if defined(SIM_GDB_SERVER_SUPPORT) || defined(SIM_INLINE_DEBUGGER_SUPPORT) || defined(SIM_SIM_DEBUGGER_SUPPORT)
	DEBUGGER *debugger;
	PROFILER *profiler;
	TEE_MEMORY_ACCESS_REPORTING *tee_memory_access_reporting;
	bool enable_debugger;
	void EnableDebugger();
#endif
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
	POWER_ESTIMATOR *ltlb_power_estimator;
	POWER_ESTIMATOR *tlb_power_estimator;
	bool enable_power_estimation;
	unisim::kernel::service::Parameter<bool> param_enable_power_estimation;
	unisim::kernel::service::Formula<double> *formula_caches_total_dynamic_energy;
	unisim::kernel::service::Formula<double> *formula_caches_total_dynamic_power;
	unisim::kernel::service::Formula<double> *formula_caches_total_leakage_power;
	unisim::kernel::service::Formula<double> *formula_caches_total_power;
	unisim::kernel::service::Formula<double> *formula_tlbs_total_dynamic_energy;
	unisim::kernel::service::Formula<double> *formula_tlbs_total_dynamic_power;
	unisim::kernel::service::Formula<double> *formula_tlbs_total_leakage_power;
	unisim::kernel::service::Formula<double> *formula_tlbs_total_power;
	unisim::kernel::service::Formula<double> *formula_total_dynamic_energy;
	unisim::kernel::service::Formula<double> *formula_total_dynamic_power;
	unisim::kernel::service::Formula<double> *formula_total_leakage_power;
	unisim::kernel::service::Formula<double> *formula_total_power;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

#ifdef SIM_LIBRARY
	std::map < std::string, std::vector<void * (*)(const char *)> * > notif_list;
	virtual void VariableBaseNotify(const unisim::kernel::service::VariableBase *name);
	void *trap_handler_context;
	void (*trap_handler_function)(void *, unsigned int);
	virtual void ExternalTrap(unsigned int id);
#endif // SIM_LIBRARY
};

#endif /* SIMULATOR_HH_ */