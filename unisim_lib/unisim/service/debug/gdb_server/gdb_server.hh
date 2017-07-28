/*
 *  Copyright (c) 2007,
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
 */

#ifndef __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_HH__
#define __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/xml/xml.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/util/debug/event.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>

#include <string>
#include <vector>

#include <inttypes.h>

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

using std::string;
using std::vector;

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::DebugEventTrigger;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::TrapReporting;

using unisim::util::debug::Event;
using unisim::util::debug::Breakpoint;
using unisim::util::debug::Watchpoint;
using unisim::util::debug::Symbol;

using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;


typedef enum { GDBSERVER_MODE_WAITING_GDB_CLIENT, GDBSERVER_MODE_STEP, GDBSERVER_MODE_CONTINUE } GDBServerRunningMode;

typedef enum { GDB_LITTLE_ENDIAN, GDB_BIG_ENDIAN } GDBEndian;

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

inline uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

inline bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

class GDBRegister
{
public:
	GDBRegister();
	GDBRegister(const string& reg_name, int reg_bitsize, GDBEndian endian, unsigned int reg_num, const std::string& type, const std::string& group);
	GDBRegister(unisim::service::interfaces::Register *reg, const string& reg_name, GDBEndian endian, unsigned int reg_num, const std::string& type, const std::string& group);
	inline const char *GetName() const { return name.c_str(); }
	inline int GetBitSize() const { return bitsize; }
	inline int GetByteSize() const { return bitsize / 8; }
	bool SetValue(const string& hex);
	bool SetValue(const void *buffer);
	bool GetValue(string& hex) const;
	bool GetValue(void *buffer) const;
	inline int GetHexLength() const { return bitsize / 4; }
	inline unisim::service::interfaces::Register *GetRegisterInterface() { return reg; }
	inline void SetRegisterInterface(unisim::service::interfaces::Register *reg) { this->reg = reg; }
	inline GDBEndian GetEndian() const { return endian; }
	unsigned int GetRegNum() const { return reg_num; }
	bool IsEmpty() const { return (bitsize == 0) and (not reg); }
	const char *GetType() const { return type.c_str(); }
	const char *GetGroup() const { return group.c_str(); }
	std::ostream& ToXML(std::ostream& os, unsigned int reg_num) const;
private:
	string name;
	int bitsize;
	unisim::service::interfaces::Register *reg;
	GDBEndian endian;
	unsigned int reg_num;
	std::string type;
	std::string group;
};

class GDBFeature
{
public:
	GDBFeature(const std::string& feature_name, unsigned int id);
	void AddRegister(const GDBRegister *gdb_register);
	unsigned int GetId() const;
	std::ostream& ToXML(std::ostream& os, std::string req_filename = std::string()) const;
private:
	std::string name;
	unsigned int id;
	std::vector<const GDBRegister *> gdb_registers;
};

template <class ADDRESS>
class GDBServer :
	public Service<DebugControl<ADDRESS> >,
	public Service<DebugEventListener<ADDRESS> >,
	public Service<TrapReporting>,
	public Client<DebugEventTrigger<ADDRESS> >,
	public Client<Memory<ADDRESS> >,
	public Client<Disassembly<ADDRESS> >,
	public Client<SymbolTableLookup<ADDRESS> >,
	public Client<Registers>
{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<DebugEventListener<ADDRESS> > debug_event_listener_export;
	ServiceExport<TrapReporting> trap_reporting_export;

	ServiceImport<DebugEventTrigger<ADDRESS> > debug_event_trigger_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;

	GDBServer(const char *name, Object *parent = 0);
	virtual ~GDBServer();

	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);
	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);
	virtual bool EndSetup();
	virtual void OnDisconnect();

private:
	static const unsigned int MAX_BUFFER_SIZE = 256;
	bool VisitArchitecture(unisim::util::xml::Node *xml_node);
	bool VisitBreakPointKind(unisim::util::xml::Node *xml_node);
	bool VisitFeature(unisim::util::xml::Node *xml_node);
	bool VisitRegister(unisim::util::xml::Node *xml_node, GDBFeature *gdb_feature);
	bool VisitProgramCounter(unisim::util::xml::Node *xml_node);
	
	bool ParseHex(const string& s, size_t& pos, ADDRESS& value);
	bool GetChar(char& c, bool blocking);
	bool PutChar(char c);
	bool GetPacket(string& s, bool blocking);
	bool PutPacket(const string& s);
	bool FlushOutput();
	bool OutputText(const char *s, int count);
	bool ReadRegisters();
	bool WriteRegisters(const string& hex);
	bool ReadRegister(unsigned int regnum);
	bool WriteRegister(unsigned int regnum, const string& hex);
	bool ReadMemoryHex(ADDRESS addr, uint32_t size);
	bool WriteMemoryHex(ADDRESS addr, const string& hex, uint32_t size);
	bool ReadMemoryBin(ADDRESS addr, uint32_t size);
	bool WriteMemoryBin(ADDRESS addr, const string& bin, uint32_t size);
	bool ReportProgramExit();
	bool ReportSignal(unsigned int signum);
	bool ReportTracePointTrap();
	bool SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t kind);
	bool RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t kind);

	void HandleQRcmd(string command);
	void HandleQSupported(std::string features);
	void SetGDBClientFeature(std::string gdb_client_feature);
	void HandleQC();
	void HandleQAttached(std::string command);
	void HandleQTStatus();
	void HandleQStartNoAckMode();
	void HandleQXferFeaturesRead(std::string command);
	void HandleQfThreadInfo();
	void HandleQsThreadInfo();
	void HandleQRegisterInfo(std::string hex_reg_order_num); // LLDB specific

	void Disasm(ADDRESS addr, unsigned int size);
	
	void Kill();
	
	std::string EscapeString(const std::string& s) const;

	void DisplayMonitoredInternals();

	unisim::kernel::logger::Logger logger;

	int tcp_port;
	string architecture_description_filename;
	GDBEndian endian;
	unsigned int gdb_arch_reg_num;
	std::string program_counter_name;

	int sock;
	vector<GDBFeature *> gdb_features;
	vector<GDBRegister *> gdb_registers;
	GDBRegister *gdb_pc;
	bool killed;
	bool trap;
	bool synched;
	GDBServerRunningMode running_mode;
	bool extended_mode;
	int32_t counter;
	int32_t period;
	bool gdb_client_feature_multiprocess;
	bool gdb_client_feature_xmlregisters;
	bool gdb_client_feature_qrelocinsn;
	bool gdb_client_feature_swbreak;
	bool gdb_client_feature_hwbreak;
	bool gdb_client_feature_fork_events;
	bool gdb_client_feature_vfork_events;
	bool gdb_client_feature_exec_events;
	bool gdb_client_feature_vcont;
	bool gdb_client_feature_t32extensions;
	unsigned int current_thread_id;
	bool no_ack_mode;
	std::map<uint32_t, uint32_t> arch_specific_breakpoint_kinds;
	

	ADDRESS disasm_addr;

	unsigned int input_buffer_size;
	unsigned int input_buffer_index;
	char input_buffer[MAX_BUFFER_SIZE];

	unsigned int output_buffer_size;
	char output_buffer[MAX_BUFFER_SIZE];

	unsigned int memory_atom_size;
	bool verbose;
	std::string monitor_internals;

	Parameter<unsigned int> param_memory_atom_size;
	Parameter<int> param_tcp_port;
	Parameter<string> param_architecture_description_filename;
	Parameter<bool> param_verbose;
	Parameter<std::string> param_monitor_internals;
};

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
