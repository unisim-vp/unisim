/*
 *  Copyright (c) 2009-2013,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr), Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "unisim/component/cxx/processor/tms320c3x/config.hh"
#include "unisim/component/tlm2/processor/tms320c3x/cpu.hh"
#include <unisim/component/tlm2/memory/ram/memory.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/debug/debug.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include "unisim/service/tee/memory_access_reporting/tee.hh"
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/os/ti_c_io/ti_c_io.hh>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
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
	virtual unisim::kernel::service::Simulator::SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

#ifdef DEBUG_TMS320C3X
	static const bool CPU_DEBUG = true;
	static const bool MEMORY_DEBUG = true;
#else
	static const bool CPU_DEBUG = false;
	static const bool MEMORY_DEBUG = false;
#endif

	typedef unisim::component::cxx::processor::tms320c3x::TMS320VC33_Config CPU_CONFIG;
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::processor::tms320c3x::CPU<CPU_CONFIG, CPU_DEBUG> CPU;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_CONFIG::address_t> LOADER;
	typedef unisim::component::tlm2::memory::ram::Memory<32, CPU_CONFIG::address_t, 8, 1024 * 1024, MEMORY_DEBUG> MEMORY;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_CONFIG::address_t> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_CONFIG::address_t> INLINE_DEBUGGER;
	typedef unisim::service::debug::debugger::Debugger<CPU_CONFIG::address_t> DEBUGGER;
	typedef unisim::service::profiling::addr_profiler::Profiler<CPU_CONFIG::address_t> PROFILER;
	typedef unisim::service::time::sc_time::ScTime SC_TIME;
	typedef unisim::service::time::host_time::HostTime HOST_TIME;
	typedef unisim::service::os::ti_c_io::TI_C_IO<CPU_CONFIG::address_t> TI_C_IO;
	typedef unisim::service::tee::memory_access_reporting::Tee<CPU_CONFIG::address_t> TEE_MEMORY_ACCESS_REPORTING;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - TMS320C3X processor
	CPU *cpu;
	//  - RAM
	MEMORY *memory;
	//  - IRQ Stubs
	IRQ_STUB *int0_stub;
	IRQ_STUB *int1_stub;
	IRQ_STUB *int2_stub;
	IRQ_STUB *int3_stub;
	IRQ_STUB *xint0_stub;
	IRQ_STUB *rint0_stub;
	IRQ_STUB *xint1_stub;
	IRQ_STUB *rint1_stub;
	IRQ_STUB *tint0_stub;
	IRQ_STUB *tint1_stub;
	IRQ_STUB *dint_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	// - Multiformat Loader
	LOADER *loader;
	// - TI C I/O
	TI_C_IO *ti_c_io;
	//  - GDB server
	GDB_SERVER *gdb_server;
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger;
	//  - debugger
	DEBUGGER *debugger;
	//  - profiler
	PROFILER *profiler;
	//  - SystemC Time
	SC_TIME *sim_time;
	//  - Host Time
	HOST_TIME *host_time;
	//  - Tee Memory Access Reporting
	TEE_MEMORY_ACCESS_REPORTING *tee_memory_access_reporting;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};



Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, int0_stub(0)
	, int1_stub(0)
	, int2_stub(0)
	, int3_stub(0)
	, xint0_stub(0)
	, rint0_stub(0)
	, xint1_stub(0)
	, rint1_stub(0)
	, tint0_stub(0)
	, tint1_stub(0)
	, dint_stub(0)
	, loader(0)
	, ti_c_io(0)
	, gdb_server(0)
	, inline_debugger(0)
	, debugger(0)
	, profiler(0)
	, sim_time(0)
	, host_time(0)
	, tee_memory_access_reporting(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, exit_status(0)
{
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - RAM
	memory = new MEMORY("memory");
	//  - IRQ Stubs
	int0_stub = new IRQ_STUB("int0-stub");
	int1_stub = new IRQ_STUB("int1-stub");
	int2_stub = new IRQ_STUB("int2-stub");
	int3_stub = new IRQ_STUB("int3-stub");
	xint0_stub = new IRQ_STUB("xint0-stub");
	rint0_stub = new IRQ_STUB("rint0-stub");
	xint1_stub = new IRQ_STUB("xint1-stub");
	rint1_stub = new IRQ_STUB("rint1-stub");
	tint0_stub = new IRQ_STUB("tint0-stub");
	tint1_stub = new IRQ_STUB("tint1-stub");
	dint_stub = new IRQ_STUB("dint-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	VariableBase *program = FindVariable("cmd-args[0]");
	std::string program_name = std::string(*program);
	if(!program_name.empty())
	{
		SetVariable("loader.filename", (program_name + ",c31boot.out").c_str());
	}

	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - Debugger
	debugger = (enable_gdb_server || enable_inline_debugger) ? new DEBUGGER("debugger") : 0;
	//  - Profiler
	profiler = enable_inline_debugger ? new PROFILER("profiler") : 0;
	//  - Multiformat Loader
	loader = new LOADER("loader");
	//  - TI C I/O
	ti_c_io = new TI_C_IO("ti-c-io");
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Tee Memory Access Reporting
	tee_memory_access_reporting = 
		(enable_gdb_server || enable_inline_debugger) ?
			new TEE_MEMORY_ACCESS_REPORTING("tee-memory-access-reporting") :
			0;

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->bus_master_sock(memory->slave_sock); // CPU <-> RAM
	int0_stub->master_sock(cpu->int0_slave_sock); // INT0 STUB <-> CPU
	int1_stub->master_sock(cpu->int1_slave_sock); // INT1 STUB <-> CPU
	int2_stub->master_sock(cpu->int2_slave_sock); // INT2 STUB <-> CPU
	int3_stub->master_sock(cpu->int3_slave_sock); // INT3 STUB <-> CPU
	xint0_stub->master_sock(cpu->xint0_slave_sock); // XINT0 STUB <-> CPU
	rint0_stub->master_sock(cpu->rint0_slave_sock); // RINT0 STUB <-> CPU
	xint1_stub->master_sock(cpu->xint1_slave_sock); // XINT1 STUB <-> CPU
	rint1_stub->master_sock(cpu->rint1_slave_sock); // RINT1 STUB <-> CPU
	tint0_stub->master_sock(cpu->tint0_slave_sock); // TINT0 STUB <-> CPU
	tint1_stub->master_sock(cpu->tint1_slave_sock); // TINT1 STUB <-> CPU
	dint_stub->master_sock(cpu->dint_slave_sock); // DINT STUB <-> CPU

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory->memory_export;
	cpu->ti_c_io_import >> ti_c_io->ti_c_io_export;
	ti_c_io->memory_import >> cpu->memory_export;
	ti_c_io->memory_injection_import >> cpu->memory_injection_export;
	ti_c_io->registers_import >> cpu->registers_export;
	ti_c_io->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	ti_c_io->blob_import >> loader->blob_export;
	*loader->memory_import[0] >> memory->memory_export;
	
	if(enable_inline_debugger || enable_gdb_server)
	{
		// Connect debugger to CPU
		cpu->debug_control_import >> debugger->debug_control_export;
		cpu->trap_reporting_import >> debugger->trap_reporting_export;
		cpu->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
		debugger->disasm_import >> cpu->disasm_export;
		debugger->memory_import >> cpu->memory_export;
		debugger->registers_import >> cpu->registers_export;
		debugger->blob_import >> loader->blob_export;
		
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
			cpu->memory_access_reporting_import >> debugger->memory_access_reporting_export;
			debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
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
			inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;
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
		}
	}
}

Simulator::~Simulator()
{
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(debugger) delete debugger;
	if(loader) delete loader;
	if(ti_c_io) delete ti_c_io;
	if(profiler) delete profiler;
	if(cpu) delete cpu;
	if(int0_stub) delete int0_stub;
	if(int1_stub) delete int1_stub;
	if(int2_stub) delete int2_stub;
	if(int3_stub) delete int3_stub;
	if(xint0_stub) delete xint0_stub;
	if(rint0_stub) delete rint0_stub;
	if(xint1_stub) delete xint1_stub;
	if(rint1_stub) delete rint1_stub;
	if(tint0_stub) delete tint0_stub;
	if(tint1_stub) delete tint1_stub;
	if(dint_stub) delete dint_stub;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	double cpu_frequency = 75.0; // in Mhz
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double mem_cycle_time = cpu_cycle_time;

	// meta information
	simulator->SetVariable("program-name", "UNISIM tms320c3x");
	simulator->SetVariable("copyright", "Copyright (C) 2009-2013, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM tms320c3x, a TMS320C3X DSP simulator with support of TI COFF binaries, and TI C I/O (RTS run-time)");
	
	// - Loader
	simulator->SetVariable("loader.filename", "c31boot.out");
	
	// - CPU
	simulator->SetVariable("cpu.mimic-dev-board", "true");
	simulator->SetVariable("cpu.ipc", cpu_ipc);
	simulator->SetVariable("cpu.cpu-cycle-time", sc_time(cpu_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.nice-time", "1 us");
	
	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", (uint32_t) -1);

	// - Loader memory router
	simulator->SetVariable("loader.memory-mapper.mapping", "memory=memory:0x0-0xffffffff");

	// - TI C I/O
	simulator->SetVariable("ti-c-io.enable", "true");
	simulator->SetVariable("ti-c-io.pc-register-name", "PC");
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);

	//  - Inline debugger
	simulator->SetVariable("inline-debugger.memory-atom-size", 4);
	
	//  - GDB server
	simulator->SetVariable("gdb-server.architecture-description-filename", "gdb_tms320c3x.xml");
	simulator->SetVariable("gdb-server.memory-atom-size", 4);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	void (*prev_sig_int_handler)(int) = 0;

	if(!inline_debugger)
	{
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);
	}

	sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages
	
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
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	if(enable_gdb_server)
	{
		SetVariable("cpu.trap-on-trap-instruction", 0x7400003f); // TRAP 0x1f
	}
	
	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();

	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " shall not be used together. Use " << param_enable_inline_debugger.GetName() << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
		return unisim::kernel::service::Simulator::ST_ERROR;
	}
	
	return setup_status;
}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_TMS320C3X
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
#endif
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

int Simulator::GetExitStatus() const
{
	return exit_status;
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
			cerr << "Starting simulation" << endl;
			simulator->Run();
			break;
		case unisim::kernel::service::Simulator::ST_ERROR:
			cerr << "Can't start simulation because of previous errors" << endl;
			break;
	}

	int exit_status = simulator->GetExitStatus();
	if(simulator) delete simulator;
#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return exit_status;
}
