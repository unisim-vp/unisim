/*
 *  Copyright (c) 2018-2021,
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

#ifndef __ARM64VP_DEBUGGER_HH__
#define __ARM64VP_DEBUGGER_HH__

#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/util/identifier/identifier.hh>
#include <iosfwd>
#include <memory>
#include <inttypes.h>

struct Arch;
struct LinuxOS;

struct Debugger
  : public unisim::kernel::Object
{
  struct DEBUGGER_CONFIG
  {
    typedef uint64_t ADDRESS;
    typedef uint64_t TIME_TYPE;
    static const unsigned int NUM_PROCESSORS = 1;
    /* gdb_server, inline_debugger and/or monitor */
    static const unsigned int MAX_FRONT_ENDS = 2;
  };

  typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DebugHub;
  typedef unisim::service::debug::gdb_server::GDBServer<uint64_t> GDBServer;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<uint64_t> InlineDebugger;

  Debugger(char const*, unisim::kernel::Object*, Arch&, LinuxOS&);

  DebugHub debug_hub;
  std::unique_ptr<GDBServer> gdb_server;
  std::unique_ptr<InlineDebugger> inline_debugger;
  bool enable_gdb_server;
  bool enable_inline_debugger;
  unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
  unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;
};

#endif // __ARM64VP_LINUX_DEBUGGER_HH__
