/*
 *  Copyright (c) 2018,
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

//unisim/util/debug/coff_symtab/coff_symtab.tcc

#include <debugger.hh>
#include <arch.hh>
#include <linuxsystem.hh>
#include <unisim/util/loader/coff_loader/coff_loader.hh>
#include <unisim/util/loader/coff_loader/coff_loader.tcc>
#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/profile.tcc>
#include <unisim/util/debug/coff_symtab/coff_symtab.hh>
#include <unisim/util/debug/coff_symtab/coff_symtab.tcc>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.tcc>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/util/simfloat/floating.hh>
#include <unisim/util/simfloat/floating.tcc>

Debugger::Debugger(Arch& cpu, LinuxOS& linux_os)
  : debugger_router("debugger_router", 0)
  , inline_debugger("inline_debugger", 0)
{
  // Debugger <-> CPU connections
  cpu.debug_yielding_import                                  >> *debugger_router.debug_yielding_export[0];
  cpu.trap_reporting_import                                  >> *debugger_router.trap_reporting_export[0];
  cpu.memory_access_reporting_import                         >> *debugger_router.memory_access_reporting_export[0];
  *debugger_router.disasm_import[0]                          >> cpu.disasm_export;
  *debugger_router.memory_import[0]                          >> cpu.memory_export;
  *debugger_router.registers_import[0]                       >> cpu.registers_export;
  *debugger_router.memory_access_reporting_control_import[0] >> cpu.memory_access_reporting_control_export;
  
  // Debugger_Router <-> Loader connections
  debugger_router.blob_import                                >> linux_os.blob_export;
  
  // inline-debugger_router <-> debugger_router connections
  *debugger_router.debug_event_listener_import[0]            >> inline_debugger.debug_event_listener_export;
  *debugger_router.debug_yielding_import[0]                  >> inline_debugger.debug_yielding_export;
  inline_debugger.debug_yielding_request_import              >> *debugger_router.debug_yielding_request_export[0];
  inline_debugger.debug_event_trigger_import                 >> *debugger_router.debug_event_trigger_export[0];
  inline_debugger.disasm_import                              >> *debugger_router.disasm_export[0];
  inline_debugger.memory_import                              >> *debugger_router.memory_export[0];
  inline_debugger.registers_import                           >> *debugger_router.registers_export[0];
  inline_debugger.stmt_lookup_import                         >> *debugger_router.stmt_lookup_export[0];
  inline_debugger.symbol_table_lookup_import                 >> *debugger_router.symbol_table_lookup_export[0];
  inline_debugger.backtrace_import                           >> *debugger_router.backtrace_export[0];
  inline_debugger.debug_info_loading_import                  >> *debugger_router.debug_info_loading_export[0];
  inline_debugger.data_object_lookup_import                  >> *debugger_router.data_object_lookup_export[0];
  inline_debugger.subprogram_lookup_import                   >> *debugger_router.subprogram_lookup_export[0];
}

Debugger::~Debugger()
{
}
