/*
 * simulator.cc
 *
 *  Created on: 15 mars 2011
 *      Author: rnouacer
 */

#include <simulator.hh>

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


Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, mmc(0)
	, crg(0)
	, ect(0)
	, atd1(0)
	, atd0(0)
	, pwm(0)
	, pit(0)
	, global_ram(0)
	, global_eeprom(0)
	, global_flash(0)
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
	, debugger(0)
	, pim_server(0)
	, gdb_server(0)
	, inline_debugger(0)
	, profiler(0)
	, sim_time(0)
	, host_time(0)
	, isS19(false)
	, enable_pim_server(false)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, dump_parameters(false)
	, dump_formulas(false)
	, dump_statistics(true)

	, endian("")
	, program_counter_name("")

	, param_enable_pim_server("enable-pim-server", 0, enable_pim_server, "Enable/Disable PIM server instantiation")
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_dump_parameters("dump-parameters", 0, dump_parameters, "")
	, param_dump_formulas("dump-formulas", 0, dump_formulas, "")
	, param_dump_statistics("dump-statistics", 0, dump_statistics, "")

	, null_stat_var(0)
	, stat_data_load_ratio("data-load-ratio %", 0, null_stat_var, "Data Load Ratio")
	, stat_data_store_ratio("data-store-ratio %", 0, null_stat_var, "Data Store Ratio")

{

	param_endian = new Parameter<string>("endian", 0, endian, "Target endianness");
	param_endian->SetMutable(false);
	param_endian->SetVisible(true);

	param_pc_reg_name = new Parameter<string>("program-counter-name", 0, program_counter_name, "Target CPU program counter name");
	param_pc_reg_name->SetMutable(false);
	param_pc_reg_name->SetVisible(true);

	stat_data_load_ratio.setCallBack(this, DATA_LOAD_RATIO, NULL, &CallBackObject::read);
	stat_data_store_ratio.setCallBack(this, DATA_STORE_RATIO, NULL, &CallBackObject::read);

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
	xgate =new XGATE("XGATE");

	mmc = 	new MMC("MMC");

	crg = new CRG("CRG");
	ect = new ECT("ECT");
	pit = new PIT("PIT");

	atd1 = new ATD1("ATD1");
	atd0 = new ATD0("ATD0");

	pwm = new PWM("PWM");

	//  - Memories
	global_ram = new RAM("RAM");
	global_flash = new FLASH("FLASH");
	global_eeprom = new EEPROM("EEPROM");

	// - Interrupt controller
	s12xint = new XINT("XINT");

	registersTee = new RegistersTee("registersTee");

	memoryImportExportTee = new MemoryImportExportTee("memoryImportExportTee");

	//  - Tee Memory Access Reporting
	tee_memory_access_reporting = enable_inline_debugger ? new MemoryAccessReportingTee("tee-memory-access-reporting") : 0;

	//  - profiler
	profiler = enable_inline_debugger ? new Profiler<CPU_ADDRESS_TYPE>("profiler") : 0;

	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server || enable_pim_server) ? new Debugger<CPU_ADDRESS_TYPE>("debugger") : 0;

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
		loaderS19 = new S19_Loader<CPU_ADDRESS_TYPE>("S19_Loader");
		loaderELF = new Elf32Loader("elf32-loader");
	} else {
		loaderELF = new Elf32Loader("elf32-loader");
	}

	//  - PIM server
	pim_server = enable_pim_server ? new PIMServer<CPU_ADDRESS_TYPE>("pim-server") : 0;

	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
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
	xgate->initiator_socket(mmc->xgate_socket);

	s12xint->toCPU12X_request(cpu->xint_interrupt_request);
	s12xint->toXGATE_request(xgate->xint_interrupt_request);

	crg->interrupt_request(s12xint->interrupt_request);
	ect->interrupt_request(s12xint->interrupt_request);
	pit->interrupt_request(s12xint->interrupt_request);
	pwm->interrupt_request(s12xint->interrupt_request);
	atd1->interrupt_request(s12xint->interrupt_request);
	atd0->interrupt_request(s12xint->interrupt_request);
	xgate->interrupt_request(s12xint->interrupt_request);
	global_eeprom->interrupt_request(s12xint->interrupt_request);

#ifdef HAVE_RTBCOB
	rtbStub->atd1_master_sock(atd1->anx_socket);
	rtbStub->atd0_master_sock(atd0->anx_socket);
	rtbStub->slave_sock(pwm->master_sock);
#else
	xml_atd_pwm_stub->atd1_master_sock(atd1->anx_socket);
	xml_atd_pwm_stub->atd0_master_sock(atd0->anx_socket);
	xml_atd_pwm_stub->slave_sock(pwm->master_sock);
#endif

	// This order is mandatory (see the memoryMapping)
	(*mmc->init_socket[0])(crg->slave_socket);
	(*mmc->init_socket[1])(ect->slave_socket);
	(*mmc->init_socket[2])(atd1->slave_socket);
	(*mmc->init_socket[3])(global_eeprom->slave_socket);
	(*mmc->init_socket[4])(s12xint->slave_socket);
	(*mmc->init_socket[5])(atd0->slave_socket);
	(*mmc->init_socket[6])(pwm->slave_socket);
	(*mmc->init_socket[7])(pit->slave_socket);
	(*mmc->init_socket[8])(xgate->target_socket);

	(*mmc->init_socket[9])(global_ram->slave_sock);
	(*mmc->init_socket[10])(global_eeprom->slave_sock);
	(*mmc->init_socket[11])(global_flash->slave_sock);

	crg->bus_clock_socket(cpu->bus_clock_socket);
	crg->bus_clock_socket(ect->bus_clock_socket);
	crg->bus_clock_socket(pit->bus_clock_socket);
	crg->bus_clock_socket(global_eeprom->bus_clock_socket);
	crg->bus_clock_socket(pwm->bus_clock_socket);
	crg->bus_clock_socket(atd1->bus_clock_socket);
	crg->bus_clock_socket(atd0->bus_clock_socket);
	crg->bus_clock_socket(xgate->bus_clock_socket);

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
	*(memoryImportExportTee->memory_import[6]) >> pit->memory_export;
	*(memoryImportExportTee->memory_import[7]) >> xgate->memory_export;

	*(memoryImportExportTee->memory_import[8]) >> global_ram->memory_export;
	*(memoryImportExportTee->memory_import[9]) >> global_eeprom->memory_export;
	*(memoryImportExportTee->memory_import[10]) >> global_flash->memory_export;

	mmc->memory_import >> memoryImportExportTee->memory_export;


	*(registersTee->registers_import[0]) >> cpu->registers_export;
	*(registersTee->registers_import[1]) >> mmc->registers_export;
	*(registersTee->registers_import[2]) >> s12xint->registers_export;
	*(registersTee->registers_import[3]) >> crg->registers_export;
	*(registersTee->registers_import[4]) >> atd1->registers_export;
	*(registersTee->registers_import[5]) >> atd0->registers_export;
	*(registersTee->registers_import[6]) >> pwm->registers_export;
	*(registersTee->registers_import[7]) >> ect->registers_export;
	*(registersTee->registers_import[8]) >> pit->registers_export;
	*(registersTee->registers_import[9]) >> xgate->registers_export;

// ***********************************************************
	if(enable_inline_debugger || enable_gdb_server || enable_pim_server)
	{
		if(enable_inline_debugger)
		{
			// Connect tee-memory-access-reporting to CPU, debugger and profiler
			cpu->memory_access_reporting_import >> tee_memory_access_reporting->in;
			*tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
			*tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
			profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];
			debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1];
			tee_memory_access_reporting->out_control >> cpu->memory_access_reporting_control_export;

		}
		else
		{
			// Connect CPU to debugger
			cpu->memory_access_reporting_import >> debugger->memory_access_reporting_export;
			debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
		}

		// Connect debugger to CPU

		cpu->debug_control_import >> debugger->debug_control_export;
		cpu->trap_reporting_import >> debugger->trap_reporting_export;
		debugger->disasm_import >> cpu->disasm_export;
		debugger->memory_import >> cpu->memory_export;

		debugger->registers_import >> registersTee->registers_export;

		pwm->trap_reporting_import >> debugger->trap_reporting_export;
		atd0->trap_reporting_import >> debugger->trap_reporting_export;
		atd1->trap_reporting_import >> debugger->trap_reporting_export;
		xgate->trap_reporting_import >> debugger->trap_reporting_export;

		mmc->trap_reporting_import >> debugger->trap_reporting_export;


	}

	if(enable_inline_debugger)
	{
		// Connect inline-debugger to debugger
		debugger->debug_event_listener_import >> inline_debugger->debug_event_listener_export;
		debugger->trap_reporting_import >> inline_debugger->trap_reporting_export;
		debugger->debug_control_import >> inline_debugger->debug_control_export;
		inline_debugger->debug_event_trigger_import >> debugger->debug_event_trigger_export;
		inline_debugger->disasm_import >> debugger->disasm_export;
		inline_debugger->memory_import >> debugger->memory_export;
		inline_debugger->registers_import >> debugger->registers_export;
		inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;
		inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;

		inline_debugger->backtrace_import >> debugger->backtrace_export;
		inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;
	}
	else if(enable_gdb_server)
	{
		// Connect gdb-server to debugger
		debugger->debug_control_import >> gdb_server->debug_control_export;
		debugger->debug_event_listener_import >> gdb_server->debug_event_listener_export;
		debugger->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->debug_event_trigger_import >> debugger->debug_event_trigger_export;
		gdb_server->memory_import >> debugger->memory_export;
		gdb_server->registers_import >> debugger->registers_export;
		gdb_server->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
	}
	else if (enable_pim_server)
	{
		// Connect pim-server to debugger
		debugger->debug_event_listener_import >> pim_server->debug_event_listener_export;
		debugger->trap_reporting_import >> pim_server->trap_reporting_export;
		debugger->debug_control_import >> pim_server->debug_control_export;
		pim_server->debug_event_trigger_import >> debugger->debug_event_trigger_export;
		pim_server->disasm_import >> debugger->disasm_export;
		pim_server->memory_import >> debugger->memory_export;
		pim_server->registers_import >> debugger->registers_export;
		pim_server->stmt_lookup_import >> debugger->stmt_lookup_export;
		pim_server->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;

	}

	if (isS19) {
		loaderS19->memory_import >> mmc->memory_export;
	}

	if (loaderELF) {
		loaderELF->memory_import >> mmc->memory_export;

		if(enable_inline_debugger || enable_gdb_server || enable_pim_server) {
			debugger->loader_import >> loaderELF->loader_export;
			debugger->blob_import >> loaderELF->blob_export;
		}

		cpu->symbol_table_lookup_import >> loaderELF->symbol_table_lookup_export;

	}

}

Simulator::~Simulator()
{

	if (pim_server) { delete pim_server; pim_server = NULL; }

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

	if(global_ram) { delete global_ram; global_ram = NULL; }
	if(global_flash) { delete global_eeprom; global_eeprom = NULL; }
	if (global_eeprom) { delete global_eeprom; global_eeprom = NULL; }

	if (xgate) { delete xgate; xgate = NULL; }
	if (crg) { delete crg; crg = NULL; }
	if (ect) { delete ect; ect = NULL; }
	if (pit) { delete pit; pit = NULL; }
	if (pwm) { delete pwm; pwm = NULL; }
	if (atd1) { delete atd1; atd1 = NULL; }
	if (atd0) { delete atd0; atd0 = NULL; }
	if (s12xint) { delete s12xint; s12xint = NULL; }
	if (mmc) { delete mmc; mmc = NULL; }

	if(cpu) { delete cpu; cpu = NULL; }
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM star12x");
	simulator->SetVariable("copyright", "Copyright (C) 2008-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Reda Nouacer <reda.nouacer@cea.fr>");

#ifdef HAVE_CONFIG_H
	simulator->SetVariable("version", VERSION);
#endif

	simulator->SetVariable("description", "UNISIM star12x, a Star12X System-on-Chip simulator with support of ELF32 binaries and s19 hex files, and targeted for automotive applications");

	simulator->SetVariable("enable-pim-server", false);
	simulator->SetVariable("enable-gdb-server", false);
	simulator->SetVariable("enable-inline-debugger", false);
	simulator->SetVariable("dump-parameters", false);
	simulator->SetVariable("dump-formulas", false);
	simulator->SetVariable("dump-statistics", true);

	simulator->SetVariable("endian", "big");
	simulator->SetVariable("program-counter-name", "CPU.PC");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_hcs12x.xml";
	const char *filename = "";
	const char *symbol_filename = "";

	bool force_use_virtual_address = true;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================



	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - PIM Server run-time configuration
	simulator->SetVariable("pim.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name
	simulator->SetVariable("pim.tcp-port", 1234);
	simulator->SetVariable("pim.filename", "pim.xml");

	simulator->SetVariable("pim-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("pim-server.architecture-description-filename", gdb_server_arch_filename);
	simulator->SetVariable("pim-server.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	simulator->SetVariable("gdb-server.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name

	simulator->SetVariable("debugger.parse-dwarf", true);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", "68hc12_dwarf_register_number_mapping.xml");

	simulator->SetVariable("S19_Loader.filename", filename);
	simulator->SetVariable("elf32-loader.filename", symbol_filename);
	simulator->SetVariable("elf32-loader.force-use-virtual-address", force_use_virtual_address);
	simulator->SetVariable("elf32-loader.initialize-extra-segment-bytes", false);

	simulator->SetVariable("atd-pwm-stub.anx-stimulus-period", 80000000); // 80 us
	simulator->SetVariable("atd-pwm-stub.pwm-fetch-period", 1e9); // 1 ms
	simulator->SetVariable("atd-pwm-stub.atd0-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd0-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd0-anx-wrap-around-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd1-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-wrap-around-channel", 0);
	simulator->SetVariable("atd-pwm-stub.trace-enable", false);
	simulator->SetVariable("atd-pwm-stub.atd0-stub-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd1-stub-enabled", false);

	simulator->SetVariable("ATD0.bus-cycle-time", 250000);
	simulator->SetVariable("ATD0.base-address", 0x2c0);
	simulator->SetVariable("ATD0.interrupt-offset", 0xd2);
	simulator->SetVariable("ATD0.vrl", 0.000000e+00);
	simulator->SetVariable("ATD0.vrh", 5.120000e+00);
	simulator->SetVariable("ATD0.debug-enabled", false);
	simulator->SetVariable("ATD0.use-atd-stub", false);
	simulator->SetVariable("ATD0.atd-anx-stimulus-file", "ATD.xml");
//	simulator->SetVariable("ATD0.start-scan-at", 900);
	simulator->SetVariable("ATD0.vih", 3.250000e+00);
	simulator->SetVariable("ATD0.vil", 1.750000e+00);
	simulator->SetVariable("ATD0.Has-External-Trigger", false);

	simulator->SetVariable("ATD1.bus-cycle-time", 250000);
	simulator->SetVariable("ATD1.base-address", 0x80);
	simulator->SetVariable("ATD1.interrupt-offset", 0xd0);
	simulator->SetVariable("ATD1.vrl", 0.000000e+00);
	simulator->SetVariable("ATD1.vrh", 5.120000e+00);
	simulator->SetVariable("ATD1.debug-enabled", false);
	simulator->SetVariable("ATD1.use-atd-stub", false);
	simulator->SetVariable("ATD1.atd-anx-stimulus-file", "ATD.xml");
//	simulator->SetVariable("ATD1.start-scan-at", 900);
	simulator->SetVariable("ATD1.vih", 3.250000e+00);
	simulator->SetVariable("ATD1.vil", 1.750000e+00);
	simulator->SetVariable("ATD1.Has-External-Trigger", false);

	simulator->SetVariable("XGATE.software_channel_id[0]", 0x39);
	simulator->SetVariable("XGATE.software_channel_id[1]", 0x38);
	simulator->SetVariable("XGATE.software_channel_id[2]", 0x37);
	simulator->SetVariable("XGATE.software_channel_id[3]", 0x36);
	simulator->SetVariable("XGATE.software_channel_id[4]", 0x35);
	simulator->SetVariable("XGATE.software_channel_id[5]", 0x34);
	simulator->SetVariable("XGATE.software_channel_id[6]", 0x33);
	simulator->SetVariable("XGATE.software_channel_id[7]", 0x32);
	simulator->SetVariable("XGATE.software-error-interrupt", 0x62);

	simulator->SetVariable("XGATE.trace-enable", false);
	simulator->SetVariable("XGATE.verbose-all", false);
	simulator->SetVariable("XGATE.verbose-setup", false);
	simulator->SetVariable("XGATE.verbose-step", false);
	simulator->SetVariable("XGATE.verbose-dump-regs-start", false);
	simulator->SetVariable("XGATE.verbose-dump-regs-end", false);
	simulator->SetVariable("XGATE.verbose-exception", false);
	simulator->SetVariable("XGATE.requires-memory-access-reporting", false);
	simulator->SetVariable("XGATE.requires-finished-instruction-reporting", false);
	simulator->SetVariable("XGATE.debug-enabled", false);
	simulator->SetVariable("XGATE.max-inst", 0xffffffffffffffffULL);
	simulator->SetVariable("XGATE.nice-time", "1 ms");
	simulator->SetVariable("XGATE.core-clock", 125000);
	simulator->SetVariable("XGATE.verbose-tlm-bus-synchronize", false);
	simulator->SetVariable("XGATE.verbose-tlm-run-thread", false);
	simulator->SetVariable("XGATE.verbose-tlm-commands", false);
	simulator->SetVariable("XGATE.trap-on-instruction-counter", -1);
	simulator->SetVariable("XGATE.enable-fine-timing", true);


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
	simulator->SetVariable("CPU.core-clock", 125000);
	simulator->SetVariable("CPU.verbose-tlm-bus-synchronize", false);
	simulator->SetVariable("CPU.verbose-tlm-run-thread", false);
	simulator->SetVariable("CPU.verbose-tlm-commands", false);
	simulator->SetVariable("CPU.trap-on-instruction-counter", -1);
	simulator->SetVariable("CPU.enable-fine-timing", true);

	simulator->SetVariable("CRG.oscillator-clock", 125000);
	simulator->SetVariable("CRG.base-address", 0x34);
	simulator->SetVariable("CRG.interrupt-offset-rti", 0xf0);
	simulator->SetVariable("CRG.interrupt-offset-pll-lock", 0xc6);
	simulator->SetVariable("CRG.interrupt-offset-self-clock-mode", 0xc4);
	simulator->SetVariable("CRG.debug-enabled", false);
	simulator->SetVariable("CRG.pll-stabilization-delay", 0.24);
	simulator->SetVariable("CRG.self-clock-mode-clock", 100000);

	simulator->SetVariable("ECT.bus-cycle-time", 250000);
	simulator->SetVariable("ECT.base-address", 0x40);
	simulator->SetVariable("ECT.interrupt-offset-channel0", 0xee);
	simulator->SetVariable("ECT.interrupt-offset-timer-overflow", 0xde);
	simulator->SetVariable("ECT.pulse-accumulatorA-overflow-interrupt", 0xDC);
	simulator->SetVariable("ECT.pulse-accumulatorB-overflow-interrupt", 0xC8);
	simulator->SetVariable("ECT.pulse-accumulatorA-input-edge-interrupt", 0xDA);
	simulator->SetVariable("ECT.modulus-counter-interrupt", 0xCA);
	simulator->SetVariable("ECT.debug-enabled", false);

	simulator->SetVariable("ECT.built-in-signal-generator-enable", true);
	simulator->SetVariable("ECT.built-in-signal-generator-period", 80000);

	simulator->SetVariable("PIT.bus-cycle-time", 250000);
	simulator->SetVariable("PIT.base-address", 0x0340);
	simulator->SetVariable("PIT.interrupt-offset-channel[0]", 0x7A);
	simulator->SetVariable("PIT.interrupt-offset-channel[1]", 0x78);
	simulator->SetVariable("PIT.interrupt-offset-channel[2]", 0x76);
	simulator->SetVariable("PIT.interrupt-offset-channel[3]", 0x74);
	simulator->SetVariable("PIT.interrupt-offset-channel[4]", 0x5E);
	simulator->SetVariable("PIT.interrupt-offset-channel[5]", 0x5C);
	simulator->SetVariable("PIT.interrupt-offset-channel[6]", 0x5A);
	simulator->SetVariable("PIT.interrupt-offset-channel[7]", 0x58);
	simulator->SetVariable("PIT.debug-enabled", false);

	simulator->SetVariable("MMC.debug-enabled", false);
	simulator->SetVariable("MMC.mode", 0x80);
	simulator->SetVariable("MMC.mmcctl1", 0x5);
	simulator->SetVariable("MMC.address-encoding", 0x0);
	simulator->SetVariable("MMC.ppage-address", 0x15); // ppage address for S12XE is 0x15

	simulator->SetVariable("PWM.bus-cycle-time", 250000);
	simulator->SetVariable("PWM.base-address", 0x300);
	simulator->SetVariable("PWM.interrupt-offset", 0x8c);
	simulator->SetVariable("PWM.debug-enabled", false);

	simulator->SetVariable("XINT.debug-enabled", false);

	simulator->SetVariable("RAM.org", 0x000800);
	simulator->SetVariable("RAM.bytesize", 1024*1024); // 1MByte
	simulator->SetVariable("RAM.initial-byte-value", 0x00);
	simulator->SetVariable("RAM.cycle-time", 250000);
	simulator->SetVariable("RAM.verbose", false);

	simulator->SetVariable("EEPROM.org", 0x100000);
	simulator->SetVariable("EEPROM.bytesize", 256*1024); // 256KByte
	simulator->SetVariable("EEPROM.initial-byte-value", 0xFF);
	simulator->SetVariable("EEPROM.cycle-time", 250000);
	simulator->SetVariable("EEPROM.oscillator-cycle-time", 125000);
	simulator->SetVariable("EEPROM.base-address", 0x0110);
	simulator->SetVariable("EEPROM.erase-fail-ratio", 0.01);
	simulator->SetVariable("EEPROM.command-interrupt", 0xBA);
	simulator->SetVariable("EEPROM.verbose", false);

	simulator->SetVariable("FLASH.org", 0x400000);
	simulator->SetVariable("FLASH.bytesize", 4*1024*1024); // 4MByte
	simulator->SetVariable("FLASH.initial-byte-value", 0xFF);
	simulator->SetVariable("FLASH.cycle-time", 250000);
	simulator->SetVariable("FLASH.verbose", false);

	simulator->SetVariable("kernel_logger.std_err", true);
	simulator->SetVariable("kernel_logger.std_out", false);
	simulator->SetVariable("kernel_logger.std_err_color", false);
	simulator->SetVariable("kernel_logger.std_out_color", false);
	simulator->SetVariable("kernel_logger.file", false);
	simulator->SetVariable("kernel_logger.filename", "logger_output.txt");
	simulator->SetVariable("kernel_logger.xml_file", true);
	simulator->SetVariable("kernel_logger.xml_filename", "logger_output.xml");
	simulator->SetVariable("kernel_logger.xml_file_gzipped", false);

	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
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

bool Simulator::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	uint64_t total_load = (uint64_t) (*cpu)["instruction-counter"] + (uint64_t) (*cpu)["data-load-counter"];
	uint64_t total_access = total_load + (uint64_t) (*cpu)["store-counter"];

	switch (offset) {
		case DATA_LOAD_RATIO: {
			*((double *) buffer) = (double) ((uint64_t) (*cpu)["data-load-counter"])/(total_access)*100;
			return (true);
		}
		case DATA_STORE_RATIO: {
			*((double *) buffer) = (double) ((uint64_t) (*cpu)["data-store-counter"])/(total_access)*100;
			return (true);
		}

	}

	return (false);
}

bool Simulator::write(unsigned int offset, const void *buffer, unsigned int data_length) {
	return (false);
}

void Simulator::Run() {

	// If no filename has been specified, abort simulation
	if(filename.empty())
	{
		std::cerr << "ERROR! No file to load. You should provide a file to load as command line argument." << std::endl;
		return;
	}

	physical_address_t entry_point;


	const unisim::util::debug::blob::Blob<physical_address_t>* blob = loaderELF->GetBlob();
	entry_point = blob->GetEntryPoint();

	address_t cpu_address;
	uint8_t page = 0;

	if (isS19) {
		mmc->splitPagedAddress(entry_point, page, cpu_address);
	} else {
		cpu_address = (address_t) entry_point;
	}

	cpu->setEntryPoint(cpu_address);

	cerr << "Starting simulation ..." << endl;

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
		cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	if(!inline_debugger)
	{
		signal(SIGINT, prev_sig_int_handler);
	}

	cerr << "Simulation finished" << endl << endl;

	if (dump_parameters) {
		cerr << "Simulation run-time parameters:" << endl;
		DumpParameters(cerr);
		cerr << endl;
	}

	if (dump_formulas) {
		cerr << "Simulation formulas:" << endl;
		DumpFormulas(cerr);
		cerr << endl;
	}

	if (dump_statistics) {
		double time_stop = host_time->GetTime();
		double spent_time = time_stop - time_start;

		cerr << "Simulation statistics:" << endl;
		DumpStatistics(cerr);
		cerr << endl;

		cerr << "Simulated time         : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
		cerr << "Core Clock   (MHz)     : " << (double) (1 / (double) (*cpu)["core-clock"] * 1000000)  << endl;
		cerr << "Target speed (MIPS)    : " << (((double) (*cpu)["instruction-counter"] / sc_time_stamp().to_seconds()) / 1000000.0) << endl;
		cerr << "Target speed (MHz)     : " << (((double) ((uint64_t) (*cpu)["cycles-counter"]) / sc_time_stamp().to_seconds()) / 1000000.0) << endl;
		cerr << "cycles-per-instruction : " << (double) ((uint64_t) (*cpu)["cycles-counter"]) / ((uint64_t) (*cpu)["instruction-counter"]) << endl;

		uint64_t total_load = (uint64_t) (*cpu)["instruction-counter"] + (uint64_t) (*cpu)["data-load-counter"];
		uint64_t total_access = total_load + (uint64_t) (*cpu)["store-counter"];

		cerr << "data-load ratio             : " << (double) ((uint64_t) (*cpu)["data-load-counter"])/(total_access)*100 << " %" << endl;
		cerr << "data-store ratio            : " << (double) ((uint64_t) (*cpu)["data-store-counter"])/(total_access)*100 << " %" << endl;
		cerr << endl;

		cerr << "simulation time        : " << spent_time << " seconds" << endl;
		cerr << "host simulation speed  : " << (((double) (*cpu)["instruction-counter"] / spent_time) / 1000000.0) << " MIPS" << endl;

		cerr << "time dilation          : " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
		cerr << endl;

	}

}

