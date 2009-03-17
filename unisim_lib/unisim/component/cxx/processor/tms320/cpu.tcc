/*
 *  Copyright (c) 2009,
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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_TCC__
  
#include "unisim/component/cxx/processor/tms320/cpu.hh"
#include "unisim/component/cxx/processor/tms320/config.hh"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

#define LOCATION "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::LittleEndian2Host;
using unisim::util::endian::Host2LittleEndian;

using std::endl;
using std::hex;
using std::dec;

using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;
using namespace unisim::kernel::logger;

//===============================================================
//= Client/Service setup methods                          START =
//===============================================================
	
template<class CONFIG, bool DEBUG>
CPU<CONFIG, DEBUG> ::
CPU(const char *name,
		Object *parent) :
	Object(name, parent),
	Client<DebugControl<typename CONFIG::address_t> >(name, parent),
	Client<MemoryAccessReporting<typename CONFIG::address_t> >(name, parent),
	Service<MemoryAccessReporting<typename CONFIG::address_t> > (name, parent),
	Service<Disassembly<typename CONFIG::address_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<MemoryInjection<typename CONFIG::address_t> >(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
	memory_injection_export("memory_injection_export", this),
	memory_access_reporting_control_export(
		"memory_access_reporting_control_export",
		this),
	memory_export("memory_export", this),
	debug_control_import("debug_control_import", this),
	memory_access_reporting_import("memory_access_reporting_import", this),
	symbol_table_lookup_import("symbol_table_lookup_import", this),
	memory_import("memory_import", this),
	logger(*this),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup)
{
}

template<class CONFIG, bool DEBUG>
CPU<CONFIG, DEBUG> ::
~CPU() 
{
}

//===============================================================
//= Client/Service setup methods                           STOP =
//===============================================================
	
//===============================================================
//= Client/Service setup methods                          START =
//===============================================================

template<class CONFIG, bool DEBUG>
bool 
CPU<CONFIG, DEBUG> ::
Setup() 
{
	bool success = true;
	
	if (VerboseAll)
	{
		verbose_setup = true;
	}
	
	if (VerboseSetup)
	{
		logger << DebugInfo << "Starting \"" << name << "\" setup" << endl;
		if (VerboseAll())
			logger << "- verbose_all = true" << endl;
		else
		{
			if (VerboseSetup())
				logger << "- verbose_setup = true" << endl;
		}
	}
	
	/* setting debugging registers */
	if (VerboseSetup())
		logger << "- initializing debugging registers" << endl;
		
	//	for (int i = 0; i < 13; i++)
	//	{
	//		stringstream str;
	//		str << "r" << i;
	//		registers_registry[str.str()] =
	//			new SimpleRegister<reg_t>(str.str().c_str() &gpr[i]);
	//	}
	//	register_registry["sp"] = new SimpleRegister<reg_t>("sp", &gpr[13]);
	
	if (!memory_access_reporting_import)
	{
		if (VerboseSetup())
			logger << "- memory access reporting not active";
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}
	
	return success;
	return true;
}

template<class CONFIG, bool DEBUG>
void
CPU<CONFIG, DEBUG> ::
OnDisconnect() 
{
}

//===============================================================
//= Client/Service setup methods                           STOP =
//===============================================================

//===============================================================
//= Memory injection interface methods                    START =
//===============================================================

template<class CONFIG, bool DEBUG>
bool 
CPU<CONFIG, DEBUG> ::
InjectReadMemory(address_t addr, void *buffer, uint32_t size)
{
	logger << DebugWarning << "TODO: implement InjectReadMemory" << endl
		<< LOCATION << EndDebug;
	return false;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	logger << DebugWarning << "TODO: implement InjectWriteMemory" << endl
		<< LOCATION << EndDebug;
	return false;
}

//===============================================================
//= Memory injection interface methods                     STOP =
//===============================================================

//===============================================================
//= Memory access reporting control interface methods     START =
//===============================================================

template<class CONFIG, bool DEBUG>
void
CPU<CONFIG, DEBUG> ::
RequiresMemoryAccessReporting(bool report)
{
	requires_memory_access_reporting = report;
}

template<class CONFIG, bool DEBUG>
void
CPU<CONFIG, DEBUG> ::
RequiresFinishedInstructionReporting(bool report)
{
	requires_finished_instruction_reporting = report;
}

//===============================================================
//= Memory access reporting control interface methods      STOP =
//===============================================================

//===============================================================
//= Memory interface methods                              START =
//===============================================================

template<class CONFIG, bool DEBUG>
void
CPU<CONFIG, DEBUG> ::
Reset()
{
	logger << DebugWarning << "TODO: implement reset" << endl
		<< LOCATION << EndDebug;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
ReadMemory(address_t addr, void *buffer, uint32_t size)
{
	if (memory_import)
		return memory_import->ReadMemory(addr, buffer, size);
	return false;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
WriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	if (memory_import)
		return memory_import->WriteMemory(addr, buffer, size);
	return false;
}

//===============================================================
//= Memory interface methods                               STOP =
//===============================================================

//===============================================================
//= CPURegistersInterface interface methods               START =
//===============================================================

/**
 * Gets a register interface to the register specified by name.
 * 
 * @param  name   The name of the requested register.
 * @return        A pointer to the RegisterInterface corresponding to name.
 */
template<class CONFIG, bool DEBUG>
Register *
CPU<CONFIG, DEBUG> ::
GetRegister(const char *name)
{
	if (registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;
}

//===============================================================
//= CPURegistersInterface interface methods                STOP =
//===============================================================

//===============================================================
//= DebugDisasmInterface interface methods                START =
//===============================================================

/**
 * Returns a string with the disassembling for the instruction found
 *   at address addr.
 *
 * @param   addr       The address of the instruction to disassemble.
 * @param   next_addr  The address following the requested instruction.
 * @return             The disassembling of the requested instruction address.
 */
template<class CONFIG, bool DEBUG>
string 
CPU<CONFIG, DEBUG> ::
Disasm(address_t addr, address_t &next_addr) 
{
	typename isa::tms320::Operation<CONFIG> *op = NULL;
	typename CONFIG::insn_t insn;
	stringstream buffer;
	
	if (!memory_import)
	{
		buffer << "no memory_import";
		return buffer.str();
	}
	
	if (!memory_import->ReadMemory(addr, &insn, sizeof(insn)))
	{
		buffer << "error with memory_import";
		return buffer.str();
	}
	insn = LittleEndian2Host(insn);
	op = decoder.Decode(addr, insn);
	op->disasm(*this, buffer);

	return buffer.str();
}

//===============================================================
//= DebugDisasmInterface interface methods                 STOP =
//===============================================================

//===============================================================
//= Execution methods                                     START =
//===============================================================

/** Execute one complete instruction
 */
template<class CONFIG, bool DEBUG>
void 
CPU<CONFIG, DEBUG> ::
StepInstruction()
{
	logger << DebugWarning << "TODO: implement StepInstruction" << endl
		<< LOCATION << EndDebug;
}

template<class CONFIG, bool DEBUG>
void
CPU<CONFIG, DEBUG> ::
Stop(int ret)
{
	logger << DebugWarning << "TODO: implement Stop" << endl
		<< LOCATION << EndDebug;
}

//===============================================================
//= Execution methods                                      STOP =
//===============================================================

//===============================================================
//= Verbose variables, parameters, and methods            START =
//===============================================================

template<class CONFIG, bool DEBUG>
inline INLINE 
bool
CPU<CONFIG, DEBUG> ::
VerboseAll()
{
	return DEBUG && verbose_all;
}

template<class CONFIG, bool DEBUG>
inline INLINE 
bool
CPU<CONFIG, DEBUG> ::
VerboseSetup()
{
	return DEBUG && verbose_setup;
}

//===============================================================
//= Verbose variables, parameters, and methods             STOP =
//===============================================================

} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#undef INLINE
#undef LOCATION