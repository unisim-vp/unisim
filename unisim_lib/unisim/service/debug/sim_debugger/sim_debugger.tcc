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

#ifndef __UNISIM_SERVICE_DEBUG_SIM_DEBUGGER_SIM_DEBUGGER_TCC_
#define __UNISIM_SERVICE_DEBUG_SIM_DEBUGGER_SIM_DEBUGGER_TCC_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <fstream>
#include <iostream>

#ifdef WIN32
#include <unistd.h>
#endif

#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace service {
namespace debug {
namespace sim_debugger {

using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;
using unisim::util::debug::Statement;
using unisim::util::debug::Breakpoint;
using unisim::util::debug::Watchpoint;
using unisim::util::debug::Symbol;
using std::cerr;
using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::list;

template <class ADDRESS>
SimDebugger<ADDRESS>::
SimDebugger(const char *_name, Object *_parent)
	: Object(_name, _parent, "Simulator debugger")
	, Service<DebugControl<ADDRESS> >(_name, _parent)
	, Service<MemoryAccessReporting<ADDRESS> >(_name, _parent)
	, Service<TrapReporting>(_name, _parent)
	, Client<MemoryAccessReportingControl>(_name, _parent)
	, Client<Disassembly<ADDRESS> >(_name, _parent)
	, Client<Memory<ADDRESS> >(_name, _parent)
	, Client<Registers>(_name, _parent)
	, Client<SymbolTableLookup<ADDRESS> >(_name, _parent)
	, Client<Loader>(_name, _parent)
	, Client<StatementLookup<ADDRESS> >(_name, _parent)
	, SimDebuggerBase()
	, unisim::api::debug::DebugAPI(_name, this)
	, debug_control_export("debug-control-export", this)
	, memory_access_reporting_export("memory-access-reporting-export", this)
	, trap_reporting_export("trap-reporting-export", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, memory_access_reporting_control_import("memory-access-reporting-control-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import(0)
	, loader_import(0)
	, stmt_lookup_import(0)
	, memory_atom_size(1)
	, num_loaders(0)
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, param_num_loaders("num-loaders", this, num_loaders, "number of loaders")
	, param_search_path("search-path", this, search_path, "Search path for source (separated by ';')")
	, breakpoint_registry()
	, watchpoint_registry()
	, program_profile()
	, data_read_profile()
	, running_mode(SIM_DEBUGGER_MODE_CONTINUE)
	, disasm_addr(0)
	, dump_addr(0)
{
	param_num_loaders.SetMutable(false);
	param_num_loaders.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	trap = false;
	running_mode = SIM_DEBUGGER_MODE_CONTINUE;
	disasm_addr = 0;
	dump_addr = 0;

	if ( num_loaders )
	{
		unsigned int i;
		typedef ServiceImport<Loader> *PLoaderImport;
		loader_import = new PLoaderImport[num_loaders];

		for ( i = 0; i < num_loaders; i++ )
		{
			std::stringstream sstr_name;
			sstr_name << "loader-import[" << i << "]";
			loader_import[i] =
					new ServiceImport<Loader>(sstr_name.str().c_str(),
							this);
		}

		typedef ServiceImport<SymbolTableLookup<ADDRESS> > *PSymbolTableLookupImport;
		symbol_table_lookup_import = new PSymbolTableLookupImport[num_loaders];

		for ( i = 0; i < num_loaders; i++ )
		{
			std::stringstream sstr_name;
			sstr_name << "symbol-table-lookup-import[" << i << "]";
			symbol_table_lookup_import[i] =
					new ServiceImport<SymbolTableLookup<ADDRESS> >(sstr_name.str().c_str(),
							this);
		}

		typedef ServiceImport<StatementLookup<ADDRESS> > *PStatementLookupImport;
		stmt_lookup_import = new PStatementLookupImport[num_loaders];

		for ( i = 0; i < num_loaders; i++ )
		{
			std::stringstream sstr_name;
			sstr_name << "stmt-lookup-import[" << i << "]";
			stmt_lookup_import[i] =
					new ServiceImport<StatementLookup<ADDRESS> >(sstr_name.str().c_str(),
							this);
		}
	}

	debug_control_export.SetupDependsOn(memory_access_reporting_control_import);
}

template <class ADDRESS>
SimDebugger<ADDRESS>::
~SimDebugger()
{
	if ( num_loaders )
	{
		unsigned int i;

		for ( i = 0; i < num_loaders; i++ )
		{
			delete loader_import[i];
			delete stmt_lookup_import[i];
		}
		delete[] loader_import;
		delete[] stmt_lookup_import;
	}
}

template<class ADDRESS>
bool
SimDebugger<ADDRESS>::
EndSetup() {
	if ( memory_access_reporting_control_import ) {
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				false);
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				false);
	}

	if ( memory_atom_size != 1 &&
			memory_atom_size != 2 &&
			memory_atom_size != 4 &&
			memory_atom_size != 8 &&
			memory_atom_size != 16 )
	{
		cerr << Object::GetName() << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << endl;
		return false;
	}
	return true;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
OnDisconnect()
{
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat,
		typename MemoryAccessReporting<ADDRESS>::MemoryType mt,
		ADDRESS addr,
		uint32_t size)
{
	if ( watchpoint_registry.HasWatchpoint(mat, mt, addr, size) )
		trap = true;
	//if ( mt == MemoryAccessReporting<ADDRESS>::MT_DATA )
	if ( mt == unisim::util::debug::MT_DATA ) {
		//if ( mat == MemoryAccessReporting<ADDRESS>::MAT_WRITE )
		if ( mat == unisim::util::debug::MAT_WRITE ) {
			data_write_profile.Accumulate(addr, 1);
		} else {
			data_read_profile.Accumulate(addr, 1);
		}
	}
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
ReportFinishedInstruction(ADDRESS next_addr)
{
	if ( breakpoint_registry.HasBreakpoint(next_addr) )
		trap = true;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
ReportTrap()
{
	trap = true;
	cout << "-> Trap recieved" << endl;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj)
{
	trap = true;
	cout << "-> Received trap from \"" << obj.GetName() << "\"" << endl;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const std::string &str)
{
	trap = true;
	cout << "-> Received trap from \"" << obj.GetName() << "\": " << str << endl;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const char *c_str)
{
	trap = true;
	cout << "-> Received trap from \"" << obj.GetName() << "\": " << c_str << endl;
}

template <class ADDRESS>
typename DebugControl<ADDRESS>::DebugCommand
SimDebugger<ADDRESS>::
FetchDebugCommand(ADDRESS cia)
{
	if ( !trap &&
			(running_mode == SIM_DEBUGGER_MODE_CONTINUE) )
	{
		program_profile.Accumulate(cia, 1);
		return DebugControl<ADDRESS>::DBG_STEP;
	}

	// A trap was produced or we are in step mode
	CallBreakpointHandler(cia);
	program_profile.Accumulate(cia, 1);
	trap = false;
	return DebugControl<ADDRESS>::DBG_STEP;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetStepMode()
{
	running_mode = SIM_DEBUGGER_MODE_STEP;
	return true;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetContinueMode()
{
	running_mode = SIM_DEBUGGER_MODE_CONTINUE;
	return true;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
IsModeStep()
{
	return running_mode == SIM_DEBUGGER_MODE_STEP;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
IsModeContinue()
{
	return running_mode == SIM_DEBUGGER_MODE_CONTINUE;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
Disasm(uint64_t addr, int count)
{
	if ( count > 0 )
	{
		const Symbol<ADDRESS> *last_symbol = 0;
		ADDRESS next_addr;
		do
		{
			const Statement<ADDRESS> *stmt = 0;
			unsigned int i;
			for ( i = 0; (!stmt) && (i < num_loaders); i++ )
			{
				if ( *stmt_lookup_import[i] )
				{
					stmt = (*stmt_lookup_import[i])->FindStatement(addr);
				}
			}

			if ( stmt )
			{
				const char *source_filename = stmt->GetSourceFilename();
				if ( source_filename )
				{
					unsigned int lineno = stmt->GetLineNo();
					unsigned int colno = stmt->GetColNo();
					std::string source_path;
					const char *source_dirname = stmt->GetSourceDirname();
					if ( source_dirname )
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;
					DumpSource(source_path.c_str(), lineno, colno, 1);
				}
			}

			cout.fill('0');
			const Symbol<ADDRESS> *symbol = 0;

			for ( i = 0; (!symbol) && (i < num_loaders); i++ )
			{
				if ( *symbol_table_lookup_import[i] )
				{
					symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
				}
			}
			string s = disasm_import->Disasm(addr, next_addr);

			if ( symbol )
			{
				if ( symbol != last_symbol )
				{
					cout << "0x" << hex;
					cout.width(8);
					cout << (addr / memory_atom_size) << dec;
					cout << " <";
					cout << symbol->GetFriendlyName(addr);

					cout << ">:" << endl;

					last_symbol = symbol;
				}
			}
			else
			{
				if ( symbol != last_symbol )
				{
					cout << "?";
					last_symbol = symbol;
				}
			}
			cout << "0x" << hex;
			cout.width(8);
			cout << (addr / memory_atom_size) << ":" << dec << s << endl;
			cout.fill(' ');
		} while ( addr = next_addr, --count > 0 );
	}
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
HasBreakpoint(uint64_t addr)
{
	return breakpoint_registry.HasBreakpoint(addr);
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
HasBreakpoint(const char *str)
{
	uint64_t addr = 0;

	if ( !GetSymbolAddress(str, addr) )
		if ( !GetFileSystemAddress(str, addr) )
			return false;

	return HasBreakpoint(addr);
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetBreakpoint(uint64_t addr)
{
	bool set = false;
	if ( !breakpoint_registry.SetBreakpoint(addr) )
		cout << "Can't set breakpoint at 0x" << hex << addr << dec << endl;
	else
		set = true;

	if ( memory_access_reporting_control_import )
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				breakpoint_registry.HasBreakpoints());

	return set;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetBreakpoint(const char *str)
{
	uint64_t addr = 0;

	if ( !GetSymbolAddress(str, addr) )
		if ( !GetFileSystemAddress(str, addr) )
		{
			return false;
		}

	return SetBreakpoint(addr);
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
DeleteBreakpoint(uint64_t addr)
{
	bool deleted = false;
	if ( !breakpoint_registry.RemoveBreakpoint(addr) )
		cout << "Can't remove breakpoint at 0x" << hex << addr << dec << endl;
	else
		deleted = true;

	if ( memory_access_reporting_control_import )
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				breakpoint_registry.HasBreakpoints());

	return deleted;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
DeleteBreakpoint(const char *str)
{
	uint64_t addr = 0;

	if ( !GetSymbolAddress(str, addr) )
		if ( !GetFileSystemAddress(str, addr) )
			return false;

	return DeleteBreakpoint(addr);
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetWatchpoint(uint64_t addr, uint32_t size)
{
	bool set = false;

	set = SetReadWatchpoint(addr, size);
	if ( set )
	{
		set = SetWriteWatchpoint(addr, size);
		if ( !set )
		{
			DeleteReadWatchpoint(addr, size);
		}
	}

	return set;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetReadWatchpoint(uint64_t addr, uint32_t size)
{
	bool set = false;

	//if ( !watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ,
			//MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size) )
	if ( !watchpoint_registry.SetWatchpoint(unisim::util::debug::MAT_READ,
			unisim::util::debug::MT_DATA, addr, size) )
		cout << "Can't set watchpoint at 0x" << hex << addr << dec << endl;
	else
		set = true;

	if ( memory_access_reporting_control_import )
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());

	return set;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
SetWriteWatchpoint(uint64_t addr, uint32_t size)
{
	bool set = false;

	//if ( !watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE,
			//MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size) )
	if ( !watchpoint_registry.SetWatchpoint(unisim::util::debug::MAT_WRITE,
			unisim::util::debug::MT_DATA, addr, size) )
		cout << "Can't set watchpoint at 0x" << hex << addr << dec << endl;
	else
		set = true;

	if ( memory_access_reporting_control_import )
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());

	return set;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
DeleteWatchpoint(uint64_t addr, uint32_t size)
{
	bool r_deleted = false;
	bool w_deleted = false;

	r_deleted = DeleteReadWatchpoint(addr, size);
	w_deleted = DeleteWriteWatchpoint(addr, size);

	return (r_deleted && w_deleted);
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
DeleteReadWatchpoint(uint64_t addr, uint32_t size)
{
	bool deleted = false;

	//if ( !watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ,
			//MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size) )
	if ( !watchpoint_registry.RemoveWatchpoint(unisim::util::debug::MAT_READ,
			unisim::util::debug::MT_DATA, addr, size) )
		cout << "Can't remove read watchpoint at 0x" << hex << addr << dec << " (" << size << " bytes)" << endl;
	else
		deleted = true;

	if ( memory_access_reporting_control_import )
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());

	return deleted;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
DeleteWriteWatchpoint(uint64_t addr, uint32_t size)
{
	bool deleted = false;

	//if ( !watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE,
			//MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size) )
	if ( !watchpoint_registry.RemoveWatchpoint(unisim::util::debug::MAT_WRITE,
			unisim::util::debug::MT_DATA, addr, size) )
		cout << "Can't remove write watchpoint at 0x" << hex << addr << dec << " (" << size << " bytes)" << endl;
	else
		deleted = true;

	if ( memory_access_reporting_control_import )
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());

	return deleted;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
GetSymbolAddress(const char *str, uint64_t &addr)
{
	const Symbol<ADDRESS> *symbol = 0;

	unsigned int i;
	for(i = 0; (!symbol) && (i < num_loaders); i++)
	{
		if(*symbol_table_lookup_import[i])
		{
			symbol = (*symbol_table_lookup_import[i])->FindSymbolByName(str);
		}
	}

	if(symbol)
	{
		addr = symbol->GetAddress();
		return true;
	}

	return false;
}

template <class ADDRESS>
bool
SimDebugger<ADDRESS>::
GetFileSystemAddress(const char *str, uint64_t &addr)
{
	std::string filename;
	unsigned int lineno;
	unsigned int i;

	while ( *str && *str != '#' )
	{
		filename += *str;
		str++;
	}
	if ( *str == '#' ) str++;
	if ( sscanf(str, "%u", &lineno) != 1 ) return false;

	const Statement<ADDRESS> *stmt = 0;
	for ( i = 0; (!stmt) && (i < num_loaders); i++ )
	{
		if ( *stmt_lookup_import[i] )
		{
			stmt = (*stmt_lookup_import[i])->FindStatement(filename.c_str(), lineno, 0);
		}
	}

	if ( stmt )
	{
		addr = stmt->GetAddress();
		return true;
	}

	return false;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpBreakpoints()
{
	const list<Breakpoint<ADDRESS> >& breakpoints = breakpoint_registry.GetBreakpoints();
	typename list<Breakpoint<ADDRESS> >::const_iterator iter;

	for ( iter = breakpoints.begin(); iter != breakpoints.end(); iter++ )
	{
		ADDRESS addr = iter->GetAddress();

		cout << "*0x" << hex << (addr / memory_atom_size) << dec << " (";

		const Symbol<ADDRESS> *symbol = 0;

		unsigned int i;
		for ( i = 0; (!symbol) && (i < num_loaders); i++ )
		{
			if ( *symbol_table_lookup_import[i] )
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}

		if ( symbol )
		{
			cout << symbol->GetFriendlyName(addr);
		}
		else
		{
			cout << "?";
		}
		cout << ")";

		const Statement<ADDRESS> *stmt = 0;
		//unsigned int i;
		for ( i = 0; (!stmt) && (i < num_loaders); i++ )
		{
			if ( *stmt_lookup_import[i] )
			{
				stmt = (*stmt_lookup_import[i])->FindStatement(addr);
			}
		}

		if ( stmt )
		{
			const char *source_filename = stmt->GetSourceFilename();
			if ( source_filename )
			{
				unsigned int lineno = stmt->GetLineNo();
				unsigned int colno = stmt->GetColNo();
				std::string source_path;
				const char *source_dirname = stmt->GetSourceDirname();
				if ( source_dirname )
				{
					source_path += source_dirname;
					source_path += '/';
				}
				source_path += source_filename;
				cout << " in ";
				DumpSource(source_path.c_str(), lineno, colno, 1);
			}
		}

		cout << endl;
	}
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpWatchpoints()
{
	const list<Watchpoint<ADDRESS> >& watchpoints = watchpoint_registry.GetWatchpoints();
	typename list<Watchpoint<ADDRESS> >::const_iterator iter;

	for ( iter = watchpoints.begin(); iter != watchpoints.end(); iter++ )
	{
		ADDRESS addr = iter->GetAddress();
		uint32_t size = iter->GetSize();
		//typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat = iter->GetMemoryAccessType();
		//typename MemoryAccessReporting<ADDRESS>::MemoryType mt = iter->GetMemoryType();
		typename unisim::util::debug::MemoryAccessType mat = iter->GetMemoryAccessType();
		typename unisim::util::debug::MemoryType mt = iter->GetMemoryType();

		switch ( mt )
		{
		//case MemoryAccessReporting<ADDRESS>::MT_INSN:
      case unisim::util::debug::MT_INSN:
			cout << "insn"; // it should never occur
			break;
		//case MemoryAccessReporting<ADDRESS>::MT_DATA:
      case unisim::util::debug::MT_DATA:
			cout << "data";
			break;
		}
		cout << " ";
		switch ( mat )
		{
		//case MemoryAccessReporting<ADDRESS>::MAT_READ:
      case unisim::util::debug::MAT_READ:
			cout << " read";
			break;
		//case MemoryAccessReporting<ADDRESS>::MAT_WRITE:
      case unisim::util::debug::MAT_WRITE:
			cout << "write";
			break;
		default:
			cout << "  (?)";
			break;
		}
		cout << " ";

		cout << "*0x" << hex << (addr / memory_atom_size) << dec << ":" << (size / memory_atom_size) << " (";

		const Symbol<ADDRESS> *symbol = 0;

		unsigned int i;
		for ( i = 0; (!symbol) && (i < num_loaders); i++ )
		{
			if ( *symbol_table_lookup_import[i] )
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}

		if ( symbol )
		{
			cout << symbol->GetFriendlyName(addr);
		}
		else
		{
			cout << "?";
		}
		cout << ")" << endl;
	}
}


template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpMemory(uint64_t addr)
{
	unsigned int i, j;
	std::streamsize width = cout.width();

	cout.fill(' ');
	cout.width(2 * sizeof(addr));
	cout << "address" << hex;
	cout.width(0);
	for ( i = 0; i < 16 / memory_atom_size; i++ )
	{
		cout << "  " << i;
		for ( j = 1; j < memory_atom_size; j++ )
			cout << "   ";
	}
	cout << endl;
	for ( i = 0; i < 16; i++ )
	{
		cout << hex;
		cout.fill('0');
		cout.width(2 * sizeof(addr));
		cout << (addr / memory_atom_size) << " ";
		cout.fill(' ');
		for ( j = 0; j < 16; j++, addr++ )
		{
			uint8_t value;
			memory_import->ReadMemory(addr, &value, 1);
			cout << (uint32_t)(value >> 4);
			cout << (uint32_t)(value & 15);
			if ( j < 15 )
				cout << " ";
		}
		addr -= 16;
		cout << dec << "  ";
		for ( j = 0; j < 16; j++, addr++ )
		{
			uint8_t value;
			memory_import->ReadMemory(addr, &value, 1);
			cout << (char)((value >= ' ' && value < 128) ? value : '.');
		}
		cout << endl;
	}
	cout.width(width);
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpProgramProfile()
{
	cout << "Program profile:" << endl;

	typename std::map<ADDRESS, uint64_t> map = program_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for ( iter = map.begin(); iter != map.end(); iter++ )
	{
		ADDRESS addr = (*iter).first;
		const Symbol<ADDRESS> *symbol = 0;

		unsigned int i;
		for ( i = 0; (!symbol) && (i < num_loaders); i++ )
		{
			if ( *symbol_table_lookup_import[i] )
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}

		ADDRESS next_addr;

		string s = disasm_import->Disasm(addr, next_addr);

		cout << "0x" << hex;
		cout.fill('0');
		cout.width(8);
		cout << (addr / memory_atom_size) << dec;

		if ( symbol )
		{
			if ( symbol )
			{
				cout << " <";
				cout << symbol->GetFriendlyName(addr);
				cout << ">";
			}
		}
		else
		{
			cout << " <?>";
		}

		cout.fill(' ');
		cout.width(0);
		cout << ":" << (*iter).second << " times:" << s << endl;
	}
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpAvailableLoaders()
{
	if ( num_loaders && loader_import )
	{
		unsigned int i;
		for ( i = 0; i < num_loaders; i++ )
		{
			ServiceImport<Loader> *import = loader_import[i];
			if ( *import )
			{
				Object *service = import->GetService();
				if ( service )
				{
					cout << service->GetName() << endl;
				}
			}
		}
	}
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
Load(const char *loader_name)
{
	if ( num_loaders && loader_import )
	{
		unsigned int i;
		for( i = 0; i < num_loaders; i++ )
		{
			ServiceImport<Loader> *import = loader_import[i];
			if ( *import )
			{
				Object *service = import->GetService();
				if ( service )
				{
					if ( strcmp(service->GetName(), loader_name) == 0 )
					{
						// Found loader
						if ( !(*import)->Load() )
						{
							cerr << Object::GetName() << ": ERROR! Loader \"" 
								<< loader_name 
								<< "\" was not able to load data/program" 
								<< endl;
						}

						return;
					}
				}
			}
		}
	}
	cerr << Object::GetName() << ": ERROR! Loader \"" << loader_name << "\" does not exist" << endl;
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpDataProfile(bool write)
{
	cout << "Data " << (write ? "write" : "read") << " profile:" << endl;
	typename std::map<ADDRESS, uint64_t> map = write ? data_write_profile : data_read_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for ( iter = map.begin(); iter != map.end(); iter++ )
	{
		ADDRESS addr = (*iter).first;
		const Symbol<ADDRESS> *symbol = 0;

		unsigned int i;
		for ( i = 0; (!symbol) && (i < num_loaders); i++ )
		{
			if ( *symbol_table_lookup_import[i] )
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}

		cout << "0x" << hex;
		cout.fill('0');
		cout.width(8);
		cout << (addr / memory_atom_size) << dec;

		if ( symbol )
		{
			if ( symbol )
			{
				cout << " <";
				cout << symbol->GetFriendlyName(addr);
				cout << ">";
			}
		}
		else
		{
			cout << " <?>";
		}

		cout.fill(' ');
		cout.width(0);
		cout << ":" << (*iter).second << " times" << endl;
	}
}

template <class ADDRESS>
void
SimDebugger<ADDRESS>::
DumpSource(const char *source_path, unsigned int lineno, unsigned int colno, unsigned int count)
{
	std::string s;
	const char *p;

	std::vector<std::string> search_paths;

	s.clear();
	p = search_path.c_str();
	do
	{
		if ( *p == 0 || *p == ';' )
		{
			search_paths.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while ( *(p++) );

	std::vector<std::string> hierarchical_source_path;

	s.clear();
	p = source_path;
	do
	{
		if ( *p == 0 || *p == '/' || *p == '\\' )
		{
			hierarchical_source_path.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while ( *(p++) );

	bool match = false;
	std::string match_source_path;

	int hierarchical_source_path_depth = hierarchical_source_path.size();

	if ( hierarchical_source_path_depth > 0 )
	{
		int num_search_paths = search_paths.size();
		int k;

		for ( k = 0; k < num_search_paths; k++ )
		{
			const std::string& search_path = search_paths[k];
			int i;
			for ( i = 0; (!match) && (i < hierarchical_source_path_depth); i++ )
			{
				std::string try_source_path = search_path;
				int j;
				for ( j = i; j < hierarchical_source_path_depth; j++ )
				{
					if ( !try_source_path.empty() ) try_source_path += '/';
					try_source_path += hierarchical_source_path[j];
				}
				//std::cerr << "try_source_path=\"" << try_source_path << "\":";
				if ( access(try_source_path.c_str(), R_OK) == 0 )
				{
					//std::cerr << "found" << std::endl;
					match = true;
					match_source_path = try_source_path;
				}
				else
				{
					//std::cerr << "not found" << std::endl;
				}
			}
		}
	}

	if ( match )
	{
		std::cout << match_source_path;
	}
	else
	{
		std::cout << source_path << " (not found)";
	}

	std::cout << " at line #" << lineno;
	if ( colno )
	{
		std::cout << ", column #" << colno;
	}
	cout << ": ";

	if ( match )
	{
		std::ifstream f(match_source_path.c_str(), std::ifstream::in);
		std::string line;

		unsigned int n = 1;
		unsigned int printed = 0;
		do
		{
			getline(f, line);
			if ( f.fail() || f.eof() ) break;

			if ( (n >= lineno) && (n < (lineno + count)) )
			{
				std::cout << line << std::endl;
				printed++;
			}
			n++;
		} while ( printed < count );
	}
	else
	{
		std::cout << std::endl;
	}
}

} // end of namespace sim_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
