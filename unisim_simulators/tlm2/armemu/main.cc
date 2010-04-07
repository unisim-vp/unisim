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

/* possible configurations for the arm9tdmi:
- unisim::component::cxx::processor::arm::ARM9TDMI_NoCache_DebugConfig
- unisim::component::cxx::processor::arm::ARM9TDMI_DebugConfig
- unisim::component::cxx::processor::arm::ARM9TDMI_NoCache_Config
- unisim::component::cxx::processor::arm::ARM9TDMI_Config
*/

// typedef unisim::component::cxx::processor::arm::ARM926EJS_Config CPU_CONFIG;

//static const bool DEBUG_INFORMATION = true;

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

// TODO: REMOVE
#if 0
// Front Side Bus template parameters
// typedef CPU_CONFIG::address_t FSB_ADDRESS_TYPE;
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

#endif
// TODO: END REMOVE

using namespace std;
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
using unisim::service::time::sc_time::ScTime;
using unisim::service::time::host_time::HostTime;
using unisim::kernel::service::ServiceManager;

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
	cerr << "            get the simulator default configuration xml file (you "
			<< "can use it to create your own configuration)" << endl << endl;
	cerr << " --debug-gdb-server" << endl;
	cerr << " -d" << endl;
	cerr << "            enable debugging of the simulator using the gdb_server (if the inline debugger is used at the same time, this option will be ignored)" << endl << endl;
	cerr << " --debug-inline-debugger" << endl;
	cerr << " -i" << endl;
	cerr << "            enable debugging of the simulator using the inline debugger" << endl << endl;
	cerr << " --set <param=value>" << endl;
	cerr << " -s <param=value>" << endl;
	cerr << "            set value of parameter 'param' to 'value'" << endl << endl;
	cerr << " --list[<parameter name|object name>]" << endl;
	cerr << " -l[<parameter name|object name>]" << endl;
	cerr << "            if no parameter name is given lists all availabe parameters, their type, and their current value" << endl;
	cerr << "            if a parameter name is given displays the parameter type and its current value" << endl;
	cerr << "            if an object name is given lists all the object available parameters, their type, and their current value" << endl << endl;
	cerr << " --no-run" << endl;
	cerr << " -n" << endl;
	cerr << "            do not launch the simulator, only apply the options" << endl << endl;
	cerr << " --python-debugger" << endl;
	cerr << " -p" << endl;
	cerr << "            run in python mode" << endl;
}

// void LaunchPython()
// {
// 	int argc = 1;
// 	wchar_t *argv[3];
// 	const char *p1 = "python";
// 	const char *p2 = "-i";
// 	argv[0] = (wchar_t *)malloc(1000 * sizeof(wchar_t));
// 	argv[1] = 0; // (wchar_t *)malloc(1000 * sizeof(wchar_t));
// 	argv[2] = 0;
// 	mbsrtowcs(argv[0], &p1, 1000, 0);
// 	mbsrtowcs(argv[1], &p2, 1000, 0);
// 	
// 	Py_Initialize();
// 	PyRun_SimpleString("from time import time,ctime\n"
// 					   "print('Today is', ctime(time()))\n");
// 	Py_Main(argc, argv);
// 	Py_Finalize();
// 	
// }

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

int main(int argc, char *argv[], char **envp) {
	ShowVersion();
//	LaunchPython();

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
		{"debug-gdb-server", no_argument, 0, 'd'},
		{"debug-inline-debugger", no_argument, 0, 'i'},
		{"set", required_argument, 0, 's'},
		{"list", optional_argument, 0, 'l'},
		{"no-run", no_argument, 0, 'n'},
		{0, 0, 0, 0}
	};

	char const *set_config_name = "default_parameters.xml";
	char const *get_config_name = "default_parameters.xml";
	char const *get_variables_name = "default_variables.xml";
	bool get_variables = false;
	bool get_config = false;
	bool set_config = false;
	bool use_gdb_server = false;
	bool use_inline_debugger = false;
	std::list<string> set_vars;
	bool list_parms = false;
	std::list<string> list_parms_list;
	bool do_not_run = false;

	// Parse the command line arguments
	int c;
	while((c = getopt_long (argc, argv, "hv:g:c:dis:l::n", long_options, 0)) != -1) {
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
			case 'c':
				set_config_name = optarg;
				set_config = true;
				break;
			case 'd':
				use_gdb_server = true;
				break;
			case 'i':
				use_inline_debugger = true;
				break;
			case 's':
				set_vars.push_back(string(optarg));
				break;
			case 'l':
				if (optarg != 0)
					list_parms_list.push_back(string(optarg));
				else
					list_parms = true;
				break;
			case 'n':
				do_not_run = true;
				break;
		}
	}

	if(optind != argc) {
		cerr << "Error? (optind = " << optind << ", argc = " << argc <<" )" << endl;
		help(argv[0]);
		return 0;
	}

	// Time
	CPU *cpu = new CPU("cpu");
	IRQ_MASTER_STUB *irq_master_stub = new IRQ_MASTER_STUB("irq-master-stub");
	FIQ_MASTER_STUB *fiq_master_stub = new FIQ_MASTER_STUB("fiq-master-stub");
	MEMORY *memory = new MEMORY("memory");
	ScTime *time = new ScTime("time");
	HostTime *host_time = new HostTime("host-time");
	ELF32_LOADER *elf32_loader = new ELF32_LOADER("elf-loader");
	LINUX_LOADER *linux_loader = new LINUX_LOADER("linux-loader");
	LINUX_OS *linux_os = new LINUX_OS("linux-os");
	GDB_SERVER *gdb_server = 0;
	if (use_gdb_server || get_config)
		gdb_server = new GDB_SERVER("gdb-server");
	INLINE_DEBUGGER *inline_debugger = 0;
	if (use_inline_debugger || get_config)
		inline_debugger = new INLINE_DEBUGGER("inline-debugger");
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	POWER_ESTIMATOR *il1_power_estimator =
			new POWER_ESTIMATOR("il1-power-estimator");
	POWER_ESTIMATOR *dl1_power_estimator =
			new POWER_ESTIMATOR("dl1-power-estimator");
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	// now that all the components have been created, we can set the config and/or
	//   save it as required

	// First set the default configuration
	{
		unisim::kernel::service::VariableBase *var =
				ServiceManager::GetVariable("kernel_logger.std_err",
						unisim::kernel::service::VariableBase::VAR_PARAMETER);
		*var = true;
		var = ServiceManager::GetVariable("kernel_logger.std_err_color",
				unisim::kernel::service::VariableBase::VAR_PARAMETER);
		*var = true;

		(*cpu)["default-endianness"] = "little-endian";
		(*memory)["bytesize"] = 0xffffffffUL;
		(*memory)["cycle-time"] = 1000000UL;
		(*linux_loader)["stack-base"] = 0xc0000000UL;
		(*linux_loader)["max-environ"] = 0x4000UL;
		(*linux_loader)["default-endianess"] = "little-endian";

#ifdef SIM_POWER_ESTIMATOR_SUPPORT
		(*il1_power_estimator)["cache-size"] = 32 * 128;
		(*il1_power_estimator)["line-size"] = 32;
		(*il1_power_estimator)["associativity"] = 4;
		(*il1_power_estimator)["rw-ports"] = 0;
		(*il1_power_estimator)["excl-read-ports"] = 1;
		(*il1_power_estimator)["excl-write-ports"] = 0;
		(*il1_power_estimator)["single-ended-read-ports"] = 0;
		(*il1_power_estimator)["banks"] = 1;
		(*il1_power_estimator)["tech-node"] = 130; // in nm
		(*il1_power_estimator)["output-width"] = 32 * 8;
		(*il1_power_estimator)["tag-width"] = 32; // to fix
		(*il1_power_estimator)["access-mode"] = "fast";
		(*il1_power_estimator)["verbose"] = true;

		(*dl1_power_estimator)["cache-size"] = 32 * 128;
		(*dl1_power_estimator)["line-size"] = 32;
		(*dl1_power_estimator)["associativity"] = 4;
		(*dl1_power_estimator)["rw-ports"] = 1;
		(*dl1_power_estimator)["excl-read-ports"] = 0;
		(*dl1_power_estimator)["excl-write-ports"] = 0;
		(*dl1_power_estimator)["single-ended-read-ports"] = 0;
		(*dl1_power_estimator)["banks"] = 1;
		(*dl1_power_estimator)["tech-node"] = 130; // in nm
		(*dl1_power_estimator)["output-width"] = 32 * 8;
		(*dl1_power_estimator)["tag-width"] = 32; // to fix
		(*dl1_power_estimator)["access-mode"] = "fast";
		(*dl1_power_estimator)["verbose"] = true;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

		/* TODO REMOVE */
		(*elf32_loader)["filename"] = "test";
		(*linux_loader)["argc"] = 1;
		(*linux_loader)["argv[0]"] = "test";
		(*linux_os)["system"] = "arm";
		/* END TODO */
	}

	// First use the passed configuration file to set the configuration
	if (set_config)
	{
		cerr << "Setting simulator configuration in \"" << set_config_name << "\"." << endl;
		ServiceManager::LoadXmlParameters(set_config_name);
	}
	
	// Second overload the passed configuration with the command line arguments "-s param=value"
	std::list<string>::iterator set_vars_it;
	for (set_vars_it = set_vars.begin(); set_vars_it != set_vars.end(); set_vars_it++)
	{
		string varname_sstr;
		string value_sstr;
		
		unsigned int pos = set_vars_it->find("=");
		unsigned int len = set_vars_it->length();
		
		if (pos == 0 || pos == len)
		{
			cerr << "Ignoring " << *set_vars_it << endl;
			continue;
		}
		
		varname_sstr = set_vars_it->substr(0, pos);
		value_sstr = set_vars_it->substr(pos + 1);
		
		unisim::kernel::service::VariableBase *var =
				ServiceManager::GetVariable(varname_sstr.c_str(),
						unisim::kernel::service::VariableBase::VAR_PARAMETER);
		
		if (var == &ServiceManager::void_variable)
		{
			cerr << "WARNING! variable \"" << varname_sstr << "\" does not exist" << endl;
			continue;
		}
		
		cerr << "Using " << var->GetName() << " = \"" << value_sstr << "\"" << endl;
		*var = value_sstr.c_str();
	}
	
	
	// Dump the variables if requested
	if (list_parms)
			ServiceManager::DumpVariables(cerr, unisim::kernel::service::VariableBase::VAR_PARAMETER);
	else
		if (!list_parms_list.empty())
		{
			std::list<string>::iterator list_parms_it;
			list<unisim::kernel::service::VariableBase *> params;
			ServiceManager::GetParameters(params);
			list<unisim::kernel::service::VariableBase *>::iterator params_it;
			
			for (list_parms_it = list_parms_list.begin(); list_parms_it != list_parms_list.end(); list_parms_it++)
			{
				bool found = false;
				for (params_it = params.begin(); params_it != params.end(); params_it++)
				{
					if (string((*params_it)->GetName()).find((*list_parms_it), 0) != string::npos)
					{
						if (!found)
							cerr << "+ Listing \"" << *list_parms_it << "\" parameter(s)" << endl;
						found = true;
						const char *dt = (*params_it)->GetDataTypeName();
						const char *desc = (*params_it)->GetDescription();
						cerr << "   ";
						if(strlen(dt) != 0)
							cerr << dt << " ";
						cerr << (*params_it)->GetName();
						cerr << " = \"" << (string)*(*params_it) << "\"";
						if(strlen(desc) != 0)
							cerr << " (" << (*params_it)->GetDescription() << ")";
						cerr << endl;
					}
				}
				if (!found)
					cerr << "WARNING! Could not find description for variable/object \"" << *list_parms_it << "\"" << endl;
			}
		}
	
	// Save the current configuration
	if (get_config)
	{
		cerr << "Getting simulator configuration from \"" << get_config_name << "\"." << endl;
		ServiceManager::XmlfyParameters(get_config_name);
		return 0;
	}
	if (!set_config || do_not_run)
	{
		if (!get_config && !do_not_run) help(argv[0]);
		return 0;
	}	

	// In Linux mode, the system is not entirely simulated.
	// This mode allows to run Linux applications without simulating all the peripherals.
	// cpu->memory_import >> memory->memory_export;
	
	cpu->master_socket(memory->slave_sock);
	irq_master_stub->out_interrupt(cpu->in_irq);
	fiq_master_stub->out_interrupt(cpu->in_fiq);
	
	if(inline_debugger)
	{
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
	cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
	cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
	cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
	cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;

	il1_power_estimator->time_import >> time->time_export;
	dl1_power_estimator->time_import >> time->time_export;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	cpu->symbol_table_lookup_import >> elf32_loader->symbol_table_lookup_export;
	// bridge->memory_import >> memory->memory_export;

	if(use_inline_debugger)
	{
		inline_debugger->symbol_table_lookup_import >> 
			elf32_loader->symbol_table_lookup_export;
	}
	
#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at user privilege level (Linux system calls translation enabled)" << endl;

		double time_start = host_time->GetTime();

		EnableDebug();
		void (*prev_sig_int_handler)(int);

		if(!inline_debugger)
		{
			prev_sig_int_handler = signal(SIGINT, SigIntHandler);
		}

		try
		{
			sc_set_stop_mode(SC_STOP_IMMEDIATE);
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

	if(elf32_loader) delete elf32_loader;
	if(linux_loader) delete linux_loader;
	if(linux_os) delete linux_os;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(time) delete time;
	// if(logger) delete logger;
#ifdef SIM_POWER_ESTIMATOR_SUPPORT
	if (il1_power_estimator) delete il1_power_estimator;
	if (dl1_power_estimator) delete dl1_power_estimator;
#endif // SIM_POWER_ESTIMATOR_SUPPORT

	return 0;
}
