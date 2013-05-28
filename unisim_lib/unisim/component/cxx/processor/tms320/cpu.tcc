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
#include "unisim/util/arithmetic/arithmetic.hh"

#include "unisim/util/debug/simple_register.hh"
#include <iomanip>
#include <sstream>

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
using std::stringstream;
using std::map;

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
	Client<TrapReporting>(name, parent),
	Client<SymbolTableLookup<uint64_t> >(name, parent),
	Service<MemoryInjection<uint64_t> >(name, parent),
	Service<MemoryAccessReportingControl> (name, parent),
	Service<Disassembly<uint64_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<Memory<uint64_t> >(name, parent),
	Client<Memory<uint64_t> >(name, parent),
	Client<TI_C_IO>(name, parent),
	Client<Loader>(name, parent),
	logger(*this),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
	memory_export("memory_export", this),
	memory_injection_export("memory_injection_export", this),
	memory_access_reporting_control_export(
		"memory_access_reporting_control_export",
		this),
	debug_control_import("debug_control_import", this),
	memory_access_reporting_import("memory_access_reporting_import", this),
	trap_reporting_import("trap-reporting-import", this),
	symbol_table_lookup_import("symbol_table_lookup_import", this),
	memory_import("memory_import", this),
	ti_c_io_import("ti-c-io-import", this),
	requires_memory_access_reporting(true),
	requires_finished_instruction_reporting(true),
	instruction_counter(0),
	trap_on_instruction_counter(0xffffffffffffffffULL),
	max_inst(0xffffffffffffffffULL),
	mimic_dev_board(false),
	param_max_inst("max-inst", this, max_inst),
	param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter),
	stat_instruction_counter("instruction-counter", this, instruction_counter),
	param_mimic_dev_board("mimic-dev-board", this, mimic_dev_board),
	delay_before_branching(0),
	branch_addr(0),
	repeat_single(false),
	first_time_through_repeat_single(false),
	reset(false),
	idle(0),
	reg_ir(0),
	reg_pc(0),
	reg_npc(0),
	insn_cache_hits(0),
	insn_cache_misses(0),
	stat_insn_cache_hits("insn-cache-hits", this, insn_cache_hits),
	stat_insn_cache_misses("insn-cache-misses", this, insn_cache_misses),
	enable_parallel_load_bug(true),
	param_enable_parallel_load_bug("enable-parallel-load-bug", this, enable_parallel_load_bug,
		"When using parallel loads (LDF src2, dst2 || LDF src1, dst1) the src1 load doesn't transform incorrect zero values to valid zero representation, instead they copy the contents of the memory to the register. Set to this parameter to false to transform incorrect zero values."),
	enable_rnd_bug(true),
	param_enable_rnd_bug("enable-rnd-bug", this, enable_rnd_bug,
		"If enabled the `rnd` instruction sets the Z flag to 0 systematically, as it is done in the evaluation board. Otherwise, Z is unchanged as it is written in the documentation."),
	enable_parallel_store_bug(true),
	param_enable_parallel_store_bug("enable-parallel-store-bug", this, enable_parallel_store_bug,
		"If enabled, when using parallel stores (STF src2, dst2 || STF src1, dst1) the first store is treated as a NOP."),
	enable_float_ops_with_non_ext_regs(false),
	param_enable_float_ops_with_non_ext_regs("enable-float-ops-with-non-ext-regs", this, enable_float_ops_with_non_ext_regs,
		"If enabled non extended registers can be used on all the float instructions, however the behavior is not documented and can differ between chips revision. If disabled, it stops simulation when using non extended registers on float instructions."),
	verbose_all(false),
	verbose_setup(false),
	param_verbose_all("verbose-all", this, verbose_all),
	param_verbose_setup("verbose-setup", this, verbose_setup)
{
	regs[REG_ST].SetLoWriteMask(ST_WRITE_MASK);
	regs[REG_IE].SetLoWriteMask(IE_WRITE_MASK);
	regs[REG_IF].SetLoWriteMask(IF_WRITE_MASK);
	regs[REG_IOF].SetLoWriteMask(IOF_WRITE_MASK);

	registers_registry["PC"] = new unisim::util::debug::SimpleRegister<uint32_t>("PC", &reg_pc);

	unsigned int i;
	for (i = 8; i < 32; i++)
		regs[i].SetHiWriteMask(0, (uint8_t)0x08);

	for(i = 0; i < 8; i++)
	{
		stringstream sstr;
		sstr << "R" << i;
		registers_registry[sstr.str().c_str()] = new RegisterDebugInterface(sstr.str().c_str(), &regs[REG_R0 + i], true /* extended precision */);
	}

	for(i = 0; i < 8; i++)
	{
		stringstream sstr;
		sstr << "AR" << i;
		registers_registry[sstr.str().c_str()] = new RegisterDebugInterface(sstr.str().c_str(), &regs[REG_AR0 + i]);
	}

	registers_registry["DP"] = new RegisterDebugInterface("DP", &regs[REG_DP]);
	registers_registry["IR0"] = new RegisterDebugInterface("IR0", &regs[REG_IR0]);
	registers_registry["IR1"] = new RegisterDebugInterface("IR1", &regs[REG_IR1]);
	registers_registry["BK"] = new RegisterDebugInterface("BK", &regs[REG_BK]);
	registers_registry["SP"] = new RegisterDebugInterface("SP", &regs[REG_SP]);

	registers_registry["ST"] = new RegisterDebugInterface("ST", &regs[REG_ST]);
	registers_registry["ST:C"] = new RegisterBitFieldDebugInterface("ST:C", &regs[REG_ST], ST_C);
	registers_registry["ST:V"] = new RegisterBitFieldDebugInterface("ST:V", &regs[REG_ST], ST_V);
	registers_registry["ST:Z"] = new RegisterBitFieldDebugInterface("ST:Z", &regs[REG_ST], ST_Z);
	registers_registry["ST:N"] = new RegisterBitFieldDebugInterface("ST:N", &regs[REG_ST], ST_N);
	registers_registry["ST:UF"] = new RegisterBitFieldDebugInterface("ST:UF", &regs[REG_ST], ST_UF);
	registers_registry["ST:LV"] = new RegisterBitFieldDebugInterface("ST:LV", &regs[REG_ST], ST_LV);
	registers_registry["ST:LUF"] = new RegisterBitFieldDebugInterface("ST:LUF", &regs[REG_ST], ST_LUF);
	registers_registry["ST:OVM"] = new RegisterBitFieldDebugInterface("ST:OVM", &regs[REG_ST], ST_OVM);
	registers_registry["ST:RM"] = new RegisterBitFieldDebugInterface("ST:RM", &regs[REG_ST], ST_RM);
	registers_registry["ST:CF"] = new RegisterBitFieldDebugInterface("ST:CF", &regs[REG_ST], ST_CF);
	registers_registry["ST:CE"] = new RegisterBitFieldDebugInterface("ST:CE", &regs[REG_ST], ST_CE);
	registers_registry["ST:CC"] = new RegisterBitFieldDebugInterface("ST:CC", &regs[REG_ST], ST_CC);
	registers_registry["ST:GIE"] = new RegisterBitFieldDebugInterface("ST:GIE", &regs[REG_ST], ST_GIE);

	registers_registry["IE"] = new RegisterDebugInterface("IE", &regs[REG_IE]);
	registers_registry["IF"] = new RegisterDebugInterface("IF", &regs[REG_IF]);
	registers_registry["IOF"] = new RegisterDebugInterface("IOF", &regs[REG_IOF]);
	registers_registry["RS"] = new RegisterDebugInterface("RS", &regs[REG_RS]);
	registers_registry["RE"] = new RegisterDebugInterface("RE", &regs[REG_RE]);
	registers_registry["RC"] = new RegisterDebugInterface("RC", &regs[REG_RC]);

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
BeginSetup() 
{
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
	reg_pc = 0;
	reg_npc = 0;

	unsigned int reg_num;
	for(reg_num = 0; reg_num < sizeof(regs) / sizeof(regs[0]); reg_num++)
	{
		regs[reg_num].SetLo(0);
		regs[reg_num].SetHi(0);
	}

	if(unlikely(verbose_setup))
	{
		logger << DebugInfo << "Reseting" << EndDebugInfo;
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

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
InjectReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	if(unlikely(!PrRead(addr, buffer, size, false)))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(addr / 4);
	}

	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, 4);
	}

	return true;
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
InjectWriteMemory(uint64_t addr, const void *buffer, uint32_t size)
{
	if(unlikely(!PrWrite(addr, buffer, size, false)))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(addr / 4);
	}

	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, 4);
	}

	return true;
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
unisim::util::debug::Register *
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
	uint32_t input_fraction = 0;
	int32_t input_decimal = 0;
	long double value = 0.0;
	
	// compute exponent
	int16_t exponent = (int16_t)x;
	exponent = exponent >> 12;
	if (exponent == -8) 
		// the coded value is 0
		sstr << (long double)value;
	else
	{
		// get the input fraction and decimal parts of the input value x
		if (x & (uint16_t)0x0800)
			input_decimal = (int32_t)((uint32_t)0xfffff000 | (uint32_t)(x & ~(uint16_t)0x0800));
		else
			input_decimal = (int32_t)((uint32_t)(x & (uint16_t)0x07ff) | (uint32_t)0x0800);
		input_fraction = (uint32_t)input_decimal;
		input_decimal = input_decimal >> (11 - exponent);
		input_fraction = input_fraction << (21 + exponent);
		
		// compute the float value
		long double factor = 2.0;
		uint32_t mask;
		for (mask = (uint32_t)0x80000000; mask != 0; mask = mask >> 1)
		{
			if (mask & input_fraction)
				value += (long double)1.0/factor;
			factor *= 2.0;
		}
		value += input_decimal;
	}
	
	// and return a formated string
	sstr << std::scientific << value;
	return sstr.str();
}

template<class CONFIG, bool DEBUG>
string
CPU<CONFIG, DEBUG> ::
GetObjectFriendlyName(address_t addr)
{
	stringstream sstr;
	const Symbol<uint64_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(4 * addr, Symbol<uint64_t>::SYM_OBJECT) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(4 * addr) << " @0x" << std::hex << addr << std::dec;
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
	
	const Symbol<uint64_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(4 * addr, Symbol<uint64_t>::SYM_FUNC) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(4 * addr);
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
	if(index >= (int32_t) bk) index = index - bk;

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

			dbg_cmd = debug_control_import->FetchDebugCommand(4 * GetPC23_0());
	
			if(dbg_cmd == DebugControl<uint64_t>::DBG_STEP) break;
			if(dbg_cmd == DebugControl<uint64_t>::DBG_KILL)
			{
				Stop(0);
				return;
			}
		} while(1);
	}

	try
	{
		if(unlikely(reset))
		{
			if(ti_c_io_import && mimic_dev_board)
			{
				if(unlikely(verbose_setup))
				{
					logger << DebugInfo << "Mimicing development board behavior after reset" << EndDebugInfo;
				}

				// Invalidate the instruction cache
				InvalidateInsnCache();

				// Reset TI C I/O service
				ti_c_io_import->Reset();
				
				// Fetch first instruction
				SetIR(Fetch(GetPC23_0()));

				// Compute the address of the next instruction, i.e. PC + 1
				SetNPC(GetPC() + 1);

				reset = false;
			}
			else
			{
				// Invalidate the instruction cache
				InvalidateInsnCache();

				// Load the reset interrupt handler address
				address_t reset_interrupt_handler_addr = IntLoad(0);

				// Branch to the reset interrupt handler
				SetPC(reset_interrupt_handler_addr);

				// Fetch first instruction of the interrupt handler
				SetIR(Fetch(GetPC23_0()));

				// Compute the address of the next instruction, i.e. PC + 1
				SetNPC(GetPC() + 1);
			}

			// Reset finished
			reset = false;
		}
		else
		{
			// Note:
			// Delayed branches disable interrupts until the completion of the three instructions
			// that follow the delayed branch, regardless if the branch is or is not performed. (pg 13-82)

			// Note:
			// The RPTS instruction cannot be interrupted because instruction fetches are
			// halted. (pg 13-212)

			// Check if:
			//   - there are some enabled pending IRQs
			//   - there is no pending branch
			//   - there is no running RPTS instruction
			if(unlikely(GetIF() & GetIE() && !GetST_GIE()) && !delay_before_branching && (!GetST_RM() || !GetS()))
			{
				unsigned int irq_num;

				// Check whether processor is idle
				if(idle)
				{
					// Check whether processor is idle (low power version)
					if(idle >= 2)
					{
						// Check whether one of the INT0-3 external signals is asserted
						if(GetIF() & (M_IF_INT0 | M_IF_INT1 | M_IF_INT2 | M_IF_INT3))
						{
							// wake-up CPU
							idle = 0;
						}
					}
					else
					{
						// wake-up CPU
						idle = 0;
					}
				}

				// Select an IRQ according priority (from the higher to the lower)
				if(unlikely(!BitScanForward(irq_num, GetIF())))
				{
					throw InternalErrorException<CONFIG, DEBUG>("No pending IRQ found");
				}

				// Read SP and compute its new value
				typename CONFIG::address_t sp = GetSP() + 1;

				// Store the PC at SP + 1
				IntStore(GetSP23_0(), GetPC());

				// Update SP
				SetSP(sp);

				// Load the interrupt handler address
				address_t interrupt_handler_addr = IntLoad(irq_num + 1);

				// Branch to interrupt handler
				SetPC(interrupt_handler_addr);

				// Reset ST[GIE] to disable further IRQs until reactivated by software
				ResetST_GIE();

				// Reset bit corresponding to the IRQ in register IF
				SetIRQLevel(irq_num, false);

				// Fetch first instruction of the interrupt handler
				SetIR(Fetch(GetPC23_0()));

				// Compute the address of the next instruction, i.e. PC + 1
				SetNPC(GetPC() + 1);
			}
			else
			{
				// Check whether processor is in idle mode
				if(unlikely(idle)) return; // Note: PC is not incremented

				// Check whether the processor is running in repeat mode (RPTB or RPTS)
				if(unlikely(GetST_RM()))
				{
					// Check whether this is a repeat single (RPTS)
					if(GetS())
					{
						// Check whether instruction to repeat has already been fetched from memory to IR
						if(unlikely(first_time_through_repeat_single))
						{
							// Fetch the instruction from memory into IR
							SetIR(Fetch(GetPC23_0()));
							first_time_through_repeat_single = false;
						}

						// Decrement RC
						SetRC(GetRC() - 1);

						// Check if RC is < 0
						if((int32_t) GetRC() < 0)
						{
							// Disable the repeat mode by resetting ST[RM] bit
							ResetST_RM();
							ResetS();
							// Compute the address of the next instruction, i.e. PC + 1
							SetNPC(GetPC() + 1);
						}
						else
						{
							// Compute the address of the next instruction, i.e. PC so that current instruction in IR will be repeated again
							SetNPC(GetPC());
						}
					}
					else
					{
						// Fetch the instruction from memory into IR
						SetIR(Fetch(GetPC23_0()));

						// Check whether the end of the block to repeat has been reached
						if(GetPC() == GetRE())
						{
							// Decrement RC
							SetRC(GetRC() - 1);

							// Note: RE < RS
							// If RE < RS and the block mode is enabled, the code between RE and RS is
							// bypassed when the program counter encounters the repeat end (RE) address.

							// Check if RC is >= 0
							if((int32_t) GetRC() >= 0)
							{
								// Compute the address the next instruction, i.e. the start of the block to repeat
								SetNPC(GetRS());
							}
							else
							{
								// Disable the repeat mode by resetting ST[RM] bit
								ResetST_RM();
								ResetS();
								// Compute the address of the next instruction, i.e. PC + 1
								SetNPC(GetPC() + 1);
							}
						}
						else
						{
							// Compute the address of the next instruction, i.e. PC + 1
							SetNPC(GetPC() + 1);
						}
					}
				}
				else
				{
					// Fetch the instruction from memory into IR
					SetIR(Fetch(GetPC23_0()));

					// Compute the address of the next instruction, i.e. PC + 1
					SetNPC(GetPC() + 1);
				}
			}
		}

		// Decode the instruction
		// Note: GenISSLib generated decoder handle byte address not word address
		typename isa::tms320::Operation<CONFIG, DEBUG> *operation = decoder.Decode(4 * GetPC23_0(), GetIR());

		// Execute the instruction
		operation->execute(*this);


// 		stringstream sstr;
// 		operation->disasm(*this, sstr);
// 		cerr << "EXECUTE: " << sstr.str() << endl;

		// Check whether a branch is pending
		if(unlikely(delay_before_branching))
		{
			// Decrement the delay and branch once it has reached zero
			if(--delay_before_branching == 0)
			{
				SetNPC(branch_addr);
			}
		}

		if(unlikely(requires_finished_instruction_reporting))
		{
			if(unlikely(memory_access_reporting_import != 0))
			{
				memory_access_reporting_import->ReportFinishedInstruction(4 * GetPC23_0(), 4 * GetNPC23_0());
			}
		}
		
		/* go to the next instruction */
		SetPC(GetNPC());

		/* update the instruction counter */
		instruction_counter++;

		if(unlikely(trap_reporting_import && instruction_counter == trap_on_instruction_counter))
		{
			trap_reporting_import->ReportTrap();
		}
	}
	catch(Exception& exc)
	{
		logger << DebugError << "Unpredictable behavior: " << exc.what() << EndDebugError;
		Stop(-1);
	}

	if(unlikely(instruction_counter >= max_inst)) Stop(0);
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
EnableFloatOpsWithNonExtRegs()
{
	return enable_float_ops_with_non_ext_regs;
}

template<class CONFIG, bool DEBUG>
inline INLINE
bool
CPU<CONFIG, DEBUG> ::
EnableParallelLoadBug()
{
	return enable_parallel_load_bug;
}
	
template<class CONFIG, bool DEBUG>
inline INLINE
bool
CPU<CONFIG, DEBUG> ::
EnableRndBug()
{
	return enable_rnd_bug;
}
	
template<class CONFIG, bool DEBUG>
inline INLINE
bool
CPU<CONFIG, DEBUG> ::
EnableParallelStoreBug()
{
	return enable_parallel_store_bug;
}

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
bool
CPU<CONFIG, DEBUG> ::
PrWrite(address_t addr, const void *buffer, uint32_t size, bool interlocked)
{
	return memory_import ? memory_import->WriteMemory(addr, buffer, size) : false;
}

template<class CONFIG, bool DEBUG>
inline
bool
CPU<CONFIG, DEBUG> ::
PrRead(address_t addr, void *buffer, uint32_t size, bool interlocked)
{
	return memory_import ? memory_import->ReadMemory(addr, buffer, size) : false;
}

template<class CONFIG, bool DEBUG>
inline
void
CPU<CONFIG, DEBUG> ::
IntStore(address_t ea, uint32_t value, bool interlocked)
{
	uint64_t addr = 4 * ea; // convert word effective address to a byte address

	value = Host2LittleEndian(value);

	if(unlikely(!PrWrite(addr, &value, sizeof(value), interlocked)))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(ea);
	}

	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, 4);
	}
}

template<class CONFIG, bool DEBUG>
inline
uint32_t
CPU<CONFIG, DEBUG> ::
IntLoad(address_t ea, bool interlocked)
{
	uint32_t value;
	uint64_t addr = 4 * ea; // convert word effective address to a byte address

	if(unlikely(!PrRead(addr, &value, sizeof(value), interlocked)))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(ea);
	}

	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, 4);
	}

	return LittleEndian2Host(value);
}

template<class CONFIG, bool DEBUG>
inline
void
CPU<CONFIG, DEBUG> ::
InvalidateInsnCache()
{
	InsnCacheSet *insn_cache_set;
	uint32_t insn_cache_index;

	for(insn_cache_index = 0; insn_cache_index < NUM_INSN_CACHE_SETS; insn_cache_index++)
	{
		uint32_t insn_cache_way;

		insn_cache_set = &insn_cache[insn_cache_index];
		insn_cache_set->mru_way = 0;

		for(insn_cache_way = 0; insn_cache_way < INSN_CACHE_ASSOCIATIVITY; insn_cache_way++)
		{
			InsnCacheLine *insn_cache_line = &insn_cache_set->lines[insn_cache_way];
			insn_cache_line->valid = false;

			uint32_t insn_cache_sector;
			for(insn_cache_sector = 0; insn_cache_sector < INSN_CACHE_BLOCKS_PER_LINE; insn_cache_sector++)
			{
				InsnCacheBlock *insn_cache_block = &insn_cache_line->blocks[insn_cache_sector];
				insn_cache_block->valid = false;
			}
		}
	}
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
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, 4 * addr, 4);
	}

	uint32_t insn;
	InsnCacheSet *insn_cache_set = 0;
	uint32_t insn_cache_way = 0;
	InsnCacheLine *insn_cache_line = 0;
	uint32_t insn_cache_line_base_addr = 0;
	uint32_t insn_cache_sector = 0;
	bool insn_cache_line_hit = false;

	// Check wether instruction cache is being cleared
	if(unlikely(CONFIG::ENABLE_INSN_CACHE && GetST_CC()))
	{
		InvalidateInsnCache();
		ResetST_CC();
	}

	// Check wether instruction cache is enabled
	if(likely(CONFIG::ENABLE_INSN_CACHE && GetST_CE()))
	{
		uint32_t insn_cache_index;

		// Decode the address
//		insn_cache_index = addr % NUM_INSN_CACHE_SETS;
		insn_cache_line_base_addr = addr & ~(INSN_CACHE_BLOCKS_PER_LINE - 1);
		insn_cache_sector = addr & (INSN_CACHE_BLOCKS_PER_LINE - 1);
		insn_cache_index = (addr / INSN_CACHE_BLOCKS_PER_LINE) % NUM_INSN_CACHE_SETS;
		insn_cache_set = &insn_cache[insn_cache_index];

		// Associative search
		unsigned i;

		// the most recently used way is the first that is checked
		for(insn_cache_way = insn_cache_set->mru_way, i = 0; i < INSN_CACHE_ASSOCIATIVITY; insn_cache_way = insn_cache_way ^ 1, i++)
		{
			insn_cache_line = &insn_cache_set->lines[insn_cache_way];
			
			if(insn_cache_line->valid && insn_cache_line->base_addr == insn_cache_line_base_addr)
			{
				// Line hit
				insn_cache_line_hit = true;
				InsnCacheBlock *insn_cache_block = &insn_cache_line->blocks[insn_cache_sector];
				if(!insn_cache_block->valid) break;

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
	if(unlikely(!PrRead(4 * addr, &insn, sizeof(insn))))
	{
		throw BadMemoryAccessException<CONFIG, DEBUG>(addr);
	}

	if(likely(CONFIG::ENABLE_INSN_CACHE && GetST_CE() && !GetST_CF()))
	{
		// Instruction cache miss
		insn_cache_misses++;

		if(!insn_cache_line_hit)
		{
			// Choose a way
			insn_cache_way = (INSN_CACHE_ASSOCIATIVITY > 1) ? insn_cache_set->mru_way ^ 1 : 0;
				
			// Invalidate all blocks in the line
			insn_cache_line = &insn_cache_set->lines[insn_cache_way];

			uint32_t i;
			for(i = 0; i < INSN_CACHE_BLOCKS_PER_LINE; i++)
			{
				InsnCacheBlock *insn_cache_block = &insn_cache_line->blocks[i];
				insn_cache_block->valid = false;
			}

			// Allocate line
			insn_cache_line->valid = true;
			insn_cache_line->base_addr = insn_cache_line_base_addr;
		}

		// Fill block
		InsnCacheBlock *insn_cache_block = &insn_cache_line->blocks[insn_cache_sector];
		insn_cache_block->valid = true;
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
void
CPU<CONFIG, DEBUG> ::
SignalInterlock()
{
}

template<class CONFIG, bool DEBUG>
bool
CPU<CONFIG, DEBUG> ::
SWI()
{
	if(!ti_c_io_import || !mimic_dev_board) return false;
	TI_C_IO::Status status = ti_c_io_import->HandleEmulatorInterrupt();

	switch(status)
	{
		case TI_C_IO::ERROR:
			return false;
		case TI_C_IO::EXIT:
			logger << DebugInfo << "Program exited normally" << EndDebugInfo;
			Stop(0);
		case TI_C_IO::OK:
			return true;
	}
	return true;
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
		case COND_HI: // ~C AND ~Z <==> ~(C OR Z)
			return (~((st >> ST_C) | (st >> ST_Z)) & 1);
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
			return st & M_ST_LV;
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
GenFlags(uint32_t result, uint32_t reset_mask, uint32_t or_mask, uint32_t sign, uint32_t carry_out, uint32_t overflow)
{
	// Read ST
	uint32_t st = GetST();

	// Apply a reset mask
	st = st & ~reset_mask;

	// LV
	if(or_mask & M_ST_LV) st |= (overflow << ST_LV);

	// N
	if(or_mask & M_ST_N) st |= (sign << ST_N);

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

template<class CONFIG, bool DEBUG>
inline
void
CPU<CONFIG, DEBUG> ::
GenFlags(const Register& result, uint32_t reset_mask, uint32_t or_mask, uint32_t overflow, uint32_t underflow, uint32_t neg)
{
	// Read ST
	uint32_t st = GetST();
	
	// Apply a reset mask
	st = st & ~reset_mask;
	
	// LUF
	if (or_mask & M_ST_LUF) st |= (underflow << ST_LUF);
	
	// LV
	if (or_mask & M_ST_LV) st |= (overflow << ST_LV);
	
	// UF
	if (or_mask & M_ST_UF) st |= (underflow << ST_UF);
	
	// N
	if(or_mask & M_ST_N) st |= (neg << ST_N);

	// Z
	if (or_mask & M_ST_Z)
		if (underflow == 0)
		{
			uint32_t is_zero = ((result.GetLo() == 0) && (result.GetHi() == (uint8_t)0x80))?1:0;
			st |= (is_zero << ST_Z);
		}
	
	// C
	// if(or_mask & M_ST_C) st |= (carry_out << ST_C);
	
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

