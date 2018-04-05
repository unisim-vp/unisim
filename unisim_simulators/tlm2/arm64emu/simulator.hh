/*
 *  Copyright (c) 2016,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <getopt.h>
#include <stdlib.h>

#include <unisim/kernel/service/service.hh>
#include <unisim/component/tlm2/processor/arm/cortex_a53/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/os/linux_os/arm_linux64.hh>
#include <unisim/service/trap_handler/trap_handler.hh>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/debugger/debugger.hh>

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else
#include <signal.h>
#endif

struct Simulator
  : public unisim::kernel::service::Simulator
{
  Simulator(int argc, char **argv);
  virtual ~Simulator();
  int Run();
  int Run(double time, sc_core::sc_time_unit unit);
  bool IsRunning() const;
  bool SimulationStarted() const;
  bool SimulationFinished() const;
  virtual unisim::kernel::service::Simulator::SetupStatus Setup();
  virtual void Stop(unisim::kernel::service::Object *object, int exit_status, bool asynchronous = false);
  int GetExitStatus() const;

 protected:
 private:
  static void DefaultConfiguration(unisim::kernel::service::Simulator *sim);
  typedef unisim::component::tlm2::processor::arm::cortex_a53::CPU CPU;
  typedef unisim::component::tlm2::memory::ram::Memory<64, uint64_t, 8, 1024 * 1024, true> MEMORY;
  
  struct DEBUGGER_CONFIG
  {
    typedef uint64_t ADDRESS;
    static const unsigned int NUM_PROCESSORS = 1;
    /* gdb_server, inline_debugger */
    static const unsigned int MAX_FRONT_ENDS = 2;
  };
  
  typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
  typedef unisim::service::debug::gdb_server::GDBServer<uint64_t> GDB_SERVER;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<uint64_t> INLINE_DEBUGGER;

  CPU                                        cpu;
  MEMORY                                     memory;
  unisim::service::time::sc_time::ScTime     time;
  unisim::service::time::host_time::HostTime host_time;
  unisim::service::os::linux_os::ArmLinux64  linux_os;

  double                                     simulation_spent_time;

  DEBUGGER*                                  debugger;
  GDB_SERVER*                                gdb_server;
  INLINE_DEBUGGER*                           inline_debugger;
  
  bool                                       enable_gdb_server;
  unisim::kernel::service::Parameter<bool>   param_enable_gdb_server;
  bool                                       enable_inline_debugger;
  unisim::kernel::service::Parameter<bool>   param_enable_inline_debugger;
  
  int exit_status;
#ifdef WIN32
  static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
  static void SigIntHandler(int signum);
#endif
};

#endif /* SIMULATOR_HH_ */
