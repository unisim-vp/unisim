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

#include "unisim/component/cxx/processor/arm/models.hh"
#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include "unisim/component/cxx/processor/arm/armemu/cache.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/debug_control.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
// #include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/component/cxx/processor/arm/isa_arm32.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/debug/register.hh"
#include <string>
#include <queue>
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

class CPU
	: public unisim::component::cxx::processor::arm::CPU
	, public unisim::kernel::service::Client<
	  	unisim::service::interfaces::LinuxOS>
	, public unisim::kernel::service::Service<
	  	unisim::service::interfaces::MemoryInjection<uint64_t> >
	, public unisim::kernel::service::Client<
	  	unisim::service::interfaces::DebugControl<uint64_t> >
	, public unisim::kernel::service::Client<
	  	unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
	, public unisim::kernel::service::Client<
	  	unisim::service::interfaces::TrapReporting>
	, public unisim::kernel::service::Service<
	  	unisim::service::interfaces::MemoryAccessReportingControl>
	, public unisim::kernel::service::Service<
	  	unisim::service::interfaces::Disassembly<uint64_t> >
	, public unisim::kernel::service::Service<
	  	unisim::service::interfaces::Registers >
	, public unisim::kernel::service::Service<
	  	unisim::service::interfaces::Memory<uint64_t> >
{
public:
	static const uint32_t MODEL = 
		unisim::component::cxx::processor::arm::ARMEMU;

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
		
	/** Disassembly service export. */
	unisim::kernel::service::ServiceExport<
		unisim::service::interfaces::Disassembly<uint64_t> >
		disasm_export;
	/** Registers service export. */
	unisim::kernel::service::ServiceExport<
		unisim::service::interfaces::Registers>
		registers_export;
	/** Memory injection service export. */
	unisim::kernel::service::ServiceExport<
		unisim::service::interfaces::MemoryInjection<uint64_t> > 
		memory_injection_export;
	/** Memory service export. */
	unisim::kernel::service::ServiceExport<
		unisim::service::interfaces::Memory<uint64_t> > 
		memory_export;
	/** Memory access reporting control service export. */
	unisim::kernel::service::ServiceExport<
		unisim::service::interfaces::MemoryAccessReportingControl> 
		memory_access_reporting_control_export;

	/** Debug control service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::DebugControl<uint64_t> >
		debug_control_import;
	/** Memory access reporting service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::MemoryAccessReporting<uint64_t> > 
		memory_access_reporting_import;
	/** Symbol table lookup service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::SymbolTableLookup<uint64_t> > 
		symbol_table_lookup_import;
	/** Linux OS service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::LinuxOS> 
		linux_os_import;
	/** Trap reporting service import. */
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::TrapReporting> 
		instruction_counter_trap_reporting_import;

	//=====================================================================
	//=                       Logger                                      =
	//=====================================================================
	
	/** Unisim logging services. */
	unisim::kernel::logger::Logger logger;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	/** Constructor.
	 *
	 * @param name the name that will be used by the UNISIM service 
	 *   infrastructure and will identify this object
	 * @param parent the parent object of this object
	 */
	CPU(const char *name, Object *parent = 0);
	/** Destructor.
	 */
	~CPU();

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	/** Object setup method.
	 * This method is required for all UNISIM objects and will be called during
	 *   the setup phase.
	 * 
	 * @return true on success, false otherwise
	 */
	virtual bool EndSetup();
	/** Object disconnect method.
	 * This method is called when this UNISIM object is disconnected from other
	 *   UNISIM objects.
	 */
	virtual void OnDisconnect();

	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================

	/** Execute one complete instruction.
	 */
	void StepInstruction();

	//=====================================================================
	//=                      memory methods                               =
	//=====================================================================

	/** Processor external memory write.
	 * Perform an external write memory access, that is an access that is not
	 *   in cache (or cache absent).
	 *
	 * @param addr the address of the access
	 * @param buffer byte buffer with the data to write
	 * @param size the size of the access
	 */
	virtual void PrWrite(uint32_t addr, 
			const uint8_t *buffer, 
			uint32_t size) = 0;
	/** Processor external memory read.
	 * Perform an external read memory access, that is an access that is not
	 *   in cache (or cache absent).
	 *
	 * @param addr the address of the access
	 * @param buffer byte buffer that will be filled with the read data
	 * @param size the size of the access
	 */
	virtual void PrRead(uint32_t addr,
			uint8_t *buffer,
			uint32_t size) = 0;

	//=====================================================================
	//=             memory injection interface methods                    =
	//=====================================================================

	/** Inject an intrusive read memory operation.
	 *
	 * @param addr the target address to read from
	 * @param buffer the buffer to fill with the read memory
	 * @param size the amount of bytes to read from starting from the target 
	 *   address
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool InjectReadMemory(uint64_t addr, 
			void *buffer,
			uint32_t size);
	/** Inject an intrusive write memory operation
	 *
	 * @param addr the target address to write to
	 * @param buffer a buffer containing the data to write
	 * @param size the amount of bytes to write
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool InjectWriteMemory(uint64_t addr, 
			const void *buffer, 
			uint32_t size);

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	/** Set/unset the reporting of memory accesses.
	 *
	 * @param report if true set the reporting of memory acesses, unset 
	 *   otherwise
	 */
	virtual void RequiresMemoryAccessReporting(bool report);
	/** Set/unset the reporting of finishing instructions.
	 * 
	 * @param report if true set the reporting of finishing instructions, 
	 *   unset otherwise
	 */
	virtual void RequiresFinishedInstructionReporting(bool report);

	//=====================================================================
	//=             non intrusive memory interface methods                =
	//=====================================================================

	/** Perform a non intrusive read access.
	 * This method performs a normal read, but it does not change the state
	 *   of the caches. It calls ExternalReadMemory if data not in cache.
	 *
	 * @param addr the address to read from
	 * @param buffer the buffer to fill with the read data (must be allocated
	 *   by the caller)
	 * @param size the amount of data to read
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool ReadMemory(uint64_t addr, 
			void *buffer, 
			uint32_t size);
	/** Perform a non intrusive write access.
	 * This method perform a normal write, but it does not change the state
	 *   of the caches. It calls ExternalWriteMemory in order to modify external
	 *   ocurrences of the address.
	 *
	 * @param addr the address to write to
	 * @param buffer the data to write
	 * @param size the amount of data to write
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool WriteMemory(uint64_t addr, 
			const void *buffer, 
			uint32_t size);
	/** Perform a non intrusive external read access.
	 * This method is called when a ReadAccess does not find the data in the
	 *   cache.
	 *
	 * @param addr the address to read from
	 * @param buffer the buffer to fill with the read data (must be allocated
	 *   by the caller)
	 * @param size the amount of data to read
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool ExternalReadMemory(uint64_t addr, 
			void *buffer, 
			uint32_t size) = 0;
	/** Perform a non intrusive external write access.
	 * This method is called to write non intrusively external memory locations.
	 *
	 * @param addr the address to write to
	 * @param buffer the data to write
	 * @param size the amount of data to write
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool ExternalWriteMemory(uint64_t addr, 
			const void *buffer, 
			uint32_t size) = 0;

	//=====================================================================
	//=             CPURegistersInterface interface methods               =
	//=====================================================================

	/** Get a register by its name.
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name the name of the requested register
	 *
	 * @return a pointer to the RegisterInterface corresponding to name
	 */
	virtual unisim::util::debug::Register *GetRegister(const char *name);
		
	//=====================================================================
	//=                   DebugDisasmInterface methods                    =
	//=====================================================================

	/** Disasm an instruction address.
	 * Returns a string with the disassembling of the instruction found 
	 *   at address addr.
	 * 
	 * @param addr the address of the instruction to disassemble
	 * @param next_addr the address following the requested instruction
	 *
	 * @return the disassembling of the requested instruction address
	 */
	virtual std::string Disasm(uint64_t addr, uint64_t &next_addr);
		
	//=====================================================================
	//=                   LinuxOSInterface methods                        =
	//=====================================================================
	
	/** Exit system call.
	 * The LinuxOS service calls this method when the program has finished.
	 *
	 * @param ret the exit cade sent by the exit system call.
	 */
	virtual void PerformExit(int ret);
	
	/**************************************************************/
	/* Memory access methods       START                          */
	/**************************************************************/
	
	/** Reads 32bits instructions from the memory system
	 * This method allows the user to read instructions from the memory system,
	 *   that is, it tries to read from the pertinent caches and if failed from
	 *   the external memory system.
	 * 
	 * @param address the address to read data from
	 * @param val the buffer to fill with the read data
	 */
	void ReadInsn(uint32_t address, uint32_t &val);
	/** Memory prefetch instruction.
	 * This method is used to make memory prefetches into the caches (if 
	 *   available), that is it sends a memory read that doesn't keep the 
	 *   request result.
	 * 
	 * @param address the address of the prefetch
	 */
	void ReadPrefetch(uint32_t address);
	/** 32bits memory read that stores result into the PC
	 * This methods reads 32bits from memory and stores the result into
	 *   the pc register of the CPU.
	 * 
	 * @param address the base address of the 32bits read
	 */
	void Read32toPCUpdateT(uint32_t address);
	/** 32bits memory read into the PC and updates thumb state.
	 * This methods reads 32bits from memory and stores the result into
	 *   the pc register of the CPU and updates thumb state if necessary
	 * 
	 * @param address the base address of the 32bits read
	 */
	void Read32toPC(uint32_t address);
	/** 32bits memory read into one of the general purpose registers.
	 * This method reads 32bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg
	 * 
	 * @param address the base address of the 32bits read
	 * @param reg the register to store the resulting read
	 */
	void Read32toGPR(uint32_t address, uint32_t reg);
	/** 32bits aligned memory read into one of the general purpose registers.
	 * This method reads 32bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. Note that this
	 *   read methods supposes that the address is 32bits aligned.
	 * 
	 * @param address the base address of the 32bits read
	 * @param reg the register to store the resulting read
	 */
	void Read32toGPRAligned(uint32_t address, uint32_t reg);
	/** 32bits memory read into one of the user general purpose registers.
	 * This method reads 32bits from memory and stores the result into
	 *   the user general purpose register indicated by the input reg. For the
	 *   armemu version this makes the same as Read32toGPR.
	 * 
	 * @param address the base address of the 32bits read
	 * @param reg the user register to store the resulting read
	 */
	void Read32toUserGPR(uint32_t address, uint32_t reg)
	{
		Read32toGPR(address, reg);
	}
	/** 32bits aligned memory read into one of the user general purpose registers.
	 * This method reads 32bits from memory and stores the result into
	 *   the user general purpose register indicated by the input reg. Note that
	 *   this read methods supposes that the address is 32bits aligned. For the
	 *   armemu version this makes the same as Read32toGPRAligned.
	 * 
	 * @param address the base address of the 32bits read
	 * @param reg the user register to store the resulting read
	 */
	void Read32toUserGPRAligned(uint32_t address, uint32_t reg)
	{
		Read32toGPRAligned(address, reg);
	}
	/** 16bits aligned memory read into one of the general purpose registers.
	 * This method reads 16bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. Note that this
	 *   read methods supposes that the address is 16bits aligned.
	 * 
	 * @param address the base address of the 16bits read
	 * @param reg the register to store the resulting read
	 */
	void Read16toGPRAligned(uint32_t address, uint32_t reg);
	/** Signed 16bits aligned memory read into one of the GPRs.
	 * This method reads 16bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. Note that this
	 *   read methods supposes that the address is 16bits aligned. The 16bits
	 *   value is considered signed and sign extended to the register size.
	 * 
	 * @param address the base address of the 16bits read
	 * @param reg the register to store the resulting read
	 */
	void ReadS16toGPRAligned(uint32_t address, uint32_t reg);
	/** 8bits memory read into one of the general purpose registers.
	 * This method reads 8bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg.
	 * 
	 * @param address the base address of the 8bits read
	 * @param reg the register to store the resulting read
	 */
	void ReadS8toGPR(uint32_t address, uint32_t reg);
	/** Signed 8bits memory read into one of the general purpose registers.
	 * This method reads 8bits from memory and stores the result into
	 *   the general purpose register indicated by the input reg. The 8bits 
	 *   value is considered signed and sign extended to the register size.
	 * 
	 * @param address the base address of the 8bits read
	 * @param reg the register to store the resulting read
	 */
	void Read8toGPR(uint32_t address, uint32_t reg);
	/** 32bits memory write.
	 * This method write the giving 32bits value into the memory system.
	 * 
	 * @param address the base address of the 32bits write
	 * @param value the value to write into memory
	 */
	void Write32(uint32_t address, uint32_t value);
	/** 16bits memory write.
	 * This method write the giving 16bits value into the memory system.
	 * 
	 * @param address the base address of the 16bits write
	 * @param value the value to write into memory
	 */
	void Write16(uint32_t address, uint16_t value);
	/** 8bits memory write.
	 * This method write the giving 8bits value into the memory system.
	 * 
	 * @param address the base address of the 8bits write
	 * @param value the value to write into memory
	 */
	void Write8(uint32_t address, uint8_t value);

	/**************************************************************/
	/* Memory access methods       END                            */
	/**************************************************************/

	/**************************************************************/
	/* Coprocessor methods          START                         */
	/* NOTE: These methods are only declared because the isa      */
	/*       requires those methods, but they are not implemented */
	/**************************************************************/

	/** Coprocessor Load
	 * This method is called each time a coprocessor load (for whatever 
	 *   coprocessor) is found in the instruction set.
	 * 
	 * @param cp_num the coprocessor for which the load is requested
	 * @param address the address for the load
	 * 
	 * @return the target coprocessor decides if the loads have finished by 
	 *   returning true, false if it has not finished
	 */ 
	bool CoprocessorLoad(uint32_t cp_num, uint32_t address);
	/** Coprocessor Load with options
	 * This method is called each time a coprocessor load (for whatever 
	 *   coprocessor) is found in the instruction set. This method has an extra 
	 *   field to pass options to the coprocessor.
	 * 
	 * @param cp_num the coprocessor for which the load is requested
	 * @param address the address for the load
	 * @param option options for the target coprocessor
	 * 
	 * @return the target coprocessor decides if the loads have finished by 
	 *   returning true, false if it has not finished
	 */ 
	bool CoprocessorLoad(uint32_t cp_num, uint32_t address, uint32_t option);
	/** Coprocessor Store
	 * This method is called each time a coprocessor store (for whatever 
	 *   coprocessor) is found in the instruction set.
	 * 
	 * @param cp_num the coprocessor for which the store is requested
	 * @param address the address for the store
	 * 
	 * @return the target coprocessor decides if the stores have finished by 
	 *   returning true, false if it has not finished
	 */ 
	bool CoprocessorStore(uint32_t cp_num, uint32_t address);
	/** Coprocessor Store with options
	 * This method is called each time a coprocessor store (for whatever 
	 *   coprocessor) is found in the instruction set. This method has an extra 
	 *   field to pass options to the coprocessor.
	 * 
	 * @param cp_num the coprocessor for which the store is requested
	 * @param address the address for the store
	 * @param option options for the target coprocessor
	 * 
	 * @return the target coprocessor decides if the stores have finished by 
	 *   returning true, false if it has not finished
	 */ 
	bool CoprocessorStore(uint32_t cp_num, uint32_t address, uint32_t option);
	/** Coprocessor Data Processing method
	 * 
	 * @param cp_num the coprocessor for which the data processing is requested
	 * @param op1 first opcode for the coprocessor
	 * @param op2 second opcode for the coprocessor
	 * @param crd coprocessor destination register
	 * @param crn coprocessor first operand register
	 * @param crm coprocessor second operand register
	 */
	void CoprocessorDataProcess(uint32_t cp_num, uint32_t op1, uint32_t op2,
								uint32_t crd, uint32_t crn, uint32_t crm);
	/** Move to Coprocessor from ARM register
	 * Copies the value from the rd ARM register to the coprocessor cp_num 
	 *   register crn (and depending on the opcodes to crm)
	 * 
	 * @param cp_num the coprocessor for which the move operation is requested
	 * @param op1 first opcode for the coprocessor
	 * @param op2 second opcode for the coprocessor
	 * @param rd the index of the ARM register whose value will be copied
	 * @param crn the destination coprocessor register
	 * @param crm the additional destination or source coprocessor register
	 */
	void MoveToCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
						   uint32_t rd, uint32_t crn, uint32_t crm);
	/** Move to ARM register from Coprocessor.
	 * Transfers a value from the coprocessor cp_num to an ARM register or to 
	 *   the condition flags
	 * 
	 * @param cp_num the coprocessor for the which the move operation is 
	 *   requested
	 * @param op1 first opcode for the coprocessor
	 * @param op2 second opcode for the coprocessor
	 * @param rd the destination ARM register
	 * @param crn specifies the coprocessor register that contains the first 
	 *   operand for the instruction
	 * @param crm additional coprocessor source or destination register 
	 */
	void MoveFromCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
							 uint32_t rd, uint32_t crn, uint32_t crm);

	/**************************************************************/
	/* Coprocessor methods          END                           */
	/**************************************************************/
		
 	/** Unpredictable Instruction Behaviour.
 	 * This method is just called when an unpredictable behaviour is detected to
 	 *   notifiy the processor.
 	 */
 	void UnpredictableInsnBehaviour();

	/** Instruction cache */
	Cache icache;
	/** Data cache */
	Cache dcache;
	
protected:
	/** Decoder for the arm32 instruction set. */
	unisim::component::cxx::processor::arm::isa::arm32::Decoder<
		unisim::component::cxx::processor::arm::armemu::CPU>
		arm32_decoder;
	/* The thumb instruction set decoder is not needed for armemu 
	 * typename isa::thumb::Decoder<CONFIG> thumb_decoder;
	 */

	/** Instruction counter */
	uint64_t instruction_counter;
	
	/** CPU cycle time in picoseconds.
	 */
	uint64_t cpu_cycle_time_ps;
	/** CPU voltage in mV, required to compute cache power consumption.
	 */
	uint64_t voltage;
	// uint64_t cpu_cycle;      //!< Number of cpu cycles
	// uint64_t bus_cycle;      //!< Number of front side bus cycles

	/** Indicates if the emulator is running in verbose mode.
	 * 0 indicates no verbose.
	 * Different than 0 indicates verbose.
	 */
	uint32_t verbose;
	/** Trap when reaching the number of instructions indicated.
	 */
	uint64_t trap_on_instruction_counter;
	/** String describing the endianness of the processor.
	 */
	std::string default_endianness_string;
	
	/** Indicates if the memory accesses require to be reported.
	 */
	bool requires_memory_access_reporting;
	/** Indicates if the finished instructions require to be reported.
	 */
	bool requires_finished_instruction_reporting;
		
	/************************************************************************/
	/* UNISIM parameters, statistics and registers                    START */
	/************************************************************************/

	/** UNISIM Parameter to set the default endianness.
	 */
	unisim::kernel::service::Parameter<std::string> param_default_endianness;
	/** UNISIM Parameter to set the CPU cycle time.
	 */
	unisim::kernel::service::Parameter<uint64_t> param_cpu_cycle_time_ps;
	/** UNISIM Parameter to set the CPU voltage.
	 */
	unisim::kernel::service::Parameter<uint64_t> param_voltage;
	/** UNISIM Parameter to set/unset verbose mode.
	 */
	unisim::kernel::service::Parameter<uint32_t> param_verbose;
	/** UNISIM Parameter to set traps on instruction counter.
	 */
	unisim::kernel::service::Parameter<uint64_t> 
		param_trap_on_instruction_counter;
	
	/** UNISIM Statistic of the number of instructions executed.
	 */
	unisim::kernel::service::Statistic<uint64_t> stat_instruction_counter;

	/** UNISIM registers for the physical registers.
	 */
	unisim::kernel::service::Register<uint32_t> *reg_phys_gpr[32];
	/** UNISIM registers for the logical registers.
	 */
	unisim::kernel::service::Register<uint32_t> *reg_gpr[16];
	/** UNISIM register for the stack pointer register (gpr 13).
	 */
	unisim::kernel::service::Register<uint32_t> reg_sp;
	/** UNISIM register for the link register (gpr 14).
	 */
	unisim::kernel::service::Register<uint32_t> reg_lr;
	/** UNISIM register for the program counter register (gpr 15).
	 */
	unisim::kernel::service::Register<uint32_t> reg_pc;
	/** UNISIM register for the CPSR register.
	 */
	unisim::kernel::service::Register<uint32_t> reg_cpsr;
	/** UNISIM registers for the SPRS registers.
	 */
	unisim::kernel::service::Register<uint32_t> *reg_spsr[5];

	/************************************************************************/
	/* UNISIM parameters, statistics and registers                      END */
	/************************************************************************/

	/************************************************************************/
	/* Memory queue and handling                                      START */
	/************************************************************************/
	
	/** Queue of memory operations.
	 * Load and store instructions do not directly access the memory system. 
	 *   This is called memory decoupling. Load/stores are kept in the 
	 *   load/store queue and performed once the instruction has been executed 
	 *   (you can think of it as a writeback face). To perform the memory 
	 *   accesses at the end of the execution the method 
	 *   PerformLoadStoreAccesses must be called 
	 */
	std::queue<unisim::component::cxx::processor::arm::MemoryOp *> ls_queue;
	/** Current memory operation.
	 * First load/store operation pending to be sent to the memory system
	 */
	unisim::component::cxx::processor::arm::MemoryOp *first_ls;
	/** Indicates if the firstLS item has been sent or not.
	 */
	bool has_sent_first_ls;
	/** Queue of free entries of memory operations.
	 * Used memory operations are not removed from the system but kept in a 
	 *   free list for their reuse.
	 */
	std::queue<unisim::component::cxx::processor::arm::MemoryOp *> 
		free_ls_queue;
	/** Performs the load/stores present in the queue of memory operations.
	 */
	void PerformLoadStoreAccesses();
	/** Performs a prefetch access.
	 *
	 * @param memop the memory operation containing the prefetch access
	 */
	void PerformPrefetchAccess(unisim::component::cxx::processor::arm::MemoryOp
			*memop);
	/** Performs a write access.
	 * @param memop the memory operation containing the write access
	 */
	void PerformWriteAccess(unisim::component::cxx::processor::arm::MemoryOp
			*memop);
	/** Performs a read access.
	 * @param memop the memory operation containing the read access
	 */
	void PerformReadAccess(unisim::component::cxx::processor::arm::MemoryOp
			*memop);
	/** Performs a read access and puts result in the PC register.
	 * @param memop the memory operation containing the read access
	 */
	void PerformReadToPCAccess(unisim::component::cxx::processor::arm::MemoryOp
			*memop);
	/** Performs a read access and puts result in the PC register.
	 * Performs a read access and puts result in the PC register and updates the
	 * thumb status if necessary
	 * 
	 * @param memop the memory operation containing the read access
	 */
	void PerformReadToPCUpdateTAccess(
			unisim::component::cxx::processor::arm::MemoryOp *memop);
};

} // end of namespace armemu
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARMEMU_CPU_HH__