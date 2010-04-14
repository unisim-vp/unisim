/*
 *  Copyright (c) 2009,
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

// #include <Python.h>
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
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/loader/elf_loader/elf_loader.hh"
#include "unisim/service/loader/elf_loader/elf_loader.tcc"
#include "unisim/service/loader/linux_loader/linux_loader.hh"
#include "unisim/service/os/linux_os/linux_os_32/linux_os_32.hh"
#include "config.hh"
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
#include "unisim/service/power/cache_power_estimator.hh"
#endif // SIM_POWER_ESTIMATOR_SUPPORT

bool debug_enabled;

void EnableDebug() {
	debug_enabled = true;
}

void DisableDebug() {
	debug_enabled = false;
}

void SigIntHandler(int signum) {
	cerr << "Interrupted by Ctrl-C or SIGINT signal (" << signum << ")" << endl;
	sc_stop();
}

using namespace std;
using unisim::service::time::sc_time::ScTime;
using unisim::service::time::host_time::HostTime;

class Simulator : public unisim::kernel::service::Simulator
{
public:
	Simulator(int argc, char **argv,
			void (*LoadBuiltInConfig)(unisim::kernel::service::Simulator *));
	virtual ~Simulator();
	int Run();

protected:
private:
	typedef unisim::component::tlm2::processor::arm::arm926ejs::ARM926EJS CPU;
	typedef unisim::component::tlm2::interrupt::InterruptMasterStub IRQ_MASTER_STUB;
	typedef unisim::component::tlm2::interrupt::InterruptMasterStub FIQ_MASTER_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, true> MEMORY;
	typedef unisim::service::debug::gdb_server::GDBServer<uint64_t> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<uint64_t> INLINE_DEBUGGER;
	typedef unisim::service::loader::linux_loader::LinuxLoader<uint64_t> LINUX_LOADER;
	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> ELF32_LOADER;
	typedef unisim::service::os::linux_os::linux_os_32::LinuxOS32 LINUX_OS;
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	typedef unisim::service::power::CachePowerEstimator POWER_ESTIMATOR;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	CPU *cpu;
	IRQ_MASTER_STUB *irq_master_stub;
	FIQ_MASTER_STUB *fiq_master_stub;
	MEMORY *memory;
	ScTime *time;
	HostTime *host_time;
	ELF32_LOADER *elf32_loader;
	LINUX_LOADER *linux_loader;
	LINUX_OS *linux_os;
	GDB_SERVER *gdb_server;
	INLINE_DEBUGGER *inline_debugger;
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	POWER_ESTIMATOR *il1_power_estimator;
	POWER_ESTIMATOR *dl1_power_estimator;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool enable_power_estimation;
	unisim::kernel::service::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::service::Parameter<bool> param_enable_inline_debugger;
	unisim::kernel::service::Parameter<bool> param_enable_power_estimation;
};

Simulator ::
Simulator(int argc, char **argv,
		void (*LoadBuiltInConfig)(unisim::kernel::service::Simulator *)) :
	unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig),
	cpu(0),
	irq_master_stub(0),
	fiq_master_stub(0),
	memory(0),
	time(0),
	host_time(0),
	elf32_loader(0),
	linux_loader(0),
	linux_os(0),
	gdb_server(0),
	inline_debugger(0),
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	il1_power_estimator(0),
	dl1_power_estimator(0),
#endif // SIM_POWER_ESTIMATOR_SUPPORT
	enable_gdb_server(false),
	enable_inline_debugger(false),
	enable_power_estimation(false),
	param_enable_gdb_server("enable-gdb-server", 0,
			enable_gdb_server,
			"Enable gdb server."),
	param_enable_inline_debugger("enable-inline-debugger", 0,
			enable_inline_debugger,
			"Activate the inline debugger."),
	param_enable_power_estimation("enable-power-estimation", 0,
			enable_power_estimation,
			"Activate caches power estimation.")
{
	cpu = new CPU("cpu");
	irq_master_stub = new IRQ_MASTER_STUB("irq-master-stub");
	fiq_master_stub = new FIQ_MASTER_STUB("fiq-master-stub");
	memory = new MEMORY("memory");
	time = new ScTime("time");
	host_time = new HostTime("host-time");
	elf32_loader = new ELF32_LOADER("elf-loader");
	linux_loader = new LINUX_LOADER("linux-loader");
	linux_os = new LINUX_OS("linux-os");
	if ( enable_gdb_server )
		gdb_server = new GDB_SERVER("gdb-server");
	if ( enable_inline_debugger )
		inline_debugger = new INLINE_DEBUGGER("inline-debugger");
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	if ( enable_power_estimation )
	{
		il1_power_estimator = new POWER_ESTIMATOR("il1-power-estimator");
		dl1_power_estimator = new POWER_ESTIMATOR("dl1-power-estimator");
	}
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	// In Linux mode, the system is not entirely simulated.
	// This mode allows to run Linux applications without simulating all the peripherals.

	cpu->master_socket(memory->slave_sock);
	irq_master_stub->out_interrupt(cpu->in_irq);
	fiq_master_stub->out_interrupt(cpu->in_fiq);

	if(enable_inline_debugger)
	{
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
		inline_debugger->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
	}
	else if(enable_gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
	}

	// Connect everything
	elf32_loader->memory_import >> memory->memory_export;
	linux_loader->memory_import >> memory->memory_export;
	linux_loader->loader_import >> elf32_loader->loader_export;
	cpu->linux_os_import >> linux_os->linux_os_export;
	linux_os->cpu_linux_os_import >> cpu->cpu_linux_os_export;
	linux_os->memory_import >> cpu->memory_export;
	linux_os->memory_injection_import >> cpu->memory_injection_export;
	linux_os->registers_import >> cpu->registers_export;
	linux_os->loader_import >> linux_loader->loader_export;
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	// connecting power estimator
	if ( enable_power_estimation )
	{
		cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
		cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
		cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;

		il1_power_estimator->time_import >> time->time_export;
		dl1_power_estimator->time_import >> time->time_export;
	}
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	cpu->symbol_table_lookup_import >> elf32_loader->symbol_table_lookup_export;
	// bridge->memory_import >> memory->memory_export;

	if ( enable_inline_debugger )
	{
		inline_debugger->symbol_table_lookup_import >>
			elf32_loader->symbol_table_lookup_export;
	}
}

Simulator::~Simulator()
{
	if ( cpu ) delete cpu;
	if ( irq_master_stub ) delete irq_master_stub;
	if ( fiq_master_stub ) delete fiq_master_stub;
	if ( memory ) delete memory;
	if ( time ) delete time;
	if ( host_time ) delete host_time;
	if ( elf32_loader ) delete elf32_loader;
	if ( linux_loader ) delete linux_loader;
	if ( linux_os ) delete linux_os;
	if ( gdb_server ) delete gdb_server;
	if ( inline_debugger ) delete inline_debugger;
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	if ( il1_power_estimator ) delete il1_power_estimator;
	if ( dl1_power_estimator ) delete dl1_power_estimator;
#endif // SIM_POWER_ESTIMATOR_SUPPORT
}

int Simulator::Run()
{
	double time_start = host_time->GetTime();

	EnableDebug();
	void (*prev_sig_int_handler)(int);

	if ( ! inline_debugger )
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}

	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	if ( !inline_debugger )
	{
		signal(SIGINT, prev_sig_int_handler);
	}

	cerr << "Simulation finished" << endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(cerr);
	cerr << endl;
	cerr << "Simulation formulas:" << endl;
	DumpFormulas(cerr);
	cerr << endl;
	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	if ( enable_estimate_power )
	{
		{
			double total_dynamic_energy = il1_power_estimator->GetDynamicEnergy()
					+ dl1_power_estimator->GetDynamicEnergy();

			double total_dynamic_power = il1_power_estimator->GetDynamicPower()
					+ dl1_power_estimator->GetDynamicPower();

			double total_leakage_power = il1_power_estimator->GetLeakagePower()
					+ dl1_power_estimator->GetLeakagePower();

			double total_power = total_dynamic_power + total_leakage_power;

			cerr << "L1 instruction cache dynamic energy: " << il1_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "L1 data cache dynamic energy: " << dl1_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "L1 instruction cache dynamic power: " << il1_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "L1 data cache dynamic power: " << dl1_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "L1 instruction cache leakage power: " << il1_power_estimator->GetLeakagePower() << " W" << endl;
			cerr << "L1 data cache leakage power: " << dl1_power_estimator->GetLeakagePower() << " W" << endl;
			cerr << "Total power (dynamic+leakage): " << total_power << " W" << endl;

			cerr << "Total dynamic energy: " << total_dynamic_energy << " J" << endl;
			cerr << "Total dynamic power: " << total_dynamic_power << " W" << endl;
			cerr << "Total leakage power: " << total_leakage_power << " W" << endl;
		}
	}
#endif // SIM_POWER_ESTIMATOR_SUPPORT
	return 0;
}

void ShowVersion()
{
	cerr << "UNISIM ARMv5 User Level Simulator "
		<< "v" << SIM_VERSION_MAJOR
		<< "." << SIM_VERSION_MINOR
		<< "-" << SIM_VERSION_PATCH
		<< " (" << SIM_VERSION_CODENAME << ")" << endl;
	cerr << "Author: " << SIM_AUTHOR << endl;
	cerr << "================================================" << endl;
	cerr << "================================================" << endl;
}

static void DefaultConfiguration(unisim::kernel::service::Simulator *sim)
{
	sim->SetVariable("kernel_logger.std_err", true);
	sim->SetVariable("kernel_logger.std_err_color", true);

	sim->SetVariable("cpu.default-endianness", "little-endian");
	sim->SetVariable("cpu.cpu-cycle-time", 1000UL);
	sim->SetVariable("memory.bytesize", 0xffffffffUL);
	sim->SetVariable("memory.cycle-time", 1000000UL);
	sim->SetVariable("linux-loader.stack-base", 0xc0000000UL);
	sim->SetVariable("linux-loader.max-environ", 0x4000UL);
	sim->SetVariable("linux-loader.endianess", "little-endian");
	sim->SetVariable("gdb-server.architecture-description-filename",
			"gdb_server.gdb_armv5l.xml");
	sim->SetVariable("elf-loader.filename", "test/install/test.armv5l");
	sim->SetVariable("linux-loader.argc", 1);
	sim->SetVariable("linux-loader.argv[0]", "test/install/test.armv5l");
	sim->SetVariable("linux-os.system", "arm");


#ifdef SIM_POWER_ESTIMATOR_SUPPORT
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
	sim->SetVariable("il1-power-estimator.verbose", true);

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
	sim->SetVariable("dl1-power-estimator.verbose", true);
#endif // SIM_POWER_ESTIMATOR_SUPPORT


int sc_main(int argc, char *argv[]) {
	int ret = 0;

	ShowVersion();

#ifdef WIN32
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cerr << "WSAStartup failed" << endl;
		return -1;
	}
#endif

	Simulator *simulator = new Simulator(argc, argv, DefaultConfiguration);

	if ( simulator->Setup() )
	{
		cerr << "Starting simulation" << endl;
		ret = simulator->Run();
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
		ret = 0;
	}

	if (simulator) delete simulator;
#ifdef WIN32
	//releases the winsock2 resources
	WSACleanup();
#endif

	return ret;
}
