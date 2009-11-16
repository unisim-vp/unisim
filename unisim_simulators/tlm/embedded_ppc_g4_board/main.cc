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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <stdexcept>
#include <signal.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/component/tlm/processor/powerpc/powerpc.hh"
#include "unisim/component/tlm/memory/ram/memory.hh"
#include "unisim/component/tlm/memory/flash/am29lv/am29lv.hh"
#include "unisim/component/tlm/pci/video/display.hh"
#include "unisim/component/tlm/fsb/snooping_bus/bus.hh"
#include "unisim/component/tlm/chipset/mpc107/mpc107.hh"
#include "unisim/component/tlm/pci/bus/bus.hh"

#ifdef WITH_PCI_STUB
#include "unisim/component/tlm/pci/debug/pci_stub.hh"
#endif

#include "unisim/component/cxx/pci/types.hh"
#include "unisim/component/cxx/processor/powerpc/config.hh"
#include "unisim/component/tlm/debug/transaction_spy.hh"
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/loader/elf_loader/elf32_loader.hh"
#include "unisim/service/power/cache_power_estimator.hh"
#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/tee/memory_access_reporting/tee.hh"
#include "unisim/service/tee/symbol_table_lookup/tee.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif


#ifdef EMBEDDED_PPC_G4_BOARD_DEBUG
	static const bool DEBUG_INFORMATION = true;
#else
	static const bool DEBUG_INFORMATION = false;
#endif

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
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::component::cxx::pci::pci64_address_t;
using unisim::component::cxx::pci::pci32_address_t;
using unisim::kernel::service::ServiceManager;

void help(char *prog_name)
{
	cerr << "Usage: " << prog_name << " [<options>] <program> [program arguments]" << endl << endl;
	cerr << "       'program' is an ELF32 statically linked Linux binary" << endl;
	cerr << "       'program' can be a Linux kernel if option '-k' is used" << endl << endl;
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
	cerr << "-u <pipe name>" << endl;
#ifdef WITH_PCI_STUB
	cerr << "--pci-stub-use-pipe <pipe name>" << endl;
	cerr << "            use file system pipes for the pci-stub communications using <pipe name> (disables tcp-ip communication)" << endl << endl;
	cerr << "-s <server name>" << endl;
	cerr << "--pci-stub-server <server name>" << endl;
	cerr << "            specify the server name for pci-stub" << endl << endl;
	cerr << "-r <tcp port>" << endl;
	cerr << "--pci-stub-tcp-port <tcp port>" << endl;
	cerr << "            specify the tcp port used by pci-stub" << endl << endl;
	cerr << "-v" << endl;
	cerr << "--pci-stub-is-server" << endl;
	cerr << "            make pci-stub act as a server instead of a client" << endl << endl;
	cerr << "-n <base address>,<byte size>,<address space>[...]" << endl;
	cerr << "--pci-stub-regions <base address>,<byte size>,<address space>[...]" << endl;
	cerr << "            specify the pci-stub regions (up to 6 separated by one or more space characters). 'address space' is either 'mem' or 'i/o'" << endl << endl;
	cerr << "-q <irq number>" << endl;
	cerr << "--pci-stub-irq <irq number>" << endl;
	cerr << "            specify the pci-stub IRQ number" << endl << endl;
#endif
	cerr << "-f" << endl;
	cerr << "--force-use-virtual-address" << endl;
	cerr << "            force the ELF Loader to use segment virtual address instead of segment physical address" << endl << endl;
	cerr << "-b <ram memory size in MB>" << endl;
	cerr << "--ram-size <ram memory size in MB>" << endl;
	cerr << "            specify the ram memory size (default = 256MB)" << endl << endl;
	cerr << "--help" << endl;
	cerr << "-h" << endl;
	cerr << "            displays this help" << endl;
}

//=========================================================================
//===                       Constants definitions                       ===
//=========================================================================

// CPU parameters
#ifdef EMBEDDED_PPC_G4_BOARD_DEBUG
typedef unisim::component::cxx::processor::powerpc::MPC7447ADebugConfig CPU_CONFIG;
#else
typedef unisim::component::cxx::processor::powerpc::MPC7447AConfig CPU_CONFIG;
#endif
// Front Side Bus template parameters
typedef CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;

// PCI Bus template parameters
typedef pci32_address_t PCI_ADDRESS_TYPE;
const uint32_t PCI_MAX_DATA_SIZE = 32;        // in bytes
const unsigned int PCI_NUM_MASTERS = 1;
#ifdef WITH_PCI_STUB
const unsigned int PCI_NUM_TARGETS = 2;
#else
const unsigned int PCI_NUM_TARGETS = 1;
#endif
const unsigned int PCI_NUM_MAPPINGS = 7;

// PCI device numbers
const unsigned int PCI_MPC107_DEV_NUM = 0;
#ifdef WITH_PCI_STUB
const unsigned int PCI_STUB_DEV_NUM = 1;
#endif

// PCI target port numbers
const unsigned int PCI_MPC107_TARGET_PORT = 0;
#ifdef WITH_PCI_STUB
const unsigned int PCI_STUB_TARGET_PORT = 1;
#endif

// PCI master port numbers
const unsigned int PCI_MPC107_MASTER_PORT = 0;

// the maximum number of transaction spies (per type of message)
const unsigned int MAX_BUS_TRANSACTION_SPY = 4;
const unsigned int MAX_MEM_TRANSACTION_SPY = 3;
const unsigned int MAX_PCI_TRANSACTION_SPY = 3;
const unsigned int MAX_IRQ_TRANSACTION_SPY = 3;

// Flash memory
const uint32_t FLASH_BYTESIZE = 4 * unisim::component::cxx::memory::flash::am29lv::M; // 4 MB
const uint32_t FLASH_IO_WIDTH = 8; // 64 bits
typedef unisim::component::cxx::memory::flash::am29lv::AM29LV800BConfig FLASH_CONFIG;

//=========================================================================
//===                     Aliases for components classes                ===
//=========================================================================

typedef unisim::component::tlm::memory::flash::am29lv::AM29LV<FLASH_CONFIG, FLASH_BYTESIZE, FLASH_IO_WIDTH, FSB_MAX_DATA_SIZE> FLASH;
typedef unisim::component::tlm::chipset::mpc107::MPC107<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, DEBUG_INFORMATION> MPC107;
typedef unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, FSB_NUM_PROCS> FRONT_SIDE_BUS;
typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION> PCI_BUS;
#ifdef WITH_PCI_STUB
typedef unisim::component::tlm::pci::debug::PCIStub<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_STUB;
#endif
typedef unisim::component::tlm::processor::powerpc::PowerPC<CPU_CONFIG> CPU;

//=========================================================================
//===               Aliases for transaction Spies classes               ===
//=========================================================================

typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, FSB_NUM_PROCS>::ReqType BusMsgReqType;
typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, FSB_NUM_PROCS>::RspType BusMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<BusMsgReqType, BusMsgRspType> BusMsgSpyType;
typedef unisim::component::tlm::message::MemoryRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MemMsgReqType;
typedef unisim::component::tlm::message::MemoryResponse<FSB_MAX_DATA_SIZE> MemMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<MemMsgReqType, MemMsgRspType> MemMsgSpyType;
typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION>::ReqType PCIMsgReqType;
typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION>::RspType PCIMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<PCIMsgReqType, PCIMsgRspType> PCIMsgSpyType;
typedef unisim::component::tlm::message::InterruptRequest IRQReqSpyType;
typedef unisim::component::tlm::debug::TransactionSpy<IRQReqSpyType> IRQMsgSpyType;

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
	{"power", no_argument, 0, 'p'},
#ifdef WITH_PCI_STUB
	{"pci-stub-server", required_argument, 0, 's'},
	{"pci-stub-port", required_argument, 0, 'r'},
	{"pci-stub-is-server", no_argument, 0, 'v'},
	{"pci-stub-use-pipe", required_argument, 0, 'u'},
	{"pci-stub-regions", required_argument, 0, 'n'},
	{"pci-stub-irq", required_argument, 0, 'q'},
#endif
	{"force-use-virtual-address", no_argument, 0, 'f'},
	{"ram-size", required_argument, 0, 'b'},
	{0, 0, 0, 0}
	};

	int c;
	bool use_gdb_server = false;
	bool use_inline_debugger = false;
	bool estimate_power = false;
	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	const char *ramdisk_filename = "";
	uint64_t maxinst = 0; // maximum number of instruction to simulate
	uint32_t pci_bus_frequency = 33; // in Mhz
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	double fsb_frequency = cpu_frequency / cpu_clock_multiplier; // FIXME: to be removed
	uint32_t tech_node = 130; // in nm
	uint32_t memory_size = 256 * 1024 * 1024; // 256 MB
#ifdef WITH_PCI_STUB
	const char *pci_stub_regions = "";
	unsigned int pci_stub_irq = 0;
	bool pci_stub_use_pipe = false;
	unsigned int pci_stub_tcp_port = 12345;
	const char *pci_stub_server_name = "localhost";
	bool pci_stub_is_server = false;
	const char *pci_stub_pipe_name = "pipe";
#endif
	double cpu_ipc = 1.0; // in instructions per cycle
	bool force_use_virtual_address = false;
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;

	// build the option string for getopt
	char optstring[256];
	struct option *long_opt;
	char *optstring_p;
	for(long_opt = long_options, optstring_p = optstring; long_opt->name; long_opt++)
	{
		*optstring_p++ = long_opt->val;
		if(long_opt->has_arg == required_argument) *optstring_p++ = ':';
		
	}
	*optstring_p = 0;
	cerr << "optstring =\"" << optstring << "\"\n" << endl;

	// Parse the command line arguments
	while((c = getopt_long (argc, argv, optstring, long_options, 0)) != -1)
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
			case 'p':
				estimate_power = true;
				break;
#ifdef WITH_PCI_STUB
			case 's':
				pci_stub_server_name = optarg;
				break;
			case 'r':
				pci_stub_tcp_port = atoi(optarg);
				break;
			case 'v':
				pci_stub_is_server = true;
				break;
			case 'u':
				pci_stub_use_pipe = true;
				pci_stub_pipe_name = optarg;
				break;
			case 'n':
				pci_stub_regions = strdup(optarg);
				break;
			case 'q':
				pci_stub_irq = atoi(optarg);
				break;
#endif
			case 'f':
				force_use_virtual_address = true;
				break;
			case 'b':
				memory_size = atoi(optarg) * 1024 * 1024;
				break;
		}
	}

	if(optind >= argc)
	{
		help(argv[0]);
		return 0;
	}

	// get the binaries to load
	vector<string> filename;
	for(unsigned int i = optind; i < argc; i++)
		filename.push_back(argv[i]);

	// If no filename has been specified then display the help
	if(filename.size() == 0)
	{
		help(argv[0]);
		return 0;
	}
	

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu =new CPU("cpu");
	//  - Front side bus
	FRONT_SIDE_BUS *bus = new FRONT_SIDE_BUS("bus");
	//  - MPC107 chipset
	MPC107 *mpc107 = new MPC107("mpc107");
	//  - EROM
	MEMORY *erom = new MEMORY("erom");
	//  - Flash memory
	FLASH *flash = new FLASH("flash");
	//  - RAM
	MEMORY *memory = new MEMORY("memory");
	//  - PCI Bus
	PCI_BUS *pci_bus = new PCI_BUS("pci-bus");
	//  - PCI Stub
#ifdef WITH_PCI_STUB
	PCI_STUB *pci_stub = new PCI_STUB("pci-stub");
#endif

	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================
	BusMsgSpyType *bus_msg_spy[MAX_BUS_TRANSACTION_SPY];
	MemMsgSpyType *mem_msg_spy[MAX_MEM_TRANSACTION_SPY];
	PCIMsgSpyType *pci_msg_spy[MAX_PCI_TRANSACTION_SPY];
	IRQMsgSpyType *irq_msg_spy[MAX_IRQ_TRANSACTION_SPY];

	if(false)
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
		for(unsigned int i = 0; i < MAX_PCI_TRANSACTION_SPY; i++)
		{
			stringstream sstr;
			sstr << "pci_msg_spy[" << i << "]";
			string name = sstr.str();
			pci_msg_spy[i] = new PCIMsgSpyType(name.c_str());
		}
		for(unsigned int i = 0; i < MAX_IRQ_TRANSACTION_SPY; i++)
		{
			stringstream sstr;
			sstr << "irq_msg_spy[" << i << "]";
			string name = sstr.str();
			irq_msg_spy[i] = new IRQMsgSpyType(name.c_str());
		}
	}
	else
	{
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++) bus_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++) mem_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_PCI_TRANSACTION_SPY; i++) pci_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_IRQ_TRANSACTION_SPY; i++) irq_msg_spy[i] = 0;
	}

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - one elf32 loader for each of the binaries
	vector<unisim::service::loader::elf_loader::Elf32Loader *> elf32_loader;
	for(unsigned int i = 0; i < filename.size(); i++)
	{
    	stringstream str;
    	str << "elf32-loader[" << i << "]";
	    elf32_loader.push_back(new unisim::service::loader::elf_loader::Elf32Loader(str.str().c_str()));
    }
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Tee Symbol Table Lookup
	unisim::service::tee::symbol_table_lookup::Tee<CPU_ADDRESS_TYPE> *tee_symbol_table_lookup = new unisim::service::tee::symbol_table_lookup::Tee<CPU_ADDRESS_TYPE>("tee-symbol-table-lookup");
	//  - Tee Memory Access Reporting
	unisim::service::tee::memory_access_reporting::Tee<PCI_ADDRESS_TYPE> * tee_memory_access_reporting = 
		(use_gdb_server || use_inline_debugger) ?
			new unisim::service::tee::memory_access_reporting::Tee<PCI_ADDRESS_TYPE>("tee-memory-access-reporting") :
			0;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server = use_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger = use_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - the optional power estimators
	CachePowerEstimator *il1_power_estimator = estimate_power ? new CachePowerEstimator("il1-power-estimator") : 0;
	CachePowerEstimator *dl1_power_estimator = estimate_power ? new CachePowerEstimator("dl1-power-estimator") : 0;
	CachePowerEstimator *l2_power_estimator = estimate_power ? new CachePowerEstimator("l2-power-estimator") : 0;
	CachePowerEstimator *itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	CachePowerEstimator *dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - Front Side Bus
	(*bus)["cycle-time"] = fsb_cycle_time;

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	(*cpu)["cpu-cycle-time"] = cpu_cycle_time;
	(*cpu)["bus-cycle-time"] = fsb_cycle_time;
	(*cpu)["voltage"] = 1.3 * 1e3; // mV
	(*cpu)["nice-time"] = 1000000000; // 1 ms
	if(maxinst)
	{
		(*cpu)["max-inst"] = maxinst;
	}
	(*cpu)["ipc"] = cpu_ipc;

	//  - RAM
	(*memory)["cycle-time"] = mem_cycle_time;
	(*memory)["org"] = 0x00000000UL;
	(*memory)["bytesize"] = memory_size;

    // MPC107 run-time configuration
    (*mpc107)["a_address_map"] = false;
    (*mpc107)["host_mode"] = true;
    (*mpc107)["memory_32bit_data_bus_size"] = true;
    (*mpc107)["rom0_8bit_data_bus_size"] = false;
    (*mpc107)["rom1_8bit_data_bus_size"] = false;
    (*mpc107)["frequency"] = fsb_frequency;
	(*mpc107)["sdram_cycle_time"] = mem_cycle_time;

	//  - EROM run-time configuration
	(*erom)["org"] =  0x78000000UL;
	(*erom)["bytesize"] = 2 * 8 * 1024 * 1024;
	(*erom)["cycle-time"] = mem_cycle_time;
	
	//  - Flash memory run-time configuration
    (*flash)["org"] = 0xff800000UL; //0xff000000UL;
    (*flash)["bytesize"] = 8 * 1024 * 1024;
    (*flash)["cycle-time"] = mem_cycle_time;
	(*flash)["endian"] = "big-endian";

	// PCI Bus run-time configuration
	unsigned int mapping_index = 0;
	(*pci_bus)["frequency"] = pci_bus_frequency;

    (*pci_bus)["base-address"][mapping_index] = 0;
    (*pci_bus)["size"][mapping_index] = 1024 * 1024 * 1024;
    (*pci_bus)["device-number"][mapping_index] = PCI_MPC107_DEV_NUM;
    (*pci_bus)["target-port"][mapping_index] = 0;
    (*pci_bus)["register-number"][mapping_index] = 0x10;
    (*pci_bus)["addr-type"][mapping_index] = "mem";
	(*pci_bus)["num-mappings"] = ++mapping_index; 

#ifdef WITH_PCI_STUB
	// PCI stub run-time configuration
	{
		uint32_t pci_stub_initial_base_addr[6];
		uint32_t pci_stub_region_size[6];
		char pci_stub_address_space[6][256];
		int nargs = sscanf(pci_stub_regions,
		"%" PRIx32 ",%" PRIu32 ",%255s "
		"%" PRIx32 ",%" PRIu32 ",%255s "
		"%" PRIx32 ",%" PRIu32 ",%255s "
		"%" PRIx32 ",%" PRIu32 ",%255s "
		"%" PRIx32 ",%" PRIu32 ",%255s "
		"%" PRIx32 ",%" PRIu32 ",%255s",
		&pci_stub_initial_base_addr[0],
		&pci_stub_region_size[0],
		&pci_stub_address_space[0],
		&pci_stub_initial_base_addr[1],
		&pci_stub_region_size[1],
		&pci_stub_address_space[1],
		&pci_stub_initial_base_addr[2],
		&pci_stub_region_size[2],
		&pci_stub_address_space[2],
		&pci_stub_initial_base_addr[3],
		&pci_stub_region_size[3],
		&pci_stub_address_space[3],
		&pci_stub_initial_base_addr[4],
		&pci_stub_region_size[4],
		&pci_stub_address_space[4],
		&pci_stub_initial_base_addr[5],
		&pci_stub_region_size[5],
		&pci_stub_address_space[5]);

		if(nargs > 0)
		{
			int num_regions = nargs / 3;
			int num_region;
			for(num_region = 0; num_region < num_regions; num_region++)
			{
				(*pci_bus)["base-address"][mapping_index] = pci_stub_initial_base_addr[num_region];
				(*pci_bus)["size"][mapping_index] = pci_stub_region_size[num_region];
				(*pci_bus)["device-number"][mapping_index] = PCI_STUB_DEV_NUM;
				(*pci_bus)["target-port"][mapping_index] = PCI_STUB_TARGET_PORT;
				(*pci_bus)["register-number"][mapping_index] = 0x10UL + (4 * num_region);
				(*pci_bus)["addr-type"][mapping_index] = pci_stub_address_space[num_region];
				(*pci_bus)["num-mappings"] = ++mapping_index; 

				(*pci_stub)["initial-base-addr"][num_region] = pci_stub_initial_base_addr[num_region];
				(*pci_stub)["region-size"][num_region] = pci_stub_region_size[num_region];
				(*pci_stub)["address-space"][num_region] = pci_stub_address_space[num_region];
			}
		}

		(*pci_stub)["pci-bus-frequency"] = pci_bus_frequency;
		(*pci_stub)["bus-frequency"] = fsb_frequency;
		(*pci_stub)["tcp-port"] = pci_stub_tcp_port;
		(*pci_stub)["server-name"] = pci_stub_server_name;
		(*pci_stub)["is-server"] = pci_stub_is_server;
		(*pci_stub)["protocol"] = pci_stub_use_pipe ? 1 : 0;
		(*pci_stub)["pipe-name"] = pci_stub_pipe_name;
	}
#endif
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

    //  - ELF32 Loader run-time configuration
    for(unsigned int i = 0; i < filename.size(); i++)
	{
    	(*elf32_loader[i])["filename"] = filename[i].c_str();
		(*elf32_loader[i])["force-use-virtual-address"] = force_use_virtual_address;
	}

	//  - GDB Server run-time configuration
	if(gdb_server)
	{
		(*gdb_server)["tcp-port"] = gdb_server_tcp_port;
		(*gdb_server)["architecture-description-filename"] = gdb_server_arch_filename;
	}

	//  - Cache/TLB power estimators run-time configuration
	if(estimate_power)
	{
		(*il1_power_estimator)["cache-size"] = 32 * 1024;
		(*il1_power_estimator)["line-size"] = 32;
		(*il1_power_estimator)["associativity"] = 8;
		(*il1_power_estimator)["rw-ports"] = 0;
		(*il1_power_estimator)["excl-read-ports"] = 1;
		(*il1_power_estimator)["excl-write-ports"] = 0;
		(*il1_power_estimator)["single-ended-read-ports"] = 0;
		(*il1_power_estimator)["banks"] = 4;
		(*il1_power_estimator)["tech-node"] = tech_node;
		(*il1_power_estimator)["output-width"] = 128;
		(*il1_power_estimator)["tag-width"] = 64;
		(*il1_power_estimator)["access-mode"] = "fast";
	
		(*dl1_power_estimator)["cache-size"] = 32 * 1024;
		(*dl1_power_estimator)["line-size"] = 32;
		(*dl1_power_estimator)["associativity"] = 8;
		(*dl1_power_estimator)["rw-ports"] = 1;
		(*dl1_power_estimator)["excl-read-ports"] = 0;
		(*dl1_power_estimator)["excl-write-ports"] = 0;
		(*dl1_power_estimator)["single-ended-read-ports"] = 0;
		(*dl1_power_estimator)["banks"] = 4;
		(*dl1_power_estimator)["tech-node"] = tech_node;
		(*dl1_power_estimator)["output-width"] = 64;
		(*dl1_power_estimator)["tag-width"] = 64;
		(*dl1_power_estimator)["access-mode"] = "fast";
	
		(*l2_power_estimator)["cache-size"] = 512 * 1024;
		(*l2_power_estimator)["line-size"] = 32;
		(*l2_power_estimator)["associativity"] = 8;
		(*l2_power_estimator)["rw-ports"] = 1;
		(*l2_power_estimator)["excl-read-ports"] = 0;
		(*l2_power_estimator)["excl-write-ports"] = 0;
		(*l2_power_estimator)["single-ended-read-ports"] = 0;
		(*l2_power_estimator)["banks"] = 4;
		(*l2_power_estimator)["tech-node"] = tech_node;
		(*l2_power_estimator)["output-width"] = 256;
		(*l2_power_estimator)["tag-width"] = 64;
		(*l2_power_estimator)["access-mode"] = "fast";
	
		(*itlb_power_estimator)["cache-size"] = 128 * 2 * 4;
		(*itlb_power_estimator)["line-size"] = 4;
		(*itlb_power_estimator)["associativity"] = 2;
		(*itlb_power_estimator)["rw-ports"] = 1;
		(*itlb_power_estimator)["excl-read-ports"] = 0;
		(*itlb_power_estimator)["excl-write-ports"] = 0;
		(*itlb_power_estimator)["single-ended-read-ports"] = 0;
		(*itlb_power_estimator)["banks"] = 4;
		(*itlb_power_estimator)["tech-node"] = tech_node;
		(*itlb_power_estimator)["output-width"] = 32;
		(*itlb_power_estimator)["tag-width"] = 64;
		(*itlb_power_estimator)["access-mode"] = "fast";
	
		(*dtlb_power_estimator)["cache-size"] = 128 * 2 * 4;
		(*dtlb_power_estimator)["line-size"] = 4;
		(*dtlb_power_estimator)["associativity"] = 2;
		(*dtlb_power_estimator)["rw-ports"] = 1;
		(*dtlb_power_estimator)["excl-read-ports"] = 0;
		(*dtlb_power_estimator)["excl-write-ports"] = 0;
		(*dtlb_power_estimator)["single-ended-read-ports"] = 0;
		(*dtlb_power_estimator)["banks"] = 4;
		(*dtlb_power_estimator)["tech-node"] = tech_node;
		(*dtlb_power_estimator)["output-width"] = 32;
		(*dtlb_power_estimator)["tag-width"] = 64;
		(*dtlb_power_estimator)["access-mode"] = "fast";
	}

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	if(false)
	{
		unsigned bus_msg_spy_index = 0;
		unsigned mem_msg_spy_index = 0;
		unsigned pci_msg_spy_index = 0;
		unsigned irq_msg_spy_index = 0;

		cpu->bus_port(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = cpu->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = cpu->bus_port.name();
		bus_msg_spy[bus_msg_spy_index]->master_port(*bus->inport[0]);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = bus->inport[0]->name();
		bus_msg_spy_index++;

		(*bus->outport[0])(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->outport[0]->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(cpu->snoop_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = cpu->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = cpu->snoop_port.name();
		bus_msg_spy_index++;

		(*bus->chipset_outport)(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->chipset_outport->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(mpc107->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = mpc107->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = mpc107->slave_port.name();
		bus_msg_spy_index++;

		mpc107->master_port(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = mpc107->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = mpc107->master_port.name();
		bus_msg_spy[bus_msg_spy_index]->master_port(*bus->chipset_inport);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = bus->chipset_inport->name();
		bus_msg_spy_index++;

		mpc107->ram_master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = mpc107->name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = mpc107->ram_master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(memory->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = memory->name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = memory->slave_port.name();
		mem_msg_spy_index++;

		mpc107->rom_master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = mpc107->name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = mpc107->rom_master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(flash->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = flash->name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = flash->slave_port.name();
		mem_msg_spy_index++;

		mpc107->erom_master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = mpc107->name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = mpc107->erom_master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(erom->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = erom->name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = erom->slave_port.name();
		mem_msg_spy_index++;
		
		mpc107->pci_master_port(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = mpc107->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = mpc107->pci_master_port.name();
		pci_msg_spy[pci_msg_spy_index]->master_port(*pci_bus->input_port[PCI_MPC107_MASTER_PORT]);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_bus->input_port[PCI_MPC107_MASTER_PORT]->name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_MPC107_TARGET_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_MPC107_TARGET_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(mpc107->pci_slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = mpc107->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = mpc107->pci_slave_port.name();
		pci_msg_spy_index++;

#ifdef WITH_PCI_STUB
		(*pci_bus->output_port[PCI_STUB_TARGET_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_STUB_TARGET_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_stub->bus_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_stub->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_stub->bus_port.name();
		pci_msg_spy_index++;
#endif

		mpc107->irq_master_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = mpc107->name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = mpc107->irq_master_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(cpu->external_interrupt_port);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = cpu->name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = cpu->external_interrupt_port.name();
		irq_msg_spy_index++;

		mpc107->soft_reset_master_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = mpc107->name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = mpc107->soft_reset_master_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(cpu->soft_reset_port);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = cpu->name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = cpu->soft_reset_port.name();
		irq_msg_spy_index++;

#ifdef WITH_PCI_STUB
		pci_stub->cpu_irq_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = pci_stub->name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = pci_stub->cpu_irq_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(*mpc107->irq_slave_port[pci_stub_irq]);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = mpc107->name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = mpc107->irq_slave_port[pci_stub_irq]->name();
		irq_msg_spy_index++;
#endif
	}
	else
	{
		cpu->bus_port(*bus->inport[0]);
		(*bus->outport[0])(cpu->snoop_port);
		(*bus->chipset_outport)(mpc107->slave_port);
		mpc107->master_port(*bus->chipset_inport);
		mpc107->ram_master_port(memory->slave_port);
		mpc107->rom_master_port(flash->slave_port);
		mpc107->erom_master_port(erom->slave_port);
		mpc107->pci_master_port(*pci_bus->input_port[PCI_MPC107_MASTER_PORT]);
		(*pci_bus->output_port[PCI_MPC107_TARGET_PORT])(mpc107->pci_slave_port);
#ifdef WITH_PCI_STUB
		(*pci_bus->output_port[PCI_STUB_TARGET_PORT])(pci_stub->bus_port);
#endif
		mpc107->irq_master_port(cpu->external_interrupt_port);
		mpc107->soft_reset_master_port(cpu->soft_reset_port);
#ifdef WITH_PCI_STUB
		pci_stub->cpu_irq_port(*mpc107->irq_slave_port[pci_stub_irq]);
#endif
	}

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> bus->memory_export;
	
	if(use_inline_debugger || use_gdb_server) 
	{
		cpu->memory_access_reporting_import >> tee_memory_access_reporting->in;
		tee_memory_access_reporting->out_control >> cpu->memory_access_reporting_control_export;
	}

	if(inline_debugger)
	{
		// Connect inline-debugger to CPU
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		*tee_memory_access_reporting->out[1] >> inline_debugger->memory_access_reporting_export;
		inline_debugger->memory_access_reporting_control_import >>
			*tee_memory_access_reporting->in_control[1];
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
	}
	else if(gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		*tee_memory_access_reporting->out[1] >> gdb_server->memory_access_reporting_export;
		gdb_server->memory_access_reporting_control_import >>
			*tee_memory_access_reporting->in_control[1];
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
	}

	if(estimate_power)
	{
		// Connect everything related to power estimation
		cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
		cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
		cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
		cpu->l2_power_estimator_import >> l2_power_estimator->power_estimator_export;
		cpu->l2_power_mode_import >> l2_power_estimator->power_mode_export;
		cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
		cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
		cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
		cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;

		il1_power_estimator->time_import >> time->time_export;
		dl1_power_estimator->time_import >> time->time_export;
		l2_power_estimator->time_import >> time->time_export;
		itlb_power_estimator->time_import >> time->time_export;
		dtlb_power_estimator->time_import >> time->time_export;
	}

    for(unsigned int i = 0; i < elf32_loader.size(); i++) 
    {
	    elf32_loader[i]->memory_import >> mpc107->memory_export;
		*tee_symbol_table_lookup->out[i] >> elf32_loader[i]->symbol_table_lookup_export;
    }
    cpu->symbol_table_lookup_import >> tee_symbol_table_lookup->in;
	bus->memory_import >> mpc107->memory_export;
	mpc107->ram_import >> memory->memory_export;
	mpc107->rom_import >> flash->memory_export;
	mpc107->erom_import >> erom->memory_export;
	mpc107->pci_import >> *pci_bus->memory_export[PCI_MPC107_MASTER_PORT];

#ifdef WITH_PCI_STUB
	*pci_bus->memory_import[PCI_STUB_TARGET_PORT] >> pci_stub->memory_export; 
	if(use_inline_debugger || use_gdb_server) 
	{
		*tee_memory_access_reporting->out[0] >> pci_stub->memory_access_reporting_export;
		pci_stub->memory_access_reporting_control_import >> 
			*tee_memory_access_reporting->in_control[0];
	}
	else
	{
		cpu->memory_access_reporting_import >> pci_stub->memory_access_reporting_export;
		pci_stub->memory_access_reporting_control_import >>
			cpu->memory_access_reporting_control_export;
	}
	pci_stub->symbol_table_lookup_import >> elf32_loader[0]->symbol_table_lookup_export;
	pci_stub->synchronizable_import >> cpu->synchronizable_export;
	pci_stub->memory_import >> cpu->memory_export;
	pci_stub->registers_import >> cpu->registers_export;
#endif

	if(inline_debugger)
	{
		inline_debugger->symbol_table_lookup_import >> tee_symbol_table_lookup->in;
	}
	
#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

//	ServiceManager::XmlfyParameters("parameters.xml");
//	ServiceManager::LoadXmlParameters("parameters.xml");
//	ServiceManager::XmlfyParameters("parameters.copy.xml");
//	exit(-1);
	
	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at supervisor privilege level (kernel mode)" << endl;

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
		if(estimate_power)
		{
			double total_dynamic_energy = il1_power_estimator->GetDynamicEnergy()
				+ dl1_power_estimator->GetDynamicEnergy()
				+ l2_power_estimator->GetDynamicEnergy()
				+ itlb_power_estimator->GetDynamicEnergy()
				+ dtlb_power_estimator->GetDynamicEnergy();
	
			double total_dynamic_power = il1_power_estimator->GetDynamicPower()
				+ dl1_power_estimator->GetDynamicPower()
				+ l2_power_estimator->GetDynamicPower()
				+ itlb_power_estimator->GetDynamicPower()
				+ dtlb_power_estimator->GetDynamicPower();
	
			double total_leakage_power = il1_power_estimator->GetLeakagePower()
				+ dl1_power_estimator->GetLeakagePower()
				+ l2_power_estimator->GetLeakagePower()
				+ itlb_power_estimator->GetLeakagePower()
				+ dtlb_power_estimator->GetLeakagePower();
	
			double total_power = total_dynamic_power + total_leakage_power;

			cerr << "L1 instruction cache dynamic energy: " << il1_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "L1 data cache dynamic energy: " << dl1_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "L2 cache dynamic energy: " << l2_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "Instruction TLB dynamic energy: " << itlb_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "Data TLB dynamic energy: " << dtlb_power_estimator->GetDynamicEnergy() << " J" << endl;
			cerr << "L1 instruction cache dynamic power: " << il1_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "L1 data cache dynamic power: " << dl1_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "L2 cache dynamic power: " << l2_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "Instruction TLB dynamic power: " << itlb_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "Data TLB dynamic power: " << dtlb_power_estimator->GetDynamicPower() << " W" << endl;
			cerr << "L1 instruction cache leakage power: " << il1_power_estimator->GetLeakagePower() << " W" << endl;
			cerr << "L1 data cache leakage power: " << dl1_power_estimator->GetLeakagePower() << " W" << endl;
			cerr << "L2 cache leakage power: " << l2_power_estimator->GetLeakagePower() << " W" << endl;
			cerr << "Instruction TLB leakage power: " << itlb_power_estimator->GetLeakagePower() << " W" << endl;
			cerr << "Data TLB leakage power: " << dtlb_power_estimator->GetLeakagePower() << " W" << endl;
	
			cerr << "Total dynamic energy: " << total_dynamic_energy << " J" << endl;
			cerr << "Total dynamic power: " << total_dynamic_power << " W" << endl;
			cerr << "Total leakage power: " << total_leakage_power << " W" << endl;
			cerr << "Total power (dynamic+leakage): " << total_power << " W" << endl;
		}
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
	}

	for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++) if(bus_msg_spy[i]) delete bus_msg_spy[i];
	for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++) if(mem_msg_spy[i]) delete mem_msg_spy[i];
	for(unsigned int i = 0; i < MAX_PCI_TRANSACTION_SPY; i++) if(pci_msg_spy[i]) delete pci_msg_spy[i];
	for(unsigned int i = 0; i < MAX_IRQ_TRANSACTION_SPY; i++) if(irq_msg_spy[i]) delete irq_msg_spy[i];

	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	while(!elf32_loader.empty()) {
		elf32_loader.pop_back();
		filename.pop_back();
	}
	if(bus) delete bus;
	if(cpu) delete cpu;
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(l2_power_estimator) delete l2_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(time) delete time;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
	if(tee_symbol_table_lookup) delete tee_symbol_table_lookup;
	if(flash) delete flash;
	if(erom) delete erom;
	if(mpc107) delete mpc107;
	if(pci_bus) delete pci_bus;
#ifdef WITH_PCI_STUB
	if(pci_stub) delete pci_stub;
#endif

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
