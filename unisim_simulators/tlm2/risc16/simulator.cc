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
//	sc_stop();
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
//	, cpu(0)
	, global_ram(0)
//	, memoryImportExportTee(0)
//	, mem_Fault_injector(0)

	, device(0)
	, bus_fault_injector(0)

	//, bus(0)
	, endian("")
	, program_counter_name(std::string("PC"))
	, filename(std::string("binary_file_name"))

	, exit_status(0)

{

	param_endian = new Parameter<string>("endian", 0, endian, "Target endianness");
	param_endian->SetMutable(false);
	param_endian->SetVisible(true);

	param_pc_reg_name = new Parameter<string>("program-counter-name", 0, program_counter_name, "Target CPU program counter name");
	param_pc_reg_name->SetMutable(false);
	param_pc_reg_name->SetVisible(true);

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
	//  - RISC16 processor
		top = new Top("Top");
//
	//  - Memories
	global_ram = new RAM("RAM");

	//    CAN
	device = new Device("Device");

	// Router
//	bus = new Bus("Bus");

//
//	// - Fault_Injector
//	mem_Fault_injector = new MFI("Memory_Fault_injector");

	// - bus fault injector

	bus_fault_injector = new BFI("BFI");
//
//
//	memoryImportExportTee = new MemoryImportExportTee("memoryImportExportTee");

//	top = new Top("top");

	tee_memory_access_reporting = new MemoryAccessReportingTee("tee-memory-access-reporting");

	profiler = new Profiler<CPU_ADDRESS_TYPE>("profiler");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	debugger = new Debugger<CPU_ADDRESS_TYPE>("debugger");
	inline_debugger = new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger");

	raw_loader = new RawLoader("raw-loader");

	//=========================================================================
	//===                       Service parameterization     ===
	//=========================================================================


	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

//	cpu->initsocket(global_ram->slave_sock);

	top->top_socket[0](global_ram->slave_sock);

	top->top_socket[1](device->device_rout_socket);

	device->IRQ_socket(top->top_target_socket);



	// This order is mandatory (see the memoryMapping)

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	top->router->router_import >> bus_fault_injector->bus_fault_export;

	top->initiator->memory_import >> global_ram->memory_export;


// ********   added by Reda ***************************
	raw_loader->memory_import >> global_ram->memory_export;
	debugger->blob_import >> raw_loader->blob_export;

	// Connect tee-memory-access-reporting to CPU, debugger and profiler
	top->initiator->memory_access_reporting_import >> tee_memory_access_reporting->in;
	*tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
	*tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
	profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];
	debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1];
	tee_memory_access_reporting->out_control >> top->initiator->memory_access_reporting_control_export;

	// Connect debugger to CPU
	top->initiator->debug_control_import >> debugger->debug_control_export;
	top->initiator->trap_reporting_import >> debugger->trap_reporting_export;
	debugger->disasm_import >> top->initiator->disasm_export;
	debugger->memory_import >> top->initiator->memory_export;

	debugger->registers_import >> top->initiator->registers_export;

	// Connect inline-debugger to debugger
	debugger->debug_control_import >> inline_debugger->debug_control_export;
	inline_debugger->debug_event_trigger_import >> debugger->debug_event_trigger_export;
	debugger->debug_event_listener_import >> inline_debugger->debug_event_listener_export;
	debugger->trap_reporting_import >> inline_debugger->trap_reporting_export;
	inline_debugger->disasm_import >> debugger->disasm_export;
	inline_debugger->memory_import >> debugger->memory_export;
	inline_debugger->registers_import >> debugger->registers_export;
	inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;
	inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;
	inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;

	inline_debugger->backtrace_import >> debugger->backtrace_export;
	inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;

	inline_debugger->profiling_import >> profiler->profiling_export;


// ********* end in progress ************************


//	cpu->memory_import >> mmc->memory_export;

//	*(memoryImportExportTee->memory_import[8]) >> global_ram->memory_export;

//	mmc->memory_import >> memoryImportExportTee->memory_export;

	// ce qui je dois avoir
	// une classe géneral fault_injector qui dérive du serviceinterface
	//une classe qui dérive de Fault injector qui sapel Memory_Fault_injector
	//d'une tel façon quel contienne un service export



	///


// ***********************************************************

	// Connect debugger to CPU


}

Simulator::~Simulator()
{

//	if (mem_Fault_injector) { delete mem_Fault_injector; mem_Fault_injector = NULL; }

	if (bus_fault_injector){ delete bus_fault_injector; bus_fault_injector = NULL; }
//	if (memoryImportExportTee) { delete memoryImportExportTee; memoryImportExportTee = NULL; }


	if(global_ram) { delete global_ram; global_ram = NULL; }

//	if(cpu) { delete cpu; cpu = NULL; }

	if(top) { delete top; top = NULL; }

	if (device) { delete device; device = NULL; }

//	if (bus) { delete bus; bus = NULL; }

	if (inline_debugger) { delete inline_debugger; inline_debugger = NULL; }
	if (raw_loader) { delete raw_loader; raw_loader = NULL; }

}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM risc16");
	simulator->SetVariable("copyright", "Copyright (C) 2008-2014, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Reda Nouacer <reda.nouacer@cea.fr>");

#ifdef HAVE_CONFIG_H
	simulator->SetVariable("version", VERSION);
#endif

	simulator->SetVariable("description", "UNISIM risc16, a simple risc16 simulator");

	simulator->SetVariable("enable-pim-server", false);
	simulator->SetVariable("enable-gdb-server", false);
	simulator->SetVariable("enable-inline-debugger", false);
	simulator->SetVariable("dump-parameters", false);
	simulator->SetVariable("dump-formulas", false);
	simulator->SetVariable("dump-statistics", true);

	simulator->SetVariable("endian", "big");
	simulator->SetVariable("program-counter-name", "CPU.PC");

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	simulator->SetVariable("Top.initiator.requires-memory-access-reporting", true);
	simulator->SetVariable("Top.initiator.requires-finished-instruction-reporting", true);
	simulator->SetVariable("Top.initiator.load-address", 0x800);
	simulator->SetVariable("Top.initiator.entry-point", 0x804);

//	simulator->SetVariable("RAM.org", 0x000800);
	simulator->SetVariable("RAM.org", 0x000800);
	simulator->SetVariable("RAM.bytesize", 0xFFFF + 1 - 0x800); // 64k - 0x800
	simulator->SetVariable("RAM.initial-byte-value", 0x00);
	simulator->SetVariable("RAM.cycle-time", 250000);
	simulator->SetVariable("RAM.verbose", false);


	simulator->SetVariable("Device.vect", 0xFF20); //Interupt Vecotr

	simulator->SetVariable("Top.router.low-address[0]", 0x000800);  //RAM
	simulator->SetVariable("Top.router.high-address[0]", 0xFFFF); //
	simulator->SetVariable("Top.router.low-address[1]", 0x0400);  //Device
	simulator->SetVariable("Top.router.high-address[1]", 0x040F); //

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	simulator->SetVariable("raw-loader.filename", "tt.bin");
	simulator->SetVariable("raw-loader.base-addr", 0x800);
	simulator->SetVariable("raw-loader.size", 0);
	/*
	 * Size: Number of bytes to copy.
		If 0 then all the file contents will be copied."
		If smaller than the file, the contents of the file will be cropped.
		If bigger than the file size, all the file will be copied.
	 */
	simulator->SetVariable("raw-loader.verbose", false);

	simulator->SetVariable("kernel_logger.std_err", true);
	simulator->SetVariable("kernel_logger.std_out", false);
	simulator->SetVariable("kernel_logger.std_err_color", false);
	simulator->SetVariable("kernel_logger.std_out_color", false);
	simulator->SetVariable("kernel_logger.file", false);
	simulator->SetVariable("kernel_logger.filename", "logger_output.txt");
	simulator->SetVariable("kernel_logger.xml_file", true);
	simulator->SetVariable("kernel_logger.xml_filename", "logger_output.xml");
	simulator->SetVariable("kernel_logger.xml_file_gzipped", false);

}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
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

	// TODO: use entry_point to initialize the program counter correctly
//	physical_address_t entry_point = 0x00;

//	cpu->setEntryPoint(cpu_address);

	cerr << "Starting simulation ..." << endl;

	EnableDebug();

	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	cerr << "Simulation finished" << endl << endl;

}

