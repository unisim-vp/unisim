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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <stdexcept>
#include <signal.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/component/tlm/processor/powerpc/mpc7447a/cpu.hh"
#include "unisim/component/tlm/memory/ram/memory.hh"
#include "unisim/component/tlm/memory/flash/am29/am29.hh"
#include <unisim/component/cxx/memory/flash/am29/am29lv800b_config.hh>
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
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
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


#ifdef DEBUG_EMBEDDED_PPC_G4_BOARD
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
using unisim::service::loader::multiformat_loader::MultiFormatLoader;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::component::cxx::pci::pci64_address_t;
using unisim::component::cxx::pci::pci32_address_t;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Variable;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Object;

class Simulator : public unisim::kernel::service::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual void Stop(Object *object, int exit_status);
protected:
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// CPU parameters
#ifdef DEBUG_EMBEDDED_PPC_G4_BOARD
	typedef unisim::component::cxx::processor::powerpc::mpc7447a::DebugConfig CPU_CONFIG;
#else
	typedef unisim::component::cxx::processor::powerpc::mpc7447a::Config CPU_CONFIG;
#endif
	// Front Side Bus template parameters
	typedef CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
	typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	static const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
	static const uint32_t FSB_NUM_PROCS = 1;

	// PCI Bus template parameters
	typedef pci32_address_t PCI_ADDRESS_TYPE;
	static const uint32_t PCI_MAX_DATA_SIZE = 32;        // in bytes
	static const unsigned int PCI_NUM_MASTERS = 1;
#ifdef WITH_PCI_STUB
	static const unsigned int PCI_NUM_TARGETS = 2;
#else
	static const unsigned int PCI_NUM_TARGETS = 1;
#endif
	static const unsigned int PCI_NUM_MAPPINGS = 7;

	// PCI device numbers
	static const unsigned int PCI_MPC107_DEV_NUM = 0;
	#ifdef WITH_PCI_STUB
	static const unsigned int PCI_STUB_DEV_NUM = 1;
	#endif

	// PCI target port numbers
	static const unsigned int PCI_MPC107_TARGET_PORT = 0;
#ifdef WITH_PCI_STUB
	static const unsigned int PCI_STUB_TARGET_PORT = 1;
#endif

	// PCI master port numbers
	static const unsigned int PCI_MPC107_MASTER_PORT = 0;

	// the maximum number of transaction spies (per type of message)
	static const unsigned int MAX_BUS_TRANSACTION_SPY = 4;
	static const unsigned int MAX_MEM_TRANSACTION_SPY = 3;
	static const unsigned int MAX_PCI_TRANSACTION_SPY = 3;
	static const unsigned int MAX_IRQ_TRANSACTION_SPY = 3;

	// Flash memory
	static const uint32_t FLASH_BYTESIZE = 4 * unisim::component::cxx::memory::flash::am29::M; // 4 MB
	static const uint32_t FLASH_IO_WIDTH = 8; // 64 bits
	typedef unisim::component::cxx::memory::flash::am29::AM29LV800BTConfig FLASH_CONFIG;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm::memory::flash::am29::AM29<FLASH_CONFIG, FLASH_BYTESIZE, FLASH_IO_WIDTH, FSB_MAX_DATA_SIZE> FLASH;
	typedef unisim::component::tlm::chipset::mpc107::MPC107<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, DEBUG_INFORMATION> MPC107;
	typedef unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
	typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, FSB_NUM_PROCS> FRONT_SIDE_BUS;
	typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION> PCI_BUS;
#ifdef WITH_PCI_STUB
	typedef unisim::component::tlm::pci::debug::PCIStub<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_STUB;
#endif
	typedef unisim::component::tlm::processor::powerpc::mpc7447a::CPU<CPU_CONFIG> CPU;

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

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - Front side bus
	FRONT_SIDE_BUS *bus;
	//  - MPC107 chipset
	MPC107 *mpc107;
	//  - EROM
	MEMORY *erom;
	//  - Flash memory
	FLASH *flash;
	//  - RAM
	MEMORY *memory;
	//  - PCI Bus
	PCI_BUS *pci_bus;
	//  - PCI Stub
#ifdef WITH_PCI_STUB
	PCI_STUB *pci_stub;
#endif

	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================
	BusMsgSpyType *bus_msg_spy[MAX_BUS_TRANSACTION_SPY];
	MemMsgSpyType *mem_msg_spy[MAX_MEM_TRANSACTION_SPY];
	PCIMsgSpyType *pci_msg_spy[MAX_PCI_TRANSACTION_SPY];
	IRQMsgSpyType *irq_msg_spy[MAX_IRQ_TRANSACTION_SPY];

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - a multiformat loader
	MultiFormatLoader<CPU_ADDRESS_TYPE> *loader;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - Tee Symbol Table Lookup
	unisim::service::tee::symbol_table_lookup::Tee<CPU_ADDRESS_TYPE> *tee_symbol_table_lookup;
	//  - Tee Memory Access Reporting
	unisim::service::tee::memory_access_reporting::Tee<PCI_ADDRESS_TYPE> *tee_memory_access_reporting;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;
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
	, erom(0)
	, flash(0)
	, memory(0)
	, pci_bus(0)
#ifdef WITH_PCI_STUB
	, pci_stub(0)
#endif
	, loader(0)
	, sim_time(0)
	, host_time(0)
	, tee_symbol_table_lookup(0)
	, tee_memory_access_reporting(0)
	, gdb_server(0)
	, inline_debugger(0)
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
#ifdef WITH_PCI_STUB
	unsigned int pci_stub_irq = 0;
#endif

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu =new CPU("cpu");
	//  - Front side bus
	bus = new FRONT_SIDE_BUS("bus");
	//  - MPC107 chipset
	mpc107 = new MPC107("mpc107");
	//  - EROM
	erom = new MEMORY("erom");
	//  - Flash memory
	flash = new FLASH("flash");
	//  - RAM
	memory = new MEMORY("memory");
	//  - PCI Bus
	pci_bus = new PCI_BUS("pci-bus");
	//  - PCI Stub
#ifdef WITH_PCI_STUB
	pci_stub = new PCI_STUB("pci-stub");
#endif

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
	//  - Multiformat loader
	loader = new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Tee Symbol Table Lookup
	tee_symbol_table_lookup = new unisim::service::tee::symbol_table_lookup::Tee<CPU_ADDRESS_TYPE>("tee-symbol-table-lookup");
	//  - Tee Memory Access Reporting
	tee_memory_access_reporting = 
		(enable_gdb_server || enable_inline_debugger) ?
			new unisim::service::tee::memory_access_reporting::Tee<PCI_ADDRESS_TYPE>("tee-memory-access-reporting") :
			0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - the optional power estimators
	il1_power_estimator = estimate_power ? new CachePowerEstimator("il1-power-estimator") : 0;
	dl1_power_estimator = estimate_power ? new CachePowerEstimator("dl1-power-estimator") : 0;
	l2_power_estimator = estimate_power ? new CachePowerEstimator("l2-power-estimator") : 0;
	itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;

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
	
	if(enable_inline_debugger || enable_gdb_server) 
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

		il1_power_estimator->time_import >> sim_time->time_export;
		dl1_power_estimator->time_import >> sim_time->time_export;
		l2_power_estimator->time_import >> sim_time->time_export;
		itlb_power_estimator->time_import >> sim_time->time_export;
		dtlb_power_estimator->time_import >> sim_time->time_export;
	}

	(*loader->memory_import[0]) >> mpc107->memory_export;
    cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	bus->memory_import >> mpc107->memory_export;
	mpc107->ram_import >> memory->memory_export;
	mpc107->rom_import >> flash->memory_export;
	mpc107->erom_import >> erom->memory_export;
	mpc107->pci_import >> *pci_bus->memory_export[PCI_MPC107_MASTER_PORT];

#ifdef WITH_PCI_STUB
	*pci_bus->memory_import[PCI_STUB_TARGET_PORT] >> pci_stub->memory_export; 
	if(enable_inline_debugger || enable_gdb_server) 
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
	pci_stub->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	pci_stub->synchronizable_import >> cpu->synchronizable_export;
	pci_stub->memory_import >> cpu->memory_export;
	pci_stub->registers_import >> cpu->registers_export;
#endif

	if(inline_debugger)
	{
		*inline_debugger->loader_import[0] >> loader->loader_export;
		*inline_debugger->symbol_table_lookup_import[0] >> loader->symbol_table_lookup_export;
		*inline_debugger->stmt_lookup_import[0] >> loader->stmt_lookup_export;
	}
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
	if(loader) delete loader;
	if(bus) delete bus;
	if(cpu) delete cpu;
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(l2_power_estimator) delete l2_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(sim_time) delete sim_time;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
	if(tee_symbol_table_lookup) delete tee_symbol_table_lookup;
	if(flash) delete flash;
	if(erom) delete erom;
	if(mpc107) delete mpc107;
	if(pci_bus) delete pci_bus;
#ifdef WITH_PCI_STUB
	if(pci_stub) delete pci_stub;
#endif
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM embedded-ppc-g4-board");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2011, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM embedded-ppc-g4-board simulator is a MPC7447A/MPC107 board simulator with support of ELF32, ELF64, S19, and RAW binaries and targeted for industrial applications");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	uint32_t pci_bus_frequency = 33; // in Mhz
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	double fsb_frequency = cpu_frequency / cpu_clock_multiplier; // FIXME: to be removed
	uint32_t tech_node = 130; // in nm
	uint32_t memory_size = 256 * 1024 * 1024; // 256 MB
#ifdef WITH_PCI_STUB
	bool pci_stub_use_pipe = false;
	unsigned int pci_stub_tcp_port = 12345;
	const char *pci_stub_server_name = "localhost";
	bool pci_stub_is_server = false;
	const char *pci_stub_pipe_name = "pipe";
#endif
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;
	
	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - Front Side Bus
	simulator->SetVariable("bus.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", cpu_cycle_time);
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.3 * 1e3); // mV
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.max-inst", maxinst);
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
	simulator->SetVariable("erom.org", 0x78000000UL);
	simulator->SetVariable("erom.bytesize", 2 * 8 * 1024 * 1024);
	simulator->SetVariable("erom.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	
	//  - Flash memory run-time configuration
	simulator->SetVariable("flash.org", 0xff800000UL); //0xff000000UL;
	simulator->SetVariable("flash.bytesize", 8 * 1024 * 1024);
	simulator->SetVariable("flash.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("flash.endian", "big-endian");

	// PCI Bus run-time configuration
	simulator->SetVariable("pci-bus.frequency", pci_bus_frequency);

	simulator->SetVariable("pci-bus.base-address[0]", 0);
	simulator->SetVariable("pci-bus.size[0]", 1024 * 1024 * 1024);
	simulator->SetVariable("pci-bus.device-number[0]", PCI_MPC107_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[0]", 0);
	simulator->SetVariable("pci-bus.register-number[0]", 0x10);
	simulator->SetVariable("pci-bus.addr-type[0]", "mem");

	simulator->SetVariable("pci-bus.num-mappings", 1); 

#ifdef WITH_PCI_STUB
	// PCI stub run-time configuration
	{
		int num_regions = 6;
		int num_region;
		int mapping_index;
		for(num_region = 0, mapping_index = 1; num_region < num_regions; num_region++)
		{
			std::stringstream sstr_subscript;
			sstr_subscript << "[" << mapping_index << "]";
			string subscript = sstr_subscript.str();
			
			simulator->SetVariable((string("pci-stub.base-address") + subscript).c_str(), 0);
			simulator->SetVariable((string("pci-stub.size") + subscript).c_str(), 0);
			simulator->SetVariable((string("pci-stub.device-number") + subscript).c_str(), PCI_STUB_DEV_NUM);
			simulator->SetVariable((string("pci-stub.target-port") + subscript).c_str(), PCI_STUB_TARGET_PORT);
			simulator->SetVariable((string("pci-stub.register-number") + subscript).c_str(), 0x10UL + (4 * num_region));
			simulator->SetVariable((string("pci-stub.addr-type") + subscript).c_str(), "mem");

			simulator->SetVariable((string("pci-stub.initial-base-addr") + subscript).c_str(), 0);
			simulator->SetVariable((string("pci-stub.region-size") + subscript).c_str(), 0);
			simulator->SetVariable((string("pci-stub.address-space") + subscript).c_str(), 0);
		}

		simulator->SetVariable("pci-stub.num-mappings", 0); 

		simulator->SetVariable("pci-stub.pci-bus-frequency", pci_bus_frequency);
		simulator->SetVariable("pci-stub.bus-frequency", fsb_frequency);
		simulator->SetVariable("pci-stub.tcp-port", pci_stub_tcp_port);
		simulator->SetVariable("pci-stub.server-name", pci_stub_server_name);
		simulator->SetVariable("pci-stub.is-server", pci_stub_is_server);
		simulator->SetVariable("pci-stub.protocol", pci_stub_use_pipe ? 1 : 0);
		simulator->SetVariable("pci-stub.pipe-name", pci_stub_pipe_name);
	}
#endif
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);

	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);

	// - Loader memory mapper
	simulator->SetVariable("loader.memory-mapper.mapping", "mpc107:0x00000000-0xffffffff"); // whole linear address space

	//  - Cache/TLB power estimators run-time configuration
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

void Simulator::Stop(Object *object, int exit_status)
{
	std::cerr << object->GetName() << " has requested simulation stop" << std::endl;
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	wait();
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
			cerr << "Starting simulation at user privilege level (Linux system call translation mode)" << endl;
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
