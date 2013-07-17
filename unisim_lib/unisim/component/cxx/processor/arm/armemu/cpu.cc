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
#include "unisim/component/cxx/processor/arm/armemu/cpu.hh"
#include <unisim/kernel/debug/debug.hh>

#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <sstream>
#include <string>

#include "unisim/component/cxx/processor/arm/armemu/cache.hh"
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/masks.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/likely/likely.hh"

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

// #define ARM_OPTIMIZATION

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace armemu {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::service::interfaces::LinuxOS;
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
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using std::string;
using std::stringstream;

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
	, Client<LinuxOS>(name, parent)
	, Service<MemoryInjection<uint32_t> >(name, parent)
	, Client<DebugControl<uint32_t> >(name, parent)
	, Client<MemoryAccessReporting<uint32_t> >(name, parent)
	, Client<TrapReporting>(name, parent)
	, Service<MemoryAccessReportingControl>(name, parent)
	, Service<Disassembly<uint32_t> >(name, parent)
	, Service<Registers>(name, parent)
	, Service<Memory<uint32_t> >(name, parent)
	, disasm_export("disasm-export", this)
	, registers_export("registers-export", this)
	, memory_injection_export("memory-injection-export", this)
	, memory_export("memory-export", this)
	, memory_access_reporting_control_export(
			"memory-access-reporting-control-export", this)
	, debug_control_import("debug-control-import", this)
	, memory_access_reporting_import("memory-access-reporting-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, linux_os_import("linux-os-import", this)
	, instruction_counter_trap_reporting_import(
			"instruction-counter-trap-reporting-import", this)
	, logger(*this)
	, icache("icache", this)
	, dcache("dcache", this)
	, arm32_decoder()
	, instruction_counter(0)
	, voltage(0)
	, verbose(0)
	, trap_on_instruction_counter(0)
	, default_endianness_string(default_endianness == E_BIG_ENDIAN ? 
			"big-endian" : "little-endian")
	, requires_memory_access_reporting(true)
	, requires_finished_instruction_reporting(true)
	, param_default_endianness("default-endianness", this,
			default_endianness_string,
			"The processor default/boot endianness. Available values are: "
			"little-endian and big-endian.")
	, param_cpu_cycle_time_ps("cpu-cycle-time-ps", this,
			cpu_cycle_time_ps,
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
	, stat_instruction_counter("instruction-counter", this,
			instruction_counter,
			"Number of instructions executed.")
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
	for (unsigned int i = 0; i < (num_log_gprs - 1); i++)
	{
		stringstream ss;
		stringstream ss_desc;
		ss << "GPR[" << i << "]";
		ss_desc << "Logical register " << i;
		reg_gpr[i] = 
			new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), 
					this, gpr[i], ss_desc.str().c_str());
	}
        reg_gpr[15] = new unisim::kernel::service::Register<uint32_t>("GPR[15]", this, pc, "Logical register 15");
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

	// This implementation of the arm architecture can only run in user mode,
	//   so we can already set CPSR to that mode.
	SetCPSR_Mode(USER_MODE);

	// Set the right format for various of the variables
	param_cpu_cycle_time_ps.SetFormat(
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
EndSetup()
{
	if (verbose)
		logger << DebugInfo
			<< "Verbose activated."
			<< EndDebugInfo;
	
	/* check that the linux_os_import is connected, otherwise setup fails
	 * NOTE: is not that the setup fails, it is that actually this implmentation
	 *   is not supposed to work without the linux_os_import, so better to stop 
	 *   now than later.
	 */
	if ( !linux_os_import )
	{
		logger << DebugError
			<< "The connection to the Linux OS (linux_import) is broken or has "
			<< "not been done."
			<< EndDebugError;
		return false;
	}
	/* fix the endianness depending on the endianness parameter */
	if ( (default_endianness_string.compare("little-endian") != 0) &&
			(default_endianness_string.compare("big-endian") != 0) )
	{
		logger << DebugError
			<< "Error while setting the default endianness."
			<< " '" << default_endianness_string << "' is not a correct"
			<< " value."
			<< " Available values are: little-endian and big-endian."
			<< EndDebugError;
		return false;
	}
	else
	{
		if ( verbose )
			logger << DebugInfo
				<< "Setting endianness to "
				<< default_endianness_string
				<< EndDebugInfo;
		SetEndianness(
				default_endianness_string.compare("little-endian") == 0 ?
				E_LITTLE_ENDIAN : E_BIG_ENDIAN);
	}

	if ( cpu_cycle_time_ps == 0 )
	{
		// we can't provide a valid cpu cycle time configuration
		//   automatically
		logger << DebugError
			<< "cpu-cycle-time-ps should be bigger than 0"
			<< EndDebugError;
		return false;
	}

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
		if ( cpu_cycle_time_ps < min_cycle_time )
		{
			logger << DebugWarning;
			logger << "A cycle time of " << cpu_cycle_time_ps
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
		icache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);
	if ( dcache.power_mode_import )
		dcache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);

	if ( verbose )
	{
		logger << DebugInfo
			<< "Setting cpu cycle time to "
			<< cpu_cycle_time_ps
			<< " ps."
			<< EndDebugInfo;
		logger << DebugInfo
			<< "Setting cpu voltage to "
			<< voltage
			<< " mV."
			<< EndDebugInfo;
	}
			
	/* TODO: Remove this section once all the debuggers use the unisim
	 *   kernel interface for registers.
	 */
	/* setting debugging registers */
	if (verbose)
		logger << DebugInfo
		<< "Initializing debugging registers"
		<< EndDebugError;
	
	for ( int i = 0; i < 16; i++ ) 
	{
		stringstream str;
		str << "r" << i;
		registers_registry[str.str().c_str()] =
			new SimpleRegister<uint32_t>(str.str().c_str(), &gpr[i]);
	}
	registers_registry["sp"] = new SimpleRegister<uint32_t>("sp", &gpr[13]);
	registers_registry["lr"] = new SimpleRegister<uint32_t>("lr", &gpr[14]);
	registers_registry["pc"] = new SimpleRegister<uint32_t>("pc", &pc);
	registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &cpsr);
	/* End TODO */

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
	uint32_t current_pc = GetNPC();

	if (debug_control_import) 
	{
		DebugControl<uint32_t>::DebugCommand dbg_cmd;

		do 
		{
			dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
	
			if (dbg_cmd == DebugControl<uint32_t>::DBG_STEP) 
			{
				/* Nothing to do */
				break;
			}
			if (dbg_cmd == DebugControl<uint32_t>::DBG_SYNC) 
			{
				// Sync();
				continue;
			}

			if (dbg_cmd == DebugControl<uint32_t>::DBG_KILL) {
				Stop(0);
			}
			if(dbg_cmd == DebugControl<uint32_t>::DBG_RESET) {
				// TODO : memory_interface->Reset(); 
			}
		} while(1);
	}
	
	if (GetCPSR_T()) {
		/* Thumb state */
		
		/* This implementation should never enter into thumb mode */
		logger << DebugError << "Thumb mode not supported in" << endl
		       << unisim::kernel::debug::BackTrace() << EndDebugError;
		Stop(-1);
	}
	
	else {
		/* Arm32 state */
		if(requires_memory_access_reporting and memory_access_reporting_import) {
			memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ,
			                                                   unisim::util::debug::MT_INSN,
			                                                   current_pc, 4 /*insn_size*/);
		}
		
                /* fetch instruction word from memory */
		uint32_t memword;
		ReadInsn(current_pc, memword);
		/* convert the instruction to host endianness */
		isa::arm32::CodeType insn = Target2Host(GetEndianness(), memword);
			
		/* Decode current PC */
		isa::arm32::Operation<unisim::component::cxx::processor::arm::armemu::CPU>* op;
		op = arm32_decoder.Decode(current_pc, insn);
		
		/* update PC registers value before execution */
		this->gpr[15] = this->pc + 8;
		this->pc += 4;
		
		/* Execute instruction */
		op->execute(*this);
		//op->profile(profile);
		
		/* perform the memory load/store operations */
		PerformLoadStoreAccesses();
	}
	
	/* check that an exception has not occurred, if so 
	 * stop the simulation */
	if ( GetVirtualExceptionVector() )
	{
		logger << DebugError
			<< "An exception has been found, this should never happen "
			<< "when simulating at user level."
			<< EndDebugError;
		Stop(-1);
	}
	instruction_counter++;
	if ( unlikely((trap_on_instruction_counter == instruction_counter)
			&& instruction_counter_trap_reporting_import) )
		instruction_counter_trap_reporting_import->ReportTrap(*this);
	
	if(requires_finished_instruction_reporting)
		if(memory_access_reporting_import)
			memory_access_reporting_import->ReportFinishedInstruction(current_pc, this->pc);
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
InjectReadMemory(uint32_t addr, 
	void *buffer,
	uint32_t size)
{
	uint32_t index = 0;
	uint32_t base_addr = (uint32_t)addr;
	uint32_t ef_addr;

	if ( likely(dcache.GetSize()) )
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
InjectWriteMemory(uint32_t addr, 
	const void *buffer, 
	uint32_t size)
{
	uint32_t index = 0;
	uint32_t base_addr = (uint32_t)addr;
	uint32_t ef_addr;
	
	if ( likely(dcache.GetSize()) )
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
ReadMemory(uint32_t addr, 
		void *buffer, 
		uint32_t size)
{
	bool status = true;
	uint32_t index = 0;
	uint32_t base_addr = (uint32_t)addr;
	uint32_t ef_addr;

	if ( likely(dcache.GetSize()) )
	{
		// non intrusive access with linux support
		while (size != 0 && status)
		{
			// need to access the data cache before accessing the main
			//   memory
			ef_addr = base_addr + index;

			uint32_t cache_tag = dcache.GetTag(ef_addr);
			uint32_t cache_set = dcache.GetSet(ef_addr);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( dcache.GetValid(cache_set, cache_way) )
				{
					// the cache access is a hit, data can be simply read
					//   from the cache
					uint32_t cache_index =
							dcache.GetIndex(ef_addr);
					uint32_t data_read_size =
							dcache.GetDataCopy(cache_set,
									cache_way, cache_index, size,
									&(((uint8_t *)buffer)[index]));
					index += data_read_size;
					size -= data_read_size;
					cache_hit = true;
				}
			}
			if ( !cache_hit )
			{
				status = status &&
					ExternalReadMemory(ef_addr,
									   &(((uint8_t *)buffer)[index]),
									   1);
				index++;
				size--;
			}
		}
	}
	else
	{
		// there is no data cache in this system just perform the request
		//   to the memory subsystem
		while ( size != 0 && status )
		{
			ef_addr = base_addr + index;
			status = status &&
					ExternalReadMemory(ef_addr,
							&(((uint8_t *)buffer)[index]),
							1);
			index++;
			size--;
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
WriteMemory(uint32_t addr, 
		const void *buffer, 
		uint32_t size)
{
	bool status = true;
	uint32_t index = 0;
	uint32_t base_addr = (uint32_t)addr;
	uint32_t ef_addr;

	if ( dcache.GetSize() )
	{
		// non intrusive access with linux support
		while (size != 0 && status)
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
					// the cache access is a hit, data can be simply written to 
					//   the cache
					uint32_t cache_index = dcache.GetIndex(ef_addr);
					uint32_t data_read_size =
							dcache.SetData(cache_set, cache_way, cache_index, 
									size, &(((uint8_t *)buffer)[index]));
					dcache.SetDirty(cache_set, cache_way, 1);
					index += data_read_size;
					size -= data_read_size;
					cache_hit = true;
				}
			}
			if ( !cache_hit )
			{
				status = status &&
					ExternalWriteMemory(ef_addr,
										&(((uint8_t *)buffer)[index]),
										1);
				index++;
				size--;
			}
		}
	}
	else
	{
		// there is no data cache in this system, just write the data to
		//   the memory subsystem
		while ( size != 0 && status )
		{
			ef_addr = base_addr + index;
			status = status &&
					ExternalWriteMemory(ef_addr,
							&(((uint8_t *)buffer)[index]),
							1);
			index++;
			size--;
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
	if(registers_registry.find(string(name)) != registers_registry.end())
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
Disasm(uint32_t addr, uint32_t &next_addr)
{
	isa::arm32::Operation<unisim::component::cxx::processor::arm::armemu::CPU> *
		op = NULL;
	uint32_t insn;
	
	stringstream buffer;
	if (GetCPSR_T()) 
	{
		logger << DebugError << "Thumb instructions not supported in" << endl
		       << unisim::kernel::debug::BackTrace() << EndDebugError;
		Stop(-1);
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
		buffer << "0x" << std::hex;
		buffer.fill('0'); buffer.width(8); 
		buffer << op->GetEncoding() << std::dec << " ";
		op->disasm(*this, buffer);

		next_addr = addr + 4;
	}

	return buffer.str();
}
		
/** Exit system call.
 * The LinuxOS service calls this method when the program has finished.
 *
 * @param ret the exit cade sent by the exit system call.
 */
void 
CPU::
PerformExit(int ret)
{
	// running = false;
	Stop(ret);
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

	if ( likely(icache.GetSize()) )
	{
		icache.read_accesses++;
		// check the instruction cache
		uint32_t cache_tag = icache.GetTag(address);
		uint32_t cache_set = icache.GetSet(address);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( icache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( icache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
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
				icache.GetBaseAddressFromAddress(address);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
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
		uint32_t cache_index = icache.GetIndex(address);
		(void)icache.GetData(cache_set, cache_way, cache_index,
				size, &data);
		icache.GetDataCopy(cache_set, cache_way, cache_index, size,
				(uint8_t *)&val);

		if ( unlikely(icache.power_estimator_import != 0) )
			icache.power_estimator_import->ReportReadAccess();
	}
	else
	{
		// no instruction cache present, just request the insn to the
		//   memory system
		PrRead(address, (uint8_t *)&val, size);
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,				
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_WRITE,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_WRITE,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_WRITE,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_WRITE,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
          unisim::util::debug::MAT_WRITE,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_WRITE,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
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
	logger << DebugError << "CoprocessorLoad not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
	logger << DebugError << "CoprocessorLoad not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
	logger << DebugError << "CoprocessorStore not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
	logger << DebugError << "CoprocessorStore not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
	logger << DebugError << "CoprocessorDataProcess not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
	logger << DebugError << "MoveToCoprocessor not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
	logger << DebugError << "MoveFromCoprocessor not implemented in" << endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
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
		<< "Trying to execute unpredictable behavior instruction,"
		<< "Location: " 
		<< __FUNCTION__ << ":" 
		<< __FILE__ << ":" 
		<< __LINE__ << ": "
		<< EndDebugWarning;
	instruction_counter_trap_reporting_import->ReportTrap();
}

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
			case MemoryOp::USER_READ:
				logger << DebugError << "Not permitted operation with armemu in" <<endl
				       << unisim::kernel::debug::BackTrace() << EndDebugError;
				Stop(-1);
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
	uint32_t addr = memop->GetAddress();

	if ( likely(dcache.GetSize()) )
	{
		dcache.prefetch_accesses++;
		uint32_t cache_tag = dcache.GetTag(addr);
		uint32_t cache_set = dcache.GetSet(addr);
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
		if ( likely(!cache_hit) )
		{
			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			bool cache_valid = dcache.GetValid(cache_set,
					cache_way);
			bool cache_dirty = dcache.GetDirty(cache_set,
					cache_way);
			if ( cache_valid & cache_dirty )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t rep_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(rep_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(addr);
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
	uint32_t addr = memop->GetAddress();
	uint32_t size = memop->GetSize();
	uint32_t write_addr = addr;
	uint8_t val8 = 0;
	uint16_t val16 = 0;
	uint32_t val32 = 0;
	uint8_t data8, data16[2], data32[4];
	uint8_t *data = 0;

	data8 = 0;
	memset(data16, 0, sizeof(uint8_t) * 2);
	memset(data32, 0, sizeof(uint8_t) * 4);
	// fix the write address depending on the request size and endianess
	//   and prepare the data to write
	switch(size)
	{
	case 1:
		write_addr = write_addr ^ munged_address_mask8;
		val8 = (uint8_t)memop->GetWriteValue();
		memcpy(&data8, &val8, 1);
		data = &data8;
		break;
	case 2:
		write_addr = write_addr ^ munged_address_mask16;
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

	if ( likely(dcache.GetSize()) )
	{
		dcache.write_accesses++;
		uint32_t cache_tag = dcache.GetTag(write_addr);
		uint32_t cache_set = dcache.GetSet(write_addr);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) != 0 )
			{
				// the access is a hit
				cache_hit = true;
			}
		}
		// if the access was a hit the data needs to be written into
		//   the cache, if the access was a miss the data needs to be
		//   written into memory, but the cache doesn't need to be updated
		if ( likely(cache_hit) )
		{
			dcache.write_hits++;
			uint32_t cache_index = dcache.GetIndex(write_addr);
			dcache.SetData(cache_set, cache_way, cache_index,
					size, data);
			dcache.SetDirty(cache_set, cache_way, 1);
		}
		else
		{
			PrWrite(write_addr, data, size);
		}

		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportWriteAccess();
	}
	else // there is no data cache
	{
		// there is no data cache, so just send the request to the
		//   memory interface
			PrWrite(write_addr, data, size);
	}

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
          unisim::util::debug::MAT_WRITE,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_WRITE,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
					addr, size);
}

/** Performs a read access.
 * @param memop the memory operation containing the read access
 */
void 
CPU::
PerformReadAccess(unisim::component::cxx::processor::arm::MemoryOp
		*memop)
{
	uint32_t addr = memop->GetAddress();
	uint32_t size = memop->GetSize();
	uint32_t read_addr = addr & ~(uint32_t)(size - 1);
	uint8_t data32[4];
	uint8_t *data;

	// fix the read address depending on the request size and endianess
	switch (size)
	{
		case 1: read_addr = read_addr ^ munged_address_mask8; break;
		case 2: read_addr = read_addr ^ munged_address_mask16; break;
		case 4: // nothing to do
		default: // nothing to do
			break;
	}

	if ( likely(dcache.GetSize()) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(read_addr);
		uint32_t cache_set = dcache.GetSet(read_addr);
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

			if ( (cache_valid != 0) & (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t rep_cache_address =
					dcache.GetBaseAddress(cache_set, cache_way);
					dcache.GetData(cache_set, cache_way, &rep_cache_data);
					PrWrite(rep_cache_address, rep_cache_data,
							dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(read_addr);
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
		uint32_t cache_index = dcache.GetIndex(read_addr);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else // there is no data cache
	{
			// just read the data from the memory system
		PrRead(read_addr, data32, size);
		data = data32;
	}

	// fix the data depending on its size
	uint32_t value = 0;
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
		align = addr & (uint32_t)0x03;
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

	SetGPR(memop->GetTargetReg(), value);

	if ( likely(dcache.GetSize()) )
		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
					addr, size);
}

/** Performs a read access and puts result in the PC register.
 * @param memop the memory operation containing the read access
 */
void 
CPU::
PerformReadToPCAccess(unisim::component::cxx::processor::arm::MemoryOp
		*memop)
{
	uint32_t addr = memop->GetAddress();
	const uint32_t size = 4;
	uint32_t read_addr = addr & ~(uint32_t)0x03;
	uint8_t data32[4];
	uint8_t *data;

	if ( likely(dcache.GetSize()) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(read_addr);
		uint32_t cache_set = dcache.GetSet(read_addr);
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
				uint32_t rep_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(rep_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(read_addr);
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
		uint32_t cache_index = dcache.GetIndex(read_addr);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else
	{
		PrRead(read_addr, data32, size);
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
	align = addr & 0x03;
	if (align != 0)
	{
		val32_l = (val32 << (align*8)) &
				((~((uint32_t)0)) << (align*8));
		val32_r = (val32 >> ((4 - align) * 8)) &
				((~((uint32_t)0)) >> ((4 - align) * 8));
		val32 = val32_l + val32_r;
	}
	value = val32;

	Branch(value);

	if ( unlikely(dcache.power_estimator_import != 0) )
		dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
					addr, size);
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
	uint32_t addr = memop->GetAddress();
	const uint32_t size = 4;
	uint32_t read_addr = addr & ~(uint32_t)0x03;
	uint8_t data32[4];
	uint8_t *data;

	if ( likely(dcache.GetSize()) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(read_addr);
		uint32_t cache_set = dcache.GetSet(read_addr);
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
			bool cache_valid = dcache.GetValid(cache_set,
					cache_way);
			bool cache_dirty = dcache.GetDirty(cache_set,
					cache_way);
			if ( cache_valid & cache_dirty )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t rep_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(rep_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(read_addr);
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
		uint32_t cache_index = dcache.GetIndex(read_addr);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else // there is no data cache
	{
		PrRead(read_addr, data32,	size);
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
	align = addr & 0x03;
	if (align != 0)
	{
		val32_l = (val32 << (align*8)) &
				((~((uint32_t)0)) << (align*8));
		val32_r = (val32 >> ((4 - align) * 8)) &
				((~((uint32_t)0)) >> ((4 - align) * 8));
		val32 = val32_l + val32_r;
	}
	value = val32;
	
	BranchExchange( value );
	
	if ( unlikely(dcache.power_estimator_import != 0) )
		dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
          unisim::util::debug::MAT_READ,
          unisim::util::debug::MT_DATA,
					//MemoryAccessReporting<uint32_t>::MAT_READ,
					//MemoryAccessReporting<uint32_t>::MT_DATA,
					addr, size);
}

} // end of namespace armemu
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
