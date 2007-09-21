/*
 *  Copyright (c) 2007,
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
 
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#else

#include <sys/times.h>

#endif

#include "unisim/kernel/service/service.hh"

#include "unisim/component/cxx/processor/arm/config.hh"
#include "unisim/component/tlm/processor/arm/arm.hh"
#include "unisim/component/tlm/memory/ram/memory.hh"
#include "unisim/component/tlm/bridge/simple_fsb_to_mem/config.hh"
#include "unisim/component/tlm/bridge/simple_fsb_to_mem/bridge.hh"
#include "unisim/component/tlm/debug/transaction_spy.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/component/tlm/message/memory.hh"

#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/debug/symbol_table/symbol_table.hh"
#include "unisim/service/loader/elf_loader/elf_loader.hh"
#include "unisim/service/loader/linux_loader/linux_loader.hh"
#include "unisim/service/os/linux_os/linux_os.hh"
#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/logger/logger_server.hh"

#include "unisim/service/debug/symbol_table/symbol_table.hh"

typedef unisim::component::cxx::processor::arm::ARM966E_S_BigEndian_DebugConfig CPU_CONFIG;
typedef unisim::component::tlm::bridge::simple_fsb_to_mem::Addr32BurstSize32_Config BRIDGE_CONFIG;

//static const bool DEBUG_INFORMATION = true;

bool debug_enabled;

void EnableDebug() {
	debug_enabled = true;
}

void DisableDebug() {
	debug_enabled = false;
}

void SigIntHandler(int signum) {
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}


using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::debug::symbol_table::SymbolTable;
using unisim::service::loader::linux_loader::LinuxLoader;
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::os::linux_os::LinuxOS;
using unisim::service::logger::LoggerServer;
using unisim::service::time::sc_time::ScTime;
using unisim::kernel::service::ServiceManager;

void help(char *prog_name) {
	cerr << "Usage: " << prog_name << " [<options>] <program> [program arguments]" << endl << endl;
	cerr << "       'program' is an ELF32 statically linked Linux binary" << endl;
	cerr << "Options:" << endl;
	cerr << "--inline-debugger" << endl;
	cerr << "-d" << endl;
	cerr << "            starts the inline debugger" << endl;
	cerr << "--gdb-server <TCP port>" << endl;
	cerr << "-g <TCP port>" << endl;
	cerr << "            starts a gdb server" << endl << endl;
	cerr << "--gdb-server-arch-file <arch file>" << endl;
	cerr << "-a  <arch file>" << endl;
	cerr << "            uses <arch file> as architecture description file for GDB server" << endl << endl;
	cerr << "-i <count>" << endl;
	cerr << "--max:inst <count>" << endl;
	cerr << "            execute <count> instructions then exit" << endl << endl;
	cerr << "-z" << endl;
	cerr << "--logger:zip" << endl;
	cerr << "            zip log file" << endl << endl;
	cerr << "-e" << endl;
	cerr << "--logger:error" << endl;
	cerr << "            pipe the log into the standard error" << endl << endl;
	cerr << "-o" << endl;
	cerr << "--logger:out" << endl;
	cerr << "            pipe the log into the standard output" << endl << endl;
	cerr << "-m" << endl;
	cerr << "--logger:message_spy" << endl;
	cerr << "            create message spies (requires one log to work)" << endl << endl;
	cerr << "-l <file>" << endl;
	cerr << "--logger:file <file>" << endl;
	cerr << "            store log file in <file>" << endl << endl;
	cerr << "--help" << endl;
	cerr << "-h" << endl;
	cerr << "            displays this help" << endl;
}

// Front Side Bus template parameters
typedef CPU_CONFIG::address_t FSB_ADDRESS_TYPE;
typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;

typedef unisim::component::tlm::message::SimpleFSBRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> BusMsgReqType;
typedef unisim::component::tlm::message::SimpleFSBResponse<FSB_MAX_DATA_SIZE> BusMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<BusMsgReqType, BusMsgRspType> BusMsgSpyType;
typedef unisim::component::tlm::message::MemoryRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MemMsgReqType;
typedef unisim::component::tlm::message::MemoryResponse<FSB_MAX_DATA_SIZE> MemMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<MemMsgReqType, MemMsgRspType> MemMsgSpyType;

int main(int argc, char *argv[], char **envp) {
	GarbageCollector::Setup();

	static struct option long_options[] = {
		{"inline-debugger", no_argument, 0, 'd'},
		{"gdb-server", required_argument, 0, 'g'},
		{"gdb-server-arch-file", required_argument, 0, 'a'},
		{"help", no_argument, 0, 'h'},
		{"max:inst", required_argument, 0, 'i'},
		{"logger:file", required_argument, 0, 'l'},
		{"logger:zip", no_argument, 0, 'z'},
		{"logger:error", no_argument, 0, 'e'},
		{"logger:out", no_argument, 0, 'o'},
		{"logger:message_spy", no_argument, 0, 'm'},
		{0, 0, 0, 0}
	};

	int c;
	bool use_gdb_server = false;
	bool use_inline_debugger = false;
	int gdb_server_tcp_port = 0;
	char *device_tree_filename = "device_tree.xml";
	char *gdb_server_arch_filename = "gdb_powerpc.xml";
	uint64_t maxinst = 0; // maximum number of instruction to simulate
	char *logger_filename = 0;
	bool logger_zip = false;
	bool logger_error = false;
	bool logger_out = false;
	bool logger_on = false;
	bool logger_messages = false;
	const uint32_t fsb_frequency = 75; // in Mhz
	const uint32_t mem_frequency = fsb_frequency * 4;
	uint32_t cpu_clock_multiplier = 4;
	double cpu_ipc = 1.0; // in instructions per cycle

	
	// Parse the command line arguments
	while((c = getopt_long (argc, argv, "dg:a:hi:zeoml:", long_options, 0)) != -1) {
		switch(c) {
		case 'd':
			use_inline_debugger = true;
			break;
		case 'g':
			use_gdb_server = true;
			gdb_server_tcp_port = atoi(optarg);
			break;
		case 'a':
			gdb_server_arch_filename = optarg;
			break;
		case 'h':
			help(argv[0]);
			return 0;
		case 'i':
			maxinst = strtoull(optarg, 0, 0);
			break;
		case 'l':
			logger_filename = optarg;
			break;
		case 'z':
			logger_zip = true;
			break;
		case 'e':
			logger_error = true;
			break;
		case 'o':
			logger_out = true;
			break;
		case 'm':
			logger_messages = true;
			break;
		}
	}
	logger_on = logger_error || logger_out || (logger_filename != 0);

	if(optind >= argc) {
		help(argv[0]);
		return 0;
	}

	char *filename = argv[optind];
	int sim_argc = argc - optind;
	char **sim_argv = argv + optind;
	char **sim_envp = envp;

	if(!filename) {
		help(argv[0]);
		return 0;
	}

	// Logger
	LoggerServer *logger = 0;
	if(logger_on) {
		logger = new LoggerServer("logger");
		(*logger)["show-file"] = true;
		(*logger)["show-function"] = true;
		(*logger)["show-line"] = true;
	}
	
	// Time
	ScTime *time = new ScTime("time");
	
	if(logger_on)
		logger->time_import >> time->time_export;

	Elf32Loader *elf32_loader = 0;
	LinuxLoader<FSB_ADDRESS_TYPE> *linux_loader = 0;
	LinuxOS<CPU_ADDRESS_TYPE, CPU_REG_TYPE> *linux_os = 0;
	SymbolTable<CPU_ADDRESS_TYPE> *symbol_table = 0;
	unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> *memory = 
		new unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE>("memory");
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server = 
		use_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger = 
		use_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	unisim::component::tlm::bridge::simple_fsb_to_mem::Bridge<BRIDGE_CONFIG> *bridge =
		new unisim::component::tlm::bridge::simple_fsb_to_mem::Bridge<BRIDGE_CONFIG>("bridge");
	unisim::component::tlm::processor::arm::ARM<CPU_CONFIG> *cpu =
		new unisim::component::tlm::processor::arm::ARM<CPU_CONFIG>("cpu"); 

	BusMsgSpyType *bus_msg_spy = NULL;
	MemMsgSpyType *mem_msg_spy = NULL;
	if(logger_on && logger_messages) {
		bus_msg_spy = new BusMsgSpyType("bus-msg-spy");
		mem_msg_spy = new MemMsgSpyType("mem-msg-spy");
	}
	// In Linux mode, the system is not entirely simulated.
	// This mode allows to run Linux applications without simulating all the peripherals.
	// Every Linux system calls are caught and translated in host system calls.
	
	// Instanciate an ELF32 loader
	elf32_loader = new Elf32Loader("elf32-loader");
	
	// Instanciate a Linux program loader
	linux_loader = new LinuxLoader<FSB_ADDRESS_TYPE>("linux-loader");
	
	// Instanciate the Linux syscall translator
	linux_os = new LinuxOS<CPU_ADDRESS_TYPE, CPU_REG_TYPE>("linux-os");
	
	// Instanciate a symbol table to be filled-in by the ELF32 loader
	symbol_table = new SymbolTable<CPU_ADDRESS_TYPE>("symbol_table");

	// if the following line ("cpu-frequency") is commented, the cpu will use the power estimators to find max cpu frequency
	(*cpu)["cpu-cycle-time"] = (uint64_t)(250000.0/(double)fsb_frequency);
//	(*cpu)["cpu-frequency"] = cpu_clock_multiplier * fsb_frequency; // Mhz
	(*cpu)["bus-cycle-time"] = (uint64_t)(1000000.0/(double)fsb_frequency);
//	(*cpu)["bus-frequency"] = fsb_frequency;
	(*cpu)["nice-time"] = (uint64_t)(10000000.0/(double)fsb_frequency);
//	(*cpu)["voltage"] = 1.3 * 1e3; // mV
	(*cpu)["ipc"] = cpu_ipc;

	(*bridge)["fsb-cycle-time"] = (uint64_t)(1000000.0/(double)fsb_frequency);
	(*bridge)["mem-cycle-time"] = (uint64_t)(1000000.0/(double)mem_frequency);

	(*memory)["frequency"] = mem_frequency;

	if(maxinst)
	{
		(*cpu)["max-inst"] = maxinst;
	}

	// setting verbose parameters
	// (*cpu)["verbose-all"] = true;
	// (*cpu)["verbose-setup"] = true;
	(*cpu)["verbose-step"] = true;
	// (*cpu)["verbose-tlm-bus-synchronize"] = true;
	// (*cpu)["verbose-tlm-run-thread"] = true;
	// (*cpu)["verbose-tlm-commands"] = true;
	

	if(logger_on && logger_messages) {
		(*bus_msg_spy)["source_module_name"] = cpu->name();
		(*bus_msg_spy)["source_port_name"] = cpu->master_port.name();
		(*bus_msg_spy)["target_module_name"] = bridge->name();
		(*bus_msg_spy)["target_port_name"] = bridge->slave_port.name();
		(*mem_msg_spy)["source_module_name"] = bridge->name();
		(*mem_msg_spy)["source_port_name"] = bridge->master_port.name();
		(*mem_msg_spy)["target_module_name"] = memory->name();
		(*mem_msg_spy)["target_port_name"] = memory->slave_port.name();
	}
	
	// Connect the CPU to the Front Side Bus
	if(logger_on && logger_messages) {
		cpu->master_port(bus_msg_spy->slave_port);
		bus_msg_spy->master_port(bridge->slave_port);
		bridge->master_port(mem_msg_spy->slave_port);
		mem_msg_spy->master_port(memory->slave_port);
	} else {
		cpu->master_port(bridge->slave_port);
		bridge->master_port(memory->slave_port);
	}
	cpu->memory_import >> bridge->memory_export;
	
	if(logger_on) {
		unsigned int logger_index = 0;
		cpu->logger_import >> *logger->logger_export[logger_index++];
		bridge->logger_import >> *logger->logger_export[logger_index++];
		linux_os->logger_import >> *logger->logger_export[logger_index++];
		if(logger_messages) {
			bus_msg_spy->logger_import >> *logger->logger_export[logger_index++];
			mem_msg_spy->logger_import >> *logger->logger_export[logger_index++];
		}
	}
	
	if(inline_debugger)
	{
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
	}
	else if(gdb_server)
	{
		// GDB Server run-time configuration
		(*gdb_server)["tcp-port"] = gdb_server_tcp_port;
		(*gdb_server)["architecture-description-filename"] = gdb_server_arch_filename;
		
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
	}

	// logger parameters
	if(logger_on) {
		if(logger_filename) {
			(*logger)["filename"] = logger_filename;
			(*logger)["zip"] = logger_zip;
		}
		(*logger)["std_out"] = logger_out;
		(*logger)["std_err"] = logger_error;
	}

	// Memory run-time configuration
	(*memory)["org"] = 0x00000000UL;
	(*memory)["bytesize"] = (uint32_t)-1;

	// ELF32 Loader run-time configuration
	(*elf32_loader)["filename"] = filename;

//	// Linux system calls translator run-time configuration
//	(*arm_linux_os)["endianess"] = E_BIG_ENDIAN;
//	(*arm_linux_os)["memory-page-size"] = 4096;

	(*linux_os)["system"] = "arm";
	(*linux_os)["endianess"] = E_BIG_ENDIAN;
	(*linux_os)["verbose"] = false;

	// Linux loader run-time configuration
	(*linux_loader)["endianess"] = E_BIG_ENDIAN;
	(*linux_loader)["stack-base"] = 0xc0000000;
	(*linux_loader)["max-environ"] = 16 * 1024;
	(*linux_loader)["argc"] = sim_argc;
	for(unsigned int i = 0; i < sim_argc; i++)
	{
		(*linux_loader)["argv"][i] = sim_argv[i];
		cerr << sim_argv[i] << endl;
	}
	(*linux_loader)["envc"] = 0;

	// Connect everything
	elf32_loader->memory_import >> memory->memory_export;
	elf32_loader->symbol_table_build_import >> symbol_table->symbol_table_build_export;
	linux_loader->memory_import >> memory->memory_export;
	linux_loader->loader_import >> elf32_loader->loader_export;
	cpu->linux_os_import >> linux_os->linux_os_export;
	linux_os->cpu_linux_os_import >> cpu->cpu_linux_os_export;
	linux_os->memory_import >> cpu->memory_export;
	linux_os->registers_import >> cpu->registers_export;
	linux_os->loader_import >> linux_loader->loader_export;
//	arm_linux_os->linux_loader_import >> linux_loader->loader_export;
//	arm_linux_os->linux_os_import >> linux_os->linux_os_export;
//	linux_os->cpu_linux_os_import >> arm_linux_os->cpu_linux_os_export;
//	linux_os->endian_import >> arm_linux_os->endian_export;
//	linux_os->memory_import >> cpu->memory_export;
//	arm_linux_os->cpu_arm_linux_os_import >> cpu->cpu_arm_linux_os_export;

	cpu->symbol_table_lookup_import >> symbol_table->symbol_table_lookup_export;
	bridge->memory_import >> memory->memory_export;

	/* WARNING!!!! the bus request ports are not connected !!!!! */
//	(*bus->chipset_outport)(memory->bus_port);

	if(use_inline_debugger)
	{
		inline_debugger->symbol_table_lookup_import >> 
			symbol_table->symbol_table_lookup_export;
	}
	
#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at user privilege level (Linux system calls translation enabled)" << endl;

		struct tms time_start, time_stop;
		double ratio;
		clock_t utime;
		clock_t stime;
		double spent_time;

		times(&time_start);

		EnableDebug();
		void (*prev_sig_int_handler)(int);

		if(!inline_debugger)
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

		if(!inline_debugger)
		{
			signal(SIGINT, prev_sig_int_handler);
		}

		cerr << "Simulation finished" << endl;
		cerr << "Simulation statistics:" << endl;

		times(&time_stop);
		ratio= 1.0 / sysconf(_SC_CLK_TCK);
		utime = time_stop.tms_utime - time_start.tms_utime;
		stime = time_stop.tms_stime - time_start.tms_stime;
		spent_time = ratio * (utime + stime);

		cerr << "simulation time: " << spent_time << " seconds" << endl;
		cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
		cerr << "simulated instructions : " << cpu->GetInstructionCounter() << " instructions" << endl;
		cerr << "host simulation speed: " << ((double) cpu->GetInstructionCounter() / spent_time / 1000000.0) << " MIPS" << endl;
		cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
	}


	if(elf32_loader) delete elf32_loader;
	if(linux_loader) delete linux_loader;
	if(linux_os) delete linux_os;
	if(symbol_table) delete symbol_table;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(bridge) delete bridge;
	if(cpu) delete cpu;
	if(time) delete time;
	if(logger) delete logger;
	if(bus_msg_spy) delete bus_msg_spy;
	if(mem_msg_spy) delete mem_msg_spy;

	return 0;
}
