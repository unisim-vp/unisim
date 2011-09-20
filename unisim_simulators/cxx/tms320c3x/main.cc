/*
 *  Copyright (c) 2009-2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr), Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <stdexcept>

#include "unisim/kernel/service/service.hh"
#include "unisim/component/cxx/processor/tms320/config.hh"
#include "unisim/component/cxx/processor/tms320/cpu.hh"
#include "unisim/component/cxx/memory/ram/memory.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/loader/coff_loader/coff_loader.hh"
#include "unisim/service/os/ti_c_io/ti_c_io.hh"

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef DEBUG_TMS320C3X
const bool CPU_DEBUG = true;
const bool MEMORY_DEBUG = true;
#else
const bool CPU_DEBUG = false;
const bool MEMORY_DEBUG = false;
#endif

// Front Side Bus template parameters
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;
typedef unisim::component::cxx::processor::tms320::TMS320VC33_Config CPU_CONFIG;


using namespace std;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;


class Simulator : public unisim::kernel::service::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual void Stop(Object *object, int _exit_status);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	typedef unisim::component::cxx::processor::tms320::CPU<CPU_CONFIG, CPU_DEBUG> CPU;
	typedef unisim::service::loader::coff_loader::CoffLoader<uint64_t> LOADER;
	typedef unisim::component::cxx::memory::ram::Memory<uint64_t> MEMORY;
	typedef unisim::service::debug::gdb_server::GDBServer<uint64_t> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<uint64_t> INLINE_DEBUGGER;
	typedef unisim::service::time::host_time::HostTime HOST_TIME;
	typedef unisim::service::os::ti_c_io::TI_C_IO<uint64_t> TI_C_IO;

	//=========================================================================
	//===                             Components                            ===
	//=========================================================================
	CPU *cpu;
	MEMORY *memory;

	//=========================================================================
	//===                              Services                             ===
	//=========================================================================
	HOST_TIME *host_time;
	LOADER *loader;
	LOADER *rom_loader;
	TI_C_IO *ti_c_io;
	GDB_SERVER *gdb_server;
	INLINE_DEBUGGER *inline_debugger;

	
	bool enable_gdb_server;
	bool enable_inline_debugger;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;

	int exit_status;
	bool simulating;
	static void LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator);
};

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::service::Simulator::simulator->Stop(0, 0);
}

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, host_time(0)
	, loader(0)
	, rom_loader(0)
	, ti_c_io(0)
	, gdb_server(0)
	, inline_debugger(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, exit_status(0)
	, simulating(false)
{
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	cpu = new CPU("cpu"); 
	memory = new MEMORY("memory");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Host Time
	host_time = new HOST_TIME("host-time");
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - COFF loaders
	loader = new LOADER("loader");
	rom_loader = new LOADER("rom-loader");
	//  - TI C I/O
	ti_c_io = new TI_C_IO("ti-c-io");

	VariableBase *program = FindVariable("cmd-args[0]");
	std::string program_name = std::string(*program);
	if(!program_name.empty())
	{
		(*loader)["filename"] = program_name.c_str();
	}

	//=========================================================================
	//===                       Components/Services connection              ===
	//=========================================================================

	// Connect the CPU to the memory
	cpu->memory_import >> memory->memory_export;
	cpu->ti_c_io_import >> ti_c_io->ti_c_io_export;
	ti_c_io->memory_import >> cpu->memory_export;
	ti_c_io->memory_injection_import >> cpu->memory_injection_export;
	ti_c_io->registers_import >> cpu->registers_export;
	ti_c_io->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	ti_c_io->blob_import >> loader->blob_export;

	if(enable_inline_debugger)
	{
		cpu->debug_control_import >> inline_debugger->debug_control_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		cpu->trap_reporting_import >> inline_debugger->trap_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
	}
	else if(enable_gdb_server)
	{
		// Connect gdb-server to CPU
		cpu->debug_control_import >> gdb_server->debug_control_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		cpu->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}

	// Connect everything
	loader->memory_import >> memory->memory_export;
	rom_loader->memory_import >> memory->memory_export;

	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

	if(enable_inline_debugger)
	{
		*inline_debugger->symbol_table_lookup_import[0] >> 
			loader->symbol_table_lookup_export;
	}
}

Simulator::~Simulator()
{
	if(rom_loader) delete rom_loader;
	if(loader) delete loader;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(host_time) delete host_time;
	if(ti_c_io) delete ti_c_io;
}
	
void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM tms320c3x");
	simulator->SetVariable("copyright", "Copyright (C) 2009-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM tms320c3x, a TMS320C3X DSP simulator with support of TI COFF binaries, and TI C I/O (RTS run-time)");
	
	// rom loader
	simulator->SetVariable("rom-loader.filename", "c31boot.out");
	simulator->SetVariable("cpu.mimic-dev-board", "true");
	simulator->SetVariable("ti-c-io.enable", "true");
	
	// inline-debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	void (*prev_sig_int_handler)(int) = 0;

	if(!enable_inline_debugger)
		prev_sig_int_handler = signal(SIGINT, SigIntHandler);

	try
	{
		simulating = true;
		do
		{
			cpu->StepInstruction();
		} while(simulating);
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	if(!enable_inline_debugger)
		signal(SIGINT, prev_sig_int_handler);

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

	VariableBase *stat_instruction_counter = FindVariable("cpu.instruction-counter");
	VariableBase *stat_insn_cache_hits = FindVariable("cpu.insn-cache-hits");
	VariableBase *stat_insn_cache_misses = FindVariable("cpu.insn-cache-misses");
	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "simulated instructions : " << (uint64_t)(*stat_instruction_counter) << " instructions" << endl;
	cerr << "host simulation speed: " << ((double)(*stat_instruction_counter) / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "Insn cache hits: " << (uint64_t) *stat_insn_cache_hits << endl;
	cerr << "Insn cache misses: " << (uint64_t) *stat_insn_cache_misses << endl;
	cerr << "Insn cache miss rate: " << ((double) *stat_insn_cache_misses / (double) ((uint64_t) *stat_insn_cache_hits + (uint64_t) *stat_insn_cache_misses)) << endl;
}

void Simulator::Stop(Object *object, int _exit_status)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	simulating = false;
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

int main(int argc, char *argv[])
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
