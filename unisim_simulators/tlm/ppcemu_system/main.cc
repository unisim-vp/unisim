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
 */
 
#include <unisim/component/tlm/processor/powerpc/powerpc.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.hh>
#include <iostream>
#include <map>
#include <getopt.h>
#include <unisim/kernel/service/service.hh>
#include <stdlib.h>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/component/tlm/memory/ram/memory.hh>
#include <unisim/component/tlm/memory/flash/am29lv/am29lv.hh>
#include <unisim/component/tlm/pci/video/display.hh>
#include <unisim/component/tlm/fsb/snooping_bus/bus.hh>
#include <unisim/component/tlm/chipset/mpc107/mpc107.hh>
#include <unisim/util/garbage_collector/garbage_collector.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/component/tlm/pci/bus/bus.hh>
#include <unisim/component/tlm/pci/ide/pci_ide_module.hh>
#include <unisim/component/tlm/pci/macio/heathrow.hh>
#include <unisim/component/cxx/pci/types.hh>
#include <unisim/component/cxx/processor/powerpc/config.hh>
#include <stdexcept>
#include <unisim/service/sdl/sdl.hh>
#include <unisim/component/tlm/bridge/pci_isa/bridge.hh>
#include <unisim/component/tlm/isa/i8042/i8042.hh>
#include <unisim/component/tlm/debug/transaction_spy.hh>
#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef DEBUG_PPCEMU_SYSTEM
typedef unisim::component::cxx::processor::powerpc::MPC7447ADebugConfig CPU_CONFIG;
#else
typedef unisim::component::cxx::processor::powerpc::MPC7447AConfig CPU_CONFIG;
#endif

static const bool DEBUG_INFORMATION = false;

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
using unisim::service::loader::pmac_linux_kernel_loader::PMACLinuxKernelLoader;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::component::cxx::pci::pci64_address_t;
using unisim::component::cxx::pci::pci32_address_t;
using unisim::kernel::service::ServiceManager;

void help(char *prog_name)
{
	cerr << "Usage: " << prog_name << " [<options>] [<linux kernel>] [linux kernel arguments]" << endl << endl;
	cerr << "       'linux kernel' is an ELF32 uncompressed Linux kernel (vmlinux)" << endl;
	cerr << "       A 'Mac OS BootX' loader is emulated instead of directly running an open firmware and a boot loader" << endl << endl;
	cerr << "Options:" << endl;
	cerr << "--inline-debugger" << endl;
	cerr << "-i" << endl;
	cerr << "            starts the inline debugger" << endl;
	cerr << "--gdb-server <TCP port>" << endl;
	cerr << "-d <TCP port>" << endl;
	cerr << "-p" << endl;
	cerr << "--power" << endl;
	cerr << "            estimate power consumption of caches and TLBs" << endl << endl;
	cerr << "--message-spy" << endl;
	cerr << "-m" << endl;
	cerr << "            enable message spy" << endl << endl;
	cerr << " --config <xml file>" << endl;
	cerr << " -c <xml file>" << endl;
	cerr << "            configures the simulator with the given xml configuration file" << endl << endl;
	cerr << " --get-config <xml file>" << endl;
	cerr << " -g <xml file>" << endl;
	cerr << "            get the simulator default configuration xml file (you can use it to create your own configuration)" << endl;
	cerr << "            This option can be combined with -c to get a new configuration file with existing variables from another file" << endl;
	cerr << "--help" << endl;
	cerr << "-h" << endl;
	cerr << "            displays this help" << endl;
}

//=========================================================================
//===                       Constants definitions                       ===
//=========================================================================

// Front Side Bus template parameters
typedef CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;

// PCI Bus template parameters
typedef pci32_address_t PCI_ADDRESS_TYPE;
const uint32_t PCI_MAX_DATA_SIZE = 32;        // in bytes
const unsigned int PCI_NUM_MASTERS = 2;
const unsigned int PCI_NUM_TARGETS = 5;
const unsigned int PCI_NUM_MAPPINGS = 10;

// ISA Bus template parameters
const uint32_t ISA_MAX_DATA_SIZE = PCI_MAX_DATA_SIZE;

// PCI device numbers
const unsigned int PCI_MPC107_DEV_NUM = 0;
const unsigned int PCI_HEATHROW_DEV_NUM = 1;
const unsigned int PCI_IDE_DEV_NUM = 2;
const unsigned int PCI_DISPLAY_DEV_NUM = 3;
const unsigned int PCI_ISA_BRIDGE_DEV_NUM = 4;

// PCI target port numbers
const unsigned int PCI_MPC107_SLAVE_PORT = 0;
const unsigned int PCI_HEATHROW_SLAVE_PORT = 1;
const unsigned int PCI_IDE_SLAVE_PORT = 2;
const unsigned int PCI_DISPLAY_SLAVE_PORT = 3;
const unsigned int PCI_ISA_BRIDGE_SLAVE_PORT = 4;

// PCI master port numbers
const unsigned int PCI_MPC107_MASTER_PORT = 0;
const unsigned int PCI_IDE_MASTER_PORT = 1;

// Heathrow PIC interrupts
const unsigned int PCI_IDE_IRQ = 47;
const unsigned int I8042_KBD_IRQ = 1;
const unsigned int I8042_AUX_IRQ = 12;

// the maximum number of transaction spies (per type of message)
const unsigned int MAX_BUS_TRANSACTION_SPY = 4;
const unsigned int MAX_MEM_TRANSACTION_SPY = 3;
const unsigned int MAX_PCI_TRANSACTION_SPY = 7;
const unsigned int MAX_IRQ_TRANSACTION_SPY = 4;

// Flash memory
const uint32_t FLASH_BYTESIZE = 4 * unisim::component::cxx::memory::flash::am29lv::M; // 4 MB
const uint32_t FLASH_IO_WIDTH = 8; // 64 bits
typedef unisim::component::cxx::memory::flash::am29lv::AM29LV800BConfig FLASH_CONFIG;

//=========================================================================
//===                     Aliases for components classes                ===
//=========================================================================

typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1> FRONT_SIDE_BUS;
typedef unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
typedef unisim::component::tlm::memory::flash::am29lv::AM29LV<FLASH_CONFIG, FLASH_BYTESIZE, FLASH_IO_WIDTH, FSB_MAX_DATA_SIZE> FLASH;
typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION> PCI_BUS;
typedef unisim::component::tlm::processor::powerpc::PowerPC<CPU_CONFIG> CPU;
typedef unisim::component::tlm::chipset::mpc107::MPC107<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, DEBUG_INFORMATION> MPC107;
typedef unisim::component::tlm::pci::ide::PCIDevIde<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_IDE;
typedef unisim::component::tlm::pci::macio::Heathrow<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> HEATHROW;
typedef unisim::component::tlm::pci::video::Display<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_DISPLAY;
typedef unisim::component::tlm::bridge::pci_isa::Bridge<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_ISA_BRIDGE;
typedef unisim::component::tlm::isa::i8042::I8042<ISA_MAX_DATA_SIZE> I8042;

//=========================================================================
//===               Aliases for transaction Spies classes               ===
//=========================================================================

typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1>::ReqType BusMsgReqType;
typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1>::RspType BusMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<BusMsgReqType, BusMsgRspType> BusMsgSpyType;
typedef unisim::component::tlm::message::MemoryRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MemMsgReqType;
typedef unisim::component::tlm::message::MemoryResponse<FSB_MAX_DATA_SIZE> MemMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<MemMsgReqType, MemMsgRspType> MemMsgSpyType;
typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION>::ReqType PCIMsgReqType;
typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION>::RspType PCIMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<PCIMsgReqType, PCIMsgRspType> PCIMsgSpyType;
typedef unisim::component::tlm::message::InterruptRequest IRQReqSpyType;
typedef unisim::component::tlm::debug::TransactionSpy<IRQReqSpyType> IRQMsgSpyType;

extern char **environ;

//=========================================================================
//===                             Components                            ===
//=========================================================================
//  - PowerPC processor
CPU *cpu = 0;
//  - Front side bus
FRONT_SIDE_BUS *bus = 0;
//  - MPC107 chipset
MPC107 *mpc107 = 0;
//  - RAM
MEMORY *memory = 0;
//  - EROM
MEMORY *erom = 0;
//  - Flash memory
FLASH *flash = 0;
//  - PCI Bus
PCI_BUS *pci_bus = 0;
//  - PCI PIIX4 IDE controller
PCI_IDE *pci_ide = 0;
//  - PCI Heathrow PIC controller
HEATHROW *heathrow = 0;
//  - PCI Display (just a frame buffer for now)
PCI_DISPLAY *pci_display = 0;
//  - PCI to ISA Bridge
PCI_ISA_BRIDGE *pci_isa_bridge = 0;
//  - i8042 keyboard controller
I8042 *i8042 = 0;

//=========================================================================
//===            Debugging stuff: Transaction spy instantiations        ===
//=========================================================================
BusMsgSpyType *bus_msg_spy[MAX_BUS_TRANSACTION_SPY];
MemMsgSpyType *mem_msg_spy[MAX_MEM_TRANSACTION_SPY];
PCIMsgSpyType *pci_msg_spy[MAX_PCI_TRANSACTION_SPY];
IRQMsgSpyType *irq_msg_spy[MAX_IRQ_TRANSACTION_SPY];

//=========================================================================
//===                            Services                               ===
//=========================================================================
//  - Simple Direct Media Layer (www.libsdl.org)
unisim::service::sdl::SDL<PCI_ADDRESS_TYPE> *sdl = 0;
//  - PowerMac Linux kernel loader
//    A Linux kernel loader acting as a firmware and a bootloader of a real PowerMac machine
PMACLinuxKernelLoader *kloader = 0;
//  - GDB server
GDBServer<CPU_ADDRESS_TYPE> *gdb_server = 0;
//  - Inline debugger
InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger = 0;
//  - SystemC Time
unisim::service::time::sc_time::ScTime *sim_time = 0;
//  - Host Time
unisim::service::time::host_time::HostTime *host_time = 0;
//  - the optional power estimators
CachePowerEstimator *il1_power_estimator = 0;
CachePowerEstimator *dl1_power_estimator = 0;
CachePowerEstimator *l2_power_estimator = 0;
CachePowerEstimator *itlb_power_estimator = 0;
CachePowerEstimator *dtlb_power_estimator = 0;

void LoadDefaultRunTimeConfiguration()
{
	// Default run-time configuration
	int gdb_server_tcp_port = 1234;
	const char *device_tree_filename = "device_tree.xml";
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	const char *ramdisk_filename = "initrd.img.gz";
	const char *bmp_out_filename = "capture";
	const char *keymap_filename = "keymap.xml";
	uint64_t max_inst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	uint32_t pci_bus_frequency = 33; // in Mhz
	uint32_t isa_bus_frequency = 8; // in Mhz
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	double fsb_frequency = cpu_frequency / cpu_clock_multiplier; // FIXME: to be removed
	uint32_t tech_node = 130; // in nm
	uint32_t display_width = 640; // in pixels
	uint32_t display_height = 480; // in pixels
	uint32_t display_depth = 15; // in bits per pixel
	uint32_t display_vfb_size = 8 * 1024 * 1024; // 8 MB
	uint32_t video_refresh_period = 40; // every 40 ms (25 fps)
	uint32_t memory_size = 256 * 1024 * 1024; // 256 MB
	double cpu_ipc = 1.0; // in instructions per cycle
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;

	//  - Front Side Bus
	if(bus)
	{
		(*bus)["cycle-time"] = fsb_cycle_time;
	}

	//	- PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	if(cpu)
	{
		(*cpu)["cpu-cycle-time"] = cpu_cycle_time;
		(*cpu)["bus-cycle-time"] = fsb_cycle_time;
		(*cpu)["voltage"] = 1.3 * 1e3; // mV
		(*cpu)["nice-time"] = 1000000000; // 1 ms
		(*cpu)["max-inst"] = max_inst;
		(*cpu)["ipc"] = cpu_ipc;
	}

	//  - RAM
	if(memory)
	{
		(*memory)["cycle-time"] = mem_cycle_time;
		(*memory)["org"] = 0x00000000UL;
		(*memory)["bytesize"] = memory_size;
	}

	// MPC107 run-time configuration
	if(mpc107)
	{
		(*mpc107)["a_address_map"] = false;
		(*mpc107)["host_mode"] = true;
		(*mpc107)["memory_32bit_data_bus_size"] = true;
		(*mpc107)["rom0_8bit_data_bus_size"] = false;
		(*mpc107)["rom1_8bit_data_bus_size"] = false;
		(*mpc107)["frequency"] = fsb_frequency;
		(*mpc107)["sdram_cycle_time"] = mem_cycle_time;
	}

	//  - EROM run-time configuration
	if(erom)
	{
		(*erom)["org"] =  0x78000000UL;
		(*erom)["bytesize"] = 2 * 8 * 1024 * 1024;
		(*erom)["cycle-time"] = mem_cycle_time;
	}

	//  - Flash memory run-time configuration
	if(flash)
	{
		(*flash)["org"] = 0xff800000UL; //0xff000000UL;
		(*flash)["bytesize"] = 8 * 1024 * 1024;
		(*flash)["cycle-time"] = mem_cycle_time;
		(*flash)["endian"] = "big-endian";
	}

	// PCI Bus run-time configuration
	if(pci_bus)
	{
		unsigned int mapping_index = 0;
		(*pci_bus)["frequency"] = pci_bus_frequency;

		(*pci_bus)["base-address"][mapping_index] = 0;
		(*pci_bus)["size"][mapping_index] = 1024 * 1024 * 1024;
		(*pci_bus)["device-number"][mapping_index] = PCI_MPC107_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_MPC107_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x10;
		(*pci_bus)["addr-type"][mapping_index] = "mem";
		(*pci_bus)["num-mappings"] = ++mapping_index;

		(*pci_bus)["base-address"][mapping_index] = 0xf3000000UL;
		(*pci_bus)["size"][mapping_index] = 0x80000;
		(*pci_bus)["device-number"][mapping_index] = PCI_HEATHROW_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_HEATHROW_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x10UL;
		(*pci_bus)["addr-type"][mapping_index] = "mem";
		(*pci_bus)["num-mappings"] = ++mapping_index;
		
		(*pci_bus)["base-address"][mapping_index] = 0x18100UL;
		(*pci_bus)["size"][mapping_index] = 8;
		(*pci_bus)["device-number"][mapping_index] = PCI_IDE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_IDE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x10UL;
		(*pci_bus)["addr-type"][mapping_index] = "i/o";
		(*pci_bus)["num-mappings"] = ++mapping_index;
		
		(*pci_bus)["base-address"][mapping_index] = 0x18108UL;
		(*pci_bus)["size"][mapping_index] = 4;
		(*pci_bus)["device-number"][mapping_index] = PCI_IDE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_IDE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x14UL;
		(*pci_bus)["addr-type"][mapping_index] = "i/o";
		(*pci_bus)["num-mappings"] = ++mapping_index;
		
		(*pci_bus)["base-address"][mapping_index] = 0x4UL;
		(*pci_bus)["size"][mapping_index] = 8;
		(*pci_bus)["device-number"][mapping_index] = PCI_IDE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_IDE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x18UL;
		(*pci_bus)["addr-type"][mapping_index] = "i/o";
		(*pci_bus)["num-mappings"] = ++mapping_index;
		
		(*pci_bus)["base-address"][mapping_index] = 0xcUL;
		(*pci_bus)["size"][mapping_index] = 4;
		(*pci_bus)["device-number"][mapping_index] = PCI_IDE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_IDE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x1cUL;
		(*pci_bus)["addr-type"][mapping_index] = "i/o";
		(*pci_bus)["num-mappings"] = ++mapping_index;
		
		(*pci_bus)["base-address"][mapping_index] = 0x18118UL;
		(*pci_bus)["size"][6] = 16;
		(*pci_bus)["device-number"][mapping_index] = PCI_IDE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_IDE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x20UL;
		(*pci_bus)["addr-type"][mapping_index] = "i/o";
		(*pci_bus)["num-mappings"] = ++mapping_index;

		(*pci_bus)["base-address"][mapping_index] = 0xa0000000UL;
		(*pci_bus)["size"][mapping_index] = 0x800000;
		(*pci_bus)["device-number"][mapping_index] = PCI_DISPLAY_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_DISPLAY_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x10UL;
		(*pci_bus)["addr-type"][mapping_index] = "mem";
		(*pci_bus)["num-mappings"] = ++mapping_index;
		
		(*pci_bus)["base-address"][mapping_index] = 0; //0xfe000000UL; // ISA I/O is at the very beginning of PCI I/O space
		(*pci_bus)["size"][mapping_index] = 0x10000; // 64 KB
		(*pci_bus)["device-number"][mapping_index] = PCI_ISA_BRIDGE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_ISA_BRIDGE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x10UL; // ISA I/O space mapped by BAR0
		(*pci_bus)["addr-type"][mapping_index] = "i/o";
		(*pci_bus)["num-mappings"] = ++mapping_index;

		(*pci_bus)["base-address"][mapping_index] = 0x000a0000UL; // ISA Memory is at the very beginning of compatibility hole
		(*pci_bus)["size"][mapping_index] = 0x60000; // 384 KB
		(*pci_bus)["device-number"][mapping_index] = PCI_ISA_BRIDGE_DEV_NUM;
		(*pci_bus)["target-port"][mapping_index] = PCI_ISA_BRIDGE_SLAVE_PORT;
		(*pci_bus)["register-number"][mapping_index] = 0x14UL; // ISA Memory space mapped by BAR1
		(*pci_bus)["addr-type"][mapping_index] = "mem";
		(*pci_bus)["num-mappings"] = ++mapping_index;
	}

	//  - PCI MAC I/O Heathrow run-time configuration
	if(heathrow)
	{
		(*heathrow)["initial-base-addr"] = 0xf3000000UL;
		(*heathrow)["pci-device-number"] = PCI_HEATHROW_DEV_NUM;
		(*heathrow)["bus-frequency"] = pci_bus_frequency;
	}

	//  - PCI IDE run-time configuration
	if(pci_ide)
	{
		(*pci_ide)["device-number"] = PCI_IDE_DEV_NUM;
		(*pci_ide)["base-address"][0] = 0x18101;
		(*pci_ide)["size"][0] = 8;
		(*pci_ide)["register-number"][0] = 0x10;
		(*pci_ide)["base-address"][1] = 0x18109;
		(*pci_ide)["size"][1] = 4;
		(*pci_ide)["register-number"][1] = 0x14;
		(*pci_ide)["base-address"][2] = 0x5;
		(*pci_ide)["size"][2] = 8;
		(*pci_ide)["register-number"][2] = 0x18;
		(*pci_ide)["base-address"][3] = 0xd;
		(*pci_ide)["size"][3] = 4;
		(*pci_ide)["register-number"][3] = 0x1c;
		(*pci_ide)["base-address"][4] = 0x18119;
		(*pci_ide)["size"][4] = 16;
		(*pci_ide)["register-number"][4] = 0x20;
	}

	//  - Display run-time configuration
	if(pci_display)
	{
		(*pci_display)["initial-base-addr"] = 0xa0000000UL;
		(*pci_display)["bytesize"] = display_vfb_size; 
		(*pci_display)["width"] = display_width;
		(*pci_display)["height"] = display_height;
		(*pci_display)["depth"] = display_depth;
		(*pci_display)["pci-bus-frequency"] = pci_bus_frequency;
	}

	// - PCI-ISA Bridge run-time configuration
	if(pci_isa_bridge)
	{
		(*pci_isa_bridge)["initial-base-addr"] = 0x000a0000UL;
		(*pci_isa_bridge)["initial-io-base-addr"] = 0; //0xfe000000UL;
		(*pci_isa_bridge)["pci-bus-frequency"] = pci_bus_frequency;
		(*pci_isa_bridge)["isa-bus-frequency"] = isa_bus_frequency;
		(*pci_isa_bridge)["pci-device-number"] = PCI_ISA_BRIDGE_DEV_NUM;
	}

	//  - i8042 run-time configuration
	if(i8042)
	{
		(*i8042)["fsb-frequency"] = fsb_frequency;
		(*i8042)["isa-bus-frequency"] = isa_bus_frequency;
	}
	
	// - Cache/TLB power estimators run-time configuration
	if(il1_power_estimator)
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
	}
	
	if(dl1_power_estimator)
	{
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
	}
	
	if(l2_power_estimator)
	{
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
	}
	
	if(itlb_power_estimator)
	{
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
	}
	
	if(dtlb_power_estimator)
	{
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

	if(gdb_server)
	{
		(*gdb_server)["tcp-port"] = gdb_server_tcp_port;
		(*gdb_server)["architecture-description-filename"] = gdb_server_arch_filename;
	}

	//  - SDL run-time configuration
	if(sdl)
	{
		(*sdl)["refresh-period"] = video_refresh_period;
		(*sdl)["bmp-out-filename"] = bmp_out_filename;	
		(*sdl)["keymap-filename"] = keymap_filename;	
	}

	// - Kernel loader configuration
	if(kloader)
	{
		(*kloader)["pmac-bootx.device-tree-filename"] = device_tree_filename;
		(*kloader)["pmac-bootx.kernel-params"] = ""; // no kernel parameters
		(*kloader)["pmac-bootx.ramdisk-filename"] = ramdisk_filename;
		(*kloader)["pmac-bootx.screen-width"] = display_width;
		(*kloader)["pmac-bootx.screen-height"] = display_height;
		(*kloader)["elf32-loader.base-addr"] = 0x00400000UL;
	}
}

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
	{"inline-debugger", no_argument, 0, 'i'},
	{"gdb-server", required_argument, 0, 'd'},
	{"help", no_argument, 0, 'h'},
	{"power", no_argument, 0, 'p'},
	{"message-spy", no_argument, 0, 'm'},
	{"get-config", required_argument, 0, 'g'},
	{"config", required_argument, 0, 'c'},
	{"set-variable", required_argument, 0, 's'},
	{0, 0, 0, 0}
	};

	int c;
	bool use_gdb_server = false;
	bool use_inline_debugger = false;
	bool estimate_power = false;
	int gdb_server_tcp_port = 0;
	char const *set_config_name = "default_config.xml";
	char const *get_config_name = "default_config.xml";
	bool get_config = false;
	bool set_config = false;
	bool message_spy = false;
	std::list<string> set_vars;
	
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

	
	// Parse the command line arguments
	while((c = getopt_long (argc, argv, optstring, long_options, 0)) != -1)
	{
		switch(c)
		{
			case 'i':
				use_inline_debugger = true;
				break;
			case 'd':
				use_gdb_server = true;
				gdb_server_tcp_port = atoi(optarg);
				break;
			case 'h':
				help(argv[0]);
				return 0;
			case 'p':
				estimate_power = true;
				break;
			case 'm':
				message_spy = true;
				break;
			case 'g':
				get_config_name = optarg;
				get_config = true;
				break;
			case 'c':
				set_config_name = optarg;
				set_config = true;
				break;
			case 's':
				set_vars.push_back(string(optarg));
				break;
		}
	}

	char *filename = argv[optind];
	int sim_argc = argc - optind;
	char **sim_argv = argv + optind;
	char **sim_envp = environ;

	// Build the kernel parameters string from the command line arguments
	string kernel_params;
	
	if(filename)
	{
		int i;

		for(i = 1; i < sim_argc; i++)
		{
			kernel_params += sim_argv[i];
			if(i < sim_argc - 1) kernel_params += " ";
		}
	}
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu =new CPU("cpu");
	//  - Front side bus
	bus = new FRONT_SIDE_BUS("bus");
	//  - MPC107 chipset
	mpc107 = new MPC107("mpc107");
	//  - RAM
	memory = new MEMORY("memory");
	//  - EROM
	erom = new MEMORY("erom");
	//  - Flash memory
	flash = new FLASH("flash");
	//  - PCI Bus
	pci_bus = new PCI_BUS("pci-bus");
	//  - PCI PIIX4 IDE controller
	pci_ide = new PCI_IDE("pci-ide");
	//  - PCI Heathrow PIC controller
	heathrow = new HEATHROW("heathrow");
	//  - PCI Display (just a frame buffer for now)
	pci_display = new PCI_DISPLAY("pci-display");
	//  - PCI to ISA Bridge
	pci_isa_bridge = new PCI_ISA_BRIDGE("pci-isa-bridge");
	//  - i8042 keyboard controller
	i8042 = new I8042("i8042");

	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================

	if(message_spy)
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
	//  - Simple Direct Media Layer (www.libsdl.org)
	sdl = new unisim::service::sdl::SDL<PCI_ADDRESS_TYPE>("sdl");
	//  - PowerMac Linux kernel loader
	//    A Linux kernel loader acting as a firmware and a bootloader of a real PowerMac machine
	kloader = new PMACLinuxKernelLoader("pmac-linux-kernel-loader");
	//  - GDB server
	gdb_server = (use_gdb_server || get_config) ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = (use_inline_debugger || get_config) ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - the optional power estimators
	il1_power_estimator = (estimate_power || get_config) ? new CachePowerEstimator("il1-power-estimator") : 0;
	dl1_power_estimator = (estimate_power || get_config) ? new CachePowerEstimator("dl1-power-estimator") : 0;
	l2_power_estimator = (estimate_power || get_config) ? new CachePowerEstimator("l2-power-estimator") : 0;
	itlb_power_estimator = (estimate_power || get_config) ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = (estimate_power || get_config) ? new CachePowerEstimator("dtlb-power-estimator") : 0;

	//=========================================================================
	//===                        Run-time configuration                     ===
	//=========================================================================

	// From the default set of parameters
	LoadDefaultRunTimeConfiguration();
	
	//=========================================================================
	//===                    Overload of run-time configuration             ===
	//=========================================================================

	// From a configuration file
	if(set_config)
	{
		ServiceManager::LoadXmlParameters(set_config_name);
		cerr << "Parameters set using file \"" << set_config_name << "\"" << endl;
	}

	//  - GDB Server run-time configuration
	if(gdb_server)
	{
		cerr << "Using " << (*gdb_server)["tcp-port"].GetName() << " = " << gdb_server_tcp_port << endl;
		(*gdb_server)["tcp-port"] = gdb_server_tcp_port;
	}

	//  - Loader run-time configuration
	if(filename)
	{
		cerr << "Using " << (*kloader)["elf32-loader.filename"].GetName() << " = \"" << filename << "\"" << endl;
		(*kloader)["elf32-loader.filename"] = filename;
	}

	if(!kernel_params.empty())
	{
		cerr << "Using " << (*kloader)["pmac-bootx.kernel-params"].GetName() << " = \"" << kernel_params << "\"" << endl;
		(*kloader)["pmac-bootx.kernel-params"] = kernel_params.c_str();
	}

	//=========================================================================
	//===                      Save run-time configuration                  ===
	//=========================================================================

	if(get_config)
	{
		ServiceManager::XmlfyParameters(get_config_name);
		cerr << "Parameters saved on file \"" << get_config_name << "\"" << endl;
		return 0;
	}

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	if(message_spy)
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

		(*pci_bus->output_port[PCI_MPC107_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_MPC107_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(mpc107->pci_slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = mpc107->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = mpc107->pci_slave_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_HEATHROW_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_HEATHROW_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(heathrow->bus_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = heathrow->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = heathrow->bus_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_IDE_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_IDE_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_ide->input_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_ide->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_ide->input_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_DISPLAY_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_DISPLAY_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_display->bus_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_display->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_display->bus_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_ISA_BRIDGE_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_ISA_BRIDGE_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_isa_bridge->pci_slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_isa_bridge->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_isa_bridge->pci_slave_port.name();
		pci_msg_spy_index++;

		pci_ide->output_port(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_ide->name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_ide->output_port.name();
		pci_msg_spy[pci_msg_spy_index]->master_port(*pci_bus->input_port[PCI_IDE_MASTER_PORT]);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_bus->name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_bus->input_port[PCI_IDE_MASTER_PORT]->name();
		pci_msg_spy_index++;
		
		pci_ide->irq_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = pci_ide->name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = pci_ide->irq_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(*heathrow->irq_port[PCI_IDE_IRQ]);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = heathrow->name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = heathrow->irq_port[PCI_IDE_IRQ]->name();
		irq_msg_spy_index++;

		heathrow->cpu_irq_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = heathrow->name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = heathrow->cpu_irq_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(*mpc107->irq_slave_port[0]);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = mpc107->name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = mpc107->irq_slave_port[0]->name();
		irq_msg_spy_index++;

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
	}
	else
	{
		// Connect the CPU to the Front Side Bus
		cpu->bus_port(*bus->inport[0]);
		(*bus->outport[0])(cpu->snoop_port);

		(*bus->chipset_outport)(mpc107->slave_port);
		mpc107->master_port(*bus->chipset_inport);
		mpc107->ram_master_port(memory->slave_port);
		mpc107->rom_master_port(flash->slave_port);
		mpc107->erom_master_port(erom->slave_port);
		mpc107->pci_master_port(*pci_bus->input_port[PCI_MPC107_MASTER_PORT]);
	
		(*pci_bus->output_port[PCI_MPC107_SLAVE_PORT])(mpc107->pci_slave_port);
		(*pci_bus->output_port[PCI_HEATHROW_SLAVE_PORT])(heathrow->bus_port);
		(*pci_bus->output_port[PCI_IDE_SLAVE_PORT])(pci_ide->input_port);
		(*pci_bus->output_port[PCI_DISPLAY_SLAVE_PORT])(pci_display->bus_port);
		(*pci_bus->output_port[PCI_ISA_BRIDGE_SLAVE_PORT])(pci_isa_bridge->pci_slave_port);
		pci_ide->output_port (*pci_bus->input_port[PCI_IDE_MASTER_PORT]);
		pci_ide->irq_port (*heathrow->irq_port[PCI_IDE_IRQ]);
		heathrow->cpu_irq_port(*mpc107->irq_slave_port[0]);
		mpc107->irq_master_port(cpu->external_interrupt_port);
		mpc107->soft_reset_master_port(cpu->soft_reset_port);
	}

	pci_isa_bridge->isa_master_port(i8042->bus_port);
	i8042->kbd_irq_port(*heathrow->irq_port[I8042_KBD_IRQ]);
	i8042->aux_irq_port(*heathrow->irq_port[I8042_AUX_IRQ]);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> bus->memory_export;
	
	if(inline_debugger)
	{
		// Connect inline-debugger to CPU
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
	}
	else if(gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
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

		il1_power_estimator->time_import >> sim_time->time_export;
		dl1_power_estimator->time_import >> sim_time->time_export;
		l2_power_estimator->time_import >> sim_time->time_export;
		itlb_power_estimator->time_import >> sim_time->time_export;
		dtlb_power_estimator->time_import >> sim_time->time_export;
	}

	kloader->memory_import >> memory->memory_export;
	kloader->registers_import >> cpu->registers_export;
	cpu->kernel_loader_import >> kloader->loader_export;
	cpu->symbol_table_lookup_import >> kloader->symbol_table_lookup_export;
	bus->memory_import >> mpc107->memory_export;
	pci_display->video_import >> sdl->video_export;
	sdl->memory_import >> pci_display->memory_export;
	mpc107->ram_import >> memory->memory_export;
	mpc107->rom_import >> flash->memory_export;
	mpc107->erom_import >> erom->memory_export;
	mpc107->pci_import >> *pci_bus->memory_export[PCI_MPC107_MASTER_PORT];
	i8042->keyboard_import >> sdl->keyboard_export;

	if(inline_debugger)
	{
		inline_debugger->symbol_table_lookup_import >> kloader->symbol_table_lookup_export;
	}

#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

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
	if(bus) delete bus;
	if(cpu) delete cpu;
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(l2_power_estimator) delete l2_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(flash) delete flash;
	if(erom) delete erom;
	if(pci_display) delete pci_display;
	if(pci_isa_bridge) delete pci_isa_bridge;
	if(i8042) delete i8042;
	if(sdl) delete sdl;
	if(kloader) delete kloader;
	if(pci_ide) delete pci_ide;
	if(mpc107) delete mpc107;
	if(pci_bus) delete pci_bus;
	if(heathrow) delete heathrow;

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
