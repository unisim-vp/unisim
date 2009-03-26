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
#include <stdexcept>

#include <string>
#include <map>
#include <inttypes.h>

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

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

// Status register bitfields offsets
static const unsigned int ST_C = 0;
static const unsigned int ST_V = 1;
static const unsigned int ST_Z = 2;
static const unsigned int ST_N = 3;
static const unsigned int ST_UF = 4;
static const unsigned int ST_LV = 5;
static const unsigned int ST_LUF = 6;
static const unsigned int ST_OVM = 7;
static const unsigned int ST_RM = 8;
static const unsigned int ST_CF = 10;
static const unsigned int ST_CE = 11;
static const unsigned int ST_CC = 12;
static const unsigned int ST_GIE = 13;

// Status register bitfields masks
static const unsigned int M_ST_C = 1 << ST_C;
static const unsigned int M_ST_V = 1 << ST_V;
static const unsigned int M_ST_Z = 1 << ST_Z;
static const unsigned int M_ST_N = 1 << ST_N;
static const unsigned int M_ST_UF = 1 << ST_UF;
static const unsigned int M_ST_LV = 1 << ST_LV;
static const unsigned int M_ST_LUF = 1 << ST_LUF;
static const unsigned int M_ST_OVM = 1 << ST_OVM;
static const unsigned int M_ST_RM = 1 << ST_RM;
static const unsigned int M_ST_CF = 1 << ST_CF;
static const unsigned int M_ST_CE = 1 << ST_CE;
static const unsigned int M_ST_CC = 1 << ST_CC;
static const unsigned int M_ST_GIE = 1 << ST_GIE;

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

	// maximal circular buffer length is 64K
	static const uint32_t MAX_BLOCK_SIZE = 64 * 1024;

	/** Compute the next address in a circular buffer after an address increment
	    @param ar Current address in the circular buffer
	    @param bk Circular buffer length
	    @param step Address increment step
		@return the next address in the circular buffer
	*/
	uint32_t CircularAdd(uint32_t ar, uint32_t bk, uint32_t step);

	/** Compute the next address in a circular buffer after an address decrement
	    @param ar Current address in the circular buffer
	    @param bk Circular buffer length
	    @param step Address decrement step
		@return the next address in the circular buffer
	*/
	uint32_t CircularSubstract(uint32_t ar, uint32_t bk, uint32_t step);

	/** Compute the effective address for indirect addressing mode
	    @param ea The effective address (output)
	    @param update_ar Whether the auxiliary register must be updated with 'next_ar' (output)
	    @param output_ar The effective address to write into the auxiliary register (output)
	    @param mod The 5-bit 'mod' instruction bit field
	    @param ar_num The 3-bit 'ar' instruction bit field (AR register number)
	    @param mod The 8-bit 'disp' instruction bit field (sign extended) or an implied 1 displacement
		@return whether the encoding of mod is valid
	*/
	bool ComputeIndirEA(address_t& ea, bool& update_ar, address_t& ouput_ar, unsigned int mod, unsigned int ar_num, uint32_t disp = 1);

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

    //===============================================================
	//= Interface with the outside world                      START =
	//===============================================================

	virtual bool PrWrite(address_t addr, const void *buffer, uint32_t size) = 0;
	virtual bool PrRead(address_t addr, void *buffer, uint32_t size) = 0;

    //===============================================================
	//= Interface with the outside world                        END =
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

	// Condition encoding                          cond4-0
	static const unsigned int COND_U    = 0x00;  // 00000
	static const unsigned int COND_LO   = 0x01;  // 00001
	static const unsigned int COND_LS   = 0x02;  // 00010
	static const unsigned int COND_HI   = 0x03;  // 00011
	static const unsigned int COND_HS   = 0x04;  // 00100
	static const unsigned int COND_EQ   = 0x05;  // 00101
	static const unsigned int COND_NE   = 0x06;  // 00110
	static const unsigned int COND_LT   = 0x07;  // 00111
	static const unsigned int COND_LE   = 0x08;  // 01000
	static const unsigned int COND_GT   = 0x09;  // 01001
	static const unsigned int COND_GE   = 0x0a;  // 01010
	static const unsigned int COND_NV   = 0x0c;  // 01100
	static const unsigned int COND_V    = 0x0d;  // 01101
	static const unsigned int COND_NUF  = 0x0e;  // 01110
	static const unsigned int COND_UF   = 0x0f;  // 01111
	static const unsigned int COND_NLV  = 0x10;  // 10000
	static const unsigned int COND_LV   = 0x11;  // 10001
	static const unsigned int COND_NLUF = 0x12;  // 10010
	static const unsigned int COND_LUF  = 0x13;  // 10011
	static const unsigned int COND_ZUF  = 0x14;  // 10100


	// Registers
	address_t reg_pc;    // Program counter
	address_t reg_npc;   // Next program counter (invisible for the programmer)
	reg_t regs[32];

public:
	/** Get ARn[23-0]
	    @param reg_num the AR register number
	    @return bits 23-0 of ARn
	*/
	inline uint32_t GetAR(unsigned int reg_num) const
	{
		return regs[REG_AR0 + reg_num].lo & 0xffffff;
	}

	/** Get SP[23-0]
	    @return bits 23-0 of SP
	*/
	inline uint32_t GetSP() const
	{
		return regs[REG_SP].lo & 0xffffff;
	}

	/** Get ST
	    @return 32-bit value of ST
	*/
	inline uint32_t GetST() const
	{
		return regs[REG_ST].lo;
	}

	/** Set ST
	    @param the 32-bit value to write into ST
	*/
	inline void SetST(uint32_t value)
	{
		regs[REG_ST].lo = value;
	}

	/** Check wether a register exists
	    @param reg_num the register number
	    @return true if the register exists (i.e. 0 <= reg_num <= 27), false otherwise
	*/
	inline static bool HasReg(unsigned int reg_num)
	{
		return (VALID_REGS_MASK >> reg_num) & 1;
	}

	/** Get 40-bit extended precision register value.
	    @param reg_num the register number (0 <= reg_num <= 7)
	    @return the 40-bit value stored into the extended precision register
	*/
	inline void GetExtReg(unsigned int reg_num, reg_t& value) const
	{
		value = regs[REG_R0 + reg_num];
	}

	/** Get 32-bit integer register value.
	    @param reg_num the register number (0 <= reg_num <= 27)
	    @return the 32-bit value stored into the register
	*/
	inline uint32_t GetIntReg(unsigned int reg_num) const
	{
		return regs[reg_num].lo;
	}

	/** Set 32-bit integer register value.
	    @param reg_num the register number (0 <= reg_num <= 27)
	    @param value the 32-bit value to write into the register
	*/
	inline void SetIntReg(unsigned int reg_num, uint32_t value)
	{
		regs[reg_num].lo = value;
	}

	/** Set ARn[23-0] to a new value
	    @param reg_num the AR register number
	    @param value the value to write (bits 31-24 are ignored)
	*/
	inline void SetAR(unsigned int reg_num, uint32_t value)
	{
		regs[REG_AR0 + reg_num].lo = (regs[REG_AR0 + reg_num].lo & 0xff000000) | (value & 0xffffff);
	}

	/** Set SP[23-0] to a new value
	    @param value the value to write (bits 31-24 are ignored)
	*/
	inline void SetSP(uint32_t value)
	{
		regs[REG_SP].lo = (regs[REG_SP].lo & 0xff000000) | (value & 0xffffff);
	}

	/** Set 40-bit extended precision register value.
	    @param reg_num the register number (0 <= reg_num <= 7)
	    @param value the 40-bit value to write into the register
	*/
	inline void SetExtReg(unsigned int reg_num, const reg_t& value)
	{
		regs[REG_R0 + reg_num] = value;
	}

	/** Get the address of the current instruction
	    @param addr the current instruction word address
	*/
	inline address_t GetPC() const { return reg_pc; }

	/** Set the address of the current instruction
	    @param addr the current instruction word address
	*/
	inline void SetPC(address_t addr) { reg_pc = addr; }

	/** Get the address of the next instruction
	    @return the instruction word address to branch
	*/
	inline address_t GetNPC() const { return reg_npc; }

	/** Set the address of the next instruction (used by control instructions)
	    @param addr the instruction word address to branch
	*/
	inline void SetNPC(address_t addr) { reg_npc = addr; }

	/** Get BK value
	    @return the value of register BK
	*/
	inline uint32_t GetBK() const { return regs[REG_BK].lo; }

	/** Get IR0[23-0]
	    @return bits 23-0 of IR0
	*/
	inline uint32_t GetIR0() const { return regs[REG_IR0].lo & 0xffffff; }

	/** Get IR1[23-0]
	    @return bits 23-0 of IR1
	*/
	inline uint32_t GetIR1() const { return regs[REG_IR1].lo & 0xffffff; }

	/** Store a 32-bit integer into memory
	    @param ea the effective word address
		@param value the 32-bit integer value to store into memory
	*/
	inline void IntStore(address_t ea, uint32_t value);

	/** Load a 32-bit integer from memory
	    @param ea the effective word address
		@return the 32-bit integer value read from memory
	*/
	inline uint32_t IntLoad(address_t ea);

	/** Load a 32-bit instruction word from memory
	    @param ea the effective word address
		@return the 32-bit instruction word read from memory
	*/
	inline uint32_t Fetch(address_t pc);

	/** Check a condition
	    @param cond 5-bit encoding of condition
		@return whether the condition is met
	*/
	inline bool CheckCondition(unsigned int cond) const;

	inline uint8_t GetST_C() const { return (GetST() >> ST_C) & 1; }
	inline uint8_t GetST_V() const { return (GetST() >> ST_V) & 1; }
	inline uint8_t GetST_N() const { return (GetST() >> ST_N) & 1; }
	inline uint8_t GetST_UF() const { return (GetST() >> ST_UF) & 1; }
	inline uint8_t GetST_LV() const { return (GetST() >> ST_LV) & 1; }
	inline uint8_t GetST_LUF() const { return (GetST() >> ST_LUF) & 1; }
	inline uint8_t GetST_OVM() const { return (GetST() >> ST_OVM) & 1; }
	inline uint8_t GetST_RM() const { return (GetST() >> ST_RM) & 1; }
	inline uint8_t GetST_CF() const { return (GetST() >> ST_CF) & 1; }
	inline uint8_t GetST_CE() const { return (GetST() >> ST_CE) & 1; }
	inline bool GetST_CC() const { return (GetST() >> ST_CC) & 1; }
	inline bool GetST_GIE() const { return (GetST() >> ST_GIE) & 1; }

	inline void GenFlags(uint32_t result, uint32_t reset_mask, uint32_t or_mask, uint32_t carry_out = 0, uint32_t overflow = 0);

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

