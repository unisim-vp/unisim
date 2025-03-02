/*
 *  Copyright (c) 2018-2022,
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

#include <debugger.hh>
#include <arch.hh>
#include <linuxsystem.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/service/debug/inline_debugger/inline_debugger.tcc>
#include <unisim/service/debug/gdb_server/gdb_server.tcc>

Debugger::Debugger(char const* name, unisim::kernel::Object* parent, Arch& arch, LinuxOS& linux_os)
  : unisim::kernel::Object(name, parent)
  , debug_hub("debug-hub", this)
  , gdb_server()
  , inline_debugger()
  , enable_gdb_server()
  , enable_inline_debugger()
  , param_enable_gdb_server("enable-gdb-server", this, enable_gdb_server, "enable the gdb-server debug sessions")
  , param_enable_inline_debugger("enable-inline-debugger", this, enable_inline_debugger, "enable the inline-debugger debug sessions")
{
  // DebHub <-> ARCH connections
  arch.debug_yielding_import                           >> *debug_hub.debug_yielding_export[0];
  //arch.trap_reporting_import                           >> *debug_hub.trap_reporting_export[0];
  arch.memory_access_reporting_import                  >> *debug_hub.memory_access_reporting_export[0];
  debug_hub.disasm_import                         [0]  -> Bind(arch);
  debug_hub.memory_import                         [0]  -> Bind(arch);
  debug_hub.registers_import                      [0]  -> Bind(arch);
  debug_hub.memory_access_reporting_control_import[0]  -> Bind(arch);

  // Debug_Hub <-> Loader connections
  debug_hub.blob_import                                . Bind(linux_os);

  unsigned front_end = 0;

  if (enable_inline_debugger)
    {
      inline_debugger = std::make_unique<InlineDebugger>("inline-debugger",this);
      unsigned i = front_end++;
      *debug_hub.debug_event_listener_import[i]     >> inline_debugger->debug_event_listener_export;
      *debug_hub.debug_yielding_import      [i]     >> inline_debugger->debug_yielding_export;
      inline_debugger->debug_yielding_request_import >> *debug_hub.debug_yielding_request_export[i];
      inline_debugger->debug_event_trigger_import    >> *debug_hub.debug_event_trigger_export   [i];
      inline_debugger->disasm_import                 >> *debug_hub.disasm_export                [i];
      inline_debugger->memory_import                 >> *debug_hub.memory_export                [i];
      inline_debugger->registers_import              >> *debug_hub.registers_export             [i];
      inline_debugger->stmt_lookup_import            >> *debug_hub.stmt_lookup_export           [i];
      inline_debugger->symbol_table_lookup_import    >> *debug_hub.symbol_table_lookup_export   [i];
      inline_debugger->stack_frame_import              >> *debug_hub.stack_frame_export             [i];
      inline_debugger->debug_info_loading_import     >> *debug_hub.debug_info_loading_export    [i];
      inline_debugger->data_object_lookup_import     >> *debug_hub.data_object_lookup_export    [i];
      inline_debugger->subprogram_lookup_import      >> *debug_hub.subprogram_lookup_export     [i];
    }
  else
    {
      gdb_server = std::make_unique<GDBServer>("gdb-server",this);
      unsigned i = front_end++;
      *debug_hub.debug_event_listener_import[i] >> gdb_server->debug_event_listener_export;
      *debug_hub.debug_yielding_import      [i] >> gdb_server->debug_yielding_export;
      gdb_server->debug_yielding_request_import >> *debug_hub.debug_yielding_request_export[i];
      gdb_server->debug_selecting_import        >> *debug_hub.debug_selecting_export       [i];
      gdb_server->debug_event_trigger_import    >> *debug_hub.debug_event_trigger_export   [i];
      gdb_server->memory_import                 >> *debug_hub.memory_export                [i];
      gdb_server->registers_import              >> *debug_hub.registers_export             [i];
    }
}

