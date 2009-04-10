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
#include "unisim/util/arithmetic/arithmetic.hh"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

#include "unisim/component/cxx/processor/tms320/isa_tms320.tcc"
#include "unisim/util/arithmetic/arithmetic.hh"

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
using unisim::util::arithmetic::Log2;
using unisim::util::arithmetic::ReverseCarryPropagationAdd;
using unisim::util::arithmetic::BitScanForward;

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
	requires_memory_access_reporting(true),
	requires_finished_instruction_reporting(true),
	logger(*this),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup),
	instruction_counter(0),
	stat_instruction_counter("instruction-counter", this, instruction_counter),
	max_inst(0xffffffffffffffffULL),
	param_max_inst("max-inst", this, max_inst),
	stat_insn_cache_hits("insn-cache-hits", this, insn_cache_hits),
	stat_insn_cache_misses("insn-cache-misses", this, insn_cache_misses)
{
	regs[REG_R0].lo_write_mask = 0xffffffff;
	regs[REG_R1].lo_write_mask = 0xffffffff;
	regs[REG_R2].lo_write_mask = 0xffffffff;
	regs[REG_R3].lo_write_mask = 0xffffffff;
	regs[REG_R4].lo_write_mask = 0xffffffff;
	regs[REG_R5].lo_write_mask = 0xffffffff;
	regs[REG_R6].lo_write_mask = 0xffffffff;
	regs[REG_R7].lo_write_mask = 0xffffffff;
	regs[REG_AR0].lo_write_mask = 0xffffffff;
	regs[REG_AR1].lo_write_mask = 0xffffffff;
	regs[REG_AR2].lo_write_mask = 0xffffffff;
	regs[REG_AR3].lo_write_mask = 0xffffffff;
	regs[REG_AR4].lo_write_mask = 0xffffffff;
	regs[REG_AR5].lo_write_mask = 0xffffffff;
	regs[REG_AR6].lo_write_mask = 0xffffffff;
	regs[REG_AR7].lo_write_mask = 0xffffffff;
	regs[REG_DP].lo_write_mask = 0xffffffff;
	regs[REG_IR0].lo_write_mask = 0xffffffff;
	regs[REG_IR1].lo_write_mask = 0xffffffff;
	regs[REG_BK].lo_write_mask = 0xffffffff;
	regs[REG_SP].lo_write_mask = 0xffffffff;
	regs[REG_ST].lo_write_mask = ST_WRITE_MASK;
	regs[REG_IE].lo_write_mask = IE_WRITE_MASK;
	regs[REG_IF].lo_write_mask = IF_WRITE_MASK;
	regs[REG_IOF].lo_write_mask = IOF_WRITE_MASK;
	regs[REG_RS].lo_write_mask = 0xffffffff;
	regs[REG_RE].lo_write_mask = 0xffffffff;
	regs[REG_RC].lo_write_mask = 0xffffffff;

	registers_registry["PC"] = new unisim::util::debug::SimpleRegister<uint32_t>("PC", &reg_pc);

	unsigned int i;
	for(i = 0; i < 8; i++)
	{
		stringstream sstr;
		sstr << "R" << i;
		registers_registry[sstr.str().c_str()] = new ExtendedPrecisionRegisterDebugInterface(sstr.str().c_str(), &regs[REG_R0 + i]);
	}

	for(i = 0; i < 8; i++)
	{
		stringstream sstr;
		sstr << "AR" << i;
		registers_registry[sstr.str().c_str()] = new unisim::util::debug::SimpleRegister<uint32_t>(sstr.str().c_str(), &regs[REG_AR0 + i].lo);
	}

	registers_registry["DP"] = new unisim::util::debug::SimpleRegister<uint32_t>("DP", &regs[REG_DP].lo);
	registers_registry["IR0"] = new unisim::util::debug::SimpleRegister<uint32_t>("IR0", &regs[REG_IR0].lo);
	registers_registry["IR1"] = new unisim::util::debug::SimpleRegister<uint32_t>("IR1", &regs[REG_IR1].lo);
	registers_registry["BK"] = new unisim::util::debug::SimpleRegister<uint32_t>("BK", &regs[REG_BK].lo);
	registers_registry["SP"] = new unisim::util::debug::SimpleRegister<uint32_t>("SP", &regs[REG_SP].lo);
	registers_registry["ST"] = new unisim::util::debug::SimpleRegister<uint32_t>("ST", &regs[REG_ST].lo);
	registers_registry["IE"] = new unisim::util::debug::SimpleRegister<uint32_t>("IE", &regs[REG_IE].lo);
	registers_registry["IF"] = new unisim::util::debug::SimpleRegister<uint32_t>("IF", &regs[REG_IF].lo);
	registers_registry["IOF"] = new unisim::util::debug::SimpleRegister<uint32_t>("IOF", &regs[REG_IOF].lo);
	registers_registry["RS"] = new unisim::util::debug::SimpleRegister<uint32_t>("RS", &regs[REG_RS].lo);
	registers_registry["RE"] = new unisim::util::debug::SimpleRegister<uint32_t>("RE", &regs[REG_RE].lo);
	registers_registry["RC"] = new unisim::util::debug::SimpleRegister<uint32_t>("RC", &regs[REG_RC].lo);

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
	
	if(CONFIG::INSN_CACHE_ASSOCIATIVITY > 2)
	{
		logger << DebugError << "Instruction cache associativity must be 1 or 2" << EndDebugError;
		return false;
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
	
	if (!memory_access_reporting_import)
	{
		if (VerboseSetup())
			logger << "- memory access reporting not active";
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}
	
	Reset();

	return success;
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
	reg_pc = 0;
	reg_npc = 0;

	unsigned int reg_num;
	for(reg_num = 0; reg_num < sizeof(regs) / sizeof(regs[0]); reg_num++)
	{
		regs[reg_num].lo = 0;
		regs[reg_num].hi = 0;
	}

	reset = true;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
ReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	return memory_import ? memory_import->ReadMemory(addr, buffer, size) : false;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
WriteMemory(uint64_t addr, const void *buffer, uint32_t size)
{
	return memory_import ? memory_import->WriteMemory(addr, buffer, size) : false;
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
	return (reg_iter != registers_registry.end()) ? (*reg_iter).second : 0;
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
string CPU<CONFIG, DEBUG>::DisasmDir(address_t pc, uint32_t direct)
{
	stringstream sstr;
	sstr << "@0x" << hex << direct << dec;
	if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetDP() << 16) | direct) << ">";
	return sstr.str();
}

template <class CONFIG, bool DEBUG>
bool CPU<CONFIG, DEBUG>::DisasmIndir(string& s, address_t pc, unsigned int mod, unsigned int ar, uint32_t disp)
{
	stringstream sstr;

	switch(mod)
	{
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_ADD:
			sstr << "*+AR" << ar << "("<< disp << ")";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) + disp) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_SUBSTRACT:
			sstr << "*-AR" << ar << "("<< disp << ")";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) - disp) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_ADD_AND_MODIFY:
			sstr << "*++AR" << ar << "("<< disp << ")";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) + disp) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_SUBSTRACT_AND_MODIFY:
			sstr << "*--AR" << ar << "("<< disp << ")";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) - disp) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_ADD_AND_MODIFY:
			sstr << "*AR" << ar << "++("<< disp << ")";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_SUBSTRACT_AND_MODIFY:
			sstr << "*AR" << ar << "--("<< disp << ")";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_ADD_AND_CIRCULAR_MODIFY:
			sstr << "*AR" << ar << "++("<< disp << ")%";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_SUBSTRACT_AND_CIRCULAR_MODIFY:
			sstr << "*AR" << ar << "--("<< disp << ")%";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_ADD:
			sstr << "*+AR" << ar << "(IR0)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) + GetIR0_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_SUBSTRACT:
			sstr << "*-AR" << ar << "(IR0)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) - GetIR0_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_ADD_AND_MODIFY:
			sstr << "*++AR" << ar << "(IR0)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) + GetIR0_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_SUBSTRACT_AND_MODIFY:
			sstr << "*--AR" << ar << "(IR0)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) - GetIR0_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_MODIFY:
			sstr << "*AR" << ar << "++(IR0)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_SUBSTRACT_AND_MODIFY:
			sstr << "*AR" << ar << "--(IR0)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_CIRCULAR_MODIFY:
			sstr << "*AR" << ar << "++(IR0)%";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_SUBSTRACT_AND_CIRCULAR_MODIFY:
			sstr << "*AR" << ar << "--(IR0)%";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_ADD:
			sstr << "*+AR" << ar << "(IR1)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) + GetIR1_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_SUBSTRACT:
			sstr << "*-AR" << ar << "(IR1)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) - GetIR1_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_ADD_AND_MODIFY:
			sstr << "*++AR" << ar << "(IR1)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) + GetIR1_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_SUBSTRACT_AND_MODIFY:
			sstr << "*--AR" << ar << "(IR1)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName((GetAR23_0(ar) - GetIR1_23_0()) & ADDRESS_MASK) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_ADD_AND_MODIFY:
			sstr << "*AR" << ar << "++(IR1)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_SUBSTRACT_AND_MODIFY:
			sstr << "*AR" << ar << "--(IR1)";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_ADD_AND_CIRCULAR_MODIFY:
			sstr << "*AR" << ar << "++(IR1)%";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_SUBSTRACT_AND_CIRCULAR_MODIFY:
			sstr << "*AR" << ar << "--(IR1)%";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING:
			sstr << "*AR" << ar;
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
			s = sstr.str();
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_BIT_REVERSED_MODIFY:
			sstr << "*AR" << ar << "++(IR0)B";
			if(pc == GetPC()) sstr << " <" << GetObjectFriendlyName(GetAR23_0(ar)) << ">";
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

template<class CONFIG, bool DEBUG>
string
CPU<CONFIG, DEBUG> ::
GetObjectFriendlyName(address_t addr)
{
	stringstream sstr;
	
	const Symbol<uint64_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<uint64_t>::SYM_OBJECT) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "@0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template<class CONFIG, bool DEBUG>
string
CPU<CONFIG, DEBUG> ::
GetFunctionFriendlyName(address_t addr)
{
	stringstream sstr;
	
	const Symbol<uint64_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<uint64_t>::SYM_FUNC) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "@0x" << std::hex << addr << std::dec;

	return sstr.str();
}

//===============================================================
//= DebugDisasmInterface interface methods                 STOP =
//===============================================================

//===============================================================
//= Effective address calculation                         START =
//===============================================================

template<class CONFIG, bool DEBUG>
uint32_t
CPU<CONFIG, DEBUG> ::
CircularAdd(uint32_t ar, uint32_t bk, uint32_t step)
{
	if(bk > MAX_BLOCK_SIZE)
	{
		logger << DebugWarning << "Circular buffer length (stored in register BK) is greater than 64K" << EndDebugWarning;
	}
	if(step > bk)
	{
		logger << DebugWarning << "Step is greater than block-size/circular buffer length (stored in register BK)" << EndDebugWarning;
	}
	// Compute K
	unsigned int k = 1 + Log2(bk);

	// K LSB Mask
	address_t k_lsb_mask = (1 << k) - 1;

	// Compute the circular buffer base address
	address_t base_addr = ar & ~k_lsb_mask;

	// Compute the new index in the circular buffer
	int32_t index = (int32_t) (ar & k_lsb_mask) + (int32_t) step;
	if(index > bk) index = index - bk;

	// Return the new circular address
	return (base_addr + index) & ADDRESS_MASK;
}

template<class CONFIG, bool DEBUG>
uint32_t
CPU<CONFIG, DEBUG> ::
CircularSubstract(uint32_t ar, uint32_t bk, uint32_t step)
{
	if(bk > MAX_BLOCK_SIZE)
	{
		logger << DebugWarning << "Circular buffer length (stored in register BK) is greater than 64K" << EndDebugWarning;
	}
	if(step > bk)
	{
		logger << DebugWarning << "Step is greater than block-size/circular buffer length (stored in register BK)" << EndDebugWarning;
	}
	// Compute K
	unsigned int k = 1 + Log2(bk);

	// K LSB Mask
	address_t k_lsb_mask = (1 << k) - 1;

	// Compute the circular buffer base address
	address_t base_addr = ar & ~k_lsb_mask;

	// Compute the new index in the circular buffer
	int32_t index = (int32_t) (ar & k_lsb_mask) - (int32_t) step;
	if(index < 0) index = index + bk;

	// Return the new circular address
	return (base_addr + index) & ADDRESS_MASK;
}

template<class CONFIG, bool DEBUG>
bool 
CPU<CONFIG, DEBUG> ::
ComputeIndirEA(address_t& output_ea, bool& update_ar, address_t& output_ar, unsigned int mod, unsigned int ar_num, uint32_t disp)
{
	// Read ARn
	address_t ar = GetAR23_0(ar_num);

	switch(mod)
	{
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_ADD:
			// mnemonic: *+ARn(disp)
			{
				// Compute the effective address
				address_t ea = (ar + disp) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_SUBSTRACT:
			// mnemonic: *-ARn(disp)
			{
				// Compute the effective address
				address_t ea = (ar - disp) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_ADD_AND_MODIFY:
			// mnemonic: *++ARn(disp)
			{
				// Compute the effective address
				address_t ea = (ar + disp) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ea;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_SUBSTRACT_AND_MODIFY:
			// mnemonic: *--ARn(disp)
			{
				// Compute the effective address
				address_t ea = (ar - disp) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ea;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_ADD_AND_MODIFY:
			// mnemonic: *ARn++(disp)
			{
				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = (ar + disp) & ADDRESS_MASK;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_SUBSTRACT_AND_MODIFY:
			// mnemonic: *AR--(disp)
			{
				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = (ar - disp) & ADDRESS_MASK;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_ADD_AND_CIRCULAR_MODIFY:
			// mnemonic: *ARn++(disp)%
			{
				// Read BK
				uint32_t bk = GetBK();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = CircularAdd(ar, bk, disp);
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_SUBSTRACT_AND_CIRCULAR_MODIFY:
			// mnemonic: *ARn--(disp)%
			{
				// Read BK
				uint32_t bk = GetBK();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = CircularSubstract(ar, bk, disp);
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_ADD:
			// mnemonic: *+ARn(IR0)
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = (ar + ir0) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_SUBSTRACT:
			// mnemonic: *-ARn(IR0)
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = (ar - ir0) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_ADD_AND_MODIFY:
			// mnemonic: *++ARn(IR0)
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = (ar + ir0) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ea;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_SUBSTRACT_AND_MODIFY:
			// mnemonic: *--ARn(IR0)
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = (ar - ir0) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ea;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_MODIFY:
			// mnemonic: *ARn++(IR0)
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = (ar + ir0) & ADDRESS_MASK;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_SUBSTRACT_AND_MODIFY:
			// mnemonic: *ARn--(IR0)
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = (ar - ir0) & ADDRESS_MASK;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_CIRCULAR_MODIFY:
			// mnemonic: *ARn++(IR0)%
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Read BK
				uint32_t bk = GetBK();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = CircularAdd(ar, bk, ir0);
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_SUBSTRACT_AND_CIRCULAR_MODIFY:
			// mnemonic: *ARn--(IR0)%
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Read BK
				uint32_t bk = GetBK();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = CircularSubstract(ar, bk, ir0);
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_ADD:
			// mnemonic: *+ARn(IR1)
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Compute the effective address
				address_t ea = (ar + ir1) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_SUBSTRACT:
			// mnemonic: *-ARn(IR1)
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Compute the effective address
				address_t ea = (ar - ir1) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_ADD_AND_MODIFY:
			// mnemonic: *++ARn(IR1)
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Compute the effective address
				address_t ea = (ar + ir1) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ea;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_SUBSTRACT_AND_MODIFY:
			// mnemonic: *--ARn(IR1)
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Compute the effective address
				address_t ea = (ar - ir1) & ADDRESS_MASK;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ea;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_ADD_AND_MODIFY:
			// mnemonic: *ARn++(IR1)
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = (ar + ir1) & ADDRESS_MASK;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_SUBSTRACT_AND_MODIFY:
			// mnemonic: *ARn--(IR1)
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = (ar - ir1) & ADDRESS_MASK;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_ADD_AND_CIRCULAR_MODIFY:
			// mnemonic: *ARn++(IR1)%
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Read BK
				uint32_t bk = GetBK();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = CircularAdd(ar, bk, ir1);
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_SUBSTRACT_AND_CIRCULAR_MODIFY:
			// mnemonic: *ARn--(IR1)%
			{
				// Read IR1
				uint32_t ir1 = GetIR1_23_0();

				// Read BK
				uint32_t bk = GetBK();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = CircularSubstract(ar, bk, ir1);
			}
			return true;
		case MOD_INDIRECT_ADDRESSING:
			// mnemonic: *ARn
			{
				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Do not update ARn
				update_ar = false;
			}
			return true;
		case MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_BIT_REVERSED_MODIFY:
			// mnemonic: *ARn++(IR0)B
			{
				// Read IR0
				uint32_t ir0 = GetIR0_23_0();

				// Compute the effective address
				address_t ea = ar;

				// Output the effective address
				output_ea = ea;

				// Compute and output update for ARn
				update_ar = true;
				output_ar = ReverseCarryPropagationAdd(ar, ir0);
			}
			return true;
	}
	return false;
}

template<class CONFIG, bool DEBUG>
inline
typename CONFIG::address_t 
CPU<CONFIG, DEBUG> ::
ComputeDirEA(uint32_t direct) const
{
	return (GetDP() << 16) | direct;
}

//===============================================================
//= Effective address calculation                          STOP =
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
	if(unlikely(debug_control_import != 0))
	{
		do
		{
			typename DebugControl<uint64_t>::DebugCommand dbg_cmd;

			dbg_cmd = debug_control_import->FetchDebugCommand(4 * reg_pc);
	
			if(dbg_cmd == DebugControl<uint64_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<uint64_t>::DBG_KILL) Stop(0);
		} while(1);
	}

	try
	{
		if(unlikely(reset))
		{
			// Load the reset interrupt handler address
			address_t reset_interrupt_handler_addr = IntLoad(0);

			// Branch to the reset interrupt handler
			reg_pc = reset_interrupt_handler_addr;

			// Fetch first instruction of the interrupt handler
			reg_ir = Fetch(reg_pc & ADDRESS_MASK);

			// Compute the address of the next instruction, i.e. PC + 1
			reg_npc = reg_pc + 1;

			// Reset finished
			reset = false;
		}
		else
		{
			// Check if there are some enabled pending IRQs
			if(unlikely(GetIF() & GetIE() && !GetST_GIE()))
			{
				unsigned int irq_num;

				// Select an IRQ according priority (from the higher to the lower)
				if(unlikely(!BitScanForward(irq_num, GetIF())))
				{
					throw InternalErrorException<CONFIG, DEBUG>("No pending IRQ found");
				}

				// Read SP and compute its new value
				typename CONFIG::address_t sp = GetSP() + 1;

				// Store the PC at SP + 1
				IntStore(sp & ADDRESS_MASK, reg_pc);

				// Update SP
				SetSP(sp);

				// Load the interrupt handler address
				address_t interrupt_handler_addr = IntLoad(irq_num + 1);

				// Branch to interrupt handler
				reg_pc = interrupt_handler_addr;

				// Reset ST[GIE] to disable further IRQs until reactivated by software
				ResetST_GIE();

				// Reset bit corresponding to the IRQ in register IF
				SetIRQLevel(irq_num, false);

				// Fetch first instruction of the interrupt handler
				reg_ir = Fetch(reg_pc & ADDRESS_MASK);

				// Compute the address of the next instruction, i.e. PC + 1
				reg_npc = reg_pc + 1;
			}
			else
			{
				// Check whether the processor is running in repeat mode (RPTB or RPTS)
				if(unlikely(GetST_RM()))
				{
					// Check whether this is a repeat single (RPTS)
					if(repeat_single)
					{
						// Check whether instruction to repeat has already been fetched from memory to IR
						if(unlikely(first_time_through_repeat_single))
						{
							// Fetch the instruction from memory into IR
							reg_ir = Fetch(reg_pc & ADDRESS_MASK);
							first_time_through_repeat_single = false;
						}

						// Decrement RC
						regs[REG_RC].lo = regs[REG_RC].lo - 1;

						// Check if RC is < 0
						if((int32_t) regs[REG_RC].lo < 0)
						{
							// Disable the repeat mode by resetting ST[RM] bit
							ResetST_RM();
							repeat_single = false;
							// Compute the address of the next instruction, i.e. PC + 1
							reg_npc = reg_pc + 1;
						}
						else
						{
							// Compute the address of the next instruction, i.e. PC so that current instruction in IR will be repeated again
							reg_npc = reg_pc;
						}
					}
					else
					{
						// Fetch the instruction from memory into IR
						reg_ir = Fetch(reg_pc & ADDRESS_MASK);

						// Check whether the end of the block to repeat has been reached
						if(reg_pc == regs[REG_RE].lo)
						{
							// Decrement RC
							regs[REG_RC].lo = regs[REG_RC].lo - 1;

							// Check if RC is >= 0
							if((int32_t) regs[REG_RC].lo >= 0)
							{
								// Compute the address the next instruction, i.e. the start of the block to repeat
								reg_npc = regs[REG_RS].lo;
							}
							else
							{
								// Disable the repeat mode by resetting ST[RM] bit
								ResetST_RM();
								repeat_single = false;
								// Compute the address of the next instruction, i.e. PC + 1
								reg_npc = reg_pc + 1;
							}
						}
						else
						{
							// Compute the address of the next instruction, i.e. PC + 1
							reg_npc = reg_pc + 1;
						}
					}
				}
				else
				{
					// Fetch the instruction from memory into IR
					reg_ir = Fetch(reg_pc & ADDRESS_MASK);

					// Compute the address of the next instruction, i.e. PC + 1
					reg_npc = reg_pc + 1;
				}
			}
		}

		// Decode the instruction
		typename isa::tms320::Operation<CONFIG, DEBUG> *operation = decoder.Decode(4 * reg_pc, reg_ir);

		// Execute the instruction
		operation->execute(*this);

		// Check whether a branch is pending
		if(unlikely(delay_before_branching))
		{
			// Decrement the delay and branch once it has reached zero
			if(--delay_before_branching == 0)
			{
				reg_npc = branch_addr;
			}
		}

		/* go to the next instruction */
		reg_pc = reg_npc;

		/* update the instruction counter */
		instruction_counter++;

		if(unlikely(requires_finished_instruction_reporting))
		{
			if(unlikely(memory_access_reporting_import != 0))
			{
				memory_access_reporting_import->ReportFinishedInstruction(4 * reg_pc);
			}
		}
	}
	catch(BogusOpcodeException<CONFIG, DEBUG>& exc)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
	}
	catch(UnimplementedOpcodeException<CONFIG, DEBUG>& exc)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
	}
	catch(UnknownOpcodeException<CONFIG, DEBUG>& exc)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
	}
	catch(BadMemoryAccessException<CONFIG, DEBUG>& exc)
	{
		logger << DebugError << exc.what() << EndDebugError;
		Stop(-1);
	}
	catch(Exception& e)
	{
		logger << DebugError << "uncaught exception :" << e.what() << EndDebugError;
		Stop(-1);
	}

	if(unlikely(instruction_counter >= max_inst)) Stop(0);
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

template<class CONFIG, bool DEBUG>
inline
void
CPU<CONFIG, DEBUG> ::
IntStore(address_t ea, uint32_t value)
{
	value = Host2LittleEndian(value);

	if(unlikely(!PrWrite(ea, &value, sizeof(value))))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(ea);
	}

	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_WRITE, MemoryAccessReporting<uint64_t>::MT_DATA, 4 * ea, 4);
	}
}

template<class CONFIG, bool DEBUG>
inline
uint32_t
CPU<CONFIG, DEBUG> ::
IntLoad(address_t ea)
{
	uint32_t value;

	if(unlikely(!PrRead(ea, &value, sizeof(value))))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(ea);
	}

	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ, MemoryAccessReporting<uint64_t>::MT_DATA, 4 * ea, 4);
	}

	return LittleEndian2Host(value);
}

template<class CONFIG, bool DEBUG>
inline
uint32_t
CPU<CONFIG, DEBUG> ::
Fetch(address_t addr)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ, MemoryAccessReporting<uint64_t>::MT_INSN, 4 * addr, 4);
	}

	uint32_t insn;
	InsnCacheSet *insn_cache_set;

	// Check wether instruction cache is being cleared
	if(unlikely(CONFIG::ENABLE_INSN_CACHE && GetST_CC()))
	{
		uint32_t insn_cache_index;
		for(insn_cache_index = 0; insn_cache_index < NUM_INSN_CACHE_SETS; insn_cache_index++)
		{
			uint32_t insn_cache_way;

			insn_cache_set = &insn_cache[insn_cache_index];
			insn_cache_set->mru_way = 0;

			for(insn_cache_way = 0; insn_cache_way < INSN_CACHE_ASSOCIATIVITY; insn_cache_way++)
			{
				InsnCacheBlock *insn_cache_block = &insn_cache_set->blocks[insn_cache_way];
				insn_cache_block->valid = false;
			}
		}
		ResetST_CC();
	}

	// Check wether instruction cache is enabled
	if(likely(CONFIG::ENABLE_INSN_CACHE && GetST_CE()))
	{
		uint32_t insn_cache_index;
		insn_cache_index = addr % NUM_INSN_CACHE_SETS;
		insn_cache_set = &insn_cache[insn_cache_index];

		// Associative search
		uint32_t insn_cache_way;
		unsigned i;

		// the most recently used way is the first that is checked
		for(insn_cache_way = insn_cache_set->mru_way, i = 0; i < INSN_CACHE_ASSOCIATIVITY; insn_cache_way = insn_cache_way ^ 1, i++)
		{
			InsnCacheBlock *insn_cache_block = &insn_cache_set->blocks[insn_cache_way];
			if(insn_cache_block->valid && insn_cache_block->addr == addr)
			{
				// Hit
				insn_cache_hits++;

				// Read the instruction word from the cache block
				insn = insn_cache_block->insn;

				// Update the replacement policy
				if(INSN_CACHE_ASSOCIATIVITY > 1 && !GetST_CF())
				{
					insn_cache_set->mru_way = insn_cache_way;
				}

				return LittleEndian2Host(insn);
			}
		}
	}

	// Fetch instruction from memory
	if(unlikely(!PrRead(addr, &insn, sizeof(insn))))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(addr);
	}

	if(likely(CONFIG::ENABLE_INSN_CACHE && GetST_CE() && !GetST_CF()))
	{
		// Instruction cache miss
		insn_cache_misses++;

		// Choose a way
		uint32_t insn_cache_way = (INSN_CACHE_ASSOCIATIVITY > 1) ? insn_cache_set->mru_way ^ 1 : 0;
			
		InsnCacheBlock *insn_cache_block = &insn_cache_set->blocks[insn_cache_way];

		// Refill instruction cache
		insn_cache_block->valid = true;
		insn_cache_block->addr = addr;
		insn_cache_block->insn = insn;

		// Update the replacement policy
		if(INSN_CACHE_ASSOCIATIVITY > 1)
		{
			insn_cache_set->mru_way = insn_cache_way;
		}
	}

	return LittleEndian2Host(insn);
}

template<class CONFIG, bool DEBUG>
inline
bool
CPU<CONFIG, DEBUG> ::
CheckCondition(unsigned int cond) const
{
	uint32_t st = GetST();

	switch(cond)
	{
		case COND_U: // unconditional
			return true;
		case COND_LO: // C
			return st & M_ST_C;
		case COND_LS: // C OR Z
			return ((st >> ST_C) | (st >> ST_Z)) & 1;
		case COND_HI: // ~C OR ~Z <==> ~(C AND Z)
			return (~((st >> ST_C) & (st >> ST_Z)) & 1);
		case COND_HS: // ~C
			return !(st & M_ST_C);
		case COND_EQ: // Z
			return st & M_ST_Z;
		case COND_NE: // ~Z
			return !(st & M_ST_Z);
		case COND_LT: // N
			return st & M_ST_N;
		case COND_LE: // N OR Z
			return ((st >> ST_N) | (st >> ST_Z)) & 1;
		case COND_GT: // ~N AND ~Z <==> ~(N OR Z)
			return (~((st >> ST_N) | (st >> ST_Z))) & 1;
		case COND_GE: // ~N
			return !(st & M_ST_N);
		case COND_NV: // ~V
			return !(st & M_ST_V);
		case COND_V: // V
			return st & M_ST_V;
		case COND_NUF: // ~UF
			return !(st & M_ST_UF);
		case COND_UF: // UF
			return st & M_ST_UF;
		case COND_NLV: // ~LV
			return !(st & M_ST_LV);
		case COND_LV: // LV
			return st & ST_LV;
		case COND_NLUF: // ~LUF
			return !(st & M_ST_LUF);
		case COND_LUF: // LUF
			return st & M_ST_LUF;
		case COND_ZUF: // Z OR UF
			return ((st >> ST_Z) | (st >> ST_UF)) & 1;
	}
	return false;
}

template<class CONFIG, bool DEBUG>
inline
void
CPU<CONFIG, DEBUG> ::
GenFlags(uint32_t result, uint32_t reset_mask, uint32_t or_mask, uint32_t carry_out, uint32_t overflow)
{
	// Read ST
	uint32_t st = GetST();

	// Apply a reset mask
	st = st & ~reset_mask;

	// LV
	if(or_mask & M_ST_LV) st |= (overflow << ST_LV);

	// N
	if(or_mask & M_ST_N)
	{
		uint32_t is_negative = ((int32_t) result < 0);
		st |= (is_negative << ST_N);
	}

	// Z
	if(or_mask & M_ST_Z)
	{
		uint32_t is_zero = ((int32_t) result == 0);
		st |= (is_zero << ST_Z);
	}

	// C
	if(or_mask & M_ST_C) st |= (carry_out << ST_C);

	// V
	if(or_mask & M_ST_V) st |= (overflow << ST_V);

	// Write back ST
	SetST(st);
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

