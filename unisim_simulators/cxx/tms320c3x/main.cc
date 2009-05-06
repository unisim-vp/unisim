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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr), Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <stdexcept>

#include "unisim/kernel/service/service.hh"
#include "unisim/component/cxx/processor/tms320/config.hh"
#include "unisim/component/cxx/processor/tms320/cpu.hh"
#include "unisim/component/cxx/memory/ram/memory.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/loader/coff_loader/coff_loader.hh"
#include "unisim/service/os/ti_c_io/ti_c_io.hh"

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef TMS320C3X_DEBUG
const bool CPU_DEBUG = true;
const bool MEMORY_DEBUG = true;
#else
const bool CPU_DEBUG = false;
const bool MEMORY_DEBUG = false;
#endif

// Front Side Bus template parameters
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;
typedef unisim::component::cxx::processor::tms320::TMS320VC33_Config CPU_CONFIG;

bool simulating;

class TMS320C3X : public unisim::component::cxx::processor::tms320::CPU<CPU_CONFIG, CPU_DEBUG>
{
public:
	TMS320C3X(const char *name, Object *parent = 0);
	virtual ~TMS320C3X();
	virtual void Stop(int ret);
};

TMS320C3X::TMS320C3X(const char *name, Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, unisim::component::cxx::processor::tms320::CPU<CPU_CONFIG, CPU_DEBUG>(name, parent)
{
}

TMS320C3X::~TMS320C3X()
{
}

void TMS320C3X::Stop(int ret)
{
	simulating = false;
}

typedef TMS320C3X CPU;
typedef unisim::service::loader::coff_loader::CoffLoader<uint64_t> LOADER;
typedef unisim::component::cxx::memory::ram::Memory<uint64_t> MEMORY;
typedef unisim::service::debug::gdb_server::GDBServer<uint64_t> GDB_SERVER;
typedef unisim::service::debug::inline_debugger::InlineDebugger<uint64_t> INLINE_DEBUGGER;
typedef unisim::service::time::host_time::HostTime HOST_TIME;
typedef unisim::service::os::ti_c_io::TI_C_IO<uint64_t> TI_C_IO;
using namespace std;
using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;




void SigIntHandler(int signum) {
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	simulating = false;
}

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
	cerr << "            get the simulator default configuration xml file (you can use it to create your own configuration)" << endl;
	cerr << "            This option can be combined with -c to get a new configuration file with existing variables from another file" << endl;
	cerr << " --logger" << endl;
	cerr << " -l" << endl;
	cerr << "            activate the logger" << endl << endl;
	cerr << " --xml-gdb <file>" << endl;
	cerr << " -x <file>" << endl;
	cerr << "            processor xml description file for gdb" << endl << endl;
	cerr << " --gdb-server <port_number>" << endl;
	cerr << " -d <port_number>" << endl;
	cerr << "            activate the gdb server and use the given port" << endl << endl;
	cerr << " --inline-debugger" << endl;
	cerr << " -i" << endl;
	cerr << "            activate the inline debugger (only active if logger option used)" << endl << endl;
}

int main(int argc, char *argv[]) {

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
		{"logger", no_argument, 0, 'l'},
		{"xml-gdb", required_argument, 0, 'x'},
		{"gdb-server", required_argument, 0, 'd'},
		{"inline-debugger", no_argument, 0, 'i'},
		{0, 0, 0, 0}
	};

	char const *set_config_name = "default_parameters.xml";
	char const *get_config_name = "default_parameters.xml";
	char const *get_variables_name = "default_variables.xml";
	char *filename = 0;
	bool get_variables = false;
	bool get_config = false;
	bool set_config = false;
	bool use_logger = false;
	bool use_gdb_server = false;
	char *gdb_xml = 0;
	int gdb_server_port = 0;
	bool use_inline_debugger = false;
	

	// Parse the command line arguments
	int c;
	while((c = getopt_long (argc, argv, "hv:g:c:ld:x:i", long_options, 0)) != -1) {
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
		case 'x':
			gdb_xml = optarg;
			break;
		case 'c':
			set_config_name = optarg;
			set_config = true;
			break;
		case 'l':
			use_logger = true;
			break;
		case 'd':
			gdb_server_port = atoi(optarg);
			use_gdb_server = true;
			break;
		case 'i':
			use_inline_debugger = true;
			break;
		}
	}

	if(optind != argc) {
		cerr << "Error? (optind = " << optind << ", argc = " << argc <<" )" << endl;
		help(argv[0]);
		return 0;
	}

	filename = argv[optind];

	// Time
	HOST_TIME *host_time = new HOST_TIME("host-time");
	
	LOADER *loader = 0;
	TI_C_IO *ti_c_io = 0;
	MEMORY *memory = new MEMORY("memory");
	GDB_SERVER *gdb_server = (use_gdb_server || get_config) ? new GDB_SERVER("gdb-server") : 0;
	INLINE_DEBUGGER *inline_debugger = (use_inline_debugger || get_config) ? new INLINE_DEBUGGER("inline-debugger") : 0;
	CPU *cpu = new CPU("cpu"); 

	// Instanciate an COFF loader
	loader = new LOADER("loader");
	
	ti_c_io = new TI_C_IO("ti-c-io");

	// Connect the CPU to the memory
	cpu->memory_import >> memory->memory_export;
	cpu->ti_c_io_import >> ti_c_io->ti_c_io_export;
	ti_c_io->memory_import >> cpu->memory_export;
	ti_c_io->memory_injection_import >> cpu->memory_injection_export;
	ti_c_io->registers_import >> cpu->registers_export;
	ti_c_io->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

	if(use_inline_debugger) {
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
	} else if(use_gdb_server) {
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}

	// Connect everything
	loader->memory_import >> memory->memory_export;

	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

	if(use_inline_debugger) {
		inline_debugger->symbol_table_lookup_import >> 
			loader->symbol_table_lookup_export;
	}
	
#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if (set_config)
	{
		ServiceManager::LoadXmlParameters(set_config_name);
		cerr << "Parameters set using file \"" << set_config_name << "\"" << endl;
	}
	if (get_config)
	{
		ServiceManager::XmlfyParameters(get_config_name);
		cerr << "Parameters saved on file \"" << get_config_name << "\"" << endl;
		return 0;
	}
	if (!set_config)
	{
		if (!get_config) help(argv[0]);
		return 0;
	}

	if(use_gdb_server) {
		cerr << "gdb_server_port = " << gdb_server_port << endl;
		VariableBase *var =	ServiceManager::GetParameter("gdb-server.tcp-port");
		*var = gdb_server_port;
		if(gdb_xml != 0) {
			cerr << "gdb_xml = " << gdb_xml << endl;
			var = ServiceManager::GetParameter("gdb-server.architecture-description-filename");
			*var = gdb_xml;
		}
	}
	
	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at system privilege level" << endl;

		double time_start = host_time->GetTime();

		void (*prev_sig_int_handler)(int);

		if(!use_inline_debugger)
			prev_sig_int_handler = signal(SIGINT, SigIntHandler);

		try
		{
			simulating = true;
			do
			{
				cpu->StepInstruction();
			} while(simulating);
		}
		catch(std::runtime_error& e)
		{
			cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
			cerr << e.what() << endl;
		}

		if(!use_inline_debugger)
			signal(SIGINT, prev_sig_int_handler);

		cerr << "Simulation finished" << endl;
		cerr << "Simulation statistics:" << endl;

		double time_stop = host_time->GetTime();
		double spent_time = time_stop - time_start;

		VariableBase *stat_instruction_counter = ServiceManager::GetVariable("cpu.instruction-counter");
		VariableBase *stat_insn_cache_hits = ServiceManager::GetVariable("cpu.insn-cache-hits");
		VariableBase *stat_insn_cache_misses = ServiceManager::GetVariable("cpu.insn-cache-misses");
		cerr << "simulation time: " << spent_time << " seconds" << endl;
		cerr << "simulated instructions : " << (uint64_t)(*stat_instruction_counter) << " instructions" << endl;
		cerr << "host simulation speed: " << ((double)(*stat_instruction_counter) / spent_time / 1000000.0) << " MIPS" << endl;
		cerr << "Insn cache hits: " << (uint64_t) *stat_insn_cache_hits << endl;
		cerr << "Insn cache misses: " << (uint64_t) *stat_insn_cache_misses << endl;
		cerr << "Insn cache miss rate: " << ((double) *stat_insn_cache_misses / (double) ((uint64_t) *stat_insn_cache_hits + (uint64_t) *stat_insn_cache_misses)) << endl;
		
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


	if(loader) delete loader;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(host_time) delete host_time;
	if(ti_c_io) delete ti_c_io;

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
