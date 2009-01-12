/*
 *  Copyright (c) 2008,
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_TCC__

//#include <unisim/component/cxx/processor/tesla/exception.tcc>
#include <unisim/component/cxx/processor/tesla/register.tcc>
#include <unisim/component/cxx/processor/tesla/instruction.tcc>
//#include <unisim/component/cxx/cache/cache.tcc>
//#include <unisim/component/cxx/tlb/tlb.tcc>
//#include <unisim/util/queue/queue.tcc>

#include <sstream>
#include <stdexcept>
#include <cassert>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;



template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent) :
	Object(name, parent),
	Client<Loader<typename CONFIG::physical_address_t> >(name, parent),
	Client<SymbolTableLookup<typename CONFIG::address_t> >(name, parent),
	Client<DebugControl<typename CONFIG::address_t> >(name, parent),
//	Client<MemoryAccessReporting<typename CONFIG::address_t> >(name, parent),
//	Client<TrapReporting>(name, parent),
//	Service<MemoryAccessReportingControl>(name, parent),
	Service<Disassembly<typename CONFIG::address_t> >(name, parent),
	Service<unisim::service::interfaces::Registers>(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Service<MemoryInjection<typename CONFIG::address_t> >(name, parent),
//	Service<CPULinuxOS>(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
//	Client<LinuxOS>(name, parent),
	Client<Logger>(name, parent),
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
	logger_import("logger-import", this),
//	synchronizable_export("synchronizable-export", this),
	cpu_cycle_time(0),
	voltage(0),
	bus_cycle_time(0),
	max_inst(0xffffffffffffffffULL),
//	num_insn_in_prefetch_buffer(0),
//	cur_insn_in_prefetch_buffer(0),
//	verbose_all(false),
//	trap_on_instruction_counter(0xffffffffffffffffULL),
	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time),
	param_voltage("voltage", this, voltage),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time),
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
void CPU<CONFIG>::Reset()
{
	effective_address = 0;
}

template <class CONFIG>
void CPU<CONFIG>::Step()
{
#if 0
	address_t sequential_nia = GetCIA() + 4;
	Operation<CONFIG> *operation = 0;

	SetNIA(sequential_nia);

	try
	{
		address_t addr = GetCIA();
		uint32_t insn;
		
		EmuFetch(addr, &insn, 4);

		operation = Decoder<CONFIG>::Decode(addr, insn);

		if(IsVerboseStep())
		{
			stringstream sstr;
			operation->disasm(this, sstr);
			(*logger_import) << DebugInfo << "#" << instruction_counter << ":0x" << Hex << addr << Dec << ":" << sstr.str() << Endl << EndDebugInfo;
		}

		/* execute the instruction */
		operation->execute(this);

	}
	catch(Exception& e)
	{
		if(logger_import)
			(*logger_import) << DebugError << "uncaught processor exception :" << e.what() << Endl << EndDebugError;
		Stop(1);
	}

	SetCIA(GetNIA());

	/* update the instruction counter */
	instruction_counter++;
	if(trap_reporting_import && instruction_counter == trap_on_instruction_counter)
	{
		trap_reporting_import->ReportTrap();
	}
	
	if(requires_finished_instruction_reporting)
	{
		if(memory_access_reporting_import)
		{
			memory_access_reporting_import->ReportFinishedInstruction(GetNIA());
		}
	}

	if(instruction_counter >= max_inst) Stop(0);
#endif
}

template <class CONFIG>
void CPU<CONFIG>::StepWarp(uint32_t warpid)
{
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
	do
	{
		Step();
		if(instruction_counter >= max_inst) Stop(0);
	} while(1);
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
//	return ReadMemory(addr, buffer, size, MT_DATA, true);
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size)
{
//	return WriteMemory(addr, buffer, size, MT_DATA, true);
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::InjectReadMemory(address_t addr, void *buffer, uint32_t size)
{
#if 0
	if(size > 0)
	{
		uint32_t sz;
		uint8_t *dst = (uint8_t *) buffer;
		do
		{
			uint32_t size_to_fsb_boundary = FSB_WIDTH - (addr & (FSB_WIDTH - 1));
			sz = size > size_to_fsb_boundary ? size_to_fsb_boundary : size;
			EmuLoad<true>(addr, dst, sz);
			dst += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
#endif
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
{
#if 0
	if(size > 0)
	{
		uint32_t sz;
		const uint8_t *src = (const uint8_t *) buffer;
		do
		{
			uint32_t size_to_fsb_boundary = FSB_WIDTH - (addr & (FSB_WIDTH - 1));
			sz = size > size_to_fsb_boundary ? size_to_fsb_boundary : size;
			EmuStore<true>(addr, src, sz);
			src += sz;
			addr += sz;
			size -= sz;
		} while(size > 0);
	}
#endif
	return true;
}

template <class CONFIG>
string CPU<CONFIG>::Disasm(address_t addr, address_t& next_addr)
{
	stringstream sstr;
	insn_t insn;

	physical_address_t physical_addr;

	physical_addr = addr;

	if(!memory_import->ReadMemory(physical_addr, &insn, 4)) return string("unreadable ?");

	Instruction<CONFIG> instruction(this, physical_addr, insn);

//	operation = Decoder<CONFIG>::Decode(addr, insn);

	// disassemble the instruction
	sstr << "0x" << std::hex;
	sstr.fill('0'); sstr.width(8); 
	sstr << insn << std::dec << " ";	// TODO: endianness...
	instruction.Disasm(sstr);

	next_addr = addr + 8;
	return sstr.str();
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

inline uint32_t Max(uint32_t a, uint32_t b)
{
	return a > b ? a : b;
}

inline uint32_t Min(uint32_t a, uint32_t b)
{
	return a < b ? a : b;
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
void CPU<CONFIG>::ExecMarker(uint32_t marker)
{
	// 0 = normal
	// 1 = end
	// 2 = join
	// 3 shouldn't happen here
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadOperandFP32(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg)
{
	VectorRegister<CONFIG> dest = ReadOperand(reg, cm, sh);
	if(neg)
		dest.NegateFP32();
	return dest;
}


template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadOperandFP32(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm)
{
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadOperand(uint32_t reg, uint32_t cm, uint32_t sh)
{
	assert(!(cm && sh));
	if(cm)
		return ReadConstant(reg);
	else if(sh)
		return ReadShared(reg);
	else
		return GetGPR(reg);
}

#if 0
template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadOperand(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm)
{
}
#endif

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadImmediate(uint32_t imm_hi, uint32_t imm_lo)
{
	uint32_t imm_val = (imm_hi << 7) | imm_lo;
	return VectorRegister<CONFIG>(imm_val);
}

template <class CONFIG>
void CPU<CONFIG>::WriteOutput(VectorRegister<CONFIG> const & v, uint32_t reg, uint32_t pred_cond, uint32_t pred_reg)
{
	assert(pred_reg < MAX_PRED_REGS);
	Warp & warp = CurrentWarp();
	// Masked write with pred AND current mask (top of stack)
	GetGPR(reg).Write(v, IsPredSet(pred_cond, warp.pred_flags[pred_reg]) & warp.mask);
}

template <class CONFIG>
void CPU<CONFIG>::WritePred(uint32_t set_pred_reg, uint32_t pred_cond, uint32_t pred_reg, VectorFlags<CONFIG> flags)
{
	assert(pred_reg < MAX_PRED_REGS);
	Warp & warp = CurrentWarp();
	// Masked write with pred AND current mask
	bitset<WARP_SIZE> mask = IsPredSet(pred_cond, warp.pred_flags[pred_reg]) & warp.mask;
	CurrentWarp().pred_flags[pred_reg].Write(flags, mask);
}

template <class CONFIG>
VectorRegister<CONFIG> & CPU<CONFIG>::GetGPR(int reg)	// for current warp
{
	// If special reg return dummy??
	// Compute physical register ID
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::GetGPR(int reg) const	// for current warp
{
	// If special reg (zero...), return value
	// Compute physical register ID
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
VectorRegister<CONFIG> CPU<CONFIG>::ReadConstant(VectorRegister<CONFIG> const & addr, uint32_t seg) const
{
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadConstant(int addr, uint32_t seg) const
{
}


// For current block
template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadShared(VectorRegister<CONFIG> const & addr) const
{
}


// For current block
template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadShared(int addr) const
{
	// bits 6-5: size, sign
}

//template <class CONFIG>
//int CPU<CONFIG>::GetCurrentWarpID() const
//{
//	return current_warpid;
//}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::EffectiveAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, uint32_t pred_cond, uint32_t pred_reg)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	// [seg][$a#addr_reg + addr_imm]
}

template <class CONFIG>
uint32_t CPU<CONFIG>::Warp::GetGPRAddress(uint32_t reg)
{
	// 32-bit access
	// No special register here
	assert(reg < gpr_window_size);
	return gpr_window_base + reg;
}

template <class CONFIG>
uint32_t CPU<CONFIG>::Warp::GetSMAddress(uint32_t sm)
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
