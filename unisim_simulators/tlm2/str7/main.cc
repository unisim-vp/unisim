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
 
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>

#include "unisim/kernel/service/service.hh"

#include "unisim/component/cxx/processor/arm/config.hh"
#include "unisim/component/tlm2/processor/arm/arm.hh"
#include "unisim/component/tlm2/memory/ram/memory.hh"
#include "unisim/component/tlm2/interconnect/generic_router/router.hh"

#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/debug/symbol_table/symbol_table.hh"
#include "unisim/service/loader/elf_loader/elf_loader.hh"
#include "unisim/service/loader/elf_loader/elf_loader.tcc"

#include "unisim/service/debug/symbol_table/symbol_table.hh"

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef STR7_DEBUG 
	typedef unisim::component::cxx::processor::arm::ARM7TDMI_DebugConfig CPU_CONFIG;
#elif STR7_DEBUG_INLINE
	typedef unisim::component::cxx::processor::arm::ARM7TDMI_DebugConfig CPU_CONFIG;
#elif STR7_VERBOSE
	typedef unisim::component::cxx::processor::arm::ARM7TDMI_DebugConfig CPU_CONFIG;
#else
	typedef unisim::component::cxx::processor::arm::ARM7TDMI_Config CPU_CONFIG;
#endif
	
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
#ifdef STR7_DEBUG
using unisim::service::debug::gdb_server::GDBServer;
#endif
#ifdef STR7_DEBUG_INLINE
using unisim::service::debug::inline_debugger::InlineDebugger;
#endif

using unisim::service::debug::symbol_table::SymbolTable;

typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> ElfLoader;
typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, true> MEMORY;
typedef unisim::component::tlm2::interconnect::generic_router::Router<> ROUTER;

using unisim::service::time::sc_time::ScTime;
using unisim::service::time::host_time::HostTime;
using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;

void help(char *prog_name) {
	cerr << "Usage: " << prog_name << " [<options>] <binary to simulate>" << endl << endl;
	cerr << "Options:" << endl;
	cerr << " --help" << endl;
	cerr << " -h" << endl;
	cerr << "            displays this help" << endl << endl;
	cerr << " --get-variables <xml file>" << endl;
	cerr << " -v <xml file>" << endl;
	cerr << "            get the simulation default configuration variables on a xml file" << endl << endl;
	cerr << " --config <xml file>" << endl;
	cerr << " -c <xml file>" << endl;
	cerr << "            configures the simulator with the given xml configuration file" << endl << endl;
	cerr << " --get-config <xml file>" << endl;
	cerr << " -g <xml file>" << endl;
	cerr << "            get the simulator default configuration xml file (you can use it to create your own configuration)" << endl << endl;
//	cerr << " --xml-gdb <file>" << endl;
//	cerr << " -x <file>" << endl;
//	cerr << "            processor xml description file for gdb" << endl << endl;
//	cerr << " --gdb-server <port_number>" << endl;
//	cerr << " -d <port_number>" << endl;
//	cerr << "            activate the gdb server and use the given port" << endl << endl;
//	cerr << " --inline-debugger" << endl;
//	cerr << " -i" << endl;
//	cerr << "            activate the inline debugger (only active if logger option used)" << endl << endl;
}

// Front Side Bus template parameters
typedef CPU_CONFIG::address_t FSB_ADDRESS_TYPE;
typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;

int sc_main(int argc, char *argv[]) {

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
		{"help", no_argument, 0, 'h'},
		{"get-variables", required_argument, 0, 'v'},
		{"get-config", required_argument, 0, 'g'},
		{"config", required_argument, 0, 'c'},
//		{"logger", no_argument, 0, 'l'},
//		{"xml-gdb", required_argument, 0, 'x'},
//		{"gdb-server", required_argument, 0, 'd'},
//		{"inline-debugger", no_argument, 0, 'i'},
		{0, 0, 0, 0}
	};

	char const *set_config_name = "default_parameters.xml";
	char const *get_config_name = "default_parameters.xml";
	char const *get_variables_name = "default_variables.xml";
//	char *filename = 0;
	bool get_variables = false;
	bool get_config = false;
	bool set_config = false;
//	bool use_logger = false;
//	bool use_gdb_server = false;
//	char *gdb_xml = 0;
//	int gdb_server_port = 0;
//	bool use_inline_debugger = false;
	

	// Parse the command line arguments
	int c;
//	while((c = getopt_long (argc, argv, "hv:g:c:ld:x:i", long_options, 0)) != -1) {
	while((c = getopt_long (argc, argv, "hv:g:c:", long_options, 0)) != -1) {
		switch(c) {
		case 'h':
			help(argv[0]);
			return 0;
			break;
		case 'v':
			get_variables_name = optarg;
			get_variables = true;
			break;
		case 'g':
			get_config_name = optarg;
			get_config = true;
			break;
//		case 'x':
//			gdb_xml = optarg;
//			break;
		case 'c':
			set_config_name = optarg;
			set_config = true;
			break;
//		case 'l':
//			use_logger = true;
//			break;
//		case 'd':
//			gdb_server_port = atoi(optarg);
//			use_gdb_server = true;
//			break;
//		case 'i':
//			use_inline_debugger = true;
//			break;
		}
	}

	if(optind != argc) {
		cerr << "Error? (optind = " << optind << ", argc = " << argc <<" )" << endl;
		help(argv[0]);
		return 0;
	}

//	filename = argv[optind];

	// // Logger
	// LoggerServer *logger = 0;
	// if(use_logger)
	// 	logger = new LoggerServer("logger");
	
	// Time
	ScTime *time = new ScTime("time");
	HostTime *host_time = new HostTime("host-time");
	
	// if(use_logger)
	// 	logger->time_import >> time->time_export;

	ElfLoader *elf_loader = 0;
	MEMORY *memory = new MEMORY("memory");
	ROUTER *router = new ROUTER("router");
	unisim::component::tlm2::processor::arm::ARM<CPU_CONFIG, true> *cpu =
		new unisim::component::tlm2::processor::arm::ARM<CPU_CONFIG, true>("cpu"); 

	// Instanciate an ELF32 loader
	elf_loader = new ElfLoader("elf-loader");
	
#ifdef STR7_DEBUG
	GDBServer<uint64_t> *gdb_server = 
		new GDBServer<uint64_t>("gdb-server");
#endif // STR7_DEBUG
#ifdef STR7_DEBUG_INLINE
	InlineDebugger<uint64_t> *inline_debugger = 
		new InlineDebugger<uint64_t>("inline-debugger");
#endif // STR7_DEBUG_INLINE
	// Instanciate a symbol table to be filled-in by the ELF32 loader
	SymbolTable<uint64_t> *symbol_table = new SymbolTable<uint64_t>("symbol_table");

	// Connect the CPU to the memory
	// cpu->master_socket(memory->slave_sock);
	cpu->master_socket(router->targ_socket);
	router->init_socket(memory->slave_sock);
	cpu->memory_import >> memory->memory_export;

	// Connect everything
	elf_loader->memory_import >> memory->memory_export;
	elf_loader->symbol_table_build_import >> symbol_table->symbol_table_build_export;

	cpu->symbol_table_lookup_import >> symbol_table->symbol_table_lookup_export;

#ifdef STR7_DEBUG
	cpu->debug_control_import >> gdb_server->debug_control_export;
	cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
	gdb_server->memory_import >> cpu->memory_export;
	gdb_server->registers_import >> cpu->registers_export;
	gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
#endif // STR7_DEBUG

#ifdef STR7_DEBUG_INLINE
	cpu->debug_control_import >> inline_debugger->debug_control_export;
	cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
	inline_debugger->disasm_import >> cpu->disasm_export;
	inline_debugger->memory_import >> cpu->memory_export;
	inline_debugger->registers_import >> cpu->registers_export;
	inline_debugger->symbol_table_lookup_import >> 
		symbol_table->symbol_table_lookup_export;
#endif // STR7_DEBUG_INLINE

#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(set_config)
		ServiceManager::LoadXmlParameters(set_config_name);
	if(get_config)
	{
		cerr << "getting simulator configuration in \"" << get_config_name << "\"" << endl;
		ServiceManager::XmlfyParameters(get_config_name);
		return 0;
	}
	if(!set_config) {
		if(!get_config) help(argv[0]);
		return 0;
	}

/*	if(use_gdb_server) {
		cerr << "gdb_server_port = " << gdb_server_port << endl;
		VariableBase *var =	ServiceManager::GetParameter("gdb-server.tcp-port");
		*var = gdb_server_port;
		if(gdb_xml != 0) {
			cerr << "gdb_xml = " << gdb_xml << endl;
			var = ServiceManager::GetParameter("gdb-server.architecture-description-filename");
			*var = gdb_xml;
		}
	}
*/

	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at system privilege level" << endl;

		double time_start = host_time->GetTime();

		EnableDebug();
		void (*prev_sig_int_handler)(int);

#ifndef STR7_DEBUG_INLINE
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
#endif // not STR7_DEBUG_INLINE

		try
		{
			sc_start();
		}
		catch(std::runtime_error& e)
		{
			cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
			cerr << e.what() << endl;
		}
		
#ifdef STR7_DEBUG_INLINE
		signal(SIGINT, prev_sig_int_handler);
#endif // STR7_DEBUG_INLINE

		cerr << "Simulation finished" << endl;
		cerr << "Simulation statistics:" << endl;

		double time_stop = host_time->GetTime();
		double spent_time = time_stop - time_start;

		cerr << "simulation time: " << spent_time << " seconds" << endl;
		cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
		cerr << "simulated instructions : " << cpu->GetInstructionCounter() << " instructions" << endl;
		cerr << "host simulation speed: " << ((double) cpu->GetInstructionCounter() / spent_time / 1000000.0) << " MIPS" << endl;
		cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
		if(get_variables)
		{
			cerr << "getting variables" << endl;
			ServiceManager::XmlfyVariables(get_variables_name);
		}
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
	}


	if(elf_loader) delete elf_loader;
	if(symbol_table) delete symbol_table;
	if(time) delete time;

	if(memory) delete memory;
	if(cpu) delete cpu;
	if(router) delete router;
	
#ifdef STR7_DEBUG
	if(gdb_server) delete gdb_server;
#endif // STR7_DEBUG
#ifdef STR7_DEBUG_INLINE
	if(inline_debugger) delete inline_debugger;
#endif // STR7_DEBUG_INLINE

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
