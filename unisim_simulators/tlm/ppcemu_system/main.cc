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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/component/tlm/processor/powerpc/mpc7447a/cpu.hh>
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
#include <unisim/component/cxx/processor/powerpc/mpc7447a/config.hh>
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
typedef unisim::component::cxx::processor::powerpc::mpc7447a::DebugConfig CPU_CONFIG;
static const bool DEBUG_INFORMATION = true;
#else
typedef unisim::component::cxx::processor::powerpc::mpc7447a::Config CPU_CONFIG;
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
using unisim::service::loader::pmac_linux_kernel_loader::PMACLinuxKernelLoader;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::component::cxx::pci::pci64_address_t;
using unisim::component::cxx::pci::pci32_address_t;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;

class Simulator : public unisim::kernel::service::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual unisim::kernel::service::Simulator::SetupStatus Setup();
	void Run();
	virtual void Stop(Object *object, int exit_status);
protected:
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// Front Side Bus template parameters
	typedef CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
	typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	//typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
	static const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
	static const uint32_t FSB_NUM_PROCS = 1;

	// PCI Bus template parameters
	typedef pci32_address_t PCI_ADDRESS_TYPE;
	static const uint32_t PCI_MAX_DATA_SIZE = 32;        // in bytes
	static const unsigned int PCI_NUM_MASTERS = 2;
	static const unsigned int PCI_NUM_TARGETS = 5;
	static const unsigned int PCI_NUM_MAPPINGS = 10;

	// ISA Bus template parameters
	static const uint32_t ISA_MAX_DATA_SIZE = PCI_MAX_DATA_SIZE;

	// PCI device numbers
	static const unsigned int PCI_MPC107_DEV_NUM = 0;
	static const unsigned int PCI_HEATHROW_DEV_NUM = 1;
	static const unsigned int PCI_IDE_DEV_NUM = 2;
	static const unsigned int PCI_DISPLAY_DEV_NUM = 3;
	static const unsigned int PCI_ISA_BRIDGE_DEV_NUM = 4;

	// PCI target port numbers
	static const unsigned int PCI_MPC107_SLAVE_PORT = 0;
	static const unsigned int PCI_HEATHROW_SLAVE_PORT = 1;
	static const unsigned int PCI_IDE_SLAVE_PORT = 2;
	static const unsigned int PCI_DISPLAY_SLAVE_PORT = 3;
	static const unsigned int PCI_ISA_BRIDGE_SLAVE_PORT = 4;

	// PCI master port numbers
	static const unsigned int PCI_MPC107_MASTER_PORT = 0;
	static const unsigned int PCI_IDE_MASTER_PORT = 1;

	// Heathrow PIC interrupts
	static const unsigned int PCI_IDE_IRQ = 47;
	static const unsigned int I8042_KBD_IRQ = 1;
	static const unsigned int I8042_AUX_IRQ = 12;

	// the maximum number of transaction spies (per type of message)
	static const unsigned int MAX_BUS_TRANSACTION_SPY = 4;
	static const unsigned int MAX_MEM_TRANSACTION_SPY = 3;
	static const unsigned int MAX_PCI_TRANSACTION_SPY = 7;
	static const unsigned int MAX_IRQ_TRANSACTION_SPY = 4;

	// Flash memory
	static const uint32_t FLASH_BYTESIZE = 4 * unisim::component::cxx::memory::flash::am29lv::M; // 4 MB
	static const uint32_t FLASH_IO_WIDTH = 8; // 64 bits
	typedef unisim::component::cxx::memory::flash::am29lv::AM29LV800BConfig FLASH_CONFIG;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1> FRONT_SIDE_BUS;
	typedef unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
	typedef unisim::component::tlm::memory::flash::am29lv::AM29LV<FLASH_CONFIG, FLASH_BYTESIZE, FLASH_IO_WIDTH, FSB_MAX_DATA_SIZE> FLASH;
	typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION> PCI_BUS;
	typedef unisim::component::tlm::processor::powerpc::mpc7447a::CPU<CPU_CONFIG> CPU;
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

	//=========================================================================
	//===                             Components                            ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - Front side bus
	FRONT_SIDE_BUS *bus;
	//  - MPC107 chipset
	MPC107 *mpc107;
	//  - RAM
	MEMORY *memory;
	//  - EROM
	MEMORY *erom;
	//  - Flash memory
	FLASH *flash;
	//  - PCI Bus
	PCI_BUS *pci_bus;
	//  - PCI PIIX4 IDE controller
	PCI_IDE *pci_ide;
	//  - PCI Heathrow PIC controller
	HEATHROW *heathrow;
	//  - PCI Display (just a frame buffer for now)
	PCI_DISPLAY *pci_display;
	//  - PCI to ISA Bridge
	PCI_ISA_BRIDGE *pci_isa_bridge;
	//  - i8042 keyboard controller
	I8042 *i8042;
	
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
	unisim::service::sdl::SDL<PCI_ADDRESS_TYPE> *sdl;
	//  - PowerMac Linux kernel loader
	//    A Linux kernel loader acting as a firmware and a bootloader of a real PowerMac machine
	PMACLinuxKernelLoader *kloader;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - the optional power estimators
	CachePowerEstimator *il1_power_estimator;
	CachePowerEstimator *dl1_power_estimator;
	CachePowerEstimator *l2_power_estimator;
	CachePowerEstimator *itlb_power_estimator;
	CachePowerEstimator *dtlb_power_estimator;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool estimate_power;
	bool message_spy;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_estimate_power;
	Parameter<bool> param_message_spy;

	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, bus(0)
	, mpc107(0)
	, memory(0)
	, erom(0)
	, flash(0)
	, pci_bus(0)
	, pci_ide(0)
	, heathrow(0)
	, pci_display(0)
	, pci_isa_bridge(0)
	, i8042(0)
	, sdl(0)
	, kloader(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, l2_power_estimator(0)
	, itlb_power_estimator(0)
	, dtlb_power_estimator(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, message_spy(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, param_message_spy("message-spy", 0, message_spy, "Enable/Disable message spies instantiation")
{
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
	gdb_server = (enable_gdb_server) ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = (enable_inline_debugger) ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - the optional power estimators
	il1_power_estimator = (estimate_power) ? new CachePowerEstimator("il1-power-estimator") : 0;
	dl1_power_estimator = (estimate_power) ? new CachePowerEstimator("dl1-power-estimator") : 0;
	l2_power_estimator = (estimate_power) ? new CachePowerEstimator("l2-power-estimator") : 0;
	itlb_power_estimator = (estimate_power) ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = (estimate_power) ? new CachePowerEstimator("dtlb-power-estimator") : 0;
	
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
		*inline_debugger->loader_import[0] >> kloader->loader_export;
		*inline_debugger->symbol_table_lookup_import[0] >> kloader->symbol_table_lookup_export;
		*inline_debugger->stmt_lookup_import[0] >> kloader->stmt_lookup_export;
	}
	else if(gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->disasm_import >> cpu->disasm_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->symbol_table_lookup_import >> kloader->symbol_table_lookup_export;
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
	i8042->mouse_import >> sdl->mouse_export;
}

Simulator::~Simulator()
{

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
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM ppcemu-system");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM ppcemu-system is a full system simulator of a board including a MPC7447A PowerPC processor, a MPC107 chipset, and supporting Linux boot. The simulated board is very similar to a PowerMac G4 PCI machine. Computations on IEEE 754 floating point numbers are emulated using Simfloat++. Altivec instructions are currently decoded but not implemented. The running PowerPC application is a PowerMac Linux Kernel and all the applications installed on the hard disk image and/or the initial RAM disk image. Software running on the simulated hardware can be debugged by connecting a GDB client to the simulator through the GDB serial remote protocol. The GDB client can be either the standard text based client (i.e. command gdb), a graphical front-end to GDB (e.g. ddd), or even Eclipse CDT.");


	// Default run-time configuration
	int gdb_server_tcp_port = 1234;
	const char *filename = "vmlinux";
	const char *kernel_params = "/dev/ram0 rw";
	const char *device_tree_filename = "device_tree_pmac_g4.xml";
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	const char *ramdisk_filename = "initrd.img";
	const char *bmp_out_filename = "";
	const char *keymap_filename = "pc_linux_fr_keymap.xml";
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
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//  - Front Side Bus
	simulator->SetVariable("bus.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//	- PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", cpu_cycle_time);
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.3 * 1e3); // mV
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.max-inst", max_inst);
	simulator->SetVariable("cpu.ipc", cpu_ipc);

	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", memory_size);

	// MPC107 run-time configuration
	simulator->SetVariable("mpc107.a_address_map", false);
	simulator->SetVariable("mpc107.host_mode", true);
	simulator->SetVariable("mpc107.memory_32bit_data_bus_size", true);
	simulator->SetVariable("mpc107.rom0_8bit_data_bus_size", false);
	simulator->SetVariable("mpc107.rom1_8bit_data_bus_size", false);
	simulator->SetVariable("mpc107.frequency", fsb_frequency);
	simulator->SetVariable("mpc107.sdram_cycle_time", mem_cycle_time);

	//  - EROM run-time configuration
	simulator->SetVariable("erom.org",  0x78000000UL);
	simulator->SetVariable("erom.bytesize", 2 * 8 * 1024 * 1024);
	simulator->SetVariable("erom.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());

	//  - Flash memory run-time configuration
	simulator->SetVariable("flash.org", 0xff800000UL); //0xff000000UL);
	simulator->SetVariable("flash.bytesize", 8 * 1024 * 1024);
	simulator->SetVariable("flash.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("flash.endian", "big-endian");

	// PCI Bus run-time configuration
	simulator->SetVariable("pci-bus.frequency", pci_bus_frequency);

	simulator->SetVariable("pci-bus.base-address[0]", 0);
	simulator->SetVariable("pci-bus.size[0]", 1024 * 1024 * 1024);
	simulator->SetVariable("pci-bus.device-number[0]", PCI_MPC107_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[0]", PCI_MPC107_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[0]", 0x10);
	simulator->SetVariable("pci-bus.addr-type[0]", "mem");

	simulator->SetVariable("pci-bus.base-address[1]", 0xf3000000UL);
	simulator->SetVariable("pci-bus.size[1]", 0x80000);
	simulator->SetVariable("pci-bus.device-number[1]", PCI_HEATHROW_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[1]", PCI_HEATHROW_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[1]", 0x10UL);
	simulator->SetVariable("pci-bus.addr-type[1]", "mem");
	
	simulator->SetVariable("pci-bus.base-address[2]", 0x18100UL);
	simulator->SetVariable("pci-bus.size[2]", 8);
	simulator->SetVariable("pci-bus.device-number[2]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[2]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[2]", 0x10UL);
	simulator->SetVariable("pci-bus.addr-type[2]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[3]", 0x18108UL);
	simulator->SetVariable("pci-bus.size[3]", 4);
	simulator->SetVariable("pci-bus.device-number[3]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[3]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[3]", 0x14UL);
	simulator->SetVariable("pci-bus.addr-type[3]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[4]", 0x4UL);
	simulator->SetVariable("pci-bus.size[4]", 8);
	simulator->SetVariable("pci-bus.device-number[4]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[4]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[4]", 0x18UL);
	simulator->SetVariable("pci-bus.addr-type[4]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[5]", 0xcUL);
	simulator->SetVariable("pci-bus.size[5]", 4);
	simulator->SetVariable("pci-bus.device-number[5]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[5]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[5]", 0x1cUL);
	simulator->SetVariable("pci-bus.addr-type[5]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[6]", 0x18118UL);
	simulator->SetVariable("pci-bus.size[6]", 16);
	simulator->SetVariable("pci-bus.device-number[6]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[6]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[6]", 0x20UL);
	simulator->SetVariable("pci-bus.addr-type[6]", "i/o");

	simulator->SetVariable("pci-bus.base-address[7]", 0xa0000000UL);
	simulator->SetVariable("pci-bus.size[7]", 0x800000);
	simulator->SetVariable("pci-bus.device-number[7]", PCI_DISPLAY_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[7]", PCI_DISPLAY_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[7]", 0x10UL);
	simulator->SetVariable("pci-bus.addr-type[7]", "mem");
	
	simulator->SetVariable("pci-bus.base-address[8]", 0); //0xfe000000UL); // ISA I/O is at the very beginning of PCI I/O space
	simulator->SetVariable("pci-bus.size[8]", 0x10000); // 64 KB
	simulator->SetVariable("pci-bus.device-number[8]", PCI_ISA_BRIDGE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[8]", PCI_ISA_BRIDGE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[8]", 0x10UL); // ISA I/O space mapped by BAR0
	simulator->SetVariable("pci-bus.addr-type[8]", "i/o");

	simulator->SetVariable("pci-bus.base-address[9]", 0x000a0000UL); // ISA Memory is at the very beginning of compatibility hole
	simulator->SetVariable("pci-bus.size[9]", 0x60000); // 384 KB
	simulator->SetVariable("pci-bus.device-number[9]", PCI_ISA_BRIDGE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[9]", PCI_ISA_BRIDGE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[9]", 0x14UL); // ISA Memory space mapped by BAR1
	simulator->SetVariable("pci-bus.addr-type[9]", "mem");

	simulator->SetVariable("pci-bus.num-mappings", 10);

	//  - PCI MAC I/O Heathrow run-time configuration
	simulator->SetVariable("heathrow.initial-base-addr", 0xf3000000UL);
	simulator->SetVariable("heathrow.pci-device-number", PCI_HEATHROW_DEV_NUM);
	simulator->SetVariable("heathrow.bus-frequency", pci_bus_frequency);

	//  - PCI IDE run-time configuration
	simulator->SetVariable("pci-ide.device-number", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-ide.base-address[0]", 0x18101);
	simulator->SetVariable("pci-ide.size[0]", 8);
	simulator->SetVariable("pci-ide.register-number[0]", 0x10);
	simulator->SetVariable("pci-ide.base-address[1]", 0x18109);
	simulator->SetVariable("pci-ide.size[1]", 4);
	simulator->SetVariable("pci-ide.register-number[1]", 0x14);
	simulator->SetVariable("pci-ide.base-address[2]", 0x5);
	simulator->SetVariable("pci-ide.size[2]", 8);
	simulator->SetVariable("pci-ide.register-number[2]", 0x18);
	simulator->SetVariable("pci-ide.base-address[3]", 0xd);
	simulator->SetVariable("pci-ide.size[3]", 4);
	simulator->SetVariable("pci-ide.register-number[3]", 0x1c);
	simulator->SetVariable("pci-ide.base-address[4]", 0x18119);
	simulator->SetVariable("pci-ide.size[4]", 16);
	simulator->SetVariable("pci-ide.register-number[4]", 0x20);

	//  - Display run-time configuration
	simulator->SetVariable("pci-display.initial-base-addr", 0xa0000000UL);
	simulator->SetVariable("pci-display.bytesize", display_vfb_size); 
	simulator->SetVariable("pci-display.width", display_width);
	simulator->SetVariable("pci-display.height", display_height);
	simulator->SetVariable("pci-display.depth", display_depth);
	simulator->SetVariable("pci-display.pci-bus-frequency", pci_bus_frequency);
	simulator->SetVariable("pci-display.pci-device-number", PCI_DISPLAY_DEV_NUM);

	// - PCI-ISA Bridge run-time configuration
	simulator->SetVariable("pci-isa-bridge.initial-base-addr", 0x000a0000UL);
	simulator->SetVariable("pci-isa-bridge.initial-io-base-addr", 0); //0xfe000000UL);
	simulator->SetVariable("pci-isa-bridge.pci-bus-frequency", pci_bus_frequency);
	simulator->SetVariable("pci-isa-bridge.isa-bus-frequency", isa_bus_frequency);
	simulator->SetVariable("pci-isa-bridge.pci-device-number", PCI_ISA_BRIDGE_DEV_NUM);

	//  - i8042 run-time configuration
	simulator->SetVariable("i8042.fsb-frequency", fsb_frequency);
	simulator->SetVariable("i8042.isa-bus-frequency", isa_bus_frequency);
	
	// - Cache/TLB power estimators run-time configuration
	simulator->SetVariable("il1-power-estimator.cache-size", 32 * 1024);
	simulator->SetVariable("il1-power-estimator.line-size", 32);
	simulator->SetVariable("il1-power-estimator.associativity", 8);
	simulator->SetVariable("il1-power-estimator.rw-ports", 0);
	simulator->SetVariable("il1-power-estimator.excl-read-ports", 1);
	simulator->SetVariable("il1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("il1-power-estimator.banks", 4);
	simulator->SetVariable("il1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("il1-power-estimator.output-width", 128);
	simulator->SetVariable("il1-power-estimator.tag-width", 64);
	simulator->SetVariable("il1-power-estimator.access-mode", "fast");
	
	simulator->SetVariable("dl1-power-estimator.cache-size", 32 * 1024);
	simulator->SetVariable("dl1-power-estimator.line-size", 32);
	simulator->SetVariable("dl1-power-estimator.associativity", 8);
	simulator->SetVariable("dl1-power-estimator.rw-ports", 1);
	simulator->SetVariable("dl1-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.banks", 4);
	simulator->SetVariable("dl1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dl1-power-estimator.output-width", 64);
	simulator->SetVariable("dl1-power-estimator.tag-width", 64);
	simulator->SetVariable("dl1-power-estimator.access-mode", "fast");
	
	simulator->SetVariable("l2-power-estimator.cache-size", 512 * 1024);
	simulator->SetVariable("l2-power-estimator.line-size", 32);
	simulator->SetVariable("l2-power-estimator.associativity", 8);
	simulator->SetVariable("l2-power-estimator.rw-ports", 1);
	simulator->SetVariable("l2-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("l2-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("l2-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("l2-power-estimator.banks", 4);
	simulator->SetVariable("l2-power-estimator.tech-node", tech_node);
	simulator->SetVariable("l2-power-estimator.output-width", 256);
	simulator->SetVariable("l2-power-estimator.tag-width", 64);
	simulator->SetVariable("l2-power-estimator.access-mode", "fast");
	
	simulator->SetVariable("itlb-power-estimator.cache-size", 128 * 2 * 4);
	simulator->SetVariable("itlb-power-estimator.line-size", 4);
	simulator->SetVariable("itlb-power-estimator.associativity", 2);
	simulator->SetVariable("itlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("itlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("itlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.banks", 4);
	simulator->SetVariable("itlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("itlb-power-estimator.output-width", 32);
	simulator->SetVariable("itlb-power-estimator.tag-width", 64);
	simulator->SetVariable("itlb-power-estimator.access-mode", "fast");
	
	simulator->SetVariable("dtlb-power-estimator.cache-size", 128 * 2 * 4);
	simulator->SetVariable("dtlb-power-estimator.line-size", 4);
	simulator->SetVariable("dtlb-power-estimator.associativity", 2);
	simulator->SetVariable("dtlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("dtlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.banks", 4);
	simulator->SetVariable("dtlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dtlb-power-estimator.output-width", 32);
	simulator->SetVariable("dtlb-power-estimator.tag-width", 64);
	simulator->SetVariable("dtlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);

	//  - SDL run-time configuration
	simulator->SetVariable("sdl.refresh-period", video_refresh_period);
	simulator->SetVariable("sdl.bmp-out-filename", bmp_out_filename);	
	simulator->SetVariable("sdl.keymap-filename", keymap_filename);	

	// - Kernel loader configuration
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.device-tree-filename", device_tree_filename);
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.kernel-params", kernel_params); // no kernel parameters
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.ramdisk-filename", ramdisk_filename);
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.screen-width", display_width);
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.screen-height", display_height);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.filename", filename);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.base-addr", 0x00400000UL);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.force-base-addr", true);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.force-use-virtual-address", true);
	
	// - kernel logger
	simulator->SetVariable("kernel_logger.std_err", true);
	
	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
}

void Simulator::Stop(Object *object, int exit_status)
{
	std::cerr << object->GetName() << " has requested simulation stop" << std::endl;
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	wait();
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	EnableDebug();
	void (*prev_sig_int_handler)(int) = 0;

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
}

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
	// Build the kernel parameters string from the command line arguments
	string filename;
	string kernel_params;
	
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		filename = (string) (*cmd_args)[0];
		unsigned int i;
		for(i = 1; i < cmd_args_length; i++)
		{
			kernel_params += (string) (*cmd_args)[i];
			if(i < cmd_args_length - 1) kernel_params += " ";
		}
	}
	
	//  - Loader run-time configuration
	if(!filename.empty())
	{
		SetVariable("pmac-linux-kernel-loader.elf32-loader.filename", filename.c_str());
	}

	if(!kernel_params.empty())
	{
		SetVariable("pmac-linux-kernel-loader.pmac-bootx.kernel-params", kernel_params.c_str());
	}

	return unisim::kernel::service::Simulator::Setup();
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
	Simulator *simulator = new Simulator(argc, argv);

	switch(simulator->Setup())
	{
		case unisim::kernel::service::Simulator::ST_OK_DONT_START:
			break;
		case unisim::kernel::service::Simulator::ST_WARNING:
			cerr << "Some warnings occurred during setup" << endl;
		case unisim::kernel::service::Simulator::ST_OK_TO_START:
			cerr << "Starting simulation at supervisor privilege level (kernel mode)" << endl;
			simulator->Run();
			break;
		case unisim::kernel::service::Simulator::ST_ERROR:
			cerr << "Can't start simulation because of previous errors" << endl;
			break;
	}

	if(simulator) delete simulator;
#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}

extern "C"
int main(int argc, char *argv[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}
