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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <stdexcept>

#include <unisim/kernel/kernel.hh>

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

//#include <unisim/service/pim/pim.hh>

#include "xml_atd_pwm_stub.hh"

#ifdef HAVE_RTBCOB
#include "rtb_unisim.hh"
#endif

#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif


bool debug_enabled = false;

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

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using namespace std;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;

using unisim::component::tlm2::processor::hcs12x::XINT;
using unisim::component::tlm2::processor::hcs12x::CRG;
using unisim::component::tlm2::processor::hcs12x::ECT;
using unisim::service::loader::s19_loader::S19_Loader;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::VariableBase;
using unisim::service::interfaces::Loader;

//using unisim::service::pim::PIM;

class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual void Stop(Object *object, int _exit_status);

	void Run();
protected:
private:
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

//	PIM *pim;

	//  - GDB server
	GDBServer<SERVICE_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<SERVICE_ADDRESS_TYPE> *inline_debugger;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	string filename;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	int exit_status;
	bool isS19;

	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, mmc(0)
	, crg(0)
	, ect(0)
	, atd1(0)
	, atd0(0)
	, pwm(0)
	, external_router(0)
	, internal_router(0)
	, internal_memory(0)
	, external_memory(0)
	, s12xint(0)
	, registersTee(0)
	, memoryImportExportTee(0)
#ifdef HAVE_RTBCOB
	, rtbStub(0)
#else
	, xml_atd_pwm_stub(0)
#endif
	, loaderS19(0)
	, loaderELF(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, isS19(false)
{
	//=========================================================================
	//===      Handling of file to load passed as command line argument     ===
	//=========================================================================

	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		filename = (string)(*cmd_args)[0];
		std::cerr << "filename=\"" << filename << "\"" << std::endl;
	}

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor

	cpu =new CPU("CPU");

	mmc = 	new MMC("MMC");

	crg = new CRG("CRG");
	ect = new ECT("ECT");

	atd1 = new ATD1("ATD1");
	atd0 = new ATD0("ATD0");

	pwm = new PWM("PWM");

	//  - tlm2 router
	external_router = new EXTERNAL_ROUTER("external_router");
	internal_router = new INTERNAL_ROUTER("internal_router");

	//  - Memories
	internal_memory = new MEMORY("internal-memory");
	external_memory = new MEMORY("external-memory");

	// - Interrupt controller
	s12xint = new XINT("XINT");

	registersTee = new RegistersTee("registersTee");

	memoryImportExportTee = new MemoryImportExportTee("memoryImportExportTee");

#ifdef HAVE_RTBCOB
	rtbStub = new RTBStub("atd-pwm-stub"/*, fsb_cycle_time*/);
#else
	xml_atd_pwm_stub = new XML_ATD_PWM_STUB("atd-pwm-stub"/*, fsb_cycle_time*/);
#endif
	
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	isS19 = (filename.find(".s19") != std::string::npos) ||
		 (filename.find(".S19") != std::string::npos);

	if (isS19) {
		loaderS19 = new S19_Loader<SERVICE_ADDRESS_TYPE>("S19_Loader");
		loaderELF = new Elf32Loader("elf32-loader");
	} else {
		loaderELF = new Elf32Loader("elf32-loader");
	}

//	pim = new PIM("PIM", this, 1234);

	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<SERVICE_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<SERVICE_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");

	//=========================================================================
	//===                       Service parameterization     ===
	//=========================================================================

	if(isS19) {
		(*loaderS19)["filename"] = filename.c_str();
	}
	else
	{
		(*loaderELF)["filename"] = filename.c_str();
	}

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->socket(mmc->cpu_socket);
	cpu->toXINT(s12xint->fromCPU_Target);

	s12xint->toCPU_Initiator(cpu->interrupt_request);

	crg->interrupt_request(s12xint->interrupt_request);
	ect->interrupt_request(s12xint->interrupt_request);
	pwm->interrupt_request(s12xint->interrupt_request);
	atd1->interrupt_request(s12xint->interrupt_request);
	atd0->interrupt_request(s12xint->interrupt_request);

#ifdef HAVE_RTBCOB
	rtbStub->atd1_master_sock(atd1->anx_socket);
	rtbStub->atd0_master_sock(atd0->anx_socket);
	rtbStub->slave_sock(pwm->master_sock);
#else
	xml_atd_pwm_stub->atd1_master_sock(atd1->anx_socket);
	xml_atd_pwm_stub->atd0_master_sock(atd0->anx_socket);
	xml_atd_pwm_stub->slave_sock(pwm->master_sock);
#endif

	mmc->local_socket(*internal_router->targ_socket[0]);
	mmc->external_socket(*external_router->targ_socket[0]);

	// This order is mandatory (see the memoryMapping)
	(*internal_router->init_socket[0])(crg->slave_socket);
	(*internal_router->init_socket[1])(ect->slave_socket);
	(*internal_router->init_socket[2])(atd1->slave_socket);
	(*internal_router->init_socket[3])(s12xint->slave_socket);
	(*internal_router->init_socket[4])(atd0->slave_socket);
	(*internal_router->init_socket[5])(pwm->slave_socket);
	(*internal_router->init_socket[6])(internal_memory->slave_sock); // to connect to the MMC

	(*external_router->init_socket[0])(external_memory->slave_sock);

	crg->bus_clock_socket(cpu->bus_clock_socket);
	crg->bus_clock_socket(ect->bus_clock_socket);
	crg->bus_clock_socket(pwm->bus_clock_socket);
	crg->bus_clock_socket(atd1->bus_clock_socket);
	crg->bus_clock_socket(atd0->bus_clock_socket);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> mmc->memory_export;

	*(memoryImportExportTee->memory_import[0]) >> s12xint->memory_export;
	*(memoryImportExportTee->memory_import[1]) >> crg->memory_export;
	*(memoryImportExportTee->memory_import[2]) >> atd1->memory_export;
	*(memoryImportExportTee->memory_import[3]) >> atd0->memory_export;
	*(memoryImportExportTee->memory_import[4]) >> pwm->memory_export;
	*(memoryImportExportTee->memory_import[5]) >> ect->memory_export;
	*(memoryImportExportTee->memory_import[6]) >> internal_memory->memory_export;

	mmc->internal_memory_import >> memoryImportExportTee->memory_export;

	mmc->external_memory_import >> external_memory->memory_export;

	*(registersTee->registers_import[0]) >> cpu->registers_export;
	*(registersTee->registers_import[1]) >> mmc->registers_export;
	*(registersTee->registers_import[2]) >> s12xint->registers_export;
	*(registersTee->registers_import[3]) >> crg->registers_export;
	*(registersTee->registers_import[4]) >> atd1->registers_export;
	*(registersTee->registers_import[5]) >> atd0->registers_export;
	*(registersTee->registers_import[6]) >> pwm->registers_export;
	*(registersTee->registers_import[7]) >> ect->registers_export;

	if(enable_inline_debugger)
	{
		// Connect inline-debugger to CPU
		cpu->debug_yielding_import >> inline_debugger->debug_yielding_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;

		pwm->trap_reporting_import >> inline_debugger->trap_reporting_export;
		atd0->trap_reporting_import >> inline_debugger->trap_reporting_export;
		atd1->trap_reporting_import >> inline_debugger->trap_reporting_export;

		mmc->trap_reporting_import >> inline_debugger->trap_reporting_export;

		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;

		inline_debugger->registers_import >> registersTee->registers_export;

		inline_debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}
	else if(enable_gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_yielding_import >> gdb_server->debug_yielding_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;

		pwm->trap_reporting_import >> gdb_server->trap_reporting_export;
		atd0->trap_reporting_import >> gdb_server->trap_reporting_export;
		atd1->trap_reporting_import >> gdb_server->trap_reporting_export;

		mmc->trap_reporting_import >> gdb_server->trap_reporting_export;

		gdb_server->disasm_import >> cpu->disasm_export;
		gdb_server->memory_import >> cpu->memory_export;

		gdb_server->registers_import >> registersTee->registers_export;
//		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}

	if (isS19) {
		((S19_Loader<SERVICE_ADDRESS_TYPE> *) loaderS19)->memory_import >> mmc->memory_export;

	} else {
		((Elf32Loader *) loaderELF)->memory_import >> mmc->memory_export;
	}

	if (loaderELF) {
		cpu->symbol_table_lookup_import >> ((Elf32Loader *) loaderELF)->symbol_table_lookup_export;
		if(inline_debugger)
		{
			*inline_debugger->symbol_table_lookup_import[0] >> ((Elf32Loader *) loaderELF)->symbol_table_lookup_export;
			*inline_debugger->stmt_lookup_import[0] >> ((Elf32Loader *) loaderELF)->stmt_lookup_export;
		} else if(gdb_server)
		{
			gdb_server->symbol_table_lookup_import >> ((Elf32Loader *) loaderELF)->symbol_table_lookup_export;
		}
	}

}

Simulator::~Simulator()
{
//	if (pim) { delete pim; pim = NULL; }

	if (registersTee) { delete registersTee; registersTee = NULL; }
	if (memoryImportExportTee) { delete memoryImportExportTee; memoryImportExportTee = NULL; }

	if(gdb_server) { delete gdb_server; gdb_server = NULL; }
	if(inline_debugger) { delete inline_debugger; inline_debugger = NULL; }

	if (host_time) { delete host_time; host_time = NULL; }
	if(sim_time) { delete sim_time; sim_time = NULL; }

	if(loaderS19) { delete loaderS19; loaderS19 = NULL; }
	if(loaderELF) { delete loaderELF; loaderELF = NULL; }

#ifdef HAVE_RTBCOB
	if (rtbStub) { delete rtbStub; rtbStub = NULL; }
#else
	if (xml_atd_pwm_stub) { delete xml_atd_pwm_stub; xml_atd_pwm_stub = NULL; }
#endif

	if(external_memory) { delete external_memory; external_memory = NULL; }
	if(internal_memory) { delete internal_memory; internal_memory = NULL; }
	if(external_router) { delete external_router; external_router = NULL; }
	if(internal_router) { delete internal_router; internal_router = NULL; }

	if (crg) { delete crg; crg = NULL; }
	if (ect) { delete ect; ect = NULL; }
	if (pwm) { delete pwm; pwm = NULL; }
	if (atd1) { delete atd1; atd1 = NULL; }
	if (atd0) { delete atd0; atd0 = NULL; }
	if (s12xint) { delete s12xint; s12xint = NULL; }
	if (mmc) { delete mmc; mmc = NULL; }

	if(cpu) { delete cpu; cpu = NULL; }
}

void Simulator::Stop(Object *object, int _exit_status)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	wait();
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM star12x");
	simulator->SetVariable("copyright", "Copyright (C) 2008-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Reda Nouacer <reda.nouacer@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM star12x, a Star12X System-on-Chip simulator with support of ELF32 binaries and s19 hex files, and targeted for automotive applications");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_hcs12x.xml";
	const char *filename = "";
	const char *symbol_filename = "";

//	double cpu_frequency = 4.0; // in Mhz

//	uint8_t cpu_clock_multiplier = 1;
//	uint8_t xgate_clock_multiplier = 2;
//	double cpu_ipc = 1.0; // in instructions per cycle
//	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
//	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
//	uint32_t mem_cycle_time = fsb_cycle_time;
	bool force_use_virtual_address = true;

//	ADDRESS::ENCODING address_encoding = ADDRESS::BANKED;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================



	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);

	simulator->SetVariable("S19_Loader.filename", filename);
	simulator->SetVariable("elf32-loader.filename", symbol_filename);
	simulator->SetVariable("elf32-loader.force-use-virtual-address", force_use_virtual_address);

	simulator->SetVariable("atd-pwm-stub.anx-stimulus-period", 80000000); // 80 us
	simulator->SetVariable("atd-pwm-stub.pwm-fetch-period", 1e9); // 1 ms
	simulator->SetVariable("atd-pwm-stub.atd0-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd0-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd0-anx-wrap-around-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd1-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-wrap-around-channel", 0);
	simulator->SetVariable("atd-pwm-stub.trace-enable", false);

	simulator->SetVariable("ATD0.bus-cycle-time", 250000);
	simulator->SetVariable("ATD0.base-address", 0x2c0);
	simulator->SetVariable("ATD0.interrupt-offset", 0xd2);
	simulator->SetVariable("ATD0.vrl", 0.000000e+00);
	simulator->SetVariable("ATD0.vrh", 5.120000e+00);
	simulator->SetVariable("ATD0.debug-enabled", false);
	simulator->SetVariable("ATD0.vih", 3.250000e+00);
	simulator->SetVariable("ATD0.vil", 1.750000e+00);
	simulator->SetVariable("ATD0.Has-External-Trigger", false);
	simulator->SetVariable("ATD1.bus-cycle-time", 250000);
	simulator->SetVariable("ATD1.base-address", 0x80);
	simulator->SetVariable("ATD1.interrupt-offset", 0xd0);
	simulator->SetVariable("ATD1.vrl", 0.000000e+00);
	simulator->SetVariable("ATD1.vrh", 5.120000e+00);
	simulator->SetVariable("ATD1.debug-enabled", false);
	simulator->SetVariable("ATD1.vih", 3.250000e+00);
	simulator->SetVariable("ATD1.vil", 1.750000e+00);
	simulator->SetVariable("ATD1.Has-External-Trigger", false);
	simulator->SetVariable("CPU.trace-enable", false);
	simulator->SetVariable("CPU.verbose-all", false);
	simulator->SetVariable("CPU.verbose-setup", false);
	simulator->SetVariable("CPU.verbose-step", false);
	simulator->SetVariable("CPU.verbose-dump-regs-start", false);
	simulator->SetVariable("CPU.verbose-dump-regs-end", false);
	simulator->SetVariable("CPU.verbose-exception", false);
	simulator->SetVariable("CPU.requires-memory-access-reporting", false);
	simulator->SetVariable("CPU.requires-finished-instruction-reporting", false);
	simulator->SetVariable("CPU.debug-enabled", false);
	simulator->SetVariable("CPU.max-inst", 0xffffffffffffffffULL);
	simulator->SetVariable("CPU.nice-time", "1 ms");
	simulator->SetVariable("CPU.cpu-cycle-time", 250000);
	simulator->SetVariable("CPU.bus-cycle-time", 250000);
	simulator->SetVariable("CPU.verbose-tlm-bus-synchronize", false);
	simulator->SetVariable("CPU.verbose-tlm-run-thread", false);
	simulator->SetVariable("CPU.verbose-tlm-commands", false);
	simulator->SetVariable("CRG.oscillator-clock", 125000);
	simulator->SetVariable("CRG.base-address", 0x34);
	simulator->SetVariable("CRG.interrupt-offset-rti", 0xf0);
	simulator->SetVariable("CRG.interrupt-offset-pll-lock", 0xc6);
	simulator->SetVariable("CRG.interrupt-offset-self-clock-mode", 0xc4);
	simulator->SetVariable("CRG.debug-enabled", false);
	simulator->SetVariable("ECT.bus-cycle-time", 250000);
	simulator->SetVariable("ECT.base-address", 0x40);
	simulator->SetVariable("ECT.interrupt-offset-channel0", 0xee);
	simulator->SetVariable("ECT.interrupt-offset-overflow", 0xde);
	simulator->SetVariable("ECT.debug-enabled", false);
	simulator->SetVariable("external-memory.org", 0x0);
	simulator->SetVariable("external-memory.bytesize", 0x800000);
	simulator->SetVariable("external-memory.cycle-time", 250000);
	simulator->SetVariable("external-memory.verbose", false);
	simulator->SetVariable("external_router.cycle_time", 250000);
	simulator->SetVariable("external_router.port_buffer_size", 0x0);
	simulator->SetVariable("external_router.mapping_0", "range_start=\"0x800\" range_end=\"0x7fffff\" output_port=\"0\" translation=\"800\"");
	simulator->SetVariable("internal-memory.org", 0x0);
	simulator->SetVariable("internal-memory.bytesize", 0x10000);
	simulator->SetVariable("internal-memory.cycle-time", 250000);
	simulator->SetVariable("internal-memory.verbose", false);
	simulator->SetVariable("internal_router.cycle_time", 250000);
	simulator->SetVariable("internal_router.port_buffer_size", 0x0);
	simulator->SetVariable("internal_router.mapping_0", "range_start=\"0x34\" range_end=\"0x40\" output_port=\"0\" translation=\"34\"");
	simulator->SetVariable("internal_router.mapping_1", "range_start=\"0x40\" range_end=\"0x7f\" output_port=\"1\" translation=\"40\"");
	simulator->SetVariable("internal_router.mapping_2", "range_start=\"0x80\" range_end=\"0xb0\" output_port=\"2\" translation=\"80\"");
	simulator->SetVariable("internal_router.mapping_3", "range_start=\"0x120\" range_end=\"0x130\" output_port=\"3\" translation=\"120\"");
	simulator->SetVariable("internal_router.mapping_4", "range_start=\"0x2c0\" range_end=\"0x2e0\" output_port=\"4\" translation=\"2c0\"");
	simulator->SetVariable("internal_router.mapping_5", "range_start=\"0x300\" range_end=\"0x328\" output_port=\"5\" translation=\"300\"");
	simulator->SetVariable("internal_router.mapping_6", "range_start=\"0x800\" range_end=\"0xffff\" output_port=\"6\" translation=\"800\"");
	simulator->SetVariable("kernel_logger.std_err", true);
	simulator->SetVariable("kernel_logger.std_out", false);
	simulator->SetVariable("kernel_logger.std_err_color", false);
	simulator->SetVariable("kernel_logger.std_out_color", false);
	simulator->SetVariable("kernel_logger.file", false);
	simulator->SetVariable("kernel_logger.filename", "logger_output.txt");
	simulator->SetVariable("kernel_logger.xml_file", true);
	simulator->SetVariable("kernel_logger.xml_filename", "logger_output.xml");
	simulator->SetVariable("kernel_logger.xml_file_gzipped", false);
	simulator->SetVariable("MMC.debug-enabled", false);
	simulator->SetVariable("MMC.mode", 0x80);
	simulator->SetVariable("MMC.mmcctl1", 0x5);
	simulator->SetVariable("MMC.address-encoding", 0x0);
	simulator->SetVariable("PWM.bus-cycle-time", 250000);
	simulator->SetVariable("PWM.base-address", 0x300);
	simulator->SetVariable("PWM.interrupt-offset", 0x8c);
	simulator->SetVariable("PWM.debug-enabled", false);
	simulator->SetVariable("XINT.debug-enabled", false);
	
	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
}

void Simulator::Run()
{
	// If no filename has been specified, abort simulation
	if(filename.empty())
	{
		std::cerr << "ERROR! No file to load. You should provide a file to load as command line argument." << std::endl;
		return;
	}

	physical_address_t entry_point;

	if (isS19) {
		entry_point = loaderS19->GetEntryPoint();
		std::cerr << "entry_point=0x" << std::hex << entry_point << std::dec << std::endl;
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
	void (*prev_sig_int_handler)(int) = 0;

	if(!inline_debugger)
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}

//	pim->start();

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

int sc_main(int argc, char *argv[])
{
	Simulator *simulator = new Simulator(argc, argv);

	switch(simulator->Setup())
	{
		case unisim::kernel::Simulator::ST_OK_DONT_START:
			break;
		case unisim::kernel::Simulator::ST_WARNING:
			cerr << "Some warnings occurred during setup" << endl;
		case unisim::kernel::Simulator::ST_OK_TO_START:
			cerr << "Starting simulation at supervisor privilege level (kernel mode)" << endl;
			simulator->Run();
			break;
		case unisim::kernel::Simulator::ST_ERROR:
			cerr << "Can't start simulation because of previous errors" << endl;
			break;
	}

	if(simulator) delete simulator;

	return 0;
}
