/*
 * simulator.hh
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

#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>

#include <unisim/service/profiling/addr_profiler/profiler.hh>

#include <unisim/service/interfaces/loader.hh>

#include <unisim/service/loader/elf_loader/elf_loader.hh>
#include <unisim/service/loader/elf_loader/elf_loader.tcc>
#include <unisim/service/loader/s19_loader/s19_loader.hh>

#include <unisim/service/tee/memory_access_reporting/tee.hh>

#include <unisim/service/tee/registers/registers_tee.hh>
#include <unisim/service/tee/memory_import_export/memory_import_export_tee.hh>

#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>

#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/s12mpu_if.hh>

#include <unisim/component/tlm2/processor/hcs12x/hcs12x.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xgate.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>
#include <unisim/component/tlm2/processor/hcs12x/atd10b.hh>
#include <unisim/component/tlm2/processor/hcs12x/pwm.hh>
#include <unisim/component/tlm2/processor/hcs12x/crg.hh>
#include <unisim/component/tlm2/processor/hcs12x/ect.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12pit24b.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12sci.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12spi.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12mpu.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xftmx.hh>

#include <unisim/component/tlm2/memory/ram/memory.hh>

#include <unisim/util/garbage_collector/garbage_collector.hh>

#include <unisim/service/pim/pim.hh>
#include <unisim/service/pim/pim_server.hh>

#include <unisim/service/debug/inline_debugger/inline_debugger.hh>

#include <xml_atd_pwm_stub.hh>

#ifdef HAVE_RTBCOB
#include "rtb_unisim.hh"
#endif

#ifdef WIN32

#include <windows.h>
//#include <winsock2.h>

#endif

using namespace std;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::S12MPU_IF;

using unisim::component::tlm2::processor::hcs12x::XINT;
using unisim::component::tlm2::processor::hcs12x::CRG;
using unisim::component::tlm2::processor::hcs12x::ECT;
using unisim::component::tlm2::processor::hcs12x::S12XFTMX;
using unisim::component::tlm2::processor::hcs12x::S12PIT24B;
using unisim::component::tlm2::processor::hcs12x::S12SCI;
using unisim::component::tlm2::processor::hcs12x::S12SPI;
using unisim::component::tlm2::processor::hcs12x::S12MPU;

using unisim::service::debug::debugger::Debugger;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;

using unisim::service::interfaces::Loader;
using unisim::service::loader::s19_loader::S19_Loader;
using unisim::service::pim::PIM;
using unisim::service::pim::PIMServer;

using unisim::service::profiling::addr_profiler::Profiler;

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::VariableBase;

using unisim::kernel::service::CallBackObject;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;


class Simulator :
	public unisim::kernel::service::Simulator
	, public CallBackObject

{
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	typedef unisim::component::cxx::processor::hcs12x::physical_address_t CPU_ADDRESS_TYPE;
	typedef uint64_t MEMORY_ADDRESS_TYPE;
	typedef unisim::component::cxx::processor::hcs12x::service_address_t SERVICE_ADDRESS_TYPE;

public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual void Stop(Object *object, int _exit_status);

	void Run();

	virtual double GetSimTime()	{ if (sim_time) { return (sim_time->GetTime()); } else { return (0); }	}
	virtual double GetHostTime()	{ if (host_time) { return (host_time->GetTime()); } else { return (0); }	}

	virtual long   GetStructuredAddress(long logicalAddress) { return (mmc->getCPU12XPagedAddress(logicalAddress)); }
	virtual long   GetPhysicalAddress(long logicalAddress) { return (mmc->getCPU12XPhysicalAddress(logicalAddress, ADDRESS::EXTENDED, false, false, 0x00)); }

	void GeneratePim() {
		PIM *pim = new PIM("pim");
		pim->generatePimFile();
		if (pim) { delete pim; pim = NULL; }
	};

	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);

private:

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

//	typedef unisim::component::tlm2::memory::ram::Memory<> RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<32, physical_address_t, 8, 1024*1024, false>  RAM;
//	typedef unisim::component::tlm2::memory::ram::Memory<> FLASH;
//	typedef unisim::component::tlm2::memory::ram::Memory<32, physical_address_t, 8, 1024*1024, false>  FLASH;
	typedef unisim::component::tlm2::processor::hcs12x::S12XFTMX<32, physical_address_t, 8, 1024*1024, false>  FTM;

	typedef unisim::component::tlm2::processor::hcs12x::HCS12X CPU;
	typedef unisim::component::tlm2::processor::s12xgate::S12XGATE XGATE;

	typedef unisim::component::tlm2::processor::hcs12x::S12MPU MPU;
	typedef unisim::component::tlm2::processor::hcs12x::S12XMMC MMC;

	typedef unisim::component::tlm2::processor::hcs12x::PWM<8> PWM;
	typedef unisim::component::tlm2::processor::hcs12x::ATD10B<16> ATD1;
	typedef unisim::component::tlm2::processor::hcs12x::ATD10B<8> ATD0;
	typedef unisim::component::tlm2::processor::hcs12x::S12PIT24B<8> PIT;

// ******* REGARDE Interface ElfLoader pour le typedef ci-dessous
	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<CPU_ADDRESS_TYPE, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;

	typedef unisim::service::tee::registers::RegistersTee<32> RegistersTee;
	typedef unisim::service::tee::memory_import_export::MemoryImportExportTee<physical_address_t, 32> MemoryImportExportTee;
	typedef unisim::service::tee::memory_access_reporting::Tee<CPU_ADDRESS_TYPE> MemoryAccessReportingTee;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor

	CPU *cpu;
	XGATE *xgate;

	MMC *mmc;
	MPU *mpu;

	CRG  *crg;
	ECT  *ect;

	ATD1 *atd1;
	ATD0 *atd0;

	PWM *pwm;

	PIT *pit;

	S12SCI *sci0, *sci1, *sci2, *sci3, *sci4, *sci5;

	S12SPI *spi0, *spi1, *spi2;

	//  - Memories
	RAM *global_ram;
	FTM *global_flash;

	// - Interrupt controller
	XINT *s12xint;

	RegistersTee* registersTee;

	MemoryImportExportTee* memoryImportExportTee;

	//  - Tee Memory Access Reporting
	MemoryAccessReportingTee *tee_memory_access_reporting;

#ifdef HAVE_RTBCOB
	RTBStub *rtbStub;
#else
	XML_ATD_PWM_STUB *xml_atd_pwm_stub;
#endif

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	S19_Loader<CPU_ADDRESS_TYPE> *loaderS19;
	Elf32Loader *loaderELF;

	//  - profiler
	Profiler<CPU_ADDRESS_TYPE> *profiler;

	//  - Debugger
	Debugger<CPU_ADDRESS_TYPE> *debugger;

	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;

	// PIM server
	PIMServer<CPU_ADDRESS_TYPE> *pim_server;

	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;

	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;

	bool enable_pim_server;
	bool enable_gdb_server;
	bool enable_inline_debugger;
	string filename;
	Parameter<bool> param_enable_pim_server;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;

	string endian;
	Parameter<string> *param_endian;
	string program_counter_name;
	Parameter<string> *param_pc_reg_name;

	int exit_status;
	bool isS19;

	bool dump_parameters;
	bool dump_formulas;
	bool dump_statistics;
	Parameter<bool> param_dump_parameters;
	Parameter<bool> param_dump_formulas;
	Parameter<bool> param_dump_statistics;

	enum {DATA_LOAD_RATIO, DATA_STORE_RATIO} ReadBackOffsets;

	double null_stat_var;
	Statistic<double> stat_data_load_ratio;
	Statistic<double> stat_data_store_ratio;

	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};

#endif /* SIMULATOR_HH_ */
