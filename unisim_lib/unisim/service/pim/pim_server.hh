/*
 *  Copyright (c) 2007, 2010
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_SERVICE_PIM_PIM_SERVER_HH__
#define __UNISIM_SERVICE_PIM_PIM_SERVER_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/time.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>

#include <unisim/service/pim/network/GenericThread.hpp>
#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/network/SocketServerThread.hpp>
#include <unisim/service/pim/pim_thread.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/register.hh>

#include <string>
#include <vector>
#include <map>

#include <inttypes.h>

#include <unisim/util/converter/convert.hh>

namespace unisim {
namespace service {
namespace pim {

using std::string;
using std::vector;

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::DebugEventTrigger;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Time;
using unisim::service::interfaces::StatementLookup;

using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::WatchpointRegistry;
using unisim::util::debug::Watchpoint;
using unisim::util::debug::Symbol;

using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;
using unisim::kernel::service::Variable;

using unisim::service::pim::network::SocketThread;
using unisim::service::pim::network::GenericThread;
using unisim::service::pim::network::SocketServerThread;

using unisim::service::pim::PIMThread;

typedef enum { GDBSERVER_MODE_WAITING_GDB_CLIENT, GDBSERVER_MODE_STEP, GDBSERVER_MODE_CONTINUE } GDBServerRunningMode;

typedef enum { GDB_LITTLE_ENDIAN, GDB_BIG_ENDIAN } GDBEndian;

class Request {
public:

	enum TargetCOMMANDS {CONTINUE, SUSPEND, DISCONNECT, READ_REGISTERS, WRITE_REGISTER, SET_THREAD_CONTEXT, STEP_CYCLE, KILL_COMMAND, READ_MEMORY, WRITE_MEMORY, READ_SELECTED_REGISTER, WRITE_SELECTED_REGISTER, QUERY_VARIABLE, STEP_INSTRUCTION, REMOVE_BREAKPOINT, SET_BREAKPOINT, GET_LAST_SIGNAL};

	Request() {}
	~Request() {}

	bool addAttribute(std::string name, std::string value) {

		if (attributes.find(name) == attributes.end()) {
			return (false);
		} else {
			attributes.insert ( std::pair<std::string, std::string>(name, value) );
		}

		return (true);
	}

	bool removeAttribute(std::string name) {

		if (attributes.find(name) == attributes.end()) {
			return (false);
		} else {
			attributes.erase (name);
		}

		return (true);
	}

	std::string getAttribute(std::string name) {

		std::map<std::string, std::string>::iterator it = attributes.find(name);
		if (it == attributes.end()) {
			return (NULL);
		}

		return (it->second);
	}

	bool setAttribute(std::string name, std::string value) {

		std::map<std::string, std::string>::iterator it = attributes.find(name);
		if (it == attributes.end()) {
			return (false);
		} else {
			it->second = value;
		}

		return  (true);
	}

protected:
private:
	std::map<std::string, std::string> attributes;
};

template <class ADDRESS>
class PIMServer :
	public Service<DebugControl<ADDRESS> >,
	public Service<DebugEventListener<ADDRESS> >,
	public Service<TrapReporting>,
	public Client<DebugEventTrigger<ADDRESS> >,
	public Client<Memory<ADDRESS> >,
	public Client<Disassembly<ADDRESS> >,
	public Client<SymbolTableLookup<ADDRESS> >,
	public Client<StatementLookup<ADDRESS> >,
	public Client<Registers>

{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceExport<TrapReporting> trap_reporting_export;

	ServiceExport<DebugEventListener<ADDRESS> > debug_event_listener_export;
	ServiceImport<DebugEventTrigger<ADDRESS> > debug_event_trigger_import;

	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;
	ServiceImport<StatementLookup<ADDRESS> > stmt_lookup_import;

	PIMServer(const char *name, Object *parent = 0);
	virtual ~PIMServer();

	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);

	// DebugEventListener
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event);

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void Stop(int exit_status);

	virtual void run() { cerr << "PIM-Server:: start RUN " << std::endl; }

	uint16_t GetTCPPort() { return (tcp_port);}
	string GetHost() { return (fHost); }

	double GetSimTime();
	double GetHostTime();

	inline GDBEndian GetEndian() const { return (endian); }

protected:
	vector<SocketThread*> protocolHandlers;

	SocketServerThread *socketServer;

	SocketThread *debuggerThread;
	SocketThread *monitorThread;
	SocketThread *pimServerThread;

private:
	static const unsigned int MAX_BUFFER_SIZE = 256;
	bool ParseHex(const string& s, unsigned int& pos, ADDRESS& value);

	bool InternalReadMemory(ADDRESS addr, uint32_t size, string& packet);

	bool ReadRegisters();
	bool WriteRegisters(const string& hex);
	bool ReadRegister(unsigned int regnum);
	bool WriteRegister(unsigned int regnum, const string& hex);
	bool ReadMemory(ADDRESS addr, uint32_t size);
	bool WriteMemory(ADDRESS addr, const string& hex, uint32_t size);
	bool ReportProgramExit();
	bool ReportSignal(unsigned int signum);
	bool ReportTracePointTrap();
	bool SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size);
	bool RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size);

	void HandleQRcmd(string command);
	bool HandleSymbolLookup();
	bool ReadSymbol(const string name);
	bool WriteSymbol(const string name, const string& hex);

	void Disasm(ADDRESS addr, unsigned int size);
	
	void Kill();

	unisim::kernel::logger::Logger logger;

	uint16_t tcp_port;
	string architecture_description_filename;

	vector<VariableBase*> simulator_registers;
	VariableBase* pc_reg;
	uint32_t pc_reg_index;
	GDBEndian endian;

	bool killed;
	bool trap;
	bool synched;
	const Watchpoint<ADDRESS> *watchpoint_hit;

	BreakpointRegistry<ADDRESS> breakpoint_registry;
	WatchpointRegistry<ADDRESS> watchpoint_registry;
	GDBServerRunningMode running_mode;
	bool extended_mode;
	int32_t counter;
	int32_t period;

	ADDRESS disasm_addr;

	unsigned int memory_atom_size;
	bool verbose;

	string fHost;

	Parameter<unsigned int> param_memory_atom_size;
	Parameter<uint16_t> param_tcp_port;
	Parameter<string> param_architecture_description_filename;
	Parameter<bool> param_verbose;
	Parameter<string> param_host;


};

} // end of namespace pim
} // end of namespace service
} // end of namespace unisim

#endif
