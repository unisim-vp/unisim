/*
 * simulator.cc
 *
 *  Created on: 15 mars 2011
 *      Author: rnouacer
 */

#include <simulator.hh>
#include <unisim/util/endian/endian.hh>

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
//	sc_stop();
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, mmc(0)
	, mpu(0)
	, crg(0)
	, ect(0)
	, atd1(0)
	, atd0(0)
	, pwm(0)
	, pit(0)
	, sci0(0)
	, sci1(0)
	, sci2(0)
	, sci3(0)
	, sci4(0)
	, sci5(0)
	, sci6(0)
	, sci7(0)
	, can0(0)
	, can1(0)
	, can2(0)
	, can3(0)
	, can4(0)
	, spi0(0)
	, spi1(0)
	, spi2(0)
	, global_ram(0)
	, global_flash(0)
	, s12xint(0)
	, registersTee(0)
	, memoryImportExportTee(0)
#ifdef HAVE_RTBCOB
	, rtbStub(0)
#else
	, xml_atd_pwm_stub(0)
#endif
//	, loaderS19(0)
//	, loaderELF(0)
	, loader(0)

	, profiler(0)
	, debugger(0)
	, gdb_server(0)
	, pim_server(0)
	, inline_debugger(0)

	, sim_time(0)
	, host_time(0)

//	, filename("")
//	, symbol_filename("")

	, enable_pim_server(false)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_pim_server("enable-pim-server", 0, enable_pim_server, "Enable/Disable PIM server instantiation")
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")

	, sci_enable_telnet(false)
	, param_sci_enable_telnet("sci-enable-telnet", 0, sci_enable_telnet, "SCI Enable/Disable telnet instantiation")
	, spi_enable_telnet(false)
	, param_spi_enable_telnet("spi-enable-telnet", 0, spi_enable_telnet, "SPI Enable/Disable telnet instantiation")

	, endian("")
	, program_counter_name("")

	, exit_status(-1)
//	, isS19(false)

	, dump_parameters(false)
	, dump_formulas(false)
	, dump_statistics(true)

	, param_dump_parameters("dump-parameters", 0, dump_parameters, "")
	, param_dump_formulas("dump-formulas", 0, dump_formulas, "")
	, param_dump_statistics("dump-statistics", 0, dump_statistics, "")

	, null_stat_var(0)
	, stat_data_load_ratio("data-load-ratio %", 0, null_stat_var, "Data Load Ratio")
	, stat_data_store_ratio("data-store-ratio %", 0, null_stat_var, "Data Store Ratio")

	, prev_sig_int_handler(0)

	, spent_time(0)
	, isStop(false)

	, entry_point(0)
	, param_entry_point("entry-point", 0, entry_point, "Entry point, to use if there is no reset address")

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

//	VariableBase *cmd_args = FindVariable("cmd-args");
//	unsigned int cmd_args_length = cmd_args->GetLength();
//	if(cmd_args_length > 0)
//	{
//		filename = (string)(*cmd_args)[0];
//
//		std::cerr << "filename=\"" << filename << "\"" << std::endl;
//	}

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor

	mpu = 	new MPU("MPU");
	mmc = 	new MMC("MMC", mpu);

	cpu =new CPU("CPU", mmc);
	xgate =new XGATE("XGATE", mmc);

	crg = new CRG("CRG");
	ect = new ECT("ECT");
	pit = new PIT("PIT");

	sci0 = new S12SCI("SCI0");
	sci1 = new S12SCI("SCI1");
	sci2 = new S12SCI("SCI2");
	sci3 = new S12SCI("SCI3");
	sci4 = new S12SCI("SCI4");
	sci5 = new S12SCI("SCI5");
	sci6 = new S12SCI("SCI6");
	sci7 = new S12SCI("SCI7");

	can0 = new MSCAN("CAN0");
	can1 = new MSCAN("CAN1");
	can2 = new MSCAN("CAN2");
	can3 = new MSCAN("CAN3");
	can4 = new MSCAN("CAN4");

	spi0 = new S12SPI("SPI0");
	spi1 = new S12SPI("SPI1");
	spi2 = new S12SPI("SPI2");

	atd1 = new ATD1("ATD1");
	atd0 = new ATD0("ATD0");

	pwm = new PWM("PWM");

	//  - Memories
	global_ram = new RAM("RAM");
	global_flash = new FTM("FTM");

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

//	isS19 = (filename.find(".s19") != std::string::npos) ||
//		 (filename.find(".S19") != std::string::npos);
//
//	if (isS19) {
//		loaderS19 = new S19_Loader<CPU_ADDRESS_TYPE>("S19_Loader");
//		loaderELF = new Elf32Loader("elf32-loader");
//	} else {
//		loaderELF = new Elf32Loader("elf32-loader");
//	}

	//  - Multiformat loader
	loader = new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");

	//  - PIM server
	pim_server = enable_pim_server ? new PIMServer<CPU_ADDRESS_TYPE>("pim-server") : 0;

	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;

	// - telnet
	sci_telnet = (sci_enable_telnet) ? new unisim::service::telnet::Telnet("sci-telnet") : 0;
	spi_telnet = (spi_enable_telnet) ? new unisim::service::telnet::Telnet("spi-telnet") : 0;

	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");

	//=========================================================================
	//===                       Service parameterization     ===
	//=========================================================================

//	if (!filename.empty()) {
//		if(isS19) {
//			(*loaderS19)["filename"] = filename.c_str();
//		}
//		else
//		{
//			(*loaderELF)["filename"] = filename.c_str();
//		}
//	}

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	s12xint->toCPU12X_request(cpu->xint_interrupt_request);
	s12xint->toXGATE_request(xgate->xint_interrupt_request);

	crg->interrupt_request(s12xint->interrupt_request);
	ect->interrupt_request(s12xint->interrupt_request);
	pit->interrupt_request(s12xint->interrupt_request);
	pwm->interrupt_request(s12xint->interrupt_request);
	atd1->interrupt_request(s12xint->interrupt_request);
	atd0->interrupt_request(s12xint->interrupt_request);
	xgate->interrupt_request(s12xint->interrupt_request);

	global_flash->interrupt_request(s12xint->interrupt_request);
	mpu->interrupt_request(s12xint->interrupt_request);

	sci0->interrupt_request(s12xint->interrupt_request);
	sci1->interrupt_request(s12xint->interrupt_request);
	sci2->interrupt_request(s12xint->interrupt_request);
	sci3->interrupt_request(s12xint->interrupt_request);
	sci4->interrupt_request(s12xint->interrupt_request);
	sci5->interrupt_request(s12xint->interrupt_request);
	sci6->interrupt_request(s12xint->interrupt_request);
	sci7->interrupt_request(s12xint->interrupt_request);

	can0->interrupt_request(s12xint->interrupt_request);
	can1->interrupt_request(s12xint->interrupt_request);
	can2->interrupt_request(s12xint->interrupt_request);
	can3->interrupt_request(s12xint->interrupt_request);
	can4->interrupt_request(s12xint->interrupt_request);

	spi0->interrupt_request(s12xint->interrupt_request);
	spi1->interrupt_request(s12xint->interrupt_request);
	spi2->interrupt_request(s12xint->interrupt_request);

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
	mmc->init_socket(crg->slave_socket);
	mmc->init_socket(ect->slave_socket);
	mmc->init_socket(atd1->slave_socket);
	mmc->init_socket(sci2->slave_socket);
	mmc->init_socket(sci3->slave_socket);
	mmc->init_socket(sci0->slave_socket);
	mmc->init_socket(sci1->slave_socket);
	mmc->init_socket(spi0->slave_socket);
	mmc->init_socket(spi1->slave_socket);
	mmc->init_socket(spi2->slave_socket);
	mmc->init_socket(global_flash->slave_socket);
	mmc->init_socket(mpu->slave_socket);
	mmc->init_socket(s12xint->slave_socket);
	mmc->init_socket(sci4->slave_socket);
	mmc->init_socket(sci5->slave_socket);
	mmc->init_socket(can0->slave_socket);
	mmc->init_socket(can1->slave_socket);
	mmc->init_socket(can2->slave_socket);
	mmc->init_socket(can3->slave_socket);
	mmc->init_socket(can4->slave_socket);
	mmc->init_socket(atd0->slave_socket);
	mmc->init_socket(pwm->slave_socket);
	mmc->init_socket(sci6->slave_socket);
	mmc->init_socket(sci7->slave_socket);
	mmc->init_socket(pit->slave_socket);
	mmc->init_socket(xgate->target_socket);
	mmc->init_socket(global_ram->slave_sock);
	mmc->init_socket(global_flash->slave_sock);

	crg->bus_clock_socket(cpu->bus_clock_socket);
	crg->bus_clock_socket(ect->bus_clock_socket);
	crg->bus_clock_socket(pit->bus_clock_socket);

	crg->bus_clock_socket(global_flash->bus_clock_socket);

	crg->bus_clock_socket(pwm->bus_clock_socket);
	crg->bus_clock_socket(atd1->bus_clock_socket);
	crg->bus_clock_socket(atd0->bus_clock_socket);
	crg->bus_clock_socket(xgate->bus_clock_socket);
	crg->bus_clock_socket(sci0->bus_clock_socket);
	crg->bus_clock_socket(sci1->bus_clock_socket);
	crg->bus_clock_socket(sci2->bus_clock_socket);
	crg->bus_clock_socket(sci3->bus_clock_socket);
	crg->bus_clock_socket(sci4->bus_clock_socket);
	crg->bus_clock_socket(sci5->bus_clock_socket);
	crg->bus_clock_socket(sci6->bus_clock_socket);
	crg->bus_clock_socket(sci7->bus_clock_socket);
	crg->bus_clock_socket(can0->bus_clock_socket);
	crg->bus_clock_socket(can1->bus_clock_socket);
	crg->bus_clock_socket(can2->bus_clock_socket);
	crg->bus_clock_socket(can3->bus_clock_socket);
	crg->bus_clock_socket(can4->bus_clock_socket);
	crg->bus_clock_socket(spi0->bus_clock_socket);
	crg->bus_clock_socket(spi1->bus_clock_socket);
	crg->bus_clock_socket(spi2->bus_clock_socket);

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
	*(memoryImportExportTee->memory_import[9]) >> global_flash->memory_export;
	*(memoryImportExportTee->memory_import[10]) >> sci0->memory_export;
	*(memoryImportExportTee->memory_import[11]) >> sci1->memory_export;
	*(memoryImportExportTee->memory_import[12]) >> sci2->memory_export;
	*(memoryImportExportTee->memory_import[13]) >> sci3->memory_export;
	*(memoryImportExportTee->memory_import[14]) >> sci4->memory_export;
	*(memoryImportExportTee->memory_import[15]) >> sci5->memory_export;
	*(memoryImportExportTee->memory_import[16]) >> sci6->memory_export;
	*(memoryImportExportTee->memory_import[17]) >> sci7->memory_export;
	*(memoryImportExportTee->memory_import[18]) >> can0->memory_export;
	*(memoryImportExportTee->memory_import[19]) >> can1->memory_export;
	*(memoryImportExportTee->memory_import[20]) >> can2->memory_export;
	*(memoryImportExportTee->memory_import[21]) >> can3->memory_export;
	*(memoryImportExportTee->memory_import[22]) >> can4->memory_export;
	*(memoryImportExportTee->memory_import[23]) >> spi0->memory_export;
	*(memoryImportExportTee->memory_import[24]) >> spi1->memory_export;
	*(memoryImportExportTee->memory_import[25]) >> spi2->memory_export;
	*(memoryImportExportTee->memory_import[26]) >> mpu->memory_export;

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

	*(registersTee->registers_import[10]) >> global_flash->registers_export;

	*(registersTee->registers_import[11]) >> sci0->registers_export;
	*(registersTee->registers_import[12]) >> sci1->registers_export;
	*(registersTee->registers_import[13]) >> sci2->registers_export;
	*(registersTee->registers_import[14]) >> sci3->registers_export;
	*(registersTee->registers_import[15]) >> sci4->registers_export;
	*(registersTee->registers_import[16]) >> sci5->registers_export;
	*(registersTee->registers_import[17]) >> sci6->registers_export;
	*(registersTee->registers_import[18]) >> sci7->registers_export;

	*(registersTee->registers_import[19]) >> can0->registers_export;
	*(registersTee->registers_import[20]) >> can1->registers_export;
	*(registersTee->registers_import[21]) >> can2->registers_export;
	*(registersTee->registers_import[22]) >> can3->registers_export;
	*(registersTee->registers_import[23]) >> can4->registers_export;
	*(registersTee->registers_import[24]) >> spi0->registers_export;
	*(registersTee->registers_import[25]) >> spi1->registers_export;
	*(registersTee->registers_import[26]) >> spi2->registers_export;
	*(registersTee->registers_import[27]) >> mpu->registers_export;

// ***********************************************************
	cpu->loader_import >> loader->loader_export;

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
		debugger->loader_import >> loader->loader_export;
		debugger->blob_import >> loader->blob_export;

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
		inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;
		inline_debugger->disasm_import >> debugger->disasm_export;
		inline_debugger->memory_import >> debugger->memory_export;
		inline_debugger->registers_import >> debugger->registers_export;
		inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;
		inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;

		inline_debugger->backtrace_import >> debugger->backtrace_export;
		inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;

		inline_debugger->profiling_import >> profiler->profiling_export;
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

	if (sci_enable_telnet)
	{
		sci0->char_io_import >> sci_telnet->char_io_export;
	}

	if(spi_enable_telnet)
	{
		spi0->char_io_import >> spi_telnet->char_io_export;
	}

//	if (isS19) {
//		loaderS19->memory_import >> mmc->memory_export;
//	} else if (loaderELF) {
//		loaderELF->memory_import >> mmc->memory_export;
//	}
//
//	if (loaderELF) {
//
//		if(enable_inline_debugger || enable_gdb_server || enable_pim_server) {
//			debugger->loader_import >> loaderELF->loader_export;
//			debugger->blob_import >> loaderELF->blob_export;
//		}
//
//		cpu->symbol_table_lookup_import >> loaderELF->symbol_table_lookup_export;
//
//	}

	*loader->memory_import[0] >>  mmc->memory_export;
	loader->registers_import >> cpu->registers_export; // je ne comprend pas cette ligne !!!
	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

}

Simulator::~Simulator()
{

// ************
	if(!inline_debugger)
	{
		signal(SIGINT, prev_sig_int_handler);
	}

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

		cerr << "Simulation statistics:" << endl;
		DumpStatistics(cerr);
		cerr << endl;

		cerr << "CPU Clock   (MHz)      : " << (double) (1 / (double) (*cpu)["core-clock"] * 1000000)  << endl;
		cerr << "CPU CPI                : " << (double) ((uint64_t) (*cpu)["cycles-counter"]) / ((uint64_t) (*cpu)["instruction-counter"]) << endl;

		uint64_t total_load = (uint64_t) (*cpu)["instruction-counter"] + (uint64_t) (*cpu)["data-load-counter"];
		uint64_t total_access = total_load + (uint64_t) (*cpu)["store-counter"];
		total_access = ((total_access == 0)? 1: total_access);

		cerr << "CPU data-load ratio    : " << (double) ((uint64_t) (*cpu)["data-load-counter"])/(total_access)*100 << " %" << endl;
		cerr << "CPU data-store ratio   : " << (double) ((uint64_t) (*cpu)["data-store-counter"])/(total_access)*100 << " %" << endl;

		cerr << endl;

		cerr << "XGATE Clock (MHz)      : " << (double) (1 / (double) (*xgate)["core-clock"] * 1000000)  << endl;
		cerr << "XGATE CPI              : " << (double) ((uint64_t) (*xgate)["cycles-counter"]) / ((uint64_t) (*cpu)["instruction-counter"]) << endl;

		total_load = (uint64_t) (*xgate)["instruction-counter"] + (uint64_t) (*xgate)["data-load-counter"];
		total_access = total_load + (uint64_t) (*xgate)["store-counter"];
		total_access = ((total_access == 0)? 1: total_access);

		cerr << "XGATE data-load ratio    : " << (double) ((uint64_t) (*xgate)["data-load-counter"])/(total_access)*100 << " %" << endl;
		cerr << "XGATE data-store ratio   : " << (double) ((uint64_t) (*xgate)["data-store-counter"])/(total_access)*100 << " %" << endl;

		cerr << endl;

		cerr << "Target Simulated time  : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
		cerr << "Target speed (MHz)     : " << (((double) (((uint64_t) (*cpu)["cycles-counter"]) + ((uint64_t) (*xgate)["cycles-counter"])) / sc_time_stamp().to_seconds()) / 1000000.0) << endl;
		cerr << "Target speed (MIPS)    : " << ((((double) (*cpu)["instruction-counter"] + (double) (*xgate)["instruction-counter"]) / sc_time_stamp().to_seconds()) / 1000000.0) << endl;

		cerr << "Host simulation time   : " << spent_time << " seconds" << endl;
		cerr << "Host simulation speed  : " << ((((double) (*cpu)["instruction-counter"] + (double) (*xgate)["instruction-counter"]) / spent_time) / 1000000.0) << " MIPS" << endl;

		cerr << "Time dilation          : " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
		cerr << endl;

	}

// ****************************************************


	if (pim_server) { delete pim_server; pim_server = NULL; }

	if (registersTee) { delete registersTee; registersTee = NULL; }
	if (memoryImportExportTee) { delete memoryImportExportTee; memoryImportExportTee = NULL; }

	if(gdb_server) { delete gdb_server; gdb_server = NULL; }
	if(inline_debugger) { delete inline_debugger; inline_debugger = NULL; }

	if (sci_telnet) { delete sci_telnet; sci_telnet = NULL; }
	if (spi_telnet) { delete spi_telnet; spi_telnet = NULL; }

	if (host_time) { delete host_time; host_time = NULL; }
	if(sim_time) { delete sim_time; sim_time = NULL; }

	if(loader) delete loader;
//	if(loaderS19) { delete loaderS19; loaderS19 = NULL; }
//	if(loaderELF) { delete loaderELF; loaderELF = NULL; }

#ifdef HAVE_RTBCOB
	if (rtbStub) { delete rtbStub; rtbStub = NULL; }
#else
	if (xml_atd_pwm_stub) { delete xml_atd_pwm_stub; xml_atd_pwm_stub = NULL; }
#endif

	if(global_ram) { delete global_ram; global_ram = NULL; }
	if(global_flash) { delete global_flash; global_flash = NULL; }

	if (xgate) { delete xgate; xgate = NULL; }
	if (crg) { delete crg; crg = NULL; }
	if (ect) { delete ect; ect = NULL; }
	if (pit) { delete pit; pit = NULL; }
	if (pwm) { delete pwm; pwm = NULL; }
	if (atd1) { delete atd1; atd1 = NULL; }
	if (atd0) { delete atd0; atd0 = NULL; }
	if (s12xint) { delete s12xint; s12xint = NULL; }
	if (mmc) { delete mmc; mmc = NULL; }
	if (mpu) { delete mpu; mpu = NULL; }
	if (sci0) { delete sci0; sci0 = NULL; }
	if (sci1) { delete sci1; sci1 = NULL; }
	if (sci2) { delete sci2; sci2 = NULL; }
	if (sci3) { delete sci3; sci3 = NULL; }
	if (sci4) { delete sci4; sci4 = NULL; }
	if (sci5) { delete sci5; sci5 = NULL; }
	if (sci6) { delete sci6; sci6 = NULL; }
	if (sci7) { delete sci7; sci7 = NULL; }

	if (can0) { delete can0; can0 = NULL; }
	if (can1) { delete can1; can1 = NULL; }
	if (can2) { delete can2; can2 = NULL; }
	if (can3) { delete can3; can3 = NULL; }
	if (can4) { delete can4; can4 = NULL; }

	if (spi0) { delete spi0; spi0 = NULL; }
	if (spi1) { delete spi1; spi1 = NULL; }
	if (spi2) { delete spi2; spi2 = NULL; }

	if(cpu) { delete cpu; cpu = NULL; }

#if defined(WIN32) || defined(WIN64)
	// releases the winsock2 resources
	WSACleanup();
#endif

}


Simulator::SetupStatus Simulator::Setup()
{
// *********************************
#if defined(WIN32) || defined(WIN64)
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cerr << "WSAStartup failed" << endl;
		return unisim::kernel::service::Simulator::ST_ERROR;
	}
#endif

// **********************************

	Simulator::SetupStatus result = unisim::kernel::service::Simulator::Setup();

// **********
//	physical_address_t entry_point = 0;
	std::cout << "entry-point 0x" << std::hex << entry_point << std::dec << std::endl;

	address_t cpu_address;
	uint8_t page = 0;

//	if (isS19) {
//		entry_point = loaderS19->GetEntryPoint();
//		if (entry_point == 0) {
//			address_t reset_addr;
//			const address_t reset_vect = 0xFFFE;
//			cpu->ReadMemory(reset_vect, &reset_addr, 2);
//
//			entry_point = unisim::util::endian::BigEndian2Host(reset_addr);
//		}
//		mmc->splitPagedAddress(entry_point, page, cpu_address);
//	} else {
//		const unisim::util::debug::blob::Blob<physical_address_t>* blob = loaderELF->GetBlob();
//		if (blob != NULL) {
//			entry_point = blob->GetEntryPoint();
//		}
//
//		cpu_address = (address_t) entry_point;
//	}
// *****************

/* TODO: Discuss with Gilles about this
	const unisim::util::debug::blob::Blob<physical_address_t>* blob = loader->GetBlob();
	if (blob != NULL) {
		entry_point = blob->GetEntryPoint();
	}
*/
	if (entry_point == 0) {
		address_t reset_addr;
		const address_t reset_vect = 0xFFFE;
		cpu->ReadMemory(reset_vect, &reset_addr, 2);

		entry_point = unisim::util::endian::BigEndian2Host(reset_addr);
		mmc->splitPagedAddress(entry_point, page, cpu_address);
	}
	else {
		cpu_address = (address_t) entry_point;
	}
// *****************

	cpu->setEntryPoint(cpu_address);

	EnableDebug();
	prev_sig_int_handler = 0;

	if(!inline_debugger)
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}

	isStop = false;

	return result;
}

bool Simulator::RunSample(double inVal) {

	if (!isStop) {
		cerr << "Starting simulation RunSample ..." << endl;

		double time_start = host_time->GetTime();

		try
		{
			sc_start(inVal, SC_MS);
		}
		catch(std::runtime_error& e)
		{
			cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << endl;
			cerr << e.what() << endl;
		}

		double time_stop = host_time->GetTime();

		spent_time += time_stop - time_start;

		cerr << "Finishing simulation RunSample " << endl;

		return true;
	}

	return false;
}

void Simulator::Run() {

	cerr << "Starting simulation ..." << endl;

	double time_start = host_time->GetTime();

	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	double time_stop = host_time->GetTime();

	spent_time += time_stop - time_start;

	cerr << "Simulation finished" << endl << endl;


}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	isStop = true;

	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}

	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	if(!asynchronous)
	{
		switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
		{
			case SC_THREAD_PROC_:
			case SC_CTHREAD_PROC_:
				wait();
				break;
			default:
				break;
		}
	}

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

	simulator->SetVariable("entry-point", 0x0000);

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

	simulator->SetVariable("pim-server.tcp-port", 0);
	simulator->SetVariable("pim-server.architecture-description-filename", "gdb_hcs12x.xml");
	simulator->SetVariable("pim-server.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", 0);
	simulator->SetVariable("gdb-server.architecture-description-filename", "gdb_hcs12x.xml");
	simulator->SetVariable("gdb-server.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name

	simulator->SetVariable("debugger.parse-dwarf", true);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", "68hc12_dwarf_register_number_mapping.xml");

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	sstr_loader_mapping << "MMC:0x000000-0xFFFFFF" << std::dec;
	simulator->SetVariable("loader.memory-mapper.mapping", sstr_loader_mapping.str().c_str()); // 256 MB RAM / 256 KB BRAM / 32 MB Flash memory

//	simulator->SetVariable("S19_Loader.filename", "");
//	simulator->SetVariable("elf32-loader.filename", "");
//	simulator->SetVariable("elf32-loader.force-use-virtual-address", true);
//	simulator->SetVariable("elf32-loader.initialize-extra-segment-bytes", false);

	simulator->SetVariable("atd-pwm-stub.anx-stimulus-period", 80000000); // 80 us
	simulator->SetVariable("atd-pwm-stub.pwm-fetch-period", 1e9); // 1 ms
	simulator->SetVariable("atd-pwm-stub.atd0-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd0-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd0-anx-wrap-around-channel", 7);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd1-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-wrap-around-channel", 15);
	simulator->SetVariable("atd-pwm-stub.trace-enabled", false);
	simulator->SetVariable("atd-pwm-stub.cosim-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd0-stub-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd1-stub-enabled", false);

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

	simulator->SetVariable("XGATE.version", "V3");
	simulator->SetVariable("XGATE.base-address", 0x0380);
	simulator->SetVariable("XGATE.software_channel_id[0]", 0x39);
	simulator->SetVariable("XGATE.software_channel_id[1]", 0x38);
	simulator->SetVariable("XGATE.software_channel_id[2]", 0x37);
	simulator->SetVariable("XGATE.software_channel_id[3]", 0x36);
	simulator->SetVariable("XGATE.software_channel_id[4]", 0x35);
	simulator->SetVariable("XGATE.software_channel_id[5]", 0x34);
	simulator->SetVariable("XGATE.software_channel_id[6]", 0x33);
	simulator->SetVariable("XGATE.software_channel_id[7]", 0x32);
	simulator->SetVariable("XGATE.software-error-interrupt", 0x62);

	simulator->SetVariable("XGATE.enable-trace", false);
	simulator->SetVariable("XGATE.enable-file-trace", false),
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

	simulator->SetVariable("CPU.enable-trace", false);
	simulator->SetVariable("CPU.enable-file-trace", false),
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

	simulator->SetVariable("CRG.oscillator-clock", 125000); // 8 MHz
	simulator->SetVariable("CRG.base-address", 0x34);
	simulator->SetVariable("CRG.interrupt-offset-rti", 0xf0);
	simulator->SetVariable("CRG.interrupt-offset-pll-lock", 0xc6);
	simulator->SetVariable("CRG.interrupt-offset-self-clock-mode", 0xc4);
	simulator->SetVariable("CRG.debug-enabled", false);
	simulator->SetVariable("CRG.pll-stabilization-delay", 0.24);
	simulator->SetVariable("CRG.self-clock-mode-clock", 125000);

	simulator->SetVariable("ECT.bus-cycle-time", 250000);
	simulator->SetVariable("ECT.base-address", 0x40);
	simulator->SetVariable("ECT.interrupt-offset-channel0", 0xee);
	simulator->SetVariable("ECT.interrupt-offset-timer-overflow", 0xde);
	simulator->SetVariable("ECT.pulse-accumulatorA-overflow-interrupt", 0xDC);
	simulator->SetVariable("ECT.pulse-accumulatorB-overflow-interrupt", 0xC8);
	simulator->SetVariable("ECT.pulse-accumulatorA-input-edge-interrupt", 0xDA);
	simulator->SetVariable("ECT.modulus-counter-interrupt", 0xCA);
	simulator->SetVariable("ECT.debug-enabled", false);

	simulator->SetVariable("ECT.built-in-signal-generator-enable", false);
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

	simulator->SetVariable("sci-telnet.telnet-tcp-port", 1234);
	simulator->SetVariable("sci-enable-telnet", false);

	simulator->SetVariable("SCI2.bus-cycle-time", 250000);
	simulator->SetVariable("SCI2.base-address", 0x00B8);
	simulator->SetVariable("SCI2.interrupt-offset", 0x8A);
	simulator->SetVariable("SCI2.txd-pin-enable", true);
	simulator->SetVariable("SCI2.telnet-enabled", false);
	simulator->SetVariable("SCI2.debug-enabled", false);
	simulator->SetVariable("SCI2.TXD", true);
	simulator->SetVariable("SCI2.RXD", true);

	simulator->SetVariable("SCI3.bus-cycle-time", 250000);
	simulator->SetVariable("SCI3.base-address", 0x00C0);
	simulator->SetVariable("SCI3.interrupt-offset", 0x88);
	simulator->SetVariable("SCI3.txd-pin-enable", true);
	simulator->SetVariable("SCI3.telnet-enabled", false);
	simulator->SetVariable("SCI3.debug-enabled", false);
	simulator->SetVariable("SCI3.TXD", true);
	simulator->SetVariable("SCI3.RXD", true);

	simulator->SetVariable("SCI0.bus-cycle-time", 250000);
	simulator->SetVariable("SCI0.base-address", 0x00C8);
	simulator->SetVariable("SCI0.interrupt-offset", 0xD6);
	simulator->SetVariable("SCI0.txd-pin-enable", true);
	simulator->SetVariable("SCI0.telnet-enabled", false);
	simulator->SetVariable("SCI0.debug-enabled", false);
	simulator->SetVariable("SCI0.TXD", true);
	simulator->SetVariable("SCI0.RXD", true);

	simulator->SetVariable("SCI1.bus-cycle-time", 250000);
	simulator->SetVariable("SCI1.base-address", 0x00D0);
	simulator->SetVariable("SCI1.interrupt-offset", 0xD4);
	simulator->SetVariable("SCI1.txd-pin-enable", true);
	simulator->SetVariable("SCI1.telnet-enabled", false);
	simulator->SetVariable("SCI1.debug-enabled", false);
	simulator->SetVariable("SCI1.TXD", true);
	simulator->SetVariable("SCI1.RXD", true);

	simulator->SetVariable("SCI4.bus-cycle-time", 250000);
	simulator->SetVariable("SCI4.base-address", 0x0130);
	simulator->SetVariable("SCI4.interrupt-offset", 0x86);
	simulator->SetVariable("SCI4.txd-pin-enable", true);
	simulator->SetVariable("SCI4.telnet-enabled", false);
	simulator->SetVariable("SCI4.debug-enabled", false);
	simulator->SetVariable("SCI4.TXD", true);
	simulator->SetVariable("SCI4.RXD", true);

	simulator->SetVariable("SCI5.bus-cycle-time", 250000);
	simulator->SetVariable("SCI5.base-address", 0x0138);
	simulator->SetVariable("SCI5.interrupt-offset", 0x84);
	simulator->SetVariable("SCI5.txd-pin-enable", true);
	simulator->SetVariable("SCI5.telnet-enabled", false);
	simulator->SetVariable("SCI5.debug-enabled", false);
	simulator->SetVariable("SCI5.TXD", true);
	simulator->SetVariable("SCI5.RXD", true);

	simulator->SetVariable("CAN0.bus-cycle-time", 250000);
	simulator->SetVariable("CAN0.base-address", 0x0140);
	simulator->SetVariable("CAN0.transmit-interrupt-offset", 0xB0);
	simulator->SetVariable("CAN0.receive-interrupt-offset", 0xB2);
	simulator->SetVariable("CAN0.errors-interrupt-offset", 0xB4);
	simulator->SetVariable("CAN0.wakeup-interrupt-offset", 0xB6);
	simulator->SetVariable("CAN0.telnet-enabled", false);
	simulator->SetVariable("CAN0.debug-enabled", false);
	simulator->SetVariable("CAN0.TXD", true);
	simulator->SetVariable("CAN0.RXD", true);

	simulator->SetVariable("CAN1.bus-cycle-time", 250000);
	simulator->SetVariable("CAN1.base-address", 0x0180);
	simulator->SetVariable("CAN1.transmit-interrupt-offset", 0xA8);
	simulator->SetVariable("CAN1.receive-interrupt-offset", 0xAA);
	simulator->SetVariable("CAN1.errors-interrupt-offset", 0xAC);
	simulator->SetVariable("CAN1.wakeup-interrupt-offset", 0xAE);
	simulator->SetVariable("CAN1.telnet-enabled", false);
	simulator->SetVariable("CAN1.debug-enabled", false);
	simulator->SetVariable("CAN1.TXD", true);
	simulator->SetVariable("CAN1.RXD", true);

	simulator->SetVariable("CAN2.bus-cycle-time", 250000);
	simulator->SetVariable("CAN2.base-address", 0x01C0);
	simulator->SetVariable("CAN2.transmit-interrupt-offset", 0xA0);
	simulator->SetVariable("CAN2.receive-interrupt-offset", 0xA2);
	simulator->SetVariable("CAN2.errors-interrupt-offset", 0xA4);
	simulator->SetVariable("CAN2.wakeup-interrupt-offset", 0xA6);
	simulator->SetVariable("CAN2.telnet-enabled", false);
	simulator->SetVariable("CAN2.debug-enabled", false);
	simulator->SetVariable("CAN2.TXD", true);
	simulator->SetVariable("CAN2.RXD", true);

	simulator->SetVariable("CAN3.bus-cycle-time", 250000);
	simulator->SetVariable("CAN3.base-address", 0x0200);
	simulator->SetVariable("CAN3.transmit-interrupt-offset", 0x98);
	simulator->SetVariable("CAN3.receive-interrupt-offset", 0x9A);
	simulator->SetVariable("CAN3.errors-interrupt-offset", 0x9C);
	simulator->SetVariable("CAN3.wakeup-interrupt-offset", 0x9E);
	simulator->SetVariable("CAN3.telnet-enabled", false);
	simulator->SetVariable("CAN3.debug-enabled", false);
	simulator->SetVariable("CAN3.TXD", true);
	simulator->SetVariable("CAN3.RXD", true);

	simulator->SetVariable("CAN4.bus-cycle-time", 250000);
	simulator->SetVariable("CAN4.base-address", 0x0280);
	simulator->SetVariable("CAN4.transmit-interrupt-offset", 0x48);
	simulator->SetVariable("CAN4.receive-interrupt-offset", 0x4A);
	simulator->SetVariable("CAN4.errors-interrupt-offset", 0x4C);
	simulator->SetVariable("CAN4.wakeup-interrupt-offset", 0x4E);
	simulator->SetVariable("CAN4.telnet-enabled", false);
	simulator->SetVariable("CAN4.debug-enabled", false);
	simulator->SetVariable("CAN4.TXD", true);
	simulator->SetVariable("CAN4.RXD", true);

	simulator->SetVariable("spi-telnet.telnet-tcp-port", 1234);
	simulator->SetVariable("spi-enable-telnet", false);

	simulator->SetVariable("SPI0.bus-cycle-time", 250000);
	simulator->SetVariable("SPI0.base-address", 0x00D8);
	simulator->SetVariable("SPI0.interrupt-offset", 0xD8);
	simulator->SetVariable("SPI0.debug-enabled", false);
	simulator->SetVariable("SPI0.txd-pin-enable", true);
	simulator->SetVariable("SPI0.telnet-enabled", false);

	simulator->SetVariable("SPI1.bus-cycle-time", 250000);
	simulator->SetVariable("SPI1.base-address", 0x00F0);
	simulator->SetVariable("SPI1.interrupt-offset", 0xBE);
	simulator->SetVariable("SPI1.debug-enabled", false);
	simulator->SetVariable("SPI1.txd-pin-enable", true);
	simulator->SetVariable("SPI1.telnet-enabled", false);

	simulator->SetVariable("SPI2.bus-cycle-time", 250000);
	simulator->SetVariable("SPI2.base-address", 0x00F8);
	simulator->SetVariable("SPI2.interrupt-offset", 0xBC);
	simulator->SetVariable("SPI2.debug-enabled", false);
	simulator->SetVariable("SPI2.txd-pin-enable", true);
	simulator->SetVariable("SPI2.telnet-enabled", false);

	simulator->SetVariable("MMC.version", "V4");
	simulator->SetVariable("MMC.debug-enabled", false);
	simulator->SetVariable("MMC.mode", 0x80);
	simulator->SetVariable("MMC.mmcctl1", 0x5);
	simulator->SetVariable("MMC.address-encoding", 0x0);
	simulator->SetVariable("MMC.ppage-address", 0x15); // ppage address for S12XE is 0x15

/*
	simulator->SetVariable("MMC.memory-map",
"0,0034,003F;1,0040,007F;2,0080,00AF;3,00B8,00BF;4,00C0,00C7;5,00C8,00CF;\
6,00D0,00D7;7,00D8,00DF;8,00F0,00F7;9,00F8,00FF;10,0100,0113;11,0114,011F;\
12,0120,012F;13,0130,0137;14,0138,013F;15,0140,017F;16,0180,01BF;17,01C0,01FF;\
18,0200,023F;19,0280,02BF;20,02C0,02EF;21,0300,0327;22,330,337;23,338,33F;\
24,0340,0367;25,0380,03BF;26,0007FF,0FFFFF;27,100000,13FFFF;27,400000,7FFFFF");
*/

	// index 27 reference two memory regions (EEPROM, FLASH). For S12XE, the eeprom is emulated by flash
	simulator->SetVariable("MMC.memory-map",
"0,0034,003F;\
1,0040,007F;\
2,0080,00AF;\
3,00B8,00BF;\
4,00C0,00C7;\
5,00C8,00CF;\
6,00D0,00D7;\
7,00D8,00DF;\
8,00F0,00F7;\
9,00F8,00FF;\
10,0100,0113;\
11,0114,011F;\
12,0120,012F;\
13,0130,0137;\
14,0138,013F;\
15,0140,017F;\
16,0180,01BF;\
17,01C0,01FF;\
18,0200,023F;\
19,0280,02BF;\
20,02C0,02EF;\
21,0300,0327;\
22,0330,0337;\
23,0338,033F;\
24,0340,0367;\
25,0380,03BF;\
26,0007FF,0FFFFF;\
27,100000,13FFFF;\
27,400000,7FFFFF"
	);

	simulator->SetVariable("MPU.debug-enabled", false);
	simulator->SetVariable("MPU.base-address", 0x0114);
	simulator->SetVariable("MPU.interrupt-offset", 0x14);

	simulator->SetVariable("PWM.bus-cycle-time", 250000);
	simulator->SetVariable("PWM.base-address", 0x300);
	simulator->SetVariable("PWM.interrupt-offset", 0x8c);
	simulator->SetVariable("PWM.debug-enabled", false);

	simulator->SetVariable("SCI6.bus-cycle-time", 250000);
	simulator->SetVariable("SCI6.base-address", 0x0330);
	simulator->SetVariable("SCI6.interrupt-offset", 0xC2);
	simulator->SetVariable("SCI6.txd-pin-enable", true);
	simulator->SetVariable("SCI6.telnet-enabled", false);
	simulator->SetVariable("SCI6.debug-enabled", false);
	simulator->SetVariable("SCI6.TXD", true);
	simulator->SetVariable("SCI6.RXD", true);

	simulator->SetVariable("SCI7.bus-cycle-time", 250000);
	simulator->SetVariable("SCI7.base-address", 0x0338);
	simulator->SetVariable("SCI7.interrupt-offset", 0x56);
	simulator->SetVariable("SCI7.txd-pin-enable", true);
	simulator->SetVariable("SCI7.telnet-enabled", false);
	simulator->SetVariable("SCI7.debug-enabled", false);
	simulator->SetVariable("SCI7.TXD", true);
	simulator->SetVariable("SCI7.RXD", true);

	simulator->SetVariable("XINT.debug-enabled", false);

	simulator->SetVariable("RAM.org", 0x000800);
	simulator->SetVariable("RAM.bytesize", 1024*1024); // 1MByte
	simulator->SetVariable("RAM.initial-byte-value", 0x00);
	simulator->SetVariable("RAM.cycle-time", 250000);
	simulator->SetVariable("RAM.verbose", false);

	simulator->SetVariable("FTM.org", 0x100000);
	simulator->SetVariable("FTM.bytesize", 7*1024*1024); // 7MByte
	simulator->SetVariable("FTM.initial-byte-value", 0xFF);
	simulator->SetVariable("FTM.cycle-time", 250000);
	simulator->SetVariable("FTM.bus-cycle-time", 250000);
	simulator->SetVariable("FTM.oscillator-cycle-time", 125000);
	simulator->SetVariable("FTM.base-address", 0x0100);
	simulator->SetVariable("FTM.erase-fail-ratio", 0.01);
	simulator->SetVariable("FTM.flash-Security-Byte-Address", 0x7FFF0F);
	simulator->SetVariable("FTM.blackdoor-comparison-key-address", 0x7FFF00);
	simulator->SetVariable("FTM.Protection-Byte-Address", 0x7FFF0C);
	simulator->SetVariable("FTM.EEE-Protection-Byte-Address", 0x7FFF0D);
	simulator->SetVariable("FTM.Flash-Option-Byte-Address", 0x7FFF0E);
	simulator->SetVariable("FTM.flash-fault-detect-interrupt", 0xBA);
	simulator->SetVariable("FTM.flash-interrupt", 0xB8);
	simulator->SetVariable("FTM.verbose", false);
	simulator->SetVariable("FTM.dflash-start-address", 0x100000);
	simulator->SetVariable("FTM.dflash-end-address", 0x107FFF);
	simulator->SetVariable("FTM.eee-nonvolatile-information-register-start-address", 0x120000);
	simulator->SetVariable("FTM.eee-nonvolatile-information-register-size", 128);
	simulator->SetVariable("FTM.eee_tag-ram-start-address", 0x122000);
	simulator->SetVariable("FTM.EEE-tag-RAM-size", 256);
	simulator->SetVariable("FTM.eee-protectable-high-address", 0x13FFFF);
	simulator->SetVariable("FTM.memory-controller-scratch-ram-start-address", 0x124000);
	simulator->SetVariable("FTM.memory-controller-scratch-ram-size", 1024);
	simulator->SetVariable("FTM.buffer-ram-start-address", 0x13F000);
	simulator->SetVariable("FTM.buffer-ram-end-address", 0x13FFFF);
	simulator->SetVariable("FTM.pflash-start-address", 0x700000);
	simulator->SetVariable("FTM.pflash-end-address", 0x7FFFFF);
	simulator->SetVariable("FTM.pflash-protectable-high-address", 0x7FFFFF);
	simulator->SetVariable("FTM.pflash-protectable-low-address", 0x7F8000);
	simulator->SetVariable("FTM.pflash-blocks-description", "7C0000,7FFFFF;7A0000,7BFFFF;780000,79FFFF;740000,77FFFF;700000,73FFFF");
	simulator->SetVariable("FTM.pflash-protection-byte-address", 0x7FFF0C);
	simulator->SetVariable("FTM.eee-protection-byte-address", 0x7FFF0D);
	simulator->SetVariable("FTM.flash-nonvolatile-byte-address", 0x7FFF0E);
	simulator->SetVariable("FTM.flash-security-byte-address", 0x7FFF0F);
	simulator->SetVariable("FTM.dflash-partition-user-access-address", 0x120000);
	simulator->SetVariable("FTM.dflash-partition-user-access-address-duplicate", 0x120002);
	simulator->SetVariable("FTM.buffer-ram-partition-eee-operation-address", 0x120004);
	simulator->SetVariable("FTM.buffer-ram-partition-eee-operation-address-duplicate", 0x120006);
	simulator->SetVariable("FTM.max-number-sectors-dflash", 128);
	simulator->SetVariable("FTM.max-number-sectors-buffer-ram", 16);
	simulator->SetVariable("FTM.min-number-sectors-in-dflash-for-eee", 12);
	simulator->SetVariable("FTM.min-ratio-dflash-buffer-ram", 8);
	simulator->SetVariable("FTM.min-fclk-time", 1250000); // 0.8 MHz
	simulator->SetVariable("FTM.max-fclk-time", 952000);   // 1.05 MHz

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
	simulator->SetVariable("inline-debugger.search-path", "");

}

