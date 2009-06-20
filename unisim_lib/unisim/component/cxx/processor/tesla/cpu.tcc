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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_TCC

#include <unisim/component/cxx/processor/tesla/cpu.hh>
#include <unisim/component/cxx/processor/tesla/warp.tcc>
#include <unisim/component/cxx/processor/tesla/exception.tcc>
#include <unisim/component/cxx/processor/tesla/register.tcc>
#include <unisim/component/cxx/processor/tesla/strided_register.tcc>
#include <unisim/component/cxx/processor/tesla/instruction.tcc>
#include <unisim/component/cxx/processor/tesla/flags.tcc>
#include <unisim/component/cxx/processor/tesla/exec.tcc>
#include <unisim/component/cxx/processor/tesla/simfloat.tcc>
#include <unisim/component/cxx/processor/tesla/memory.tcc>
#include <unisim/component/cxx/processor/tesla/maskstack.tcc>
#include <unisim/component/cxx/processor/tesla/implicit_flow.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_flow.tcc>
#include <unisim/component/cxx/processor/tesla/hostfloat/hostfloat.tcc>
#include <unisim/component/cxx/processor/tesla/stats.tcc>
#include <unisim/component/cxx/processor/tesla/vectorfp32.tcc>
#include <unisim/component/cxx/processor/tesla/sampler.tcc>

//#include <unisim/component/cxx/cache/cache.tcc>
//#include <unisim/component/cxx/tlb/tlb.tcc>
//#include <unisim/util/queue/queue.tcc>

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <iomanip>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent, int coreid) :
	Object(name, parent),
	Service<Disassembly<typename CONFIG::address_t> >(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Service<MemoryInjection<typename CONFIG::address_t> >(name, parent),

	Client<Loader<typename CONFIG::physical_address_t> >(name, parent),
	Client<DebugControl<typename CONFIG::address_t> >(name, parent),
	Client<SymbolTableLookup<typename CONFIG::address_t> >(name, parent),
//	Client<MemoryAccessReporting<typename CONFIG::address_t> >(name, parent),
//	Client<TrapReporting>(name, parent),
//	Service<MemoryAccessReportingControl>(name, parent),
//	Service<unisim::service::interfaces::Registers>(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
//	Client<LinuxOS>(name, parent),
//	Client<Logger>(name, parent),
//	Client<CachePowerEstimator>(name, parent),
//	Client<PowerMode>(name, parent),
//	Service<Synchronizable>(name, parent),
	disasm_export("disasm-export", this),
//	registers_export("registers-export", this),
	memory_export("memory-export", this),
	memory_injection_export("memory-injection-export", this),
//	cpu_linux_os_export("cpu-linux-os-export", this),
//	memory_access_reporting_control_export("memory_access_reporting_control_export", this),
	kernel_loader_import("kernel-loader-import", this),
	debug_control_import("debug-control-import", this),
//	memory_access_reporting_import("memory-access-reporting-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	memory_import("memory-import", this),
//	trap_reporting_import("trap-reporting-import", this),
//	logger_import("logger-import", this),
//	synchronizable_export("synchronizable-export", this),
//	cpu_cycle_time(0),
//	voltage(0),
//	bus_cycle_time(0),
	coreid(coreid),
	zero_reg(0),
	instruction_counter(0),
	max_inst(0xffffffffffffffffULL),
//	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time),
//	param_voltage("voltage", this, voltage),
//	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time),
	param_max_inst("max-inst", this, max_inst),
//	param_verbose_all("verbose-all", this, verbose_all),
//	param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter),
	param_trace_insn("trace-insn", this, trace_insn),
	param_trace_mask("trace-mask", this, trace_mask),
	param_trace_reg("trace-reg", this, trace_reg),
	param_trace_reg_float("trace-reg-float", this, trace_reg_float),
	param_trace_loadstore("trace-loadstore", this, trace_loadstore),
	param_trace_branch("trace-branch", this, trace_branch),
	param_trace_sync("trace-sync", this, trace_sync),
	param_trace_reset("trace-reset", this, trace_reset),
	stat_instruction_counter("instruction-counter", this, instruction_counter),
	stat_cpu_cycle("cpu-cycle", this, cpu_cycle),
	stat_bus_cycle("bus-cycle", this, bus_cycle),
	trace_insn(CONFIG::TRACE_INSN),
	trace_mask(CONFIG::TRACE_MASK),
	trace_reg(CONFIG::TRACE_REG),
	trace_reg_float(CONFIG::TRACE_REG_FLOAT),
	trace_loadstore(CONFIG::TRACE_LOADSTORE),
	trace_branch(CONFIG::TRACE_BRANCH),
	trace_sync(CONFIG::TRACE_SYNC),
	trace_reset(CONFIG::TRACE_RESET),
	trace_logger(*this),
	stats(0)
{
//	Object::SetupDependsOn(logger_import);

	Reset();
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
}

template <class CONFIG>
bool CPU<CONFIG>::Setup()
{
	return true;
}

#if 0
//=====================================================================
//=         memory access reporting control interface methods   START =
//=====================================================================

template<class CONFIG>
void 
CPU<CONFIG>::RequiresMemoryAccessReporting(bool report) {
	requires_memory_access_reporting = report;
}

template<class CONFIG>
void 
CPU<CONFIG>::RequiresFinishedInstructionReporting(bool report) {
	requires_finished_instruction_reporting = report;
}

//=====================================================================
//=         memory access reporting control interface methods   END   =
//=====================================================================

#endif


template <class CONFIG>
void CPU<CONFIG>::OnDisconnect()
{
}

template <class CONFIG>
void CPU<CONFIG>::Stop(int ret)
{
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
}

template <class CONFIG>
void CPU<CONFIG>::Reset()
{
	for(unsigned int i = 0; i != MAX_WARPS; ++i)
	{
		warps[i].id = i;
		warps[i].state = Warp<CONFIG>::Finished;
	}
	num_warps = 0;
	
	for(unsigned int i = 0; i != CONFIG::MAX_SAMPLERS; ++i)
	{
		samplers[i].Reset(this);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Reset(unsigned int threadsperblock, unsigned int numblocks, unsigned int gprs_per_warp, unsigned int sm_size)
{
	Reset();

	// Round up
	unsigned int warpsperblock = (threadsperblock + WARP_SIZE - 1) / WARP_SIZE;
	
	// TODO: move this to the driver/loader?
	assert(numblocks <= MAX_BLOCKS);
	assert(warpsperblock <= MAX_WARPS_PER_BLOCK);
	unsigned int total_warps = warpsperblock * numblocks;
	assert(total_warps <= MAX_WARPS);
	
	num_warps = total_warps;
	
	
	//int gprs_per_warp = MAX_VGPR / total_warps;	// TODO: round to power of 2?
												// and/or use count given by the compiler?
	assert(gprs_per_warp * total_warps <= MAX_VGPR);

	//int sm_size = SHARED_MEM_SIZE / numblocks;	// TODO: round to power of 2?
												// at least align on DWord boundary!
												// To be filled by driver.
	assert((sm_size & 0x3) == 0);
	assert(sm_size * numblocks <= SHARED_MEM_SIZE);

	address_t sm_base = CONFIG::SHARED_START + coreid * CONFIG::SHARED_SIZE;

	if(trace_reset) {
		cerr << dec;
		cerr << "Core " << coreid << ": reset: " << threadsperblock << " threads (" << warpsperblock << "warps) * " << numblocks << " blocks\n";
		cerr << " total " << total_warps << " warps.\n";
		cerr << " " << gprs_per_warp << " GPRs/warp, " << sm_size << "B SM/warp\n";
	}

	for(unsigned int b = 0; b != numblocks; ++b)
	{
		//address_t sm_block = sm_base + b * sm_size;
		for(unsigned int w = 0; w != warpsperblock; ++w)
		{
			// Compute mask for last (partial) warp
			bitset<WARP_SIZE> mask;
 			unsigned int nt = threadsperblock - (w * WARP_SIZE);
			if(nt < WARP_SIZE) {
				// Partial warp
				mask = (1 << nt) - 1;
			}
			else {
				mask.set();
			}
			unsigned int wid = b * warpsperblock + w;
			GetWarp(wid).Reset(wid, b, gprs_per_warp, sm_size, mask, sm_base, this);
		}
	}
	instruction_counter = 0;
}

template <class CONFIG>
void CPU<CONFIG>::InitStats(unsigned int code_size)
{
	typename CONFIG::address_t pc = CONFIG::CODE_START;
	while(pc < CONFIG::CODE_START + code_size)
	{
		insn_t insn;
		if(!memory_import->ReadMemory(pc, &insn, 8)) assert(false);

		Instruction<CONFIG> instruction(this, pc, insn);
		instruction.InitStats();

		if(instruction.IsLong()) {
			pc += 8;
		}
		else {
			pc += 4;
		}
	}
}

template <class CONFIG>
bool CPU<CONFIG>::Step()
{
	// Simple round-robin policy

	bool all_finished = true;
	for(unsigned int i = 0; i != num_warps; ++i) {
		if(GetWarp(i).state == Warp<CONFIG>::Active) {
			StepWarp(i);
		}
		if(GetWarp(i).state != Warp<CONFIG>::Finished) {
			all_finished = false;
		}
	}
	return all_finished;
}

template <class CONFIG>
void CPU<CONFIG>::StepWarp(uint32_t warpid)
{
	current_warpid = warpid;
	
	// Fetch
	address_t fetchaddr = GetPC();
	typename CONFIG::insn_t iw;
	if(!ReadMemory(fetchaddr, &iw, sizeof(typename CONFIG::insn_t))) {
		throw MemoryAccessException<CONFIG>();
	}

	// Decode
	Instruction<CONFIG> insn(this, fetchaddr, iw);
	insn.Stats()->Begin();

	if(trace_insn)
	{
		cerr << warpid << ": ";
		//address_t dummy;
		//cerr << Disasm(fetchaddr, dummy);
		cerr << std::hex;
		cerr.fill('0'); cerr.width(6); 
		cerr << fetchaddr - CONFIG::CODE_START << " ";
		cerr << std::dec;
		insn.Disasm(cerr);
		cerr << endl;
	}
	if(insn.IsLong()) {
		SetNPC(fetchaddr + 8);
	}
	else {
		SetNPC(fetchaddr + 4);
	}
	
	insn.Read();	// Read operands
	insn.Execute(); // Execute
	insn.Write();	// Writeback
	
	// Join or take other branch?
	CheckJoin();
	insn.Stats()->End();

	SetPC(GetNPC());
}

template <class CONFIG>
void CPU<CONFIG>::CheckJoin()
{
	CurrentWarp().flow.CheckJoin();
}

template <class CONFIG>
void CPU<CONFIG>::OnBusCycle()
{
	/* update the bus cycle counter */
	bus_cycle++;
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
	for(unsigned int wid = 0; wid != num_warps; ++wid) {
		VectorRegister<CONFIG> & r0 = GetGPR(wid, 0);
		r0.SetScalar(r0.CheckScalar());
		r0.SetScalar16(false, r0.CheckScalar16(false));
		r0.SetScalar16(true, r0.CheckScalar16(true));
		r0.SetStrided(r0.CheckStrided());
		r0.SetStrided16(false, r0.CheckStrided16(false));
		r0.SetStrided16(true, r0.CheckStrided16(true));
	}

	while(!Step() && instruction_counter < max_inst)
	{
	}
	
	if(trace_reset)
		cerr << "All warps finished\n";
	//DumpRegisters(0, cerr);
}

template <class CONFIG>
const char *CPU<CONFIG>::GetArchitectureName() const
{
	return "Tesla";
}

#if 0
template <class CONFIG>
void CPU<CONFIG>::BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, WIMG wimg, bool rwitm)
{
	memory_import->ReadMemory(physical_addr, buffer, size);
}

template <class CONFIG>
void CPU<CONFIG>::BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, WIMG wimg)
{
	memory_import->WriteMemory(physical_addr, buffer, size);
}

template <class CONFIG>
void CPU<CONFIG>::BusZeroBlock(physical_address_t physical_addr)
{
	uint8_t zero[32];
	memset(zero, 0, sizeof(zero));
	memory_import->WriteMemory(physical_addr & (~31), zero, sizeof(zero));
}

template <class CONFIG>
void CPU<CONFIG>::BusFlushBlock(physical_address_t physical_addr)
{
}


template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size, MemoryType mt, bool translate_addr)
{

	return true;
}
#endif

template <class CONFIG>
bool CPU<CONFIG>::ReadMemory(address_t addr, void *buffer, uint32_t size)
{
	// No addr translation for now
	return memory_import->ReadMemory(addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	return memory_import->WriteMemory(addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::InjectReadMemory(address_t addr, void *buffer, uint32_t size)
{
	return ReadMemory(addr, buffer, size);
}

template <class CONFIG>
bool CPU<CONFIG>::InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	return WriteMemory(addr, buffer, size);
}

template <class CONFIG>
string CPU<CONFIG>::Disasm(address_t addr, address_t& next_addr)
{
	stringstream sstr;
	insn_t insn;

	physical_address_t physical_addr;

	physical_addr = addr;

	if(!memory_import->ReadMemory(physical_addr, &insn, 8)) return string("unreadable ?");

	Instruction<CONFIG> instruction(this, physical_addr, insn);

//	operation = Decoder<CONFIG>::Decode(addr, insn);

	sstr << std::hex;
	sstr.fill('0'); sstr.width(8); 
	sstr << addr - CONFIG::CODE_START << " ";
	
	if(instruction.IsLong()) {
		sstr << "0x" << std::hex;// << std::setw(16);
		sstr.fill('0'); sstr.width(16); 
		sstr << insn << std::dec << std::noshowbase << " ";	// TODO: endianness...
	}
	else {
		sstr << "        ";
		sstr << "0x" << std::hex;// << std::setw(16);
		sstr.fill('0'); sstr.width(8); 
		sstr << uint32_t(insn) << std::dec << std::noshowbase << " ";	// TODO: endianness...
	}

	// disassemble the instruction
	instruction.Disasm(sstr);

	if(instruction.IsLong()) {
		next_addr = addr + 8;
	}
	else {
		next_addr = addr + 4;
	}
	return sstr.str();
}


template <class CONFIG>
void CPU<CONFIG>::DumpRegisters(int warpid, ostream & os) const
{
	Warp<CONFIG> const & warp = GetWarp(warpid);
	os << "Warp " << warpid << endl;
	for(unsigned int i = 0; i != warp.gpr_window_size; ++i)
	{
		DumpGPR(warpid, i, os);
	}
	for(unsigned int i = 0; i != MAX_PRED_REGS; ++i)
	{
		DumpFlags(warpid, i, os);
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpGPR(int warpid, int reg, ostream & os) const
{
	os << " r" << reg << " = " << GetGPR(warpid, reg) << endl;
	if(trace_reg_float) {
		os << " r" << reg << " = ";
		GetGPR(warpid, reg).DumpFloat(os);
		os << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpGPR(int reg, ostream & os) const
{
	DumpGPR(current_warpid, reg, os);
}

template <class CONFIG>
void CPU<CONFIG>::DumpFlags(int warpid, int reg, ostream & os) const
{
	Warp<CONFIG> const & warp = GetWarp(warpid);
	os << " p" << reg << " = " << warp.pred_flags[reg] << endl;
}

template <class CONFIG>
void CPU<CONFIG>::DumpFlags(int reg, ostream & os) const
{
	DumpFlags(current_warpid, reg, os);
}

template <class CONFIG>
void CPU<CONFIG>::DumpAddr(int reg, ostream & os) const
{
	os << " a" << reg << " = " << GetAddr(reg) << endl;
}


#if 0
/* PowerPC Linux OS Interface */
template <class CONFIG>
void CPU<CONFIG>::PerformExit(int ret)
{
	if(logger_import)
		(*logger_import) << DebugInfo << "Program exited with code " << ret << Endl << EndDebugInfo;
	Stop(ret);
}

/* Endian interface */
template <class CONFIG>
endian_type CPU<CONFIG>::GetEndianess()
{
	return E_LITTLE_ENDIAN;
}

#endif

template <class CONFIG>
void CPU<CONFIG>::Fetch(typename CONFIG::insn_t & insn, typename CONFIG::address_t addr)
{
	if(!ReadMemory(addr, &insn, sizeof(typename CONFIG::insn_t))) {
		throw MemoryAccessException<CONFIG>();
	}
}


//=====================================================================
//=                 Execution helper functions                        =
//=====================================================================


template <class CONFIG>
VectorRegister<CONFIG> & CPU<CONFIG>::GetGPR(unsigned int reg)	// for current warp
{
	// If special reg return dummy
	if(reg == 124) {
		// TODO: prevent code such as mov r124, 42...
		return zero_reg;
	}
	// Compute physical register ID
	return gpr[CurrentWarp().GetGPRAddress(reg)];
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::GetGPR(unsigned int reg) const	// for current warp
{
	// If special reg (zero...), return value
	if(reg == 124) {
		return zero_reg;
	}
	// Compute physical register ID
	return gpr[CurrentWarp().GetGPRAddress(reg)];
}

template <class CONFIG>
VectorFlags<CONFIG> & CPU<CONFIG>::GetFlags(unsigned int reg)	// for current warp
{
	assert(reg < MAX_PRED_REGS);
	return CurrentWarp().pred_flags[reg];
}

template <class CONFIG>
VectorFlags<CONFIG> CPU<CONFIG>::GetFlags(unsigned int reg) const
{
	assert(reg < MAX_PRED_REGS);
	return CurrentWarp().pred_flags[reg];
}

template <class CONFIG>
VectorAddress<CONFIG> & CPU<CONFIG>::GetAddr(unsigned int reg)
{
	// base 1
	assert(reg > 0 && reg <= MAX_ADDR_REGS);
	return CurrentWarp().addr[reg - 1];
}

template <class CONFIG>
VectorAddress<CONFIG> CPU<CONFIG>::GetAddr(unsigned int reg) const
{
	assert(reg <= MAX_ADDR_REGS);
	return CurrentWarp().addr[reg - 1];
}

#if 0
template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> & CPU<CONFIG>::GetCurrentMask()
{
	return CurrentWarp().flow.GetCurrentMask();
}
#endif

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> CPU<CONFIG>::GetCurrentMask() const
{
	return CurrentWarp().flow.GetCurrentMask();
}

template<class CONFIG>
void CPU<CONFIG>::Branch(address_t target_addr, std::bitset<CONFIG::WARP_SIZE> mask)
{
	CurrentWarp().flow.Branch(target_addr, mask);
}

template <class CONFIG>
VectorRegister<CONFIG> & CPU<CONFIG>::GetGPR(unsigned int wid, unsigned int reg)
{
	// If special reg return dummy??
	// Compute physical register ID
	return gpr[GetWarp(wid).GetGPRAddress(reg)];
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::GetGPR(unsigned int wid, unsigned int reg) const
{
	// If special reg (zero...), return value
	// Compute physical register ID
	return gpr[GetWarp(wid).GetGPRAddress(reg)];
}


template <class CONFIG>
Warp<CONFIG> & CPU<CONFIG>::CurrentWarp()
{
	return warps[current_warpid];
}

template <class CONFIG>
Warp<CONFIG> const & CPU<CONFIG>::CurrentWarp() const
{
	return warps[current_warpid];
}

template <class CONFIG>
Warp<CONFIG> & CPU<CONFIG>::GetWarp(unsigned int wid)
{
	assert(wid < MAX_WARPS);
	return warps[wid];
}

template <class CONFIG>
Warp<CONFIG> const & CPU<CONFIG>::GetWarp(unsigned int wid) const
{
	assert(wid < MAX_WARPS);
	return warps[wid];
}

template <class CONFIG>
Sampler<CONFIG> & CPU<CONFIG>::GetSampler(unsigned int s)
{
	assert(s < CONFIG::MAX_SAMPLERS);
	return samplers[s];
}

template <class CONFIG>
void CPU<CONFIG>::Meet(CPU<CONFIG>::address_t addr)
{
	CurrentWarp().flow.Meet(addr);
}

template <class CONFIG>
void CPU<CONFIG>::PreBreak(CPU<CONFIG>::address_t addr)
{
	CurrentWarp().flow.PreBreak(addr);
}

template <class CONFIG>
bool CPU<CONFIG>::Join()
{
	return CurrentWarp().flow.Join();
}

template <class CONFIG>
void CPU<CONFIG>::End()
{
	// Make sure stacks are now empty
	//assert(CurrentWarp().mask_stack.empty());
	//assert(CurrentWarp().join_stack.empty());
	//assert(CurrentWarp().loop_stack.empty());
	CurrentWarp().state = Warp<CONFIG>::Finished;
}

template <class CONFIG>
void CPU<CONFIG>::Fence()
{
	CurrentWarp().state = Warp<CONFIG>::WaitingFence;
	
	CheckFenceCompleted();
}

template <class CONFIG>
void CPU<CONFIG>::CheckFenceCompleted()
{
	// When all warps in a block are in WaitingFence state
	bool synchronized[MAX_BLOCKS];
	std::fill(synchronized, synchronized + MAX_BLOCKS, true);
	for(unsigned int i = 0; i != num_warps; ++i)
	{
		if(trace_sync) {
			cerr << "  Warp " << i << ", block " << warps[i].blockid << endl;
		}
		if(warps[i].state != Warp<CONFIG>::WaitingFence) {
			synchronized[warps[i].blockid] = false;
			if(trace_sync) {
				cerr << "   Not synchronized\n";
			}
		}
		else if(trace_sync) {
			cerr << "   Synchronized\n";
		}
	}
	
	// Turn them to active state
	for(unsigned int i = 0; i != num_warps; ++i)
	{
		if(synchronized[warps[i].blockid]) {
			warps[i].state = Warp<CONFIG>::Active;
			if(trace_sync) {
				cerr << "  Warp " << i << ", block " << warps[i].blockid << endl;
				cerr << "   Activated\n";
			}
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Kill(std::bitset<CONFIG::WARP_SIZE> mask)
{
	CurrentWarp().flow.Kill(mask);
}

template <class CONFIG>
void CPU<CONFIG>::Return(std::bitset<CONFIG::WARP_SIZE> mask)
{
	CurrentWarp().flow.Return(mask);
}

template <class CONFIG>
void CPU<CONFIG>::Break(std::bitset<CONFIG::WARP_SIZE> mask)
{
	CurrentWarp().flow.Break(mask);
}

template <class CONFIG>
void CPU<CONFIG>::Call(address_t target)
{
	CurrentWarp().flow.Call(target);
}

template <class CONFIG>
void CPU<CONFIG>::ReadShared(address_t addr, void *buffer, uint32_t size)
{
	assert(addr < CONFIG::SHARED_SIZE && addr + size < CONFIG::SHARED_SIZE);
	address_t effective_addr = CONFIG::SHARED_START + coreid * CONFIG::SHARED_SIZE + addr;
	if(!ReadMemory(effective_addr, buffer, size)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::WriteShared(address_t addr, const void *buffer, uint32_t size)
{
	assert(addr < CONFIG::SHARED_SIZE && addr + size < CONFIG::SHARED_SIZE);
	address_t effective_addr = CONFIG::SHARED_START + coreid * CONFIG::SHARED_SIZE + addr;
	if(!WriteMemory(effective_addr, buffer, size)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::SetSampler(Sampler<CONFIG> const & sampler, unsigned int n)
{
	samplers[n] = sampler;
}

template <class CONFIG>
typename CONFIG::operationstats_t & CPU<CONFIG>::GetOpStats()
{
	// TODO: cache this...
	return (*stats)[GetPC() - CONFIG::CODE_START];
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
