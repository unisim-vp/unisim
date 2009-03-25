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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_HH__

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/component/cxx/processor/tms320/isa_tms320.hh"
#include "unisim/util/endian/endian.hh"

#include <string>
#include <map>
#include <inttypes.h>

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::util::debug::Register;
using std::string;

typedef struct
{
	uint32_t lo; // 32 LSB
	uint8_t hi;  // 8 MSB
} reg_t;

class ExtendedPrecisionRegisterDebugInterface : public unisim::util::debug::Register
{
public:
	ExtendedPrecisionRegisterDebugInterface(const char *name, reg_t *reg);
	virtual ~ExtendedPrecisionRegisterDebugInterface();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	string name;
	reg_t *reg;
};

template<class CONFIG, bool DEBUG = false>
class CPU :
	public Client<DebugControl<uint64_t> >,
	public Client<MemoryAccessReporting<uint64_t> >,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<uint64_t> >,
	public Service<Registers>,
	public Service<Memory<uint64_t> >,
	public Client<Memory<uint64_t> >
{
private:
	typedef typename CONFIG::address_t address_t;
	
	// the kernel logger
	unisim::kernel::logger::Logger logger;
	
public:
	//===============================================================
	//= Client/Service setup methods                          START =
	//===============================================================
	
	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	
	//===============================================================
	//= Client/Service setup methods                           STOP =
	//===============================================================
	
	//===============================================================
	//= Public service imports/exports                        START =
	//===============================================================
	
	ServiceExport<Disassembly<uint64_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<Memory<uint64_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;
	
	ServiceImport<DebugControl<uint64_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<uint64_t> > symbol_table_lookup_import;
	ServiceImport<Memory<uint64_t> > memory_import; // TODO: check for removal
	
	//===============================================================
	//= Public service imports/exports                         STOP =
	//===============================================================
	
    //===============================================================
	//= Client/Service setup methods                          START =
	//===============================================================

	virtual bool Setup();
	virtual void OnDisconnect();
	
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

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report);
	
    //===============================================================
	//= Memory access reporting control interface methods      STOP =
	//===============================================================

    //===============================================================
	//= Memory interface methods                              START =
	//===============================================================

	virtual void Reset();
	virtual bool ReadMemory(uint64_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(uint64_t addr, const void *buffer, uint32_t size);

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
	virtual Register *GetRegister(const char *name);

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
	virtual std::string Disasm(uint64_t addr, uint64_t &next_addr);
	bool DisasmIndir(string& s, unsigned int mod, unsigned int ar, uint32_t disp = 1);
	string DisasmShortFloat(uint16_t x);
	
    //===============================================================
	//= DebugDisasmInterface interface methods                 STOP =
	//===============================================================

    //===============================================================
	//= Effective address calculation                         START =
	//===============================================================

	// Indirect addressing with displacement                                                              mod4-0
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_ADD                            = 0x00; // 00000
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_SUBSTRACT                      = 0x01; // 00001
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_ADD_AND_MODIFY                 = 0x02; // 00010
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREDISPLACEMENT_SUBSTRACT_AND_MODIFY           = 0x03; // 00011
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_ADD_AND_MODIFY                = 0x04; // 00100
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_SUBSTRACT_AND_MODIFY          = 0x05; // 00101
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_ADD_AND_CIRCULAR_MODIFY       = 0x06; // 00110
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTDISPLACEMENT_SUBSTRACT_AND_CIRCULAR_MODIFY = 0x07; // 00111

	// Indirect addressing with index register IR0
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_ADD                               = 0x08; // 01000
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_SUBSTRACT                         = 0x09; // 01001
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_ADD_AND_MODIFY                    = 0x0a; // 01010
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR0_SUBSTRACT_AND_MODIFY              = 0x0b; // 01011
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_MODIFY                   = 0x0c; // 01100
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_SUBSTRACT_AND_MODIFY             = 0x0d; // 01101
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_CIRCULAR_MODIFY          = 0x0e; // 01110
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_SUBSTRACT_AND_CIRCULAR_MODIFY    = 0x0f; // 01111

	// Indirect addressing with index register IR1
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_ADD                               = 0x10; // 10000
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_SUBSTRACT                         = 0x11; // 10001
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_ADD_AND_MODIFY                    = 0x12; // 10010
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_PREINDEX_IR1_SUBSTRACT_AND_MODIFY              = 0x13; // 10011
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_ADD_AND_MODIFY                   = 0x14; // 10100
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_SUBSTRACT_AND_MODIFY             = 0x15; // 10101
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_ADD_AND_CIRCULAR_MODIFY          = 0x16; // 10110
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR1_SUBSTRACT_AND_CIRCULAR_MODIFY    = 0x17; // 10111

	// Indirect addressing (special cases)
	static const unsigned int MOD_INDIRECT_ADDRESSING                                                     = 0x18; // 11000
	static const unsigned int MOD_INDIRECT_ADDRESSING_WITH_POSTINDEX_IR0_ADD_AND_BIT_REVERSED_MODIFY      = 0x19; // 11001

	/** Compute the effective address for indirect addressing mode
	    @param ea The effective address (output)
	    @param mod The 5-bit 'mod' instruction bit field
	    @param mod The 3-bit 'ar' instruction bit field
	    @param mod The 8-bit 'disp' instruction bit field (sign extended) or an implied 1 displacement
		@return whether the encoding of mod is valid
	*/
	bool ComputeIndirEA(address_t& ea, unsigned int mod, unsigned int ar, uint32_t disp = 1);

    //===============================================================
	//= Execution methods                                     START =
	//===============================================================

	/** Execute one complete instruction
	 */
	void StepInstruction();
	virtual void Stop(int ret);
	
    //===============================================================
	//= Execution methods                                      STOP =
	//===============================================================

private:
	/** The registers interface for debugging purpose */
	std::map<std::string, Register *> registers_registry;
	/** indicates if the memory accesses require to be reported */
	bool requires_memory_access_reporting;
	/** indicates if the finished instructions require to be reported */
	bool requires_finished_instruction_reporting;
  
	//===============================================================
	//= Instruction set decoder variables                     START =
	//===============================================================

	typename isa::tms320::Decoder<CONFIG, DEBUG> decoder;

    //===============================================================
	//= Verbose variables, parameters, and methods            START =
	//===============================================================

	//===============================================================
	//= Registers                                             START =
	//===============================================================

	// Register machine values (hex)
	static const unsigned int REG_R0 = 0x00;
	static const unsigned int REG_R1 = 0x01;
	static const unsigned int REG_R2 = 0x02;
	static const unsigned int REG_R3 = 0x03;
	static const unsigned int REG_R4 = 0x04;
	static const unsigned int REG_R5 = 0x05;
	static const unsigned int REG_R6 = 0x06;
	static const unsigned int REG_R7 = 0x07;
	static const unsigned int REG_AR0 = 0x08;
	static const unsigned int REG_AR1 = 0x09;
	static const unsigned int REG_AR2 = 0x0a;
	static const unsigned int REG_AR3 = 0x0b;
	static const unsigned int REG_AR4 = 0x0c;
	static const unsigned int REG_AR5 = 0x0d;
	static const unsigned int REG_AR6 = 0x0e;
	static const unsigned int REG_AR7 = 0x0f;
	static const unsigned int REG_DP = 0x10;
	static const unsigned int REG_IR0 = 0x11;
	static const unsigned int REG_IR1 = 0x12;
	static const unsigned int REG_BK = 0x13;
	static const unsigned int REG_SP = 0x14;
	static const unsigned int REG_ST = 0x15;
	static const unsigned int REG_IE = 0x16;
	static const unsigned int REG_IF = 0x17;
	static const unsigned int REG_IOF = 0x18;
	static const unsigned int REG_RS = 0x19;
	static const unsigned int REG_RE = 0x1a;
	static const unsigned int REG_RC = 0x1b;

	static const uint32_t VALID_REGS_MASK = 
		(1 << REG_R0)  | (1 << REG_R1)  | (1 << REG_R2)  | (1 << REG_R3)  |
		(1 << REG_R4)  | (1 << REG_R5)  | (1 << REG_R6)  | (1 << REG_R7)  |
		(1 << REG_AR0) | (1 << REG_AR1) | (1 << REG_AR2) | (1 << REG_AR3) |
		(1 << REG_AR4) | (1 << REG_AR5) | (1 << REG_AR6) | (1 << REG_AR7) |
		(1 << REG_DP)  | (1 << REG_IR0) | (1 << REG_IR1) | (1 << REG_BK)  |
		(1 << REG_SP)  | (1 << REG_ST)  | (1 << REG_IE)  | (1 << REG_IF)  |
		(1 << REG_IOF) | (1 << REG_RS)  | (1 << REG_RE)  | (1 << REG_RC);

	// Registers
	address_t reg_pc;    // Program counter
	address_t reg_npc;   // Next program counter (invisible for the programmer)
	reg_t regs[32];

public:
	inline uint32_t GetAR(unsigned int reg_num) const
	{
		return regs[REG_AR0 + reg_num].lo;
	}

	inline void GetExtReg(unsigned int reg_num, reg_t& value) const
	{
		value = regs[REG_R0 + reg_num];
	}

	inline bool GetIntReg(unsigned int reg_num, uint32_t& value) const
	{
		value = regs[reg_num].lo;
		return (VALID_REGS_MASK >> reg_num);
	}

	inline bool SetIntReg(unsigned int reg_num, uint32_t value)
	{
		regs[reg_num].lo = value;
		return (VALID_REGS_MASK >> reg_num);
	}

	inline void SetAR(unsigned int reg_num, uint32_t value)
	{
		regs[REG_AR0 + reg_num].lo = value;
	}

	inline void SetExtReg(unsigned int reg_num, const reg_t& value)
	{
		regs[REG_R0 + reg_num] = value;
	}

	inline address_t GetPC() const { return reg_pc; }
	inline void SetPC(address_t value) { reg_pc = value; }

	inline address_t GetNPC(address_t value) { return reg_npc; }
	inline void SetNPC(address_t value) { reg_npc = value; }

protected:
	bool verbose_all;
	bool verbose_setup;
	
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	
	inline INLINE bool VerboseAll();
	inline INLINE bool VerboseSetup();
}; // end of calss CPU<CONFIG>

} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#undef INLINE

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CPU_HH__

