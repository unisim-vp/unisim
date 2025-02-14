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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <simulator.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/component/tlm2/memory/ram/memory.tcc>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/service/analysis/cfg/cfg.tcc>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

template class unisim::component::tlm2::memory::ram::Memory<64, uint64_t, 8, 1024 * 1024, true>;

bool debug_enabled;

Simulator::Simulator(int argc, char **argv, const sc_core::sc_module_name& name)
  : unisim::kernel::tlm2::Simulator(name, argc, argv, Simulator::DefaultConfiguration)
  , cpu("cpu", this)
  , memory("memory", this)
  , time("time")
  , host_time("host-time")
  , linux_os("linux-os")
  , simulation_spent_time(0.0)
  , debugger(0)
  , gdb_server(0)
  , inline_debugger(0)
  , profiler(0)
  , http_server(0)
  , instrumenter(0)
  , cfg_builder(0)
  , logger_console_printer(0)
  , logger_text_file_writer(0)
  , logger_http_writer(0)
  , logger_xml_file_writer(0)
  , logger_netstream_writer(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable GDB server.")
  , enable_inline_debugger(false)
  , param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable inline debugger.")
  , enable_profiler(false)
  , param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable profiler.")
  , enable_cfg_builder(false)
  , param_enable_cfg_builder("enable-cfg-builder", 0, enable_cfg_builder, "Enable control flow graph builder.")
{
  param_enable_gdb_server.SetMutable(false);
  param_enable_inline_debugger.SetMutable(false);
  param_enable_profiler.SetMutable(false);
  param_enable_cfg_builder.SetMutable(false);

  logger_console_printer = new LOGGER_CONSOLE_PRINTER();
  logger_text_file_writer = new LOGGER_TEXT_FILE_WRITER();
  logger_http_writer = new LOGGER_HTTP_WRITER();
  logger_xml_file_writer = new LOGGER_XML_FILE_WRITER();
  logger_netstream_writer = new LOGGER_NETSTREAM_WRITER();

  instrumenter = new INSTRUMENTER("instrumenter", this);
  http_server = new HTTP_SERVER("http-server");

  // - debugger
  if (enable_gdb_server or enable_inline_debugger or enable_profiler)
    debugger = new DEBUGGER("debugger");
  if (enable_gdb_server)
    gdb_server = new GDB_SERVER("gdb-server");
  if (enable_inline_debugger)
    inline_debugger = new INLINE_DEBUGGER("inline-debugger");
  if (enable_profiler)
    profiler = new PROFILER("profiler");
  if (enable_cfg_builder)
    cfg_builder = new CFG_BUILDER("cfg-builder");

  // In Linux mode, the system is not entirely simulated.
  // This mode allows to run Linux applications without simulating all the peripherals.

  cpu.master_socket(memory.slave_sock);

  // CPU <-> Memory connections
  cpu.memory_import >> memory.memory_export;

  // CPU <-> LinuxOS connections
  cpu.linux_os_import >> linux_os.linux_os_export_;
  // cpu.symbol_table_lookup_import >> linux_os.symbol_table_lookup_export;
  linux_os.memory_import_ >> cpu.memory_export;
  linux_os.memory_injection_import_ >> cpu.memory_injection_export;
  linux_os.registers_import_ >> cpu.registers_export;

  if (debugger)
    {
      // Debugger <-> CPU connections
      cpu.debug_yielding_import                            >> *debugger->debug_yielding_export[0];
      cpu.trap_reporting_import                            >> *debugger->trap_reporting_export[0];
      cpu.memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
      *debugger->disasm_import[0]                          >> cpu.disasm_export;
      *debugger->memory_import[0]                          >> cpu.memory_export;
      *debugger->registers_import[0]                       >> cpu.registers_export;
      *debugger->memory_access_reporting_control_import[0] >> cpu.memory_access_reporting_control_export;

      // Debugger <-> LinuxOS connections
      debugger->blob_import >> linux_os.blob_export_;
    }

  {
    unsigned idx = 0;
    if (gdb_server)
      {
        // gdb-server <-> debugger connections
        ++idx;
        *debugger->debug_yielding_import[idx]       >> gdb_server->debug_yielding_export;
        *debugger->debug_event_listener_import[idx] >> gdb_server->debug_event_listener_export;
        gdb_server->debug_yielding_request_import   >> *debugger->debug_yielding_request_export[idx];
        gdb_server->debug_selecting_import          >> *debugger->debug_selecting_export[idx];
        gdb_server->debug_event_trigger_import      >> *debugger->debug_event_trigger_export[idx];
        gdb_server->memory_import                   >> *debugger->memory_export[idx];
        gdb_server->registers_import                >> *debugger->registers_export[idx];
      }

    if (inline_debugger)
      {
        // inline-debugger <-> debugger connections
        ++idx;
        *debugger->debug_event_listener_import[idx]    >> inline_debugger->debug_event_listener_export;
        *debugger->debug_yielding_import[idx]          >> inline_debugger->debug_yielding_export;
        inline_debugger->debug_yielding_request_import >> *debugger->debug_yielding_request_export[idx];
        inline_debugger->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[idx];
        inline_debugger->disasm_import                 >> *debugger->disasm_export[idx];
        inline_debugger->memory_import                 >> *debugger->memory_export[idx];
        inline_debugger->registers_import              >> *debugger->registers_export[idx];
        inline_debugger->stmt_lookup_import            >> *debugger->stmt_lookup_export[idx];
        inline_debugger->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[idx];
        inline_debugger->stack_frame_import            >> *debugger->stack_frame_export[idx];
        inline_debugger->debug_info_loading_import     >> *debugger->debug_info_loading_export[idx];
        inline_debugger->data_object_lookup_import     >> *debugger->data_object_lookup_export[idx];
        inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[idx];
      }

    if (profiler)
      {
        // profiler <-> debugger connections
        ++idx;
        *debugger->debug_event_listener_import[idx] >> profiler->debug_event_listener_export;
        *debugger->debug_yielding_import[idx]       >> profiler->debug_yielding_export;
        profiler->debug_yielding_request_import     >> *debugger->debug_yielding_request_export[idx];
        profiler->debug_event_trigger_import        >> *debugger->debug_event_trigger_export[idx];
        profiler->disasm_import                     >> *debugger->disasm_export[idx];
        profiler->memory_import                     >> *debugger->memory_export[idx];
        profiler->registers_import                  >> *debugger->registers_export[idx];
        profiler->stmt_lookup_import                >> *debugger->stmt_lookup_export[idx];
        profiler->symbol_table_lookup_import        >> *debugger->symbol_table_lookup_export[idx];
        profiler->stack_frame_import                >> *debugger->stack_frame_export[idx];
        profiler->debug_info_loading_import         >> *debugger->debug_info_loading_export[idx];
        profiler->data_object_lookup_import         >> *debugger->data_object_lookup_export[idx];
        profiler->subprogram_lookup_import          >> *debugger->subprogram_lookup_export[idx];
      }
  }

  if (cfg_builder)
    {
      // CFG builder <-> debugger connections
      cpu.instruction_collecting_import >> cfg_builder->instruction_collecting_export;
      cfg_builder->disasm_import >> cpu.disasm_export;
      cfg_builder->symbol_table_lookup_import >> linux_os.symbol_table_lookup_export;
    }

  {
    unsigned idx = 0;
    *http_server->http_server_import[idx++] >> instrumenter->http_server_export;
    *http_server->http_server_import[idx++] >> logger_http_writer->http_server_export;
    if (profiler)
      {
        *http_server->http_server_import[idx++] >> profiler->http_server_export;
      }
  }
  {
    unsigned idx = 0;
    *http_server->registers_import[idx++] >> cpu.registers_export;
  }
}

Simulator::~Simulator()
{
  delete gdb_server;
  delete inline_debugger;
  delete profiler;
  delete http_server;
  delete instrumenter;
  delete cfg_builder;
  delete debugger;
  delete logger_console_printer;
  delete logger_text_file_writer;
  delete logger_http_writer;
  delete logger_xml_file_writer;
  delete logger_netstream_writer;
}

int
Simulator::Run()
{
  if ( unlikely(SimulationFinished()) ) return 0;

  //  double time_start = host_time->GetTime();

  unisim::kernel::tlm2::Simulator::Run();

  // double time_stop = host_time->GetTime();
  // double spent_time = time_stop - time_start;
  // simulation_spent_time += spent_time;

  // std::cerr << "Simulation run-time parameters:" << std::endl;
  // DumpParameters(std::cerr);
  // std::cerr << std::endl;
  //
  // std::cerr << "Simulation statistics:" << std::endl;
  // DumpStatistics(std::cerr);
  // std::cerr << std::endl;

  // std::cerr << "simulation time: " << simulation_spent_time << " seconds" << std::endl;
  // std::cerr << "simulated time : " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
  // std::cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << std::endl;
  // std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;

  if (profiler)
  {
    profiler->Output();
  }

  return GetExitStatus();
}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
  if (inline_debugger or profiler)
    {
      SetVariable("debugger.parse-dwarf", true);
    }

  unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();
	
  return setup_status;
}

bool Simulator::EndSetup()
{
  if (profiler)
  {
    http_server->AddJSAction(
      unisim::service::interfaces::ToolbarOpenTabAction(
        /* name */      profiler->GetName(),
        /* label */     "<img src=\"/unisim/service/debug/profiler/icon_profile_cpu0.svg\" alt=\"Profile\">",
        /* tips */      std::string("Profile of ") + cpu.GetName(),
        /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
        /* uri */       profiler->URI()
    ));
  }

  return true;
}

void
Simulator::DefaultConfiguration(unisim::kernel::Simulator *sim)
{
  new unisim::kernel::config::xml::XMLConfigFileHelper(sim);
  new unisim::kernel::config::ini::INIConfigFileHelper(sim);
  new unisim::kernel::config::json::JSONConfigFileHelper(sim);

  // meta information
  sim->SetVariable("program-name", "UNISIM ARMEMU");
  sim->SetVariable("copyright", "Copyright (C) 2017, Commissariat a l'Energie Atomique (CEA)");
  sim->SetVariable("license", "BSD (see file COPYING)");
  sim->SetVariable("authors", "Yves Lhuillier <yves.lhuillier@cea.fr>");
  sim->SetVariable("version", VERSION);
  sim->SetVariable("description", "UNISIM ARMEMU, ARMv8 generic simulator with linux emulation.");

  //=========================================================================
  //===                     Component run-time configuration              ===
  //=========================================================================

  sim->SetVariable("logger.std_err", true);
  sim->SetVariable("logger.std_err_color", true);

  sim->SetVariable("HARDWARE.cpu.default-endianness",   "little-endian");
  sim->SetVariable("HARDWARE.cpu.cpu-cycle-time",       "31250 ps"); // 32Mhz
  sim->SetVariable("HARDWARE.cpu.bus-cycle-time",       "31250 ps"); // 32Mhz
  sim->SetVariable("HARDWARE.cpu.icache.size",          0x020000); // 128 KB
  sim->SetVariable("HARDWARE.cpu.dcache.size",          0x020000); // 128 KB
  sim->SetVariable("HARDWARE.cpu.nice-time",            "1 ms"); // 1ms
  sim->SetVariable("HARDWARE.cpu.ipc",                  1.0);
  sim->SetVariable("HARDWARE.cpu.voltage",              1.8 * 1e3); // 1800 mV
  sim->SetVariable("HARDWARE.cpu.enable-dmi",           true); // Enable SystemC TLM 2.0 DMI
  sim->SetVariable("HARDWARE.memory.bytesize",          0xffffffffUL);
  sim->SetVariable("HARDWARE.memory.cycle-time",        "31250 ps");
  sim->SetVariable("HARDWARE.memory.read-latency",      "31250 ps");
  sim->SetVariable("HARDWARE.memory.write-latency",     "0 ps");
  sim->SetVariable("linux-os.system",          "arm-eabi");
  sim->SetVariable("linux-os.endianness",      "little-endian");
  sim->SetVariable("linux-os.memory-page-size",0x01000UL);
  sim->SetVariable("linux-os.stack-base",      0x40000000UL);
  sim->SetVariable("linux-os.envc",            0);
  sim->SetVariable("linux-os.utsname-sysname", "Linux");
  sim->SetVariable("linux-os.utsname-nodename","localhost");
  sim->SetVariable("linux-os.utsname-release", "5.15.110-unisim");
  sim->SetVariable("linux-os.utsname-version", "#UNISIM SMP Mon May 8 17:24:35 UTC 2023");
  sim->SetVariable("linux-os.utsname-machine", "aarch64");
  sim->SetVariable("linux-os.utsname-domainname","localhost");
  sim->SetVariable("linux-os.apply-host-environment", false);
  sim->SetVariable("linux-os.hwcap", "");

  sim->SetVariable("gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_aarch64.xml");
  sim->SetVariable("debugger.parse-dwarf", false);
  sim->SetVariable("debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/aarch64_eabi_dwarf_register_number_mapping.xml");
  sim->SetVariable("debugger.architecture[0]", "aarch64");

  sim->SetVariable("inline-debugger.num-loaders", 1);
  sim->SetVariable("inline-debugger.search-path", "");
  sim->SetVariable("il1-power-estimator.cache-size", 32 * 128);
  sim->SetVariable("il1-power-estimator.line-size", 32);
  sim->SetVariable("il1-power-estimator.associativity", 4);
  sim->SetVariable("il1-power-estimator.rw-ports", 0);
  sim->SetVariable("il1-power-estimator.excl-read-ports", 1);
  sim->SetVariable("il1-power-estimator.excl-write-ports", 0);
  sim->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
  sim->SetVariable("il1-power-estimator.banks", 1);
  sim->SetVariable("il1-power-estimator.tech-node", 130); // in nm
  sim->SetVariable("il1-power-estimator.output-width", 32 * 8);
  sim->SetVariable("il1-power-estimator.tag-width", 32); // to fix
  sim->SetVariable("il1-power-estimator.access-mode", "fast");
  sim->SetVariable("il1-power-estimator.verbose", false);

  sim->SetVariable("dl1-power-estimator.cache-size", 32 * 128);
  sim->SetVariable("dl1-power-estimator.line-size", 32);
  sim->SetVariable("dl1-power-estimator.associativity", 4);
  sim->SetVariable("dl1-power-estimator.rw-ports", 1);
  sim->SetVariable("dl1-power-estimator.excl-read-ports", 0);
  sim->SetVariable("dl1-power-estimator.excl-write-ports", 0);
  sim->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
  sim->SetVariable("dl1-power-estimator.banks", 1);
  sim->SetVariable("dl1-power-estimator.tech-node", 130); // in nm
  sim->SetVariable("dl1-power-estimator.output-width", 32 * 8);
  sim->SetVariable("dl1-power-estimator.tag-width", 32); // to fix
  sim->SetVariable("dl1-power-estimator.access-mode", "fast");
  sim->SetVariable("dl1-power-estimator.verbose", false);

  sim->SetVariable("http-server.http-port", 12360);
}
