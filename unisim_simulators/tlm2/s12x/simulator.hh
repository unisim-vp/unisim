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

#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>

#include <unisim/service/interfaces/loader.hh>

#include <unisim/service/loader/elf_loader/elf_loader.hh>
#include <unisim/service/loader/elf_loader/elf_loader.tcc>
#include <unisim/service/loader/s19_loader/s19_loader.hh>

#include <unisim/service/tee/registers/registers_tee.hh>
#include <unisim/service/tee/memory_import_export/memory_import_export_tee.hh>

#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>

#include <unisim/service/pim/pim.hh>
#include <unisim/service/pim/pim_server.hh>

#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/hcs12x.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>
#include <unisim/component/tlm2/processor/hcs12x/atd10b.hh>
#include <unisim/component/tlm2/processor/hcs12x/pwm.hh>
#include <unisim/component/tlm2/processor/hcs12x/crg.hh>
#include <unisim/component/tlm2/processor/hcs12x/ect.hh>

#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>

#include <unisim/util/garbage_collector/garbage_collector.hh>

#include <xml_atd_pwm_stub.hh>

#ifdef HAVE_RTBCOB
#include "rtb_unisim.hh"
#endif

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif


using namespace std;

using unisim::component::cxx::processor::hcs12x::ADDRESS;

using unisim::component::tlm2::processor::hcs12x::XINT;
using unisim::component::tlm2::processor::hcs12x::CRG;
using unisim::component::tlm2::processor::hcs12x::ECT;

using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::interfaces::Loader;
using unisim::service::loader::s19_loader::S19_Loader;
using unisim::service::pim::PIM;
using unisim::service::pim::PIMServer;

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::VariableBase;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;


class Simulator : public unisim::kernel::service::Simulator
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

	virtual double GetSimTime()	{ if (sim_time) { return sim_time->GetTime(); } else { return 0; }	}

	void GeneratePim() {
		PIM *pim = new PIM("pim");
		pim->GeneratePimFile();
		if (pim) { delete pim; pim = NULL; }
	};

private:

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<> MEMORY;

	typedef unisim::component::tlm2::processor::hcs12x::HCS12X CPU;

	class InternalRouterConfig {
	public:
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 7;
		static const unsigned int MAX_NUM_MAPPINGS = 7; //256;
		static const unsigned int BUSWIDTH = 32;
		typedef tlm::tlm_base_protocol_types TYPES;
		static const bool VERBOSE = false;
	};
	typedef unisim::component::tlm2::interconnect::generic_router::Router<InternalRouterConfig> INTERNAL_ROUTER;

	class ExternalRouterConfig {
	public:
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 1;
		static const unsigned int MAX_NUM_MAPPINGS = 1; //256;
		static const unsigned int BUSWIDTH = 32;
		typedef tlm::tlm_base_protocol_types TYPES;
		static const bool VERBOSE = false;
	};
	typedef unisim::component::tlm2::interconnect::generic_router::Router<ExternalRouterConfig> EXTERNAL_ROUTER;

	typedef unisim::component::tlm2::processor::hcs12x::S12XMMC MMC;

	typedef unisim::component::tlm2::processor::hcs12x::PWM<8> PWM;
	typedef unisim::component::tlm2::processor::hcs12x::ATD10B<16> ATD1;
	typedef unisim::component::tlm2::processor::hcs12x::ATD10B<8> ATD0;

// ******* REGARDE Interface ElfLoader pour le typedef ci-dessous
	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;

	typedef unisim::service::tee::registers::RegistersTee<> RegistersTee;
	typedef unisim::service::tee::memory_import_export::MemoryImportExportTee<service_address_t> MemoryImportExportTee;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor

	CPU *cpu;

	MMC *mmc;

	CRG  *crg;
	ECT  *ect;

	ATD1 *atd1;
	ATD0 *atd0;

	PWM *pwm;

	//  - tlm2 router
	EXTERNAL_ROUTER	*external_router;
	INTERNAL_ROUTER	*internal_router;

	//  - Memories
	MEMORY *internal_memory;
	MEMORY *external_memory;

	// - Interrupt controller
	XINT *s12xint;

	RegistersTee* registersTee;

	MemoryImportExportTee* memoryImportExportTee;

#ifdef HAVE_RTBCOB
	RTBStub *rtbStub;
#else
	XML_ATD_PWM_STUB *xml_atd_pwm_stub;
#endif

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	Service<Loader<SERVICE_ADDRESS_TYPE> > *loaderS19;
	Service<Loader<SERVICE_ADDRESS_TYPE> > *loaderELF;


	//  - GDB server
	GDBServer<SERVICE_ADDRESS_TYPE> *gdb_server;

	// PIM server
	PIMServer<SERVICE_ADDRESS_TYPE> *pim_server;

	//  - Inline debugger
	InlineDebugger<SERVICE_ADDRESS_TYPE> *inline_debugger;
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
	int exit_status;
	bool isS19;

	bool dump_parameters;
	bool dump_formulas;
	bool dump_statistics;
	Parameter<bool> param_dump_parameters;
	Parameter<bool> param_dump_formulas;
	Parameter<bool> param_dump_statistics;

	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};

#endif /* SIMULATOR_HH_ */
