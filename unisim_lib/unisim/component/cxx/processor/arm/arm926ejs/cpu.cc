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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
#include "unisim/component/cxx/processor/arm/arm926ejs/cpu.hh"
#include "unisim/component/cxx/processor/arm/arm926ejs/cp15.hh"
#include "unisim/component/cxx/processor/arm/arm926ejs/cache.hh"
#include "unisim/component/cxx/processor/arm/arm926ejs/tlb.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include <sstream>
#include <string>
#include <string.h>

#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/masks.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/kernel/logger/logger.hh"

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif // __STDC_CONSTANT_MACROS
#include <assert.h>
#include <stdint.h>

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Memory;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::LittleEndian2Host;
using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Register;
using std::stringstream;
using std::string;

/** Constructor.
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
CPU::
CPU(const char *name, Object *parent)
	: Object(name, parent)
	, unisim::component::cxx::processor::arm::CPU()
	, unisim::component::cxx::processor::arm::arm926ejs::CP15Interface()
	, Service<MemoryInjection<uint64_t> >(name, parent)
	, Client<DebugControl<uint64_t> >(name, parent)
	, Client<MemoryAccessReporting<uint64_t> >(name, parent)
	, Client<TrapReporting>(name, parent)
	, Service<MemoryAccessReportingControl>(name, parent)
	, Service<Disassembly<uint64_t> >(name, parent)
	, Service<Registers>(name, parent)
	, Service<Memory<uint64_t> >(name, parent)
	, disasm_export("disasm-export", this)
	, registers_export("registers-export", this)
	, memory_injection_export("memory-injection-export", this)
	, memory_export("memory-export", this)
	, memory_access_reporting_control_export(
			"memory-access-reporting-control-export", this)
	, debug_control_import("debug-control-import", this)
	, memory_access_reporting_import("memory-access-reporting-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, instruction_counter_trap_reporting_import(
			"instruction-counter-trap-reporting-import", this)
	, exception_trap_reporting_import(
			"exception-trap-reporting-import", this)
	, logger(*this)
	, icache("icache", this)
	, dcache("dcache", this)
	, ltlb("lockdown-tlb", this)
	, tlb("tlb", this)
	, arm32_decoder()
	, thumb_decoder()
	, exception(0)
	, cp15(this, "cp15", this)
	, instruction_counter(0)
	, cur_instruction_address(0)
	, verbose(0)
	, trap_on_instruction_counter(0)
	, trap_on_exception(false)
	, requires_memory_access_reporting(true)
	, requires_finished_instruction_reporting(true)
	, param_cpu_cycle_time("cpu-cycle-time", this,
			cpu_cycle_time,
			"The processor cycle time in picoseconds.")
	, param_voltage("voltage", this,
			voltage,
			"The processor voltage in mV.")
	, param_verbose("verbose", this,
			verbose,
			"Activate the verbose system (0 = inactive, different than 0 = "
			"active).")
	, param_trap_on_instruction_counter("trap-on-instruction-counter", this,
			trap_on_instruction_counter,
			"Produce a trap when the given instruction count is reached.")
	, param_trap_on_exception("trap-on-exception", this,
			trap_on_exception,
			"Produce a trap when an exception occurs.")
	, stat_instruction_counter("instruction-counter", this,
			instruction_counter,
			"Number of instructions executed.")
	, stat_cur_instruction_address("cur-instruction-address", this,
			cur_instruction_address,
			"Address of the instruction currently being executed.")
	, reg_sp("SP", this, gpr[13],
			"The stack pointer (SP) register (alias of GPR[13]).")
	, reg_lr("LR", this, gpr[14],
			"The link register (LR) (alias of GPR[14]).")
	, reg_pc("PC", this, gpr[15],
			"The program counter (PC) register (alias of GPR[15]).")
	, reg_cpsr("CPSR", this, cpsr,
			"The CPSR register.")
	, ls_queue()
	, first_ls(0)
	, has_sent_first_ls(false)
	, free_ls_queue()
{
	for (unsigned int i = 0; i < num_phys_gprs; i++)
	{
		stringstream ss;
		stringstream ss_desc;
		ss << "PHYS_GPR[" << i << "]";
		ss_desc << "Physical register " << i;
		reg_phys_gpr[i] =  
			new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), 
					this, phys_gpr[i], ss_desc.str().c_str());
	}
	for (unsigned int i = 0; i < num_log_gprs; i++)
	{
		stringstream ss;
		stringstream ss_desc;
		ss << "GPR[" << i << "]";
		ss_desc << "Logical register " << i;
		reg_gpr[i] = 
			new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), 
					this, gpr[i], ss_desc.str().c_str());
	}
	for (unsigned int i = 0; i < num_phys_spsrs; i++)
	{
		stringstream ss;
		stringstream ss_desc;
		ss << "SPSR[" << i << "]";
		ss_desc << "SPSR[" << i << "] register";
		reg_spsr[i] =
			new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), 
					this, spsr[i], ss_desc.str().c_str());
	}

	// Init the processor at SUPERVISOR mode
	SetCPSR_Mode(SUPERVISOR_MODE);
	// Disable fast and normal interruptions
	SetCPSR_I();
	SetCPSR_F();

	// Set the right format for various of the variables
	param_cpu_cycle_time.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	param_voltage.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	param_trap_on_instruction_counter.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
}

/** Destructor.
 */
CPU::
~CPU()
{
	for (unsigned int i = 0; i < num_phys_gprs; i++)
		if (reg_phys_gpr[i]) delete reg_phys_gpr[i];
	for (unsigned int i = 0; i < num_log_gprs; i++)
		if (reg_gpr[i]) delete reg_gpr[i];
	for (unsigned int i = 0; i < num_phys_spsrs; i++)
		if (reg_spsr[i]) delete reg_spsr[i];
}

/** Object setup method.
 * This method is required for all UNISIM objects and will be called during
 *   the setup phase.
 * 
 * @return true on success, false otherwise
 */
bool 
CPU::
BeginSetup()
{
	logger << DebugInfo << "CPU Setup" << EndDebugInfo;
	if (verbose)
		logger << DebugInfo
			<< "Verbose activated."
			<< EndDebugInfo;

	/* endianness should be fixed by the time we arrive here by the
	 *   cp15
	 */
	if ( verbose )
	{
		if ( cp15.GetEndianness() == E_LITTLE_ENDIAN )
			logger << DebugInfo
				<< "Setting endianness to little endian"
				<< EndDebugInfo;
		else
			logger << DebugInfo
				<< "Setting endianness to big endian"
				<< EndDebugInfo;
	}
	SetEndianness(cp15.GetEndianness());

	/* setting initial pc by the cp15 vinithi parameter */
	uint32_t init_pc = cp15.GetVINITHI() ? 0xFFFF0000UL : 0;
	if ( verbose )
		logger << DebugInfo
			<< "Setting initial pc to 0x"
			<< hex << init_pc << dec
			<< EndDebugInfo;
	SetGPR(15, init_pc);

	if ( cpu_cycle_time == 0 )
	{
		// we can't provide a valid cpu cycle time configuration
		//   automatically
		logger << DebugError
			<< "cpu-cycle-time should be bigger than 0"
			<< EndDebugError;
		return false;
	}

	/* TODO: Remove this section once all the debuggers use the unisim
	 *   kernel interface for registers.
	 */
	/* setting debugging registers */
	if (verbose)
		logger << DebugInfo
		<< "Initializing debugging registers"
		<< EndDebugError;
	
	for (int i = 0; i < 16; i++) 
	{
		stringstream str;
		str << "r" << i;
		registers_registry[str.str().c_str()] =
			new SimpleRegister<uint32_t>(str.str().c_str(), &gpr[i]);
	}
	registers_registry["sp"] = new SimpleRegister<uint32_t>("sp", &gpr[13]);
	registers_registry["lr"] = new SimpleRegister<uint32_t>("lr", &gpr[14]);
	registers_registry["pc"] = new SimpleRegister<uint32_t>("pc", &gpr[15]);
	registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &cpsr);
	/* End TODO */

	return true;
}

/** Object end setup method.
 * This method is required for all UNISIM objects and will be called during
 *   the end setup phase.
 * 
 * @return true on success, false otherwise
 */
bool
CPU::
EndSetup()
{
	/* Initialize the caches and power support as required. */
	unsigned int min_cycle_time = 0;
	uint64_t il1_def_voltage = 0;
	uint64_t dl1_def_voltage = 0;

	if ( icache.power_mode_import )
	{
		min_cycle_time =
			icache.power_mode_import->GetMinCycleTime();
		il1_def_voltage =
			icache.power_mode_import->GetDefaultVoltage();
	}
	if ( dcache.power_mode_import )
	{
		if ( dcache.power_mode_import->GetMinCycleTime() >
				min_cycle_time )
			min_cycle_time =
				dcache.power_mode_import->GetMinCycleTime();
		dl1_def_voltage = 
			dcache.power_mode_import->GetDefaultVoltage();
	}

	if ( min_cycle_time > 0 )
	{
		if ( cpu_cycle_time < min_cycle_time )
		{
			logger << DebugWarning;
			logger << "A cycle time of " << cpu_cycle_time
				<< " ps is too low for the simulated"
				<< " hardware !" << endl;
			logger << "cpu cycle time should be >= "
				<< min_cycle_time << " ps." << endl;
			logger << EndDebugWarning;
		}
	}

	if ( voltage == 0 )
	{
		voltage = (il1_def_voltage > dl1_def_voltage) ? 
			il1_def_voltage :
			dl1_def_voltage;
		logger << DebugWarning
			<< "A cpu voltage was not defined (set to 0)."
			<< " Using the maximum voltage found from the caches as "
			<< " current voltage. Voltage used is "
			<< voltage
			<< " mV." << endl;
		if ( icache.power_mode_import )
			logger << "  - instruction cache voltage = "
				<< il1_def_voltage
				<< " mV";
		if ( dcache.power_mode_import )
		{
			if ( icache.power_mode_import )
				logger << endl;
			logger << "  - data cache voltage = "
				<< dl1_def_voltage
				<< " mV";
		}
		logger << EndDebugWarning;
	}
	
	if ( icache.power_mode_import )
		icache.power_mode_import->SetPowerMode(cpu_cycle_time, voltage);
	if ( dcache.power_mode_import )
		dcache.power_mode_import->SetPowerMode(cpu_cycle_time, voltage);

	if ( verbose )
	{
		logger << DebugInfo
			<< "Setting cpu cycle time to "
			<< cpu_cycle_time
			<< " ps."
			<< EndDebugInfo;
		logger << DebugInfo
			<< "Setting cpu voltage to "
			<< voltage
			<< " mV."
			<< EndDebugInfo;
	}
			
	/* If the memory access reporting import is not connected remove the need of
	 *   reporting memory accesses and finished instruction.
	 */
	if(!memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_finished_instruction_reporting = false;
	}
	
	return true;
}

/** Object disconnect method.
 * This method is called when this UNISIM object is disconnected from other
 *   UNISIM objects.
 */
void 
CPU::
OnDisconnect()
{
	/* TODO */
}

/** Execute one complete instruction.
 */
void 
CPU::
StepInstruction()
{
	uint32_t current_pc;

	current_pc = GetGPR(PC_reg);
	cur_instruction_address = current_pc;

	if (debug_control_import) 
	{
		DebugControl<uint64_t>::DebugCommand dbg_cmd;

		do 
		{
			dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
	
			if (dbg_cmd == DebugControl<uint64_t>::DBG_STEP) 
			{
				/* Nothing to do */
				break;
			}
			if (dbg_cmd == DebugControl<uint64_t>::DBG_SYNC) 
			{
				// Sync();
				continue;
			}

			if (dbg_cmd == DebugControl<uint64_t>::DBG_KILL) 
			{
				Stop(0);
			}
			if(dbg_cmd == DebugControl<uint64_t>::DBG_RESET) 
			{
				// TODO : memory_interface->Reset(); 
			}
		}
		while(1);
	}

	if (requires_memory_access_reporting) 
	{
		if(memory_access_reporting_import)
		{
			uint32_t insn_size;
			if (GetCPSR_T())
			{
				/* This implementation should never enter into thumb mode
				 *   so if we enter this code we have an error in the 
				 *   implementation.
				 */
				insn_size = 2;
				assert("Thumb mode not supported" == 0);
			}
			else
				insn_size = 4;
			memory_access_reporting_import->ReportMemoryAccess(
				MemoryAccessReporting<uint64_t>::MAT_READ, 
				MemoryAccessReporting<uint64_t>::MT_INSN, 
				current_pc, insn_size);
		}
	}
	
	/* arm32 state */
	isa::arm32::Operation<unisim::component::cxx::processor::arm::arm926ejs::CPU> *
		op = NULL;
	uint32_t insn;
	
	ReadInsn(current_pc, insn);
	/* convert the instruction to host endianness */
	insn = Target2Host(GetEndianness(), insn);
		
	/* Decode current PC */
	op = arm32_decoder.Decode(current_pc, insn);
	/* Execute instruction */
	op->execute(*this);
	//op->profile(profile);
	
	/* perform the memory load/store operations */
	PerformLoadStoreAccesses();

	bool exception_occurred = false;
	if ( unlikely(exception) )
		exception_occurred = HandleException();
	
	instruction_counter++;
	if ( unlikely((trap_on_instruction_counter == instruction_counter)
				&& instruction_counter_trap_reporting_import) )
		instruction_counter_trap_reporting_import->ReportTrap(*this);

	if ( unlikely(trap_on_exception && exception_occurred
				&& exception_trap_reporting_import) )
		exception_trap_reporting_import->ReportTrap(*this);
	
	if(requires_finished_instruction_reporting)
		if(memory_access_reporting_import)
			memory_access_reporting_import->ReportFinishedInstruction(
					GetGPR(PC_reg));
}

/** Inject an intrusive read memory operation.
 *
 * @param addr the target address to read from
 * @param buffer the buffer to fill with the read memory
 * @param size the amount of bytes to read from starting from the target 
 *   address
 *
 * @return true on success, false otherwise
 */
bool 
CPU::
InjectReadMemory(uint64_t addr, 
	void *buffer,
	uint32_t size)
{
	uint32_t index = 0;
	uint32_t base_addr = (uint32_t)addr;
	uint32_t ef_addr;

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		while (size != 0)
		{
			// need to access the data cache before accessing the main memory
			ef_addr = base_addr + index;

			uint32_t cache_tag = dcache.GetTag(ef_addr);
			uint32_t cache_set = dcache.GetSet(ef_addr);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( dcache.GetValid(cache_set, cache_way) )
				{
					// the cache access is a hit, data can be simply read from 
					//   the cache
					uint32_t cache_index = dcache.GetIndex(ef_addr);
					uint32_t read_data_size =
						dcache.GetDataCopy(cache_set, cache_way, 
								cache_index, size, 
								&(((uint8_t *)buffer)[index]));
					index += read_data_size;
					size -= read_data_size;
					cache_hit = true;
				}
			}
			if ( !cache_hit )
			{
				PrRead(ef_addr,
						&(((uint8_t *)buffer)[index]),
						1);
				index++;
				size--;
			}
		}
	}
	else
	{
		// no data cache on this system, just send request to the memory
		//   subsystem
		while ( size != 0 )
		{
			ef_addr = base_addr + index;
			PrRead(ef_addr,
					&(((uint8_t *)buffer)[index]),
					1);
			index++;
			size--;
		}
	}

	return true;
}

/** Inject an intrusive write memory operation
 *
 * @param addr the target address to write to
 * @param buffer a buffer containing the data to write
 * @param size the amount of bytes to write
 *
 * @return true on success, false otherwise
 */
bool 
CPU::
InjectWriteMemory(uint64_t addr, 
	const void *buffer, 
	uint32_t size)
{
	uint32_t index = 0;
	uint32_t base_addr = (uint32_t)addr;
	uint32_t ef_addr;
	
	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		// access memory while using the linux_os_import
		//   tcm is ignored
		while ( size != 0 )
		{
			// need to access the data cache before accessing the main memory
			ef_addr = base_addr + index;

			uint32_t cache_tag = dcache.GetTag(ef_addr);
			uint32_t cache_set = dcache.GetSet(ef_addr);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( dcache.GetValid(cache_set, cache_way) )
				{
					// the cache access is a hit, data can be simply read from 
					//   the cache
					uint32_t cache_index = dcache.GetIndex(ef_addr);
					uint32_t write_data_size =
							dcache.SetData(cache_set, cache_way, 
									cache_index, size, 
									&(((uint8_t *)buffer)[index]));
					dcache.SetDirty(cache_set, cache_way, 1);
					index += write_data_size;
					size -= write_data_size;
					cache_hit = true;
				}
			}
			if ( !cache_hit )
			{
				PrWrite(ef_addr,
						&(((uint8_t *)buffer)[index]),
						1);
				index++;
				size--;
			}
		}
	}
	else
	{
		// there is no data cache in this system just send the request to
		//   the memory subsystem
		while ( size != 0 )
		{
			ef_addr = base_addr + index;
			PrWrite(ef_addr,
					&(((uint8_t *)buffer)[index]),
					1);
			index++;
			size--;
		}
	}

	return true;
}

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true set the reporting of memory acesses, unset 
 *   otherwise
 */
void 
CPU::
RequiresMemoryAccessReporting(bool report)
{
	requires_memory_access_reporting = report;
}

/** Set/unset the reporting of finishing instructions.
 * 
 * @param report if true set the reporting of finishing instructions, 
 *   unset otherwise
 */
void 
CPU::
RequiresFinishedInstructionReporting(bool report)
{
	requires_finished_instruction_reporting = report;
}

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
bool 
CPU::
ReadMemory(uint64_t addr, 
		void *buffer, 
		uint32_t size)
{
	bool status = true;
	uint32_t index = 0;
	uint32_t va = addr;
	uint32_t mva = va;
	uint32_t pa = mva;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		// non intrusive access with linux support
		while (size != 0 && status)
		{
			mva = va + index;
			pa = mva;
			if ( likely(cp15.IsMMUEnabled()) )
				status = NonIntrusiveTranslateVA(true, 
						va + index, mva, pa, cacheable, bufferable);

			bool cache_hit = false;
			if ( likely(status && cacheable) )
			{
				uint32_t cache_tag = dcache.GetTag(mva);
				uint32_t cache_set = dcache.GetSet(mva);
				uint32_t cache_way;
				if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
				{
					if ( dcache.GetValid(cache_set, cache_way) )
					{
						// the cache access is a hit, data can be simply read
						//   from the cache
						uint32_t cache_index =
							dcache.GetIndex(mva);
						uint32_t data_read_size =
							dcache.GetDataCopy(cache_set,
								cache_way, cache_index, size,
								&(((uint8_t *)buffer)[index]));
						index += data_read_size;
						size -= data_read_size;
						cache_hit = true;
					}
				}
			}
			if ( status && !cache_hit )
			{
				status = status &&
					ExternalReadMemory(pa,
									   &(((uint8_t *)buffer)[index]),
									   1);
				if ( status )
				{
					index++;
					size--;
				}
			}
		}
	}
	else
	{
		// there is no data cache in this system just perform the request
		//   to the memory subsystem
		while ( size != 0 && status )
		{
			if ( likely(cp15.IsMMUEnabled()) )
				status = NonIntrusiveTranslateVA(true, 
						va + index, mva, pa, cacheable, bufferable);

			status = status &&
					ExternalReadMemory(pa + index,
							&(((uint8_t *)buffer)[index]),
							1);
			if ( status )
			{
				index++;
				size--;
			}
		}
	}

	return status;
}

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
bool 
CPU::
WriteMemory(uint64_t addr, 
		const void *buffer, 
		uint32_t size)
{
	bool status = true;
	uint32_t index = 0;
	uint32_t va = addr;
	uint32_t mva = va;
	uint32_t pa = mva;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		// non intrusive access with linux support
		while ( size != 0 && status )
		{
			mva = va + index;
			pa = mva;
			if ( likely(cp15.IsMMUEnabled()) )
				status = NonIntrusiveTranslateVA(false,
						va + index, mva, pa, cacheable, bufferable);

			bool cache_hit = false;
			if ( likely(status && cacheable) )
			{
				uint32_t cache_tag = dcache.GetTag(mva);
				uint32_t cache_set = dcache.GetSet(mva);
				uint32_t cache_way;
				if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
				{
					if ( dcache.GetValid(cache_set, cache_way) )
					{
						uint32_t cache_index = dcache.GetIndex(mva);
						uint32_t write_size =
							dcache.SetData(cache_set, cache_way, cache_index, 
									size, &(((uint8_t *)buffer)[index]));
						dcache.SetDirty(cache_set, cache_way, 1);
						index += write_size;
						size -= write_size;
						cache_hit = true;
					}
				}
			}
			if ( status && !cache_hit )
			{
				status = status &&
					ExternalWriteMemory(pa,
										&(((uint8_t *)buffer)[index]),
										1);
				if ( status )
				{
					index++;
					size--;
				}
			}
		}
	}
	else
	{
		// there is no data cache in this system, just write the data to
		//   the memory subsystem
		while ( size != 0 && status )
		{
			if ( likely(cp15.IsMMUEnabled()) )
				status = NonIntrusiveTranslateVA(false,
						va + index, mva, pa, cacheable, bufferable);
			status = status &&
				ExternalWriteMemory(pa,
							&(((uint8_t *)buffer)[index]),
							1);
			if ( status )
			{
				index++;
				size--;
			}
		}
	}

	return status;
}

/** Get a register by its name.
 * Gets a register interface to the register specified by name.
 *
 * @param name the name of the requested register
 *
 * @return a pointer to the RegisterInterface corresponding to name
 */
Register *
CPU::
GetRegister(const char *name)
{
	if (registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;
}
		
/** Disasm an instruction address.
 * Returns a string with the disassembling of the instruction found 
 *   at address addr.
 * 
 * @param addr the address of the instruction to disassemble
 * @param next_addr the address following the requested instruction
 *
 * @return the disassembling of the requested instruction address
 */
string 
CPU::
Disasm(uint64_t addr, uint64_t &next_addr)
{
	isa::arm32::Operation<unisim::component::cxx::processor::arm::arm926ejs::CPU> *
		op = NULL;
	uint32_t insn;
	
	stringstream buffer;
	if (GetCPSR_T()) 
	{
		assert("Thumb instructions not supported" != 0);
	} 
	else 
	{
		if (!ReadMemory(addr, &insn, 4)) 
		{
			buffer << "Could not read from memory";
			return buffer.str();
		}
		if (GetEndianness() == E_BIG_ENDIAN)
			insn = BigEndian2Host(insn);
		else
			insn = LittleEndian2Host(insn);
		op = arm32_decoder.Decode(addr, insn);
		op->disasm(*this, buffer);
	}

	return buffer.str();
}
		
/** Reads 32bits instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * 
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 */
void 
CPU::
ReadInsn(uint32_t address, uint32_t &val)
{
	uint32_t size = 4;
	uint8_t *data;
	uint32_t va = address;
	uint32_t mva = va;
	uint32_t pa = mva;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1; // instruction cache ignores the bufferable status

	if ( verbose & 0x04 )
		logger << DebugInfo
			<< "Fetching instruction at address 0x" << std::hex << va
			<< std::dec
			<< EndDebugInfo;

	if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing instruction read"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}


	if ( likely(cp15.IsICacheEnabled() && icache.GetSize() && cacheable) )
	{
		icache.read_accesses++;
		// check the instruction cache
		uint32_t cache_tag = icache.GetTag(mva);
		uint32_t cache_set = icache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( icache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( icache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
				if ( verbose & 0x04 )
					logger << DebugInfo
						<< "Cache hit when fetching instruction with:" << std::endl
						<< " - va = 0x" << std::hex << va << std::endl
						<< " - mva = 0x" << mva << std::endl
						<< " - pa = 0x" << pa << std::dec
						<< EndDebugInfo;
			}
		}
		if ( unlikely(!cache_hit) )
		{
			// get a way to replace
			cache_way = icache.GetNewWay(cache_set);
			// no need to check valiad and dirty bits
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
				icache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			if ( verbose & 0x04 )
				logger << DebugInfo
					<< "Cache miss when fetching instruction at address with:" << std::endl
					<< " - va = 0x" << std::hex << va << std::endl
					<< " - mva = 0x" << mva << std::endl
					<< " - pa = 0x" << pa << std::dec << std::endl
					<< "Accessing the memory system."
					<< EndDebugInfo;
			uint32_t cache_line_size = icache.GetData(cache_set,
					cache_way, &cache_data);
			PrRead(cache_address, cache_data,
					cache_line_size);
			icache.SetTag(cache_set, cache_way, cache_tag);
			icache.SetValid(cache_set, cache_way, 1);
		}
		else
		{
			// cache hit
			icache.read_hits++;
		}

		// at this point the data is in the cache, we can read it from the
		//   cache
		uint32_t cache_index = icache.GetIndex(mva);
		(void)icache.GetData(cache_set, cache_way, cache_index,
				size, &data);
		icache.GetDataCopy(cache_set, cache_way, cache_index, size,
				(uint8_t *)&val);

		if ( unlikely(icache.power_estimator_import != 0) )
			icache.power_estimator_import->ReportReadAccess();
	}
	else
	{
		if ( verbose & 0x04 )
			logger << DebugInfo
				<< "Fetching instruction from memory (no cache or entry not cacheable):" << std::endl
				<< " - va = 0x" << std::hex << va << std::endl
				<< " - mva = 0x" << mva << std::endl
				<< " - pa = 0x" << pa << std::dec
				<< EndDebugInfo;

		// no instruction cache present, just request the insn to the
		//   memory system
		// another possibility is that the access is not cacheable
		PrRead(pa, (uint8_t *)&val, size);
	}
}

/** Memory prefetch instruction.
 * This method is used to make memory prefetches into the caches (if 
 *   available), that is it sends a memory read that doesn't keep the 
 *   request result.
 * 
 * @param address the address of the prefetch
 */
void 
CPU::
ReadPrefetch(uint32_t address)
{
	address = address & ~((uint32_t)0x3);
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetPrefetch(address);
	ls_queue.push(memop);
}

/** 32bits memory read that stores result into the PC
 * This methods reads 32bits from memory and stores the result into
 *   the pc register of the CPU.
 * 
 * @param address the base address of the 32bits read
 */
void 
CPU::
Read32toPCUpdateT(uint32_t address)
{
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetReadToPCUpdateT(address);
	ls_queue.push(memop);

	if (requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address & ~((uint32_t)0x3), 4);
}

/** 32bits memory read into the PC and updates thumb state.
 * This methods reads 32bits from memory and stores the result into
 *   the pc register of the CPU and updates thumb state if necessary
 * 
 * @param address the base address of the 32bits read
 */
void 
CPU::
Read32toPC(uint32_t address)
{
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetReadToPC(address);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,				
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address & ~((uint32_t)0x3), 4);
}

/** 32bits memory read into one of the general purpose registers.
 * This method reads 32bits from memory and stores the result into
 *   the general purpose register indicated by the input reg
 * 
 * @param address the base address of the 32bits read
 * @param reg the register to store the resulting read
 */
void 
CPU::
Read32toGPR(uint32_t address, uint32_t reg)
{
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetRead(address, 4, reg, false, false);
	ls_queue.push(memop);

	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address & ~((uint32_t)0x3), 4);
}

/** 32bits aligned memory read into one of the general purpose registers.
 * This method reads 32bits from memory and stores the result into
 *   the general purpose register indicated by the input reg. Note that this
 *   read methods supposes that the address is 32bits aligned.
 * 
 * @param address the base address of the 32bits read
 * @param reg the register to store the resulting read
 */
void 
CPU::
Read32toGPRAligned(uint32_t address, uint32_t reg)
{
	MemoryOp *memop;
	
	address = address & ~((uint32_t)0x3);

	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetRead(address, 4, reg, true, false);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 4);
}

/** 16bits aligned memory read into one of the general purpose registers.
 * This method reads 16bits from memory and stores the result into
 *   the general purpose register indicated by the input reg. Note that this
 *   read methods supposes that the address is 16bits aligned.
 * 
 * @param address the base address of the 16bits read
 * @param reg the register to store the resulting read
 */
void 
CPU::
Read16toGPRAligned(uint32_t address, uint32_t reg)
{
	MemoryOp *memop;
	
	address = address & ~((uint32_t)0x1);
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetRead(address, 2, reg, true, false);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 2);
}

/** Signed 16bits aligned memory read into one of the GPRs.
 * This method reads 16bits from memory and stores the result into
 *   the general purpose register indicated by the input reg. Note that this
 *   read methods supposes that the address is 16bits aligned. The 16bits
 *   value is considered signed and sign extended to the register size.
 * 
 * @param address the base address of the 16bits read
 * @param reg the register to store the resulting read
 */
void 
CPU::
ReadS16toGPRAligned(uint32_t address, uint32_t reg)
{
	MemoryOp *memop;
	
	address = address & ~((uint32_t)0x1);
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetRead(address, 2, reg, /* aligned */true, /* signed */true);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 2);
}

/** 8bits memory read into one of the general purpose registers.
 * This method reads 8bits from memory and stores the result into
 *   the general purpose register indicated by the input reg.
 * 
 * @param address the base address of the 8bits read
 * @param reg the register to store the resulting read
 */
void 
CPU::
ReadS8toGPR(uint32_t address, uint32_t reg)
{
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetRead(address, 1, reg, /* aligned */true, /* signed */true);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 1);
}

/** signed 8bits memory read into one of the general purpose registers.
 * This method reads 8bits from memory and stores the result into
 *   the general purpose register indicated by the input reg. The 8bits 
 *   value is considered signed and sign extended to the register size.
 * 
 * @param address the base address of the 8bits read
 * @param reg the register to store the resulting read
 */
void 
CPU::
Read8toGPR(uint32_t address, uint32_t reg)
{
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetRead(address, 1, reg, /* aligned */true, /* signed */false);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 1);
}

/** 32bits memory write.
 * This method write the giving 32bits value into the memory system.
 * 
 * @param address the base address of the 32bits write
 * @param value the value to write into memory
 */
void 
CPU::
Write32(uint32_t address, uint32_t value)
{
	MemoryOp *memop;
	
	address = address & ~((uint32_t)0x3);
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetWrite(address, 4, value);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_WRITE,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 4);
}

/** 16bits memory write.
 * This method write the giving 16bits value into the memory system.
 * 
 * @param address the base address of the 16bits write
 * @param value the value to write into memory
 */
void 
CPU::
Write16(uint32_t address, uint16_t value)
{
	address = address & ~((uint32_t)0x1);
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetWrite(address, 2, value);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_WRITE,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 2);
}

/** 8bits memory write.
 * This method write the giving 8bits value into the memory system.
 * 
 * @param address the base address of the 8bits write
 * @param value the value to write into memory
 */
void 
CPU::
Write8(uint32_t address, uint8_t value)
{
	MemoryOp *memop;
	
	if ( free_ls_queue.empty() )
		memop = new MemoryOp();
	else
	{
		memop = free_ls_queue.front();
		free_ls_queue.pop();
	}
	memop->SetWrite(address, 1, value);
	ls_queue.push(memop);
	
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_WRITE,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					address, 1);
}

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
bool 
CPU::
CoprocessorLoad(uint32_t cp_num, uint32_t address)
{
	assert("CoprocessorLoad not implemented" == 0);
	return false;
}

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
bool 
CPU::
CoprocessorLoad(uint32_t cp_num, uint32_t address, uint32_t option)
{
	assert("CoprocessorLoad not implemented" == 0);
	return false;
}

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
bool 
CPU::
CoprocessorStore(uint32_t cp_num, uint32_t address)
{
	assert("CoprocessorStore not implemented" == 0);
	return false;
}

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
bool 
CPU::
CoprocessorStore(uint32_t cp_num, uint32_t address, uint32_t option)
{
	assert("CoprocessorStore not implemented" == 0);
	return false;
}

/** Coprocessor Data Processing method
 * 
 * @param cp_num the coprocessor for which the data processing is requested
 * @param op1 first opcode for the coprocessor
 * @param op2 second opcode for the coprocessor
 * @param crd coprocessor destination register
 * @param crn coprocessor first operand register
 * @param crm coprocessor second operand register
 */
void 
CPU::
CoprocessorDataProcess(uint32_t cp_num, uint32_t op1, uint32_t op2,
		uint32_t crd, uint32_t crn, uint32_t crm)
{
	assert("CoprocessorDataProcess not implemented" == 0);
}

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
void 
CPU::
MoveToCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
		uint32_t rd, uint32_t crn, uint32_t crm)
{
	// the only coprocessor available is cp15
	if ( unlikely(cp_num != 15) )
		return;
	cp15.WriteRegister(
			op1, op2,
			crn, crm,
			GetGPR(rd));
}

/**
 * Move to ARM register from Coprocessor
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
void
CPU::
MoveFromCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
		uint32_t rd, uint32_t crn, uint32_t crm)
{
	uint32_t val = 0;

	// the only coprocessor available is cp15
	if ( unlikely(cp_num != 15) )
		return;
	cp15.ReadRegister(
			op1, op2,
			crn, crm,
			val);
	if ( rd == 15 ) 
	{
		// N flag = data[31] 
		// Z flag = data[30] 
		// C flag = data[29] 
		// V flag = data[28]
		if ( val & 0x80000000UL ) SetCPSR_N(true);
		else SetCPSR_N(false);
		if ( val & 0x40000000UL ) SetCPSR_Z(true);
		else SetCPSR_Z(false);
		if ( val & 0x20000000UL ) SetCPSR_C(true);
		else SetCPSR_C(false);
		if ( val & 0x10000000UL ) SetCPSR_V(true);
		else SetCPSR_V(false);
	}
	else
		SetGPR(rd, val);
}

/** Get caches info
 *
 */
void
CPU::GetCacheInfo(bool &unified, 
		uint32_t &isize, uint32_t &iassoc, uint32_t &ilen,
		uint32_t &dsize, uint32_t &dassoc, uint32_t &dlen)
{
	unified = false;
	isize = icache.GetSize();
	iassoc = icache.GetNumWays();
	ilen = icache.LINE_SIZE;
	dsize = icache.GetSize();
	dassoc = dcache.GetNumWays();
	dlen = dcache.LINE_SIZE;
}

/** Drain write buffer.
 * Perform a memory barrier by draining the write buffer.
 */
void 
CPU::
DrainWriteBuffer()
{
	logger << DebugWarning
		<< "TODO: Drain write buffer once implemented"
		<< EndDebugWarning;
}

/** Invalidate ICache single entry using MVA
 *
 * Perform an invalidation of a single entry in the ICache using the
 *   given address in MVA format.
 *
 * @param mva the address to invalidate
 */
void 
CPU::
InvalidateICacheSingleEntryWithMVA(uint32_t init_mva)
{
	uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);

	if ( unlikely(verbose & 0x02) )
	{
		logger << DebugInfo
			<< "Invalidating ICache single entry with MVA:" << std::endl
			<< " - mva               = 0x" << std::hex << init_mva << std::endl
			<< " - cache aligned mva = 0x" << mva << std::dec
			<< EndDebugInfo;
	}

	if ( likely(cp15.IsICacheEnabled() && icache.GetSize()) )
	{
		uint32_t cache_tag = icache.GetTag(mva);
		uint32_t cache_set = icache.GetSet(mva);
		uint32_t cache_way = 0;
		bool cache_hit = false;
		if ( likely(icache.GetWay(cache_tag, cache_set, &cache_way)) )
			cache_hit = true;

		if ( likely(cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "ICache hit (set = "
					<< cache_set << ", way = "
					<< cache_way << "), invalidating it"
					<< EndDebugInfo;
			icache.SetValid(cache_set, cache_way, 0);
		}
		else
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "ICache miss (set = "
					<< cache_set
					<< ")"
					<< EndDebugInfo;
		}
	}
}

/** Clean DCache single entry using MVA
 *
 * Perform a clean of a single entry in the DCache using the given
 *   address in MVA format.
 *
 * @param mva the address to clean
 * @param invalidate true if the line needs to be also invalidated
 */
void 
CPU::
CleanDCacheSingleEntryWithMVA(uint32_t init_mva, bool invalidate)
{
	uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);
	uint32_t pa = mva;

	if ( unlikely(verbose & 0x02) )
	{
		logger << DebugInfo
			<< "Cleaning DCache single entry with MVA:" << std::endl
			<< " - mva               = 0x" << std::hex << init_mva << std::endl
			<< " - cache aligned mva = 0x" << mva << std::dec
			<< EndDebugInfo;
	}

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( likely(dcache.GetWay(cache_tag, cache_set, &cache_way)) )
			cache_hit = true;

		if ( likely(cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache hit (set = "
					<< cache_set << ", way = "
					<< cache_way << ")"
					<< EndDebugInfo;
			uint8_t cache_dirty = dcache.GetDirty(cache_set, cache_way);

			if ( cache_dirty != 0 )
			{
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Line is dirty, performing a cleaning"
						<< EndDebugInfo;
				uint8_t *data = 0;
				// translate the address
				if ( likely(cp15.IsMMUEnabled()) )
					TranslateMVA(mva, pa);
				dcache.GetData(cache_set, cache_way, &data);
				if ( unlikely(verbose & 0x02) )
				{
					logger << DebugInfo
						<< "Cleaning cache line:" << std::endl
						<< " - mva = 0x" << std::hex << mva << std::endl
						<< " - pa  = 0x" << pa << std::endl
						<< " - tag = 0x" << cache_tag << std::endl
						<< " - set = " << std::dec << cache_set << std::endl
						<< " - way = " << cache_way << std::endl
						<< " - data =" << std::hex;
					for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
						logger << " " << (unsigned int)data[i];
					logger << std::dec
						<< EndDebugInfo;
				}
				PrWrite(pa, data, dcache.LINE_SIZE);
				dcache.SetDirty(cache_set, cache_way, 0);
			}
			else
			{
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Line is already cleaned, doing nothing"
						<< EndDebugInfo;
			}
			if ( invalidate )
				dcache.SetValid(cache_set, cache_way, 0);
		}
		else
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache miss (set = "
					<< cache_set
					<< ")"
					<< EndDebugInfo;
		}
	}
}

/** Invalidate the caches.
 * Perform a complete invalidation of the instruction cache and/or the 
 *   data cache.
 *
 * @param insn_cache whether or not the instruction cache should be 
 *   invalidated
 * @param data_cache whether or not the data cache should be invalidated
 */
void 
CPU::
InvalidateCache(bool insn_cache, bool data_cache)
{
	if ( insn_cache )
	{
		icache.Invalidate();
	}
	if ( data_cache )
	{
		dcache.Invalidate();
	}
}

/** Invalidate the TLBs.
 * Perform a complete invalidation of the instruction TLB and/or the 
 *   data TLB.
 *
 * @param insn_tlb whether or not the instruction tlb should be invalidated
 * @param data_tlb whether or not the data tlb should be invalidated
 */
void 
CPU::
InvalidateTLB()
{
	// only the tlb needs to be invalidated, do not touch the lockdown tlb
	tlb.Invalidate();
}

/** Test and clean DCache.
 * Perform a test and clean operation of the DCache.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
bool 
CPU::
TestAndCleanDCache()
{
	bool cleaned = true;
	uint32_t num_sets = dcache.GetNumSets();
	uint32_t num_ways = dcache.GetNumWays();
	uint32_t set_index = 0;
	uint32_t way_index = 0;

	uint32_t dirty_set = 0;
	uint32_t dirty_way = 0;

	for ( set_index = 0; 
			cleaned && (set_index < num_sets);
			set_index++ )
	{
		for ( way_index = 0;
				cleaned && (way_index < num_ways); 
				way_index++ )
		{
			if ( dcache.GetValid(set_index, way_index) != 0 )
			{
				if ( dcache.GetDirty(set_index, way_index) != 0 )
				{
					cleaned = false;
					dirty_set = set_index;
					dirty_way = way_index;
				}
			}
		}
	}

	if ( !cleaned )
	{
		/* Get the address and data of the dirty line */
		uint32_t mva = dcache.GetBaseAddress(dirty_set, dirty_way);
		uint32_t pa = mva;
		if ( likely(cp15.IsMMUEnabled()) )
			TranslateMVA(mva, pa);
		uint8_t *data = 0;
		dcache.GetData(dirty_set, dirty_way, &data);

		/* Write the data into memory */
		PrWrite(pa, data, dcache.LINE_SIZE);
		/* Set the line as clean */
		dcache.SetDirty(dirty_set, dirty_way, 0);

		/* Check if there are still dirty lines */
		cleaned = true;
		for ( set_index = dirty_set;
				cleaned && (set_index < num_sets);
				set_index++ )
		{
			for ( way_index = 0; // we might be doing some useless work
					cleaned && (way_index < num_ways);
					way_index++ )
			{
				if ( (dcache.GetValid(set_index, way_index) != 0)
						&& (dcache.GetDirty(set_index, way_index) != 0) )
				{
					cleaned = false;
				}
			}
		}
	}

	return cleaned;
}

/** Test, clean and invalidate DCache.
 * Perform a test and clean operation of the DCache, and invalidate the
 *   complete cache if it is clean.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
bool 
CPU::
TestCleanAndInvalidateDCache()
{
	bool cleaned = TestAndCleanDCache();

	if ( cleaned ) dcache.Invalidate();

	return cleaned;
}

/** Unpredictable Instruction Behaviour.
 * This method is just called when an unpredictable behaviour is detected to
 *   notifiy the processor.
 */
void 
CPU::
UnpredictableInsnBehaviour()
{
	logger << DebugWarning
		<< "Trying to execute unpredictable behavior instruction"
		<< "Location: " 
		<< __FUNCTION__ << ":" 
		<< __FILE__ << ":" 
		<< __LINE__ << ": "
		<< EndDebugWarning;
}

/** Check access permission and domain bits.
 *
 * @param is_read true if the access is a read
 * @param ap the access permission bits
 * @param domain the domain being used
 * 
 * @return true if correct, false otherwise
 */
bool 
CPU::
CheckAccessPermission(bool is_read,
		uint32_t ap,
		uint32_t domain)
{
	if ( ap == 0x00UL )
	{
		return false;
	}

	else if ( ap == 0x01UL )
	{
		uint32_t mode = GetCPSR_Mode();
		if ( mode == USER_MODE )
		{
			logger << DebugError
				<< "Accessing privileged data under USER mode: "
				<< std::endl
				<< "- is_read = " << is_read << std::endl
				<< "- ap  = " << ap << std::endl
				<< "- domain = " << domain << std::endl
				<< EndDebugError;
			return false;
		}
		else
		{
			if ( verbose & 0x010 )
				logger << DebugInfo
					<< "Accessing privileged data under privileged mode."
					<< EndDebugInfo;
		}
	}

	else if ( ap == 0x02UL )
	{
		uint32_t mode = GetCPSR_Mode();
		if ( mode == USER_MODE )
		{
			if ( !is_read )
			{
				logger << DebugError
					<< "Accessing privileged data under USER mode: "
					<< std::endl
					<< "- is_read = " << is_read << std::endl
					<< "- ap  = " << ap << std::endl
					<< "- domain = " << domain << std::endl
					<< EndDebugError;
				return false;
			}
			else
			{
				if ( verbose & 0x010 )
					logger << DebugInfo
						<< "Accessing privileged data under user mode"
						<< " (read-only)"
						<< EndDebugInfo;
			}
		}
		else
		{
			if ( verbose & 0x010 )
			{
				logger << DebugInfo
					<< "Accessing privileged data under privileged mode."
					<< EndDebugInfo;
			}
		}
	}	

	else if ( ap == 0x03UL )
	{
		if ( verbose & 0x010 )
			logger << DebugInfo
				<< "Accessing non protected data."
				<< EndDebugInfo;
	}

	// all the checks succeded
	return true;
}

/** Translate address from MVA to physical address.
 *
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool 
CPU::
TranslateMVA(uint32_t mva, uint32_t &pa)
{
	// Build first the address of the Translation Table Base
	uint32_t ttb_addr = 
		cp15.GetTTB() | ((mva & 0xfff00000UL) >> 18);
	// Get the first level descriptor
	// Two TLB are available, the lockdown TLB and the regular TLB
	// If the address is not found on the TLBs then perform a page walk
	// Check first the lockdown TLB and afterwards the regular TLB
	bool found_first_level = false;
	uint32_t first_level = 0;
	// bool found_second_level = false;
	// uint32_t second_level = 0;
	uint32_t way = 0;
	if ( ltlb.GetWay(ttb_addr, &way) )
	{
		if ( ltlb.GetValid(way) )
		{
			first_level = ltlb.GetData(way);
			found_first_level = true;
		}
	}
	if ( !found_first_level )
	{
		// the entry was not found in the lockdown tlb
		// check the main tlb
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		if ( tlb.GetWay(tag, set, &way) )
		{
			if ( tlb.GetValid(set, way) )
			{
				first_level = tlb.GetData(set, way);
				found_first_level = true;
			}
		}
	}
	if ( !found_first_level )
	{
		uint8_t first_level_data[4];
		// oops, we are forced to perform a page walk
		PrRead(ttb_addr, first_level_data, 4);
		memcpy(&first_level, first_level_data, 4);
		first_level = LittleEndian2Host(first_level);
	}
	// NOTE: we let the found_first_level to false to signal that the
	//   entry was found in the memory table
	// now we have to check that the first level entry obtained is valid
	if ( (first_level & 0x03) == 0 )
	{
		// this is an invalid entry, provoke a data abort
		logger << DebugError
			<< "Address translation for mva 0x"
			<< std::hex << mva << std::dec
			<< " was not found"
			<< EndDebugError;
		assert("Translation fault" == 0);
		return false;
	}

	// ok, we have a valid entry check if it needs to be added to the main tlb
	if ( !found_first_level)
	{
		// this is a correct entry fill the main tlb with it
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		uint32_t way = tlb.GetNewWay(set);
		tlb.SetEntry(tag, set, way, first_level, 1);
		logger << DebugInfo
			<< "New entry created with:" << std::endl
			<< "- tag = 0x"
			<< std::hex << tag << std::dec << std::endl
			<< "- set = " << set << std::endl
			<< "- way = " << way << std::endl
			<< "- data = 0x"
			<< std::hex << first_level << std::dec
			<< EndDebugInfo;
	}

	// check wether is a section page or a second access is required
	if ( first_level & 0x02UL )
	{
		// the descriptor is a section descriptor
		pa = (first_level & 0xfff00000UL) |
			(mva & 0x000fffffUL);
	}
	else
	{
		// the page is not a section a second access to the tlb is required
		assert("Translation fault (TODO: second level access)" == 0);
	}

	return true;
}

/** Non intrusive translate address from VA to MVA and physical address.
 *
 * @param va the virtual address to handle
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool
CPU::
NonIntrusiveTranslateVA(bool is_read, 
		uint32_t va, 
		uint32_t &mva, 
		uint32_t &pa,
		uint32_t &cacheable,
		uint32_t &bufferable)
{
	bool status = true;

	// transform the VA into MVA
	if ( (va & cp15.FCSE_PID_MASK) == 0 )
		mva = cp15.GetFCSE_PID() + va;
	else
		mva = va;
	if ( verbose & 0x020 )
	{
		logger << DebugWarning
			<< "Non intrusive access" << std::endl
			<< " - va = 0x" << std::hex << va 
			<< std::endl
			<< " - mva = 0x" << mva << std::dec
			<< EndDebugWarning;
	}
	// Build first the address of the Translation Table Base
	uint32_t ttb_addr = 
		cp15.GetTTB() | ((mva & 0xfff00000UL) >> 18);
	// Get the first level descriptor
	// Two TLB are available, the lockdown TLB and the regular TLB
	// If the address is not found on the TLBs then perform a page walk
	// Check first the lockdown TLB and afterwards the regular TLB
	bool found_first_level = false;
	uint32_t first_level = 0;
	bool found_second_level = false;
	uint32_t second_level = 0;
	uint32_t way = 0;
	if ( ltlb.GetWay(ttb_addr, &way) )
	{
		first_level = ltlb.GetData(way);
		found_first_level = true;
	}

	if ( !found_first_level )
	{
		if ( verbose & 0x020 )
		{
			logger << DebugWarning
				<< " - not found in the lookdown TLB"
				<< EndDebugWarning;
		}
		// the entry was not found in the lockdown tlb
		// check the main tlb
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		if ( tlb.GetWay(tag, set, &way) )
		{
			first_level = tlb.GetData(set, way);
			found_first_level = true;
		}
	}

	if ( !found_first_level )
	{
		if ( verbose & 0x020 )
		{
			logger << DebugWarning
				<< " - not found in the main TLB"
				<< EndDebugWarning;
		}
		uint8_t first_level_data[4];
		// oops, we are forced to perform a page walk
		status = ExternalReadMemory(ttb_addr,
				first_level_data, 4);
		if ( !status ) 
		{
			logger << DebugInfo
				<< "Could not read first level descriptor from memory"
				<< " during non intrusive access:" << std::endl
				<< " - ttb address = 0x" << std::hex << ttb_addr
				<< std::dec
				<< EndDebugInfo;
			return false;
		}
		memcpy(&first_level, first_level_data, 4);
		first_level = LittleEndian2Host(first_level);
	}

	// NOTE: we let the found_first_level to false to signal that the
	//   entry was found in the memory table
	// now we have to check that the first level entry obtained is valid
	if ( (first_level & 0x03) == 0 )
	{
		// this is an invalid entry, provoke a data abort
		logger << DebugError
			<< "Address translation for 0x"
			<< std::hex << va << std::dec
			<< " was not found (or returned a fault page)" << std::endl
			<< " - ttb address = 0x" << std::hex << ttb_addr << std::dec
			<< std::endl
			<< " - first level fetched = 0x" << std::hex
			<< first_level << std::dec
			<< EndDebugError;
		return false;
	}

	// check wether is a section page or a second access is required
	if ( (first_level & 0x03UL) == 0 )
	{
		// fault page
		logger << DebugError
			<< "First level descriptor fault when making a non intrusive"
			<< " access:" << std::endl
			<< " - ttb address = 0x" << std::hex << ttb_addr << std::dec
			<< std::endl
			<< " - first level fetched = 0x" << std::hex
			<< first_level << std::dec
			<< EndDebugError;
		return false;
	}

	else if ( (first_level & 0x03UL) == 0x01UL ) // coarse page
	{
		// a second level descriptor fetch is initiated
		// build the address of the 2nd level descriptor
		uint32_t coarse_addr =
			(first_level & 0xfffffc00UL) |
			((mva & 0x000ff000UL) >> 10);
		if ( verbose & 0x020 )
		{
			logger << DebugInfo
				<< "Non intrusive access to coarse page descriptor" << std::endl
				<< " - first level entry   = 0x" << std::hex
				<< first_level << std::endl
				<< " - mva                 = 0x" << mva << std::endl
				<< " - coarse page address = 0x" << coarse_addr << std::dec
				<< EndDebugInfo;
		}
		uint32_t second_way = 0;
		if ( ltlb.GetWay(coarse_addr, &second_way) )
		{
			second_level = ltlb.GetData(second_way);
			found_second_level = true;
		}
		if ( !found_second_level )
		{
			if ( verbose & 0x020 )
			{
				logger << DebugInfo
					<< "Non intrusive access, coarse page descriptor not found"
					<< " in the lockdown TLB"
					<< EndDebugInfo;
			}
			// the entry was not faound in the lockdown tlb
			// check the main tlb
			uint32_t second_tag = tlb.GetTag(coarse_addr);
			uint32_t second_set = tlb.GetSet(coarse_addr);
			if ( tlb.GetWay(second_tag, second_set, &second_way) )
			{
				second_level = tlb.GetData(second_set, second_way);
				found_second_level = true;
			}
		}
		if ( !found_second_level )
		{
			if ( verbose & 0x020 )
			{
				logger << DebugInfo
					<< "Non intrusive access, coarse page descriptor not found"
					<< " in the main TLB"
					<< EndDebugInfo;
			}
			uint32_t second_level_data[4];
			// oops, we are forced to perform a page walk
			status = ExternalReadMemory(coarse_addr,
					second_level_data, 4);
			if ( !status ) 
			{
				logger << DebugInfo
					<< "Could not read coarse page descriptor from memory"
					<< " during non intrusive access:" << std::endl
					<< " - address = 0x" << std::hex << coarse_addr
					<< std::dec
					<< EndDebugInfo;
				return false;
			}
			memcpy(&second_level, second_level_data, 4);
			second_level = LittleEndian2Host(second_level);
		}
		// NOTE: we let the found_second_level to false to signal that the entry
		//   was found in the memory table
		// now we can check that the second level entry obtained is valid
		if ( (second_level & 0x03UL) == 0 )
		{
			// this is an invalid entry
			logger << DebugError
				<< "Address translation for 0x" << std::hex << va << std::dec
				<< " was not found during non intrusive access" << std::endl
				<< " - mva = 0x" << std::hex << mva << std::endl
				<< " - first_level = 0x" << first_level << std::endl
				<< " - coarse page address = 0x" << coarse_addr << std::endl
				<< " - second_level = 0x" << second_level << std::dec
				<< EndDebugError;
			return false;
		}

		// check whether is a large, small or tiny page
		if ( (second_level & 0x03UL) == 0 )
		{
			// fault page
			logger << DebugError
				<< "Found faulty translation second level entry"
				<< "for address 0x" << std::hex << va << std::dec
				<< " was not found during non intrusive access" << std::endl
				<< " - mva = 0x" << std::hex << mva << std::endl
				<< " - first_level = 0x" << first_level << std::endl
				<< " - coarse page address = 0x" << coarse_addr << std::endl
				<< " - second_level = 0x" << second_level << std::dec
				<< EndDebugError;
			return false;
		}

		else if ( (second_level & 0x03UL) == 1 )
		{
			// large page
			pa = (second_level & 0xffff0000UL) |
				(mva & 0x0000ffffUL);
			assert("TODO:Large page descriptor" == 0);
		}

		else if ( (second_level & 0x03UL) == 2 )
		{
			// small page
			pa = (second_level & 0xfffff000UL) |
				(mva & 0x00000fffUL);
			uint32_t ap[4];
			ap[0] = (second_level >> 4) & 0x03UL;
			ap[1] = (second_level >> 6) & 0x03UL;
			ap[2] = (second_level >> 8) & 0x03UL;
			ap[3] = (second_level >> 10) & 0x03UL;
			uint32_t ap_used = 0;
			switch ( ((mva & 0x0c00UL) >> 10) & 0x03UL )
			{
				case 0x00UL: ap_used = ap[0]; break;
				case 0x01UL: ap_used = ap[1]; break;
				case 0x02UL: ap_used = ap[2]; break;
				case 0x03UL: ap_used = ap[3]; break;
			}
			uint32_t domain = (first_level >> 5) & 0x0fUL;
			uint32_t c = (second_level >> 3) & 0x01UL;
			cacheable = c;
			uint32_t b = (second_level >> 2) & 0x01UL;
			bufferable = b;
			if ( verbose & 0x020 )
				logger << DebugInfo
					<< "Address translated using a small page descriptor"
					<< " (non intrusive access):"
					<< std::endl
					<< " - current pc = 0x" << std::hex << GetGPR(PC_reg)
					<< std::endl
					<< " - va  = 0x" << va << std::endl
					<< " - mva = 0x" << mva << std::endl
					<< " - pa  = 0x" << pa << std::dec << std::endl
					<< " - ap[0-3] = " << ap[0]
					<< " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
					<< " - ap  = " << ap_used << std::endl
					<< " - domain = " << domain << std::endl
					<< " - c   = " << c << std::endl
					<< " - b   = " << b
					<< EndDebugInfo;
		}
	}

	else if ( (first_level & 0x02UL) == 0x02UL ) // section page
	{
		// the descriptor is a section descriptor
		pa = (first_level & 0xfff00000UL) |
			(mva & 0x000fffffUL);
		uint32_t ap = (first_level >> 10) & 0x03UL;
		uint32_t domain = (first_level >> 5) & 0x0fUL;
		uint32_t c = (first_level >> 3) & 0x01UL;
		cacheable = c;
		uint32_t b = (first_level >> 2) & 0x01UL;
		bufferable = b;
		if ( verbose & 0x020 )
			logger << DebugInfo
				<< "Address translated using a section descriptor:" << std::endl
				<< "- current pc = 0x" << std::hex << GetGPR(PC_reg)
				<< std::dec << std::endl
				<< "- va  = 0x" << std::hex << va << std::endl
				<< "- mva = 0x" << mva << std::endl
				<< "- pa  = 0x" << pa << std::dec << std::endl
				<< "- ap  = " << ap << std::endl
				<< "- domain = " << domain << std::endl
				<< "- c   = " << c << std::endl
				<< "- b   = " << b
				<< EndDebugInfo;
	}

	return true;
}

/** Translate address from VA to MVA and physical address.
 *
 * @param va the virtual address to handle
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool
CPU::
TranslateVA(bool is_read, 
		uint32_t va, 
		uint32_t &mva, 
		uint32_t &pa,
		uint32_t &cacheable,
		uint32_t &bufferable)
{
	// transform the VA into MVA
	if ( (va & cp15.FCSE_PID_MASK) == 0 )
		mva = cp15.GetFCSE_PID() + va;
	else
		mva = va;
	// Build first the address of the Translation Table Base
	uint32_t ttb_addr = 
		cp15.GetTTB() | ((mva & 0xfff00000UL) >> 18);
	if ( verbose & 0x010 )
	{
		logger << DebugInfo
			<< "va = 0x" << std::hex << va 
			<< std::endl
			<< " - mva = 0x" << mva << std::endl
			<< " - ttb = 0x" << cp15.GetTTB() << std::dec
			<< EndDebugInfo;
	}
	// Get the first level descriptor
	// Two TLB are available, the lockdown TLB and the regular TLB
	// If the address is not found on the TLBs then perform a page walk
	// Check first the lockdown TLB and afterwards the regular TLB
	bool found_first_level = false;
	uint32_t first_level = 0;
	bool found_second_level = false;
	uint32_t second_level = 0;
	uint32_t way = 0;
	if ( ltlb.GetWay(ttb_addr, &way) )
	{
		first_level = ltlb.GetData(way);
		found_first_level = true;
	}
	if ( !found_first_level )
	{
		if ( verbose & 0x010 )
		{
			logger << DebugInfo
				<< " - not found in the lockdown TLB"
				<< EndDebugInfo;
		}
		// the entry was not found in the lockdown tlb
		// check the main tlb
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		if ( tlb.GetWay(tag, set, &way) )
		{
			first_level = tlb.GetData(set, way);
			found_first_level = true;
		}
	}
	if ( !found_first_level )
	{
		if ( verbose & 0x010 )
		{
			logger << DebugInfo
				<< " - not found in the main TLB"
				<< EndDebugInfo;
		}
		uint8_t first_level_data[4];
		// oops, we are forced to perform a page walk
		PrRead(ttb_addr, first_level_data, 4);
		memcpy(&first_level, first_level_data, 4);
		first_level = LittleEndian2Host(first_level);
	}
	// NOTE: we let the found_first_level to false to signal that the
	//   entry was found in the memory table
	// now we have to check that the first level entry obtained is valid
	if ( (first_level & 0x03UL) == 0 )
	{
		// this is an invalid entry, provoke a data abort
		logger << DebugError
			<< "Address translation for 0x"
			<< std::hex << va << std::dec
			<< " was not found (or returned a fault page)" << std::endl
			<< " - ttb address = 0x" << std::hex << ttb_addr << std::dec
			<< std::endl
			<< " - first level fetched = 0x" << std::hex
			<< first_level << std::endl
			<< " - pc = 0x" << GetGPR(PC_reg) << std::dec
			<< EndDebugError;
		return false;
	}

	// ok, we have a valid entry check if it needs to be added to the main tlb
	if ( !found_first_level)
	{
		if ( verbose & 0x010 )
		{
			logger << DebugInfo
				<< " - filling main TLB with 0x" << std::hex
				<< first_level << std::dec
				<< EndDebugInfo;
		}
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		uint32_t way = tlb.GetNewWay(set);
		tlb.SetEntry(tag, set, way, first_level, 1);
	}

	// check wether is a section page or a second access is required
	if ( (first_level & 0x03UL) == 0 )
	{
		// fault page
		assert("Translation fault (TODO: handle first level descriptor fault)"
				== 0);
	}

	else if ( (first_level & 0x03UL) == 0x01UL ) // Coarse page
	{
		// a second level descriptor fetch is initiated
		// build the address of the 2nd level descriptor
		uint32_t coarse_addr =
			(first_level & 0xfffffc00UL) |
			((mva & 0x000ff000UL) >> 10);
		if ( verbose & 0x010)
		{
			logger << DebugInfo
				<< " - first level entry   = 0x" << std::hex
				<< first_level << std::endl
				<< " - mva                 = 0x" << mva << std::endl
				<< " - coarse page address = 0x"
				<< coarse_addr << std::dec
				<< EndDebugInfo;
		}
		uint32_t second_way = 0;
		if ( ltlb.GetWay(coarse_addr, &second_way) )
		{
			second_level = ltlb.GetData(second_way);
			found_second_level = true;
		}
		if ( !found_second_level )
		{
			if ( verbose & 0x010 )
			{
				logger << DebugInfo
					<< " - not found in the lockdown TLB"
					<< EndDebugInfo;
			}
			// the entry was not found in the lockdown tlb
			// check the main tlb
			uint32_t second_tag = tlb.GetTag(coarse_addr);
			uint32_t second_set = tlb.GetSet(coarse_addr);
			if ( tlb.GetWay(second_tag, second_set, &second_way) )
			{
				second_level = tlb.GetData(second_set, second_way);
				found_second_level = true;
			}
		}
		if ( !found_second_level )
		{
			if ( verbose & 0x010 )
			{
				logger << DebugInfo
					<< " - not found in the main TLB"
					<< EndDebugInfo;
			}
			uint8_t second_level_data[4];
			// oops, we are forced to perform a page walk
			PrRead(coarse_addr, second_level_data, 4);
			memcpy(&second_level, second_level_data, 4);
			second_level = LittleEndian2Host(second_level);
		}
		// NOTE: we let the found_second_level to false to signal that the
		//   entry was found in the memory table
		// now we can check that the second level entry obtained is valid
		if ( (second_level & 0x03UL) == 0 )
		{
			// this is an invalid entry, provoke a data abort
			logger << DebugError
				<< "Address translation for 0x"
				<< std::hex << va << std::dec
				<< " was not found" << std::endl
				<< " - mva = 0x" << std::hex << mva << std::endl
				<< " - first_level = 0x" << first_level << std::endl
				<< " - coarse page address = 0x" << coarse_addr << std::endl
				<< " - second_level = 0x" << second_level
				<< std::dec
				<< EndDebugError;
			assert("Translation fault (TODO: data abort)" == 0);
			return false;
		}

		// ok, we have a valid entry check if it needs to be added to 
		//   the main tlb
		if ( !found_second_level )
		{
			if ( verbose & 0x010 )
			{
				logger << DebugInfo
					<< " - filling main TLB with 0x" << std::hex
					<< second_level << std::dec
					<< " (second level entry)"
					<< EndDebugInfo;
			}
			uint32_t second_tag = tlb.GetTag(coarse_addr);
			uint32_t second_set = tlb.GetSet(coarse_addr);
			uint32_t second_way = tlb.GetNewWay(second_set);
			tlb.SetEntry(second_tag, second_set, second_way, second_level, 1);
		}

		// check whether is a large, small or tiny page
		if ( (second_level & 0x03UL) == 0 )
		{
			// fault page
			assert("Translation fault (TODO: handle second level descriptor"
					" fault)" == 0);
		}

		else if ( (second_level & 0x03UL) == 1 )
		{
			// large page
			pa = (second_level & 0xffff0000UL) |
				(mva & 0x0000ffffUL);
			assert("TODO: Large page descriptor" == 0);
		}

		else if ( (second_level & 0x03UL) == 2 )
		{
			// small page
			pa = (second_level & 0xfffff000UL ) |
				(mva & 0x00000fffUL);
			uint32_t ap[4];
			ap[0] = (second_level >> 4) & 0x03UL;
			ap[1] = (second_level >> 6) & 0x03UL;
			ap[2] = (second_level >> 8) & 0x03UL;
			ap[3] = (second_level >> 10) & 0x03UL;
			uint32_t ap_used = 0;
			switch ( ((mva & 0x0c00UL) >> 10) & 0x03UL )
			{
				case 0x00UL: ap_used = ap[0]; break;
				case 0x01UL: ap_used = ap[1]; break;
				case 0x02UL: ap_used = ap[2]; break;
				case 0x03UL: ap_used = ap[3]; break;
			}
			uint32_t domain = (first_level >> 5) & 0x0fUL;
			uint32_t c = (second_level >> 3) & 0x01UL;
			cacheable = c;
			uint32_t b = (second_level >> 2) & 0x01UL;
			bufferable = b;
			if ( verbose & 0x010 )
				logger << DebugInfo
					<< "Address translated using a small page descriptor:"
					<< std::endl
					<< " - current pc = 0x" << std::hex << GetGPR(PC_reg)
					<< std::endl
					<< " - va  = 0x" << va << std::endl
					<< " - mva = 0x" << mva << std::endl
					<< " - pa  = 0x" << pa << std::dec << std::endl
					<< " - ap[0-3] = " << ap[0]
					<< " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
					<< " - ap  = " << ap_used << std::endl
					<< " - domain = " << domain << std::endl
					<< " - c   = " << c << std::endl
					<< " - b   = " << b
					<< EndDebugInfo;

			// check access permissions
			if ( unlikely(!CheckAccessPermission(is_read,
							ap_used, domain)) )
			{
				logger << DebugError
					<< "Check access permission and domain"
					<< std::endl
					<< " - current pc = 0x" << std::hex << GetGPR(PC_reg)
					<< " - va  = 0x" << va << std::endl
					<< " - mva = 0x" << mva << std::endl
					<< " - pa  = 0x" << pa << std::dec << std::endl
					<< " - ap[0-3] = " << ap[0]
					<< " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
					<< " - ap  = " << ap_used << std::endl
					<< " - domain = " << domain << std::endl
					<< " - c   = " << c << std::endl
					<< " - b   = " << b
					<< EndDebugError;
				assert("TODO: check access permission and domain" == 0);
			}

		}

		else // 3 tiny page
		{
			// tiny page
			assert("TODO: Tiny page descriptor" == 0);
		}
	}
	
	else if ( (first_level & 0x03UL) == 0x02UL ) // Section page
	{
		// the descriptor is a section descriptor
		pa = (first_level & 0xfff00000UL) |
			(mva & 0x000fffffUL);
		uint32_t ap = (first_level >> 10) & 0x03UL;
		uint32_t domain = (first_level >> 5) & 0x0fUL;
		uint32_t c = (first_level >> 3) & 0x01UL;
		cacheable = c;
		uint32_t b = (first_level >> 2) & 0x01UL;
		bufferable = b;
		if ( verbose & 0x010 )
			logger << DebugInfo
				<< "Address translated using a section descriptor:" << std::endl
				<< "- current pc = 0x" << std::hex << GetGPR(PC_reg)
				<< std::dec << std::endl
				<< "- va  = 0x" << std::hex << va << std::endl
				<< "- mva = 0x" << mva << std::endl
				<< "- pa  = 0x" << pa << std::dec << std::endl
				<< "- ap  = " << ap << std::endl
				<< "- domain = " << domain << std::endl
				<< "- c   = " << c << std::endl
				<< "- b   = " << b
				<< EndDebugInfo;

		// check the access permissions - domain couple
		//  if ap == 3 then no need to check anything else,
		//    read/write access are allowed
		if ( unlikely(!CheckAccessPermission(is_read,
						ap, domain)) )
		{
			logger << DebugError
				<< "Check access permission and domain"
				<< std::endl
				<< " - current pc = 0x" << std::hex << GetGPR(PC_reg)
				<< " - va  = 0x" << va << std::endl
				<< " - mva = 0x" << mva << std::endl
				<< " - pa  = 0x" << pa << std::dec << std::endl
				<< " - ap  = " << ap << std::endl
				<< " - domain = " << domain << std::endl
				<< " - c   = " << c << std::endl
				<< " - b   = " << b
				<< EndDebugError;
			assert("TODO: check access permission and domain" == 0);
		}
	}

	else // 0x03UL fine page table
	{
		assert("Translation fault (TODO: fine page table)" == 0);
	}

	return true;
}

/************************************************************************/
/* Exception handling                                             START */
/************************************************************************/
	
/** Process exceptions
 *
 * Returns true if there is an exception to handle.
 * @return true if an exception handling begins
 */
bool
CPU::
HandleException()
{
	bool handled = false;
	bool report = false;

	// Exception priorities (from higher to lower)
	// - 1 Reset
	// - 2 Data Abort (including data TLB miss)
	// - 3 FIQ
	// - 4 IRQ
	// - 5 Imprecise Abort (external abort) - ARMv6 (so ignored here)
	// - 6 Prefetch Abort (including prefetch TLB miss)
	// - 7 Undefined instruction / SWI
	
	if ( exception & 
			unisim::component::cxx::processor::arm::exception::RESET )
	{
	}

	else if ( exception & 
			unisim::component::cxx::processor::arm::exception::DATA_ABORT )
	{
	}

	else if ( (exception &
			unisim::component::cxx::processor::arm::exception::FIQ) &&
			!GetCPSR_F() )
	{
		report = true;
	}

	else if ( (exception & 
			unisim::component::cxx::processor::arm::exception::IRQ) &&
			!GetCPSR_I() )
	{
		report = true;
		logger << DebugInfo
			<< "Received IRQ interrupt, handling it."
			<< EndDebugInfo;
		handled = true;
		uint32_t cur_pc = GetGPR(15);
		spsr[3] = cpsr;
		SetGPRMapping(GetCPSR_Mode(), IRQ_MODE);
		SetGPR(14, cur_pc + 4);
		SetCPSR_Mode(IRQ_MODE);
		SetCPSR_T(false);
		SetCPSR_I(true);
		if ( cp15.GetVINITHI() )
			SetGPR(PC_reg, 0xffff0018UL);
		else
			SetGPR(PC_reg, 0x00000018UL);
	}
	
	else if ( exception &
			unisim::component::cxx::processor::arm::exception::PREFETCH_ABORT )
	{
	}
	
	else if ( exception &
			unisim::component::cxx::processor::arm::exception::SWI )
	{
	}

	else if ( exception &
			unisim::component::cxx::processor::arm::exception::UNDEFINED_INSN )
	{
	}

	if ( !handled )
	{
		if ( (exception &
					unisim::component::cxx::processor::arm::exception::FIQ) &&
				GetCPSR_F() )
			handled = true;

		if ( (exception & 
					unisim::component::cxx::processor::arm::exception::IRQ) &&
				GetCPSR_I() )
			handled = true;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Exception not handled (" << (unsigned int)exception << ")" 
			<< std::endl
			<< " - CPSR = 0x" << std::hex << cpsr << std::dec
			<< " - irq? = " 
			<< (exception & 
					unisim::component::cxx::processor::arm::exception::IRQ)
			<< std::endl
			<< " - CPSR_I = " << GetCPSR_I()
			<< EndDebugError;
		unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
	}

	return report;
}

/************************************************************************/
/* Exception handling                                               END */
/************************************************************************/

/** Performs the load/stores present in the queue of memory operations.
 */
void 
CPU::
PerformLoadStoreAccesses()
{
	// while the ls_queue is not empty process entries
	while (!ls_queue.empty()) 
	{
		MemoryOp *memop = ls_queue.front();
		ls_queue.pop();
		switch (memop->GetType()) 
		{
			case MemoryOp::PREFETCH:
				PerformPrefetchAccess(memop);
				break;
			case MemoryOp::WRITE:
				PerformWriteAccess(memop);
				break;
			case MemoryOp::READ:
				PerformReadAccess(memop);
				break;
			case MemoryOp::READ_TO_PC_UPDATE_T:
				PerformReadToPCUpdateTAccess(memop);
				break;
			case MemoryOp::READ_TO_PC:
				PerformReadToPCAccess(memop);
				break;
		}
		free_ls_queue.push(memop);
	}
}

/** Performs a prefetch access.
 *
 * @param memop the memory operation containing the prefetch access
 */
void 
CPU::
PerformPrefetchAccess(unisim::component::cxx::processor::arm::MemoryOp
		*memop)
{
	uint32_t va = memop->GetAddress();
	uint32_t mva = va;
	uint32_t pa = mva;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing prefetch with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec 
			<< EndDebugInfo;

	if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Prefetch not performed because there is no translation"
					<< " for the requested address (0x" << std::hex
					<< va << std::dec << ")"
					<< EndDebugInfo;
			// no translation found for the given address, do nothing
			return;
		}

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable) )
	{
		dcache.prefetch_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Cache prefetch hit"
						<< EndDebugInfo;
			}
		}
		// if the access was a miss, data needs to be fetched from main
		//   memory and placed into the cache
		if ( unlikely(!cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache prefetch miss"
					<< EndDebugInfo;

			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			uint32_t cache_valid = dcache.GetValid(cache_set,
					cache_way);
			uint32_t cache_dirty = dcache.GetDirty(cache_set,
					cache_way);
			if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t mva_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				uint32_t pa_cache_address = mva_cache_address;
				// translate the address
				if ( likely(cp15.IsMMUEnabled()) )
					TranslateMVA(mva_cache_address,
							pa_cache_address);
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Replace cache line with:" << std::endl
						<< " - mva = 0x" << std::hex << mva_cache_address
						<< std::endl
						<< " - pa = 0x" << pa_cache_address << std::dec
						<< EndDebugInfo;
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(pa_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Fetching cache line with:" << std::endl
					<< " - pa (base_address) = 0x" << std::hex << cache_address
					<< std::endl
					<< " - set = " << std::dec << cache_set << std::endl
					<< " - way = " << cache_way
					<< EndDebugInfo;
			uint32_t cache_line_size = dcache.GetData(cache_set,
					cache_way, &cache_data);
			PrRead(cache_address, cache_data,
					cache_line_size);
			dcache.SetTag(cache_set, cache_way, cache_tag);
			dcache.SetValid(cache_set, cache_way, 1);
			dcache.SetDirty(cache_set, cache_way, 0);
		}
		else
		{
			dcache.prefetch_hits++;
		}
		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportReadAccess();
	}
	else
	{
		/* it is just a cache prefetch, ignore the request if there is 
		 * no cache */
	}
	/* CHECK: should we report a memory access for a prefetch???? */
}

/** Performs a write access.
 * @param memop the memory operation containing the write access
 */
void 
CPU::
PerformWriteAccess(unisim::component::cxx::processor::arm::MemoryOp
		*memop)
{
	uint32_t va = memop->GetAddress();
	uint32_t mva = va;
	uint32_t pa = va;
	uint32_t size = memop->GetSize();
	uint8_t val8 = 0;
	uint16_t val16 = 0;
	uint32_t val32 = 0;
	uint8_t data8, data16[2], data32[4];
	uint8_t *data;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing write with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec << std::endl
			<< "- size = " << size
			<< EndDebugInfo;

	data8 = 0;
	memset(data16, 0, sizeof(uint8_t) * 2);
	memset(data32, 0, sizeof(uint8_t) * 4);
	// fix the write address depending on the request size and endianess
	//   and prepare the data to write
	switch(size)
	{
	case 1:
		va = va ^ munged_address_mask8;
		val8 = (uint8_t)memop->GetWriteValue();
		memcpy(&data8, &val8, 1);
		data = &data8;
		break;
	case 2:
		va = va ^ munged_address_mask16;
		val16 = (uint16_t)memop->GetWriteValue();
		val16 = Host2Target(GetEndianness(), val16);
		memcpy(data16, &val16, 2);
		data = data16;
		break;
	case 4:
		val32 = memop->GetWriteValue();
		val32 = Host2Target(GetEndianness(), val32);
		memcpy(data32, &val32, 4);
		data = data32;
		break;
	default: // should never happen
		break;
	}

	if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(false, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing write:"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - pc = 0x" << std::hex << GetGPR(PC_reg) << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}

	if ( unlikely(verbose & 0x02) )
	{
		logger << DebugInfo
			<< "Writing into 0x" << std::hex << pa
			<< " with data = "
			<< (unsigned int)data[0]
			<< " " << (unsigned int)data[1]
			<< " " << (unsigned int)data[2]
			<< " " << (unsigned int)data[3]
			<< std::dec;
		if ( cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable )
			logger << std::endl
				<< " - line cacheable"
				<< EndDebugInfo;
		else
			logger << EndDebugInfo;
	}

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable) )
	{
		dcache.write_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit
				cache_hit = true;
			}
			else
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Way found but not valid (way = "
						<< cache_way << ")"
						<< EndDebugInfo;
		}
		if ( likely(cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache hit (set = "
					<< cache_set << ", way = "
					<< cache_way << ")"
					<< EndDebugInfo;
			dcache.write_hits++;
			uint32_t cache_index = dcache.GetIndex(mva);
			dcache.SetData(cache_set, cache_way, cache_index,
					size, data);
			dcache.SetDirty(cache_set, cache_way, 1);
			// if the access is not bufferable the data has to be written to the
			//   memory system like a write through cache
			if ( bufferable == 0 )
			{
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Seding to memory"
						<< EndDebugInfo;
				PrWrite(pa, data, size);
			}
		}
		else
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache miss"
					<< EndDebugInfo;
			if ( bufferable )
			{
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Bufferable"
						<< EndDebugInfo;
				// the cache act as a write back cache, so the line is fetched
				//   into cache and modified in the cache but not sent into
				//   memory
				// get a way to replace
				cache_way = dcache.GetNewWay(cache_set);
				// get the valid and dirty bits from the way to replace
				uint8_t cache_valid = dcache.GetValid(cache_set,
						cache_way);
				uint8_t cache_dirty = dcache.GetDirty(cache_set,
						cache_way);

				if ( (cache_valid != 0) && (cache_dirty != 0) )
				{
					// the cache line to replace is valid and dirty so it needs
					//   to be sent to the main memory
					uint8_t *rep_cache_data = 0;
					uint32_t mva_cache_address =
						dcache.GetBaseAddress(cache_set, cache_way);
					dcache.GetData(cache_set, cache_way, &rep_cache_data);
					uint32_t pa_cache_address = mva_cache_address;
					// translate the address
					if ( likely(cp15.IsMMUEnabled()) )
						TranslateMVA(mva_cache_address,
								pa_cache_address);
					if ( unlikely(verbose & 0x02) )
					{
						uint32_t rep_tag = dcache.GetTag(cache_set, cache_way);
						logger << DebugInfo
							<< "Evicting cache line:" << std::endl
							<< " - mva = 0x" << std::hex << mva_cache_address << std::endl
							<< " - pa  = 0x" << pa_cache_address << std::endl
							<< " - tag = 0x" << rep_tag << std::endl
							<< " - set = " << std::dec << cache_set << std::endl
							<< " - way = " << cache_way << std::hex << std::endl
							<< " - data =";
						for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
							logger << " " << (unsigned int)rep_cache_data[i];
						logger << std::dec
							<< EndDebugInfo;
					}
					PrWrite(pa_cache_address, rep_cache_data, 
							dcache.LINE_SIZE);
				}
				// the new data can be requested
				uint8_t *cache_data = 0;
				uint8_t prev_data[64];
				uint32_t cache_address =
						dcache.GetBaseAddressFromAddress(pa);
				// when getting the data we get the pointer to the cache line
				//   containing the data, so no need to write the cache
				//   afterwards
				uint32_t cache_line_size = dcache.GetData(cache_set,
						cache_way, &cache_data);
				PrRead(cache_address, cache_data,
						cache_line_size);
				memcpy(prev_data, cache_data, cache_line_size);
				dcache.SetTag(cache_set, cache_way, cache_tag);
				dcache.SetValid(cache_set, cache_way, 1);
				uint32_t cache_index = dcache.GetIndex(mva);
				dcache.SetData(cache_set, cache_way, cache_index,
						size, data);
				dcache.SetDirty(cache_set, cache_way, 1);
				if ( unlikely(verbose & 0x02) )
				{
					cache_line_size = dcache.GetData(cache_set,
							cache_way, &cache_data);
					logger << DebugInfo
						<< "Written line:" << std::endl
						<< " - mva = 0x" << std::hex << mva << std::dec << std::endl
						<< " - pa  = 0x" << std::hex << pa << std::dec << std::endl
						<< " - tag = 0x" << std::hex << cache_tag << std::dec << std::endl
						<< " - set = " << cache_set << std::endl
						<< " - way = " << cache_way << std::endl
						<< " - index = " << cache_index << std::endl
						<< " - size = " << size << std::endl
						<< " - valid = " << (uint32_t)dcache.GetValid(cache_set, cache_way) << std::endl
						<< " - dirty = " << (uint32_t)dcache.GetDirty(cache_set, cache_way) << std::endl
						<< " - data =" << std::hex;
					for ( unsigned int i = 0; i < size; i++ )
						logger << " " << (unsigned int)data[i];
					logger << std::endl
						<< " - pre line =";
					for ( unsigned int i = 0; i < cache_line_size; i++ )
						logger << " " << (unsigned int)prev_data[i];
					logger << std::endl
						<< " - new line =";
					for ( unsigned int i = 0; i < cache_line_size; i++ )
						logger << " " << (unsigned int)cache_data[i];
					logger << std::dec
						<< EndDebugInfo;
				}
			}
			else
				PrWrite(pa, data, size);
		}

		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportWriteAccess();
	}
	else // there is no data cache
	{
		if ( unlikely(verbose & 0x02) )
			logger << DebugInfo
				<< "No cache, sending to memory"
				<< EndDebugInfo;
		// there is no data cache, so just send the request to the
		//   memory interface
		PrWrite(pa, data, size);
	}

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_WRITE,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					memop->GetAddress(), size);
}

/** Performs a read access.
 * @param memop the memory operation containing the read access
 */
void 
CPU::
PerformReadAccess(unisim::component::cxx::processor::arm::MemoryOp
		*memop)
{
	uint32_t size = memop->GetSize();
	uint32_t va = memop->GetAddress() & ~(uint32_t)(size - 1);
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;
	uint8_t data32[4];
	uint8_t *data;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing read with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec << std::endl
			<< "- size = " << size
			<< EndDebugInfo;

	// fix the read address depending on the request size and endianess
	switch (size)
	{
		case 1: va = va ^ munged_address_mask8; break;
		case 2: va = va ^ munged_address_mask16; break;
		case 4: // nothing to do
		default: // nothing to do
			break;
	}
	uint32_t mva = va;
	uint32_t pa = mva;

	if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing read:"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - pc = 0x" << std::hex << GetGPR(PC_reg) << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Read address translated:" << std::endl
			<< " - va = 0x" << std::hex << va << std::endl
			<< " - mva = 0x" << mva << std::endl
			<< " - pa = 0x" << pa << std::dec << std::endl
			<< " - cacheable = " << cacheable << std::endl
			<< " - bufferable = " << bufferable
			<< EndDebugInfo;

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Cache read hit"
						<< EndDebugInfo;
			}
		}
		// if the access was a miss, data needs to be fetched from main
		//   memory and placed into the cache
		if ( unlikely(!cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache read miss"
					<< EndDebugInfo;
			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			uint8_t cache_valid = dcache.GetValid(cache_set,
					cache_way);
			uint8_t cache_dirty = dcache.GetDirty(cache_set,
					cache_way);

			if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t mva_cache_address =
					dcache.GetBaseAddress(cache_set, cache_way);
				uint32_t pa_cache_address = mva_cache_address;
				// translate the address
				if ( likely(cp15.IsMMUEnabled()) )
					TranslateMVA(mva_cache_address,
							pa_cache_address);
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Replacing cache line with:" << std::endl
						<< " - mva = 0x" << std::hex << mva_cache_address 
						<< std::endl
						<< " - pa = 0x" << pa_cache_address << std::dec
						<< EndDebugInfo;
				dcache.GetData(cache_set, cache_way, &rep_cache_data);
				PrWrite(pa_cache_address, rep_cache_data,
							dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Fetching cache line with:" << std::endl
					<< " - pa (base_address) = 0x" << std::hex << cache_address
					<< std::endl
					<< " - set = " << std::dec << cache_set << std::endl
					<< " - way = " << cache_way
					<< EndDebugInfo;
			uint32_t cache_line_size = dcache.GetData(cache_set,
					cache_way, &cache_data);
			PrRead(cache_address, cache_data,
					cache_line_size);
			dcache.SetTag(cache_set, cache_way, cache_tag);
			dcache.SetValid(cache_set, cache_way, 1);
			dcache.SetDirty(cache_set, cache_way, 0);
		}
		else
		{
			// cache hit
			dcache.read_hits++;
		}

		// at this point the data is in the cache, we can read it from the
		//   cache
		uint32_t cache_index = dcache.GetIndex(mva);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else // there is no data cache
	{
		if ( unlikely(verbose & 0x02) )
			logger << DebugInfo
				<< "Fetching data from memory with:" << std::endl
				<< " - pa = 0x" << std::hex << pa << std::dec << std::endl
				<< " - size = " << size
				<< EndDebugInfo;
		// just read the data from the memory system
		PrRead(pa, data32, size);
		data = data32;
	}

	// fix the data depending on its size
	uint32_t value;
	if (size == 1)
	{
		uint8_t val8 = *data;
		if (memop->IsSigned())
			value = (int32_t)(int8_t)val8;
		else
			value = val8;
	}
	else if (size == 2)
	{
		uint16_t val16 = 0;
		memcpy(&val16, data, 2);
		val16 = Target2Host(GetEndianness(), val16);
		if (memop->IsSigned())
			value = (int32_t)(int16_t)val16;
		else
			value = val16;
	}
	else if (size == 4)
	{
		uint32_t val32;
		uint32_t val32_l, val32_r;
		uint32_t align;

		memcpy(&val32, data, 4);
		val32 = Target2Host(GetEndianness(), val32);
		// we need to check alignment
		align = memop->GetAddress() & (uint32_t)0x03;
		if (align != 0)
		{
			val32_l = (val32 << (align*8)) &
					((~((uint32_t)0)) << (align*8));
			val32_r = (val32 >> ((4 - align) * 8)) &
					((~((uint32_t)0)) >> ((4 - align) * 8));
			val32 = val32_l + val32_r;
		}
		value = val32;
	}

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Finished performing read access, setting register with:" 
			<< std::endl
			<< " - register = " << memop->GetTargetReg() << std::endl
			<< " - value = 0x" << std::hex << value << std::dec
			<< EndDebugInfo;

	SetGPR(memop->GetTargetReg(), value);

	if ( likely(dcache.GetSize()) )
		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					memop->GetAddress(), size);
}

/** Performs a read access and puts result in the PC register.
 * @param memop the memory operation containing the read access
 */
void 
CPU::
PerformReadToPCAccess(unisim::component::cxx::processor::arm::MemoryOp
		*memop)
{
	uint32_t va = memop->GetAddress() & ~(uint32_t)0x03;
	uint32_t mva = va;
	uint32_t pa = mva;
	const uint32_t size = 4;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;
	uint8_t data32[4];
	uint8_t *data;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing read to PC with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec << std::endl
			<< "- size = " << size
			<< EndDebugInfo;

	if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing read to PC:"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - pc = 0x" << std::hex << GetGPR(PC_reg) << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;

		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
			}
		}
		// if the access was a miss, data needs to be fetched from main
		//   memory and placed into the cache
		if ( unlikely(!cache_hit) )
		{
			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			uint8_t cache_valid = dcache.GetValid(cache_set,
					cache_way);
			uint8_t cache_dirty = dcache.GetDirty(cache_set,
					cache_way);

			if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t mva_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				uint32_t pa_cache_address = mva_cache_address;
				// translate the address
				if ( likely(cp15.IsMMUEnabled()) )
					TranslateMVA(mva_cache_address,
							pa_cache_address);
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(pa_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			uint32_t cache_line_size = dcache.GetData(cache_set,
					cache_way, &cache_data);
			PrRead(cache_address, cache_data,
					cache_line_size);
			dcache.SetTag(cache_set, cache_way, cache_tag);
			dcache.SetValid(cache_set, cache_way, 1);
			dcache.SetDirty(cache_set, cache_way, 0);
		}
		else
		{
			// cache hit
			dcache.read_hits++;
		}

		// at this point the data is in the cache, we can read it from the
		//   cache
		uint32_t cache_index = dcache.GetIndex(mva);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else
	{
		PrRead(pa, data32, size);
		data = data32;
	}
	// fix the data depending on its size
	uint32_t value;
	uint32_t val32;
	uint32_t val32_l, val32_r;
	uint32_t align;

	memcpy(&val32, data, sizeof(uint32_t));
	val32 = Target2Host(GetEndianness(), val32);
	// we need to check alignment
	align = memop->GetAddress() & 0x03;
	if (align != 0)
	{
		val32_l = (val32 << (align*8)) &
				((~((uint32_t)0)) << (align*8));
		val32_r = (val32 >> ((4 - align) * 8)) &
				((~((uint32_t)0)) >> ((4 - align) * 8));
		val32 = val32_l + val32_r;
	}
	value = val32;

	SetGPR(PC_reg, value & ~(uint32_t)0x01);

	if ( unlikely(dcache.power_estimator_import != 0) )
		dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					memop->GetAddress(), size);
}

/** Performs a read access and puts result in the PC register.
 * Performs a read access and puts result in the PC register and updates the
 * thumb status if necessary
 * 
 * @param memop the memory operation containing the read access
 */
void 
CPU::
PerformReadToPCUpdateTAccess(
		unisim::component::cxx::processor::arm::MemoryOp *memop)
{
	uint32_t va = memop->GetAddress() & ~(uint32_t)0x03;
	uint32_t mva = va;
	uint32_t pa = mva;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;
	const uint32_t size = 4;
	uint8_t data32[4];
	uint8_t *data;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing read to PC update T with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec << std::endl
			<< "- size = " << size
			<< EndDebugInfo;

	if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing read to PC with T update:"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - pc = 0x" << std::hex << GetGPR(PC_reg) << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}

	if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;

		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
			}
		}
		// if the access was a miss, data needs to be fetched from main
		//   memory and placed into the cache
		if ( unlikely(!cache_hit) )
		{
			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			uint8_t cache_valid = dcache.GetValid(cache_set,
					cache_way);
			uint8_t cache_dirty = dcache.GetDirty(cache_set,
					cache_way);
			if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t mva_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				uint32_t pa_cache_address = mva_cache_address;
				// translate the address
				if ( likely(cp15.IsMMUEnabled()) )
					TranslateMVA(mva_cache_address,
							pa_cache_address);
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(pa_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			uint32_t cache_line_size = dcache.GetData(cache_set,
					cache_way, &cache_data);
			PrRead(cache_address, cache_data,
					cache_line_size);
			dcache.SetTag(cache_set, cache_way, cache_tag);
			dcache.SetValid(cache_set, cache_way, 1);
			dcache.SetDirty(cache_set, cache_way, 0);
		}
		else
		{
			// cache hit
			dcache.read_hits++;
		}

		// at this point the data is in the cache, we can read it from the
		//   cache
		uint32_t cache_index = dcache.GetIndex(mva);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else // there is no data cache
	{
		PrRead(pa, data32, size);
		data = data32;
	}

	// fix the data depending on its size
	uint32_t value;
	uint32_t val32;
	uint32_t val32_l, val32_r;
	uint32_t align;

	memcpy(&val32, data, sizeof(uint32_t));
	val32 = Target2Host(GetEndianness(), val32);
	// we need to check alignment
	align = memop->GetAddress() & 0x03;
	if (align != 0)
	{
		val32_l = (val32 << (align*8)) &
				((~((uint32_t)0)) << (align*8));
		val32_r = (val32 >> ((4 - align) * 8)) &
				((~((uint32_t)0)) >> ((4 - align) * 8));
		val32 = val32_l + val32_r;
	}
	value = val32;

	SetGPR(PC_reg, value & ~(uint32_t)0x01);
	SetCPSR_T((value & (uint32_t)0x01) == 0x01);

	if ( unlikely(dcache.power_estimator_import != 0) )
		dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					MemoryAccessReporting<uint64_t>::MAT_READ,
					MemoryAccessReporting<uint64_t>::MT_DATA,
					memop->GetAddress(), size);
}

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
