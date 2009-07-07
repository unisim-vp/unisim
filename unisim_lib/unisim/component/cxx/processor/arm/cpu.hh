/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

#ifdef SOCLIB

#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"
#include "unisim/component/cxx/processor/arm/cache/cache.hh"
#include "unisim/component/cxx/processor/arm/coprocessor_interface.hh"
#include "unisim/component/cxx/processor/arm/coprocessor/arm966e_s/cp15.hh"
#include "unisim/component/cxx/processor/arm/tcm/tcm.hh"
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

#else // SOCLIB

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
#include "unisim/util/debug/register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"
#include "unisim/component/cxx/processor/arm/cache/cache.hh"
#include "unisim/component/cxx/processor/arm/coprocessor_interface.hh"
#include "unisim/component/cxx/processor/arm/coprocessor/arm966e_s/cp15.hh"
#include "unisim/component/cxx/processor/arm/tcm/tcm.hh"
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

#endif // SOCLIB

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

#ifdef SOCLIB
	
	using std::string;
	using std::stringstream;
	using std::map;
	using std::ostream;
	using std::vector;
	using std::queue;
	
#ifdef PROFILE_ARM966
	class insn_profile_t {
	public:
		uint64_t ex_time;
		uint64_t num_times_executed;
	};
	class mem_profile_t {
	public:
		uint64_t num_read;
		uint64_t num_write;
	};
#endif // PROFILE_ARM966
	
#else
	
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
	using unisim::util::debug::Register;
	using unisim::util::arithmetic::Add32;
	using unisim::component::cxx::processor::arm::CacheInterface;
	using unisim::component::cxx::processor::arm::CacheInterfaceWithMemoryService;
	using unisim::component::cxx::processor::arm::cache::Cache;
	using unisim::component::cxx::processor::arm::Instruction;
	using unisim::util::endian::endian_type;
	using std::string;
	using std::stringstream;
	using std::map;
	using std::ostream;
	using std::vector;
	using std::queue;
	
#endif // SOCLIB
	template<class CONFIG>
	class CPU :
#ifdef SOCLIB
	
	public CPUCPInterface
	
#else
	
	public CPUCPInterface,
	// public Client<Loader<typename CONFIG::address_t> >,
    public Client<LinuxOS>,
    public Service<CPULinuxOS>,
    public Service<MemoryInjection<uint64_t> >,
	public Client<DebugControl<uint64_t> >,
	public Client<MemoryAccessReporting<uint64_t> >,
	public Client<TrapReporting>,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<uint64_t> >,
    public Service<Registers>,
	public Service<Memory<uint64_t> > 
	
#endif // SOCLIB
	
	{
		
#ifndef SOCLIB
		
		/* profiling methods          START */
	private:
		map<uint64_t, uint32_t> profile;
	public:
		void DumpInstructionProfile(ostream *output);
		/* profiling methods          END */
		
#endif // SOCLIB
		
	private:
		typedef typename CONFIG::address_t address_t;
		typedef typename CONFIG::reg_t reg_t;
		typedef typename CONFIG::sreg_t sreg_t;
		typedef typename CONFIG::insn_t insn_t;
		typedef typename CONFIG::thumb_insn_t thumb_insn_t;
		
		typedef 
			unisim::component::cxx::processor::arm::coprocessor::arm966e_s::CP15<CONFIG> 
			cp15_966es_t;
		typedef
			unisim::component::cxx::processor::arm::tcm::DTCM<CONFIG>
			dtcm_t;
		typedef
			unisim::component::cxx::processor::arm::tcm::ITCM<CONFIG>
			itcm_t;
		
	public:
		//=====================================================================
		//=                  public service imports/exports                   =
		//=====================================================================
		
#ifndef SOCLIB
		
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
		ServiceImport<TrapReporting> trap_reporting_import;
		
		// the kernel logger
		unisim::kernel::logger::Logger logger;
		
#endif // SOCLIB
		
		//=====================================================================
		//=                    Constructor/Destructor                         =
		//=====================================================================
		
#ifdef SOCLIB
		
		CPU(CacheInterface<address_t> *memory_interface);
		~CPU();
#ifdef PROFILE_ARM966
		void EndProfile();
#endif // PROFILE_ARM966
		
#else
		
		CPU(const char *name, CacheInterface<address_t> *memory_interface, Object *parent = 0);
		virtual ~CPU();
		
#endif // SOCLIB
		
		//=====================================================================
		//=                  Client/Service setup methods                     =
		//=====================================================================
		
#ifndef SOCLIB
		
		virtual bool Setup();
		virtual void OnDisconnect();
		
#endif // SOCLIB
		
		//=====================================================================
		//=                    execution handling methods                     =
		//=====================================================================
		
#ifdef SOCLIB
		
		void SetEndianness(endian_type type);
		
		virtual void Stop(int ret);
		
		void Reset();
		
		uint32_t IsBusy();
		void Step();
		void StepCycle();
		void NullStep(uint32_t time_passed = 1);
		
		void GetInstructionRequest(bool &req, uint32_t &addr) const;
		void SetInstruction(bool error, uint32_t val);
		
		void GetDataRequest(bool &reg, bool &is_read, int &size, uint32_t &addr,
							uint32_t &data) const;
		void SetDataResponse(bool error, uint32_t rdata);
		void SetWriteBerr();
		
		void SetIrq(uint32_t irq);
		
		// processor internal registers access API, used by
		// debugger. Register numbering must match gdb packet order.
		
		unsigned int GetDebugRegisterCount() const;
		uint32_t GetDebugRegisterValue(unsigned int reg) const;
		void SetDebugRegisterValue(unsigned int reg, uint32_t value);
		size_t GetDebugRegisterSize(unsigned int reg) const;
		
		uint32_t GetDebugPC() const;
		void SetDebugPC(uint32_t);
		
	private:
		void StepExecute();
		void FlushPipeline();
		
	public:
#else // SOCLIB
		
		void OnBusCycle();
		/** Execute one complete instruction
		 * */
		void StepInstruction();
		/** Execute one processor cycle 
		 * */
		void StepCycle();
		/** Execute at most a given number of processor cycles
		 * This method executes at most max_num_cycles processor cycles. Returns
		 *   the number of processor cycles executed.
		 *
		 * @param max_num_cycles the maximum number of processor cycles
		 * @return the number of processor cycles executed
		 * */
		uint32_t StepCycle(uint32_t max_num_cycles);
		void Run();
		virtual void Stop(int ret);
		virtual void Sync();
		
#endif // SOCLIB
		
		// methods required by coprocessors
		virtual void CoprocessorStop(unsigned int cp_id, int ret);
		virtual endian_type CoprocessorGetEndianness();
		virtual bool CoprocessorGetVinithi();
		virtual bool CoprocessorGetInitram();
		
#ifndef SOCLIB
		
		//=====================================================================
		//=             memory injection interface methods                    =
		//=====================================================================
		virtual bool InjectReadMemory(uint64_t addr, void *buffer, uint32_t size);
		virtual bool InjectWriteMemory(uint64_t addr, const void *buffer, uint32_t size);
		
		//=====================================================================
		//=             memory access reporting control interface methods     =
		//=====================================================================
		
		virtual void RequiresMemoryAccessReporting(bool report);
		virtual void RequiresFinishedInstructionReporting(bool report) ;
		
		//=====================================================================
		//=             non intrusive memory methods                          =
		//=====================================================================
		
		virtual bool ReadMemory(uint64_t addr, void *buffer, uint32_t size);
		virtual bool WriteMemory(uint64_t addr, const void *buffer, uint32_t size);
		virtual bool ExternalReadMemory(uint64_t addr, void *buffer, uint32_t size);
		virtual bool ExternalWriteMemory(uint64_t addr, const void *buffer, uint32_t size);
		virtual bool CoprocessorReadMemory(uint64_t addr, void *buffer, uint32_t size);
		virtual bool CoprocessorWriteMemory(uint64_t addr, const void *buffer, uint32_t size);
		
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
		//=                   Debugging methods                               =
		//=====================================================================
		
		inline uint64_t GetInstructionCounter() const 
		{ return instruction_counter; }
		string GetObjectFriendlyName(uint64_t addr);
		string GetFunctionFriendlyName(uint64_t addr);
		
#endif // SOCLIB
		
		/**************************************************************/
		/* Operand decoding methods     START                         */
		/**************************************************************/
		/* Data processing operand decoding */
		static inline reg_t ShiftOperand32imm(const uint32_t rotate_imm, 
											  const uint32_t imm) GCC_INLINE;
		inline reg_t ShiftOperand32imm(const uint32_t rotate_imm, 
									   const uint32_t imm, 
									   bool *shift_carry_out) GCC_INLINE;
		inline reg_t ShiftOperandImmShift(const uint32_t shift_imm, 
										  const uint32_t shift, 
										  const reg_t val_reg) GCC_INLINE;
		inline reg_t ShiftOperandImmShift(const uint32_t shift_imm, 
										  const uint32_t shift, 
										  const reg_t val_reg, 
										  bool *shift_carry_out) GCC_INLINE;
		static inline reg_t ShiftOperandRegShift(const uint32_t shift_reg, 
												 const uint32_t shift, 
												 const reg_t val_reg) GCC_INLINE;
		inline reg_t ShiftOperandRegShift(const uint32_t shift_reg, 
										  const uint32_t shift, 
										  const reg_t val_reg,
										  bool *shift_carry_out) GCC_INLINE;
		
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
		inline address_t CLSOpDec(const uint32_t u,
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
		
#ifndef SOCLIB
		
		// Linux OS Interface
		virtual void PerformExit(int ret);
		
#endif // SOCLIB
		
		// Endian interface
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
		/** reads a complete cache line from the memory system
		 * This method reads a full cache line of processor instructions from the
		 *   memory system (accessing the pertinent cache levels as necessary)
		 * 
		 * @param address the base address to read the data from
		 * @param val an array of bytes containing the instructions
		 * @param size the number of bytes to read
		 */
		inline void ReadInsnLine(address_t address, uint8_t *val, uint32_t size) GCC_INLINE;
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
		
		/**************************************************************/
		/* Coprocessor methods          START                         */
		/**************************************************************/
		
		/** Coprocessor Load
		 * This method is called each time a coprocessor load (for whatever coprocessor) 
		 *   is found in the instruction set.
		 * 
		 * @param cp_num the coprocessor for which the load is requested
		 * @param address the address for the load
		 * 
		 * @return the target coprocessor decides if the loads have finished by returning
		 *         true, false if it has not finished
		 */ 
		bool CoprocessorLoad(uint32_t cp_num, address_t address);
		/** Coprocessor Load with options
		 * This method is called each time a coprocessor load (for whatever coprocessor) 
		 *   is found in the instruction set. This method has an extra field to pass 
		 *   options to the coprocessor.
		 * 
		 * @param cp_num the coprocessor for which the load is requested
		 * @param address the address for the load
		 * @param option options for the target coprocessor
		 * 
		 * @return the target coprocessor decides if the loads have finished by returning
		 *         true, false if it has not finished
		 */ 
		bool CoprocessorLoad(uint32_t cp_num, address_t address, uint32_t option);
		/** Coprocessor Store
		 * This method is called each time a coprocessor store (for whatever coprocessor) 
		 *   is found in the instruction set.
		 * 
		 * @param cp_num the coprocessor for which the store is requested
		 * @param address the address for the store
		 * 
		 * @return the target coprocessor decides if the stores have finished by returning
		 *         true, false if it has not finished
		 */ 
		bool CoprocessorStore(uint32_t cp_num, address_t address);
		/** Coprocessor Store with options
		 * This method is called each time a coprocessor store (for whatever coprocessor) 
		 *   is found in the instruction set. This method has an extra field to pass 
		 *   options to the coprocessor.
		 * 
		 * @param cp_num the coprocessor for which the store is requested
		 * @param address the address for the store
		 * @param option options for the target coprocessor
		 * 
		 * @return the target coprocessor decides if the stores have finished by returning
		 *         true, false if it has not finished
		 */ 
		bool CoprocessorStore(uint32_t cp_num, address_t address, uint32_t option);
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
		 * Copies the value from the rd ARM register to the coprocessor cp_num register crn (and
		 *   depending on the opcodes to crm)
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
		/**
		 * Move to ARM register from Coprocessor
		 * Transfers a value from the coprocessor cp_num to an ARM register or to the condition flags
		 * 
		 * @param cp_num the coprocessor for the which the move operation is requested
		 * @param op1 first opcode for the coprocessor
		 * @param op2 second opcode for the coprocessor
		 * @param rd the destination ARM register
		 * @param crn specifies the coprocessor register that contains the first operand for the instruction
		 * @param crm additional coprocessor source or destination register 
		 */
		void MoveFromCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
								 uint32_t rd, uint32_t crn, uint32_t crm);
		
		/**************************************************************/
		/* Coprocessor methods          END                           */
		/**************************************************************/
		
		/**************************************************************/
		/* Exception methods            START                         */
		/**************************************************************/
		
		inline void PerformResetException() GCC_INLINE;
		inline void PerformUndefInsnException() GCC_INLINE;
		inline void PerformSWIException() GCC_INLINE;
		inline void PerformPrefetchAbortException() GCC_INLINE;
		inline void PerformDataAbortException() GCC_INLINE;
		inline void PerformIRQException() GCC_INLINE;
		inline void PerformFIQException() GCC_INLINE;
		
		inline address_t GetExceptionVectorAddr() GCC_INLINE;
		
		inline address_t GetResetExceptionAddr() GCC_INLINE;
		inline address_t GetUndefInsnExceptionAddr() GCC_INLINE;
		inline address_t GetSWIExceptionAddr() GCC_INLINE;
		inline address_t GetPrefetchAbortExceptionAddr() GCC_INLINE;
		inline address_t GetDataAbortExceptionAddr() GCC_INLINE;
		inline address_t GetIRQExceptionAddr() GCC_INLINE;
		inline address_t GetFIQExceptionAddr() GCC_INLINE;
		
		uint32_t has_pending_exception;
		enum {IRQ_IRQ = 1,
		FIQ_IRQ = 2};
		enum exception_mask_t {
			NONE = 0,
			RESET_EXCEPTION = 1,
			UNDEFINED_INSTRUCTION_EXCEPTION = 2,
			SOFTWARE_INTERRUPT_EXCEPTION = 4,
			PREFETCH_ABORT_EXCEPTION = 8,
			DATA_ABORT_EXCEPTION = 16,
			IRQ_EXCEPTION = 32,
			FIQ_EXCEPTION = 64
		};
#ifdef SOCLIB
		/** Handles possible exceptions
		 * This method checks if there is any pending exception and handles it.
		 * Returns true if the pipeline needs to be flushed.
		 * 
		 * @return true if the pipeline needs to be flushed.
		 */
		inline bool HandleExceptions() GCC_INLINE;
#endif
		/** Sets the given exception in the pending status
		 *
		 * @param exception the exception to set
		 */
		inline void SetPendingException(exception_mask_t exception) GCC_INLINE
		{
			has_pending_exception |= exception;
		}
		/** Clear the given exception from the pending status
		 *
		 * @param exception the exception to be cleared
		 */
		inline void ClearPendingException(exception_mask_t exception) GCC_INLINE
		{
			has_pending_exception &= ~(uint32_t)exception;
		}
		/** Returns true if an exception is pending
		 *
		 * @return true if an exception is pending
		 */
		inline bool HasPendingException() GCC_INLINE
		{
			return has_pending_exception != 0;
		}
		/** Returns true if the given exception is pending
		 *
		 * @param exception the exception to check
		 *
		 * @return true if the given exception is pending
		 */
		inline bool HasPendingException(exception_mask_t exception) GCC_INLINE
		{
			return has_pending_exception & exception;
		}
		
		/**************************************************************/
		/* Exception methods            END                           */
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
		typename CONFIG::address_t munged_address_mask8;
		/**
		 * this variable is used to compute the effective address of a 2 byte memory access
		 * using the arm endianess mapping (only if os support is not connected to the simulator.
		 * this variable is set during the setup face or later when the endianess of the processor
		 * is changed
		 */
		typename CONFIG::address_t munged_address_mask16;
		
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
		
#ifndef SOCLIB
		
		/** The registers interface for debugging purpose */
		map<string, Register *> registers_registry;
		
#endif // SOCLIB
		
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
#ifdef SOCLIB
	protected:
		/** defines if the first request in the lsQueue is an external access or not */
		bool external_memory_request;
#endif // SOCLIB
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
		// parameters for the 966E_S configuration
		bool arm966es_initram;
		bool arm966es_vinithi;
		// verbose parameters
		bool verbose_all;
		bool verbose_setup;
		bool verbose_step;
		bool verbose_exception;
		bool verbose_dump_regs_start;
		bool verbose_dump_regs_end;
		// trap parameters
		bool trap_on_exception;
		uint64_t trap_on_instruction_counter;
		
#ifndef SOCLIB
		
		unisim::kernel::service::Parameter<endian_type> param_default_endianness;
		unisim::kernel::service::Parameter<bool> param_arm966es_initram;
		unisim::kernel::service::Parameter<bool> param_arm966es_vinithi;
		unisim::kernel::service::Parameter<bool> param_verbose_all;
		unisim::kernel::service::Parameter<bool> param_verbose_setup;
		unisim::kernel::service::Parameter<bool> param_verbose_step;
		unisim::kernel::service::Parameter<bool> param_verbose_exception;
		unisim::kernel::service::Parameter<bool> param_verbose_dump_regs_start;
		unisim::kernel::service::Parameter<bool> param_verbose_dump_regs_end;
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
		
#endif // SOCLIB
		
		// verbose methods
		inline bool VerboseSetup() GCC_INLINE;
		inline bool VerboseStep() GCC_INLINE;
		inline bool VerboseException() GCC_INLINE;
		inline void VerboseDumpRegs() GCC_INLINE;
		inline void VerboseDumpRegsStart() GCC_INLINE;
		inline void VerboseDumpRegsEnd() GCC_INLINE;
		inline bool TrapOnException() GCC_INLINE;
		
#ifdef SOCLIB
#ifdef PROFILE_ARM966
	private:
		map<uint32_t, insn_profile_t *> insn_profile;
		map<uint32_t, mem_profile_t *> mem_profile;
		
		void MemProfile(bool read, uint32_t addr, uint32_t size);
#endif // PROFILE_ARM966
#endif // SOCLIB
	};
	
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

