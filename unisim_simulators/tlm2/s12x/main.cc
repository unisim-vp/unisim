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
#include <unisim/service/loader/elf_loader/elf_loader.tcc>
#include <unisim/service/loader/s19_loader/s19_loader.hh>
#include <unisim/service/debug/symbol_table/symbol_table.hh>
#include <iostream>
#include <getopt.h>
#include <unisim/kernel/service/service.hh>
#include <stdlib.h>

#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/hcs12x.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>
#include <unisim/component/tlm2/processor/hcs12x/atd10b.hh>
#include <unisim/component/tlm2/processor/hcs12x/pwm.hh>

#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>

#include <unisim/util/garbage_collector/garbage_collector.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/logger/logger_server.hh>
#include <unisim/service/loader/s19_loader/s19_loader.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/loader.hh>

#include <stdexcept>

#include <signal.h>

#include "rtb_unisim.cc"

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


//=========================================================================
//===                       Constants definitions                       ===
//=========================================================================

typedef unisim::component::cxx::processor::hcs12x::physical_address_t CPU_ADDRESS_TYPE;
typedef uint64_t MEMORY_ADDRESS_TYPE;
typedef unisim::component::cxx::processor::hcs12x::service_address_t SERVICE_ADDRESS_TYPE;

//=========================================================================
//===                     Aliases for components classes                ===
//=========================================================================

typedef unisim::component::tlm2::memory::ram::Memory<> MEMORY;

typedef unisim::component::tlm2::processor::hcs12x::HCS12X CPU;

class InternalRouterConfig {
public:
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 5;
	static const unsigned int MAX_NUM_MAPPINGS = 256;
	static const unsigned int BUSWIDTH = 32;
	typedef tlm::tlm_base_protocol_types TYPES;
	static const bool VERBOSE = false;
};
typedef unisim::component::tlm2::interconnect::generic_router::Router<InternalRouterConfig> INTERNAL_ROUTER;

class ExternalRouterConfig {
public:
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 1;
	static const unsigned int MAX_NUM_MAPPINGS = 256;
	static const unsigned int BUSWIDTH = 32;
	typedef tlm::tlm_base_protocol_types TYPES;
	static const bool VERBOSE = false;
};
typedef unisim::component::tlm2::interconnect::generic_router::Router<ExternalRouterConfig> EXTERNAL_ROUTER;

typedef unisim::component::tlm2::processor::hcs12x::S12XMMC MMC;

typedef unisim::component::tlm2::processor::hcs12x::PWM<8> PWM;
typedef unisim::component::tlm2::processor::hcs12x::ATD10B<16> ATD1;
typedef unisim::component::tlm2::processor::hcs12x::ATD10B<8> ATD0;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using namespace std;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;

typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;

using unisim::component::tlm2::processor::hcs12x::INT_GEN;
using unisim::component::tlm2::processor::hcs12x::XINT;
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
	cerr << "-s <symbol_filename>" << endl;
	cerr << "--symbol-filename <symbol_filename>" << endl;
	cerr << "            used for debbug purpose when loading an S19-File" << endl;
	cerr << "-f" << endl;
	cerr << "--force-use-virtual-address" << endl;
	cerr << "            force the ELF Loader to use segment virtual address instead of segment physical address" << endl << endl;
	cerr << "--help" << endl;
	cerr << "-h" << endl;
	cerr << "            displays this help" << endl;
}

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
	{"symbol-filename", required_argument, 0, 's'},
	{"force-use-virtual-address", no_argument, 0, 'f'},
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

	char *symbol_filename = NULL;

//	double cpu_frequency = 40.0; // in Mhz
	double cpu_frequency = 2.0; // in Mhz

	uint8_t cpu_clock_multiplier = 1;
	uint8_t xgate_clock_multiplier = 2;
//	double cpu_ipc = 1.0; // in instructions per cycle
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;
	bool force_use_virtual_address = false;

	ADDRESS::ENCODING address_encoding = ADDRESS::BANKED;

	// Parse the command line arguments
	while((c = getopt_long (argc, argv, "cs:dg:a:hi:l:zeomf", long_options, 0)) != -1)
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
				address_encoding = (ADDRESS::ENCODING) atoi(optarg);
				break;
			case 's':
				symbol_filename = optarg;
				break;
			case 'f':
				force_use_virtual_address = true;
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

	MMC *mmc = 	new MMC("mmc");

	ATD1 *atd1 = new ATD1("ATD1");
	ATD0 *atd0 = new ATD0("ATD0");

	PWM *pwm = new PWM("PWM");

	//  - tlm2 router
	EXTERNAL_ROUTER	*external_router = new EXTERNAL_ROUTER("external_router");
	INTERNAL_ROUTER	*internal_router = new INTERNAL_ROUTER("internal_router");

	//  - Memories
	MEMORY *internal_memory = new MEMORY("internal-memory");
	MEMORY *external_memory = new MEMORY("external-memory");

	// - Interrupt controller
	XINT *s12xint = new XINT("s12xint");

	INT_GEN *int_gen = new INT_GEN("INT_GEN");

	RTBStub *rtbStub = new RTBStub("RTB_STUB", fsb_cycle_time);

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	bool isS19 = false;

	if ((strstr(filename, ".s19") != NULL) ||
		 (strstr(filename, ".S19") != NULL))  {
		isS19 = true;
	}

	Service<Loader<SERVICE_ADDRESS_TYPE> > *loaderS19 = NULL;
	Service<Loader<SERVICE_ADDRESS_TYPE> > *loaderELF = NULL;

	if (isS19) {
		loaderS19 = new S19_Loader<SERVICE_ADDRESS_TYPE>("S19_Loader");
		if (symbol_filename != NULL) {
			loaderELF = new Elf32Loader("elf32-loader");
		}
	} else {
		loaderELF = new Elf32Loader("elf32-loader");
	}


	//  - Symbol table
	SymbolTable<SERVICE_ADDRESS_TYPE> *symbol_table = new SymbolTable<SERVICE_ADDRESS_TYPE>("symbol-table");
	//  - GDB server
	GDBServer<SERVICE_ADDRESS_TYPE> *gdb_server = use_gdb_server ? new GDBServer<SERVICE_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	InlineDebugger<SERVICE_ADDRESS_TYPE> *inline_debugger = use_inline_debugger ? new InlineDebugger<SERVICE_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Logger
	LoggerServer *logger = logger_on ? new LoggerServer("logger") : 0;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================


	//  - 68HCS12X processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	(*cpu)["cpu-cycle-time"] = cpu_cycle_time;
	(*cpu)["bus-cycle-time"] = fsb_cycle_time;
	if(maxinst)
	{
		(*cpu)["max-inst"] = maxinst;
	}

	(*pwm)["bus-cycle-time"] = fsb_cycle_time;
	(*pwm)["base-address"] = 0x0300;

	(*atd1)["bus-cycle-time"] = fsb_cycle_time;
	(*atd1)["base-address"] = 0x0080;
	(*atd1)["interrupt-offset"] = 0xD0;

	(*atd0)["bus-cycle-time"] = fsb_cycle_time;
	(*atd0)["base-address"] = 0x02C0;
	(*atd0)["interrupt-offset"] = 0xD2;

	//  -External Router
	unisim::kernel::service::VariableBase *var = ServiceManager::GetParameter("external_router.cycle_time");
	*var = fsb_cycle_time;

	var = ServiceManager::GetParameter("external_router.mapping_0");
	*var = "range_start=\"0x000800\" range_end=\"0x7FFFFF\" output_port=\"0\""; // 8MByte - RAM-EEPROM-FLASH

	var = ServiceManager::GetParameter("external_router.verbose_all");
 	*var = false;

	//  -Internal Router
	unisim::kernel::service::VariableBase *var1 = ServiceManager::GetParameter("internal_router.cycle_time");
	*var1 = fsb_cycle_time;

	var1 = ServiceManager::GetParameter("internal_router.mapping_0");
	*var1 = "range_start=\"0x000080\" range_end=\"0x0000AF\" output_port=\"0\""; // ATD10B16C

	var1 = ServiceManager::GetParameter("internal_router.mapping_1");
	*var1 = "range_start=\"0x000120\" range_end=\"0x00012F\" output_port=\"1\""; // S12XINT

	var1 = ServiceManager::GetParameter("internal_router.mapping_2");
	*var1 = "range_start=\"0x0002C0\" range_end=\"0x0002DF\" output_port=\"2\""; // ATD10B8C

	var1 = ServiceManager::GetParameter("internal_router.mapping_3");
	*var1 = "range_start=\"0x000300\" range_end=\"0x000327\" output_port=\"3\""; // PWM - 37 bytes

	var1 = ServiceManager::GetParameter("internal_router.mapping_4");
	*var1 = "range_start=\"0x000800\" range_end=\"0xFFFF\" output_port=\"4\""; // 64KByte - RAM-EEPROM-FLASH

	var1 = ServiceManager::GetParameter("internal_router.verbose_all");
 	*var1 = false;

	// MMC parameter
	(*mmc)["address-encoding"] = address_encoding;

	//  - External Memory
	(*external_memory)["cycle-time"] = mem_cycle_time;
	(*external_memory)["org"] = 0x00000000UL;
	(*external_memory)["bytesize"] = (uint32_t) 0x800000; // memory size is 8Mo
	(*external_memory)["verbose"] = false;

	//  - Internal Memory
	(*internal_memory)["cycle-time"] = mem_cycle_time;
	(*internal_memory)["org"] = 0x00000000UL;
	(*internal_memory)["bytesize"] = (uint32_t)0x10000; // memory size is 64KByte
	(*internal_memory)["verbose"] = false;

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	if(gdb_server)
	{
		(*gdb_server)["tcp-port"] = gdb_server_tcp_port;
		(*gdb_server)["architecture-description-filename"] = gdb_server_arch_filename;
	}


	if(isS19) {
		(*loaderS19)["filename"] = filename;
		if (symbol_filename != NULL) {
			(*loaderELF)["filename"] = symbol_filename;
		}
	}
	else
	{
		(*loaderELF)["filename"] = filename;
		(*loaderELF)["force-use-virtual-address"] = force_use_virtual_address;
	}


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

	cpu->socket(mmc->cpu_socket);
	cpu->toXINT(s12xint->fromCPU_Target);

	sc_signal<bool>  interruptReq;
	s12xint->toCPU_Initiator(interruptReq);
	cpu->interruptRequest(interruptReq);

	int_gen->interrupt_request(s12xint->interrupt_request);
	pwm->interrupt_request(s12xint->interrupt_request);
	atd1->interrupt_request(s12xint->interrupt_request);
	atd0->interrupt_request(s12xint->interrupt_request);

	rtbStub->atd1_master_sock(atd1->anx_socket);
	rtbStub->atd0_master_sock(atd0->anx_socket);
	rtbStub->slave_sock(pwm->master_sock);

	mmc->local_socket(internal_router->targ_socket[0]);
	mmc->external_socket(external_router->targ_socket[0]);

	// This order is mandatory (see the memoryMapping)
	internal_router->init_socket[0](atd1->slave_socket);
	internal_router->init_socket[1](s12xint->slave_socket);
	internal_router->init_socket[2](atd0->slave_socket);
	internal_router->init_socket[3](pwm->slave_socket);
	internal_router->init_socket[4](internal_memory->slave_sock); // to connect to the MMC

	external_router->init_socket[0](external_memory->slave_sock);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> mmc->memory_export;
	mmc->internal_memory_import >> internal_memory->memory_export;
	mmc->external_memory_import >> external_memory->memory_export;

	if(inline_debugger)
	{
		// Connect inline-debugger to CPU
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;

		mmc->trap_reporting_import >> inline_debugger->trap_reporting_export;

		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
		inline_debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}
	else if(gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;

		mmc->trap_reporting_import >> gdb_server->trap_reporting_export;

		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}

	if (isS19) {
		((S19_Loader<SERVICE_ADDRESS_TYPE> *) loaderS19)->memory_import >> mmc->memory_export;

		if (symbol_filename != NULL) {
			((Elf32Loader *) loaderELF)->symbol_table_build_import >> symbol_table->symbol_table_build_export;
		}
	} else {
		((Elf32Loader *) loaderELF)->memory_import >> mmc->memory_export;
		((Elf32Loader *) loaderELF)->symbol_table_build_import >> symbol_table->symbol_table_build_export;
	}

	if(inline_debugger)
	{
		inline_debugger->symbol_table_lookup_import >> symbol_table->symbol_table_lookup_export;
	}

	/* logger connections */
	if(logger_on) {
		unsigned int logger_index = 0;
		logger->time_import >> time->time_export;
		cpu->logger_import >> *logger->logger_export[logger_index++];

		if(gdb_server) gdb_server->logger_import >> *logger->logger_export[logger_index++];
	}

#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(ServiceManager::Setup())
	{

		physical_address_t entry_point;

		if (isS19) {
			entry_point = loaderS19->GetEntryPoint();
		}
		else{
			entry_point = loaderELF->GetEntryPoint();
		}

		address_t cpu_address;
		uint8_t page = 0;

		if (isS19) {
			mmc->SplitPagedAddress(entry_point, page, cpu_address);
		} else {
			cpu_address = (address_t) entry_point;
		}

		cpu->SetEntryPoint(cpu_address);

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

	if(loaderS19) { delete loaderS19; loaderS19 = NULL; }
	if(loaderELF) { delete loaderELF; loaderELF = NULL; }

	if (int_gen) { delete int_gen; int_gen = NULL; }
	if (rtbStub) { delete rtbStub; rtbStub = NULL; }

	if (atd1) { delete atd1; atd1 = NULL; }
	if (atd0) { delete atd0; atd0 = NULL; }
	if (s12xint) { delete s12xint; s12xint = NULL; }
	if (mmc) { delete mmc; mmc = NULL; }
	if(external_memory) { delete external_memory; external_memory = NULL; }
	if(internal_memory) { delete internal_memory; internal_memory = NULL; }
	if(external_router) { delete external_router; external_router = NULL; }
	if(internal_router) { delete internal_router; internal_router = NULL; }
	if(cpu) { delete cpu; cpu = NULL; }

	if(logger) { delete logger; logger = NULL; }

	if(gdb_server) { delete gdb_server; gdb_server = NULL; }
	if(inline_debugger) { delete inline_debugger; inline_debugger = NULL; }
	if (symbol_table) { delete symbol_table; symbol_table = NULL; }

	if (host_time) { delete host_time; host_time = NULL; }
	if(time) { delete time; time = NULL; }

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
