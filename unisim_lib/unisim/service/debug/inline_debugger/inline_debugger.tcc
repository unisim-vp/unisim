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
 
#ifndef __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_TCC_
#define __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_TCC_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>

#if defined(HAVE_CONFIG_H)
//#include "unisim/service/debug/inline_debugger/config.h"
#include "config.h"
#endif

#if defined(HAVE_LIBEDIT)
#include <editline/readline.h>
#endif

#include <unisim/util/endian/endian.hh>
#include <fstream>
#include <iostream>

#if defined(WIN32)
#include <io.h>     // for function access()
#else
#include <unistd.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

using std::list;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::streamsize;
using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;
using unisim::util::debug::Statement;

template <class ADDRESS>
InlineDebugger<ADDRESS>::InlineDebugger(const char *_name, Object *_parent)
	: Object(_name, _parent, "this service implements a built-in debugger in the terminal console")
	, Service<DebugControl<ADDRESS> >(_name, _parent)
	, Service<MemoryAccessReporting<ADDRESS> >(_name, _parent)
	, Service<TrapReporting>(_name, _parent)
	, Client<MemoryAccessReportingControl>(_name, _parent)
	, Client<Disassembly<ADDRESS> >(_name, _parent)
	, Client<Memory<ADDRESS> >(_name, _parent)
	, Client<Registers>(_name, _parent)
	, Client<SymbolTableLookup<ADDRESS> >(_name, _parent)
	, Client<Loader<ADDRESS> >(_name, _parent)
	, Client<StatementLookup<ADDRESS> >(_name, _parent)
	, InlineDebuggerBase()
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
	, data_write_profile()
	, running_mode(INLINE_DEBUGGER_MODE_WAITING_USER)
	, disasm_addr(0)
	, dump_addr(0)
	, prompt(string(_name) + "> ")
	, hex_addr_fmt(0)
	, int_addr_fmt(0)
{
	param_num_loaders.SetMutable(false);
	param_num_loaders.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	trap = false;
	strcpy(last_line, "");
	strcpy(line, "");

	switch(sizeof(ADDRESS))
	{
		case 1:
			hex_addr_fmt = strdup(PRIx8);
			break;
		case 2:
			hex_addr_fmt = strdup(PRIx16);
			break;
		case 4:
			hex_addr_fmt = strdup(PRIx32);
			break;
		case 8:
			hex_addr_fmt = strdup(PRIx64);
			break;
	}

	switch(sizeof(ADDRESS))
	{
		case 1:
			int_addr_fmt = strdup(PRIu8);
			break;
		case 2:
			int_addr_fmt = strdup(PRIu16);
			break;
		case 4:
			int_addr_fmt = strdup(PRIu32);
			break;
		case 8:
			int_addr_fmt = strdup(PRIu64);
			break;
	}
	
	if(num_loaders)
	{
		unsigned int i;
		typedef ServiceImport<Loader<ADDRESS> > *PLoaderImport;
		loader_import = new PLoaderImport[num_loaders];
		
		for(i = 0; i < num_loaders; i++)
		{
			std::stringstream sstr_name;
			sstr_name << "loader-import[" << i << "]";
			loader_import[i] = new ServiceImport<Loader<ADDRESS> >(sstr_name.str().c_str(), this);
		}

		typedef ServiceImport<SymbolTableLookup<ADDRESS> > *PSymbolTableLookupImport;
		symbol_table_lookup_import = new PSymbolTableLookupImport[num_loaders];
		
		for(i = 0; i < num_loaders; i++)
		{
			std::stringstream sstr_name;
			sstr_name << "symbol-table-lookup-import[" << i << "]";
			symbol_table_lookup_import[i] = new ServiceImport<SymbolTableLookup<ADDRESS> >(sstr_name.str().c_str(), this);
		}

		typedef ServiceImport<StatementLookup<ADDRESS> > *PStatementLookupImport;
		stmt_lookup_import = new PStatementLookupImport[num_loaders];
		
		for(i = 0; i < num_loaders; i++)
		{
			std::stringstream sstr_name;
			sstr_name << "stmt-lookup-import[" << i << "]";
			stmt_lookup_import[i] = new ServiceImport<StatementLookup<ADDRESS> >(sstr_name.str().c_str(), this);
		}
	}
}

template <class ADDRESS>
InlineDebugger<ADDRESS>::~InlineDebugger()
{
	free(hex_addr_fmt);
	free(int_addr_fmt);

	if(num_loaders)
	{
		unsigned int i;
		
		for(i = 0; i < num_loaders; i++)
		{
			delete loader_import[i];
			delete symbol_table_lookup_import[i];
			delete stmt_lookup_import[i];
		}
		delete[] loader_import;
		delete[] symbol_table_lookup_import;
		delete[] stmt_lookup_import;
	}
}

template<class ADDRESS>
bool InlineDebugger<ADDRESS>::EndSetup()
{
	if(memory_access_reporting_control_import)
	{
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(false);
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(false);
	}

	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		cerr << Object::GetName() << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << endl;
		return false;
	}
	return true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(watchpoint_registry.HasWatchpoint(mat, mt, addr, size)) trap = true;
	if(mt == MemoryAccessReporting<ADDRESS>::MT_DATA)
	{
		if(mat == MemoryAccessReporting<ADDRESS>::MAT_WRITE)
		{
			data_write_profile.Accumulate(addr, 1);
		}
		else
		{
			data_read_profile.Accumulate(addr, 1);
		}
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ReportFinishedInstruction(ADDRESS next_addr)
{
	if(breakpoint_registry.HasBreakpoint(next_addr)) trap = true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ReportTrap()
{
	trap = true;
	cout << "-> Received trap" << endl;
}

template <class ADDRESS>
void 
InlineDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj)
{
	trap = true;
	cout << "-> Received trap from \"" << obj.GetName() << "\"" << endl;
}
	
template <class ADDRESS>
void
InlineDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const std::string &str)
{
	trap = true;
	cout << "-> Received trap from \"" << obj.GetName() << "\": " << str << endl;
}
	
template <class ADDRESS>
void 
InlineDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const char *c_str)
{
	trap = true;
	cout << "-> Received trap from \"" << obj.GetName() << "\": " << c_str << endl;
}
	
template <class ADDRESS>
typename DebugControl<ADDRESS>::DebugCommand InlineDebugger<ADDRESS>::FetchDebugCommand(ADDRESS cia)
{
	bool recognized = false;
	ADDRESS addr;
	ADDRESS cont_addr;
	unsigned int size;
	char parm[32][1024];

	if(!trap && (running_mode == INLINE_DEBUGGER_MODE_CONTINUE || running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL))
	{
		program_profile.Accumulate(cia, 1);
		return DebugControl<ADDRESS>::DBG_STEP;
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL)
	{
		DeleteBreakpoint(cont_addr);
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_RESET)
	{
		running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
		program_profile.Accumulate(cia, 1);
		return DebugControl<ADDRESS>::DBG_STEP;
	}

	trap = false;

	int nparms = 0;

	Disasm(cia, 1);

	while(1)
	{
		//cout << "> ";

		if(!GetLine(line, sizeof(line)))
		{
			Object::Stop(0);
			return DebugControl<ADDRESS>::DBG_KILL;
		}

		if(IsBlankLine(line))
		{
	 		strcpy(line, last_line);
//			cout << "\033[1A\033[2C" << line << endl;
		}
		
		nparms = sscanf(line, " %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
	                parm[0], parm[1], parm[2], parm[3], parm[4], parm[5], parm[6], parm[7],
	                parm[8], parm[9], parm[10], parm[11], parm[12], parm[13], parm[14], parm[7],
	                parm[16], parm[17], parm[18], parm[19], parm[20], parm[21], parm[22], parm[23],
	                parm[24], parm[25], parm[26], parm[27], parm[28], parm[29], parm[30], parm[31]
					);

	
		recognized = false;

		switch(nparms)
		{
			case 1:
				{
					unisim::util::debug::Register *reg = registers_import->GetRegister(parm[0]);

					if(reg)
					{
						recognized = true;
						unsigned int size = reg->GetSize();
						uint8_t value[size];
						reg->GetValue(&value);
						int i;
						cout << reg->GetName() << " = 0x" << hex;
#if BYTE_ORDER == BIG_ENDIAN
						for(i = 0; i < (int) size; i++)
#else
						for(i = size - 1; i >= 0; i--)
#endif
						{
							cout << (value[i] >> 4);
							cout << (value[i] & 15);
						}
						cout << dec << endl;
						break;
					}
				}

				if(IsHelpCommand(parm[0]))
				{
					recognized = true;
					Help();
					break;
				}

				if(IsQuitCommand(parm[0]))
				{
					recognized = true;
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}

				if(IsStepCommand(parm[0]))
				{
					running_mode = INLINE_DEBUGGER_MODE_STEP;
					strcpy(last_line, line);
					program_profile.Accumulate(cia, 1);
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsNextCommand(parm[0]))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
					strcpy(last_line, line);
					disasm_import->Disasm(cia, cont_addr);
					if(HasBreakpoint(cont_addr))
					{
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					}
					else
					{
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
						SetBreakpoint(cont_addr);
					}
					program_profile.Accumulate(cia, 1);
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsContinueCommand(parm[0]))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					strcpy(last_line, line);
					program_profile.Accumulate(cia, 1);
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsDisasmCommand(parm[0]))
				{
					recognized = true;
					Disasm(disasm_addr, 10);
					disasm_addr += 10 * 4;
					break;
				}

				if(IsDumpCommand(parm[0]))
				{
					recognized = true;
					DumpMemory(dump_addr);
					dump_addr += 256;
					break;
				}

				if(IsBreakCommand(parm[0]))
				{
					recognized = true;
					DumpBreakpoints();
					break;
				}

				if(IsWatchCommand(parm[0]))
				{
					recognized = true;
					DumpWatchpoints();
					break;
				}
				
				if(IsResetCommand(parm[0]))
				{
					trap = false;
					strcpy(last_line, "");
					strcpy(line, "");
					running_mode = INLINE_DEBUGGER_MODE_RESET;
					disasm_addr = 0;
					dump_addr = 0;
					return DebugControl<ADDRESS>::DBG_RESET;
				}

				if (IsMonitorCommand(parm[0]))
				{
					recognized = true;
					DumpVariables();
					break;
				}
				
				if (IsRegisterCommand(parm[0]))
				{
					recognized = true;
					DumpRegisters();
					break;
				}
				
				if (IsStatisticCommand(parm[0]))
				{
					recognized = true;
					DumpStatistics();
					break;
				}
				
				if (IsParameterCommand(parm[0]))
				{
					recognized = true;
					DumpParameters();
					break;
				}

				if(IsProfileCommand(parm[0]))
				{
					recognized = true;
					DumpProgramProfile();
					DumpDataProfile(false /* read */);
					DumpDataProfile(true /* write */);
					break;
				}
				
				if(IsLoadCommand(parm[0]))
				{
					recognized = true;
					DumpAvailableLoaders();
					break;
				}
				
				if (recognized == false)
				{
					// check for possible variable that could have the given name (parm[0])
					recognized = DumpVariable(parm[0]);
					break;
				}

				break;
			case 2:
				if(IsDisasmCommand(parm[0]) && ParseAddr(parm[1], disasm_addr))
				{
					recognized = true;
					Disasm(disasm_addr, 10);
					disasm_addr += 10 * 4;
					break;
				}

				if(IsDumpCommand(parm[0]) && ParseAddr(parm[1], dump_addr))
				{
					recognized = true;
					DumpMemory(dump_addr);
					dump_addr += 256;
					break;
				}

				if(IsBreakCommand(parm[0]) && ParseAddr(parm[1], addr))
				{
					recognized = true;
					SetBreakpoint(addr);
					break;
				}

				if(IsWatchCommand(parm[0]) && ParseAddrRange(parm[1], addr, size))
				{
					recognized = true;
					SetWriteWatchpoint(addr, size);
					break;
				}

				if(IsDeleteCommand(parm[0]) && ParseAddr(parm[1], addr))
				{
					recognized = true;
					DeleteBreakpoint(addr);
					break;
				}
				
				if(IsDeleteWatchCommand(parm[0]) && ParseAddrRange(parm[1], addr, size))
				{
					recognized = true;
					DeleteWriteWatchpoint(addr, size);
					break;
				}

				if(IsContinueCommand(parm[0]) && ParseAddr(parm[1], cont_addr))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
					strcpy(last_line, line);
					SetBreakpoint(cont_addr);
					program_profile.Accumulate(cia, 1);
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				
				if (IsMonitorCommand(parm[0]))
				{
					recognized = true;
					DumpVariable(parm[0], parm[1]);
					break;
				}
				
				if (IsRegisterCommand(parm[0]))
				{
					recognized = true;
					DumpRegister(parm[0], parm[1]);
					break;
				}
				
				if (IsStatisticCommand(parm[0]))
				{
					recognized = true;
					DumpStatistic(parm[0], parm[1]);
					break;
				}
				
				if (IsParameterCommand(parm[0]))
				{
					recognized = true;
					DumpParameter(parm[0], parm[1]);
					break;
				}

				if(IsProfileCommand(parm[0]))
				{
					if(strcmp(parm[1], "program") == 0)
					{
						recognized = true;
						DumpProgramProfile();
						break;
					}
					else if(strcmp(parm[1], "data") == 0)
					{
						recognized = true;
						DumpDataProfile(false /* read */);
						DumpDataProfile(true /* write */);
						break;
					}
				}
				
				if(IsLoadCommand(parm[0]))
				{
					recognized = true;
					Load(parm[1]);
					break;
				}
				break;
			case 3:
				if(IsWatchCommand(parm[0]) && ParseAddrRange(parm[1], addr, size))
				{
					if(strcmp(parm[2], "read") == 0)
					{
						recognized = true;
						SetReadWatchpoint(addr, size);
					}
					if(strcmp(parm[2], "write") == 0)
					{
						recognized = true;
						SetWriteWatchpoint(addr, size);
					}
					break;
				}
				
				if(IsDeleteWatchCommand(parm[0]) && ParseAddrRange(parm[1], addr, size))
				{
					if(strcmp(parm[2], "read") == 0)
					{
						recognized = true;
						DeleteReadWatchpoint(addr, size);
					}
					if(strcmp(parm[2], "write") == 0)
					{
						recognized = true;
						DeleteWriteWatchpoint(addr, size);
					}
					break;
				}
				
				if(IsProfileCommand(parm[0]))
				{
					if(strcmp(parm[1], "data") == 0)
					{
						if(strcmp(parm[2], "read") == 0)
						{
							recognized = true;
							DumpDataProfile(false /* read */);
							break;
						}
						else if(strcmp(parm[2], "write") == 0)
						{
							recognized = true;
							DumpDataProfile(true /* write */);
							break;
						}
					}
				}
				
				if (IsMonitorCommand(parm[0], parm[1]))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0];
					str << parm[1];
					DumpVariable(str.str().c_str(), parm[2]);
					break;
				}
				
				if (IsRegisterCommand(parm[0], parm[1]))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0];
					str << parm[1];
					DumpRegister(str.str().c_str(), parm[2]);
					break;
				}
				
				if (IsStatisticCommand(parm[0], parm[1]))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0];
					str << parm[1];
					DumpStatistic(str.str().c_str(), parm[2]);
					break;
				}
				
				if (IsParameterCommand(parm[0], parm[1]))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0];
					str << parm[1];
					DumpParameter(str.str().c_str(), parm[2]);
					break;
				}

				break;

			case EOF:
				recognized = true;
				break;

		} // end of switch

		if (nparms >= 3 && nparms <= 33)
		{
			if (IsMonitorSetCommand(parm[0]))
			{
				recognized = true;
				std::stringstream str;
				for (int i = 2; i < nparms; i++)
				{
					if (i > 2) str << " ";
						str << parm[i];
				}
				SetVariable(parm[1], str.str().c_str());
			}
		}

		if(!recognized)
		{
			cout << "Unrecognized command.  Try \"help\"." << endl;
			for (int i = 0; i < nparms; i++)
				cout << parm[i] << " ";
			cout << endl;
		}
		
		strcpy(last_line, line);
	}
	Object::Stop(0);
	return DebugControl<ADDRESS>::DBG_KILL;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Help()
{
	cout << "HELP:" << endl;
	cout << "================================  LOAD =========================================" << endl;
	cout << "<l | ld | load> [<loader name> <filename>]" << endl;
	cout << "    load program file <filename> using loader <loader name>" << endl;
	cout << "================================ EXECUTE =======================================" << endl;
	cout << "<c | cont | continue> [<symbol | *address>]" << endl;
	cout << "    continue to execute instructions until program reaches a breakpoint," << endl;
	cout << "    a watchpoint, 'symbol' or 'address'" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<s | si | step | stepi>" << endl;
	cout << "    execute one instruction" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<n | ni | next | nexti>" << endl;
	cout << "    continue to execute instructions until the processor reaches" << endl;
	cout << "    next contiguous instruction, a breakpoint or a watchpoint" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<r | run>" << endl;
	cout << "    restart the simulation from the beginning" << endl;
	cout << "================================ INSPECT =======================================" << endl;
	cout << "<dis | disasm | disassemble> [<symbol | *address>]" << endl;
	cout << "    continue to disassemble starting from 'symbol', 'address', or after" << endl;
	cout << "    the previous disassembly" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<d | dump> [<symbol | *address>]" << endl;
	cout << "    dump memory starting from 'symbol', 'address', or after the previous dump" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<register name>" << endl;
	cout << "    display the register value" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<variable name | hierarchical variable name>" << endl;
	cout << "    display all the objects variables that are named as the given variable name," << endl;
	cout << "    or that contain part of the given name" << endl;
	cout << "    note: this command is overriden if the given name matches any of the other" << endl;
	cout << "    inline debugger commands" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<m | monitor>[/<format>] [<variable name>]" << endl;
	cout << "<reg | register>[/<format>] [<register name>]" << endl;
	cout << "<param | parameter>[/<format>] [<parameter name>]" << endl;
	cout << "<stat | statistic>[/<format>] [<statistic name>]" << endl;
	cout << "    display the given simulator variable (register/parameter/statistic), or " << endl;
	cout << "    display all variable (register/parameter/statistic) names if none is given" << endl;
	cout << "    when giving a variable name a format can be used, the available formats are:" << endl;
	cout << "     x     print variable as an integer in hexadecimal format" << endl;
	cout << "     d     print variable as a signed integer" << endl;
	cout << "     u     print variable as an unsigned integer" << endl;
	cout << "     o     print variable as an integer in octal format" << endl;
	cout << "     t     print variable as an integer in binary format" << endl;
	cout << "     a     print variable as an address (equivalent to 'x')" << endl;
	cout << "     c     print variable as a string" << endl;
	cout << "     f     print variable as a float (actually is considered as a double)" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "set <variable name> <value>" << endl;
	cout << "    sets the variable (register/parameter/statistic) to the given value" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<p | prof | profile>" << endl;
	cout << "<p | prof | profile> program" << endl;
	cout << "<p | prof | profile> data" << endl;
	cout << "<p | prof | profile> data read" << endl;
	cout << "<p | prof | profile> data write" << endl;
	cout << "    display the program/data profile" << endl;
	cout << "========================= BREAKPOINTS/WATCHPOINTS ==============================" << endl;
	cout << "<b | break> [<symbol | *address | filename#lineno>]" << endl;
	cout << "    set a breakpoint at 'symbol', 'address', or 'filename#lineno'. If 'symbol', 'address'," << endl;
	cout << "    or 'filename#lineno' are not specified, display the breakpoint list" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<w | watch> [<symbol | *address[:<size>]>] [<read | write>]" << endl;
	cout << "    set a watchpoint at 'symbol' or 'address'." << endl;
	cout << "    When using 'continue' and 'next' commands, the debugger will spy CPU loads" << endl;
	cout << "    and store. The debugger will return to command line prompt once a load," << endl;
	cout << "    or a store will access to 'symbol' or 'address'." << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<del | delete> <symbol | *address | filename#lineno>" << endl;
	cout << "    delete the breakpoint at 'symbol', 'address', or 'filename#lineno'" << endl << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<delw | delwatch> <symbol | *address> [<read | write>] [<size>]" << endl;
	cout << "    delete the watchpoint at 'symbol' or 'address'" << endl;
	cout << "============================== MISCELLANEOUS ===================================" << endl;
	cout << "<h | ? | help>" << endl;
	cout << "    display the help" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<quit | q>" << endl;
	cout << "    quit the built-in debugger" << endl << endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Disasm(ADDRESS addr, int count)
{
	if(count > 0)
	{
		const Symbol<ADDRESS> *last_symbol = 0;
		ADDRESS next_addr;
		do
		{
			const Statement<ADDRESS> *stmt = 0;
			unsigned int i;
			for(i = 0; (!stmt) && (i < num_loaders); i++)
			{
				if(*stmt_lookup_import[i])
				{
					stmt = (*stmt_lookup_import[i])->FindStatement(addr);
				}
			}
			
			if(stmt)
			{
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					unsigned int lineno = stmt->GetLineNo();
					unsigned int colno = stmt->GetColNo();
					std::string source_path;
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
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
			
			for(i = 0; (!symbol) && (i < num_loaders); i++)
			{
				if(*symbol_table_lookup_import[i])
				{
					symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
				}
			}
			string s = disasm_import->Disasm(addr, next_addr);
			
			if(symbol)
			{
				if(symbol != last_symbol)
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
				if(symbol != last_symbol)
				{
					cout << "?";
					last_symbol = symbol;
				}
			}
			cout << "0x" << hex;
			cout.width(8);
			cout << (addr / memory_atom_size) << ":" << dec << s << endl;
			cout.fill(' ');
		} while(addr = next_addr, --count > 0);
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::HasBreakpoint(ADDRESS addr)
{
	return breakpoint_registry.HasBreakpoint(addr);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	if(!breakpoint_registry.SetBreakpoint(addr))
	{
		cout << "Can't set breakpoint at 0x" << hex << addr << dec << endl;
	}
	
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				breakpoint_registry.HasBreakpoints());
	
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetReadWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
	{
		cout << "Can't set watchpoint at 0x" << hex << addr << dec << endl;
	}

	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetWriteWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
	{
		cout << "Can't set watchpoint at 0x" << hex << addr << dec << endl;
	}

	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteBreakpoint(ADDRESS addr)
{
	if(!breakpoint_registry.RemoveBreakpoint(addr))
	{
		cout << "Can't remove breakpoint at 0x" << hex << addr << dec << endl;
	}

	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				breakpoint_registry.HasBreakpoints());
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteReadWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
	{
		cout << "Can't remove read watchpoint at 0x" << hex << addr << dec << " (" << size << " bytes)" << endl;
	}

	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteWriteWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
	{
		cout << "Can't remove write watchpoint at 0x" << hex << addr << dec << " (" << size << " bytes)" << endl;
	}

	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				watchpoint_registry.HasWatchpoints());
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpBreakpoints()
{
	const list<Breakpoint<ADDRESS> >& breakpoints = breakpoint_registry.GetBreakpoints();
	typename list<Breakpoint<ADDRESS> >::const_iterator iter;

	for(iter = breakpoints.begin(); iter != breakpoints.end(); iter++)
	{
		ADDRESS addr = iter->GetAddress();
		
		cout << "*0x" << hex << (addr / memory_atom_size) << dec << " (";
		
		const Symbol<ADDRESS> *symbol = 0;
		
		unsigned int i;
		for(i = 0; (!symbol) && (i < num_loaders); i++)
		{
			if(*symbol_table_lookup_import[i])
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}
		
		if(symbol)
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
		for(i = 0; (!stmt) && (i < num_loaders); i++)
		{
			if(*stmt_lookup_import[i])
			{
				stmt = (*stmt_lookup_import[i])->FindStatement(addr);
			}
		}
		
		if(stmt)
		{
			const char *source_filename = stmt->GetSourceFilename();
			if(source_filename)
			{
				unsigned int lineno = stmt->GetLineNo();
				unsigned int colno = stmt->GetColNo();
				std::string source_path;
				const char *source_dirname = stmt->GetSourceDirname();
				if(source_dirname)
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
void InlineDebugger<ADDRESS>::DumpWatchpoints()
{
	const list<Watchpoint<ADDRESS> >& watchpoints = watchpoint_registry.GetWatchpoints();
	typename list<Watchpoint<ADDRESS> >::const_iterator iter;

	for(iter = watchpoints.begin(); iter != watchpoints.end(); iter++)
	{
		ADDRESS addr = iter->GetAddress();
		uint32_t size = iter->GetSize();
		typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat = iter->GetMemoryAccessType();
		typename MemoryAccessReporting<ADDRESS>::MemoryType mt = iter->GetMemoryType();
		
		switch(mt)
		{
			case MemoryAccessReporting<ADDRESS>::MT_INSN:
				cout << "insn"; // it should never occur
				break;
			case MemoryAccessReporting<ADDRESS>::MT_DATA:
				cout << "data";
				break;
		}
		cout << " ";
		switch(mat)
		{
			case MemoryAccessReporting<ADDRESS>::MAT_READ:
				cout << " read";
				break;
			case MemoryAccessReporting<ADDRESS>::MAT_WRITE:
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
		for(i = 0; (!symbol) && (i < num_loaders); i++)
		{
			if(*symbol_table_lookup_import[i])
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}
		
		if(symbol)
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
void InlineDebugger<ADDRESS>::DumpMemory(ADDRESS addr)
{
	unsigned int i, j;
	streamsize width = cout.width();

	cout.fill(' ');
	cout.width(2 * sizeof(addr));
	cout << "address" << hex;
	cout.width(0);
	for(i = 0; i < 16 / memory_atom_size; i++)
	{
		cout << "  " << i;
		for(j = 1; j < memory_atom_size; j++) cout << "   ";
	}
	cout << endl;
	for(i = 0; i < 16; i++)
	{
		cout << hex; cout.fill('0'); cout.width(2 * sizeof(addr)); cout << (addr / memory_atom_size) << " "; cout.fill(' ');
		for(j = 0; j < 16; j++, addr++)
		{
			uint8_t value;
			memory_import->ReadMemory(addr, &value, 1);
			cout << (uint32_t)(value >> 4);
			cout << (uint32_t)(value & 15);
			if(j < 15) cout << " ";
		}
		addr -= 16;
		cout << dec << "  ";
		for(j = 0; j < 16; j++, addr++)
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
void InlineDebugger<ADDRESS>::DumpVariables()
{
	list<VariableBase *> lst;
	list<VariableBase *>::iterator iter;
	
	cout << "VARIABLES LIST:";
	
	GetSimulator()->GetVariables(lst);
	if (lst.size() == 0) cout << " No variables" << endl;
	else
	{
		cout << endl;
		for (iter = lst.begin(); iter != lst.end(); iter++)
		{
			switch ((*iter)->GetType())
			{
				case VariableBase::VAR_VOID:
					cout << " V";
					break;
				case VariableBase::VAR_PARAMETER:
					cout << " P";
					break;
				case VariableBase::VAR_STATISTIC:
					cout << " S";
					break;
				case VariableBase::VAR_REGISTER:
					cout << " R";
					break;
				default:
					cout << " ?";
					break;
			}
			cout << "\t" << (*iter)->GetName() << endl;
		}
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpRegisters()
{
	list<VariableBase *> lst;
	list<VariableBase *>::iterator iter;
	
	cout << "REGISTERS LIST:";
	
	GetSimulator()->GetRegisters(lst);
	if (lst.size() == 0) cout << " No registers" << endl;
	else
	{
		cout << endl;
		for (iter = lst.begin(); iter != lst.end(); iter++)
			cout << "\t" << (*iter)->GetName() << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpStatistics()
{
	list<VariableBase *> lst;
	list<VariableBase *>::iterator iter;
	
	cout << "STATISTICS LIST:";
	
	GetSimulator()->GetStatistics(lst);
	if (lst.size() == 0) cout << " No statistics" << endl;
	else
	{
		cout << endl;
		for (iter = lst.begin(); iter != lst.end(); iter++)
			cout << "\t" << (*iter)->GetName() << endl;
	}
}
	
template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpParameters()
{
	list<VariableBase *> lst;
	list<VariableBase *>::iterator iter;
	
	cout << "PARAMETERS LIST:";
	
	GetSimulator()->GetParameters(lst);
	if (lst.size() == 0) cout << " No parameters" << endl;
	else
	{
		cout << endl;
		for (iter = lst.begin(); iter != lst.end(); iter++)
			cout << "\t" << (*iter)->GetName() << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::MonitorGetFormat(const char *cmd, char &format)
{
	unsigned int len = 0;
	
	if (strncmp(cmd, "monitor/", 8) == 0) len = 8;
	if (strncmp(cmd, "m/", 2) == 0) len = 2;
	if (strncmp(cmd, "register/", 9) == 0) len = 9;
	if (strncmp(cmd, "reg/", 4) == 0) len = 4;
	if (strncmp(cmd, "statistic/", 10) == 0) len = 10;
	if (strncmp(cmd, "stat/", 5) == 0) len = 5;
	if (strncmp(cmd, "parameter/", 10) == 0) len = 10;
	if (strncmp(cmd, "param/", 6) == 0) len = 6;
	
	if (len == 0)
	{
		// no format was used, returning default format 'c'
		format = 'c';
		return;
	}
	
	cmd = cmd + len;
	
	// accepted formats = x, d, u, o, t, a, c, f
	if (strcmp(cmd, "x") == 0) 
	{
		format = 'x';
		return;
	}
	if (strcmp(cmd, "d") == 0)
	{
		format = 'd';
		return;
	}
	if (strcmp(cmd, "u") == 0)
	{
		format = 'u';
		return;
	}
	if (strcmp(cmd, "o") == 0)
	{
		format = 'o';
		return;
	}
	if (strcmp(cmd, "t") == 0)
	{
		format = 't';
		return;
	}
	if (strcmp(cmd, "a") == 0)
	{
		format = 'a';
		return;
	}
	if (strcmp(cmd, "c") == 0)
	{
		format = 'c';
		return;
	}
	if (strcmp(cmd, "f") == 0)
	{
		format = 'f';
		return;
	}
	cerr << "WARNING: unknow monitor/register/statistic/parameter format (" << cmd << "). See help for available formats." << endl;
	format = 'c';
}

template <class ADDRESS>
bool 
InlineDebugger<ADDRESS>::
DumpVariable(const char *name)
{
	bool found = false;
	list<VariableBase *> vars;
	GetSimulator()->GetVariables(vars);
	list<VariableBase *>::iterator var_it;
	
	for (var_it = vars.begin(); var_it != vars.end(); var_it++)
	{
		if (string((*var_it)->GetName()).find(name, 0, strlen(name)) != string::npos)
		{
			found = true;
			DumpVariable("m", (*var_it)->GetName());
		}
	}
	return found;
}
	
template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpVariable(const char *cmd, const VariableBase *variable)
{
	/* extract the format if any */
	char format = 0;
	MonitorGetFormat(cmd, format);
	
	cout << variable->GetName() << " = ";
	switch (format)
	{
		case 'x':
			cout << "0x" << hex << ((unsigned long long) *variable) << dec;
			break;
		case 'd':
			cout << ((long long) *variable);
			break;
		case 'u':
			cout << ((unsigned long long) *variable);
			break;
		case 'o':
			cout.setf(std::ios::oct);
			cout << "0" << ((unsigned long long) *variable);
			cout.unsetf(std::ios::oct);
			break;
		case 't':
			cout << "0b";
			for (unsigned int i = 0; i < sizeof(unsigned long long) * 8; i++)
				cout << (int)((((unsigned long long) *variable) >> i) & 1);
			break;
		case 'a':
			cout << "0x" << hex << ((unsigned long long) *variable) << dec;
			break;
		case 'c':
			cout << ((string) *variable);
			break;
		case 'f':
			cout << ((double) *variable);
			break;
	}
	
	cout << endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpVariable(const char *cmd, const char *name)
{
	VariableBase *variable = GetSimulator()->FindVariable(name);
	
	if (variable->IsVoid())
	{
		cout << "Unknow variable (" << name << ")" << endl;
		return;
	}

	switch (variable->GetType())
	{
		case VariableBase::VAR_VOID:
			cout << " V";
			break;
		case VariableBase::VAR_PARAMETER:
			cout << " P";
			break;
		case VariableBase::VAR_STATISTIC:
			cout << " S";
			break;
		case VariableBase::VAR_REGISTER:
			cout << " R";
			break;
		default:
			cout << " ?";
			break;
	}

	cout << "\t";
	DumpVariable(cmd, variable);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpRegister(const char *cmd, const char *name)
{
	VariableBase *variable = GetSimulator()->FindRegister(name);
	
	if (variable->IsVoid())
	{
		cout << "Unknow register (" << name << ")" << endl;
		return;
	}

	DumpVariable(cmd, variable);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpStatistic(const char *cmd, const char *name)
{
	VariableBase *variable = GetSimulator()->FindStatistic(name);
	
	if (variable->IsVoid())
	{
		cout << "Unknow statistic (" << name << ")" << endl;
		return;
	}
	
	DumpVariable(cmd, variable);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpParameter(const char *cmd, const char *name)
{
	VariableBase *variable = GetSimulator()->FindParameter(name);
	
	if (variable->IsVoid())
	{
		cout << "Unknow parameter (" << name << ")" << endl;
		return;
	}
	
	DumpVariable(cmd, variable);
}
	
template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetVariable(const char *name, const char *value)
{
	VariableBase *variable = GetSimulator()->FindVariable(name);
	
	if (variable->IsVoid())
	{
		cout << "Unknow variable (" << name << ")" << endl;
		return;
	}

	*(variable) = value;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProgramProfile()
{
	cout << "Program profile:" << endl;

	typename std::map<ADDRESS, uint64_t> map = program_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for(iter = map.begin(); iter != map.end(); iter++)
	{
		ADDRESS addr = (*iter).first;
		const Symbol<ADDRESS> *symbol = 0;
		
		unsigned int i;
		for(i = 0; (!symbol) && (i < num_loaders); i++)
		{
			if(*symbol_table_lookup_import[i])
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

		if(symbol)
		{
			if(symbol)
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
void InlineDebugger<ADDRESS>::DumpAvailableLoaders()
{
	if(num_loaders && loader_import)
	{
		unsigned int i;
		for(i = 0; i < num_loaders; i++)
		{
			ServiceImport<Loader<ADDRESS> > *import = loader_import[i];
			if(*import)
			{
				Object *service = import->GetService();
				if(service)
				{
					cout << service->GetName() << endl;
				}
			}
		}
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Load(const char *loader_name)
{
	if(num_loaders && loader_import)
	{
		unsigned int i;
		for(i = 0; i < num_loaders; i++)
		{
			ServiceImport<Loader<ADDRESS> > *import = loader_import[i];
			if(*import)
			{
				Object *service = import->GetService();
				if(service)
				{
					if(strcmp(service->GetName(), loader_name) == 0)
					{
						// Found loader
						if(!(*import)->Load())
						{
							cerr << Object::GetName() << ": ERROR! Loader \"" << loader_name << "\" was not able to load data/program" << endl;
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
void InlineDebugger<ADDRESS>::DumpDataProfile(bool write)
{
	cout << "Data " << (write ? "write" : "read") << " profile:" << endl;
	typename std::map<ADDRESS, uint64_t> map = write ? data_write_profile : data_read_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for(iter = map.begin(); iter != map.end(); iter++)
	{
		ADDRESS addr = (*iter).first;
		const Symbol<ADDRESS> *symbol = 0;
		
		unsigned int i;
		for(i = 0; (!symbol) && (i < num_loaders); i++)
		{
			if(*symbol_table_lookup_import[i])
			{
				symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
			}
		}
		
		cout << "0x" << hex;
		cout.fill('0');
		cout.width(8);
		cout << (addr / memory_atom_size) << dec;

		if(symbol)
		{
			if(symbol)
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
void InlineDebugger<ADDRESS>::DumpSource(const char *source_path, unsigned int lineno, unsigned int colno, unsigned int count)
{
	std::string s;
	const char *p;

	std::vector<std::string> search_paths;

	s.clear();
	p = search_path.c_str();
	do
	{
		if(*p == 0 || *p == ';')
		{
			search_paths.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	std::vector<std::string> hierarchical_source_path;
	
	s.clear();
	p = source_path;
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_source_path.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	bool match = false;
	std::string match_source_path;
	
	int hierarchical_source_path_depth = hierarchical_source_path.size();
	
	if(hierarchical_source_path_depth > 0)
	{
		int num_search_paths = search_paths.size();
		int k;
		
		for(k = 0; k < num_search_paths; k++)
		{
			const std::string& search_path = search_paths[k];
			int i;
			for(i = 0; (!match) && (i < hierarchical_source_path_depth); i++)
			{
				std::string try_source_path = search_path;
				int j;
				for(j = i; j < hierarchical_source_path_depth; j++)
				{
					if(!try_source_path.empty()) try_source_path += '/';
					try_source_path += hierarchical_source_path[j];
				}
				//std::cerr << "try_source_path=\"" << try_source_path << "\":";
				if(access(try_source_path.c_str(), R_OK) == 0)
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

	if(match)
	{
		std::cout << match_source_path;
	}
	else
	{
		std::cout << source_path << " (not found)";
	}
	
	std::cout << " at line #" << lineno;
	if(colno)
	{
		std::cout << ", column #" << colno;
	}
	cout << ": ";

	if(match)
	{
		std::ifstream f(match_source_path.c_str(), std::ifstream::in);
		std::string line;
		
		unsigned int n = 1;
		unsigned int printed = 0;
		do
		{
			getline(f, line);
			if(f.fail() || f.eof()) break;
			
			if((n >= lineno) && (n < (lineno + count)))
			{
				std::cout << line << std::endl;
				printed++;
			}
			n++;
		} while(printed < count);
	}
	else
	{
		std::cout << std::endl;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseAddrRange(const char *s, ADDRESS& addr, unsigned int& size)
{
	char fmt1[16];
	char fmt2[16];
	snprintf(fmt1, sizeof(fmt1), "*%%%s:%%%s", hex_addr_fmt, int_addr_fmt);
	snprintf(fmt2, sizeof(fmt2), "*%%%s:%%%s", int_addr_fmt, int_addr_fmt);

	if(sscanf(s, fmt1, &addr, &size) == 2 ||
	   sscanf(s, fmt2, &addr, &size) == 2)
	{
		addr *= memory_atom_size;
		size *= memory_atom_size;
		return true;
	}

	snprintf(fmt1, sizeof(fmt1), "*%%%s", hex_addr_fmt);
	snprintf(fmt2, sizeof(fmt2), "*%%%s", int_addr_fmt);

	if(sscanf(s, fmt1, &addr) == 1 ||
	   sscanf(s, fmt2, &addr) == 1)
	{
		addr *= memory_atom_size;
		size = memory_atom_size;
		return true;
	}

	const Symbol<ADDRESS> *symbol = 0;
	
	unsigned int i;
	for(i = 0; (!symbol) && (i < num_loaders); i++)
	{
		if(*symbol_table_lookup_import[i])
		{
			symbol = (*symbol_table_lookup_import[i])->FindSymbolByName(s);
		}
	}
	
	if(symbol)
	{
		addr = symbol->GetAddress();
		size = symbol->GetSize();
		if(size < 1) size = 1;
		return true;
	}
	
	return false;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseAddr(const char *s, ADDRESS& addr)
{
	char fmt1[16];
	char fmt2[16];
	snprintf(fmt1, sizeof(fmt1), "*%%%s", hex_addr_fmt);
	snprintf(fmt2, sizeof(fmt2), "*%%%s", int_addr_fmt);

	if(sscanf(s, fmt1, &addr) == 1 ||
	   sscanf(s, fmt2, &addr) == 1)
	{
		addr *= memory_atom_size;
		return true;
	}
	
	const Symbol<ADDRESS> *symbol = 0;
	
	unsigned int i;
	for(i = 0; (!symbol) && (i < num_loaders); i++)
	{
		if(*symbol_table_lookup_import[i])
		{
			symbol = (*symbol_table_lookup_import[i])->FindSymbolByName(s);
		}
	}

	if(symbol)
	{
		addr = symbol->GetAddress();
		return true;
	}
	
	std::string filename;
	unsigned int lineno;
	while(*s && *s != '#')
	{
		filename += *s;
		s++;
	}
	if(*s == '#') s++;
	if(sscanf(s, "%u", &lineno) != 1) return false;
	
	const Statement<ADDRESS> *stmt = 0;
	for(i = 0; (!stmt) && (i < num_loaders); i++)
	{
		if(*stmt_lookup_import[i])
		{
			stmt = (*stmt_lookup_import[i])->FindStatement(filename.c_str(), lineno, 0);
		}
	}
	
	if(stmt)
	{
		addr = stmt->GetAddress();
		return true;
	}
	
	return false;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::GetLine(char *line, int size)
{
#if defined(HAVE_LIBEDIT)
	char *line_read;
	do
	{
		line_read = readline(prompt.c_str());
		if(!line_read)
		{
			rl_redisplay(); // work around when terminal size changes
		}
			
	} while(!line_read);

  /* If the line has any text in it,
     save it on the history. */
	if(*line_read) add_history(line_read);

	strcpy(line, line_read);

	return true;
#else
	cout << prompt;
	cin.getline(line, size);
	if(cin.fail()) return false;
	return true;
#endif
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsBlankLine(const char *line)
{
	while(*line)
	{
		if(*line != ' ') return false;
		line++;
	}
	return true;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsQuitCommand(const char *cmd)
{
	return strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStepCommand(const char *cmd)
{
	return strcmp(cmd, "s") == 0 || strcmp(cmd, "si") == 0 || strcmp(cmd, "step") == 0 || strcmp(cmd, "stepi") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsNextCommand(const char *cmd)
{
	return strcmp(cmd, "n") == 0 || strcmp(cmd, "ni") == 0 || strcmp(cmd, "next") == 0 || strcmp(cmd, "nexti") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsContinueCommand(const char *cmd)
{
	return strcmp(cmd, "c") == 0 || strcmp(cmd, "cont") == 0 || strcmp(cmd, "continue") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDisasmCommand(const char *cmd)
{
	return strcmp(cmd, "dis") == 0 || strcmp(cmd, "disasm") == 0 || strcmp(cmd, "disassemble") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsBreakCommand(const char *cmd)
{
	return strcmp(cmd, "b") == 0 || strcmp(cmd, "break") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsWatchCommand(const char *cmd)
{
	return strcmp(cmd, "w") == 0 || strcmp(cmd, "watch") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDeleteCommand(const char *cmd)
{
	return strcmp(cmd, "del") == 0 || strcmp(cmd, "delete") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDeleteWatchCommand(const char *cmd)
{
	return strcmp(cmd, "delw") == 0 || strcmp(cmd, "delwatch") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDumpCommand(const char *cmd)
{
	return strcmp(cmd, "d") == 0 || strcmp(cmd, "dump") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsHelpCommand(const char *cmd)
{
	return strcmp(cmd, "h") == 0 || strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsResetCommand(const char *cmd)
{
	return strcmp(cmd, "r") == 0 || strcmp(cmd, "run") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsMonitorCommand(const char *cmd, const char *format)
{
	if (format)
	{
		return (strcmp(cmd, "m") == 0 || strcmp(cmd, "monitor") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "m") == 0 || strcmp(cmd, "monitor") == 0 
	|| strncmp(cmd, "m/", 2) == 0 || strncmp(cmd, "monitor/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsRegisterCommand(const char *cmd, const char *format)
{
	if (format)
	{
		return (strcmp(cmd, "reg") == 0 || strcmp(cmd, "register") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "reg") == 0 || strcmp(cmd, "register") == 0 
	|| strncmp(cmd, "reg/", 2) == 0 || strncmp(cmd, "register/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStatisticCommand(const char *cmd, const char *format)
{
	if (format)
	{
		return (strcmp(cmd, "stat") == 0 || strcmp(cmd, "statistic") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "stat") == 0 || strcmp(cmd, "statistic") == 0 
	|| strncmp(cmd, "stat/", 2) == 0 || strncmp(cmd, "statistic/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsParameterCommand(const char *cmd, const char *format)
{
	if (format)
	{
		return (strcmp(cmd, "param") == 0 || strcmp(cmd, "parameter") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "param") == 0 || strcmp(cmd, "parameter") == 0 
	|| strncmp(cmd, "param/", 2) == 0 || strncmp(cmd, "parameter/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsMonitorSetCommand(const char *cmd)
{
	return strcmp(cmd, "set") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsProfileCommand(const char *cmd)
{
	return strcmp(cmd, "p") == 0 || strcmp(cmd, "prof") == 0 || strcmp(cmd, "profile") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsLoadCommand(const char *cmd)
{
	return strcmp(cmd, "l") == 0 || strcmp(cmd, "ld") == 0 || strcmp(cmd, "load") == 0;
}

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
