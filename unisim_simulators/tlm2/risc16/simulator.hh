/* simulator.hh
 *
 *  Created on: 15 mars 2011
 *      Author: rnouacer
 */

#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include <iostream>

#include <signal.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdexcept>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/kernel/service/service.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/breakpoint_registry.tcc>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.tcc>
#include <unisim/util/debug/blob/blob.hh>
#include <unisim/util/loader/elf_loader/elf_loader.hh>
#include <unisim/util/loader/elf_loader/elf_loader.tcc>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/debugger/debugger.tcc>

#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.tcc>

#include <unisim/service/loader/raw_loader/raw_loader.hh>

#include <unisim/service/tee/memory_access_reporting/tee.hh>
#include <unisim/service/tee/memory_access_reporting/tee.tcc>

#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/profile.tcc>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/service/profiling/addr_profiler/profiler.tcc>

#include <unisim/service/tee/registers/registers_tee.hh>
#include <unisim/service/tee/memory_import_export/memory_import_export_tee.hh>

#include <unisim/service/telnet/telnet.hh>

#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>

#include <unisim/component/tlm2/processor/risc16/initCPU_tlm2.hh>
#include <unisim/component/tlm2/processor/risc16/switch.hh>
#include <unisim/component/tlm2/processor/risc16/switch.tcc>
#include <unisim/component/tlm2/processor/risc16/device.hh>

#include <unisim/component/tlm2/memory/ram/memory.hh>
#include "unisim/component/tlm2/memory/ram/memory.tcc"
#include <unisim/component/cxx/memory/ram/memory.tcc>

#include <unisim/util/garbage_collector/garbage_collector.hh>
#include <tlm.h>

#include <unisim/service/Fault/MFI.hh>
#include <unisim/service/Fault/MFI.tcc>



#include <unisim/service/Fault/BFI.hh>
#include <unisim/service/Fault/BFI.tcc>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::VariableBase;

using unisim::service::debug::debugger::Debugger;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::profiling::addr_profiler::Profiler;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;

using unisim::service::fault::Memory_Fault_Injector;
using unisim::service::fault::Bus_Fault_Injector;

using unisim::service::fault::Memory_Fault_Injector;
using unisim::component::tlm2::processor::risc16::Initiator;
using unisim::component::tlm2::processor::risc16::Router;

//using unisim::component::tlm2::memory::ram::Memory;

using unisim::component::tlm2::processor::risc16::Router;
using unisim::component::tlm2::processor::risc16::Device;


using namespace sc_core;



class Top: public sc_module, public Object
{
public:
	enum { n = 2};
	tlm::tlm_initiator_socket<> top_socket[n]; //TODO: array will be much more better
	tlm::tlm_target_socket<> top_target_socket;

  Top(sc_module_name name): sc_module(name), Object(name)
  {
    // Instantiate components

    initiator = new Initiator("initiator", this);
    router    = new Router<PHYSICAL_ADDR, 2>("router", this);

    // Bind socket

    initiator->initsocket( router->target_socket);

    top_target_socket(initiator->targsocket);

    for(int i =0; i < n; i++)
    {
    	router-> initiator_socket[i]( top_socket[i]);
    }

  }

  Initiator* initiator;
  Router<PHYSICAL_ADDR, 2>* router;

};

#include "unisim/component/tlm2/memory/ram/memory.tcc"
#include <unisim/component/cxx/memory/ram/memory.tcc>

template class unisim::component::cxx::memory::ram::Memory<uint16_t>;

class Simulator :
	public unisim::kernel::service::Simulator

{
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	typedef uint32_t address_t;
	typedef uint32_t physical_address_t;
	typedef uint32_t CPU_ADDRESS_TYPE;
	typedef uint64_t MEMORY_ADDRESS_TYPE;
	typedef uint32_t SERVICE_ADDRESS_TYPE;


public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual void Stop(Object *object, int _exit_status, bool asynchronous);

	void Run();

	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);

private:

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<32, physical_address_t, 8, 1024*1024, true>  RAM;

	typedef unisim::service::fault::Memory_Fault_Injector<physical_address_t> MFI;

	typedef unisim::service::fault::Bus_Fault_Injector<physical_address_t> BFI;

	typedef unisim::component::tlm2::processor::risc16::Initiator CPU;

	typedef unisim::component::tlm2::processor::risc16::Device Device;

	typedef unisim::service::loader::raw_loader::RawLoader<physical_address_t> RawLoader;


	//typedef unisim::component::tlm2::processor::risc16::Router<2> Bus;

// ******* REGARDE Interface ElfLoader pour le typedef ci-dessous
//	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<CPU_ADDRESS_TYPE, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;

	typedef unisim::service::tee::memory_import_export::MemoryImportExportTee<physical_address_t, 32> MemoryImportExportTee;

	typedef unisim::service::tee::memory_access_reporting::Tee<CPU_ADDRESS_TYPE> MemoryAccessReportingTee;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - RISC16 processor

	Top *top;

//	CPU *cpu;

	//  - Memories
	RAM *global_ram;

	//   -CAN
	Device* device;

	//   -Router
//	Bus* bus;

	//  - Memory Fault injector
//	MFI *mem_Fault_injector;

	//  - Bus Fault Injector

	BFI* bus_fault_injector;

	MemoryAccessReportingTee *tee_memory_access_reporting;

	Debugger<CPU_ADDRESS_TYPE> *debugger;
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;

	Profiler<CPU_ADDRESS_TYPE> *profiler;

	RawLoader* raw_loader;

	// - Interrupt controller

//	MemoryImportExportTee* memoryImportExportTee;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	string endian;
	Parameter<string> *param_endian;
	string program_counter_name;
	Parameter<string> *param_pc_reg_name;

	string filename;

	int exit_status;

	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};

#endif /* SIMULATOR_HH_ */
