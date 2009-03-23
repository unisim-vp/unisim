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

#include "unisim/component/cxx/processor/tms320/isa_tms320.tcc"

#include "unisim/util/debug/simple_register.hh"

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
	Client<DebugControl<uint64_t> >(name, parent),
	Client<MemoryAccessReporting<uint64_t> >(name, parent),
	Service<MemoryAccessReportingControl> (name, parent),
	Service<Disassembly<uint64_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<Memory<uint64_t> >(name, parent),
	Client<Memory<uint64_t> >(name, parent),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
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
	int_reg_lookup[REG_R0] = &reg_r[0].lo;
	int_reg_lookup[REG_R1] = &reg_r[1].lo;
	int_reg_lookup[REG_R2] = &reg_r[2].lo;
	int_reg_lookup[REG_R3] = &reg_r[3].lo;
	int_reg_lookup[REG_R4] = &reg_r[4].lo;
	int_reg_lookup[REG_R5] = &reg_r[5].lo;
	int_reg_lookup[REG_R6] = &reg_r[6].lo;
	int_reg_lookup[REG_R7] = &reg_r[7].lo;
	int_reg_lookup[REG_AR0] = &reg_ar[0];
	int_reg_lookup[REG_AR1] = &reg_ar[1];
	int_reg_lookup[REG_AR2] = &reg_ar[2];
	int_reg_lookup[REG_AR3] = &reg_ar[3];
	int_reg_lookup[REG_AR4] = &reg_ar[4];
	int_reg_lookup[REG_AR5] = &reg_ar[5];
	int_reg_lookup[REG_AR6] = &reg_ar[6];
	int_reg_lookup[REG_AR7] = &reg_ar[7];
	int_reg_lookup[REG_DP] = &reg_dp;
	int_reg_lookup[REG_IR0] = &reg_ir0;
	int_reg_lookup[REG_IR1] = &reg_ir1;
	int_reg_lookup[REG_BK] = &reg_bk;
	int_reg_lookup[REG_SP] = &reg_sp;
	int_reg_lookup[REG_ST] = &reg_st;
	int_reg_lookup[REG_IE] = &reg_ie;
	int_reg_lookup[REG_IF] = &reg_if;
	int_reg_lookup[REG_IOF] = &reg_iof;
	int_reg_lookup[REG_RS] = &reg_rs;
	int_reg_lookup[REG_RE] = &reg_re;
	int_reg_lookup[REG_RC] = &reg_rc;

	unsigned int i;
	for(i = 0; i < 8; i++)
	{
		stringstream sstr;
		sstr << "R" << i;
		registers_registry[sstr.str().c_str()] = new ExtendedPrecisionRegisterDebugInterface(sstr.str().c_str(), &reg_r[0]);
	}

	for(i = 0; i < 8; i++)
	{
		stringstream sstr;
		sstr << "AR" << i;
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &reg_ar[0]);
	}

	registers_registry["IR0"] = new unisim::util::debug::SimpleRegister<uint32_t>("IR0", &reg_ir0);
	registers_registry["IR1"] = new unisim::util::debug::SimpleRegister<uint32_t>("IR1", &reg_ir1);
	registers_registry["BK"] = new unisim::util::debug::SimpleRegister<uint32_t>("BK", &reg_bk);
	registers_registry["SP"] = new unisim::util::debug::SimpleRegister<uint32_t>("SP", &reg_sp);
	registers_registry["ST"] = new unisim::util::debug::SimpleRegister<uint32_t>("ST", &reg_st);
	registers_registry["IE"] = new unisim::util::debug::SimpleRegister<uint32_t>("IE", &reg_ie);
	registers_registry["IF"] = new unisim::util::debug::SimpleRegister<uint32_t>("IF", &reg_if);
	registers_registry["IOF"] = new unisim::util::debug::SimpleRegister<uint32_t>("IOF", &reg_iof);
	registers_registry["RS"] = new unisim::util::debug::SimpleRegister<uint32_t>("RS", &reg_rs);
	registers_registry["RE"] = new unisim::util::debug::SimpleRegister<uint32_t>("RE", &reg_re);
	registers_registry["RC"] = new unisim::util::debug::SimpleRegister<uint32_t>("RC", &reg_rc);

}

template<class CONFIG, bool DEBUG>
CPU<CONFIG, DEBUG> ::
~CPU() 
{
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
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
	
	if (VerboseAll())
	{
		verbose_setup = true;
	}
	
	if (VerboseSetup())
	{
		logger << DebugInfo << "Starting \"" << Object::GetName() << "\" setup" << endl;
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
ReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	if (memory_import)
		return memory_import->ReadMemory(addr, buffer, size);
	return false;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
WriteMemory(uint64_t addr, const void *buffer, uint32_t size)
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
	map<string, unisim::util::debug::Register *>::iterator reg_iter = registers_registry.find(name);
	if(reg_iter != registers_registry.end())
	{
		return (*reg_iter).second;
	}

	return 0;
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
Disasm(uint64_t _addr, uint64_t &next_addr) 
{
	address_t addr = (address_t) _addr;
	typename isa::tms320::Operation<CONFIG, DEBUG> *op = NULL;
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
	buffer << "0x" << std::hex;
	buffer.fill('0');
	buffer.width(8); 
	op = decoder.Decode(addr, insn);
	buffer << op->GetEncoding() << std::dec << " ";
	if(!op->disasm(*this, buffer)) buffer << "?";
	next_addr = (addr + 4);

	return buffer.str();
}

template <class CONFIG, bool DEBUG>
bool CPU<CONFIG, DEBUG>::DisasmIndir(string& s, unsigned int mod, unsigned int ar, unsigned int disp)
{
	stringstream sstr;

	switch(mod)
	{
		case 0x00: // 00000
			sstr << "*+AR" << ar << "("<< disp << ")";
			s = sstr.str();
			return true;
		case 0x01: // 00001
			sstr << "*-AR" << ar << "("<< disp << ")";
			s = sstr.str();
			return true;
		case 0x02: // 00010
			sstr << "*++AR" << ar << "("<< disp << ")";
			s = sstr.str();
			return true;
		case 0x03: // 00011
			sstr << "*--AR" << ar << "("<< disp << ")";
			s = sstr.str();
			return true;
		case 0x04: // 00100
			sstr << "*AR++" << ar << "("<< disp << ")";
			s = sstr.str();
			return true;
		case 0x05: // 00101
			sstr << "*AR--" << ar << "("<< disp << ")";
			s = sstr.str();
			return true;
		case 0x06: // 00110
			sstr << "*AR++" << ar << "("<< disp << ")%";
			s = sstr.str();
			return true;
		case 0x07: // 00111
			sstr << "*AR--" << ar << "("<< disp << ")%";
			s = sstr.str();
			return true;
		case 0x08: // 01000
			sstr << "*+AR" << ar << "(IR0)";
			s = sstr.str();
			return true;
		case 0x09: // 01001
			sstr << "*-AR" << ar << "(IR0)";
			s = sstr.str();
			return true;
		case 0x0a: // 01010
			sstr << "*++AR" << ar << "(IR0)";
			s = sstr.str();
			return true;
		case 0x0b: // 01011
			sstr << "*--AR" << ar << "(IR0)";
			s = sstr.str();
			return true;
		case 0x0c: // 01100
			sstr << "*AR++" << ar << "(IR0)";
			s = sstr.str();
			return true;
		case 0x0d: // 01101
			sstr << "*AR--" << ar << "(IR0)";
			s = sstr.str();
			return true;
		case 0x0e: // 01110
			sstr << "*AR++" << ar << "(IR0)%";
			s = sstr.str();
			return true;
		case 0x0f: // 01111
			sstr << "*AR--" << ar << "(IR0)%";
			s = sstr.str();
			return true;
		case 0x10: // 10000
			sstr << "*+AR" << ar << "(IR1)";
			s = sstr.str();
			return true;
		case 0x11: // 10001
			sstr << "*-AR" << ar << "(IR1)";
			s = sstr.str();
			return true;
		case 0x12: // 10010
			sstr << "*++AR" << ar << "(IR1)";
			s = sstr.str();
			return true;
		case 0x13: // 10011
			sstr << "*--AR" << ar << "(IR1)";
			s = sstr.str();
			return true;
		case 0x14: // 10100
			sstr << "*AR++" << ar << "(IR1)";
			s = sstr.str();
			return true;
		case 0x15: // 10101
			sstr << "*AR--" << ar << "(IR1)";
			s = sstr.str();
			return true;
		case 0x16: // 10110
			sstr << "*AR++" << ar << "(IR1)%";
			s = sstr.str();
			return true;
		case 0x17: // 10111
			sstr << "*AR--" << ar << "(IR1)%";
			s = sstr.str();
			return true;
		case 0x18: // 11000
			sstr << "*AR" << ar;
			s = sstr.str();
			return true;
		case 0x19: // 11001
			sstr << "*AR" << ar << "++(IR0)B";
			s = sstr.str();
			return true;
	}
	return false;
}

template <class CONFIG, bool DEBUG>
string CPU<CONFIG, DEBUG>::DisasmShortFloat(uint16_t x)
{
	stringstream sstr;
	sstr << "short_float(0x" << hex << x << dec << ")";
	return sstr.str();
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

	if(debug_control_import)
	{
		do
		{
			typename DebugControl<uint64_t>::DebugCommand dbg_cmd;

			dbg_cmd = debug_control_import->FetchDebugCommand(0 /*pc*/);
	
			if(dbg_cmd == DebugControl<uint64_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<uint64_t>::DBG_KILL) Stop(0);
		} while(1);
	}

	Stop(-1);
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

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_TCC__

