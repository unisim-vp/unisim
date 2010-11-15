/*
 *  Copyright (c) 2010,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_SERVICE_DEBUG_SIM_DEBUGGER_SIM_DEBUGGER_HH__
#define __UNISIM_SERVICE_DEBUG_SIM_DEBUGGER_SIM_DEBUGGER_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/debugger_handler/debugger_handler.hh>

#include <unisim/kernel/service/service.hh>

#include <inttypes.h>
#include <string>

namespace unisim {
namespace service {
namespace debug {
	class DebuggerHandler
	{
	public:
		DebuggerHandler()
			: handler_context(0)
			, breakpoint_handler_function(0)
			, watchpoint_handler_function(0)
		{};
		virtual ~DebuggerHandler() {};
		virtual bool SetStepMode() = 0;
		virtual bool SetContinueMode() = 0;
		virtual bool IsModeStep() = 0;
		virtual bool IsModeContinue() = 0;
		virtual bool HasBreakpoint(uint64_t addr) = 0;
		virtual bool HasBreakpoint(const char *str) = 0;
		virtual bool SetBreakpoint(uint64_t addr) = 0;
		virtual bool SetBreakpoint(const char *str) = 0;
		virtual bool DeleteBreakpoint(uint64_t addr) = 0;
		virtual bool DeleteBreakpoint(const char *str) = 0;
		virtual bool SetWatchpoint(uint64_t addr, uint32_t size) = 0;
		virtual bool SetReadWatchpoint(uint64_t addr, uint32_t size) = 0;
		virtual bool SetWriteWatchpoint(uint64_t addr, uint32_t size) = 0;
		virtual bool DeleteWatchpoint(uint64_t addr, uint32_t size) = 0;
		virtual bool DeleteReadWatchpoint(uint64_t addr, uint32_t size) = 0;
		virtual bool DeleteWriteWatchpoint(uint64_t addr, uint32_t size) = 0;
		virtual bool SetHandlerContext(void *context)
		{
			handler_context = context;
			return true;
		};
		virtual bool SetBreakpointHandler(
				void (*function)(void *, uint64_t))
		{
			breakpoint_handler_function = function;
			return true;
		};

		virtual bool SetWatchpointHandler(
				void (*function)(void *, uint64_t, bool))
		{
			watchpoint_handler_function = function;
			return true;
		};

	protected:
		void CallBreakpointHandler(uint64_t addr)
		{
			if ( breakpoint_handler_function )
				breakpoint_handler_function(handler_context, addr);
		};

		void CallWatchpointHandler(uint64_t addr, bool read)
		{
			if ( watchpoint_handler_function )
				watchpoint_handler_function(handler_context, addr, read);
		};

	private:
		void *handler_context;
		void (*breakpoint_handler_function)(void *, uint64_t);
		void (*watchpoint_handler_function)(void *, uint64_t, bool);

	};
}
}
}

namespace unisim {
namespace service {
namespace debug {
namespace sim_debugger {

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::StatementLookup;

using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::WatchpointRegistry;

using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;

using std::string;

typedef enum
{
	SIM_DEBUGGER_MODE_STEP,
	SIM_DEBUGGER_MODE_CONTINUE
} SimDebuggerRunningMode;

class SimDebuggerBase
{
public:
	SimDebuggerBase();
	virtual ~SimDebuggerBase();
protected:
	static bool trap;
private:
	static void (*prev_sig_int_handler)(int);
	static int alive_instances;
	static void SigIntHandler(int signum);
};

template <class ADDRESS>
class SimDebugger
	: public Service<DebugControl<ADDRESS> >
	, public Service<MemoryAccessReporting<ADDRESS> >
	, public Service<TrapReporting>
	, public Client<MemoryAccessReportingControl>
	, public Client<Disassembly<ADDRESS> >
	, public Client<Memory<ADDRESS> >
	, public Client<Registers>
	, public Client<SymbolTableLookup<ADDRESS> >
	, public Client<Loader<ADDRESS> >
	, public Client<StatementLookup<ADDRESS> >
	, public SimDebuggerBase
	, public unisim::util::debug::debugger_handler::DebuggerHandler
{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceExport<TrapReporting> trap_reporting_export;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > **symbol_table_lookup_import;
	ServiceImport<Loader<ADDRESS> > **loader_import;
	ServiceImport<StatementLookup<ADDRESS> > **stmt_lookup_import;

	SimDebugger(const char *name, Object *parent = 0);
	virtual ~SimDebugger();

	// MemoryAccessReportingInterface
	virtual void ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportFinishedInstruction(ADDRESS next_addr);
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);

	// DebugControlInterface
	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);

	virtual bool Setup();
	virtual void OnDisconnect();

private:
	unsigned int memory_atom_size;
	unsigned int num_loaders;
	std::string search_path;
	Parameter<unsigned int> param_memory_atom_size;
	Parameter<unsigned int> param_num_loaders;
	Parameter<std::string> param_search_path;

	BreakpointRegistry<ADDRESS> breakpoint_registry;
	WatchpointRegistry<ADDRESS> watchpoint_registry;
	unisim::util::debug::Profile<ADDRESS> program_profile;
	unisim::util::debug::Profile<ADDRESS> data_read_profile;
	unisim::util::debug::Profile<ADDRESS> data_write_profile;
	SimDebuggerRunningMode running_mode;

	ADDRESS disasm_addr;
	ADDRESS dump_addr;

private:
	virtual bool SetStepMode();
	virtual bool SetContinueMode();
	virtual bool IsModeStep();
	virtual bool IsModeContinue();

//	string *Disasm();
//	string *Disasm(uint64_t addr);
	void Disasm(uint64_t addr, int count);

	bool HasBreakpoint(uint64_t addr);
	bool HasBreakpoint(const char *str);
	bool SetBreakpoint(uint64_t addr);
	bool SetBreakpoint(const char *str);
	bool DeleteBreakpoint(uint64_t addr);
	bool DeleteBreakpoint(const char *str);
	bool SetWatchpoint(uint64_t addr, uint32_t size);
	bool SetReadWatchpoint(uint64_t addr, uint32_t size);
	bool SetWriteWatchpoint(uint64_t addr, uint32_t size);
	bool DeleteWatchpoint(uint64_t addr, uint32_t size);
	bool DeleteReadWatchpoint(uint64_t addr, uint32_t size);
	bool DeleteWriteWatchpoint(uint64_t addr, uint32_t size);

	bool GetSymbolAddress(const char *str, uint64_t &addr);
	bool GetFileSystemAddress(const char *str, uint64_t &addr);

	void DumpBreakpoints();
	void DumpWatchpoints();
	void DumpMemory(uint64_t addr);
	void DumpProgramProfile();
	void DumpDataProfile(bool write);
	void DumpAvailableLoaders();
	void Load(const char *loader_name, const char *filename);
	void DumpSource(const char *filename, unsigned int lineno, unsigned int colno, unsigned int count);
};

} // end of namespace sim_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_SIM_DEBUGGER_SIM_DEBUGGER_HH__
