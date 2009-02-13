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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_TCC__

#include <unisim/component/cxx/processor/tesla/exception.tcc>
#include <unisim/component/cxx/processor/tesla/register.tcc>
#include <unisim/component/cxx/processor/tesla/instruction.tcc>
#include <unisim/component/cxx/processor/tesla/flags.tcc>
#include <unisim/component/cxx/processor/tesla/exec.tcc>
#include <unisim/component/cxx/processor/tesla/simfloat.tcc>
#include <unisim/component/cxx/processor/tesla/memory.tcc>

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
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;

#define VERBOSE

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
//	Service<CPULinuxOS>(name, parent),
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
//	linux_os_import("linux-os-import", this),
//	trap_reporting_import("trap-reporting-import", this),
//	logger_import("logger-import", this),
//	synchronizable_export("synchronizable-export", this),
//	cpu_cycle_time(0),
//	voltage(0),
//	bus_cycle_time(0),
	coreid(coreid),
	instruction_counter(0),
	max_inst(0xffffffffffffffffULL),
//	num_insn_in_prefetch_buffer(0),
//	cur_insn_in_prefetch_buffer(0),
//	verbose_all(false),
//	trap_on_instruction_counter(0xffffffffffffffffULL),
//	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time),
//	param_voltage("voltage", this, voltage),
//	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time),
	param_max_inst("max-inst", this, max_inst),
//	param_verbose_all("verbose-all", this, verbose_all),
//	param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter),
	stat_instruction_counter("instruction-counter", this, instruction_counter),
	stat_bus_cycle("bus-cycle", this, bus_cycle),
	stat_cpu_cycle("cpu-cycle", this, cpu_cycle)
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
	for(int i = 0; i != MAX_WARPS; ++i)
	{
		warps[i].id = i;
		warps[i].state = Warp::Finished;
	}
	num_warps = 0;
//	effective_address = 0;
}

template <class CONFIG>
void CPU<CONFIG>::Reset(int threadsperblock, int numblocks)
{
	Reset();

	// Round up
	int warpsperblock = (threadsperblock + WARP_SIZE - 1) / WARP_SIZE;
	
	// TODO: move this to the driver/loader?
	assert(numblocks <= MAX_BLOCKS);
	assert(warpsperblock <= MAX_WARPS_PER_BLOCK);
	int total_warps = warpsperblock * numblocks;
	assert(total_warps <= MAX_WARPS);
	assert(total_warps > 0);
	
	num_warps = total_warps;
	
	
	int gprs_per_warp = MAX_VGPR / total_warps;	// TODO: round to power of 2?
												// and/or use count given by the compiler?

	int sm_size = SHARED_MEM_SIZE / numblocks;	// TODO: round to power of 2?
												// at least align on DWord boundary!

	address_t sm_base = CONFIG::SHARED_START + coreid * CONFIG::SHARED_SIZE;

	cerr << dec;
	cerr << "Core " << coreid << ": reset: " << threadsperblock << " threads (" << warpsperblock << "warps) * " << numblocks << " blocks\n";
	cerr << " total " << total_warps << " warps.\n";
	cerr << " " << gprs_per_warp << " GPRs/warp, " << sm_size << "B SM/warp\n";

	for(int b = 0; b != numblocks; ++b)
	{
		for(int w = 0; w != warpsperblock; ++w)
		{
			// Compute mask for last (partial) warp
			bitset<WARP_SIZE> mask;
 			int nt = threadsperblock - (w * WARP_SIZE);
			if(nt < WARP_SIZE) {
				// Partial warp
				mask = (1 << nt) - 1;
			}
			else {
				mask.set();
			}
			int wid = b * warpsperblock + w;
			GetWarp(wid).Reset(wid, b, gprs_per_warp, sm_size, mask, sm_base);
		}
	}
	instruction_counter = 0;
}

template <class CONFIG>
bool CPU<CONFIG>::Step()
{
	// Simple round-robin policy

	bool all_finished = true;
	for(int i = 0; i != num_warps; ++i) {
		if(GetWarp(i).state != Warp::Finished) {
			StepWarp(i);
			all_finished = false;
		}
	}
	return all_finished;
}

template <class CONFIG>
void CPU<CONFIG>::StepWarp(uint32_t warpid)
{
	current_warpid = warpid;
	
	address_t fetchaddr = GetPC();
	typename CONFIG::insn_t iw;
	if(!ReadMemory(fetchaddr, &iw, sizeof(typename CONFIG::insn_t))) {
		throw MemoryAccessException<CONFIG>();
	}
	Instruction<CONFIG> insn(this, fetchaddr, iw);
#ifdef VERBOSE
	insn.Disasm(cerr);
	cerr << endl;
#endif
	if(insn.IsLong()) {
		SetNPC(fetchaddr + 8);
	}
	else {
		SetNPC(fetchaddr + 4);
	}
	
	insn.Execute();
	// Join or take other branch?
	CheckJoin();
	
	SetPC(GetNPC());
}

template <class CONFIG>
void CPU<CONFIG>::CheckJoin()
{
	if(InConditional())
	{
		if(GetNPC() == GetJoin())
		{
			// Joined
			// Restore last mask and join
			GetCurrentMask() = PopMask();
			PopJoin();
		}
		else if(GetNPC() > GetJoin())
		{
			// Jumped over join point
			// Go back following the other branch
			std::swap(GetJoin(), GetNPC());
			// Invert condition
			GetCurrentMask() = ~GetCurrentMask() & GetNextMask();
		}
	}
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
	while(!Step() && instruction_counter < max_inst)
	{
	}
	
	cerr << "All warps finished\n";
	DumpRegisters(0, cerr);
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
void CPU<CONFIG>::DumpRegisters(int warpid, ostream & os)
{
	Warp & warp = GetWarp(warpid);
	os << "Warp " << warpid << endl;
	for(int i = 0; i != warp.gpr_window_size; ++i)
	{
		os << " r" << i << " = " << GetGPR(warpid, i) << endl;
	}
	for(int i = 0; i != MAX_PRED_REGS; ++i)
	{
		os << " p" << i << " = " << warp.pred_flags[i] << endl;
	}
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
#if 0

template <class CONFIG>
void CPU<CONFIG>::Fetch()
{
	// Stall if IQ is full
	if(iq.Full()) return;
	// Send a request to IMMU
	uint32_t size_to_block_boundary = CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (cia & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	uint32_t fetch_width = Max(iq.Size(), FETCH_WIDTH);
	uint32_t fetch_size = Max(fetch_width * sizeof(uint32_t), size_to_block_boundary);
	uint8_t prefetch_buffer[fetch_size];
	uint32_t read_size = 0; //ReadInsnMemory(cia, prefetch_buffer, sizeof(prefetch_buffer) > size_to_block_boundary ? size_to_block_boundary : sizeof(prefetch_buffer));
	uint32_t offset;
	for(offset = 0; offset < read_size; offset += sizeof(uint32_t), cia += sizeof(uint32_t))
	{
		uint32_t insn = prefetch_buffer[offset];
		Operation<CONFIG> *operation = Decoder<CONFIG>::Decode(cia, insn);
		Instruction<CONFIG> *instruction = iw.Allocate();
		instruction->SetOperation(operation);
		iq.Push(instruction);
	}
}
#endif


//=====================================================================
//=                 Execution helper functions                        =
//=====================================================================


template <class CONFIG>
VectorRegister<CONFIG> & CPU<CONFIG>::GetGPR(int reg)	// for current warp
{
	// If special reg return dummy??
	// Compute physical register ID
	return gpr[CurrentWarp().GetGPRAddress(reg)];
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::GetGPR(int reg) const	// for current warp
{
	// If special reg (zero...), return value
	// Compute physical register ID
	return gpr[CurrentWarp().GetGPRAddress(reg)];
}

template <class CONFIG>
VectorFlags<CONFIG> & CPU<CONFIG>::GetFlags(int reg)	// for current warp
{
	assert(reg >= 0 && reg < MAX_PRED_REGS);
	return CurrentWarp().pred_flags[reg];
}

template <class CONFIG>
VectorFlags<CONFIG> CPU<CONFIG>::GetFlags(int reg) const
{
	assert(reg >= 0 && reg < MAX_PRED_REGS);
	return CurrentWarp().pred_flags[reg];
}

template <class CONFIG>
VectorAddress<CONFIG> & CPU<CONFIG>::GetAddr(int reg)
{
	assert(reg >= 0 && reg < MAX_ADDR_REGS);
	return CurrentWarp().addr[reg];
}

template <class CONFIG>
VectorAddress<CONFIG> CPU<CONFIG>::GetAddr(int reg) const
{
	assert(reg >= 0 && reg < MAX_ADDR_REGS);
	return CurrentWarp().addr[reg];
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> & CPU<CONFIG>::GetCurrentMask()
{
	return CurrentWarp().mask;
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> CPU<CONFIG>::GetCurrentMask() const
{
	return CurrentWarp().mask;
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> CPU<CONFIG>::PopMask()
{
	std::bitset<CONFIG::WARP_SIZE> msk = CurrentWarp().mask_stack.top();
	CurrentWarp().mask_stack.pop();
	return msk;
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> CPU<CONFIG>::GetNextMask()
{
	return CurrentWarp().mask_stack.top();
}

template <class CONFIG>
void CPU<CONFIG>::PushMask(std::bitset<CONFIG::WARP_SIZE> mask)
{
	CurrentWarp().mask_stack.push(mask);
}

template <class CONFIG>
typename CONFIG::address_t CPU<CONFIG>::PopJoin()
{
	address_t addr = CurrentWarp().join_stack.top();
	CurrentWarp().join_stack.pop();
	return addr;
}

template <class CONFIG>
void CPU<CONFIG>::PushJoin(typename CONFIG::address_t addr)
{
	// TODO: raise exn if ovf
	assert(CurrentWarp().join_stack.size() < BRANCH_STACK_DEPTH);
	CurrentWarp().join_stack.push(addr);
}

template <class CONFIG>
typename CONFIG::address_t CPU<CONFIG>::GetJoin() const
{
	return CurrentWarp().join_stack.top();
}

template <class CONFIG>
typename CONFIG::address_t & CPU<CONFIG>::GetJoin()
{
	return CurrentWarp().join_stack.top();
}

template <class CONFIG>
bool CPU<CONFIG>::InConditional() const
{
	return !CurrentWarp().join_stack.empty();
}

template <class CONFIG>
typename CONFIG::address_t CPU<CONFIG>::GetLoop() const
{
	if(CurrentWarp().loop_stack.empty()) {
		return 0;
	}
	return CurrentWarp().loop_stack.top();
}

template <class CONFIG>
void CPU<CONFIG>::PushLoop(address_t addr)
{
	CurrentWarp().loop_stack.push(addr);
}

template <class CONFIG>
void CPU<CONFIG>::PopLoop()
{
	assert(!CurrentWarp().loop_stack.empty());
	CurrentWarp().loop_stack.pop();
}

template <class CONFIG>
VectorRegister<CONFIG> & CPU<CONFIG>::GetGPR(int wid, int reg)
{
	// If special reg return dummy??
	// Compute physical register ID
	return gpr[GetWarp(wid).GetGPRAddress(reg)];
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::GetGPR(int wid, int reg) const
{
	// If special reg (zero...), return value
	// Compute physical register ID
	return gpr[GetWarp(wid).GetGPRAddress(reg)];
}


template <class CONFIG>
typename CPU<CONFIG>::Warp & CPU<CONFIG>::CurrentWarp()
{
	return warps[current_warpid];
}

template <class CONFIG>
typename CPU<CONFIG>::Warp const & CPU<CONFIG>::CurrentWarp() const
{
	return warps[current_warpid];
}

template <class CONFIG>
typename CPU<CONFIG>::Warp & CPU<CONFIG>::GetWarp(int wid)
{
	assert(wid >= 0 && wid < MAX_WARPS);
	return warps[wid];
}

template <class CONFIG>
typename CPU<CONFIG>::Warp const & CPU<CONFIG>::GetWarp(int wid) const
{
	assert(wid >= 0 && wid < MAX_WARPS);
	return warps[wid];
}

template <class CONFIG>
void CPU<CONFIG>::Join()
{
}

template <class CONFIG>
void CPU<CONFIG>::End()
{
	CurrentWarp().state = Warp::Finished;
}

//////////////////////////////////////////////////////////////////////

template <class CONFIG>
void CPU<CONFIG>::Warp::Reset(int wid, int bid, int gpr_num, int sm_size,
	bitset<WARP_SIZE> init_mask, address_t sm_base)
{
	pc = CONFIG::CODE_START;
	npc = 0;
	
	gpr_window_size = gpr_num;
	gpr_window_base = gpr_num * wid;
	
	sm_window_size = sm_size;
	sm_window_base = sm_base + sm_size * bid;
	
	mask = init_mask;
	
	for(int i = 0; i != MAX_PRED_REGS; ++i) {
		pred_flags[i].Reset();
	}
	
	for(int i = 0; i != MAX_ADDR_REGS; ++i) {
		addr[i].Reset();
	}
	
	state = Active;
	cerr << " Warp " << id << " (" << bid << ", " << wid << "): reset\n";
	cerr << "  " << gpr_window_size << " GPRs from " << gpr_window_base << "\n";
}

template <class CONFIG>
uint32_t CPU<CONFIG>::Warp::GetGPRAddress(uint32_t reg) const
{
	// 32-bit access
	// No special register here
	if(reg >= gpr_window_size) {
		std::cerr << "Warp " << id << ": accessing r" << reg << " on a " << gpr_window_size << " reg window\n";
	}
	assert(reg < gpr_window_size);
	return gpr_window_base + reg;
}

template <class CONFIG>
typename CPU<CONFIG>::address_t CPU<CONFIG>::Warp::GetSMAddress(uint32_t sm) const
{
	assert(sm < sm_window_size);
	return sm_window_base + sm;
}



//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
