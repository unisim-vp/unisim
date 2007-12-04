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
#include <readline/readline.h>

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

using std::cout;
using std::hex;
using std::dec;

template <class ADDRESS>
InlineDebugger<ADDRESS>::InlineDebugger(const char *_name, Object *_parent) :
	Object(_name, _parent),
	Service<DebugControl<ADDRESS> >(_name, _parent),
	Service<MemoryAccessReporting<ADDRESS> >(_name, _parent),
	Client<MemoryAccessReportingControl>(_name, _parent),
	Client<Disassembly<ADDRESS> >(_name, _parent),
	Client<Memory<ADDRESS> >(_name, _parent),
	Client<Registers>(_name, _parent),
	Client<SymbolTableLookup<ADDRESS> >(_name, _parent),
	debug_control_export("debug-control-export", this),
	memory_access_reporting_export("memory-access-reporting-export", this),
	memory_access_reporting_control_import("memory-access-reporting-control-import", this),
	disasm_import("disasm-import", this),
	memory_import("memory-import", this),
	registers_import("registers-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this)
{
	trap = false;
	strcpy(last_line, "");
	strcpy(line, "");
	running_mode = INLINE_DEBUGGER_MODE_WAITING_USER;
	disasm_addr = 0;
	dump_addr = 0;
	prompt = string(_name) + "> ";
	Object::SetupDependsOn(memory_access_reporting_control_import);
}

template <class ADDRESS>
InlineDebugger<ADDRESS>::~InlineDebugger()
{
}

template<class ADDRESS>
bool InlineDebugger<ADDRESS>::Setup() {
	if(memory_access_reporting_control_import) {
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				false);
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				false);
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(watchpoint_registry.HasWatchpoint(mat, mt, addr, size)) trap = true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ReportFinishedInstruction(ADDRESS next_addr)
{
	if(breakpoint_registry.HasBreakpoint(next_addr)) trap = true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Trap()
{
	trap = true;
}

template <class ADDRESS>
typename DebugControl<ADDRESS>::DebugCommand InlineDebugger<ADDRESS>::FetchDebugCommand(ADDRESS cia)
{
	int count;
	bool recognized = false;
	ADDRESS addr;
	ADDRESS cont_addr;
	unsigned int size;
	char parm[8][256];

	if(!trap && (running_mode == INLINE_DEBUGGER_MODE_CONTINUE || running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL))
	{
		return DebugControl<ADDRESS>::DBG_STEP;
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL)
	{
		DeleteBreakpoint(cont_addr);
	}
	
	if(running_mode == INLINE_DEBUGGER_MODE_RESET)
	{
		running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
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
			return DebugControl<ADDRESS>::DBG_KILL;
		}

		if(IsBlankLine(line))
		{
	 		strcpy(line, last_line);
//			cout << "\033[1A\033[2C" << line << endl;
		}
		
		nparms = sscanf(line, " %s %s %s %s %s %s %s %s",
	                parm[0], parm[1], parm[2], parm[3], parm[4], parm[5], parm[6], parm[7]);

	
		recognized = false;

		switch(nparms)
		{
			case 1:
				{
					typename unisim::util::debug::Register *reg = registers_import->GetRegister(parm[0]);

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
					return DebugControl<ADDRESS>::DBG_KILL;
				}

				if(IsStepCommand(parm[0]))
				{
					running_mode = INLINE_DEBUGGER_MODE_STEP;
					strcpy(last_line, line);
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
					return DebugControl<ADDRESS>::DBG_STEP;
				}

				if(IsContinueCommand(parm[0]))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					strcpy(last_line, line);
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
					return DebugControl<ADDRESS>::DBG_STEP;
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
				break;
		} // end of switch

		if(!recognized)
		{
			cout << "Unrecognized command.  Try \"help\"." << endl;
		}
		
		strcpy(last_line, line);
	}
	return DebugControl<ADDRESS>::DBG_KILL;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Help()
{
	cout << "HELP:" << endl;
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
	cout << "    restart the simulation from the beginning" << endl << endl;
	cout << "================================ INSPECT =======================================" << endl;
	cout << "<dis | disasm | disassemble> [<symbol | *address>]" << endl;
	cout << "    continue to disassemble starting from 'symbol', 'address', or after" << endl;
	cout << "    the previous disassembly" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<d | dump> [<symbol | *address>]" << endl;
	cout << "    dump memory starting from 'symbol', 'address', or after the previous dump" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<register name>" << endl;
	cout << "    display the register value" << endl << endl;
	cout << "========================= BREAKPOINTS/WATCHPOINTS ==============================" << endl;
	cout << "<b | break> [<symbol | *address>]" << endl;
	cout << "    set a breakpoint at 'symbol' or 'address'. If 'symbol' or 'address' are not" << endl;
	cout << "    specified, display the breakpoint list" << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<w | watch> [<symbol | *address[:<size>]>] [<read | write>]" << endl;
	cout << "    set a watchpoint at 'symbol' or 'address'." << endl;
	cout << "    When using 'continue' and 'next' commands, the debugger will spy CPU loads" << endl;
	cout << "    and store. The debugger will return to command line prompt once a load," << endl;
	cout << "    or a store will access to 'symbol' or 'address'." << endl << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<del | delete> <symbol | *address>" << endl;
	cout << "    delete the breakpoint at 'symbol' or 'address'" << endl << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "<delw | delwatch> <symbol | *address> [<read | write>] [<size>]" << endl;
	cout << "    delete the watchpoint at 'symbol' or 'address'" << endl << endl;
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
			cout.fill('0');
			const Symbol<ADDRESS> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr) : 0;
			string s = disasm_import->Disasm(addr, next_addr);
			
			if(symbol)
			{
				if(symbol != last_symbol)
				{
					cout << "0x" << hex;
					cout.width(8);
					cout << addr << dec;
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
			cout << addr << ":" << dec << s << endl;
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
		
		cout << "*0x" << hex << addr << dec << " (";
		if(symbol_table_lookup_import)
		{
			const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
		
			if(symbol)
			{
				cout << symbol->GetFriendlyName(addr);
			}
			else
			{
				cout << "?";
			}
		}
		cout << ")" << endl;
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
		}
		cout << " ";
		
		cout << "*0x" << hex << addr << dec << ":" << size << " (";
		
		if(symbol_table_lookup_import)
		{
			const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
		
			if(symbol)
			{
				cout << symbol->GetFriendlyName(addr);
			}
			else
			{
				cout << "?";
			}
		}
		cout << ")" << endl;
	}
}


template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpMemory(ADDRESS addr)
{
	int i, j;

	cout.fill(' ');
	cout.width(56);
	cout << "address  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f" << endl;
	for(i = 0; i < 16; i++)
	{
		cout << hex; cout.fill('0'); cout.width(8); cout << addr << " "; cout.fill(' ');
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
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseAddrRange(const char *s, ADDRESS& addr, unsigned int& size)
{
	if(sscanf(s, "*%u:%u", &addr, &size) == 2) return true;
	if(sscanf(s, "*%x:%u", &addr, &size) == 2) return true;
	if(sscanf(s, "*%x", &addr) == 1) return true;
	if(sscanf(s, "*%u", &addr) == 1) return true;
	
	if(symbol_table_lookup_import)
	{
		const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(s);
		
		if(symbol)
		{
			addr = symbol->GetAddress();
			size = symbol->GetSize();
			if(size < 1) size = 1;
			return true;
		}
	}
	
	return false;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseAddr(const char *s, ADDRESS& addr)
{
	if(sscanf(s, "*%x", &addr) == 1) return true;
	if(sscanf(s, "*%u", &addr) == 1) return true;
	
	if(symbol_table_lookup_import)
	{
		const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(s);
		
		if(symbol)
		{
			addr = symbol->GetAddress();
			return true;
		}
	}
	
	return false;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::GetLine(char *line, int size)
{
	//cin.getline(line, size);
	//if(cin.fail()) return false;
	//return true;
	
	char *line_read = readline(prompt.c_str());

  /* If the line has any text in it,
     save it on the history. */
	if(line_read)
	{
		if(*line_read) add_history(line_read);

		strcpy(line, line_read);

		return true;
	}
	return false;
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

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
