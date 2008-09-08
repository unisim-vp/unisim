/*
 *  Copyright (c) 2008,
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
 * Authors: Reda Nouacer (reda.nouacer@cea.fr)
 */
 

#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/elf_loader/elf_loader.hh>
#include <unisim/service/loader/s19_loader/s19_loader.hh>
#include <unisim/service/debug/symbol_table/symbol_table.hh>
#include <iostream>
#include <getopt.h>
#include <unisim/kernel/service/service.hh>
#include <stdlib.h>
#include <unisim/component/tlm/processor/hcs12x/hcs12x.hh>
#include <unisim/component/tlm/memory/ram/memory.hh>
//#include <unisim/component/tlm/fsb/snooping_bus/bus.hh>
#include <unisim/component/tlm/fsb/simple_bus/bus.hh>
#include <unisim/util/garbage_collector/garbage_collector.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/logger/logger_server.hh>
#include <unisim/service/loader/s19_loader/s19_loader.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/loader.hh>

#include <stdexcept>
#include <unisim/component/tlm/debug/transaction_spy.hh>

#include <unisim/component/tlm/bridge/simple_fsb_to_mem/bridge.hh>
#include <unisim/component/tlm/bridge/simple_fsb_to_mem/config.hh>

#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif


//static const bool DEBUG_INFORMATION = false;

bool debug_enabled;

void EnableDebug()
{
	debug_enabled = true;
}

void DisableDebug()
{
	debug_enabled = false;
}

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}



using namespace std;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;

using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::loader::s19_loader::S19_Loader;
using unisim::service::debug::symbol_table::SymbolTable;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::logger::LoggerServer;
using unisim::kernel::service::Service;
using unisim::service::interfaces::Loader;

using unisim::kernel::service::ServiceManager;

void help(char *prog_name)
{
	cerr << "Usage: " << prog_name << " [<options>] <program> [program arguments]" << endl << endl;
	cerr << "       For using ElfLoader, 'program' is statically linked ELF32 HCS12X Linux program" << endl << endl;
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
	cerr << "-p" << endl;
	cerr << "--power" << endl;
	cerr << "            estimate power consumption of caches and TLBs" << endl << endl;
	cerr << "-l <file>" << endl;
	cerr << "--logger:file <file>" << endl;
	cerr << "            store log file in <file>" << endl << endl;
	cerr << "-z" << endl;
	cerr << "--logger:zip" << endl;
	cerr << "            zip log file" << endl << endl;
	cerr << "-e" << endl;
	cerr << "--logger:error" << endl;
	cerr << "            pipe the log into the standard error" << endl << endl;
	cerr << "-o" << endl;
	cerr << "--logger:out" << endl;
	cerr << "            pipe the log into the standard output" << endl << endl;
	cerr << "-c <mode>" << endl;
	cerr << "--compiler-memory-modele <mode>" << endl;
	cerr << "            indicate the memory map modele to use for interpreting addresses" << endl;
	cerr << "            0: banked modele (default)" << endl;
	cerr << "            1: linear modele (recommended by Motorola/Freescale)" << endl;
	cerr << "            2: gnu/gcc modele " << endl << endl;
	cerr << "--help" << endl;
	cerr << "-h" << endl;
	cerr << "            displays this help" << endl;
}

//=========================================================================
//===                       Constants definitions                       ===
//=========================================================================

// Front Side Bus template parameters

typedef unisim::component::cxx::processor::hcs12x::physical_address_t CPU_ADDRESS_TYPE;
typedef unisim::component::cxx::processor::hcs12x::physical_address_t FSB_ADDRESS_TYPE;
typedef unisim::component::cxx::processor::hcs12x::physical_address_t MEMORY_ADDRESS_TYPE;
typedef unisim::component::cxx::processor::hcs12x::reg_t CPU_REG_TYPE;
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes (WARNING! should be 2)
const uint32_t FSB_NUM_PROCS = 1;

// the maximum number of transaction spies (per type of message)
const unsigned int MAX_BUS_TRANSACTION_SPY = 3;
const unsigned int MAX_MEM_TRANSACTION_SPY = 1;

//=========================================================================
//===                     Aliases for components classes                ===
//=========================================================================

typedef unisim::component::tlm::fsb::simple_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1> FRONT_SIDE_BUS;
typedef unisim::component::tlm::bridge::simple_fsb_to_mem::Bridge<unisim::component::tlm::bridge::simple_fsb_to_mem::Addr32BurstSize32_Config> FSB_TO_MEM_BRIDGE;
typedef unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
typedef unisim::component::tlm::processor::hcs12x::HCS12X CPU;

//=========================================================================
//===               Aliases for transaction Spies classes               ===
//=========================================================================

typedef unisim::component::tlm::fsb::simple_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1>::ReqType BusMsgReqType;
typedef unisim::component::tlm::fsb::simple_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1>::RspType BusMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<BusMsgReqType, BusMsgRspType> BusMsgSpyType;
typedef unisim::component::tlm::message::MemoryRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MemMsgReqType;
typedef unisim::component::tlm::message::MemoryResponse<FSB_MAX_DATA_SIZE> MemMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<MemMsgReqType, MemMsgRspType> MemMsgSpyType;

extern char **environ;

int sc_main(int argc, char *argv[])
{
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
	{"compiler-memory-modele", required_argument, 0, 'c'},
	{0, 0, 0, 0}
	};

	int c;
	bool use_gdb_server = false;
	bool use_inline_debugger = false;
	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_hcs12x.xml";
	uint64_t maxinst = 0; // maximum number of instruction to simulate
	char *logger_filename = 0;
	bool logger_zip = false;
	bool logger_error = false;
	bool logger_out = false;
	bool logger_on = false;
	bool logger_messages = false;
	double cpu_frequency = 40.0; // in Mhz
	uint32_t cpu_clock_multiplier = 1;  
//	double cpu_ipc = 1.0; // in instructions per cycle
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;

	int memoryMode = S19_Loader::BANKED;
	
	// Parse the command line arguments
	while((c = getopt_long (argc, argv, "c:dg:a:hi:l:zeom", long_options, 0)) != -1)
	{
		switch(c)
		{
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
			case 'c':
				memoryMode = atoi(optarg);
				break;
		}
	}
	logger_on = logger_error || logger_out || (logger_filename != 0);

	if(optind >= argc)
	{
		help(argv[0]);
		return 0;
	}

	char *filename = argv[optind];
	int sim_argc = argc - optind;
	char **sim_argv = argv + optind;
	char **sim_envp = environ;

	// If no filename has been specified then display the help
	if(filename == NULL)
	{
		help(argv[0]);
		return 0;
	}
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor
	CPU *cpu =new CPU("cpu");
	//  - Front side bus
	FRONT_SIDE_BUS *bus = new FRONT_SIDE_BUS("bus");
	//  - Front side bus to memory bridge
	FSB_TO_MEM_BRIDGE *fsb_to_mem_bridge = new FSB_TO_MEM_BRIDGE("fsb-to-mem-bridge");
	//  - RAM
	MEMORY *memory = new MEMORY("memory");

	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================
	BusMsgSpyType *bus_msg_spy[MAX_BUS_TRANSACTION_SPY];
	MemMsgSpyType *mem_msg_spy[MAX_MEM_TRANSACTION_SPY];

	if(logger_on && logger_messages)
	{
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++)
		{
			stringstream sstr;
			sstr << "bus_msg_spy[" << i << "]";
			string name = sstr.str();
			bus_msg_spy[i] = new BusMsgSpyType(name.c_str());
		}
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++)
		{
			stringstream sstr;
			sstr << "mem_msg_spy[" << i << "]";
			string name = sstr.str();
			mem_msg_spy[i] = new MemMsgSpyType(name.c_str());
		}
	}
	else
	{
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++) bus_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++) mem_msg_spy[i] = 0;
	}

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	
	bool isS19 = false;
	cout << filename << "\n";
	if ((strstr(filename, ".s19") != NULL) ||
		 (strstr(filename, ".S19") != NULL))  {
		isS19 = true;
	}

	Service<Loader<physical_address_t> > *loader = NULL;
	
	if (isS19) {
		loader = new S19_Loader("S19_Loader", (S19_Loader::MODE) memoryMode);
	} else {
		loader = new Elf32Loader("elf32-loader");
	}
	
		
	//  - Symbol table
	SymbolTable<CPU_ADDRESS_TYPE> *symbol_table = new SymbolTable<CPU_ADDRESS_TYPE>("symbol-table");
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server = use_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger = use_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Logger
	LoggerServer *logger = logger_on ? new LoggerServer("logger") : 0;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - Front Side Bus
	(*bus)["cycle-time"] = fsb_cycle_time;

	//  - 68HCS12X processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	(*cpu)["cpu-cycle-time"] = cpu_cycle_time;
	(*cpu)["bus-cycle-time"] = fsb_cycle_time;
	if(maxinst)
	{
		(*cpu)["max-inst"] = maxinst;
	}

	//  - Front side bus to memory bridge
	(*fsb_to_mem_bridge)["fsb-cycle-time"] = fsb_cycle_time;
	(*fsb_to_mem_bridge)["mem-cycle-time"] = mem_cycle_time;
	//  - RAM
	(*memory)["cycle-time"] = mem_cycle_time;
	(*memory)["org"] = 0x00000000UL;
	(*memory)["bytesize"] = (uint32_t)-1; //effective memory size is 8Mo

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	if(gdb_server)
	{
		(*gdb_server)["tcp-port"] = gdb_server_tcp_port;
		(*gdb_server)["architecture-description-filename"] = gdb_server_arch_filename;
	}

	(*loader)["filename"] = filename;
	
	//  - Loggers
	if(logger_on)
	{
		if(logger_filename)
		{
			(*logger)["filename"] = logger_filename;
			(*logger)["zip"] = logger_zip;
		}
		(*logger)["std_out"] = logger_out;
		(*logger)["std_err"] = logger_error;
		(*logger)["show-file"] = true;
		(*logger)["show-function"] = true;
		(*logger)["show-line"] = true;
		(*logger)["show-time"] = true;
	}


	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	if(logger_on && logger_messages)
	{
		unsigned bus_msg_spy_index = 0;
		unsigned mem_msg_spy_index = 0;
// TODO: 
		cpu->master_port(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = cpu->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = cpu->master_port.name();
		bus_msg_spy[bus_msg_spy_index]->master_port(*bus->inport[0]);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = bus->inport[0]->name();
		bus_msg_spy_index++;

		(*bus->chipset_outport)(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->chipset_outport->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(fsb_to_mem_bridge->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = fsb_to_mem_bridge->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = fsb_to_mem_bridge->slave_port.name();
		bus_msg_spy_index++;

		fsb_to_mem_bridge->master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = fsb_to_mem_bridge->name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = fsb_to_mem_bridge->master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(memory->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = memory->name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = memory->slave_port.name();
		mem_msg_spy_index++;
	}
	else
	{
		cpu->master_port(*bus->inport[0]);
		(*bus->chipset_outport)(fsb_to_mem_bridge->slave_port);
		fsb_to_mem_bridge->master_port(memory->slave_port);
	}

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> bus->memory_export;
	
	if(inline_debugger)
	{
		// Connect inline-debugger to CPU
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
		inline_debugger->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
	}
	else if(gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >>
		cpu->memory_access_reporting_control_export;
	}

	if (isS19) {
		((S19_Loader *) loader)->memory_import >> memory->memory_export;
		((S19_Loader *) loader)->symbol_table_build_import >> symbol_table->symbol_table_build_export;
	} else {
		((Elf32Loader *) loader)->memory_import >> memory->memory_export;
		((Elf32Loader *) loader)->symbol_table_build_import >> symbol_table->symbol_table_build_export;
	}
	
	cpu->symbol_table_lookup_import >> symbol_table->symbol_table_lookup_export;
	bus->memory_import >> fsb_to_mem_bridge->memory_export;
	fsb_to_mem_bridge->memory_import >> memory->memory_export;

	if(inline_debugger)
	{
		inline_debugger->symbol_table_lookup_import >> symbol_table->symbol_table_lookup_export;
	}
	
	/* logger connections */
	if(logger_on) {
		unsigned int logger_index = 0;
		logger->time_import >> time->time_export;
		cpu->logger_import >> *logger->logger_export[logger_index++];
		bus->logger_import >> *logger->logger_export[logger_index++];
		fsb_to_mem_bridge->logger_import >> *logger->logger_export[logger_index++];
		memory->logger_import >> *logger->logger_export[logger_index++];
		if(gdb_server) gdb_server->logger_import >> *logger->logger_export[logger_index++];
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++)
			if(bus_msg_spy[i] != NULL)
				bus_msg_spy[i]->logger_import >> *logger->logger_export[logger_index++];
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++)
			if(mem_msg_spy[i] != NULL)
				mem_msg_spy[i]->logger_import >> *logger->logger_export[logger_index++];
	}

#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(ServiceManager::Setup())
	{

		physical_address_t entry_point = loader->GetEntryPoint();
		address_t cpu_address;
		uint8_t page = 0;
		
		if (isS19) {
			((S19_Loader *) loader)->GetPagedAddress(entry_point, page, cpu_address); 
		} else {
			cpu_address = (address_t) entry_point;
		} 
		
		cpu->SetEntryPoint(page, cpu_address);
		
		cerr << "Starting simulation ..." << endl;

		double time_start = host_time->GetTime();

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

		double time_stop = host_time->GetTime();
		double spent_time = time_stop - time_start;

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

	for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++) if(bus_msg_spy[i]) delete bus_msg_spy[i];
	for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++) if(mem_msg_spy[i]) delete mem_msg_spy[i];

	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(bus) delete bus;
	if(cpu) delete cpu;
	if(fsb_to_mem_bridge) delete fsb_to_mem_bridge;
	if(time) delete time;
	if(logger) delete logger;

	if(loader) delete loader;

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
