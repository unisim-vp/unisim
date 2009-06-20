/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_HH

#include <stdlib.h>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
//#include <unisim/service/interfaces/logger.hh>
#include "unisim/kernel/logger/logger.hh"
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/queue/queue.hh>
#include <map>
#include <iostream>
#include <stdio.h>
#include <boost/integer.hpp>
#include <stack>
#include <bitset>
#include <unisim/component/cxx/processor/tesla/register.hh>
#include <unisim/component/cxx/processor/tesla/warp.hh>
#include <unisim/component/cxx/processor/tesla/flags.hh>
#include <unisim/component/cxx/processor/tesla/maskstack.hh>
#include <unisim/component/cxx/processor/tesla/implicit_flow.hh>
#include <unisim/component/cxx/processor/tesla/tesla_flow.hh>
#include <unisim/component/cxx/processor/tesla/hostfloat/hostfloat.hh>
#include <unisim/component/cxx/processor/tesla/sampler.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using namespace unisim::util::endian;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::service::interfaces::Loader;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::ParameterArray;
using namespace std;
using unisim::util::queue::Queue;
using namespace boost;

using unisim::kernel::logger::Logger;

template <class CONFIG>
class CPU :
//	public Decoder<CONFIG>
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<Loader<typename CONFIG::physical_address_t> >,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
//	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
//	public Client<TrapReporting>,
//	public Service<MemoryAccessReportingControl>,
//	public Service<unisim::service::interfaces::Registers>,
	public Client<Memory<typename CONFIG::address_t> >
//	public Client<Logger>
//	public Client<CachePowerEstimator>,
//	public Client<PowerMode>,
//	public Service<Synchronizable>
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::virtual_address_t virtual_address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;
	typedef typename CONFIG::insn_t insn_t;
	typedef typename CONFIG::float_t float_t;
	typedef VectorRegister<CONFIG> VecReg;
	typedef VectorFlags<CONFIG> VecFlags;
	typedef VectorAddress<CONFIG> VecAddr;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	
//	static const uint32_t MEMORY_PAGE_SIZE = CONFIG::MEMORY_PAGE_SIZE;
	
	static uint32_t const WARP_SIZE = CONFIG::WARP_SIZE;
	static uint32_t const MAX_WARPS_PER_BLOCK = CONFIG::MAX_WARPS_PER_BLOCK;
	static uint32_t const MAX_WARPS = CONFIG::MAX_WARPS;
	static uint32_t const MAX_VGPR = CONFIG::MAX_VGPR;
	static uint32_t const MAX_BLOCKS = CONFIG::MAX_BLOCKS;
	static uint32_t const MAX_THREADS = MAX_WARPS * WARP_SIZE;
	static uint32_t const SHARED_MEM_SIZE = CONFIG::SHARED_SIZE;
//	static uint32_t const BRANCH_STACK_DEPTH = CONFIG::BRANCH_STACK_DEPTH;
//	static uint32_t const CALL_STACK_DEPTH = CONFIG::CALL_STACK_DEPTH;
	static uint32_t const MAX_ADDR_REGS = CONFIG::MAX_ADDR_REGS;
	static uint32_t const MAX_PRED_REGS = CONFIG::MAX_PRED_REGS;


	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<address_t> > disasm_export;

//	ServiceExport<unisim::service::interfaces::Registers> registers_export;
	ServiceExport<Memory<address_t> > memory_export;
	ServiceExport<MemoryInjection<address_t> > memory_injection_export;
//	ServiceExport<Synchronizable> synchronizable_export;
//	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<Loader<physical_address_t> > kernel_loader_import;
	ServiceImport<DebugControl<address_t> > debug_control_import;
//	ServiceImport<MemoryAccessReporting<address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<physical_address_t> > memory_import;
//	ServiceImport<TrapReporting> trap_reporting_import;
//	ServiceImport<Logger> logger_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0, int coreid = 0);
	virtual ~CPU();
	
	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool Setup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================
	
	bool Step();	// -> true when finished
	void Run();
	virtual void Stop(int ret);
	virtual void Synchronize();
	virtual void Reset();
	void Reset(unsigned int threadsperblock, unsigned int numblocks, unsigned int regnum, unsigned int smsize);
	void InitStats(unsigned int code_size);

//	virtual void BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, WIMG wimg = CONFIG::WIMG_DEFAULT, bool rwitm = false);
//	virtual void BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, WIMG wimg = CONFIG::WIMG_DEFAULT);
//	virtual void BusZeroBlock(physical_address_t physical_addr);
//	virtual void BusFlushBlock(physical_address_t physical_addr);

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	// PowerPC Linux OS Interface
//	virtual void PerformExit(int ret);
	
	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
//	bool ReadMemory(address_t addr, void *buffer, uint32_t size, MemoryType mt, bool translate_addr);
//	bool WriteMemory(address_t addr, const void *buffer, uint32_t size, MemoryType mt, bool translate_addr);
	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size);

	void ReadShared(address_t addr, void *buffer, uint32_t size);
	void WriteShared(address_t addr, const void *buffer, uint32_t size);
	void SetSampler(Sampler<CONFIG> const & sampler, unsigned int n);
	

	//=====================================================================
	//=                         utility methods                           =
	//=====================================================================
	
	static uint32_t Mask(uint32_t mb, uint32_t me)
	{
		return (mb > me) ? ~(((uint32_t) -1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1))) : (((uint32_t)-1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1)));
	}
	
	static uint32_t CRFieldMask(uint32_t crfD)
	{
		return (crfD != 7 ? 0xFFFFFFFFul >> ((crfD + 1) * 4) : 0) | (crfD != 0 ? (0xFFFFFFFFul << (8 - crfD) * 4) : 0);
	}

	//=====================================================================
	//=                    user level registers set/get methods           =
	//=====================================================================
	
	
	//=====================================================================
	//=                  instruction address set/get methods              =
	//=====================================================================
	
	inline address_t GetPC() const { return CurrentWarp().pc; }
	inline void SetPC(address_t value) { CurrentWarp().pc = value; }
	inline address_t GetNPC() const { return CurrentWarp().npc; }
	inline address_t & GetNPC() { return CurrentWarp().npc; }
	inline void SetNPC(address_t value) { CurrentWarp().npc = value; }
	
	
	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

// 	address_t GetEA() const { return effective_address; }
	
//	Parameter<uint64_t> param_trap_on_instruction_counter;
	virtual string Disasm(address_t addr, address_t& next_addr);
	virtual const char *GetArchitectureName() const;
	inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	string GetObjectFriendlyName(address_t addr);
	string GetFunctionFriendlyName(address_t addr);
	bool ProcessCustomDebugCommand(const char *custom_debug_command);
	inline void MonitorLoad(address_t ea, uint32_t size);
	inline void MonitorStore(address_t ea, uint32_t size);
	
	void DumpRegisters(int warpid, ostream & os) const;
	void DumpGPR(int warpid, int reg, ostream & os) const;
	void DumpGPR(int reg, ostream & os) const;

	void DumpFlags(int warpid, int reg, ostream & os) const;
	void DumpFlags(int reg, ostream & os) const;

	void DumpAddr(int reg, ostream & os) const;

	//=====================================================================
	//=          DEC/TBL/TBU bus-time based update methods                =
	//=====================================================================
	
	void OnBusCycle();

	
	
	

	//=====================================================================
	//=                 Execution helper functions                        =
	//=====================================================================


	// Control flow
	void Meet(address_t addr);
	void PreBreak(address_t addr);
	bool Join();
	void End();
	void Fence();
	void Kill(std::bitset<CONFIG::WARP_SIZE> mask);
	void Return(std::bitset<CONFIG::WARP_SIZE> mask);
	void Break(std::bitset<CONFIG::WARP_SIZE> mask);


//	void ExecMarker(uint32_t marker);
//	VecReg ReadOperandFP32(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg);
//	VecReg ReadOperandFP32(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm);
//	VecReg ReadOperand(uint32_t reg, uint32_t cm, uint32_t sh);
//	VecReg ReadImmediate(uint32_t imm_hi, uint32_t imm_lo);
//	void WriteOutput(VecReg const & v, uint32_t reg, uint32_t pred_cond, uint32_t pred_reg);
//	void WritePred(uint32_t set_pred_reg, uint32_t pred_cond, uint32_t pred_reg, VecFlags flags);	// To be called last - can modify its own predicate

	
	VecReg & GetGPR(unsigned int reg);	// for current warp
	VecReg GetGPR(unsigned int reg) const;	// for current warp
	
	VecReg & GetGPR(unsigned int wid, unsigned int reg);
	VecReg GetGPR(unsigned int wid, unsigned int reg) const;
	
	VecFlags & GetFlags(unsigned int reg);	// for current warp
	VecFlags GetFlags(unsigned int reg) const;
	
	VecAddr & GetAddr(unsigned int reg);
	VecAddr GetAddr(unsigned int reg) const;
	
//	std::bitset<CONFIG::WARP_SIZE> & GetCurrentMask();
	std::bitset<CONFIG::WARP_SIZE> GetCurrentMask() const;

	void Branch(address_t target, std::bitset<CONFIG::WARP_SIZE> mask);
	void Call(address_t target);

	void StepWarp(uint32_t warpid);
	void CheckJoin();
	void CheckFenceCompleted();
	
	// Memory access helpers
	
	// Only for <= 32-bit accesses
	VecAddr LocalAddress(VecAddr const & addr, unsigned int segment);
	
	void ScatterGlobal(VecReg const output[], uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void GatherGlobal(VecReg output[], uint32_t src, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void ScatterLocal(VecReg const output[], uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void GatherLocal(VecReg output[], uint32_t src, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void ScatterShared(VecReg const & output, uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, std::bitset<CONFIG::WARP_SIZE> mask, SMType type);

	void GatherShared(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, SMType t = SM_U32);	// addr in bytes
	void GatherShared(VecReg & output, uint32_t src, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, std::bitset<CONFIG::WARP_SIZE> mask, SMType type);
	void GatherConstant(VecReg & output, uint32_t src, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, SMType type);

	void Gather(VecAddr const & addr, VecReg data[],
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void Scatter(VecAddr const & addr, VecReg const data[],
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);

	// High-level memory access
	void ReadShared(address_t addr, VecReg & data, SMType t = SM_U32);		// addr in WORDS!!
	VecReg ReadConstant(VecReg const & addr, uint32_t seg = 0);	// addr in bytes
	VecReg ReadConstant(unsigned int addr, uint32_t seg = 0);

	void LoadLocal32(VecReg & output, address_t addr);
	void StoreLocal32(VecReg const & output, address_t addr);


	// Low-level memory access
	void Gather32(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Scatter32(VecAddr const & addr, VecReg const & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Gather16(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Gather8(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Scatter16(VecAddr const & addr, VecReg const & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Scatter8(VecAddr const & addr, VecReg const & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Broadcast32(address_t addr, VecReg & data, uint32_t factor = 1, address_t offset = 0);
	void Broadcast16(address_t addr, VecReg & data, uint32_t factor = 1, address_t offset = 0);
	void Broadcast8(address_t addr, VecReg & data, uint32_t factor = 1, address_t offset = 0);
	
	void Read32(address_t addr, uint32_t & data, uint32_t factor = 1, address_t offset = 0);
	void Write32(address_t addr, uint32_t data, uint32_t factor = 1, address_t offset = 0);
	void Read16(address_t addr, uint32_t & data, uint32_t factor = 1, address_t offset = 0);
	void Write16(address_t addr, uint32_t data, uint32_t factor = 1, address_t offset = 0);
	void Read8(address_t addr, uint32_t & data, uint32_t factor = 1, address_t offset = 0);
	void Write8(address_t addr, uint32_t data, uint32_t factor = 1, address_t offset = 0);

	VecAddr EffectiveAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi,
		uint32_t addr_imm, uint32_t shift);
	
	void Sample1DS32(unsigned int sampler,
		VectorRegister<CONFIG> dest[],
		VectorRegister<CONFIG> const src[],
		uint32_t destBitfield);
	
	Warp<CONFIG> & CurrentWarp();
	Warp<CONFIG> const & CurrentWarp() const;
	
	Warp<CONFIG> & GetWarp(unsigned int wid);
	Warp<CONFIG> const & GetWarp(unsigned int wid) const;

	Sampler<CONFIG> & GetSampler(unsigned int s);
	
	typename CONFIG::operationstats_t & GetOpStats();
	
private:
	//=====================================================================
	//=                           G80 registers                           =
	//=====================================================================
	
	int coreid;

	Warp<CONFIG> warps[MAX_WARPS];
	uint32_t current_warpid;
	
	VecReg gpr[MAX_VGPR];
	Sampler<CONFIG> samplers[CONFIG::MAX_SAMPLERS];
	
	VecReg zero_reg;
	
	uint32_t num_warps;

	// GT200
//	bool mutex[SHARED_MEM_SIZE];
	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
//	address_t effective_address;
	uint64_t instruction_counter;                              //!< Number of executed instructions
	uint64_t max_inst;                                         //!< Maximum number of instructions to execute

	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
//	Parameter<uint64_t> param_cpu_cycle_time;             //!< linked to member cpu_cycle_time
//	Parameter<uint64_t> param_voltage;                    //!< linked to member voltage
//	Parameter<uint64_t> param_bus_cycle_time;             //!< linked to member bus_cycle_time
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst

	Parameter<bool> param_trace_insn;
	Parameter<bool> param_trace_mask;
	Parameter<bool> param_trace_reg;
	Parameter<bool> param_trace_reg_float;
	Parameter<bool> param_trace_loadstore;
	Parameter<bool> param_trace_branch;
	Parameter<bool> param_trace_sync;
	Parameter<bool> param_trace_reset;
	

	//=====================================================================
	//=                    CPU run-time statistics                        =
	//=====================================================================

	Statistic<uint64_t> stat_instruction_counter;
	Statistic<uint64_t> stat_cpu_cycle;                   //!< Number of cpu cycles
	Statistic<uint64_t> stat_bus_cycle;                   //!< Number of front side bus cycles
protected:
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================
	
//	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
//	uint64_t voltage;        //!< CPU voltage in mV
//	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
	uint64_t bus_cycle;      //!< Number of front side bus cycles
	uint64_t cpu_cycle;      //!< Number of cpu cycles

	void Fetch(typename CONFIG::insn_t & insn, typename CONFIG::address_t addr);
	
public:
	bool trace_insn;
	bool trace_mask;
	bool trace_reg;
	bool trace_reg_float;
	bool trace_loadstore;
	bool trace_branch;
	bool trace_sync;
	bool trace_reset;

	Logger trace_logger;

	typename CONFIG::stats_t* stats;
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
