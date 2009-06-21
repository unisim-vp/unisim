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
 
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>

#include "unisim/kernel/service/service.hh"

#include "unisim/component/cxx/processor/arm/config.hh"
#include "unisim/component/tlm2/processor/arm/arm7tdmi.hh"
#include "unisim/component/tlm2/memory/ram/memory.hh"
#include "unisim/component/tlm2/interconnect/generic_router/router.hh"
#include "router_config.hh"
#include "unisim/component/tlm2/interrupt/str7_eic/str7_eic.hh"
#include "unisim/component/tlm2/com/str7_spi/str7_spi.hh"
#include "unisim/component/tlm2/timer/str7_timer/tim.hh"
#include "unisim/component/tlm2/interrupt/master_stub.hh"
#include "unisim/component/tlm2/interrupt/slave_stub.hh"
#include "unisim/component/tlm2/signal_converter/generic_adc/adc.hh"

#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
#ifdef STR7_DEBUG
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#endif
#ifdef STR7_DEBUG_INLINE
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#endif
#include "unisim/service/loader/elf_loader/elf_loader.hh"
#include "unisim/service/loader/elf_loader/elf_loader.tcc"

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

//static const bool DEBUG_INFORMATION = true;

bool debug_enabled;

void EnableDebug() {
	debug_enabled = true;
}

void DisableDebug() {
	debug_enabled = false;
}

void SigIntHandler(int signum) {
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}

using namespace std;
#ifdef STR7_DEBUG
using unisim::service::debug::gdb_server::GDBServer;
#define STR7_VERBOSE
#endif
#ifdef STR7_DEBUG_INLINE
using unisim::service::debug::inline_debugger::InlineDebugger;
#define STR7_VERBOSE
#endif


#ifdef STR7_VERBOSE
typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, true> MEMORY;
typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, true> FLASH;
typedef unisim::component::tlm2::interconnect::generic_router::Router<RouterConfigVerbose> ROUTER;
#else
typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, false> MEMORY;
typedef unisim::component::tlm2::memory::ram::Memory<32, 1024 * 1024, false> FLASH;
typedef unisim::component::tlm2::interconnect::generic_router::Router<RouterConfig> ROUTER;
#endif
typedef unisim::component::tlm2::processor::arm::ARM7TDMI CPU;
typedef unisim::component::tlm2::timer::str7_timer::TIM<32> TIM;
typedef unisim::component::tlm2::interrupt::str7_eic::STR7_EIC<32> EIC;
typedef unisim::component::tlm2::interrupt::InterruptMasterStub IRQMSTUB;
typedef unisim::component::tlm2::interrupt::InterruptMasterStub FIQMSTUB;
typedef unisim::component::tlm2::interrupt::InterruptSlaveStub IRQSSTUB;
typedef unisim::component::tlm2::com::str7_spi::STR7_SPI<32, true> SPI;
typedef unisim::component::tlm2::signal_converter::generic_adc::ADC ADC;


typedef unisim::service::loader::elf_loader::ElfLoaderImpl<uint64_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> ElfLoader;

using unisim::service::time::sc_time::ScTime;
using unisim::service::time::host_time::HostTime;
using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;

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
	cerr << "            get the simulator default configuration xml file (you can use it to create your own configuration)" << endl << endl;
}

// Front Side Bus template parameters
//typedef CPU_CONFIG::address_t FSB_ADDRESS_TYPE;
//typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
//typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
//const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
//const uint32_t FSB_NUM_PROCS = 1;

int sc_main(int argc, char *argv[]) {

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
		{0, 0, 0, 0}
	};

	char const *set_config_name = "default_parameters.xml";
	char const *get_config_name = "default_parameters.xml";
	char const *get_variables_name = "default_variables.xml";
	bool get_variables = false;
	bool get_config = false;
	bool set_config = false;

	// Parse the command line arguments
	int c;
	while((c = getopt_long (argc, argv, "hv:g:c:", long_options, 0)) != -1) {
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
		}
	}

	if(optind != argc) {
		cerr << "Error? (optind = " << optind << ", argc = " << argc <<" )" << endl;
		help(argv[0]);
		return 0;
	}

	// Time
	ScTime *time = new ScTime("time");
	HostTime *host_time = new HostTime("host-time");
	
	ElfLoader *elf_loader = 0;
	MEMORY *memory = new MEMORY("memory");
	ROUTER *router = new ROUTER("router");
	CPU *cpu = new CPU("cpu");
	FLASH *flash = new FLASH("flash");
	EIC *eic = new EIC("eic");
	TIM *timer0 = new TIM("timer0");
	TIM *timer2 = new TIM("timer2");
	SPI *spi0 = new SPI("spi0");
	ADC *adc = new ADC("ADC");

	//Timer0  stubs
	IRQSSTUB *icia_irqsstub0 = new IRQSSTUB("icia_irqsstub0");
	IRQSSTUB *icib_irqsstub0 = new IRQSSTUB("icib_irqsstub0");
	IRQMSTUB *icapa_edgemstub0 = new IRQMSTUB("icapa_edgemstub0");
	IRQMSTUB *icapb_edgemstub0 = new IRQMSTUB("icapb_edgemstub0");
	IRQSSTUB *ocmpa_edgesstub0 = new IRQSSTUB("ocmpa_edgesstub0");
	IRQSSTUB *ocmpb_edgesstub0 = new IRQSSTUB("ocmpb_edgesstub0");
       
	//Timer2  stubs
	IRQSSTUB *toi_irqsstub2 = new IRQSSTUB("toi_irqsstub2");
	IRQSSTUB *ocia_irqsstub2 = new IRQSSTUB("ocia_irqsstub2");
	IRQSSTUB *ocib_irqsstub2 = new IRQSSTUB("ocib_irqsstub2");
	IRQSSTUB *icia_irqsstub2 = new IRQSSTUB("icia_irqsstub2");
	IRQSSTUB *icib_irqsstub2 = new IRQSSTUB("icib_irqsstub2");
	IRQMSTUB *icapb_edgemstub2 = new IRQMSTUB("icapb_edgemstub2");
   	IRQSSTUB *ocmpb_edgesstub2 = new IRQSSTUB("ocmpb_edgesstub2");


	//EIC stubs
	IRQMSTUB *irqmstubFLASH = new IRQMSTUB("irqmstubFLASH");
	IRQMSTUB *irqmstubPRCCU = new IRQMSTUB("irqmstubPRCCU");
	IRQMSTUB *irqmstubRTC = new IRQMSTUB("irqmstubRTC");
	IRQMSTUB *irqmstubWDG = new IRQMSTUB("irqmstubWDG");
	IRQMSTUB *irqmstubXTI = new IRQMSTUB("irqmstubXTI");
	IRQMSTUB *irqmstubUSB = new IRQMSTUB("irqmstubUSB");
	IRQMSTUB *irqmstubI2C0ITERR = new IRQMSTUB("irqmstubI2C0ITERR");
	IRQMSTUB *irqmstubI2C1ITERR = new IRQMSTUB("irqmstubI2C1ITERR");
	IRQMSTUB *irqmstubUART0 = new IRQMSTUB("irqmstubUART0");
	IRQMSTUB *irqmstubUART1 = new IRQMSTUB("irqmstubUART1");
	IRQMSTUB *irqmstubUART2 = new IRQMSTUB("irqmstubUART2");
	IRQMSTUB *irqmstubUART3 = new IRQMSTUB("irqmstubUART3");
	IRQMSTUB *irqmstubBSPI1 = new IRQMSTUB("irqmstubBSPI1");
	IRQMSTUB *irqmstubI2C0IRQ = new IRQMSTUB("irqmstubI2C0IRQ");
	IRQMSTUB *irqmstubI2C1IRQ = new IRQMSTUB("irqmstubI2C1IRQ");
	IRQMSTUB *irqmstubCAN = new IRQMSTUB("irqmstubCAN");
	IRQMSTUB *irqmstubADC = new IRQMSTUB("irqmstubADC");
	IRQMSTUB *irqmstubT1GLOBAL = new IRQMSTUB("irqmstubT1GLOBAL");
	IRQMSTUB *irqmstubT3GLOBAL = new IRQMSTUB("irqmstubT3GLOBAL");
	IRQMSTUB *irqmstubRESERVED0 = new IRQMSTUB("irqmstubRESERVED0");
	IRQMSTUB *irqmstubRESERVED1 = new IRQMSTUB("irqmstubRESERVED1");
	IRQMSTUB *irqmstubRESERVED2 = new IRQMSTUB("irqmstubRESERVED2");
	IRQMSTUB *irqmstubHDLC = new IRQMSTUB("irqmstubHDLC");
	IRQMSTUB *irqmstubUSBLPIRQ = new IRQMSTUB("irqmstubUSBLPIRQ");
	IRQMSTUB *irqmstubRESERVED3 = new IRQMSTUB("irqmstubRESERVED3");
	IRQMSTUB *irqmstubRESERVED4 = new IRQMSTUB("irqmstubRESERVED4");

	FIQMSTUB *fiqmstub[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		stringstream str;
		str << "fiq_master_stub[" << i << "]";
		fiqmstub[i] = new FIQMSTUB(str.str().c_str());
	}
	// Instanciate an ELF32 loader
	elf_loader = new ElfLoader("elf-loader");
	
#ifdef STR7_DEBUG
	GDBServer<uint64_t> *gdb_server = 
		new GDBServer<uint64_t>("gdb-server");
#endif // STR7_DEBUG
#ifdef STR7_DEBUG_INLINE
	InlineDebugger<uint64_t> *inline_debugger = 
		new InlineDebugger<uint64_t>("inline-debugger");
#endif // STR7_DEBUG_INLINE

	// Connect the CPU to the memory
	cpu->master_socket(router->targ_socket[0]);
	router->init_socket[0](memory->slave_sock);
	router->init_socket[1](flash->slave_sock);
	router->init_socket[2](eic->in_mem);
	router->init_socket[3](timer0->in_mem);
	router->init_socket[4](timer2->in_mem);
	router->init_socket[5](spi0->in_mem);

//Connect the interrupt controler
	eic->out_irq(cpu->in_irq);
	eic->out_fiq(cpu->in_fiq);
	timer0->timeri_irq(eic->in_irq[0]);
	irqmstubFLASH->out_interrupt(eic->in_irq[1]);
	irqmstubPRCCU->out_interrupt(eic->in_irq[2]);
	irqmstubRTC->out_interrupt(eic->in_irq[3]);
	irqmstubWDG->out_interrupt(eic->in_irq[4]);
	irqmstubXTI->out_interrupt(eic->in_irq[5]);
	irqmstubUSB->out_interrupt(eic->in_irq[6]);
	irqmstubI2C0ITERR->out_interrupt(eic->in_irq[7]);
	irqmstubI2C1ITERR->out_interrupt(eic->in_irq[8]);
	irqmstubUART0->out_interrupt(eic->in_irq[9]);
	irqmstubUART1->out_interrupt(eic->in_irq[10]);
	irqmstubUART2->out_interrupt(eic->in_irq[11]);
	irqmstubUART3->out_interrupt(eic->in_irq[12]);
	spi0->irq(eic->in_irq[13]);
	irqmstubBSPI1->out_interrupt(eic->in_irq[14]);
	irqmstubI2C0IRQ->out_interrupt(eic->in_irq[15]);
	irqmstubI2C1IRQ->out_interrupt(eic->in_irq[16]);
	irqmstubCAN->out_interrupt(eic->in_irq[17]);
	irqmstubADC->out_interrupt(eic->in_irq[18]);
	irqmstubT1GLOBAL->out_interrupt(eic->in_irq[19]);
	timer2->timeri_irq(eic->in_irq[20]);
	irqmstubT3GLOBAL->out_interrupt(eic->in_irq[21]);
	irqmstubRESERVED0->out_interrupt(eic->in_irq[22]);
	irqmstubRESERVED1->out_interrupt(eic->in_irq[23]);
	irqmstubRESERVED2->out_interrupt(eic->in_irq[24]);
	irqmstubHDLC->out_interrupt(eic->in_irq[25]);
	irqmstubUSBLPIRQ->out_interrupt(eic->in_irq[26]);
	irqmstubRESERVED3->out_interrupt(eic->in_irq[27]);
	irqmstubRESERVED4->out_interrupt(eic->in_irq[28]);
	timer0->toi_irq(eic->in_irq[29]);
	timer0->ocia_irq(eic->in_irq[30]);
	timer0->ocib_irq(eic->in_irq[31]);

	// TODO: timer0 timeri_irq should also be connected to eic->in_fiq[0]
	for (unsigned int i = 0; i < 2; i++)
		fiqmstub[i]->out_interrupt(eic->in_fiq[i]);

	//Connect the Timer0
	timer0->icia_irq(icia_irqsstub0->in_interrupt);
	timer0->icib_irq(icib_irqsstub0->in_interrupt);
	icapa_edgemstub0->out_interrupt(timer0->icapa_edge);
	icapb_edgemstub0->out_interrupt(timer0->icapb_edge);
	timer0->ocmpa_edge(ocmpa_edgesstub0->in_interrupt);
	timer0->ocmpb_edge(ocmpb_edgesstub0->in_interrupt);

	//Connect the Timer2
	timer2->toi_irq(toi_irqsstub2->in_interrupt);
	timer2->ocia_irq(ocia_irqsstub2->in_interrupt);
	timer2->ocib_irq(ocib_irqsstub2->in_interrupt);
	timer2->icia_irq(icia_irqsstub2->in_interrupt);
	timer2->icib_irq(icib_irqsstub2->in_interrupt);
	icapb_edgemstub2->out_interrupt(timer2->icapb_edge);
	timer2->ocmpa_edge(adc->chip_select);
	timer2->ocmpb_edge(ocmpb_edgesstub2->in_interrupt);

	//Connect the Spi0 to the ADC
	spi0->mosi (adc->mosi);
	adc->miso (spi0->miso);
	adc->data_ready(timer2->icapa_edge);

	// Connect everything
	elf_loader->memory_import >> router->memory_export;
	(*router->memory_import[0]) >> memory->memory_export;
	(*router->memory_import[1]) >> flash->memory_export;
	// TODO: missing a connection with the eic
	// TODO: missing a connection with the timer0

	cpu->symbol_table_lookup_import >> elf_loader->symbol_table_lookup_export;

#ifdef STR7_DEBUG
	cpu->debug_control_import >> gdb_server->debug_control_export;
	cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
	gdb_server->memory_import >> cpu->memory_export;
	gdb_server->registers_import >> cpu->registers_export;
	gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
#endif // STR7_DEBUG

#ifdef STR7_DEBUG_INLINE
	cpu->debug_control_import >> inline_debugger->debug_control_export;
	cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
	cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;
	timer0->trap_reporting_import >> inline_debugger->trap_reporting_export;
	eic->trap_reporting_import >> inline_debugger->trap_reporting_export;
	inline_debugger->disasm_import >> cpu->disasm_export;
	inline_debugger->memory_import >> cpu->memory_export;
	inline_debugger->registers_import >> cpu->registers_export;
	inline_debugger->symbol_table_lookup_import >> 
		elf_loader->symbol_table_lookup_export;
#endif // STR7_DEBUG_INLINE

#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(set_config)
		ServiceManager::LoadXmlParameters(set_config_name);
	if(get_config)
	{
		cerr << "getting simulator configuration in \"" << get_config_name << "\"" << endl;
		ServiceManager::XmlfyParameters(get_config_name);
		return 0;
	}
	if(!set_config) {
		if(!get_config) help(argv[0]);
		return 0;
	}

	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at system privilege level" << endl;

		double time_start = host_time->GetTime();

		EnableDebug();
		void (*prev_sig_int_handler)(int);

#ifndef STR7_DEBUG_INLINE
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
#endif // not STR7_DEBUG_INLINE

		try
		{
			sc_start();
		}
		catch(std::runtime_error& e)
		{
			cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
			cerr << e.what() << endl;
		}
		
#ifdef STR7_DEBUG_INLINE
		signal(SIGINT, prev_sig_int_handler);
#endif // STR7_DEBUG_INLINE

		cerr << "Simulation finished" << endl;
		cerr << "Simulation statistics:" << endl;

		double time_stop = host_time->GetTime();
		double spent_time = time_stop - time_start;

		cerr << "simulation time: " << spent_time << " seconds" << endl;
		cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
		cerr << "simulated instructions : " << cpu->GetInstructionCounter() << " instructions" << endl;
		cerr << "host simulation speed: " << ((double) cpu->GetInstructionCounter() / spent_time / 1000000.0) << " MIPS" << endl;
		cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
		if(get_variables)
		{
			cerr << "getting variables" << endl;
			ServiceManager::XmlfyVariables(get_variables_name);
		}
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
	}


	if (elf_loader) delete elf_loader;
	if (time) delete time;

	if (memory) delete memory;
	if (cpu) delete cpu;
	if (router) delete router;
	if (flash) delete flash;
	if (eic) delete eic;
	if (timer0) delete timer0;
if (timer2) delete timer2;
	
  
	if (icib_irqsstub0) delete icib_irqsstub0;
	if (icapa_edgemstub0) delete icapa_edgemstub0;
	if (icapb_edgemstub0) delete icapb_edgemstub0;
	if (ocmpa_edgesstub0) delete ocmpa_edgesstub0;
	if (ocmpb_edgesstub0) delete ocmpb_edgesstub0;
	if (toi_irqsstub2) delete toi_irqsstub2;
	if (ocia_irqsstub2) delete ocia_irqsstub2;
	if (ocib_irqsstub2) delete ocib_irqsstub2;
	if (icia_irqsstub2) delete icia_irqsstub2;
	if (icib_irqsstub2) delete icib_irqsstub2;
	if (icapb_edgemstub2) delete icapb_edgemstub2;
	if (ocmpb_edgesstub2) delete ocmpb_edgesstub2;
	if (irqmstubFLASH) delete irqmstubFLASH;
	if (irqmstubPRCCU) delete irqmstubPRCCU;
	if (irqmstubRTC) delete irqmstubRTC;
	if (irqmstubWDG) delete irqmstubWDG;
	if (irqmstubXTI) delete irqmstubXTI;
	if (irqmstubUSB) delete irqmstubUSB;
	if (irqmstubI2C0ITERR) delete irqmstubI2C0ITERR;
	if (irqmstubI2C1ITERR) delete irqmstubI2C1ITERR;
	if (irqmstubUART0) delete irqmstubUART0;
	if (irqmstubUART1) delete irqmstubUART1;
	if (irqmstubUART2) delete irqmstubUART2;
	if (irqmstubUART3) delete irqmstubUART3;
	if (irqmstubBSPI1) delete irqmstubBSPI1;
	if (irqmstubI2C0IRQ) delete irqmstubI2C0IRQ;
	if (irqmstubI2C1IRQ) delete irqmstubI2C1IRQ;
	if (irqmstubCAN) delete irqmstubCAN;
	if (irqmstubADC) delete irqmstubADC;
	if (irqmstubT1GLOBAL) delete irqmstubT1GLOBAL;
	if (irqmstubT3GLOBAL) delete irqmstubT3GLOBAL;
	if (irqmstubRESERVED0) delete irqmstubRESERVED0;
	if (irqmstubRESERVED1) delete irqmstubRESERVED1;
	if (irqmstubRESERVED2) delete irqmstubRESERVED2;
	if (irqmstubHDLC) delete irqmstubHDLC;
	if (irqmstubUSBLPIRQ) delete irqmstubUSBLPIRQ;
	if (irqmstubRESERVED3) delete irqmstubRESERVED3;
	if (irqmstubRESERVED4) delete irqmstubRESERVED4;
	
	for (int i = 0; i < 2; i++)
		if (fiqmstub[i]) delete fiqmstub[i];

#ifdef STR7_DEBUG
	if(gdb_server) delete gdb_server;
#endif // STR7_DEBUG
#ifdef STR7_DEBUG_INLINE
	if(inline_debugger) delete inline_debugger;
#endif // STR7_DEBUG_INLINE

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
