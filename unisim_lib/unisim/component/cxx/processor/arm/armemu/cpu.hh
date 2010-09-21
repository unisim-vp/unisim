/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/cpu_linux_os.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/cache_power_estimator.hh"
#include "unisim/service/interfaces/power_mode.hh"
#include "unisim/util/debug/register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"
#include "unisim/component/cxx/processor/arm/isa_arm32.hh"
#include "unisim/component/cxx/processor/arm/isa_thumb.hh"
#include "unisim/component/cxx/processor/arm/instruction.hh"
#include "unisim/util/endian/endian.hh"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <inttypes.h>

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
namespace armemu {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
//using unisim::kernel::service::Parameter;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::LinuxOS;
using unisim::service::interfaces::CPULinuxOS;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::CachePowerEstimator;
using unisim::service::interfaces::PowerMode;
using unisim::util::debug::Register;
using unisim::util::arithmetic::Add32;
using unisim::component::cxx::processor::arm::CacheInterface;
using unisim::component::cxx::processor::arm::CacheInterfaceWithMemoryService;
// using unisim::component::cxx::processor::arm::cache::Cache;
using unisim::component::cxx::processor::arm::Instruction;
using unisim::util::endian::endian_type;
using std::string;
using std::stringstream;
using std::map;
using std::ostream;
using std::vector;
using std::queue;
	
class CPU 
	: public Client<LinuxOS>
	, public Service<CPULinuxOS>
	, public Service<MemoryInjection<uint64_t> >
	, public Client<DebugControl<uint64_t> >
	, public Client<MemmoryAccessReporting<uint64_t> >
	, public Client<TrapReporting>
	, public Service<MemoryAccessReportingControl>
	, public Service<Disassembly<uint64_t> >
	, public Service<Memory<uint64_t> >
	, public Client<CachePowerEstimator>
	, public Client<PowerMode>
{
private:
	typedef uint32_t address_t;
	typedef uint32_t reg_t;
	typedef int32_t sreg_t;
	typedef uint32_t insn_t;
	typedef uint16_t thumb_insn_t;
		
public:
	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
		
	ServiceExport<Disassembly<uint64_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<MemoryInjection<uint64_t> > memory_injection_export;
	ServiceExport<Memory<uint64_t> > memory_export;
	ServiceExport<CPULinuxOS> cpu_linux_os_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;
		
	ServiceImport<DebugControl<uint64_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<uint64_t> > symbol_table_lookup_import;
	ServiceImport<LinuxOS> linux_os_import;
	// TODO: Check if we need the exception_trap_reporting_import
	ServiceImport<TrapReporting> exception_trap_reporting_import;
	// END TODO
	ServiceImport<TrapReporting> instruction_counter_trap_reporting_import;

	ServiceImport<CachePowerEstimator> il1_power_estimator_import;
	ServiceImport<CachePowerEstimator> dl1_power_estimator_import;
	ServiceImport<PowerMode> il1_power_mode_import;
	ServiceImport<PowerMode> dl1_power_mode_import;

	// the kernel logger
	unisim::kernel::logger::Logger logger;

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

	/** Execute one complete instruction
	 * */
	void StepInstruction();
	// TODO: Check if we need OnBusCycle, Run, Stop and Sync
	void OnBusCycle();
	void Run();
	virtual void Stop(int ret);
	virtual void Sync();
	// END TODO
		
	//=====================================================================
	//=             memory injection interface methods                    =
	//=====================================================================

	/** Inject an intrusive read memory operation.
	 *
	 * @param addr the target address to read from
	 * @param buffer the buffer to fill with the read memory
	 * @param size the amount of bytes to read from starting from the target address
	 *
	 * @return true on success, false otherwise
	 **/
	virtual bool InjectReadMemory(uint64_t addr, void *buffer, uint32_t size);
	/** Inject an intrusive write memory operation
	 *
	 * @param addr the target address to write to
	 * @param buffer a buffer containing the data to write
	 * @param size the amount of bytes to write
	 *
	 * @return true on success, false otherwise
	 **/
	virtual bool InjectWriteMemory(uint64_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	/** Set/unset the reporting of memory accesses.
	 *
	 * @param report if true set the reporting of memory acesses, unset otherwise
	 **/
	virtual void RequiresMemoryAccessReporting(bool report);
	/** Set/unset the reporting of finishing instructions.
	 * 
	 * @param report if true set the reporting of finishing instructions, unset otherwise
	 **/
	virtual void RequiresFinishedInstructionReporting(bool report);

	//=====================================================================
	//=             non intrusive memory methods                          =
	//=====================================================================
		
	virtual bool ReadMemory(uint64_t addr, 
			void *buffer, 
			uint32_t size);
	virtual bool WriteMemory(uint64_t addr, 
			const void *buffer, 
			uint32_t size);
	virtual bool ExternalReadMemory(uint64_t addr, 
			void *buffer, 
			uint32_t size) = 0;
	virtual bool ExternalWriteMemory(uint64_t addr, 
			const void *buffer, 
			uint32_t size) = 0;

	//=====================================================================
	//=             CPURegistersInterface interface methods               =
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
	virtual string Disasm(uint64_t addr, uint64_t &next_addr);
		
	//=====================================================================
	//=                      Linux OS Interface                           =
	//=====================================================================
	//
	virtual void PerformExit(int ret);

	//=====================================================================
	//=                       Endian interface                            =
	//=====================================================================

	virtual endian_type GetEndianness();
		
	/**************************************************************/
	/* Registers access methods    START                          */
	/**************************************************************/
		
	/* definition of the type of the registers */
	const static unsigned int nextPC_reg = 16;
	const static unsigned int PC_reg = 15;
	const static unsigned int LR_reg = 14;
	const static unsigned int SP_reg = 13;
		
	/* GPR access functions */
	/** Reset the values of the general purpose registers (GPRs)
	 */
	void InitGPR();
	/** Arrange the GPR mapping depending on initial and target running mode.
	 *
	 * @param src_mode the running mode the processor is currently in
	 * @param tar_mode the target running mode the registers should be mapped to
	 */
	void SetGPRMapping(uint32_t src_mode, uint32_t tar_mode);
	/** Get the value contained by a GPR.
	 *
	 * @param id the register index
	 * @return the value contained by the register
	 */
	inline reg_t GetGPR(uint32_t id) const GCC_INLINE;
	/** Set the value contained by a GPR.
	 *
	 * @param id the register index
	 * @param val the value to set
	 */
	inline void SetGPR(uint32_t id, reg_t val) GCC_INLINE;
	/** Get the value contained by a user GPR.
	 * Returns the value contained by a user GPR. It is the same than GetGPR but
	 *   restricting the index from 0 to 15 (only the first 16 registers).
	 *
	 * @param id the register index
	 * @return the value contained by the register
	 */
	reg_t GetGPR_usr(uint32_t id) const;
	/** Set the value contained by a user GPR.
	 * Sets the value contained by a user GPR. It is the same than SetGPR byt
	 *   restricting the index from 0 to 15 (only the first 16 registers).
	 *
	 * @param id the register index
	 * @param val the value to set
	 */
	void SetGPR_usr(uint32_t id, reg_t val);
	/* TODO: remove DumpRegs method */
	const string DumpRegs() const;
	/* END TODO */
		
	/* CPSR access functions */
	/** Get the value of the CPSR register.
	 *
	 * @return the value of the CPSR register.
	 */
	inline uint32_t GetCPSR() const GCC_INLINE;
	/** Set the value of the CPSR register.
	 *
	 * @param val the value to set
	 */
	inline void SetCPSR(uint32_t val) GCC_INLINE;
	/** Set the value of the NZCV bits of the CPSR register.
	 *
	 * @param n the N bit
	 * @param z the Z bit
	 * @param c the C bit
	 * @param v the V bit
	 */
	inline void SetCPSR_NZCV(bool n,
							 bool z,
							 bool c,
							 bool v) GCC_INLINE;
	/** Set the N bit of the CPSR register.
	 *
	 * @param val the value of the N bit
	 */
	inline void SetCPSR_N(const bool val = true) GCC_INLINE;
	/** Unset the N bit of the CPSR register (set to 0).
	 * This method is analagous to SetCPSR_N(false).
	 */
	inline void UnsetCPSR_N() GCC_INLINE;
	/** Get the value of the CPSR register N bit.
	 *
	 * @return the value of the CPSR register N bit
	 */
	inline bool GetCPSR_N() GCC_INLINE;
	/** Set the Z bit of the CPSR register.
	 *
	 * @param val the value of the Z bit
	 */
	inline void SetCPSR_Z(const bool val = true) GCC_INLINE;
	/** Unset the Z bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_Z(false).
	 */
	inline void UnsetCPSR_Z() GCC_INLINE;
	/** Get the value of the CPSR register Z bit.
	 *
	 * @return the value of the CPSR register Z bit
	 */
	inline bool GetCPSR_Z() GCC_INLINE;
	/** Set the C bit of the CPSR register.
	 *
	 * @param val the value of the C bit
	 */
	inline void SetCPSR_C(const bool val = true) GCC_INLINE;
	/** Unset the C bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_C(false).
	 */
	inline void UnsetCPSR_C() GCC_INLINE;
	/** Get the value of the CPSR register C bit.
	 *
	 * @return the value of the CPSR register C bit
	 */
	inline bool GetCPSR_C() GCC_INLINE;
	/** Set teh V bit of the CPSR register.
	 *
	 * @param val the value of the V bit
	 */
	inline void SetCPSR_V(const bool val = true) GCC_INLINE;
	/** Unset the V bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_V(false).
	 */
	inline void UnsetCPSR_V() GCC_INLINE;
	/** Get the value of the CPSR register V bit.
	 *
	 * @return the value of the CPSR register V bit
	 */
	inline bool GetCPSR_V() GCC_INLINE;
	/** Set the Q bit of the CPSR register.
	 *
	 * @param val teh value of the Q bit
	 */
	inline void SetCPSR_Q(const bool val = true) GCC_INLINE;
	/** Unset the Q bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_Q(false).
	 */
	inline void UnsetCPSR_Q() GCC_INLINE;
	/** Get the value of the CPSR register Q bit.
	 * 
	 * @return the value of the CPSR register Q bit
	 */
	inline bool GetCPSR_Q() GCC_INLINE;
	/** Set the I bit of the CPSR register.
	 *
	 * @param val the value of the I bit
	 */
	inline void SetCPSR_I(const bool val = true) GCC_INLINE;
	/** Unset the I bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_I(false).
	 */
	inline void UnsetCPSR_I() GCC_INLINE;
	/** Get the value of the CPSR register I bit.
	 *
	 * @return the value of the CPSR register I bit
	 */
	inline bool GetCPSR_I() GCC_INLINE;
	/** Set the F bit of the CPSR register.
	 *
	 * @param val the value of the F bit
	 */
	inline void SetCPSR_F(const bool val = true) GCC_INLINE;
	/** Unset the F bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_F(false).
	 */
	inline void UnsetCPSR_F() GCC_INLINE;
	/** Get the value of the CPSR register F bit.
	 *
	 * @return the value of the CPSR register F bit
	 */
	inline bool GetCPSR_F() GCC_INLINE;
	/** Set the T bit of the CPSR register.
	 *
	 * @param val the value of the T bit
	 */
	inline void SetCPSR_T(const bool val = true) GCC_INLINE;
	/** Unset the T bit of the CPSR register (set to 0).
	 * This method is analogous to SetCPSR_T(false).
	 */
	inline void UnsetCPSR_T() GCC_INLINE;
	/** Get the value of the CPSR register T bit.
	 *
	 * @return the value of the CPSR register T bit.
	 */
	inline bool GetCPSR_T() GCC_INLINE;
	/** Set the mode value of the CPSR register.
	 *
	 * @param mode the mode to set in the CPSR register
	 */
	inline void SetCPSR_Mode(uint32_t mode) GCC_INLINE;
	/** Get the mode value of the CPSR register.
	 *
	 * @return the mode value of the CPSR register
	 */
	inline uint32_t GetCPSR_Mode() GCC_INLINE;
		
	/* SPSR access functions */
	inline void SetSPSR(uint32_t val) GCC_INLINE;
	inline uint32_t GetSPSR() GCC_INLINE;
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
	inline uint32_t GetSPSR_Mode() GCC_INLINE;

	/** Get SPSR index from current running mode
	 *
	 * @return the SPSR index from current running mode
	 */
	inline uint32_t GetSPSRIndex() GCC_INLINE;

	/** Copy the value of current SPSR register into CPSR.
	 */
	inline void MoveSPSRtoCPSR() GCC_INLINE;

	/** Check the condition mask given agains current CPSR status.
	 * Returns true if the condition matches CPSR, false otherwise.
	 *
	 * @param cond the condition to check
	 * @return true if the condition matches CPSR, false otherwise
	 */
	inline bool CheckCondition(uint32_t cond) GCC_INLINE; // check condition type
	/* TODO: Condition codes update method */
	inline void UpdateConditionCodes() GCC_INLINE;
	/* END TODO */

	/* TODO: The following methods could be moved into a separate file */
	/** Compute if a carry is produced from an operation.
	 *
	 * @param res the result of the operation
	 * @param s1 the first operand
	 * @param s2 the second operand
	 * @param carry_in the input carry if any
	 * @return true if a carry is produced, false otherwise
	 */
	inline bool CarryFrom(const reg_t res, const reg_t s1, const reg_t s2,
						  const reg_t carry_in = 0) GCC_INLINE;
	/** Compute if a borrow is produced from an operation.
	 *
	 * @param res the result of the operation
	 * @param s1 the first operand
	 * @param s2 the second operand
	 * @param carry_in the input carry if any
	 * @return true if a borrow is produced, false otherwise
	 */
	inline bool BorrowFrom(const reg_t res, const reg_t s1, const reg_t s2,
						   const reg_t carry_in = 0) GCC_INLINE;
	/** Compute if an overflow is produced from an addition operation.
	 *
	 * @param res the result of the operation
	 * @param s1 the first operand
	 * @param s2 the second operand
	 * @param carry_in the input carry if any
	 * @result true if an overflow is produced, false otherwise
	 */
	inline bool AdditionOverflowFrom(const reg_t res, const reg_t s1, const reg_t s2,
									 const reg_t carry_in = 0) GCC_INLINE;
	/** Compute if an overflow is produced from a subtraction operation.
	 *
	 * @param res the result of the operation
	 * @param s1 the first operand
	 * @param s2 the second operand
	 * @param carry_in the input carry if any
	 * @result true if an overflow is produced, false otherwise
	 */
	inline bool SubtractionOverflowFrom(const reg_t res, const reg_t s1, const reg_t s2,
										const reg_t carry_in = 0) GCC_INLINE;
	/* END TODO */

	/**************************************************************/
	/* Registers access methods    END                            */
	/**************************************************************/

	/**************************************************************/
	/* Memory access methods       START                          */
	/**************************************************************/

	/** reads 16bits instructions from the memory system
	 * This method allows the user to read instructions from the memory system,
	 *   that is, it tries to read from the pertinent caches and if failed from
	 *   the external memory system.
	 * 
	 * @param address the address to read data from
	 * @param val the buffer to fill with the read data
	 */
	inline void ReadInsn(address_t address, uint16_t &val) GCC_INLINE;
	/** reads 32bits instructions from the memory system
	 * This method allows the user to read instructions from the memory system,
	 *   that is, it tries to read from the pertinent caches and if failed from
	 *   the external memory system.
	 * 
	 * @param address the address to read data from
	 * @param val the buffer to fill with the read data
	 */
	inline void ReadInsn(address_t address, uint32_t &val) GCC_INLINE;
	/** Memory prefetch instruction.
	 * This method is used to make memory prefetches into the caches (if available),
	 *   that is it sends a memory read that doesn't keep the request result.
	 * 
	 * @param address the address of the prefetch
	 */
	inline void ReadPrefetch(address_t address) GCC_INLINE;
	/** 32bits memory read that stores result into the PC
	 * This methods reads 32bits from memory and stores the result into
	 *   the pc register of the CPU.
	 * 
	 * @param address the base address of the 32bits read
	 */
	inline void Read32toPCUpdateT(address_t address) GCC_INLINE;
	/** 32bits memory read that stores result into the PC and updates thumb state
	 * This methods reads 32bits from memory and stores the result into
	 *   the pc register of the CPU and updates thumb state if necessary
	 * 
	 * @param address the base address of the 32bits read
	 */
	inline void Read32toPC(address_t address) GCC_INLINE;
	/** 32bits memory read that stores result into one of the general purpose registers
	 * This method reads 32bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg
	 * 
	 * @param address the base address of the 32bits read
	 * @param reg the register to store the resulting read
	 */
	inline void Read32toGPR(address_t address, uint32_t reg) GCC_INLINE;
	/** 32bits aligned memory read that stores result into one of the general purpose registers
	 * This method reads 32bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. Note that this
	 *   read methods supposes that the address is 32bits aligned.
	 * 
	 * @param address the base address of the 32bits read
	 * @param reg the register to store the resulting read
	 */
	inline void Read32toGPRAligned(address_t address, uint32_t reg) GCC_INLINE;
	/** 16bits aligned memory read that stores result into one of the general purpose registers
	 * This method reads 16bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. Note that this
	 *   read methods supposes that the address is 16bits aligned.
	 * 
	 * @param address the base address of the 16bits read
	 * @param reg the register to store the resulting read
	 */
	inline void Read16toGPRAligned(address_t address, uint32_t reg) GCC_INLINE;
	/** signed 16bits aligned memory read that stores result into one of the general purpose registers
	 * This method reads 16bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. Note that this
	 *   read methods supposes that the address is 16bits aligned. The 16bits value
	 *   is considered signed and sign extended to the register size 
	 * 
	 * @param address the base address of the 16bits read
	 * @param reg the register to store the resulting read
	 */
	inline void ReadS16toGPRAligned(address_t address, uint32_t reg) GCC_INLINE;
	/** 8bits memory read that stores result into one of the general purpose registers
	 * This method reads 8bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg
	 * 
	 * @param address the base address of the 8bits read
	 * @param reg the register to store the resulting read
	 */
	inline void ReadS8toGPR(address_t address, uint32_t reg) GCC_INLINE;
	/** signed 8bits memory read that stores result into one of the general purpose registers
	 * This method reads 8bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. The 8bits value
	 *   is considered signed and sign extended to the register size
	 * 
	 * @param address the base address of the 8bits read
	 * @param reg the register to store the resulting read
	 */
	inline void Read8toGPR(address_t address, uint32_t reg) GCC_INLINE;
	/** 32bits memory write.
	 * This method write the giving 32bits value into the memory system.
	 * 
	 * @param address the base address of the 32bits write
	 * @param value the value to write into memory
	 */
	inline void Write32(address_t address, uint32_t value) GCC_INLINE;
	/** 16bits memory write.
	 * This method write the giving 16bits value into the memory system.
	 * 
	 * @param address the base address of the 16bits write
	 * @param value the value to write into memory
	 */
	inline void Write16(address_t address, uint16_t value) GCC_INLINE;
	/** 8bits memory write.
	 * This method write the giving 8bits value into the memory system.
	 * 
	 * @param address the base address of the 8bits write
	 * @param value the value to write into memory
	 */
	inline void Write8(address_t address, uint8_t value) GCC_INLINE;

	/**************************************************************/
	/* Memory access methods       END                            */
	/**************************************************************/
	
	/* TODO: this method needs to be implemented. Should it be removed? */
	/** Check that an address is aligned
	 */
	void CheckAlignmentExcep(address_t addr);
	/* END TODO */
		
private:
	typename isa::arm32::Decoder<CONFIG> arm32_decoder;
	typename isa::thumb::Decoder<CONFIG> thumb_decoder;
		
	/*
	 * Memory access variables
	 */
	/**
	 * this variable is used to compute the effective address of a 1 byte memory access
	 * using the arm endianess mapping (only if os support is not connected to the simulator.
	 * this variable is set during the setup face or later when the endianess of the processor
	 * is changed
	 */
	address_t munged_address_mask8;
	/**
	 * this variable is used to compute the effective address of a 2 byte memory access
	 * using the arm endianess mapping (only if os support is not connected to the simulator.
	 * this variable is set during the setup face or later when the endianess of the processor
	 * is changed
	 */
	address_t munged_address_mask16;
		
	/** 
	 * the copy of the last cache line accessed by the cpu and its address, 
	 *   this speed ups execution, because the memory is not accessed */
	insn_t insn_cache_line[8];
	address_t insn_cache_line_address;
	/** the interface to the memory system */
	CacheInterface<address_t> *memory_interface;
		
		/** the table of coprocessors */
		CPInterface<CONFIG::DEBUG_ENABLE> *cp[16];
		/** cp15 for the arm966e_s */
		cp15_966es_t *cp15_966es;
		
		/** dtcm for the arm966e_s */
		dtcm_t *dtcm;
		/** itcm for the arm966e_s */
		itcm_t *itcm;
		
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
		
		/** the cache level 1 if data and instruction caches are unified
		 *  and data cache level 1 if not */
		CacheInterfaceWithMemoryService<address_t>
		*cache_l1;
		/** the instruction cache level 1 if instruction and data cache are not unified */
		CacheInterfaceWithMemoryService<address_t>
		*cache_il1;
		/** the unified cache level 2 */
		CacheInterfaceWithMemoryService<address_t>
		*cache_l2;
		
		/** this method initialize the cache/mmu/cp15 memory system */
		void CreateMemorySystem();
		
		/** this method creates the different coprocessors.
		 * This method needs to be called before CreateMemorySystem */
		void CreateCpSystem();
		
		/** this method creates the different TCM components.
		 * This method needs to be called before CreateMemorySystem 
		 * and CreateCpSystem */
		void CreateTCMSystem();
		
		/** indicates if the memory accesses require to be reported */
		bool requires_memory_access_reporting;
		/** indicates if the finished instructions require to be reported */
		bool requires_finished_instruction_reporting;
		
		address_t fetch_pc;
		Instruction<CONFIG> *fetchQueue;
		Instruction<CONFIG> *decodeQueue;
		Instruction<CONFIG> *executeQueue;
		void StepCycleFetch();
		void StepCycleDecode();
		void StepCycleExecute();
		void StepCycleExecuteInstruction();
		void StepCycleExecuteThumbInstruction();
		void StepCycleExecuteArm32Instruction();
		void StepCycleExecuteFinishInstruction();
		
		/** queue of memory operations
		 * Load and store instructions do not directly access the memory system. This is called
		 *   memory decoupling. Load/stores are kept in the load/store queue and performed once
		 *   the instruction has been executed (you can think of it as a writeback face).
		 *   To perform the memory accesses at the end of the execution the method 
		 *   PerformLoadStoreAccesses must be called 
		 * */
		queue<MemoryOp<CONFIG> *> lsQueue;
		/** current memory operation
		 * First load/store operation pending to be sent to the memory system
		 * */
		MemoryOp<CONFIG> *firstLS;
		/**
		 */
		bool hasSentFirstLS;
		/** queue of free entries of memory operations
		 * Used memory operations are not removed from the system but kept in a free list for their
		 *   reuse
		 * */
		queue<MemoryOp<CONFIG> *> freeLSQueue;
		/** performs the load/stores present in the queue of memory operations
		 * */
		void PerformLoadStoreAccesses();
		/** performs a prefetch access
		 * @param memop the memory operation containing the prefetch access
		 * */
		void PerformPrefetchAccess(MemoryOp<CONFIG> *memop);
		/** performs a write access
		 * @param memop the memory operation containing the write access
		 * */
		void PerformWriteAccess(MemoryOp<CONFIG> *memop);
		/** performs a read access
		 * @param memop the memory operation containing the read access
		 * */
		void PerformReadAccess(MemoryOp<CONFIG> *memop);
		/** performs a read access and puts result in the PC register
		 * @param memop the memory operation containing the read access
		 * */
		void PerformReadToPCAccess(MemoryOp<CONFIG> *memop);
		/** performs a read access and puts result in the PC register
		 * Performs a read access and puts result in the PC register and updates the
		 * thumb status if necessary
		 * 
		 * @param memop the memory operation containing the read access
		 * */
		void PerformReadToPCUpdateTAccess(MemoryOp<CONFIG> *memop);
	public:
		/** action to perform (execute) when an unpredictable behavior instruction
		 *  is found */
		inline void Unpredictable() GCC_INLINE;
		
	protected:
		/** the instruction counter */
		uint64_t instruction_counter;
		
		uint32_t statistics_id;
		// endianness parameter
		endian_type default_endianness;
		string default_endianness_string;
		// parameters for the 966E_S configuration
		bool arm966es_initram;
		bool arm966es_vinithi;
		/**********************************************************************/
		/** CPU Cycle Time/Voltage/Bus Cycle Time                             */
		/** START *************************************************************/

		uint64_t cpu_cycle_time; //!< CPU cycle time in ps
		uint64_t voltage;        //!< CPU voltage in mV
		uint64_t cpu_cycle;      //!< Number of cpu cycles
		uint64_t bus_cycle;      //!< Number of front side bus cycles

		/** END ***************************************************************/
		/** CPU Cycle Time/Voltage/Bus Cycle Time                             */
		/**********************************************************************/
		// verbose parameters
		bool verbose_all;
		bool verbose_setup;
		bool verbose_step;
		bool verbose_exception;
		bool verbose_dump_regs_start;
		bool verbose_dump_regs_end;
		bool verbose_memory;
		// trap parameters
		bool trap_on_exception;
		uint64_t trap_on_instruction_counter;
		
		unisim::kernel::service::Parameter<string> param_default_endianness;
		unisim::kernel::service::Parameter<bool> param_arm966es_initram;
		unisim::kernel::service::Parameter<bool> param_arm966es_vinithi;
		unisim::kernel::service::Parameter<uint64_t> param_cpu_cycle_time;
		unisim::kernel::service::Parameter<uint64_t> param_voltage;
		unisim::kernel::service::Parameter<bool> param_verbose_all;
		unisim::kernel::service::Parameter<bool> param_verbose_setup;
		unisim::kernel::service::Parameter<bool> param_verbose_step;
		unisim::kernel::service::Parameter<bool> param_verbose_exception;
		unisim::kernel::service::Parameter<bool> param_verbose_dump_regs_start;
		unisim::kernel::service::Parameter<bool> param_verbose_dump_regs_end;
		unisim::kernel::service::Parameter<bool> param_verbose_memory;
		unisim::kernel::service::Parameter<bool> param_trap_on_exception;
		unisim::kernel::service::Parameter<uint64_t> param_trap_on_instruction_counter;
		
		unisim::kernel::service::Statistic<uint64_t> stat_instruction_counter;
		
		unisim::kernel::service::Register<uint32_t> *reg_phys_gpr[32];
		unisim::kernel::service::Register<uint32_t> *reg_gpr[16];
		unisim::kernel::service::Register<uint32_t> reg_sp; // alias of reg_gpr[13]
		unisim::kernel::service::Register<uint32_t> reg_lr; // alias of reg_gpr[14]
		unisim::kernel::service::Register<uint32_t> reg_pc; // alias of reg_gpr[15]
		unisim::kernel::service::Register<uint32_t> reg_cpsr;
		unisim::kernel::service::Register<uint32_t> *reg_spsr[5];

		
		/**********************************************************************/
		/** ARM926EJ-S specific variables, functions and parameters           */
		/** START *************************************************************/

		arm926ejs::Cache arm926ejs_icache;
		arm926ejs::Cache arm926ejs_dcache;
		arm926ejs::ITCM arm926ejs_itcm;
		arm926ejs::DTCM arm926ejs_dtcm;
		arm926ejs::WriteBuffer arm926ejs_wb;
		arm926ejs::WriteBackBuffer arm926ejs_wbb;

		uint32_t arm926ejs_icache_size;
		uint32_t arm926ejs_dcache_size;
		bool arm926ejs_enable_write_buffer;
		bool arm926ejs_enable_writeback_buffer;
		unisim::kernel::service::Parameter<uint32_t>
			*param_arm926ejs_icache_size;
		unisim::kernel::service::Parameter<uint32_t>
			*param_arm926ejs_dcache_size;
		unisim::kernel::service::Parameter<bool> *param_arm926ejs_enable_wb;
		unisim::kernel::service::Parameter<bool> *param_arm926ejs_enable_wbb;
		
		/** END ***************************************************************/
		/** ARM926EJ-S specific variables, functions and parameters           */
		/**********************************************************************/

		// verbose methods
		inline bool VerboseSetup() GCC_INLINE;
		inline bool VerboseStep() GCC_INLINE;
		inline bool VerboseException() GCC_INLINE;
		inline void VerboseDumpRegs() GCC_INLINE;
		inline void VerboseDumpRegsStart() GCC_INLINE;
		inline void VerboseDumpRegsEnd() GCC_INLINE;
		inline bool TrapOnException() GCC_INLINE;
		
	};
	
} // end of namespace armemu
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__

