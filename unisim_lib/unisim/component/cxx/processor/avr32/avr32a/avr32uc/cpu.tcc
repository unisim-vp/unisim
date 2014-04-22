/*
 *  Copyright (c) 2014,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_TCC__

#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc>
#include <unisim/kernel/debug/debug.hh>

#include "unisim/component/cxx/processor/avr32/avr32a/avr32uc/cpu.hh"

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace avr32 {
namespace avr32a {
namespace avr32uc {

using namespace std;

template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
	: Object(name, parent, "this module implements an AVR32UC CPU core")
	, unisim::component::cxx::processor::avr32::avr32a::avr32uc::Decoder<CONFIG>()
	, Client<Loader>(name,  parent)
	, Client<SymbolTableLookup<typename CONFIG::address_t> >(name,  parent)
	, Client<DebugControl<typename CONFIG::address_t> >(name,  parent)
	, Client<MemoryAccessReporting<typename CONFIG::address_t> >(name,  parent)
	, Client<TrapReporting>(name,  parent)
	, Service<MemoryAccessReportingControl>(name,  parent)
	, Service<Disassembly<typename CONFIG::address_t> >(name,  parent)
	, Service<unisim::service::interfaces::Registers>(name,  parent)
	, Service<Memory<typename CONFIG::address_t> >(name,  parent)
	, Service<MemoryInjection<typename CONFIG::address_t> >(name,  parent)
	, Client<Memory<typename CONFIG::physical_address_t> >(name,  parent)
	, Service<Synchronizable>(name,  parent)
	, Client<AVR32_T2H_Syscalls>(name, parent)
	, disasm_export("disasm-export",  this)
	, registers_export("registers-export",  this)
	, memory_export("memory-export",  this)
	, memory_injection_export("memory-injection-export",  this)
	, synchronizable_export("synchronizable-export",this)
	, memory_access_reporting_control_export("memory_access_reporting_control_export",  this)
	, loader_import("loader-import",  this)
	, debug_control_import("debug-control-import",  this)
	, memory_access_reporting_import("memory-access-reporting-import",  this)
	, symbol_table_lookup_import("symbol-table-lookup-import",  this)
	, memory_import("memory-import", this)
	, trap_reporting_import("trap-reporting-import",  this)
	, avr32_t2h_syscalls_import("avr32-t2h-syscalls-import", this)
	, logger(*this)
	, requires_memory_access_reporting(true)
	, requires_finished_instruction_reporting(true)
	, verbose_all(false)
	, verbose_setup(false)
	, verbose_interrupt(false)
	, verbose_step(false)
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, enable_trap_on_exception(false)
	, halt_on_addr((typename CONFIG::address_t) -1)
	, halt_on()
	, max_inst(0xffffffffffffffffULL)
	, registers_registry()
	, instruction_counter(0)
	, param_max_inst("max-inst",  this,  max_inst, "maximum number of instructions to simulate")
	, param_verbose_all("verbose-all",  this,  verbose_all, "globally enable/disable verbosity")
	, param_verbose_setup("verbose-setup",  this,  verbose_all, "enable/disable verbosity while setup")
	, param_verbose_interrupt("verbose-interrupt",  this,  verbose_interrupt, "enable/disable verbosity when handling interrupts")
	, param_verbose_step("verbose-step", this, verbose_step, "enable/disable verbosity when stepping instructions")
	, param_trap_on_instruction_counter("trap-on-instruction-counter",  this,  trap_on_instruction_counter, "number of simulated instruction before traping")
	, param_enable_trap_on_exception("enable-trap-on-exception", this, enable_trap_on_exception, "enable/disable trap reporting on exception")
	, param_halt_on("halt-on", this, halt_on, "Symbol or address where to stop simulation")
	, stat_instruction_counter("instruction-counter",  this,  instruction_counter, "number of simulated instructions")
{
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	unsigned int i;

	for(i = 0; i < 13; i++)
	{
		std::stringstream sstr_register_name;
		sstr_register_name << "r" << i;
		registers_registry[sstr_register_name.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr_register_name.str().c_str(), &gpr[i]);
	}
	registers_registry["sp"] = new unisim::util::debug::SimpleRegister<uint32_t>("sp", &gpr[13]);
	registers_registry["lr"] = new unisim::util::debug::SimpleRegister<uint32_t>("lr", &gpr[14]);
	registers_registry["pc"] = new unisim::util::debug::SimpleRegister<uint32_t>("pc", &gpr[15]);
	registers_registry["sr"] = new unisim::util::debug::SimpleRegister<uint32_t>("sr", &sr);

	Reset();
	std::stringstream sstr_description;
	sstr_description << "This module implements an AVR32UC CPU core." << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
}

template <class CONFIG>
bool CPU<CONFIG>::BeginSetup()
{
	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}

	Reset();

	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::EndSetup()
{
	if(!halt_on.empty())
	{
		const Symbol<typename CONFIG::address_t> *halt_on_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByName(halt_on.c_str(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		
		if(halt_on_symbol)
		{
			halt_on_addr = halt_on_symbol->GetAddress();
			if(IsVerboseSetup())
			{
				logger << DebugInfo << "Simulation will halt at '" << halt_on_symbol->GetName() << "' (0x" << std::hex << halt_on_addr << std::dec << ")" << EndDebugInfo;
			}
		}
		else
		{
			std::stringstream sstr(halt_on);
			sstr >> std::hex;
			if(sstr >> halt_on_addr)
			{
				if(IsVerboseSetup())
				{
					logger << DebugInfo <<  "Simulation will halt at 0x" << std::hex << halt_on_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				logger << DebugWarning << "Invalid address (" << halt_on << ") in Parameter " << param_halt_on.GetName() << EndDebugWarning;
				halt_on_addr = (typename CONFIG::address_t) -1;
			}
		}
	}
	return true;
}

//=====================================================================
//=         memory access reporting control interface methods   START =
//=====================================================================

template<class CONFIG>
void 
CPU<CONFIG>::RequiresMemoryAccessReporting(bool report) {
	requires_memory_access_reporting = report;
}

template<class CONFIG>
void 
CPU<CONFIG>::RequiresFinishedInstructionReporting(bool report) {
	requires_finished_instruction_reporting = report;
}

//=====================================================================
//=         memory access reporting control interface methods   END   =
//=====================================================================

template <class CONFIG>
void CPU<CONFIG>::OnDisconnect()
{
}

template <class CONFIG>
void CPU<CONFIG>::Reset()
{
	instruction_counter = 0;
	gpr[15]=0x80000000;
	int i;
        for(i=0;i<15;i++) {gpr[i]=0;}

        sr=(0<<CONFIG::SR_SS_OFFSET)|(0<<CONFIG::SR_H_OFFSET)|(0<<CONFIG::SR_J_OFFSET)|(0<<CONFIG::SR_DM_OFFSET)|(0<<CONFIG::SR_D_OFFSET)|(0<<CONFIG::SR_M2_OFFSET)|(0<<CONFIG::SR_M1_OFFSET)|(1<<CONFIG::SR_M0_OFFSET)|(1<<CONFIG::SR_EM_OFFSET)|(0<<CONFIG::SR_I3M_OFFSET)|(0<<CONFIG::SR_I2M_OFFSET)|(0<<CONFIG::SR_I1M_OFFSET)|(0<<CONFIG::SR_I0M_OFFSET)|(1<<CONFIG::SR_GM_OFFSET)|(0<<CONFIG::SR_R_OFFSET)|(0<<CONFIG::SR_T_OFFSET)|(0<<CONFIG::SR_L_OFFSET)|(0<<CONFIG::SR_Q_OFFSET)|(0<<CONFIG::SR_V_OFFSET)|(0<<CONFIG::SR_N_OFFSET)|(0<<CONFIG::SR_Z_OFFSET)|(0<<CONFIG::SR_C_OFFSET);

	
	
	CPU<CONFIG>::InvalidateDecodingCache();
}
 
template <class CONFIG>
bool CPU<CONFIG>::Fetch(typename CONFIG::address_t addr, void *buffer,uint32_t size)
{
	
	return IHSBRead(addr,buffer,size);
}

template <class CONFIG>
void CPU<CONFIG>::StepOneInstruction()
{
	uint32_t pc = npc;

	if(unlikely(debug_control_import != 0))
	{
		do
		{
			typename DebugControl<typename CONFIG::address_t>::DebugCommand dbg_cmd;
			dbg_cmd = debug_control_import->FetchDebugCommand(pc);
	
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_SYNC)
			{
				Synchronize();
				continue;
			}

			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_KILL) Stop(0);
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_RESET)
			{
				if(loader_import)
				{
					loader_import->Load();
				}
			}
		} while(1);
	}

	unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation = 0;

	uint8_t buffer[4];
	if(likely(Fetch(pc, buffer, sizeof(buffer)*8)))
	{
		CodeType insn(buffer, sizeof(buffer) * 8);
	  
		operation = unisim::component::cxx::processor::avr32::avr32a::avr32uc::Decoder<CONFIG>::Decode(pc, insn);

		if(unlikely(IsVerboseStep()))
		{
			stringstream sstr;
			operation->disasm((CPU<CONFIG> *) this, sstr);
			logger << DebugInfo << "#" << instruction_counter << ":0x" << std::hex << pc << std::dec << ":0x" << std::hex << operation->GetEncoding() << std::dec << ":" << sstr.str() << endl << EndDebugInfo;
		}

 	        /* update PC register value before execution */
	        gpr[15] = pc;
		/* update NPC */
	        npc += operation->GetLength();

		/* execute the instruction */
		if(likely(operation->execute(this)))
		{
			/* update the instruction counter */
			instruction_counter++;
		}
	}

	//ProcessExceptions(operation);

	/* report a finished instruction */
	if(unlikely(requires_finished_instruction_reporting))
	{
		if(unlikely(memory_access_reporting_import != 0))
		{
			memory_access_reporting_import->ReportFinishedInstruction(pc, npc);
		}
	}

	if(unlikely(trap_reporting_import && (instruction_counter == trap_on_instruction_counter)))
	{
		trap_reporting_import->ReportTrap();
	}
	
	if(unlikely((instruction_counter >= max_inst) || (npc == halt_on_addr))) Stop(0);
}

template<class CONFIG>
void CPU<CONFIG>::SwitchExecutionMode(uint32_t execution_mode)
{
	// Save
	if((GetSR_M() != CONFIG::EXEC_MODE_APPLICATION) && (execution_mode == CONFIG::EXEC_MODE_APPLICATION))
	{
		// Privileged mode -> Application mode
		// Save
		sp_sys = GetSP();
		// Restore
		SetSP(sp_app);
	}
	else if((GetSR_M() == CONFIG::EXEC_MODE_APPLICATION) && (execution_mode != CONFIG::EXEC_MODE_APPLICATION))
	{
		// Application mode -> Privileged mode
		// Save
		sp_app = GetSP();
		// Restore
		SetSP(sp_sys);
	}
}
template<class CONFIG>
uint32_t CPU<CONFIG>::GetPriorityLevel(uint32_t execution_mode)
{
 	switch(execution_mode)
 	{
		case CONFIG::EXEC_MODE_NMI: return CONFIG::PRIO_LEVEL_MODE_NMI;break;
		case CONFIG::EXEC_MODE_EXC: return CONFIG::PRIO_LEVEL_MODE_EXC;break;
		case CONFIG::EXEC_MODE_INT_LEVEL3: return CONFIG::PRIO_LEVEL_MODE_INT_LEVEL3;break;
		case CONFIG::EXEC_MODE_INT_LEVEL2: return CONFIG::PRIO_LEVEL_MODE_INT_LEVEL2;break;
		case CONFIG::EXEC_MODE_INT_LEVEL1:return CONFIG::PRIO_LEVEL_MODE_INT_LEVEL1;break;
		case CONFIG::EXEC_MODE_INT_LEVEL0 :return CONFIG::PRIO_LEVEL_MODE_INT_LEVEL0;break;
		case CONFIG::EXEC_MODE_SUPERVISOR :return CONFIG::PRIO_LEVEL_MODE_SUPERVISOR;break;
		case CONFIG::EXEC_MODE_APPLICATION:return CONFIG::PRIO_LEVEL_MODE_APPLICATION;break;
	}
	
	std::runtime_error("Internal Error");

	return 0;
}

template <class CONFIG>
string CPU<CONFIG>::GetObjectFriendlyName(typename CONFIG::address_t addr)
{
	stringstream sstr;
	
	const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template <class CONFIG>
string CPU<CONFIG>::GetFunctionFriendlyName(typename CONFIG::address_t addr)
{
	stringstream sstr;
	
	const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else //SetGPR_mem
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template <class CONFIG>
unisim::util::debug::Register *CPU<CONFIG>::GetRegister(const char *name)
{
	map<string, unisim::util::debug::Register *>::iterator reg_iter = registers_registry.find(name);
	if(reg_iter != registers_registry.end())
	{
		return (*reg_iter).second;
	}

	return 0;
}

template <class CONFIG>
bool CPU<CONFIG>::IHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size)
{
	// Not implemented here
	return false;
}

template <class CONFIG>
bool CPU<CONFIG>::DHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size)
{
	// Not implemented here
	return false;
}

template <class CONFIG>
bool CPU<CONFIG>::DHSBWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size)
{
	// Not implemented here
	return false;
}

template <class CONFIG>
void CPU<CONFIG>::SetNMIREQ()
{
	// TODO
}

template <class CONFIG>
void CPU<CONFIG>::ResetNMIREQ()
{
	// TODO
}

template <class CONFIG>
void CPU<CONFIG>::SetIRQ(unsigned int irq)
{
	// TODO
}

template <class CONFIG>
void CPU<CONFIG>::ResetIRQ(unsigned int irq)
{
	// TODO
}

template <class CONFIG>
void CPU<CONFIG>::RunInternalTimers()
{
	// Not implemented here
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
	// Not implemented here
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
	// Not implemented hereIHSB
}

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	
        
	return memory_import-> ReadMemory( addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	
	return memory_import-> WriteMemory( addr, buffer, size);
}

template <class CONFIG>
string CPU<CONFIG>::Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr)
{
	uint8_t buffer[4];
        if(ReadMemory(addr, buffer ,sizeof(buffer)))
	{
		CodeType insn= CodeType(buffer, sizeof(buffer) * 8);
	        unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation=0;
		operation = unisim::component::cxx::processor::avr32::avr32a::avr32uc::Decoder<CONFIG>::Decode(addr, insn);

		next_addr = addr + operation->GetLength();
		std::stringstream sstrdisasm;
		sstrdisasm << operation->GetEncoding() << " ";

		/* disasm the instruction */
		operation->disasm(this,sstrdisasm);	
	
	
		return sstrdisasm.str();
	}
	else
        {
		next_addr= addr+2;
		return std::string("not readable");
	}
}

/* Memory injection */
template <class CONFIG>
bool CPU<CONFIG>::InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	// TODO
	return false;
}

template <class CONFIG>
bool CPU<CONFIG>::InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size)
{
	// TODO
	return false;
}

} // end of namespace avr32uc
} // end of namespace avr32a
} // end of namespace avr32
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
