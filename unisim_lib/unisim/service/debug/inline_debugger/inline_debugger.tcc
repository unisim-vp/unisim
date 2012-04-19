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
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <stdexcept>

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

template <class ADDRESS>
InlineDebugger<ADDRESS>::InlineDebugger(const char *_name, Object *_parent)
	: Object(_name, _parent, "this service implements a built-in debugger in the terminal console")
	, Service<DebugControl<ADDRESS> >(_name, _parent)
	, Service<DebugEventListener<ADDRESS> >(_name, _parent)
	, Service<TrapReporting>(_name, _parent)
	, Client<DebugEventTrigger<ADDRESS> >(_name, _parent)
	, Client<Disassembly<ADDRESS> >(_name, _parent)
	, Client<Memory<ADDRESS> >(_name, _parent)
	, Client<Registers>(_name, _parent)
	, Client<SymbolTableLookup<ADDRESS> >(_name, _parent)
	, Client<StatementLookup<ADDRESS> >(_name, _parent)
	, Client<BackTrace<ADDRESS> >(_name, _parent)
	, Client<Profiling<ADDRESS> >(_name, _parent)
	, Client<DebugInfoLoading>(_name, _parent)
	, InlineDebuggerBase()
	, debug_control_export("debug-control-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, trap_reporting_export("trap-reporting-export", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, backtrace_import("backtrace-import", this)
	, profiling_import("profiling-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, logger(*this)
	, memory_atom_size(1)
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, param_search_path("search-path", this, search_path, "Search path for source (separated by ';')")
	, param_init_macro("init-macro", this, init_macro, "path to initial macro to run when debugger starts")
	, param_output("output", this, output, "path to output file where to redirect the debugger outputs")
	, param_architecture_description_filename("architecture-description-filename", this, architecture_description_filename, "filename of a XML description of the connected processor")
	, running_mode(INLINE_DEBUGGER_MODE_WAITING_USER)
	, disasm_addr(0)
	, dump_addr(0)
	, cont_until_addr(0)
	, last_stmt(0)
	, prompt(string(_name) + "> ")
	, hex_addr_fmt(0)
	, int_addr_fmt(0)
	, output_stream(0)
	, std_output_stream(&std::cout)
	, std_error_stream(&std::cerr)
{
	trap = false;

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
}

template <class ADDRESS>
InlineDebugger<ADDRESS>::~InlineDebugger()
{
	if(output_stream)
	{
		delete output_stream;
	}
	
	free(hex_addr_fmt);
	free(int_addr_fmt);
}

template<class ADDRESS>
bool InlineDebugger<ADDRESS>::EndSetup()
{
	if(!output.empty())
	{
		output_stream = new std::ofstream(output.c_str(), std::ofstream::out);
		
		if(output_stream->fail())
		{
			cerr << "ERROR! Can't open output \"" << output << "\"" << endl;
			delete output_stream;
			output_stream = 0;
			return false;
		}
		
		std_output_stream = output_stream;
		std_error_stream = output_stream;
	}
	else
	{
		std_output_stream = &std::cout;
		std_error_stream = &std::cerr;
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
	
	if(!init_macro.empty())
	{
		LoadMacro(init_macro.c_str());
	}
	return true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event)
{
	switch(event.GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				const unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = (const unisim::util::debug::Breakpoint<ADDRESS> *) &event;
				(*std_output_stream) << "-> Reached " << (*breakpoint) << std::endl;
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				const unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = (const unisim::util::debug::Watchpoint<ADDRESS> *) &event;
				(*std_output_stream) << "-> Reached " << (*watchpoint) << std::endl;
			}
			break;
		default:
			// ignore event
			return;
	}
	
	trap = true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ReportTrap()
{
	trap = true;
	(*std_output_stream) << "-> Received trap" << endl;
}

template <class ADDRESS>
void 
InlineDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj)
{
	trap = true;
	(*std_output_stream) << "-> Received trap from \"" << obj.GetName() << "\"" << endl;
}
	
template <class ADDRESS>
void
InlineDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const std::string &str)
{
	trap = true;
	(*std_output_stream) << "-> Received trap from \"" << obj.GetName() << "\": " << str << endl;
}
	
template <class ADDRESS>
void 
InlineDebugger<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const char *c_str)
{
	trap = true;
	(*std_output_stream) << "-> Received trap from \"" << obj.GetName() << "\": " << c_str << endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Tokenize(const std::string& str, std::vector<std::string>& tokens)
{
	std::string token;
	std::stringstream sstr(str);

	while(sstr >> token) tokens.push_back(token);
}

template <class ADDRESS>
typename DebugControl<ADDRESS>::DebugCommand InlineDebugger<ADDRESS>::FetchDebugCommand(ADDRESS cia)
{
	bool recognized = false;
	ADDRESS addr;
	ADDRESS cont_addr;
	unsigned int size;
	std::vector<std::string> parm;

	if(!trap && (running_mode == INLINE_DEBUGGER_MODE_CONTINUE || running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL))
	{
		return DebugControl<ADDRESS>::DBG_STEP;
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL)
	{
		DeleteBreakpoint(cont_until_addr);
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_RESET)
	{
		running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
		return DebugControl<ADDRESS>::DBG_STEP;
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_STEP)
	{
		const Statement<ADDRESS> *stmt = FindStatement(cia);
		if(!stmt || (stmt == last_stmt)) return DebugControl<ADDRESS>::DBG_STEP;
	}

	int nparms = 0;

	Disasm(cia, 1);

	while(1)
	{
		trap = false;
		bool interactive = false;
		//(*std_output_stream) << "> ";
		if(!GetLine(prompt.c_str(), line, interactive))
		{
			Object::Stop(0);
			return DebugControl<ADDRESS>::DBG_KILL;
		}

		if(interactive && IsBlankLine(line))
		{
	 		line = last_line;
//			(*std_output_stream) << "\033[1A\033[2C" << line << endl;
		}
		
		parm.clear();
		Tokenize(line, parm);
		nparms = parm.size();
	
		recognized = false;

		switch(nparms)
		{
			case 0:
				if(!interactive) continue;
				break;
			case 1:
				{
					unisim::util::debug::Register *reg = registers_import->GetRegister(parm[0].c_str());

					if(reg)
					{
						recognized = true;
						unsigned int size = reg->GetSize();
						uint8_t value[size];
						reg->GetValue(&value);
						int i;
						(*std_output_stream) << reg->GetName() << " = 0x" << hex;
#if BYTE_ORDER == BIG_ENDIAN
						for(i = 0; i < (int) size; i++)
#else
						for(i = size - 1; i >= 0; i--)
#endif
						{
							(*std_output_stream) << (value[i] >> 4);
							(*std_output_stream) << (value[i] & 15);
						}
						(*std_output_stream) << dec << endl;
						break;
					}
				}

				if(IsHelpCommand(parm[0].c_str()))
				{
					recognized = true;
					Help();
					break;
				}

				if(IsQuitCommand(parm[0].c_str()))
				{
					recognized = true;
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}

				if(IsStepInstructionCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_STEP_INSTRUCTION;
					if(interactive) last_line = line;
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsStepCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_STEP;
					if(interactive) last_line = line;
					last_stmt = FindStatement(cia);
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsNextInstructionCommand(parm[0].c_str()))
				{
					if(interactive) last_line = line;
					disasm_import->Disasm(cia, cont_addr);
					if(HasBreakpoint(cont_addr))
					{
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					}
					else
					{
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
						cont_until_addr = cont_addr;
						SetBreakpoint(cont_until_addr);
					}
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsContinueCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					if(interactive) last_line = line;
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsDisasmCommand(parm[0].c_str()))
				{
					recognized = true;
					Disasm(disasm_addr, 10);
					disasm_addr += 10 * 4;
					break;
				}

				if(IsDumpCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpMemory(dump_addr);
					dump_addr += 256;
					break;
				}

				if(IsBreakCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpBreakpoints();
					break;
				}

				if(IsWatchCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpWatchpoints();
					break;
				}
				
				if(IsResetCommand(parm[0].c_str()))
				{
					trap = false;
					last_line.clear();
					line.clear();
					running_mode = INLINE_DEBUGGER_MODE_RESET;
					disasm_addr = 0;
					dump_addr = 0;
					return DebugControl<ADDRESS>::DBG_RESET;
				}

				if (IsMonitorCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m");
					break;
				}
				
				if (IsRegisterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m", 0, unisim::kernel::service::VariableBase::VAR_REGISTER);
					break;
				}
				
				if (IsStatisticCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m", 0, unisim::kernel::service::VariableBase::VAR_STATISTIC);
					break;
				}
				
				if (IsParameterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m", 0, unisim::kernel::service::VariableBase::VAR_PARAMETER);
					break;
				}

				if(IsProfileCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpProgramProfile();
					if(!trap) DumpDataProfile(false /* read */);
					if(!trap) DumpDataProfile(true /* write */);
					break;
				}

				if(IsBackTraceCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpBackTrace(cia);
					break;
				}
				
				if(IsListSymbolsCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpSymbols();
					break;
				}
				break;
			case 2:
				if(IsDisasmCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), disasm_addr))
				{
					recognized = true;
					Disasm(disasm_addr, 10);
					disasm_addr += 10 * 4;
					break;
				}

				if(IsDumpCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), dump_addr))
				{
					recognized = true;
					DumpMemory(dump_addr);
					dump_addr += 256;
					break;
				}

				if(IsEditCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), addr))
				{
					recognized = true;
					EditMemory(addr);
					break;
				}

				if(IsBreakCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), addr))
				{
					recognized = true;
					SetBreakpoint(addr);
					break;
				}

				if(IsWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					recognized = true;
					SetWriteWatchpoint(addr, size);
					break;
				}

				if(IsDeleteCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), addr))
				{
					recognized = true;
					DeleteBreakpoint(addr);
					break;
				}
				
				if(IsDeleteWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					recognized = true;
					DeleteWriteWatchpoint(addr, size);
					break;
				}

				if(IsContinueCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), cont_addr))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
					if(interactive) last_line = line;
					cont_until_addr = cont_addr;
					SetBreakpoint(cont_until_addr);
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				
				if (IsMonitorCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str());
					break;
				}
				
				if (IsRegisterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str(), unisim::kernel::service::VariableBase::VAR_REGISTER);
					break;
				}
				
				if (IsStatisticCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str(), unisim::kernel::service::VariableBase::VAR_STATISTIC);
					break;
				}
				
				if (IsParameterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str(), unisim::kernel::service::VariableBase::VAR_PARAMETER);
					break;
				}

				if(IsProfileCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "on") == 0)
					{
						recognized = true;
						EnableProfiling();
						break;
					}
					else if(strcmp(parm[1].c_str(), "off") == 0)
					{
						recognized = true;
						DisableProfiling();
						break;
					}
					else if(strcmp(parm[1].c_str(), "clear") == 0)
					{
						recognized = true;
						ClearProfile();
						break;
					}
					else if(strcmp(parm[1].c_str(), "status") == 0)
					{
						recognized = true;
						DumpProfilingStatus();
						break;
					}
					else if((strcmp(parm[1].c_str(), "prog") == 0) || (strcmp(parm[1].c_str(), "program") == 0))
					{
						recognized = true;
						DumpProgramProfile();
						break;
					}
					else if(strcmp(parm[1].c_str(), "data") == 0)
					{
						recognized = true;
						DumpDataProfile(false /* read */);
						DumpDataProfile(true /* write */);
						break;
					}
				}

				if(IsLoadSymbolTableCommand(parm[0].c_str()))
				{
					recognized = true;
					LoadSymbolTable(parm[1].c_str());
					break;
				}

				if(IsListSymbolsCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpSymbols(parm[1].c_str());
					break;
				}

				if(IsMacroCommand(parm[0].c_str()))
				{
					recognized = true;
					LoadMacro(parm[1].c_str());
					break;
				}
				break;
			case 3:
				if(IsWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					if(strcmp(parm[2].c_str(), "read") == 0)
					{
						recognized = true;
						SetReadWatchpoint(addr, size);
					}
					if(strcmp(parm[2].c_str(), "write") == 0)
					{
						recognized = true;
						SetWriteWatchpoint(addr, size);
					}
					break;
				}
				
				if(IsDeleteWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					if(strcmp(parm[2].c_str(), "read") == 0)
					{
						recognized = true;
						DeleteReadWatchpoint(addr, size);
					}
					if(strcmp(parm[2].c_str(), "write") == 0)
					{
						recognized = true;
						DeleteWriteWatchpoint(addr, size);
					}
					break;
				}
				
				if(IsProfileCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "data") == 0)
					{
						if(strcmp(parm[2].c_str(), "on") == 0)
						{
							recognized = true;
							EnableDataProfiling();
						}
						else if(strcmp(parm[2].c_str(), "off") == 0)
						{
							recognized = true;
							DisableDataProfiling();
						}
						else if(strcmp(parm[2].c_str(), "clear") == 0)
						{
							recognized = true;
							ClearDataProfile();
						}
						else if(strcmp(parm[2].c_str(), "status") == 0)
						{
							recognized = true;
							DumpDataProfilingStatus();
							break;
						}
						else if(strcmp(parm[2].c_str(), "read") == 0)
						{
							recognized = true;
							DumpDataProfile(false /* read */);
						}
						else if(strcmp(parm[2].c_str(), "write") == 0)
						{
							recognized = true;
							DumpDataProfile(true /* write */);
						}
					}
					else if((strcmp(parm[1].c_str(), "prog") == 0) || (strcmp(parm[1].c_str(), "program") == 0))
					{
						if(strcmp(parm[2].c_str(), "on") == 0)
						{
							recognized = true;
							EnableProgramProfiling();
						}
						else if(strcmp(parm[2].c_str(), "off") == 0)
						{
							recognized = true;
							DisableProgramProfiling();
						}
						else if(strcmp(parm[2].c_str(), "clear") == 0)
						{
							recognized = true;
							ClearProgramProfile();
						}
						else if(strcmp(parm[2].c_str(), "status") == 0)
						{
							recognized = true;
							DumpProgramProfilingStatus();
							break;
						}
					}
				}

				if (IsMonitorCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str());
					break;
				}
				
				if (IsRegisterCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str(), unisim::kernel::service::VariableBase::VAR_REGISTER);
					break;
				}
				
				if (IsStatisticCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str(), unisim::kernel::service::VariableBase::VAR_STATISTIC);
					break;
				}
				
				if (IsParameterCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str(), unisim::kernel::service::VariableBase::VAR_PARAMETER);
					break;
				}

				break;

			case 4:
				
				if(IsProfileCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "data") == 0)
					{
						if(strcmp(parm[2].c_str(), "read") == 0)
						{
							if(strcmp(parm[3].c_str(), "on") == 0)
							{
								recognized = true;
								EnableDataReadProfiling();
							}
							else if(strcmp(parm[3].c_str(), "off") == 0)
							{
								recognized = true;
								DisableDataReadProfiling();
							}
							else if(strcmp(parm[3].c_str(), "clear") == 0)
							{
								recognized = true;
								ClearDataReadProfile();
							}
							else if(strcmp(parm[3].c_str(), "status") == 0)
							{
								recognized = true;
								DumpDataReadProfilingStatus();
								break;
							}
						}
						else if(strcmp(parm[2].c_str(), "write") == 0)
						{
							if(strcmp(parm[3].c_str(), "on") == 0)
							{
								recognized = true;
								EnableDataWriteProfiling();
							}
							else if(strcmp(parm[3].c_str(), "off") == 0)
							{
								recognized = true;
								DisableDataWriteProfiling();
							}
							else if(strcmp(parm[3].c_str(), "clear") == 0)
							{
								recognized = true;
								ClearDataWriteProfile();
							}
							else if(strcmp(parm[3].c_str(), "status") == 0)
							{
								recognized = true;
								DumpDataWriteProfilingStatus();
								break;
							}
						}
					}
				}
				break;
			case EOF:
				recognized = true;
				break;

		} // end of switch

		if (nparms >= 3 && nparms <= 33)
		{
			if (IsMonitorSetCommand(parm[0].c_str()))
			{
				recognized = true;
				std::stringstream str;
				for (int i = 2; i < nparms; i++)
				{
					if (i > 2) str << " ";
						str << parm[i];
				}
				SetVariable(parm[1].c_str(), str.str().c_str());
			}
		}

		if(!recognized)
		{
			(*std_output_stream) << "Unrecognized command.  Try \"help\"." << endl;
			for (int i = 0; i < nparms; i++)
				(*std_output_stream) << parm[i] << " ";
			(*std_output_stream) << endl;
		}
		
		if(interactive) last_line = line;
	}
	Object::Stop(0);
	return DebugControl<ADDRESS>::DBG_KILL;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Help()
{
	(*std_output_stream) << "HELP:" << endl;
	(*std_output_stream) << "================================ LOAD ==========================================" << endl;
	(*std_output_stream) << "<st | symtab> [<filename>]" << endl;
	(*std_output_stream) << "    load symbol table from file <filename>" << endl;
	(*std_output_stream) << "=============================== SCRIPTING ======================================" << endl;
	(*std_output_stream) << "<mac | macro> <filename>" << endl;
	(*std_output_stream) << "    run debugger commands from file named 'filename'" << endl;
	(*std_output_stream) << "================================ EXECUTE =======================================" << endl;
	(*std_output_stream) << "<c | cont | continue> [<symbol | *address>]" << endl;
	(*std_output_stream) << "    continue to execute instructions until program reaches a breakpoint," << endl;
	(*std_output_stream) << "    a watchpoint, 'symbol' or 'address'" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<si | stepi>" << endl;
	(*std_output_stream) << "    execute one instruction" << endl;
	(*std_output_stream) << "<s | step>" << endl;
	(*std_output_stream) << "    continue executing instructions until control reaches a different source line" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<ni | nexti>" << endl;
	(*std_output_stream) << "    continue to execute instructions until the processor reaches" << endl;
	(*std_output_stream) << "    next contiguous instruction, a breakpoint or a watchpoint" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<r | run>" << endl;
	(*std_output_stream) << "    restart the simulation from the beginning" << endl;
	(*std_output_stream) << "================================ INSPECT =======================================" << endl;
	(*std_output_stream) << "<dis | disasm | disassemble> [<symbol | *address>]" << endl;
	(*std_output_stream) << "    continue to disassemble starting from 'symbol', 'address', or after" << endl;
	(*std_output_stream) << "    the previous disassembly" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<d | dump> [<symbol | *address>]" << endl;
	(*std_output_stream) << "    dump memory starting from 'symbol', 'address', or after the previous dump" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<e | ed | edit> <symbol | *address>" << endl;
	(*std_output_stream) << "    edit data memory starting from 'symbol' or 'address'" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<register name>" << endl;
	(*std_output_stream) << "    display the register value" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<variable name | hierarchical variable name>" << endl;
	(*std_output_stream) << "    display all the objects variables that are named as the given variable name," << endl;
	(*std_output_stream) << "    or that contain part of the given name" << endl;
	(*std_output_stream) << "    note: this command is overriden if the given name matches any of the other" << endl;
	(*std_output_stream) << "    inline debugger commands" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<m | monitor>[/<format>] [<variable name>]" << endl;
	(*std_output_stream) << "<reg | register>[/<format>] [<register name>]" << endl;
	(*std_output_stream) << "<param | parameter>[/<format>] [<parameter name>]" << endl;
	(*std_output_stream) << "<stat | statistic>[/<format>] [<statistic name>]" << endl;
	(*std_output_stream) << "    display the given simulator variable (register/parameter/statistic), or " << endl;
	(*std_output_stream) << "    display all variable (register/parameter/statistic) names if none is given" << endl;
	(*std_output_stream) << "    when giving a variable name a format can be used, the available formats are:" << endl;
	(*std_output_stream) << "     x     print variable as an integer in hexadecimal format" << endl;
	(*std_output_stream) << "     d     print variable as a signed integer" << endl;
	(*std_output_stream) << "     u     print variable as an unsigned integer" << endl;
	(*std_output_stream) << "     o     print variable as an integer in octal format" << endl;
	(*std_output_stream) << "     t     print variable as an integer in binary format" << endl;
	(*std_output_stream) << "     a     print variable as an address (equivalent to 'x')" << endl;
	(*std_output_stream) << "     c     print variable as a string" << endl;
	(*std_output_stream) << "     f     print variable as a float (actually is considered as a double)" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "set <variable name> <value>" << endl;
	(*std_output_stream) << "    sets the variable (register/parameter/statistic) to the given value" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<sym | symbol> [<symbol name>]" << endl;
	(*std_output_stream) << "    Display information about symbols having 'symbol name' in their name" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<p | prof | profile> on" << endl;
	(*std_output_stream) << "<p | prof | profile> prog on" << endl;
	(*std_output_stream) << "<p | prof | profile> program on" << endl;
	(*std_output_stream) << "<p | prof | profile> data on" << endl;
	(*std_output_stream) << "<p | prof | profile> data read on" << endl;
	(*std_output_stream) << "<p | prof | profile> data write on" << endl;
	(*std_output_stream) << "<p | prof | profile> off" << endl;
	(*std_output_stream) << "<p | prof | profile> prog off" << endl;
	(*std_output_stream) << "<p | prof | profile> program off" << endl;
	(*std_output_stream) << "<p | prof | profile> data off" << endl;
	(*std_output_stream) << "<p | prof | profile> data read off" << endl;
	(*std_output_stream) << "<p | prof | profile> data write off" << endl;
	(*std_output_stream) << "    enable/disable the program and data profiling" << endl;
	(*std_output_stream) << "<p | prof | profile> status" << endl;
	(*std_output_stream) << "<p | prof | profile> prog status" << endl;
	(*std_output_stream) << "<p | prof | profile> program status" << endl;
	(*std_output_stream) << "<p | prof | profile> data status" << endl;
	(*std_output_stream) << "<p | prof | profile> data read status" << endl;
	(*std_output_stream) << "<p | prof | profile> data write status" << endl;
	(*std_output_stream) << "    display the program and data profiling status (on/off)" << endl;
	(*std_output_stream) << "<p | prof | profile> clear" << endl;
	(*std_output_stream) << "<p | prof | profile> prog clear" << endl;
	(*std_output_stream) << "<p | prof | profile> program clear" << endl;
	(*std_output_stream) << "<p | prof | profile> data clear" << endl;
	(*std_output_stream) << "<p | prof | profile> data read clear" << endl;
	(*std_output_stream) << "<p | prof | profile> data write clear" << endl;
	(*std_output_stream) << "    clear the collected program and data profiles" << endl;
	(*std_output_stream) << "<p | prof | profile>" << endl;
	(*std_output_stream) << "<p | prof | profile> prog" << endl;
	(*std_output_stream) << "<p | prof | profile> program" << endl;
	(*std_output_stream) << "<p | prof | profile> data" << endl;
	(*std_output_stream) << "<p | prof | profile> data read" << endl;
	(*std_output_stream) << "<p | prof | profile> data write" << endl;
	(*std_output_stream) << "    display the program/data profile" << endl;
	(*std_output_stream) << "========================= BREAKPOINTS/WATCHPOINTS ==============================" << endl;
	(*std_output_stream) << "<b | break> [<symbol | *address | filename#lineno>]" << endl;
	(*std_output_stream) << "    set a breakpoint at 'symbol', 'address', or 'filename#lineno'. If 'symbol', 'address'," << endl;
	(*std_output_stream) << "    or 'filename#lineno' are not specified, display the breakpoint list" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<w | watch> [<symbol | *address[:<size>]>] [<read | write>]" << endl;
	(*std_output_stream) << "    set a watchpoint at 'symbol' or 'address'." << endl;
	(*std_output_stream) << "    When using 'continue' and 'next' commands, the debugger will spy CPU loads" << endl;
	(*std_output_stream) << "    and stores. The debugger will return to command line prompt once a load," << endl;
	(*std_output_stream) << "    or a store will access to 'symbol' or 'address'." << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<del | delete> <symbol | *address | filename#lineno>" << endl;
	(*std_output_stream) << "    delete the breakpoint at 'symbol', 'address', or 'filename#lineno'" << endl << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<delw | delwatch> <symbol | *address> [<read | write>] [<size>]" << endl;
	(*std_output_stream) << "    delete the watchpoint at 'symbol' or 'address'" << endl;
	(*std_output_stream) << "============================== MISCELLANEOUS ===================================" << endl;
	(*std_output_stream) << "<h | ? | help>" << endl;
	(*std_output_stream) << "    display the help" << endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << endl;
	(*std_output_stream) << "<quit | q>" << endl;
	(*std_output_stream) << "    quit the built-in debugger" << endl << endl;
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
			const Statement<ADDRESS> *stmt = FindStatement(addr);
			
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
			
			(*std_output_stream).fill('0');
			const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
			string s = disasm_import->Disasm(addr, next_addr);
			
			if(symbol)
			{
				if(symbol != last_symbol)
				{
					(*std_output_stream) << "0x" << hex;
					(*std_output_stream).width(8);
					(*std_output_stream) << (addr / memory_atom_size) << dec;
					(*std_output_stream) << " <";
					(*std_output_stream) << symbol->GetFriendlyName(addr);
					
					(*std_output_stream) << ">:" << endl;
					
					last_symbol = symbol;
				}
			}
			else
			{
				if(symbol != last_symbol)
				{
					(*std_output_stream) << "?";
					last_symbol = symbol;
				}
			}
			(*std_output_stream) << "0x" << hex;
			(*std_output_stream).width(8);
			(*std_output_stream) << (addr / memory_atom_size) << ":" << dec << s << endl;
			(*std_output_stream).fill(' ');
		} while(addr = next_addr, --count > 0);
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::HasBreakpoint(ADDRESS addr)
{
	return debug_event_trigger_import ? debug_event_trigger_import->IsEventListened(unisim::util::debug::Breakpoint<ADDRESS>(addr)) : false;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	if(!debug_event_trigger_import || !debug_event_trigger_import->Listen(unisim::util::debug::Breakpoint<ADDRESS>(addr)))
	{
		(*std_output_stream) << "Can't set breakpoint at 0x" << hex << addr << dec << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetReadWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!debug_event_trigger_import || !debug_event_trigger_import->Listen(unisim::util::debug::Watchpoint<ADDRESS>(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)))
	{
		(*std_output_stream) << "Can't set watchpoint at 0x" << hex << addr << dec << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetWriteWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!debug_event_trigger_import || !debug_event_trigger_import->Listen(unisim::util::debug::Watchpoint<ADDRESS>(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)))
	{
		(*std_output_stream) << "Can't set watchpoint at 0x" << hex << addr << dec << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteBreakpoint(ADDRESS addr)
{
	if(!debug_event_trigger_import || !debug_event_trigger_import->Unlisten(unisim::util::debug::Breakpoint<ADDRESS>(addr)))
	{
		(*std_output_stream) << "Can't remove breakpoint at 0x" << hex << addr << dec << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteReadWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!debug_event_trigger_import || !debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)))
	{
		(*std_output_stream) << "Can't remove read watchpoint at 0x" << hex << addr << dec << " (" << size << " bytes)" << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteWriteWatchpoint(ADDRESS addr, uint32_t size)
{
	if(!debug_event_trigger_import || !debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)))
	{
		(*std_output_stream) << "Can't remove write watchpoint at 0x" << hex << addr << dec << " (" << size << " bytes)" << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpBreakpoints()
{
	std::list<const Event<ADDRESS> *> events;
	debug_event_trigger_import->EnumerateListenedEvents(events, unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT);
	typename list<const Event<ADDRESS> *>::const_iterator iter;

	for(iter = events.begin(); iter != events.end(); iter++)
	{
		const Event<ADDRESS> *event = *iter;
		const Breakpoint<ADDRESS> *brkp = (const Breakpoint<ADDRESS> *) event;
		ADDRESS addr = brkp->GetAddress();
		
		(*std_output_stream) << "*0x" << hex << (addr / memory_atom_size) << dec << " (";
		
		const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
		
		if(symbol)
		{
			(*std_output_stream) << symbol->GetFriendlyName(addr);
		}
		else
		{
			(*std_output_stream) << "?";
		}
		(*std_output_stream) << ")";
		
		const Statement<ADDRESS> *stmt = FindStatement(addr);
		
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
				(*std_output_stream) << " in ";
				DumpSource(source_path.c_str(), lineno, colno, 1);
			}
		}

		(*std_output_stream) << endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpWatchpoints()
{
	std::list<const Event<ADDRESS> *> events;
	debug_event_trigger_import->EnumerateListenedEvents(events, unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT);
	typename list<const Event<ADDRESS> *>::const_iterator iter;

	for(iter = events.begin(); iter != events.end(); iter++)
	{
		const Event<ADDRESS> *event = *iter;
		const Watchpoint<ADDRESS> *wt = (const Watchpoint<ADDRESS> *) event;
		ADDRESS addr = wt->GetAddress();
		uint32_t size = wt->GetSize();
		typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat = wt->GetMemoryAccessType();
		typename MemoryAccessReporting<ADDRESS>::MemoryType mt = wt->GetMemoryType();
		
		switch(mt)
		{
			case MemoryAccessReporting<ADDRESS>::MT_INSN:
				(*std_output_stream) << "insn"; // it should never occur
				break;
			case MemoryAccessReporting<ADDRESS>::MT_DATA:
				(*std_output_stream) << "data";
				break;
		}
		(*std_output_stream) << " ";
		switch(mat)
		{
			case MemoryAccessReporting<ADDRESS>::MAT_READ:
				(*std_output_stream) << " read";
				break;
			case MemoryAccessReporting<ADDRESS>::MAT_WRITE:
				(*std_output_stream) << "write";
				break;
			default:
				(*std_output_stream) << "  (?)";
				break;
		}
		(*std_output_stream) << " ";
		
		(*std_output_stream) << "*0x" << hex << (addr / memory_atom_size) << dec << ":" << (size / memory_atom_size) << " (";
		
		const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
		
		if(symbol)
		{
			(*std_output_stream) << symbol->GetFriendlyName(addr);
		}
		else
		{
			(*std_output_stream) << "?";
		}
		(*std_output_stream) << ")" << endl;
	}
}


template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpMemory(ADDRESS addr)
{
	unsigned int i, j;
	streamsize width = (*std_output_stream).width();

	(*std_output_stream).fill(' ');
	(*std_output_stream).width(2 * sizeof(addr));
	(*std_output_stream) << "address" << hex;
	(*std_output_stream).width(0);
	for(i = 0; i < 16 / memory_atom_size; i++)
	{
		(*std_output_stream) << "  " << i;
		for(j = 1; j < memory_atom_size; j++) (*std_output_stream) << "   ";
	}
	(*std_output_stream) << endl;
	for(i = 0; i < 16; i++)
	{
		(*std_output_stream) << hex; (*std_output_stream).fill('0'); (*std_output_stream).width(2 * sizeof(addr)); (*std_output_stream) << (addr / memory_atom_size) << " "; (*std_output_stream).fill(' ');
		for(j = 0; j < 16; j++, addr++)
		{
			uint8_t value = 0;
			memory_import->ReadMemory(addr, &value, 1);
			(*std_output_stream) << (uint32_t)(value >> 4);
			(*std_output_stream) << (uint32_t)(value & 15);
			if(j < 15) (*std_output_stream) << " ";
		}
		addr -= 16;
		(*std_output_stream) << dec << "  ";
		for(j = 0; j < 16; j++, addr++)
		{
			uint8_t value = 0;
			memory_import->ReadMemory(addr, &value, 1);
			(*std_output_stream) << (char)((value >= ' ' && value < 128) ? value : '.');
		}
		(*std_output_stream) << endl;
	}
	(*std_output_stream).width(width);
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::EditMemory(ADDRESS addr)
{
	unsigned int written = 0;
	unsigned int failed = 0;
	std::string line;

	(*std_output_stream) << "Entering data memory edit mode." << std::endl;
	do
	{
		std::stringstream sstr;
		sstr << "0x" << std::hex;
		sstr.fill('0');
		sstr.width(2 * sizeof(addr));
		sstr << addr;
		sstr << ": ";
		
		bool interactive = false;
		if(!GetLine(sstr.str().c_str(), line, interactive))
		{
			return false;
		}
	
		if(IsBlankLine(line)) break;

		const char *ptr = line.c_str();
		char *endptr = 0;
		
		do
		{
			uint8_t value = strtoul(ptr, &endptr, 0);
			
			if(ptr == endptr) break;
		
			if(memory_import->WriteMemory(addr, &value, 1))
				written++;
			else
				failed++;
			
			addr++;
			ptr = endptr;
		}
		while(1);
	}
	while(1);
	(*std_output_stream) << "Leaving data memory edit mode." << std::endl;
	(*std_output_stream) << written << " of " << (failed + written) << " bytes written" << std::endl;
	return true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpVariables(const char *cmd, const char *name, typename unisim::kernel::service::VariableBase::Type type)
{
	bool found = false;
	list<VariableBase *> lst;
	list<VariableBase *>::iterator iter;

	GetSimulator()->GetVariables(lst, type);
	
	if(!lst.size()) return;
	
	for(iter = lst.begin(); iter != lst.end(); iter++)
	{
		VariableBase *var = *iter;
		std::string var_name(var->GetName());
		
		if(!name || ((*name == '~') && var_name.find(name + 1, 0, strlen(name + 1)) != string::npos) || (var_name.compare(name) == 0))
		{
			DumpVariable(cmd, var);
			found = true;
		}
	}
	
	if(!found)
	{
		(*std_output_stream) << "unknown ";
		switch(type)
		{
			case unisim::kernel::service::VariableBase::VAR_REGISTER:
				(*std_output_stream) << "register";
				break;
			case unisim::kernel::service::VariableBase::VAR_STATISTIC:
				(*std_output_stream) << "statistic";
				break;
			case unisim::kernel::service::VariableBase::VAR_PARAMETER:
				(*std_output_stream) << "parameter";
				break;
			default:
				(*std_output_stream) << "variable";
				break;
		}
		(*std_output_stream) << " \"" << ((*name == '~') ? name + 1 : name) << "\"" << std::endl;
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
	(*std_error_stream) << "WARNING: unknow monitor/register/statistic/parameter format (" << cmd << "). See help for available formats." << endl;
	format = 'c';
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpVariable(const char *cmd, const VariableBase *variable)
{
	switch (variable->GetType())
	{
		case VariableBase::VAR_ARRAY:
			(*std_output_stream) << " A";
			break;
		case VariableBase::VAR_PARAMETER:
			(*std_output_stream) << " P";
			break;
		case VariableBase::VAR_STATISTIC:
			(*std_output_stream) << " S";
			break;
		case VariableBase::VAR_FORMULA:
			(*std_output_stream) << " F";
			break;
		case VariableBase::VAR_REGISTER:
			(*std_output_stream) << " R";
			break;
		default:
			(*std_output_stream) << " ?";
			break;
	}

	(*std_output_stream) << "\t";
	/* extract the format if any */
	char format = 0;
	MonitorGetFormat(cmd, format);
	
	(*std_output_stream) << variable->GetName() << " = ";
	switch (format)
	{
		case 'x':
			(*std_output_stream) << "0x" << hex << ((unsigned long long) *variable) << dec;
			break;
		case 'd':
			(*std_output_stream) << ((long long) *variable);
			break;
		case 'u':
			(*std_output_stream) << ((unsigned long long) *variable);
			break;
		case 'o':
			(*std_output_stream).setf(std::ios::oct);
			(*std_output_stream) << "0" << ((unsigned long long) *variable);
			(*std_output_stream).unsetf(std::ios::oct);
			break;
		case 't':
			(*std_output_stream) << "0b";
			for (unsigned int i = 0; i < sizeof(unsigned long long) * 8; i++)
				(*std_output_stream) << (int)((((unsigned long long) *variable) >> i) & 1);
			break;
		case 'a':
			(*std_output_stream) << "0x" << hex << ((unsigned long long) *variable) << dec;
			break;
		case 'c':
			(*std_output_stream) << ((string) *variable);
			break;
		case 'f':
			(*std_output_stream) << ((double) *variable);
			break;
	}
	
	(*std_output_stream) << endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpSymbols(const typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& symbols, const char *name)
{
	bool found = false;
	
	typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>::const_iterator symbol_iter;
	for(symbol_iter = symbols.begin(); symbol_iter != symbols.end(); symbol_iter++)
	{
		const unisim::util::debug::Symbol<ADDRESS> *symbol = *symbol_iter;
		
		std::string symbol_name(symbol->GetName());
		
		if(!name || ((*name == '~') && symbol_name.find(name + 1, 0, strlen(name + 1)) != string::npos) || (symbol_name.compare(name) == 0))
		{
			if(!found)
			{
				// print header of table
				(*std_output_stream) << "  ";
				(*std_output_stream).fill(' ');
				(*std_output_stream).width(2 * sizeof(ADDRESS));
				(*std_output_stream) << "Address";
				(*std_output_stream).fill(' ');
				(*std_output_stream).width(1 + (2 * sizeof(ADDRESS)));
				(*std_output_stream) << "Size";
				(*std_output_stream).width(10);
				(*std_output_stream) << "Type";
				(*std_output_stream).width(0);
				(*std_output_stream) << " Name";
				(*std_output_stream) << std::endl;
			}
			
			std::string type_name;
			
			switch(symbol->GetType())
			{
				case unisim::util::debug::Symbol<ADDRESS>::SYM_NOTYPE:
					type_name = "no type";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_OBJECT:
					type_name = "object";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC:
					type_name = "function";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_SECTION:
					type_name = "section";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_FILE:
					type_name = "file";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_COMMON:
					type_name = "common";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_TLS:
					type_name = "tls";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_NUM:
					type_name = "num";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_LOOS:
					type_name = "loos";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_HIOS:
					type_name = "hios";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_LOPROC:
					type_name = "loproc";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_HIPROC:
					type_name = "hiproc";
					break;
			}
			
			(*std_output_stream) << std::hex << "0x";
			(*std_output_stream).fill('0');
			(*std_output_stream).width(2 * sizeof(ADDRESS));
			(*std_output_stream) << (symbol->GetAddress() / memory_atom_size) << std::dec;
			(*std_output_stream).fill(' ');
			(*std_output_stream).width(1 + (2 * sizeof(ADDRESS)));
			(*std_output_stream) << (symbol->GetSize() / memory_atom_size);
			(*std_output_stream).width(10);
			(*std_output_stream) << type_name;
			(*std_output_stream).width(0);
			(*std_output_stream) << " " << symbol->GetName();
			(*std_output_stream) << std::endl;
			
			found = true;
		}
	}
	
	if(!found)
	{
		(*std_output_stream) << "unknown symbol \"" << ((*name == '~') ? name + 1 : name) << "\"" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpSymbols(const char *name)
{
	unsigned int i;
	typename unisim::util::debug::Symbol<ADDRESS>::Type types[] = {
		unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC,
		unisim::util::debug::Symbol<ADDRESS>::SYM_OBJECT
	};
	
	typename std::list<const unisim::util::debug::Symbol<ADDRESS> *> symbols;
	
	for(i = 0; i < (sizeof(types) / sizeof(types[0])); i++)
	{
		typename unisim::util::debug::Symbol<ADDRESS>::Type type = types[i];

		if(symbol_table_lookup_import)
		{
			symbol_table_lookup_import->GetSymbols(symbols, type);
		}
	}
	
	if(!symbols.empty())
	{
		DumpSymbols(symbols, name);
	}
}


template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetVariable(const char *name, const char *value)
{
	VariableBase *variable = GetSimulator()->FindVariable(name);
	
	if (variable->IsVoid())
	{
		(*std_output_stream) << "Unknow variable (" << name << ")" << endl;
		return;
	}

	*(variable) = value;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProgramProfile()
{
	if(!profiling_import) return;
	(*std_output_stream) << "Program profile:" << endl;

	const unisim::util::debug::Profile<ADDRESS>& program_profile = profiling_import->GetProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC);
	typename std::map<ADDRESS, uint64_t> map = program_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for(iter = map.begin(); !trap && iter != map.end(); iter++)
	{
		ADDRESS addr = (*iter).first;
		const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
		
		ADDRESS next_addr;

		string s = disasm_import->Disasm(addr, next_addr);

		(*std_output_stream) << "0x" << hex;
		(*std_output_stream).fill('0');
		(*std_output_stream).width(8);
		(*std_output_stream) << (addr / memory_atom_size) << dec;

		if(symbol)
		{
			if(symbol)
			{
				(*std_output_stream) << " <";
				(*std_output_stream) << symbol->GetFriendlyName(addr);
				(*std_output_stream) << ">";
			}
		}
		else
		{
			(*std_output_stream) << " <?>";
		}

		(*std_output_stream).fill(' ');
		(*std_output_stream).width(0);
		(*std_output_stream) << ":" << (*iter).second << " times:" << s << endl;
	}
}

template <class ADDRESS>
std::string InlineDebugger<ADDRESS>::SearchFile(const char *filename)
{
	if(access(filename, F_OK) == 0)
	{
		return std::string(filename);
	}

	std::string s;
	const char *p;

	p = search_path.c_str();
	do
	{
		if(*p == 0 || *p == ';')
		{
			std::stringstream sstr;
			sstr << s << "/" << filename;
			std::string path(sstr.str());
			if(access(path.c_str(), F_OK) == 0)
			{
				return path;
			}
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	return Object::GetSimulator()->SearchSharedDataFile(filename);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::LoadSymbolTable(const char *filename)
{
	if(!debug_info_loading_import) return;

	std::string match_file_path;
	if(LocateFile(filename, match_file_path))
	{
		if(debug_info_loading_import->LoadDebugInfo(match_file_path.c_str()))
		{
			(*std_output_stream) << "Symbols from \"" << filename << "\" loaded" << std::endl;
		}
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::LoadMacro(const char *filename)
{
	std::string path(SearchFile(filename));
	
	std::ifstream f(path.c_str(), std::ifstream::in);
	
	if(f.fail())
	{
		(*std_error_stream) << "WARNING! Can't open macro file \"" << path << "\"" << std::endl;
		return;
	}
	
	std::stack<std::string> macro;
	std::string line;
		
	do
	{
		getline(f, line);
		if(f.bad())
		{
			(*std_error_stream) << "WARNING! I/O error while reading file \"" << path << "\"" << std::endl;
			return;
		}
		if (f.eof()) break;
		macro.push(line);
	} while(!f.eof());
	
	while(!macro.empty())
	{
		const std::string& s = macro.top();
		exec_queue.push_front(s);
		macro.pop();
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataProfile(bool write)
{
	if(!profiling_import) return;
	(*std_output_stream) << "Data " << (write ? "write" : "read") << " profile:" << endl;
	const unisim::util::debug::Profile<ADDRESS>& data_profile = profiling_import->GetProfile(write ? unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE : unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ);
	typename std::map<ADDRESS, uint64_t> map = data_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for(iter = map.begin(); !trap && iter != map.end(); iter++)
	{
		ADDRESS addr = (*iter).first;
		const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
		
		(*std_output_stream) << "0x" << hex;
		(*std_output_stream).fill('0');
		(*std_output_stream).width(8);
		(*std_output_stream) << (addr / memory_atom_size) << dec;

		if(symbol)
		{
			if(symbol)
			{
				(*std_output_stream) << " <";
				(*std_output_stream) << symbol->GetFriendlyName(addr);
				(*std_output_stream) << ">";
			}
		}
		else
		{
			(*std_output_stream) << " <?>";
		}

		(*std_output_stream).fill(' ');
		(*std_output_stream).width(0);
		(*std_output_stream) << ":" << (*iter).second << " times" << endl;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::LocateFile(const char *filename, std::string& match_file_path)
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
	
	std::vector<std::string> hierarchical_path;
	
	s.clear();
	p = filename;
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_path.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	bool match = false;
	
	int hierarchical_path_depth = hierarchical_path.size();
	
	if(hierarchical_path_depth > 0)
	{
		int num_search_paths = search_paths.size();
		int k;
		
		for(k = 0; k < num_search_paths; k++)
		{
			const std::string& search_path = search_paths[k];
			int i;
			for(i = 0; (!match) && (i < hierarchical_path_depth); i++)
			{
				std::string try_file_path = search_path;
				int j;
				for(j = i; j < hierarchical_path_depth; j++)
				{
					if(!try_file_path.empty()) try_file_path += '/';
					try_file_path += hierarchical_path[j];
				}
				//std::cerr << "try_file_path=\"" << try_file_path << "\":";
				if(access(try_file_path.c_str(), R_OK) == 0)
				{
					//std::cerr << "found" << std::endl;
					match = true;
					match_file_path = try_file_path;
				}
				else
				{
					//std::cerr << "not found" << std::endl;
				}
			}
		}
	}
	
	return match;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpSource(const char *source_path, unsigned int lineno, unsigned int colno, unsigned int count)
{
	bool match;
	std::string match_source_path;
	
	if((match = LocateFile(source_path, match_source_path)))
	{
		(*std_output_stream) << match_source_path;
	}
	else
	{
		(*std_output_stream) << source_path << " (not found)";
	}
	
	(*std_output_stream) << " at line #" << lineno;
	if(colno)
	{
		(*std_output_stream) << ", column #" << colno;
	}
	(*std_output_stream) << ": ";

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
				(*std_output_stream) << line << std::endl;
				printed++;
			}
			n++;
		} while(printed < count);
	}
	else
	{
		(*std_output_stream) << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpBackTrace(ADDRESS cia)
{
	std::vector<ADDRESS> *backtrace = backtrace_import->GetBackTrace(cia);
	
	if(backtrace)
	{
		unsigned int i;
		unsigned int n = backtrace->size();
		for(i = 0; i < n; i++)
		{
			(*std_output_stream) << "#" << i << " ";
			
			ADDRESS addr = (*backtrace)[i];
			
			const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
			
			(*std_output_stream) << "0x" << std::hex;
			(*std_output_stream) << (addr / memory_atom_size) << std::dec;
			if(symbol)
			{
				(*std_output_stream) << " <";
				(*std_output_stream) << symbol->GetFriendlyName(addr);
				(*std_output_stream) << ">";
			}

			const Statement<ADDRESS> *stmt = FindStatement(addr);
			
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
					(*std_output_stream) << " in ";
					std::string match_source_path;
					if(LocateFile(source_path.c_str(), match_source_path))
					{
						(*std_output_stream) << match_source_path;
					}
					else
					{
						(*std_output_stream) << source_path;
					}
	
					(*std_output_stream) << " at line #" << lineno;
					if(colno)
					{
						(*std_output_stream) << ", column #" << colno;
					}
				}
			}
			
			(*std_output_stream) << std::endl;
		}
		delete backtrace;
	}
	else
	{
		(*std_output_stream) << "No backtrace" << std::endl;
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

	const Symbol<ADDRESS> *symbol = FindSymbolByName(s);
	
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
	
	const Symbol<ADDRESS> *symbol = FindSymbolByName(s);
	
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
	
	const Statement<ADDRESS> *stmt = FindStatement(filename.c_str(), lineno, 0);
	
	if(stmt)
	{
		addr = stmt->GetAddress();
		return true;
	}
	
	return false;
}

template <class ADDRESS>
const Symbol<ADDRESS> *InlineDebugger<ADDRESS>::FindSymbolByAddr(ADDRESS addr)
{
	return symbol_table_lookup_import->FindSymbolByAddr(addr);
}

template <class ADDRESS>
const Symbol<ADDRESS> *InlineDebugger<ADDRESS>::FindSymbolByName(const char *s)
{
	return symbol_table_lookup_import->FindSymbolByName(s);
}

template <class ADDRESS>
const Statement<ADDRESS> *InlineDebugger<ADDRESS>::FindStatement(ADDRESS addr)
{
	return stmt_lookup_import->FindStatement(addr);
}

template <class ADDRESS>
const Statement<ADDRESS> *InlineDebugger<ADDRESS>::FindNextStatement(ADDRESS addr)
{
	const Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr);
	ADDRESS top_addr = addr;
	if(symbol)
	{
		ADDRESS symbol_addr = symbol->GetAddress();
		ADDRESS symbol_size = symbol->GetSize();
		typename Symbol<ADDRESS>::Type symbol_type = symbol->GetType();
		if((symbol_size != 0) && (symbol_type == Symbol<ADDRESS>::SYM_FUNC))
		{
			top_addr = symbol_addr + symbol_size - 1;
		}
	}

	addr++;
	const Statement<ADDRESS> *stmt = 0;
	
	if(addr <= top_addr)
	{
		do
		{
			stmt = FindStatement(addr);
			addr++;
		}
		while(!stmt && (addr <= top_addr));
	}
	
	return stmt;
}

template <class ADDRESS>
const Statement<ADDRESS> *InlineDebugger<ADDRESS>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno)
{
	return stmt_lookup_import->FindStatement(filename, lineno, colno);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableProgramProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, true);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableDataReadProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, true);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableDataWriteProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, true);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableDataProfiling()
{
	EnableDataReadProfiling();
	EnableDataWriteProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableProfiling()
{
	EnableProgramProfiling();
	EnableDataProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableProgramProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, false);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableDataReadProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, false);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableDataWriteProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, false);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableDataProfiling()
{
	DisableDataReadProfiling();
	DisableDataWriteProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableProfiling()
{
	DisableProgramProfiling();
	DisableDataProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearProgramProfile()
{
	if(!profiling_import) return;
	profiling_import->ClearProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearDataReadProfile()
{
	if(!profiling_import) return;
	profiling_import->ClearProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearDataWriteProfile()
{
	if(!profiling_import) return;
	profiling_import->ClearProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearDataProfile()
{
	ClearDataReadProfile();
	ClearDataWriteProfile();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearProfile()
{
	ClearProgramProfile();
	ClearDataProfile();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProgramProfilingStatus()
{
	if(!profiling_import) return;
	bool status = false;
	profiling_import->GetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, status);
	(*std_output_stream) << "Program profiling is " << (status ? "on" : "off") << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataReadProfilingStatus()
{
	if(!profiling_import) return;
	bool status = false;
	profiling_import->GetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, status);
	(*std_output_stream) << "Data read profiling is " << (status ? "on" : "off") << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataWriteProfilingStatus()
{
	if(!profiling_import) return;
	bool status = false;
	profiling_import->GetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, status);
	(*std_output_stream) << "Data write profiling is " << (status ? "on" : "off") << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataProfilingStatus()
{
	DumpDataReadProfilingStatus();
	DumpDataWriteProfilingStatus();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProfilingStatus()
{
	DumpProgramProfilingStatus();
	DumpDataProfilingStatus();
}




template <class ADDRESS>
bool InlineDebugger<ADDRESS>::GetLine(const char *prompt, std::string& line, bool& interactive)
{
	if(trap || exec_queue.empty())
	{
		if(trap)
		{
			trap = false;
			exec_queue.clear();
		}
#if defined(HAVE_LIBEDIT)
		char *line_read;
		do
		{
			if(std_output_stream != &std::cout)
			{
				(*std_output_stream) << prompt;
			}
			line_read = readline(prompt);
			if(!line_read)
			{
				rl_redisplay(); // work around when terminal size changes
			}
				
		} while(!line_read);

		if(std_output_stream != &std::cout)
		{
			(*std_output_stream) << line_read << std::endl;
		}
	/* If the line has any text in it,
		save it on the history. */
		if(*line_read) add_history(line_read);

		line = line_read;

		free(line_read);
		
		interactive = true;
		return true;
#else
		if(std_output_stream != &std::cout)
		{
			std::cout << prompt;
		}
		(*std_output_stream) << prompt;
		getline(cin, line);
		if(cin.fail()) return false;
		if(std_output_stream != &std::cout)
		{
			(*std_output_stream) << line << std::endl;
		}
		interactive = true;
		return true;
#endif
	}
	else
	{
		line = exec_queue.front();
		(*std_output_stream) << prompt << line << std::endl;
		exec_queue.pop_front();
		interactive = false;
		return true;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsBlankLine(const std::string& line)
{
	unsigned int n = line.length();
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		if(line[i] != ' ') return false;
	}
	return true;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsQuitCommand(const char *cmd)
{
	return strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStepInstructionCommand(const char *cmd)
{
	return strcmp(cmd, "si") == 0 || strcmp(cmd, "stepi") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStepCommand(const char *cmd)
{
	return strcmp(cmd, "s") == 0 || strcmp(cmd, "step") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsNextInstructionCommand(const char *cmd)
{
	return strcmp(cmd, "ni") == 0 || strcmp(cmd, "nexti") == 0;
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
bool InlineDebugger<ADDRESS>::IsEditCommand(const char *cmd)
{
	return strcmp(cmd, "e") == 0 || strcmp(cmd, "ed") == 0 || strcmp(cmd, "edit") == 0;
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
bool InlineDebugger<ADDRESS>::IsBackTraceCommand(const char *cmd)
{
	return strcmp(cmd, "bt") == 0 || strcmp(cmd, "backtrace") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsLoadSymbolTableCommand(const char *cmd)
{
	return strcmp(cmd, "st") == 0 || strcmp(cmd, "symtab") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsListSymbolsCommand(const char *cmd)
{
	return strcmp(cmd, "sym") == 0 || strcmp(cmd, "symbol") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsMacroCommand(const char *cmd)
{
	return strcmp(cmd, "mac") == 0 || strcmp(cmd, "macro") == 0;
}

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
