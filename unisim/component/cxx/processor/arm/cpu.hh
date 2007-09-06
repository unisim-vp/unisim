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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/cpu_linux_os.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/symbol_table_build.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/util/debug/register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/component/cxx/cache/cache_interface.hh"
#include "unisim/util/endian/endian.hh"
#include <string>
#include <sstream>
#include <inttypes.h>
//#include "unisim/component/cxx/processor/arm/config.hh"
//#include "memories/endian_interface.hh"
//#include "generic/cache/cache_interface.hh"
	
#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
	
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::LinuxOS;
using unisim::service::interfaces::CPULinuxOS;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Logger;
using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;
using unisim::util::debug::Register;
using unisim::util::arithmetic::Add32;
using unisim::component::cxx::cache::CacheInterface;
using unisim::util::endian::endian_type;
using std::string;

template<class CONFIG>
class CPU :
	public Decoder<CONFIG>,
	public Client<Loader<typename CONFIG::address_t> >,
    public Client<LinuxOS>,
    public Service<CPULinuxOS>,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Service<Disassembly<typename CONFIG::address_t> >,
    public Service<Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::address_t> >,
	public Client<Logger> {
private:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::reg_t reg_t;
	typedef typename CONFIG::sreg_t sreg_t;
	typedef typename CONFIG::insn_t insn_t;
	
public:
	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<address_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<Memory<address_t> > memory_export;
	ServiceExport<CPULinuxOS> cpu_linux_os_export;

	ServiceImport<DebugControl<address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<address_t> > memory_import;
	ServiceImport<LinuxOS> linux_os_import;
	ServiceImport<Logger> logger_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, CacheInterface<address_t> *memory_interface, Object *parent = 0);
	virtual ~CPU();

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool Setup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================
	
	void OnBusCycle();
	void Step();
	void Run();
	virtual void Stop(int ret);
	virtual void Sync();

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================
	virtual void Reset();
	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             CPURegistersInterface interface methods                              =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
    virtual Register *GetRegister(const char *name);

	//=====================================================================
	//=                   DebugDisasmInterface methods                    =
	//=====================================================================

    /**
	 * Returns a string with the disassembling of the instruction found 
	 *   at address addr.
	 * 
	 * @param addr The address of the instruction to disassemble.
	 * @param next_addr The address following the requested instruction.
	 * @return The disassembling of the requested instruction address.
	 */
    virtual string Disasm(address_t addr, address_t &next_addr);

	//=====================================================================
	//=                   Debugging methods                               =
	//=====================================================================

    inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	string GetObjectFriendlyName(address_t addr);
	string GetFunctionFriendlyName(address_t addr);
  
    //=====================================================================
	//=             instruction fetch (unused, but mandatory)             =
	//=====================================================================
	
	virtual void Fetch(void *buffer, address_t addr, uint32_t size); // called by inherited class Decoder

    /**************************************************************/
    /* Operand decoding methods     START                         */
    /**************************************************************/
    /* Data processing operand decoding */
    inline reg_t ShiftOperand32imm(const uint32_t rotate_imm, 
			    const uint32_t imm, 
			    bool *shift_carry_out = NULL) GCC_INLINE;
    inline reg_t ShiftOperandImmShift(const uint32_t shift_imm, 
			       const uint32_t shift, 
			       const reg_t val_reg, 
			       bool *shift_carry_out = NULL) GCC_INLINE;
    inline reg_t ShiftOperandRegShift(const uint32_t shift_reg, 
			       const uint32_t shift, 
			       const reg_t val_reg,
			       bool *shift_carry_out = NULL) GCC_INLINE;

    /* Load/store operand decoding */
    inline address_t LSWUBImmOffset(const uint32_t u, 
			const reg_t val_reg, 
			const uint32_t offset) GCC_INLINE;
    inline address_t LSWUBReg(const uint32_t u, 
		       const reg_t val_rn, 
		       const reg_t val_rd, 
		       const uint32_t shift_imm, 
		       const uint32_t shift, 
		       const reg_t val_rm) GCC_INLINE;

    /* Miscellaneous load/store operand decoding */
    inline address_t MLSImmOffset(const uint32_t u, 
			   const reg_t val_reg, 
			   const uint32_t immedH,
			   const uint32_t immedL) GCC_INLINE;
    inline address_t MLSReg(const uint32_t u, 
		     const reg_t val_rn, 
		     const reg_t val_rd, 
		     const reg_t val_rm) GCC_INLINE;

    /* Load/sotre multiple operand decoding */
    inline uint32_t LSMia(const reg_t val_reg,
		   const uint32_t reg_list,
		   address_t *start_address,
		   address_t *end_address,
		   reg_t *new_val_reg) GCC_INLINE;
    inline uint32_t LSMib(const reg_t val_reg,
		   const uint32_t reg_list,
		   address_t *start_address,
		   address_t *end_address,
		   reg_t *new_val_reg) GCC_INLINE;
    inline uint32_t LSMda(const reg_t val_reg,
		   const uint32_t reg_list,
		   address_t *start_address,
		   address_t *end_address,
		   reg_t *new_val_reg) GCC_INLINE;
    inline uint32_t LSMdb(const reg_t val_reg,
		   const uint32_t reg_list,
		   address_t *start_address,
		   address_t *end_address,
		   reg_t *new_val_reg) GCC_INLINE;

    /* Coprocessor load/store operand decoding */
    inline address_t CLSImm(const uint32_t u,
		     const uint32_t val_reg,
		     const uint32_t offset) GCC_INLINE;

    /**************************************************************/
    /* Operand decoding methods     END                           */
    /**************************************************************/

    /**************************************************************/
    /* Disassembling methods     START                            */
    /**************************************************************/
    /* Condition opcode bytes disassembling method */
    void DisasmCondition(const uint32_t cond, stringstream &buffer);
    void DisasmConditionFieldsMask(const uint32_t mask,
				   stringstream &buffer);
    /* Data processing operand disassembling methods */
    void DisasmShiftOperand32Imm(const uint32_t rotate_imm, 
				 const uint32_t imm, 
				 stringstream &buffer);
    void DisasmShiftOperandImmShift(const uint32_t shift_imm, 
				    const uint32_t shift, 
				    const uint32_t rm, 
				    stringstream &buffer);
    void DisasmShiftOperandRegShift(const uint32_t rs, 
				    const uint32_t shift, 
				    const uint32_t rm, 
				    stringstream &buffer);
    /* Load/store operand disassembling methods */
    void DisasmLSWUBImmOffset_post(const uint32_t u, 
				   const uint32_t rn, 
				   const uint32_t offset,
				   stringstream &buffer);
    void DisasmLSWUBImmOffset_offset(const uint32_t u, 
				     const uint32_t rn, 
				     const uint32_t offset,
				     stringstream &buffer);
    void DisasmLSWUBImmOffset_pre(const uint32_t u, 
				  const uint32_t rn, 
				  const uint32_t offset,
				  stringstream &buffer);
    void DisasmLSWUBReg_post(const uint32_t u, 
			     const uint32_t rn, 
			     const uint32_t shift_imm, 
			     const uint32_t shift, 
			     const uint32_t rm,
			     stringstream &buffer);
    void DisasmLSWUBReg_offset(const uint32_t u, 
			       const uint32_t rn, 
			       const uint32_t shift_imm, 
			       const uint32_t shift, 
			       const uint32_t rm,
			       stringstream &buffer);
    void DisasmLSWUBReg_pre(const uint32_t u, 
			    const uint32_t rn, 
			    const uint32_t shift_imm, 
			    const uint32_t shift, 
			    const uint32_t rm,
			    stringstream &buffer);
    /* Miscellaneous load/store operand disassembling methods */
    void DisasmMLSImmOffset_post(const uint32_t u, 
				 const uint32_t rn,
				 const uint32_t immedH,
				 const uint32_t immedL,
				 stringstream &buffer);
    void DisasmMLSImmOffset_offset(const uint32_t u, 
				   const uint32_t rn, 
				   const uint32_t immedH,
				   const uint32_t immedL,
				     stringstream &buffer);
    void DisasmMLSImmOffset_pre(const uint32_t u, 
				const uint32_t rn, 
				const uint32_t immedH,
				const uint32_t immedL,
				stringstream &buffer);
    void DisasmMLSReg_post(const uint32_t u, 
			   const uint32_t rn, 
			   const uint32_t rm,
			   stringstream &buffer);
    void DisasmMLSReg_offset(const uint32_t u, 
			     const uint32_t rn, 
			     const uint32_t rm,
			     stringstream &buffer);
    void DisasmMLSReg_pre(const uint32_t u, 
			  const uint32_t rn, 
			  const uint32_t rm,
			  stringstream &buffer);
    /* Coprocessor load/store operand disassembling methods */
    void DisasmCLSImm_post(const uint32_t u,
			   const uint32_t rn,
			   const uint32_t offset,
			   stringstream &buffer);
    void DisasmCLSImm_offset(const uint32_t u,
			     const uint32_t rn,
			     const uint32_t offset,
			     stringstream &buffer);
    void DisasmCLSImm_pre(const uint32_t u,
			  const uint32_t rn,
			  const uint32_t offset,
			  stringstream &buffer);
    void DisasmCLSUnindexed(const uint32_t rn,
			    const uint32_t option,
			    stringstream &buffer);
			   
    /**************************************************************/
    /* Disassembling methods     END                              */
    /**************************************************************/

    // Linux OS Interface
	virtual void PerformExit(int ret);

	// Endian interface
    virtual endian_type GetEndianess();

    /**************************************************************/
    /* Registers access methods    START                          */
    /**************************************************************/

    /* definition of the type of the registers */
    const static unsigned int nextPC_reg = 16;
    const static unsigned int PC_reg = 15;
    const static unsigned int LR_reg = 14;
    
    /* GPR access functions */
    void InitGPR();
    void SetGPRMapping(uint32_t src_mode, uint32_t tar_mode);
    inline reg_t GetGPR(uint32_t id) const GCC_INLINE;
    inline void SetGPR(uint32_t id, reg_t val) GCC_INLINE;
    reg_t GetGPR_usr(uint32_t id) const;
    void SetGPR_usr(uint32_t id, reg_t val);
    const string DumpRegs() const;

    /* CPSR access functions */
    inline uint32_t GetCPSR() const GCC_INLINE;
    inline void SetCPSR(uint32_t val) GCC_INLINE;
    inline void SetCPSR_NZCV(bool n,
		      bool z,
		      bool c,
		      bool v) GCC_INLINE;
    inline void SetCPSR_N(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_N() GCC_INLINE;
    inline bool GetCPSR_N() GCC_INLINE;
    inline void SetCPSR_Z(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_Z() GCC_INLINE;
    inline bool GetCPSR_Z() GCC_INLINE;
    inline void SetCPSR_C(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_C() GCC_INLINE;
    inline bool GetCPSR_C() GCC_INLINE;
    inline void SetCPSR_V(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_V() GCC_INLINE;
    inline bool GetCPSR_V() GCC_INLINE;
    inline void SetCPSR_Q(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_Q() GCC_INLINE;
    inline bool GetCPSR_Q() GCC_INLINE;
    inline void SetCPSR_I(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_I() GCC_INLINE;
    inline bool GetCPSR_I() GCC_INLINE;
    inline void SetCPSR_F(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_F() GCC_INLINE;
    inline bool GetCPSR_F() GCC_INLINE;
    inline void SetCPSR_T(const bool val = true) GCC_INLINE;
    inline void UnsetCPSR_T() GCC_INLINE;
    inline bool GetCPSR_T() GCC_INLINE;
    inline void SetCPSR_Mode(uint32_t mode) GCC_INLINE;
    inline uint32_t GetCPSR_Mode() GCC_INLINE;
    
    /* SPSR access functions */
    inline void SetSPSR(uint32_t val) GCC_INLINE;
    inline void SetSPSR_NZCV(bool n,
		      bool z,
		      bool c,
		      bool v) GCC_INLINE;
    inline void SetSPSR_N(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_N() GCC_INLINE;
    inline bool GetSPSR_N() GCC_INLINE;
    inline void SetSPSR_Z(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_Z() GCC_INLINE;
    inline bool GetSPSR_Z() GCC_INLINE;
    inline void SetSPSR_C(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_C() GCC_INLINE;
    inline bool GetSPSR_C() GCC_INLINE;
    inline void SetSPSR_V(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_V() GCC_INLINE;
    inline bool GetSPSR_V() GCC_INLINE;
    inline void SetSPSR_Q(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_Q() GCC_INLINE;
    inline bool GetSPSR_Q() GCC_INLINE;
    inline void SetSPSR_I(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_I() GCC_INLINE;
    inline bool GetSPSR_I() GCC_INLINE;
    inline void SetSPSR_F(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_F() GCC_INLINE;
    inline bool GetSPSR_F() GCC_INLINE;
    inline void SetSPSR_T(const bool val = true) GCC_INLINE;
    inline void UnsetSPSR_T() GCC_INLINE;
    inline bool GetSPSR_T() GCC_INLINE;
    inline void SetSPSR_Mode(uint32_t mode) GCC_INLINE;
    inline uint32_t GetSPSR() GCC_INLINE;

    /* gets spsr index from current running mode */
    inline uint32_t GetSPSRIndex() GCC_INLINE;

    /* other CPSR/SPSR methods */
    inline void MoveSPSRtoCPSR() GCC_INLINE;
    
    /* Instruction condition checking method */
    inline bool CheckCondition(uint32_t cond) GCC_INLINE; // check condition type
    /* Condition codes update method */
    inline void UpdateConditionCodes() GCC_INLINE;
    /* Result condition computation methods */
    inline bool CarryFrom(const reg_t res, const reg_t s1, const reg_t s2,
		   const reg_t carry_in = 0) GCC_INLINE;
    inline bool BorrowFrom(const reg_t res, const reg_t s1, const reg_t s2,
		    const reg_t carry_in = 0) GCC_INLINE;
    inline bool AdditionOverflowFrom(const reg_t res, const reg_t s1, const reg_t s2,
			 const reg_t carry_in = 0) GCC_INLINE;
    inline bool SubtractionOverflowFrom(const reg_t res, const reg_t s1, const reg_t s2,
			 const reg_t carry_in = 0) GCC_INLINE;

    /**************************************************************/
    /* Registers access methods    END                            */
    /**************************************************************/

    /**************************************************************/
    /* Memory access methods       START                          */
    /**************************************************************/
    
    inline bool Read8(address_t address, uint8_t &val) GCC_INLINE;
    inline bool Read16(address_t address, uint16_t &val) GCC_INLINE;
    inline bool Read32(address_t address, uint32_t &val) GCC_INLINE;
    inline bool Write8(address_t address, uint8_t &val) GCC_INLINE;
    inline bool Write16(address_t address, uint16_t &val) GCC_INLINE;
    inline bool Write32(address_t address, uint32_t &val) GCC_INLINE;
    
    /**************************************************************/
    /* Memory access methods       END                            */
    /**************************************************************/

    /**************************************************************/
    /* Coprocessor methods          START                         */
    /**************************************************************/
    
    void MCRImplementation(uint32_t op1, uint32_t crn, uint32_t rd, uint32_t cp_num, uint32_t op2, uint32_t crm);
    void MRCImplementation(uint32_t op1, uint32_t crn, uint32_t rd, uint32_t cp_num, uint32_t op2, uint32_t crm);
    
    /**************************************************************/
    /* Coprocessor methods          END                           */
    /**************************************************************/

    /* masks for the different running modes */
    static const uint32_t RUNNING_MODE_MASK = 0x1F;
    static const uint32_t USER_MODE = 0x10;
    static const uint32_t FIQ_MODE = 0x11;
    static const uint32_t IRQ_MODE = 0x12;
    static const uint32_t SUPERVISOR_MODE = 0x13;
    static const uint32_t ABORT_MODE = 0x17;
    static const uint32_t UNDEFINED_MODE = 0x1B;
    static const uint32_t SYSTEM_MODE = 0x1F;
    /* masks for the different CPSR status bits */
    static const uint32_t CPSR_N_MASK = 0x80000000;
    static const uint32_t CPSR_Z_MASK = 0x40000000;
    static const uint32_t CPSR_C_MASK = 0x20000000;
    static const uint32_t CPSR_V_MASK = 0x10000000;
    static const uint32_t CPSR_Q_MASK = 0x01000000;
    static const uint32_t CPSR_I_MASK = 0x00000080;
    static const uint32_t CPSR_F_MASK = 0x00000040;
    static const uint32_t CPSR_T_MASK = 0x00000020;
    static const uint32_t CPSR_RUNNING_MODE_MASK = 0x0000001F;
    /* masks for the different SPSR status bits */
    static const uint32_t SPSR_N_MASK = CPSR_N_MASK;
    static const uint32_t SPSR_Z_MASK = CPSR_Z_MASK;
    static const uint32_t SPSR_C_MASK = CPSR_C_MASK;
    static const uint32_t SPSR_V_MASK = CPSR_V_MASK;
    static const uint32_t SPSR_Q_MASK = CPSR_Q_MASK;
    static const uint32_t SPSR_I_MASK = CPSR_I_MASK;
    static const uint32_t SPSR_F_MASK = CPSR_F_MASK;
    static const uint32_t SPSR_T_MASK = CPSR_T_MASK;
    static const uint32_t SPSR_RUNNING_MODE_MASK = CPSR_RUNNING_MODE_MASK;
    /* masks for the different condition codes */
    static const uint32_t COND_EQ = 0x00;
    static const uint32_t COND_NE = 0x01;
    static const uint32_t COND_CS_HS = 0x02;
    static const uint32_t COND_CC_LO = 0x03;
    static const uint32_t COND_MI = 0x04;
    static const uint32_t COND_PL = 0x05;
    static const uint32_t COND_VS = 0x06;
    static const uint32_t COND_VC = 0x07;
    static const uint32_t COND_HI = 0x08;
    static const uint32_t COND_LS = 0x09;
    static const uint32_t COND_GE = 0x0a;
    static const uint32_t COND_LT = 0x0b;
    static const uint32_t COND_GT = 0x0c;
    static const uint32_t COND_LE = 0x0d;
    static const uint32_t COND_AL = 0x0e;

    /* Returns current instruction size 
     * NOTE: for the moment thumb is not supported, so only an instruction
     *   size is supported (32bits).
     */
    uint32_t InstructionByteSize();
    uint32_t InstructionBitSize();

    /* Check that the address is aligned
     */
    void CheckAlignmentExcep(address_t addr); // TODO

private:
	/** 
	 * the copy of the last cache line accessed by the cpu and its address, 
	 *   this speed ups execution, because the memory is not accessed */
	insn_t insn_cache_line[8];
	address_t insn_cache_line_address;
	/** the interface to the memory system */
	CacheInterface<address_t> *memory_interface;
	
	/** the instruction counter */
	uint64_t instruction_counter;
	
	/** The registers interface for debugging purpose */
	map<string, Register *> registers_registry;       

    /* gpr organization per running mode:
     * - user:           0-14 (R0-R14),                  15 (PC)
     * - system:         0-14 (R0-R14),                  15 (PC)
     * - supervisor:     0-12 (R0-R12), 16-17 (R13-R14), 15 (PC)
     * - abort:          0-12 (R0-R12), 18-19 (R13-R14), 15 (PC)
     * - undefined:      0-12 (R0-R12), 20-21 (R13-R14), 15 (PC)
     * - interrupt:      0-12 (R0-R12), 22-23 (R13-R14), 15 (PC)
     * - fast interrupt: 0-7 (R0-R7),   24-30 (R8-R14),  15 (PC)
    */
    /* The arm has only 31 registers, but we are using an 
     *   additional one to store the NextPC, which does not really
     *   exist */
    const static uint32_t num_phys_gprs = 32;
    const static uint32_t num_log_gprs = 16;
    reg_t phys_gpr[num_phys_gprs]; /* the physical registers */
    reg_t gpr[num_log_gprs];
    /* all the running modes share the same CPSR register
     * CPSR organization:
     * - bit 31: N
     * - bit 30: Z
     * - bit 29: C
     * - bit 28: V
     * - bit 27: Q
     * - bits 26-8: DNM(RAZ) not used in current arm versions
     * - bit 7: I
     * - bit 6: F
     * - bit 5: T
     * - bits 4-0: M bits, running mode, see bellow
     * Bits 4 to 0 indicate the processor current running mode.
     *   Accepted running modes are:
     * - 0b10000: User
     * - 0b10001: FIQ (Fast Interrupt)
     * - 0b10010: IRQ (Interrupt) 
     * - 0b10011: Supervisor
     * - 0b10111: Abort
     * - 0b11011: Undefined
     * - 0b11111: System
     */
    uint32_t cpsr;
    /* SPSR organization per running mode:
       - user:           --
       - system:         --
       - supervisor:     0
       - abort:          1
       - undefined:      2
       - interrupt:      3
       - fast interrupt: 4
    */
    uint32_t spsr[5];
    const static uint32_t num_phys_spsrs = 5;
    /* fake floating point register */
    uint64_t fake_fpr[8];
    uint32_t fake_fps;

    /** the condition table used to check condition in the instructions */
    uint16_t check_condition_table[16];
    bool CheckCondition(unsigned int cond, unsigned int cpsr_val);
    void InitializeCheckConditionTable();

    /** variable signaling if the machine is running */
    bool running;

protected:
	// verbose parameters
	bool verbose_all;
	Parameter<bool> param_verbose_all;
	bool verbose_setup;
	Parameter<bool> param_verbose_setup;
	bool verbose_step;
	Parameter<bool> param_verbose_step;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

