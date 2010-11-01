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

#include "simulator.hh"
bool debug_enabled;

void SigIntHandler(int signum) {
	cerr << "Interrupted by Ctrl-C or SIGINT signal (" << signum << ")" << endl;
	sc_stop();
}

using namespace std;

Simulator ::
Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, Simulator::DefaultConfiguration)
#ifdef SIM_LIBRARY
	, unisim::kernel::service::VariableBase::Notifiable()
	, unisim::service::trap_handler::ExternalTrapHandlerInterface()
#endif // SIM_LIBRARY
	, cpu(0)
	, devchip(0)
	//, irq_master_stub(0)
	//, fiq_master_stub(0)
	, memory(0)
	, nor_flash_2(0)
	, nor_flash_1(0)
	, time(0)
	, host_time(0)
	, elf32_loader(0)
	, raw_kernel_loader(0)
	, raw_fs_loader(0)
	, trap_handler(0)
	, simulation_spent_time(0.0)
#ifdef SIM_GDB_SERVER_SUPPORT
	, gdb_server(0)
	, enable_gdb_server(false)
	, param_enable_gdb_server(0)
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	, inline_debugger(0)
	, enable_inline_debugger(false)
	, param_enable_inline_debugger(0)
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
	, sim_debugger(0)
	, enable_sim_debugger(true) // the debugger is activated by default on api mode
	, param_enable_sim_debugger(0)
#endif // SIM_SIM_DEBUGGER_SUPPORT
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, enable_power_estimation(false)
	, param_enable_power_estimation("enable-power-estimation", 0,
			enable_power_estimation,
			"Activate caches power estimation.")
	, formula_caches_total_dynamic_energy(0)
	, formula_caches_total_dynamic_power(0)
	, formula_caches_total_leakage_power(0)
	, formula_caches_total_power(0)
#endif // SIM_POWER_ESTIMATOR_SUPPORT
#ifdef SIM_LIBRARY
	, notif_list()
	, trap_handler_context(0)
	, trap_handler_function(0)
#endif // SIM_LIBRARY
{
	cpu = new CPU("cpu");
	devchip = new DEVCHIP("pxp");
	// irq_master_stub = new IRQ_MASTER_STUB("irq-master-stub");
	// fiq_master_stub = new FIQ_MASTER_STUB("fiq-master-stub");
	memory = new MEMORY("memory");
	nor_flash_2 = new FLASH("nor-flash-2");
	nor_flash_1 = new FLASH("nor-flash-1");
	time = new unisim::service::time::sc_time::ScTime("time");
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	elf32_loader = new ELF32_LOADER("elf-loader");
	raw_kernel_loader = new RAW_LOADER("raw-kernel-loader");
	raw_fs_loader = new RAW_LOADER("raw-fs-loader");
	trap_handler = new TRAP_HANDLER("trap-handler");
#ifdef SIM_GDB_SERVER_SUPPORT
	param_enable_gdb_server = new unisim::kernel::service::Parameter<bool>(
			"enable-gdb-server", 0,
			enable_gdb_server,
			"Enable GDB server.");
	if ( enable_gdb_server )
		gdb_server = new GDB_SERVER("gdb-server");
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	param_enable_inline_debugger = new unisim::kernel::service::Parameter<bool>(
			"enable-inline-debugger", 0,
			enable_inline_debugger,
			"Enable inline debugger.");
	if ( enable_inline_debugger )
		inline_debugger = new INLINE_DEBUGGER("inline-debugger");
#endif
#ifdef SIM_SIM_DEBUGGER_SUPPORT
	param_enable_sim_debugger = new unisim::kernel::service::Parameter<bool>(
			"enable-sim-debugger", 0,
			enable_sim_debugger,
			"Enable sim debugger.");
	if ( enable_sim_debugger )
		sim_debugger = new SIM_DEBUGGER("sim-debugger");
#endif
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	if ( enable_power_estimation )
	{
		il1_power_estimator = new POWER_ESTIMATOR("il1-power-estimator");
		dl1_power_estimator = new POWER_ESTIMATOR("dl1-power-estimator");
		formula_caches_total_dynamic_energy =
				new unisim::kernel::service::Formula<double> (
						"caches-total-dynamic-energy", 0,
						unisim::kernel::service::Formula<double>::OP_ADD,
						&(*dl1_power_estimator)["dynamic-energy"],
						&(*il1_power_estimator)["dynamic-energy"],
						0,
						"caches total dynamic energy (in J)");
		formula_caches_total_dynamic_power =
				new unisim::kernel::service::Formula<double> (
						"caches-total-dynamic-power", 0,
						unisim::kernel::service::Formula<double>::OP_ADD,
						&(*dl1_power_estimator)["dynamic-power"],
						&(*il1_power_estimator)["dynamic-power"],
						0,
						"caches total dynamic power (in J)");
		formula_caches_total_leakage_power =
				new unisim::kernel::service::Formula<double> (
						"caches-total-leakage-power", 0,
						unisim::kernel::service::Formula<double>::OP_ADD,
						&(*dl1_power_estimator)["leakage-power"],
						&(*il1_power_estimator)["leakage-power"],
						0,
						"caches total leakage power (in J)");
		formula_caches_total_power =
				new unisim::kernel::service::Formula<double> (
						"caches-total-power", 0,
						unisim::kernel::service::Formula<double>::OP_ADD,
						formula_caches_total_dynamic_power,
						formula_caches_total_leakage_power,
						0,
						"caches total (dynamic+leakage) power (in J)");
	}
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	// In Linux mode, the system is not entirely simulated.
	// This mode allows to run Linux applications without simulating all the peripherals.

	cpu->master_socket(devchip->cpu_target_socket);
	devchip->ssmc0_init_socket(nor_flash_2->slave_sock);
	devchip->ssmc1_init_socket(nor_flash_1->slave_sock);
	devchip->mpmc0_init_socket(memory->slave_sock);
	// cpu->master_socket(memory->slave_sock);
	// irq_master_stub->out_interrupt(cpu->in_irq);
	// fiq_master_stub->out_interrupt(cpu->in_fiq);

#ifdef SIM_GDB_SERVER_SUPPORT
	EnableGdbServer();
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	EnableInlineDebugger();
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
	EnableSimDebugger();
#endif // SIM_SIM_DEBUGGER_SUPPORT

	// Connect everything
	elf32_loader->memory_import >> memory->memory_export;
	raw_kernel_loader->memory_import >> memory->memory_export;
	raw_fs_loader->memory_import >> memory->memory_export;
	//linux_loader->memory_import >> memory->memory_export;
	//linux_loader->loader_import >> elf32_loader->loader_export;
	//cpu->linux_os_import >> linux_os->linux_os_export;
	//linux_os->memory_import >> cpu->memory_export;
	//linux_os->memory_injection_import >> cpu->memory_injection_export;
	//linux_os->registers_import >> cpu->registers_export;
	//linux_os->loader_import >> linux_loader->loader_export;
	//cpu->exception_trap_reporting_import >> 
	//	*trap_handler->trap_reporting_export[0];
	cpu->instruction_counter_trap_reporting_import >> 
		*trap_handler->trap_reporting_export[0];
	// cpu->irq_trap_reporting_import >> *trap_handler->trap_reporting_export[2];
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	// connecting power estimator
	if ( enable_power_estimation )
	{
		cpu->icache.power_estimator_import >> 
			il1_power_estimator->power_estimator_export;
		cpu->icache.power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dcache.power_estimator_import >> 
			dl1_power_estimator->power_estimator_export;
		cpu->dcache.power_mode_import >> dl1_power_estimator->power_mode_export;

		il1_power_estimator->time_import >> time->time_export;
		dl1_power_estimator->time_import >> time->time_export;
	}
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	cpu->symbol_table_lookup_import >> elf32_loader->symbol_table_lookup_export;
	// bridge->memory_import >> memory->memory_export;
	
}

Simulator::~Simulator()
{
	if ( cpu ) delete cpu;
	// if ( irq_master_stub ) delete irq_master_stub;
	// if ( fiq_master_stub ) delete fiq_master_stub;
	if ( devchip ) delete devchip;
	if ( memory ) delete memory;
	if ( nor_flash_2 ) delete nor_flash_2;
	if ( nor_flash_1 ) delete nor_flash_1;
	if ( time ) delete time;
	if ( host_time ) delete host_time;
	if ( elf32_loader ) delete elf32_loader;
	if ( raw_kernel_loader ) delete raw_kernel_loader;
	if ( raw_fs_loader ) delete raw_fs_loader;
	if ( trap_handler ) delete trap_handler;
#ifdef SIM_GDB_SERVER_SUPPORT
	if ( param_enable_gdb_server ) delete param_enable_gdb_server;
	if ( gdb_server ) delete gdb_server;
#endif // SIM_GDB_SERVER_SUPPORT
#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	if ( param_enable_inline_debugger ) delete param_enable_inline_debugger;
	if ( inline_debugger ) delete inline_debugger;
#endif // SIM_INLINE_DEBUGGER_SUPPORT
#ifdef SIM_SIM_DEBUGGER_SUPPORT
	if ( param_enable_sim_debugger ) delete param_enable_sim_debugger;
	if ( sim_debugger ) delete sim_debugger;
#endif // SIM_SIM_DEBUGGER_SUPPORT
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	if ( il1_power_estimator ) delete il1_power_estimator;
	if ( dl1_power_estimator ) delete dl1_power_estimator;
#endif // SIM_POWER_ESTIMATOR_SUPPORT
}

int
Simulator ::
Run()
{
	static uint32_t bootloader[] = {
		0xe3a00000,
		0xe3a01083,
		0xe3811c01,
		0xe59f2000,
		0xe59ff000,
		0x00000100,
		0x00010000
	};

	memory->WriteMemory(0, bootloader, sizeof(bootloader));
	if ( unlikely(SimulationFinished()) ) return 0;

#ifndef SIM_LIBRARY
	double time_start = host_time->GetTime();
#endif // !SIM_LIBRARY

#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	void (*prev_sig_int_handler)(int);

	if ( ! inline_debugger )
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}
#endif // SIM_INLINE_DEBUGGER_SUPPORT

	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation."
			<< " Bailing out..." << endl;
		cerr << e.what() << endl;
	}

#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	if ( !inline_debugger )
	{
		signal(SIGINT, prev_sig_int_handler);
	}
#endif // SIM_INLINE_DEBUGGER_SUPPORT

#ifndef SIM_LIBRARY
	cerr << "Simulation finished" << endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;
	simulation_spent_time += spent_time;


	cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(cerr);
	cerr << endl;
	cerr << "Simulation formulas:" << endl;
	DumpFormulas(cerr);
	cerr << endl;
	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	cerr << "simulation time: " << simulation_spent_time << " seconds" << endl;
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
#endif // SIM_LIBRARY

	return 0;
}

int
Simulator ::
Run(double time, sc_time_unit unit)
{
	if ( unlikely(SimulationFinished()) ) return 0;

	double time_start = host_time->GetTime();

#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	void (*prev_sig_int_handler)(int);

	if ( ! inline_debugger )
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}
#endif // SIM_INLINE_DEBUGGER_SUPPORT

	try
	{
		sc_start(time, unit);
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	if ( !inline_debugger )
	{
		signal(SIGINT, prev_sig_int_handler);
	}
#endif // SIM_INLINE_DEBUGGER_SUPPORT

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;
	simulation_spent_time += spent_time;

	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	return 0;
}

bool
Simulator ::
IsRunning() const
{
	return sc_is_running();
}

bool
Simulator ::
SimulationStarted() const
{
	return sc_start_of_simulation_invoked();
}

bool
Simulator ::
SimulationFinished() const
{
	return sc_end_of_simulation_invoked();
}

void
Simulator ::
Stop(unisim::kernel::service::Object *object, int exit_status)
{
	sc_stop();
}

void
Simulator ::
DefaultConfiguration(unisim::kernel::service::Simulator *sim)
{
	sim->SetVariable("program-name", SIM_PROGRAM_NAME);
	sim->SetVariable("authors", SIM_AUTHOR);
	sim->SetVariable("version", SIM_VERSION);
	sim->SetVariable("copyright", SIM_COPYRIGHT);
	sim->SetVariable("license", SIM_LICENSE);
	sim->SetVariable("description", SIM_DESCRIPTION);

	sim->SetVariable("kernel_logger.std_err", true);
	sim->SetVariable("kernel_logger.std_err_color", true);

	sim->SetVariable("cpu.bigendinit",           "little-endian");
	sim->SetVariable("cpu.cpu-cycle-time",       31250UL); // 32Mhz
	sim->SetVariable("cpu.bus-cycle-time",       31250UL); // 32Mhz
	sim->SetVariable("cpu.icache.size",          0x020000); // 128 KB
	sim->SetVariable("cpu.dcache.size",          0x020000); // 128 KB
	sim->SetVariable("cpu.nice-time",            1000000000); // 1ms
	sim->SetVariable("cpu.ipc",                  1.0);
	sim->SetVariable("memory.bytesize",          0xffffffffUL); 
	sim->SetVariable("memory.cycle-time",        "13332ps");
	sim->SetVariable("nor-flash-2.bytesize",     0xffffffffUL); 
	sim->SetVariable("nor-flash-2.cycle-time",   "13332ps");
	sim->SetVariable("nor-flash-1.bytesize",     0xffffffffUL); 
	sim->SetVariable("nor-flash-1.cycle-time",   "13332ps");
	sim->SetVariable("elf-loader.filename",      "test/install/u-boot");
	sim->SetVariable("elf-loader.force-base-addr",
												 true);
	sim->SetVariable("elf-loader.base-addr",     0x10000);
	sim->SetVariable("elf-loader.force-use-virtual-address",
												 true);
	sim->SetVariable("raw-kernel-loader.filename",      
												 "test/install/uImage");
	sim->SetVariable("raw-kernel-loader.base-addr",
												 0x210000);
	sim->SetVariable("raw-fs-loader.filename",   "test/install/rootfs.tar.gz.uimg");
	sim->SetVariable("raw-fs-loader.base-addr",  0x410000);

	sim->SetVariable("trap-handler.num-traps", 3);
	sim->SetVariable("trap-handler.trap-reporting-export-name[0]",
			"cpu-exception-trap-handler");
	sim->SetVariable("trap-handler.trap-reporting-export-name[1]",
			"cpu-instruction-counter-trap-handler");
	sim->SetVariable("trap-handler.trap-reporting-export-name[2]",
			"cpu-irq-trap-handler");

#ifdef SIM_GDB_SERVER_SUPPORT
	sim->SetVariable("gdb-server.architecture-description-filename",
			"gdb_server/gdb_armv5l.xml");
#endif // SIM_GDB_SERVER_SUPPORT

#ifdef SIM_INLINE_DEBUGGER_SUPPORT
	sim->SetVariable("inline-debugger.num-loaders", 1);
	sim->SetVariable("inline-debugger.search-path", "");
#endif // SIM_INLINE_DEBUGGER_SUPPORT

#ifdef SIM_SIM_DEBUGGER_SUPPORT
	sim->SetVariable("sim-debugger.num-loaders", 1);
	sim->SetVariable("sim-debugger.search-path", "");
#endif // SIM_INLINE_DEBUGGER_SUPPORT

#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	sim->SetVariable("enable-power-estimation", true);
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
#endif // SIM_POWER_ESTIMATOR_SUPPORT
}

#ifdef SIM_LIBRARY

bool
Simulator::
AddNotifiable (void *(*notif_function)(const char *), const char *var_name)
{
	return false;
}

bool
Simulator::
SetTrapHandler (void (*function)(void *, unsigned int), void *context)
{
	trap_handler_function = function;
	trap_handler_context = context;
	trap_handler->SetExternalTrapHandler(this);

	return true;
}

unisim::util::debug::debugger_handler::DebuggerHandler *
Simulator::
GetDebugger(const char *name)
{
	if ( enable_sim_debugger )
		return sim_debugger;
	else
		return 0;
}

void
Simulator::
ExternalTrap (unsigned int id)
{
	if (trap_handler_function)
	{
		(*trap_handler_function)(trap_handler_context, id);
	}
}

void
Simulator::
VariableNotify(const char *name)
{
	// use this function to check the variable that was notified
	// NOTE: for the moment it is empty, but more to come :-P
}

#endif // SIM_LIBRARY

#ifdef SIM_GDB_SERVER_SUPPORT
void
Simulator::
EnableGdbServer()
{
	if(enable_gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
	}
}
#endif // SIM_GDB_SERVER_SUPPORT

#ifdef SIM_INLINE_DEBUGGER_SUPPORT
void
Simulator::
EnableInlineDebugger()
{
	if ( enable_inline_debugger )
	{
		// connect the inline debugger to other components
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
		inline_debugger->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
		//*inline_debugger->loader_import[0] >>
		//	linux_os->loader_export;
		*inline_debugger->loader_import[0] >>
			elf32_loader->loader_export;
		*inline_debugger->symbol_table_lookup_import[0] >>
			elf32_loader->symbol_table_lookup_export;
		*inline_debugger->stmt_lookup_import[0] >>
			elf32_loader->stmt_lookup_export;
	}
}
#endif // SIM_INLINE_DEBUGGER_SUPPORT

#ifdef SIM_SIM_DEBUGGER_SUPPORT
void
Simulator ::
EnableSimDebugger()
{
	if ( enable_sim_debugger )
	{
		// connect the simulator api debugger to other components
		cpu->debug_control_import >> sim_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> sim_debugger->memory_access_reporting_export;
		sim_debugger->disasm_import >> cpu->disasm_export;
		sim_debugger->memory_import >> cpu->memory_export;
		sim_debugger->registers_import >> cpu->registers_export;
		sim_debugger->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
		//*sim_debugger->loader_import[0] >>
		//	linux_os->loader_export;
		*sim_debugger->symbol_table_lookup_import[0] >>
			elf32_loader->symbol_table_lookup_export;
		*sim_debugger->stmt_lookup_import[0] >>
			elf32_loader->stmt_lookup_export;
	}
}
#endif // SIM_SIM_DEBUGGER_SUPPORT
