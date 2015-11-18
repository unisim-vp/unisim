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

#include "unisim/kernel/service/service.hh"
#include "unisim/component/tlm2/processor/arm/armemu/armemu.hh"
#include "unisim/component/tlm2/memory/ram/memory.hh"
#include "unisim/util/likely/likely.hh"
#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/os/linux_os/linux.hh"
#include "unisim/service/trap_handler/trap_handler.hh"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/debug/debugger/debugger.hh"
#include "unisim/service/power/cache_power_estimator.hh"
#include "unisim/service/profiling/addr_profiler/profiler.hh"
#include "unisim/service/tee/memory_access_reporting/tee.hh"

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else
#include <signal.h>
#endif

class Simulator
  : public unisim::kernel::service::Simulator
{
 public:
  Simulator(int argc, char **argv);
  virtual ~Simulator();
  int Run();
  int Run(double time, sc_time_unit unit);
  bool IsRunning() const;
  bool SimulationStarted() const;
  bool SimulationFinished() const;
  virtual unisim::kernel::service::Simulator::SetupStatus Setup();
  virtual void Stop(unisim::kernel::service::Object *object, int exit_status, bool asynchronous = false);
  int GetExitStatus() const;

 protected:
 private:
  static void DefaultConfiguration(unisim::kernel::service::Simulator *sim);
  typedef unisim::component::tlm2::processor::arm::armemu::ARMEMU CPU;
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> MEMORY;
  typedef unisim::service::os::linux_os::Linux<uint32_t, uint32_t> LINUX_OS;

  typedef unisim::service::debug::gdb_server::GDBServer<uint32_t> GDB_SERVER;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<uint32_t> INLINE_DEBUGGER;
  typedef unisim::service::power::CachePowerEstimator POWER_ESTIMATOR;
  typedef unisim::service::debug::debugger::Debugger<uint32_t> DEBUGGER;
  typedef unisim::service::profiling::addr_profiler::Profiler<uint32_t> PROFILER;
  typedef unisim::service::tee::memory_access_reporting::Tee<uint32_t> TEE_MEMORY_ACCESS_REPORTING;

  CPU *cpu;
  MEMORY *memory;
  unisim::service::time::sc_time::ScTime *time;
  unisim::service::time::host_time::HostTime *host_time;
  LINUX_OS *linux_os;
  TEE_MEMORY_ACCESS_REPORTING *tee_memory_access_reporting;

  sc_signal<bool>              nirq_signal;
  sc_signal<bool>              nfiq_signal;
  
  double simulation_spent_time;

  GDB_SERVER *gdb_server;
  INLINE_DEBUGGER *inline_debugger;
  DEBUGGER *debugger;
  PROFILER *profiler;
  bool enable_gdb_server;
  unisim::kernel::service::Parameter<bool> *param_enable_gdb_server;
  bool enable_inline_debugger;
  unisim::kernel::service::Parameter<bool> *param_enable_inline_debugger;

  POWER_ESTIMATOR *il1_power_estimator;
  POWER_ESTIMATOR *dl1_power_estimator;
  bool enable_power_estimation;
  unisim::kernel::service::Parameter<bool> param_enable_power_estimation;
  unisim::kernel::service::Formula<double> *formula_caches_total_dynamic_energy;
  unisim::kernel::service::Formula<double> *formula_caches_total_dynamic_power;
  unisim::kernel::service::Formula<double> *formula_caches_total_leakage_power;
  unisim::kernel::service::Formula<double> *formula_caches_total_power;
  int exit_status;
#ifdef WIN32
  static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
  static void SigIntHandler(int signum);
#endif
};

#endif /* SIMULATOR_HH_ */
