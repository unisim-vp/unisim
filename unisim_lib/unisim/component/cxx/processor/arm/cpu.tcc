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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/config.hh"
#include "unisim/component/cxx/processor/arm/isa_arm32.tcc"
#include "unisim/component/cxx/processor/arm/isa_thumb.tcc"
#include "unisim/component/cxx/processor/arm/instruction.tcc"
#include "unisim/component/cxx/processor/arm/exception.tcc"
#include "unisim/util/debug/simple_register.hh"
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif // __STDC_CONSTANT_MACROS
#include <stdint.h>

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

	using unisim::util::endian::E_BIG_ENDIAN;
	using unisim::util::endian::E_LITTLE_ENDIAN;
	using unisim::util::endian::BigEndian2Host;
	using unisim::util::endian::Host2BigEndian;
	using unisim::util::endian::Target2Host;
	using unisim::util::endian::Host2Target;
	using unisim::util::arithmetic::RotateRight;
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::hex;
	using std::dec;
	using std::ostringstream;
	using unisim::util::debug::SimpleRegister;
	using unisim::util::debug::Symbol;
	using namespace unisim::kernel::logger;
	
	// Constructor
	template<class CONFIG>
	CPU<CONFIG> ::
	CPU(const char *name, 
		CacheInterface<typename CONFIG::address_t> *_memory_interface, 
		Object *parent) :
	Object(name, parent),
	// Client<Loader<typename CONFIG::address_t> >(name, parent),
	Client<LinuxOS>(name, parent),
	Service<CPULinuxOS>(name, parent),
	Service<MemoryInjection<uint64_t> >(name, parent),
	Client<DebugControl<uint64_t> >(name, parent),
	Client<MemoryAccessReporting<uint64_t> >(name, parent),
	Client<TrapReporting>(name, parent),
	Service<MemoryAccessReportingControl>(name, parent),
	Service<Disassembly<uint64_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<Memory<uint64_t> >(name, parent),
	Client<CachePowerEstimator>(name,  parent),
	Client<PowerMode>(name,  parent),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
	memory_injection_export("memory_injection_export", this),
	memory_export("memory_export", this),
	cpu_linux_os_export("cpu_linux_os_export", this),
	memory_access_reporting_control_export(
										   "memory_access_reporting_control_export", 
										   this),
	debug_control_import("debug_control_import", this),
	memory_access_reporting_import("memory_access_reporting_import", this),
	symbol_table_lookup_import("symbol_table_lookup_import", this),
	linux_os_import("linux_os_import", this),
	trap_reporting_import("trap_reporting_import", this),
	il1_power_estimator_import("il1-power-estimator-import",  this),
	dl1_power_estimator_import("dl1-power-estimator-import",  this),
	il1_power_mode_import("il1-power-mode-import",  this),
	dl1_power_mode_import("dl1-power-mode-import",  this),
	logger(*this),
	insn_cache_line_address(0),
	memory_interface(_memory_interface),
	running(true),
	cache_l1(0),
	cache_il1(0),
	cache_l2(0),
	requires_memory_access_reporting(true),
	requires_finished_instruction_reporting(true),
	fetchQueue(),
	decodeQueue(),
	executeQueue(),
	lsQueue(),
	firstLS(0),
	freeLSQueue(),
	instruction_counter(0),
	default_endianness(E_BIG_ENDIAN),
	default_endianness_string("big-endian"),
	arm966es_initram(false),
	arm966es_vinithi(false),
	cpu_cycle_time(0),
	voltage(0),
	cpu_cycle(0),
	bus_cycle(0),
	verbose_all(false),
	verbose_setup(false),
	verbose_step(false),
	verbose_exception(false),
	verbose_dump_regs_start(false),
	verbose_dump_regs_end(false),
	verbose_memory(false),
	trap_on_exception(false),
	trap_on_instruction_counter(0),
	param_default_endianness("default-endianness", this, default_endianness_string, "The processor default/boot endianness. Available values are: little-endian and big-endian."),
	param_arm966es_initram("arm966es-initram", this, arm966es_initram),
	param_arm966es_vinithi("arm966es-vinithi", this, arm966es_vinithi),
	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time,
			"CPU cycle time in ps"),
	param_voltage("voltage",  this,  voltage, "CPU voltage in mV"),
	param_verbose_all("verbose-all", this, verbose_all,
					  "Activate all the verbose option"),
	param_verbose_setup("verbose-setup", this, verbose_setup,
						"Display setup information"),
	param_verbose_step("verbose-step", this, verbose_step,
					   "Display instruction step information"),
	param_verbose_exception("verbose-exception", this, verbose_exception),
	param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start),
	param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end),
	param_verbose_memory("verbose-memory", this, verbose_memory,
			"Dump detailed memory operations"),
	param_trap_on_exception("trap-on-exception", this, trap_on_exception,
							"Produce a trap when an exception occurs"),
	param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
									  "Produce a trap when the instruction counter is reached"),
	stat_instruction_counter("instruction-counter", this, instruction_counter,
							 "Number of instructions executed"),
	reg_sp("SP", this, gpr[13],
		   "The stack pointer (SP) register (alias of GPR[13])"),
	reg_lr("LR", this, gpr[14],
		   "The link register (LR) (alias of GPR[14]"),
	reg_pc("PC", this, gpr[15],
		   "The program counter (PC) register (alias of GPR[15]"),
	reg_cpsr("CPSR", this, cpsr,
			 "The CPSR register"),
	/* initialization of ARM926EJS variables START */
	arm926ejs_icache(),
	arm926ejs_dcache(),
	arm926ejs_itcm(),
	arm926ejs_dtcm(),
	arm926ejs_wb(),
	arm926ejs_wbb(),
	arm926ejs_icache_size(0x01000), // 4KB
	arm926ejs_dcache_size(0x01000), // 4KB
	arm926ejs_enable_write_buffer(true),
	arm926ejs_enable_writeback_buffer(true)
	/* initialization of ARM926EJS variables   END */
	{
	
		stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

		/* create ARM926EJS parameters if the CONFIG::MODEL is ARM926EJS */
		if (CONFIG::MODEL == ARM926EJS)
		{
			param_arm926ejs_icache_size = new
				unisim::kernel::service::Parameter<uint32_t>("icache-size",
						this,
						arm926ejs_icache_size,
						"Instruction cache size in KB (available sizes are 4KB, 8KB, 16KB,"
						" 32KB, 64KB, and 128KB). It can be desactivated setting it to 0.");
			param_arm926ejs_dcache_size = new
				unisim::kernel::service::Parameter<uint32_t>("dcache-size",
						this,
						arm926ejs_dcache_size,
						"Data cache size in KB (available sizes are 4KB, 8KB, 16KB, 32KB,"
						" 64KB, and 128KB). It can be desactivated setting it to 0.");
			param_arm926ejs_enable_wb = new
				unisim::kernel::service::Parameter<bool>("enable-wb",
						this,
						arm926ejs_enable_write_buffer,
						"Activate the processor write buffer (only considered in user mode)");
			param_arm926ejs_enable_wbb = new
				unisim::kernel::service::Parameter<bool>("enable-wbb",
						this,
						arm926ejs_enable_writeback_buffer,
						"Activate the processor write-back buffer (only considered in user"
						" mode)");
		}

		/* Reset all the registers */
		InitGPR();
		
		// set CPSR to system mode
		cpsr = 0;
		for(unsigned int i = 0; i < num_phys_spsrs; i++) {
			spsr[i] = 0;
		}
		for(unsigned int i = 0; i < 8; i++) {
			fake_fpr[i] = 0;
		}
		fake_fps = 0;
		SetCPSR_Mode(SYSTEM_MODE);
		// set initial pc
		/* Depending on the configuration being used set the initial pc */
		if(CONFIG::MODEL == ARM966E_S) {
			if(arm966es_vinithi)
				SetGPR(PC_reg, (address_t)0xffff0000);
			else
				SetGPR(PC_reg, (address_t)0x00000000);
			/* disable normal and fast interruptions */
			SetCPSR_F();
			SetCPSR_I();
		}
		
		if(CONFIG::MODEL == ARM7TDMI) {
			SetGPR(PC_reg, (address_t)0x0);
			/* disable normal and fast interruptions */
			SetCPSR_F();
			SetCPSR_I();
		}
		
		// initialize the variables to compute the final address on memory 
		//   accesses
		if(GetEndianness() == E_BIG_ENDIAN) {
			munged_address_mask8 = (typename CONFIG::address_t)0x03;;
			munged_address_mask16 = (typename CONFIG::address_t)0x02;
		} else {
			munged_address_mask8 = 0;
			munged_address_mask16 = 0;
		}
		
		/* initialize the check condition table */
		InitializeCheckConditionTable();
		
		CreateTCMSystem();
		
		CreateCpSystem();
		
		CreateMemorySystem();
		
		for (unsigned int i = 0; i < num_phys_gprs; i++)
		{
			stringstream ss;
			stringstream ss_desc;
			ss << "GPR_PHYS[" << i << "]";
			ss_desc << "Physical register " << i;
			reg_phys_gpr[i] = new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), this, phys_gpr[i], ss_desc.str().c_str());
		}
		for (unsigned int i = 0; i < num_log_gprs; i++)
		{
			stringstream ss;
			stringstream ss_desc;
			ss << "GPR[" << i << "]";
			ss_desc << "Logical register " << i;
			reg_gpr[i] = new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), this, gpr[i], ss_desc.str().c_str());
		}
		for (unsigned int i = 0; i < num_phys_spsrs; i++)
		{
			stringstream ss;
			stringstream ss_desc;
			ss << "SPSR[" << i << "]";
			ss_desc << "SPSR[" << i << "] register";
			reg_spsr[i] = new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), this, spsr[i], ss_desc.str().c_str());
		}
	}
	
	// Destructor
	template<class CONFIG> 
	CPU<CONFIG> ::
	~CPU() {
		for (unsigned int i = 0; i < num_phys_gprs; i++)
			if (reg_phys_gpr[i]) delete reg_phys_gpr[i];
		for (unsigned int i = 0; i < num_log_gprs; i++)
			if (reg_gpr[i]) delete reg_gpr[i];
		for (unsigned int i = 0; i < num_phys_spsrs; i++)
			if (reg_spsr[i]) delete reg_spsr[i];
	}
	
	//=====================================================================
	//=                  Client/Service setup methods               START =
	//=====================================================================
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	Setup() {
		/* check verbose settings */
		if (verbose_all) {
			verbose_setup = true;
			verbose_step = true;
			verbose_dump_regs_start = true;
			verbose_dump_regs_end = true;
			verbose_memory = true;
		}
		if ( verbose_setup )
		{
			logger << DebugInfo;
			logger << " - verbose-all = " << verbose_all << endl;
			logger << " - verbose-setup = " << verbose_setup << endl;
			logger << " - verbose-step = " << verbose_step << endl;
			logger << " - verbose-dump-regs-start = " << verbose_dump_regs_start
					<< endl;
			logger << " - verbose-dump-regs-end = " << verbose_dump_regs_end
					<< endl;
			logger << " - verbose-memory = " << verbose_memory;
			logger << EndDebugInfo;
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
			if ( default_endianness_string.compare("little-endian") == 0 )
				default_endianness = E_LITTLE_ENDIAN;
			else
				default_endianness = E_BIG_ENDIAN;
		}
		/* check if the emulator is running in user or system mode and perform
		 *   the corresponding initialization
		 * if linux_os_import is connected then we are running in user mode,
		 *   otherwise we are running in system mode
		 */ 
		if (linux_os_import) {
			/* we are running in user mode:
			 * - set CPSR to user mode
			 * - initialize the cache system
			 * - the initial pc will be set by linux
			 */
			// set CPSR to user mode
			SetCPSR_Mode(USER_MODE);
			if (VerboseSetup()) {
				logger << DebugInfo << "Running \"" << GetName() << "\" in user mode"
				<< EndDebugInfo;
			}
			// TO REMOVE (START)
			// // initialize the cache system
			// if(cache_l1 != 0) cache_l1->Enable();
			// if(cache_il1 != 0) cache_il1->Enable();
			// if(cache_l2 != 0) cache_l2->Enable();
			// TO REMOVE (END)

			/* models not specified will use no caches at all */
			if ( CONFIG::MODEL == ARM926EJS )
			{
				unsigned int min_cycle_time = 0;
				if ( arm926ejs_icache_size != 0 )
				{
					if ( !arm926ejs_icache.SetSize(arm926ejs_icache_size) )
					{
						logger << DebugError
							<< "Invalid instruction cache size (" 
							<< (unsigned int)arm926ejs_icache_size
							<< ")" 
							<< EndDebug;
						return false;
					}
					if ( il1_power_mode_import )
					{
						unsigned int il1_min_cycle_time =
								il1_power_mode_import->GetMinCycleTime();
						if ( il1_min_cycle_time > 0 &&
								il1_min_cycle_time > min_cycle_time )
							min_cycle_time = il1_min_cycle_time;
						unsigned int il1_default_voltage =
								il1_power_mode_import->GetDefaultVoltage();
						if ( voltage <= 0 )
							voltage = il1_default_voltage;
					}
				}
				if ( arm926ejs_dcache_size != 0 )
				{
					if ( !arm926ejs_dcache.SetSize(arm926ejs_dcache_size) )
					{
						logger << DebugError
							<< "Invalid data cache size ("
							<< (unsigned int)arm926ejs_dcache_size
							<< ")"
							<< EndDebug;
						return false;
					}
					if ( dl1_power_mode_import )
					{
						unsigned int dl1_min_cycle_time =
								dl1_power_mode_import->GetMinCycleTime();
						if ( dl1_min_cycle_time > 0 &&
								dl1_min_cycle_time > min_cycle_time )
							min_cycle_time = dl1_min_cycle_time;
						unsigned int dl1_default_voltage =
								dl1_power_mode_import->GetDefaultVoltage();
						if ( voltage <= 0 )
							voltage = dl1_default_voltage;
					}
				}

				if ( min_cycle_time > 0 )
				{
					if ( cpu_cycle_time > 0 )
					{
						if ( cpu_cycle_time < min_cycle_time )
						{
							if ( unlikely(verbose_setup) )
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
					}
					else
					{
						cpu_cycle_time = min_cycle_time;
					}
				}
				else
				{
					if ( cpu_cycle_time <= 0 )
					{
						// we can't provide a valid cpu cycle time configuration
						//   automatically
						logger << DebugError
								<< "cpu-cycle-time should be bigger than 0"
								<< EndDebugError;
						return false;
					}
				}
			}
			
			// initialize the variables to compute the final address on memory 
			//   accesses
			munged_address_mask8 = 0;
			munged_address_mask16 = 0;

			if (verbose_setup)
			{
				logger << DebugInfo;
				logger << "Setup information:" << endl;
				logger << " - debug_control service ";
				if (debug_control_import)
					logger << "CONNECTED";
				else
					logger << "DISCONNECTED";
				logger << endl;
				logger << " - cpu cycle time = " << cpu_cycle_time << " ps"
						<< endl;
				if ( arm926ejs_icache_size != 0 )
				{
					logger << " - instruction cache size = "
							<< arm926ejs_icache_size << " bytes" << endl;
					logger << " - instruction cache power mode service ";
					if ( il1_power_mode_import )
						logger << "CONNECTED";
					else
						logger << "DISCONNECTED";
				}
				logger << endl;
				if ( arm926ejs_dcache_size != 0 )
				{
					logger << " - data cache size = "
							<< arm926ejs_dcache_size << " bytes" << endl;
					logger << " - data cache power mode service ";
					if ( dl1_power_mode_import )
						logger << "CONNECTED";
					else
						logger << "DISCONNECTED";
				}
				logger << endl;
				if ( (arm926ejs_icache_size != 0) ||
						(arm926ejs_dcache_size != 0) )
				{
					if ( il1_power_mode_import || dl1_power_mode_import )
						logger << " - voltage = " << voltage << "mV" << endl;
				}
				logger << EndDebugInfo;
			}

		} else {
			/* we are running in system mode */
			/* check that the processor model is supported */
			if(CONFIG::MODEL != ARM966E_S &&
			   CONFIG::MODEL != ARM7TDMI &&
			   CONFIG::MODEL != ARM926EJS) {
				logger << DebugError
				<< "Running \"" << GetName() << "\" in system mode. "
				<< "Only arm926ej_s, arm966e_s, arm7tdmi can run under this mode."
				<< EndDebug;
				return false;
			}
			// set CPSR to system mode
			SetCPSR_Mode(SYSTEM_MODE);
			// set initial pc
			/* Depending on the configuration being used set the initial pc */
			if (CONFIG::MODEL == ARM966E_S) 
			{
				if (arm966es_vinithi)
					SetGPR(PC_reg, (address_t)0xffff0000);
				else
					SetGPR(PC_reg, (address_t)0x00000000);
				/* disable normal and fast interruptions */
				SetCPSR_F();
				SetCPSR_I();
			}
			if (CONFIG::MODEL == ARM7TDMI) 
			{
				SetGPR(PC_reg, (address_t)0x0);
				/* disable normal and fast interruptions */
				SetCPSR_F();
				SetCPSR_I();
			}
			if (CONFIG::MODEL == ARM926EJS) 
			{
				SetGPR(PC_reg, (address_t)0x0);
				/* disable normal and fast interruptions */
				SetCPSR_F();
				SetCPSR_I();

				// Set the cache sizes
				if (arm926ejs_icache_size)
				{
					if (!arm926ejs_icache.SetSize(arm926ejs_icache_size))
					{
						logger << DebugError
							<< "Invalid instruction cache size (" << arm926ejs_icache_size
							<< ")"
							<< EndDebug;
						return false;
					}
				}
				else
				{
					logger << DebugError
						<< "Invalid instruction cache size (" << arm926ejs_icache_size
						<< ")"
						<< EndDebug;
					return false;
				}

				if (arm926ejs_dcache_size)
				{
					if (!arm926ejs_dcache.SetSize(arm926ejs_dcache_size))
					{
						logger << DebugError
							<< "Invalid data cache size (" << arm926ejs_dcache_size
							<< ")"
							<< EndDebug;
						return false;
					}
				}
				else
				{
					logger << DebugError
						<< "Invalid data cache size (" << arm926ejs_dcache_size
						<< ")"
						<< EndDebug;
					return false;
				}
			}
			
			// initialize the variables to compute the final address on memory 
			//   accesses
			if(GetEndianness() == E_BIG_ENDIAN) {
				munged_address_mask8 = (typename CONFIG::address_t)0x03;;
				munged_address_mask16 = (typename CONFIG::address_t)0x02;
			} else {
				munged_address_mask8 = 0;
				munged_address_mask16 = 0;
			}
		}
		
		/* setting debugging registers */
		if (verbose_setup) 
			logger << DebugInfo
			<< "Initializing debugging registers"
			<< EndDebug;
		
		for(int i = 0; i < 16; i++) {
			stringstream str;
			str << "r" << i;
			registers_registry[str.str().c_str()] = 
	    	new SimpleRegister<reg_t>(str.str().c_str(), &gpr[i]);
		}
		registers_registry["sp"] = new SimpleRegister<reg_t>("sp", &gpr[13]);
		registers_registry["lr"] = new SimpleRegister<reg_t>("lr", &gpr[14]);
		registers_registry["pc"] = new SimpleRegister<reg_t>("pc", &gpr[15]);
		registers_registry["cpsr"] = new SimpleRegister<reg_t>("cpsr", &cpsr);
		
		if(!memory_access_reporting_import) {
			requires_memory_access_reporting = false;
			requires_finished_instruction_reporting = false;
		}
		
		return true;
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	OnDisconnect() {
	}
	
	//=====================================================================
	//=                  Client/Service setup methods               END   =
	//=====================================================================
	
	//=====================================================================
	//=             memory injection interface methods              START =
	//=====================================================================

	template<class CONFIG>
	bool 
	CPU<CONFIG> :: 
	InjectReadMemory(uint64_t addr, void *buffer, uint32_t size) 
	{
		uint32_t index = 0;
		uint32_t base_addr = (uint32_t)addr;
		uint32_t ef_addr;
		if ( (CONFIG::MODEL == ARM926EJS) && linux_os_import)
		{
			// access memory while using the linux_os_import
			//   tcm is ignored
			while (size != 0)
			{
				// need to access the data cache before accessing the main memory
				ef_addr = base_addr + index;
				
				uint32_t cache_tag = arm926ejs_dcache.GetTag(ef_addr);
				uint32_t cache_set = arm926ejs_dcache.GetSet(ef_addr);
				uint32_t cache_way;
				bool cache_hit = false;
				if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
				{
					if ( arm926ejs_dcache.GetValid(cache_set, cache_way))
					{
						// the cache access is a hit, data can be simply read from the cache
						uint32_t cache_index = arm926ejs_dcache.GetIndex(ef_addr);
						uint32_t read_data_size = 
							arm926ejs_dcache.GetDataCopy(cache_set, cache_way, cache_index, size, &(((uint8_t *)buffer)[index]));
						index += read_data_size;
						size -= read_data_size;
						cache_hit = true;
					}
				}
				if ( !cache_hit )
				{
					memory_interface->PrRead(ef_addr,
											 &(((uint8_t *)buffer)[index]),
											 1);
					index++;
					size--;
				}
			}
			return true;
		}
		if ( (CONFIG::MODEL == ARM926EJS) && !linux_os_import)
		{
			// access memory while simulating full system
			// TODO
			return false;
		}
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	InjectWriteMemory(uint64_t addr, 
					  const void *buffer, 
					  uint32_t size) {
		uint32_t index = 0;
		uint32_t base_addr = (uint32_t)addr;
		uint32_t ef_addr;
		if ( (CONFIG::MODEL == ARM926EJS) && linux_os_import)
		{
			// access memory while using the linux_os_import
			//   tcm is ignored
			while (size != 0)
			{
				// need to access the data cache before accessing the main memory
				ef_addr = base_addr + index;
				
				uint32_t cache_tag = arm926ejs_dcache.GetTag(ef_addr);
				uint32_t cache_set = arm926ejs_dcache.GetSet(ef_addr);
				uint32_t cache_way;
				bool cache_hit = false;
				if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
				{
					if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
					{
						// the cache access is a hit, data can be simply read from the cache
						uint32_t cache_index = arm926ejs_dcache.GetIndex(ef_addr);
						uint32_t write_data_size = 
							arm926ejs_dcache.SetData(cache_set, cache_way, cache_index, size, &(((uint8_t *)buffer)[index]));
						arm926ejs_dcache.SetDirty(cache_set, cache_way, 1);
						index += write_data_size;
						size -= write_data_size;
						cache_hit = true;
					}
				}
				if ( !cache_hit )
				{
					memory_interface->PrWrite(ef_addr,
							&(((uint8_t *)buffer)[index]),
							1);
					index++;
					size--;
				}
			}
			return true;
		}
		if ( (CONFIG::MODEL == ARM926EJS) && !linux_os_import)
		{
			// access memory while simulating full system
			// TODO
			return false;
		}
		return false;
	}
	
	//=====================================================================
	//=             memory injection interface methods              END   =
	//=====================================================================
	
	//=====================================================================
	//=         memory access reporting control interface methods   START =
	//=====================================================================
	
	template<class CONFIG>
	void 
	CPU<CONFIG>::
	RequiresMemoryAccessReporting(bool report) {
		requires_memory_access_reporting = report;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG>::
	RequiresFinishedInstructionReporting(bool report) {
		requires_finished_instruction_reporting = report;
	}
	
	//=====================================================================
	//=         memory access reporting control interface methods   END   =
	//=====================================================================
	
	//=====================================================================
	//=         non intrusive memory methods                        START =
	//=====================================================================
	
	// Non intrusive memory read
	template<class CONFIG>
	bool 
	CPU<CONFIG> :: 
	ReadMemory(uint64_t addr, void *buffer, uint32_t size) 
	{
		bool status = true;
		uint32_t index = 0;
		uint32_t base_addr = (uint32_t)addr;
		uint32_t ef_addr;
		
		if (CONFIG::MODEL == ARM926EJS && linux_os_import)
		{
			// non intrusive access with linux support
			//   tcm is ignored
			while (size != 0 && status)
			{
				// need to access the data cache before accessing the main
				//   memory
				ef_addr = base_addr + index;
				
				uint32_t cache_tag = arm926ejs_dcache.GetTag(ef_addr);
				uint32_t cache_set = arm926ejs_dcache.GetSet(ef_addr);
				uint32_t cache_way;
				bool cache_hit = false;
				if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
				{
					if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
					{
						// the cache access is a hit, data can be simply read
						//   from the cache
						uint32_t cache_index =
								arm926ejs_dcache.GetIndex(ef_addr);
						uint32_t data_read_size =
								arm926ejs_dcache.GetDataCopy(cache_set,
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
			return status;
		}
		
		if (CONFIG::MODEL == ARM926EJS && !linux_os_import)
		{
			// non intrusive access running on full-system mode
			// TODO
			return false;
		}
		
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	WriteMemory(uint64_t addr, 
				const void *buffer, uint32_t size) 
	{
		bool status = true;
		uint32_t index = 0;
		uint32_t base_addr = (uint32_t)addr;
		uint32_t ef_addr;
		
		if (CONFIG::MODEL == ARM926EJS && linux_os_import)
		{
			// non intrusive access with linux support
			//   tcm is ignored
			while (size != 0 && status)
			{
				// need to access the data cache before accessing the main memory
				ef_addr = base_addr + index;
				
				uint32_t cache_tag = arm926ejs_dcache.GetTag(ef_addr);
				uint32_t cache_set = arm926ejs_dcache.GetSet(ef_addr);
				uint32_t cache_way;
				bool cache_hit = false;
				if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
				{
					if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
					{
						// the cache access is a hit, data can be simply written to the cache
						uint32_t cache_index = arm926ejs_dcache.GetIndex(ef_addr);
						uint32_t data_read_size =
							arm926ejs_dcache.SetData(cache_set, cache_way, cache_index, size, &(((uint8_t *)buffer)[index]));
						arm926ejs_dcache.SetDirty(cache_set, cache_way, 1);
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
			return status;
		}
				
		if (CONFIG::MODEL == ARM926EJS && !linux_os_import)
		{
			// non intrusive access running on full-system mode
			// TODO
			return false;
		}
		
		return false;
	}
	
	template<class CONFIG>
	bool
	CPU<CONFIG> ::
	CoprocessorReadMemory(uint64_t addr,
						  void *buffer, uint32_t size)
	{
		return ExternalReadMemory(addr, buffer, size);
	}
	
	template<class CONFIG>
	bool
	CPU<CONFIG> ::
	CoprocessorWriteMemory(uint64_t addr,
						   const void *buffer, uint32_t size)
	{
		return ExternalWriteMemory(addr, buffer, size);
	}
	
	template<class CONFIG>
	bool
	CPU<CONFIG> ::
	ExternalReadMemory(uint64_t addr,
					   void *buffer, uint32_t size)
	{
		// this method should be implemented by the super class, that should
		//   provide access to the external memory
		return false;
	}
	
	template<class CONFIG>
	bool
	CPU<CONFIG> ::
	ExternalWriteMemory(uint64_t addr,
						const void *buffer, uint32_t size) 
	{
		// this method should be implemented by the super class, that shoul
		//   provide access to the external memory
		return false;
	}
	
	//=====================================================================
	//=             memory interface methods                        END   =
	//=====================================================================
	
	//=====================================================================
	//=             Registers interface methods                    START  =
	//=====================================================================
	
	template<class CONFIG>
	Register *
	CPU<CONFIG> ::
	GetRegister(const char *name) {
		if(registers_registry.find(string(name)) != registers_registry.end())
			return registers_registry[string(name)];
		else 
			return NULL;
	}
	
	//=====================================================================
	//=             CPURegistersInterface interface methods         END  ==
	//=====================================================================
	
	//=====================================================================
	//=                   Disassembly methods                      START  =
	//=====================================================================
	
	template<class CONFIG>
	string
	CPU<CONFIG> ::
	Disasm(uint64_t addr, uint64_t &next_addr) {
		typename isa::arm32::Operation<CONFIG> *op = NULL;
		typename isa::thumb::Operation<CONFIG> *top = NULL;
		insn_t insn;
		thumb_insn_t tinsn;
		
		stringstream buffer;
		if(GetCPSR_T()) {
			if(!ReadMemory(addr, &tinsn, 2)) {
				buffer << "Could not read from memory";
				return buffer.str();
			}
			if (GetEndianness() == E_BIG_ENDIAN)
				tinsn = BigEndian2Host(tinsn);
			else
				tinsn = LittleEndian2Host(tinsn);
			top = thumb_decoder.Decode(addr, tinsn);
			top->disasm(*this, buffer);
		} else {
			if(!ReadMemory(addr, &insn, 4)) {
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
	
	//=====================================================================
	//=                   DebugDisasmInterface methods              END   =
	//=====================================================================
	
	//=====================================================================
	//=                   Debugging methods                        START  =
	//=====================================================================
	
	template<class CONFIG>
	string 
	CPU<CONFIG> ::
	GetObjectFriendlyName(uint64_t addr)
	{
		stringstream sstr;
		
		const Symbol<uint64_t> *symbol = 
				symbol_table_lookup_import->FindSymbolByAddr(addr,
						Symbol<uint64_t>::SYM_OBJECT);
		if(symbol)
			sstr << symbol->GetFriendlyName(addr);
		else
			sstr << "0x" << std::hex << addr << std::dec;
		
		return sstr.str();	
	}
	
	template<class CONFIG>
	string
	CPU<CONFIG> ::
	GetFunctionFriendlyName(uint64_t addr)
	{
		stringstream sstr;
		
		const Symbol<uint64_t> *symbol = 
				symbol_table_lookup_import->FindSymbolByAddr(addr,
						Symbol<uint64_t>::SYM_FUNC);
		if ( symbol )
			sstr << symbol->GetFriendlyName(addr);
		else
			sstr << "0x" << std::hex << addr << std::dec;
		
		return sstr.str();
	}
	
	//=====================================================================
	//=                   Debugging methods                         END   =
	//=====================================================================
	
	//=====================================================================
	//=                    execution handling methods              START  =
	//=====================================================================
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	Stop(int ret) {
		exit(ret);
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	OnBusCycle() {
		
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	Run() {
		
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	Sync() {
		
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	CoprocessorStop(unsigned int cp_id, int ret) {
		Stop(ret);
	}
	
	template<class CONFIG>
	endian_type
	CPU<CONFIG> ::
	CoprocessorGetEndianness() {
		return GetEndianness();
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CoprocessorGetVinithi() {
		return arm966es_vinithi;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CoprocessorGetInitram() {
		return arm966es_initram;
	}
	
	//=====================================================================
	//=                    execution handling methods               END   =
	//=====================================================================
	
	/**************************************************************/
	/* Operand decoding methods     START                         */
	/**************************************************************/
	
	/* Data processing operand decoding */
	template<class CONFIG>
	inline 
	typename CONFIG::reg_t 
	CPU<CONFIG> ::
	ShiftOperand32imm(const uint32_t rotate_imm, 
					  const uint32_t imm) {
		reg_t shifter_operand = 0;
		
		//	if(rotate_imm == 0) {
		//		shifter_operand = imm;
		//	} else {
		shifter_operand = RotateRight(imm, rotate_imm * 2);
		//	}
		
		return shifter_operand;
	}
	
	template<class CONFIG>
	inline
	typename CONFIG::reg_t 
	CPU<CONFIG> ::
	ShiftOperand32imm(const uint32_t rotate_imm, 
					  const uint32_t imm, 
					  bool *shift_carry_out) {
		reg_t shifter_operand = 0;
		
		shifter_operand = RotateRight(imm, rotate_imm * 2);
		if(rotate_imm == 0) {
			*shift_carry_out = GetCPSR_C();
		} else {
			//		*shift_carry_out = ((((typename CONFIG::reg_t)(-1)) & ~(((typename CONFIG::reg_t)(-1)) >> 1)) & shifter_operand) != 0;
			*shift_carry_out = (((typename CONFIG::reg_t)1) << 31) & shifter_operand;
		}
		
		return shifter_operand;
	}
	
	template<class CONFIG>
	inline
	typename CONFIG::reg_t 
	CPU<CONFIG> ::
	ShiftOperandImmShift(const uint32_t shift_imm, 
						 const uint32_t shift, 
						 const typename CONFIG::reg_t val_reg) {
		typename CONFIG::reg_t shifter_operand = 0;
		
		if((shift_imm == 0) && (shift == 0)) {
			shifter_operand = val_reg;
			return shifter_operand;
		} else {
			
			if(shift == 0) {
				shifter_operand = val_reg << shift_imm;
				return shifter_operand;
			} else {
				
				if(shift == 0x01) {
					if(shift_imm == 0) {
						shifter_operand = 0;
					} else {
						shifter_operand = val_reg >> shift_imm;
					}
					return shifter_operand;
				} else {
					if(shift == 0x02) {
						if(shift_imm == 0) {
							if(((typename CONFIG::sreg_t)val_reg) > 0) {
								shifter_operand = 0;
							} else {
								shifter_operand = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
							}
						} else {
							shifter_operand = ((typename CONFIG::sreg_t)val_reg) >> shift_imm;
						}
						return shifter_operand;
					} else {
						
						if(shift == 0x03) { /* ROR */
							if(shift_imm == 0) {
								shifter_operand = 0;
								shifter_operand = ((GetCPSR_C()?1:0) << 31) | (val_reg >> 1);
							} else {
								shifter_operand = RotateRight(val_reg, shift_imm);
							}
							return shifter_operand;
						}
					}
				}
			}
		}
		
		return 0;
	}
	
	template<class CONFIG>
	inline
	typename CONFIG::reg_t 
	CPU<CONFIG> ::
	ShiftOperandImmShift(const uint32_t shift_imm, 
						 const uint32_t shift, 
						 const typename CONFIG::reg_t val_reg,
						 bool *shift_carry_out) {
		typename CONFIG::reg_t shifter_operand = 0;
		uint32_t mask = 1;
		
		if((shift_imm == 0) && (shift == 0)) {
			shifter_operand = val_reg;
			*shift_carry_out = GetCPSR_C();
			return shifter_operand;
		} else {
			
			if(shift == 0) {
				shifter_operand = val_reg << shift_imm;
				//     shifter_operand |= val_reg >> (32 - shift_imm);
				mask = mask << (32 - shift_imm);
				*shift_carry_out = ((mask & val_reg) != 0);
				return shifter_operand;
			} else {
				if(shift == 0x01) {
					if(shift_imm == 0) {
						shifter_operand = 0;
						mask = mask << 31;
						*shift_carry_out = ((mask & val_reg) != 0);
					} else {
						shifter_operand = val_reg >> shift_imm;
						mask = mask << (shift_imm - 1);
						*shift_carry_out = ((mask & val_reg) != 0);
					}
					return shifter_operand;
				} else {
					
					if(shift == 0x02) {
						if(shift_imm == 0) {
							if(((typename CONFIG::sreg_t)val_reg) > 0) {
								shifter_operand = 0;
								*shift_carry_out = false;
							} else {
								shifter_operand = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
								*shift_carry_out = true;
							}
						} else {
							shifter_operand = ((typename CONFIG::sreg_t)val_reg) >> shift_imm;
							mask = mask << (shift_imm - 1);
							*shift_carry_out = ((mask & val_reg) != 0);
						}
						return shifter_operand;
					} else {
						
						if(shift == 0x03) { /* ROR */
							if(shift_imm == 0) {
								shifter_operand = 0;
								if(GetCPSR_C()) {
									shifter_operand = ((typename CONFIG::reg_t)1) << 31;
								}
								shifter_operand |= (val_reg >> 1);
								*shift_carry_out = ((0x01 & val_reg) != 0);
							} else {
								shifter_operand = RotateRight(val_reg, shift_imm);
								*shift_carry_out = (val_reg >> (shift_imm - 1)) & 0x1;
							}
							return shifter_operand;
						}
					}
				}
			}
		}
		
		return 0;
	}
	
	template<class CONFIG>
	inline
	typename CONFIG::reg_t
	CPU<CONFIG> ::
	ShiftOperandRegShift(const uint32_t shift_reg, 
						 const uint32_t shift, 
						 const typename CONFIG::reg_t val_reg) {
		typename CONFIG::reg_t shifter_operand = 0;
		typename CONFIG::reg_t sr8 = (shift_reg & 0x0FF);
		typename CONFIG::reg_t sr5 = (shift_reg & 0x01F);
		
		if(shift == 0x0) {
			if(sr8 == 0) {
				shifter_operand = val_reg;
			} else if(sr8 < 32) {
				shifter_operand = val_reg << sr8;
			} else if(sr8 == 32) {
				shifter_operand = 0;
			} else {
				shifter_operand = 0;
			}
			return shifter_operand;
		} else {
			
			if(shift == 0x01) {
				if(sr8 == 0) {
					shifter_operand = val_reg;
				} else if(sr8 < 32) {
					shifter_operand = val_reg >> sr8;
				} else if(sr8 == 32) {
					shifter_operand = 0;
				} else {
					shifter_operand = 0;
				}
				return shifter_operand;
			} else {
				
				if(shift == 0x02) {
					if(sr8 == 0) {
						shifter_operand = val_reg;
					} else if(sr8 < 32) {
						shifter_operand = ((typename CONFIG::sreg_t)val_reg) >> sr8;
					} else {
						if((val_reg & ((typename CONFIG::reg_t)1 << 31)) == 0) {
							shifter_operand = 0;
						} else {
							shifter_operand = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
						}
					}
					return shifter_operand;
				} else {
					
					if(shift == 0x03) {
						shifter_operand = RotateRight(val_reg, sr5);
						return shifter_operand;
					}
				}
			}
		}
		
		return shifter_operand;
	}
	
	template<class CONFIG>
	inline
	typename CONFIG::reg_t
	CPU<CONFIG> ::
	ShiftOperandRegShift(const uint32_t shift_reg, 
						 const uint32_t shift, 
						 const typename CONFIG::reg_t val_reg,
						 bool *shift_carry_out) {
		typename CONFIG::reg_t shifter_operand = 0;
		typename CONFIG::reg_t sr8 = (shift_reg & 0x0FF);
		typename CONFIG::reg_t sr5 = (shift_reg & 0x01F);
		
		if(shift == 0x0) {
			if(sr8 == 0) {
				shifter_operand = val_reg;
				*shift_carry_out = GetCPSR_C();
			} else if(sr8 < 32) {
				shifter_operand = val_reg << sr8;
				*shift_carry_out = ((val_reg & (1 << (32 - sr8))) != 0);
			} else if(sr8 == 32) {
				shifter_operand = 0;
				*shift_carry_out = ((val_reg & 0x01) != 0);
			} else {
				shifter_operand = 0;
				*shift_carry_out = false;
			}
			return shifter_operand;
		} else {
			if(shift == 0x01) {
				if(sr8 == 0) {
					shifter_operand = val_reg;
					*shift_carry_out = GetCPSR_C();
				} else if(sr8 < 32) {
					shifter_operand = val_reg >> sr8;
					*shift_carry_out = ((val_reg & (1 << (sr8 - 1))) != 0);
				} else if(sr8 == 32) {
					shifter_operand = 0;
					*shift_carry_out = ((val_reg & (1 << 31)) != 0);
				} else {
					shifter_operand = 0;
					*shift_carry_out = false;
				}
				return shifter_operand;
			} else {
				
				if(shift == 0x02) {
					if(sr8 == 0) {
						shifter_operand = val_reg;
						*shift_carry_out = GetCPSR_C();
					} else if(sr8 < 32) {
						shifter_operand = ((typename CONFIG::sreg_t)val_reg) >> sr8;
						*shift_carry_out = ((val_reg & ((typename CONFIG::reg_t)1 << (sr8 - 1))) != 0);
					} else {
						if((val_reg & ((typename CONFIG::reg_t)1 << 31)) == 0) {
							shifter_operand = 0;
							*shift_carry_out = ((val_reg & ((typename CONFIG::reg_t)1 << 31)) != 0);
						} else {
							shifter_operand = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
							*shift_carry_out = ((val_reg & ((typename CONFIG::reg_t)1 << 31)) != 0);
						}
					}
					return shifter_operand;
				} else {
					
					if(shift == 0x03) {
						shifter_operand = RotateRight(val_reg, sr5);
						if(sr8 == 0) {
							*shift_carry_out = GetCPSR_C();
						} else {
							*shift_carry_out = (val_reg & (1 << ((sr5 - 1) & 0x01F)));
						}
						return shifter_operand;
					}
				}
			}
		}
		return 0;
	}
	
	/* Address operand decoding */
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	LSWUBImmOffset(const uint32_t u, 
				   const typename CONFIG::reg_t val_reg, 
				   const uint32_t offset) {
		return val_reg + (((u << 1) - 1) * offset);
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	LSWUBReg(const uint32_t u, 
			 const typename CONFIG::reg_t val_rn, 
			 const typename CONFIG::reg_t val_rd, 
			 const uint32_t shift_imm, 
			 const uint32_t shift, 
			 const typename CONFIG::reg_t val_rm) {
		
		if((shift_imm == 0) && (shift == 0)) {
			return val_rn + ((( u << 1) - 1) * val_rm);
		}
		
		typename CONFIG::reg_t index = 0;
		switch(shift) {
			case 0:
				index = val_rm << shift_imm;
				// index |= val_rm >> (32 - shift_imm);
				break;
			case 1:
				if(shift_imm == 0)
					index = 0;
				else {
					index = val_rm >> shift_imm;
					//       index |= val_rm << (32 - shift_imm);
				}
				break;
			case 2:
				if(shift_imm == 0) {
					if((val_rm & (1 << ((sizeof(typename CONFIG::reg_t) * 8) - 1))) != 0) {
						index = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
					} else {
						index = 0;
					}
				} else {
					index = ((typename CONFIG::sreg_t)val_rm) >> shift_imm;
				}
				break;
			case 3:
				if(shift_imm == 0) {
					if(GetCPSR_C()) index = 1 << ((sizeof(typename CONFIG::reg_t) * 8) - 1);
					index |= val_rm >> 1;
				} else {
					index = RotateRight(val_rm, shift_imm);
				}
				break;
			default:
				logger << DebugError
				<< "(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
				<< "unknown shift value (" << shift << ")"
				<< EndDebugError;
				Stop(-1);
		}
		return val_rn + (((u << 1) - 1) * index);
	}
	
	/* Load/store operand decoding */
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	MLSImmOffset(const uint32_t u, 
				 const typename CONFIG::reg_t val_reg, 
				 const uint32_t immedH,
				 const uint32_t immedL) {
		uint32_t offset_8 = (immedH << 4) | immedL;
		return val_reg + (((u << 1) - 1) * offset_8);
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	MLSReg(const uint32_t u, 
		   const typename CONFIG::reg_t val_rn, 
		   const typename CONFIG::reg_t val_rd, 
		   const typename CONFIG::reg_t val_rm) {
		return val_rn + (((u << 1) - 1) * val_rm);
	}
	
	/* Load/sotre multiple operand decoding */
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	LSMia(const typename CONFIG::reg_t val_reg,
		  const uint32_t reg_list,
		  typename CONFIG::address_t *start_address,
		  typename CONFIG::address_t *end_address,
		  typename CONFIG::reg_t *new_val_reg) {
		uint32_t num_regs;
		
		*start_address = val_reg;
		num_regs = 0;
		for(uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1) {
			if(mask & reg_list) num_regs++;
		}
		*end_address = val_reg + (num_regs * 4) - 4;
		*new_val_reg = val_reg + (num_regs * 4);
		
		return num_regs;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	LSMib(const typename CONFIG::reg_t val_reg,
		  const uint32_t reg_list,
		  typename CONFIG::address_t *start_address,
		  typename CONFIG::address_t *end_address,
		  typename CONFIG::reg_t *new_val_reg) {
		uint32_t num_regs;
		
		*start_address = val_reg + 4;
		num_regs = 0;
		for(uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1) {
			if(mask & reg_list) num_regs++;
		}
		*end_address = val_reg + (num_regs * 4);
		*new_val_reg = val_reg + (num_regs * 4);
		
		return num_regs;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	LSMda(const typename CONFIG::reg_t val_reg,
		  const uint32_t reg_list,
		  typename CONFIG::address_t *start_address,
		  typename CONFIG::address_t *end_address,
		  typename CONFIG::reg_t *new_val_reg) {
		uint32_t num_regs;
		
		num_regs = 0;
		for(uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1) {
			if(mask & reg_list) num_regs++;
		}
		*start_address = val_reg - (num_regs * 4) + 4;
		*end_address = val_reg;
		*new_val_reg = val_reg - (num_regs * 4);
		
		return num_regs;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	LSMdb(const typename CONFIG::reg_t val_reg,
		  const uint32_t reg_list,
		  typename CONFIG::address_t *start_address,
		  typename CONFIG::address_t *end_address,
		  typename CONFIG::reg_t *new_val_reg) {
		uint32_t num_regs;
		
		num_regs = 0;
		for(uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1) {
			if(mask & reg_list) num_regs++;
		}
		*start_address = val_reg - (num_regs * 4);
		*end_address = val_reg - 4;
		*new_val_reg = val_reg - (num_regs * 4);
		
		return num_regs;
	}
	
	/* Coprocessor load/store operand decoding */
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	CLSOpDec(const uint32_t u,
			 const uint32_t val_reg,
			 const uint32_t offset) {
		typename CONFIG::address_t res;
		
		res = val_reg + (((u << 1) - 1) * (offset * 4));
		
		return res;
	}
	
	/**************************************************************/
	/* Operand decoding methods     END                           */
	/**************************************************************/
	
	/**************************************************************/
	/* Disassembling methods     START                            */
	/**************************************************************/
	
	/* Condition opcode bytes disassembling method */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmCondition(const uint32_t cond, stringstream &buffer) {
		switch(cond) {
			case COND_EQ:
				buffer << "eq";
				break;
			case COND_NE:
				buffer << "ne";
				break;
			case COND_CS_HS:
				buffer << "cs/hs";
				break;
			case COND_CC_LO:
				buffer << "cc/lo";
				break;
			case COND_MI:
				buffer << "mi";
				break;
			case COND_PL:
				buffer << "pl";
				break;
			case COND_VS:
				buffer << "vs";
				break;
			case COND_VC:
				buffer << "vc";
				break;
			case COND_HI:
				buffer << "hi";
				break;
			case COND_LS:
				buffer << "ls";
				break;
			case COND_GE:
				buffer << "ge";
				break;
			case COND_LT:
				buffer << "lt";
				break;
			case COND_GT:
				buffer << "gt";
				break;
			case COND_LE:
				buffer << "le";
				break;
			case COND_AL:
				// buffer << "al";
				break;
			default:
				cerr << "ERROR(" << __FUNCTION__ << "): "
				<< "unknown condition code (" << cond << ")" << endl;
				Stop(-1);
				break;
		}
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmConditionFieldsMask(const uint32_t mask,
							  stringstream &buffer) {
		if((mask & 0x01) == 0x01) buffer << "c";
		if((mask & 0x02) == 0x02) buffer << "x";
		if((mask & 0x04) == 0x04) buffer << "s";
		if((mask & 0x08) == 0x08) buffer << "f";
	}
	
	/* Data processing operand disassembling methods */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmShiftOperand32Imm(const uint32_t rotate_imm, 
							const uint32_t imm, 
							stringstream &buffer) {
		uint32_t imm_r, imm_l; // immediate right and left rotated
		uint32_t imm_f; // final immediate
		
		imm_r = imm >> (rotate_imm * 2);
		imm_l = imm << (32 - (rotate_imm * 2));
		
		imm_f = imm_r | imm_l;
		buffer << "#" << dec << imm_f;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmShiftOperandImmShift(const uint32_t shift_imm, 
							   const uint32_t shift, 
							   const uint32_t rm, 
							   stringstream &buffer) {
		buffer << "r" << rm;
		if((shift_imm == 0) && (shift == 0)) {
			return;
		}
		
		if((shift_imm == 0) && (shift == 0x01)) {
			buffer << ", rrx";
			return;
		}
		
		buffer << ", ";
		switch(shift) {
			case 0x00:
				buffer << "lsl";
				break;
			case 0x01:
				buffer << "lsr";
				break;
			case 0x02:
				buffer << "asr";
				break;
			case 0x03:
				buffer << "ror";
				break;
			default:
				cerr << "ERROR(" << __FUNCTION__ << "): ";
				cerr << "unexpected case found disassembling (shift val = " 
				<< dec << shift << ")." << endl;
				Stop(-1);
		}
		
		buffer << " #" << dec << shift_imm;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmShiftOperandRegShift(const uint32_t rs, 
							   const uint32_t shift, 
							   const uint32_t rm, 
							   stringstream &buffer) {
		buffer << "r" << rm;
		buffer << ", ";
		switch(shift) {
			case 0x00:
				buffer << "lsl";
				break;
			case 0x01:
				buffer << "lsr";
				break;
			case 0x02:
				buffer << "asr";
				break;
			case 0x03:
				buffer << "ror";
				break;
			default:
				cerr << "ERROR(" << __FUNCTION__ << "): ";
				cerr << "unexpected case found disassembling (shift val = " 
				<< hex << shift << dec << ")." << endl;
				break;
		}
		buffer << " r" << rs;
	}
	
	/* Load/store operand disassembling methods */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmLSWUBImmOffset_post(const uint32_t u, 
							  const uint32_t rn, 
							  const uint32_t offset,
							  stringstream &buffer) {
		buffer << "[r" << rn << "], "
		<< "#" << (u == 0 ? "-" : "") << dec << offset;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmLSWUBImmOffset_offset(const uint32_t u, 
								const uint32_t rn, 
								const uint32_t offset,
								stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< "#" << (u == 0 ? "-" : "") << dec << offset << "]";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmLSWUBImmOffset_pre(const uint32_t u, 
							 const uint32_t rn, 
							 const uint32_t offset,
							 stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< "#" << (u == 0 ? "-" : "") << dec << offset << "]!";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmLSWUBReg_post(const uint32_t u, 
						const uint32_t rn, 
						const uint32_t shift_imm, 
						const uint32_t shift, 
						const uint32_t rm,
						stringstream &buffer) {
		buffer << "[r" << rn << "], "
		<< (u == 0 ? "-" : "") << "r" << rm;
		if(!((shift_imm == 0) && (shift == 0))) {
			buffer << ", ";
			switch(shift) {
				case 0:
					buffer << "lsl";
					break;
				case 1:
					buffer << "lsr";
					break;
				case 2:
					buffer << "asr";
					break;
				case 3:
					if(shift_imm == 0) buffer << "rrx";
					else buffer << "ror";
					break;
				default:
					cerr << "ERROR(" << __FUNCTION__ << "): "
					<< "unknown shift value (" << dec << shift << ")" << endl;
					Stop(-1);
					break;
			}
			if((shift != 3) && (shift_imm != 0))
				buffer << " #" << shift_imm;
		}
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmLSWUBReg_offset(const uint32_t u, 
						  const uint32_t rn, 
						  const uint32_t shift_imm, 
						  const uint32_t shift, 
						  const uint32_t rm,
						  stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< (u == 0 ? "-" : "") << "r" << rm;
		if(!((shift_imm == 0) && (shift == 0))) {
			buffer << ", ";
			switch(shift) {
				case 0:
					buffer << "lsl";
					break;
				case 1:
					buffer << "lsr";
					break;
				case 2:
					buffer << "asr";
					break;
				case 3:
					if(shift_imm == 0) buffer << "rrx";
					else buffer << "ror";
					break;
				default:
					cerr << "ERROR(" << __FUNCTION__ << "): "
					<< "unknown shift value (" << dec << shift << ")" << endl;
					Stop(-1);
					break;
			}
			if(shift != 3 && shift_imm != 0)
				buffer << " #" << shift_imm;
		}
		buffer << "]";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmLSWUBReg_pre(const uint32_t u, 
					   const uint32_t rn, 
					   const uint32_t shift_imm, 
					   const uint32_t shift, 
					   const uint32_t rm,
					   stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< (u == 0 ? "-" : "") << "r" << rm;
		if(!((shift_imm == 0) && (shift == 0))) {
			buffer << ", ";
			switch(shift) {
				case 0:
					buffer << "lsl";
					break;
				case 1:
					buffer << "lsr";
					break;
				case 2:
					buffer << "asr";
					break;
				case 3:
					if(shift_imm == 0) buffer << "rrx";
					else buffer << "ror";
					break;
				default:
					cerr << "ERROR(" << __FUNCTION__ << "): "
					<< "unknown shift value (" << dec << shift << ")" << endl;
					Stop(-1);
					break;
			}
			if((shift != 3) && (shift_imm != 0))
				buffer << " #" << shift_imm;
		}
		buffer << "]!";
	}
	
	/* Miscellaneous load/store operand disassembling methods */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmMLSImmOffset_post(const uint32_t u, 
							const uint32_t rn,
							const uint32_t immedH,
							const uint32_t immedL,
							stringstream &buffer) {
		buffer << "[r" << rn << "], "
		<< "#" << ((u == 1) ? "" : "-")
		<< (immedH << 4) + immedL;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmMLSImmOffset_offset(const uint32_t u, 
							  const uint32_t rn, 
							  const uint32_t immedH,
							  const uint32_t immedL,
							  stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< "#" << ((u == 1) ? "" : "-")
		<< (immedH << 4) + immedL << "]";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmMLSImmOffset_pre(const uint32_t u, 
						   const uint32_t rn, 
						   const uint32_t immedH,
						   const uint32_t immedL,
						   stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< "#" << ((u == 1) ? "" : "-")
		<< (immedH << 4) + immedL << "]!";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmMLSReg_post(const uint32_t u, 
					  const uint32_t rn, 
					  const uint32_t rm,
					  stringstream &buffer) {
		buffer << "[r" << rn << "], "
		<< ((u == 1) ? "" : "-") << "r" << rm;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmMLSReg_offset(const uint32_t u, 
						const uint32_t rn, 
						const uint32_t rm,
						stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< ((u == 1) ? "" : "-") << "r" << rm << "]";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmMLSReg_pre(const uint32_t u, 
					 const uint32_t rn, 
					 const uint32_t rm,
					 stringstream &buffer) {
		buffer << "[r" << rn << ", "
		<< ((u == 1) ? "" : "-") << "r" << rm << "]!";
	}
	
	/* Coprocessor load/store operand disassembling methods */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmCLSImm_post(const uint32_t u,
					  const uint32_t rn,
					  const uint32_t offset,
					  stringstream &buffer) {
		
		buffer << "[r" << rn << "], #"
		<< ((u == 1) ? "" : "-") << offset * 4;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmCLSImm_offset(const uint32_t u,
						const uint32_t rn,
						const uint32_t offset,
						stringstream &buffer) {
		buffer << "[r" << rn << ", #"
		<< ((u == 1) ? "" : "-") << offset * 4 << "]";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmCLSImm_pre(const uint32_t u,
					 const uint32_t rn,
					 const uint32_t offset,
					 stringstream &buffer) {
		buffer << "[r" << rn << ", #"
		<< ((u == 1) ? "" : "-") << offset * 4 << "]!";
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	DisasmCLSUnindexed(const uint32_t rn,
					   const uint32_t option,
					   stringstream &buffer) {
		buffer << "[r" << rn << "], "
		<< "{" << option << "}";
	}
	
	/**************************************************************/
	/* Disassembling methods     END                              */
	/**************************************************************/
	
	/**************************************************************/
	/* Registers access methods    START                          */
	/**************************************************************/
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	InitGPR() {
		for(unsigned int i = 0; i < num_log_gprs; i++) 
			gpr[i] = 0;
		
		for(unsigned int i = 0; i < num_phys_gprs; i++) 
			phys_gpr[i] = 0;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetGPRMapping(uint32_t src_mode, uint32_t tar_mode) {
		//   /* gpr organization per running mode:
		//    * - user:           0-14 (R0-R14),                  15 (PC)
		//    * - system:         0-14 (R0-R14),                  15 (PC)
		//    * - supervisor:     0-12 (R0-R12), 16-17 (R13-R14), 15 (PC)
		//    * - abort:          0-12 (R0-R12), 18-19 (R13-R14), 15 (PC)
		//    * - undefined:      0-12 (R0-R12), 20-21 (R13-R14), 15 (PC)
		//    * - interrupt:      0-12 (R0-R12), 22-23 (R13-R14), 15 (PC)
		//    * - fast interrupt: 0-7 (R0-R7),   24-30 (R8-R14),  15 (PC)
		//    */
		
		/* Backup current running mode */
		switch(src_mode) {
			case USER_MODE:
			case SYSTEM_MODE:
				for(unsigned int i = 0; i < 16; i++) {
					phys_gpr[i] = gpr[i];
				}
				break;
			case SUPERVISOR_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					phys_gpr[i] = gpr[i];
				}
				phys_gpr[16] = gpr[13];
				phys_gpr[17] = gpr[14];
				phys_gpr[15] = gpr[15];
				break;
			case ABORT_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					phys_gpr[i] = gpr[i];
				}
				phys_gpr[18] = gpr[13];
				phys_gpr[19] = gpr[14];
				phys_gpr[15] = gpr[15];
				break;
			case UNDEFINED_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					phys_gpr[i] = gpr[i];
				}
				phys_gpr[20] = gpr[13];
				phys_gpr[21] = gpr[14];
				phys_gpr[15] = gpr[15];
				break;
			case IRQ_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					phys_gpr[i] = gpr[i];
				}
				phys_gpr[22] = gpr[13];
				phys_gpr[23] = gpr[14];
				phys_gpr[15] = gpr[15];
				break;
			case FIQ_MODE:
				for(unsigned int i = 0; i < 8; i++) {
					phys_gpr[i] = gpr[i];
				}
				for(unsigned int i = 0; i < 7; i++) {
					phys_gpr[24 + i] = gpr[8 + i];
				}
				phys_gpr[15] = gpr[15];
				break;
			default:
				const char *c_tar_mode = "unknown";
				switch(tar_mode)
				{
				case USER_MODE:
					c_tar_mode = "USER_MODE";
					break;
				case SYSTEM_MODE:
					c_tar_mode = "SYSTEM_MODE";
					break;
				case SUPERVISOR_MODE:
					c_tar_mode = "SUPERVISOR_MODE";
					break;
				case ABORT_MODE:
					c_tar_mode = "ABORT_MODE";
					break;
				case UNDEFINED_MODE:
					c_tar_mode = "UNDEFINED_MODE";
					break;
				case IRQ_MODE:
					c_tar_mode = "IRQ_MODE";
					break;
				case FIQ_MODE:
					c_tar_mode = "FIQ_MODE";
					break;
				}
				logger << DebugError << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
					<< " unknown running mode (0x" << hex << src_mode << dec << "), target mode = " << c_tar_mode
					<< EndDebug;
				Stop(-1);
				break;
		}
		
		/* Update registers to the target mode */
		switch(tar_mode) {
			case USER_MODE:
			case SYSTEM_MODE:
				for(unsigned int i = 0; i < 16; i++) {
					gpr[i] = phys_gpr[i];
				}
				break;
			case SUPERVISOR_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					gpr[i] = phys_gpr[i];
				}
				gpr[13] = phys_gpr[16];
				gpr[14] = phys_gpr[17];
				gpr[15] = phys_gpr[15];
				break;
			case ABORT_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					gpr[i] = phys_gpr[i];
				}
				gpr[13] = phys_gpr[18];
				gpr[14] = phys_gpr[19];
				gpr[15] = phys_gpr[15];
				break;
			case UNDEFINED_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					gpr[i] = phys_gpr[i];
				}
				gpr[13] = phys_gpr[20];
				gpr[14] = phys_gpr[21];
				gpr[15] = phys_gpr[15];
				break;
			case IRQ_MODE:
				for(unsigned int i = 0; i < 13; i++) {
					gpr[i] = phys_gpr[i];
				}
				gpr[13] = phys_gpr[22];
				gpr[14] = phys_gpr[23];
				gpr[15] = phys_gpr[15];
				break;
			case FIQ_MODE:
				for(unsigned int i = 0; i < 8; i++) {
					gpr[i] = phys_gpr[i];
				}
				for(unsigned int i = 0; i < 7; i++) {
					gpr[8 + i] = phys_gpr[24 + i];
				}
				gpr[15] = phys_gpr[15];
				break;
			default:
				const char *c_src_mode = "unknown";
				switch(src_mode)
			{
				case USER_MODE:
					c_src_mode = "USER_MODE";
					break;
				case SYSTEM_MODE:
					c_src_mode = "SYSTEM_MODE";
					break;
				case SUPERVISOR_MODE:
					c_src_mode = "SUPERVISOR_MODE";
					break;
				case ABORT_MODE:
					c_src_mode = "ABORT_MODE";
					break;
				case UNDEFINED_MODE:
					c_src_mode = "UNDEFINED_MODE";
					break;
				case IRQ_MODE:
					c_src_mode = "IRQ_MODE";
					break;
				case FIQ_MODE:
					c_src_mode = "FIQ_MODE";
					break;
			}
				logger << DebugError << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
				<< " unknown target mode (0x" << hex << tar_mode << dec << "), source mode = " << c_src_mode
				<< EndDebug;
				Stop(-1);
				break;
		}
	}
	
	template<class CONFIG>
	typename CONFIG::reg_t 
	CPU<CONFIG> ::
	GetGPR_usr(uint32_t id) const {
		return gpr[id];
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetGPR_usr(uint32_t id, typename CONFIG::reg_t val) {
		gpr[id] = val;
	}
	
	template<class CONFIG>
	const string 
	CPU<CONFIG> ::
	DumpRegs() const {
		ostringstream str;
		
		for(unsigned int i = 0; i < 16; i++) {
			str << "r" << i << " = 0x" << hex << GetGPR(i) << dec << ", ";
		}
		
		//  for(unsigned int i = 0; i < 4; i++) {
		//    str << "r" << (i * 4) << " = 0x" << hex << GetGPR(i * 4) << dec; 
		//    for(unsigned int j = 1; j < 4; j++) {
		//      str << "\t r" << ((i * 4) + j) << " = 0x" << hex << GetGPR((i * 4) + j) << dec; 
		//    }
		//    str << endl;
		//  }
		
		str << "cpsr = (" << hex << GetCPSR() << dec << ") ";
		typename CONFIG::reg_t mask;
		for(mask = 0x80000000; mask != 0; mask = mask >> 1) {
			if((mask & GetCPSR()) != 0) str << "1";
			else str << "0";
		}
		//  str << endl;
		
		return str.str();
	}
	
	/* GPR access functions */
	template<class CONFIG>
	typename CONFIG::reg_t 
	CPU<CONFIG> ::
	GetGPR(uint32_t id) const {
		return gpr[id];
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	SetGPR(uint32_t id, typename CONFIG::reg_t val) {
		gpr[id] = val;
	}
	
	/* CPSR access functions */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR(uint32_t val) {
		cpsr = val;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_NZCV(bool n,
				 bool z,
				 bool c,
				 bool v) {
		if(n) cpsr = cpsr | CPSR_N_MASK;
		else cpsr = cpsr & ~(CPSR_N_MASK);
		if(z) cpsr = cpsr | CPSR_Z_MASK;
		else cpsr = cpsr & ~(CPSR_Z_MASK);
		if(c) cpsr = cpsr | CPSR_C_MASK;
		else cpsr = cpsr & ~(CPSR_C_MASK);
		if(v) cpsr = cpsr | CPSR_V_MASK;
		else cpsr = cpsr & ~(CPSR_V_MASK);
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	SetCPSR_N(const bool val) {
		if(val) cpsr = cpsr | CPSR_N_MASK;
		else cpsr = cpsr & ~(CPSR_N_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_N() {
		cpsr = cpsr & ~(CPSR_N_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_N() {
		return (cpsr & CPSR_N_MASK) == CPSR_N_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_Z(const bool val) {
		if(val) cpsr = cpsr | CPSR_Z_MASK;
		else cpsr = cpsr & ~(CPSR_Z_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_Z() {
		cpsr = cpsr & ~(CPSR_Z_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_Z() {
		return (cpsr & CPSR_Z_MASK) == CPSR_Z_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_C(const bool val) {
		if(val) cpsr = cpsr | CPSR_C_MASK;
		else cpsr = cpsr & ~(CPSR_C_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_C() {
		cpsr = cpsr & ~(CPSR_C_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_C() {
		return (cpsr & CPSR_C_MASK) == CPSR_C_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_V(const bool val) {
		if(val) cpsr = cpsr | CPSR_V_MASK;
		else cpsr = cpsr & ~(CPSR_V_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_V() {
		cpsr = cpsr & ~(CPSR_V_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_V() {
		return (cpsr & CPSR_V_MASK) == CPSR_V_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_Q(const bool val) {
		if(val) cpsr = cpsr | CPSR_Q_MASK;
		else cpsr = cpsr & ~(CPSR_Q_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_Q() {
		cpsr = cpsr & ~(CPSR_Q_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_Q() {
		return (cpsr & CPSR_Q_MASK) == CPSR_Q_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_I(const bool val) {
		if(val) cpsr = cpsr | CPSR_I_MASK;
		else cpsr = cpsr & ~(CPSR_I_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_I() {
		cpsr = cpsr & ~(CPSR_I_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_I() {
		return (cpsr & CPSR_I_MASK) == CPSR_I_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_F(const bool val) {
		if(val) cpsr = cpsr | CPSR_F_MASK;
		else cpsr = cpsr & ~(CPSR_F_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetCPSR_F() {
		cpsr = cpsr & ~(CPSR_F_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_F() {
		return (cpsr & CPSR_F_MASK) == CPSR_F_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_T(const bool val) {
		if(val) cpsr = cpsr | CPSR_T_MASK;
		else cpsr = cpsr & ~(CPSR_T_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> :: 
	UnsetCPSR_T() {
		cpsr = cpsr & ~(CPSR_T_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetCPSR_T() {
		return (cpsr & CPSR_T_MASK) == CPSR_T_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetCPSR_Mode(uint32_t mode) {
		cpsr = cpsr & ~(CPSR_RUNNING_MODE_MASK);
		cpsr = cpsr | mode;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	GetCPSR_Mode() {
		uint32_t mode = cpsr & CPSR_RUNNING_MODE_MASK;
		return mode;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	GetCPSR() const {
		return cpsr;
	}
	
	/* SPSR access functions */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR(uint32_t val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = val;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_NZCV(bool n,
				 bool z,
				 bool c,
				 bool v) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(n) spsr[rm] = spsr[rm] | SPSR_N_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_N_MASK);
		if(z) spsr[rm] = spsr[rm] | SPSR_Z_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_Z_MASK);
		if(c) spsr[rm] = spsr[rm] | SPSR_C_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_C_MASK);
		if(v) spsr[rm] = spsr[rm] | SPSR_V_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_V_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_N(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_N_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_N_MASK);
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	UnsetSPSR_N() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_N_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_N() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_N_MASK) == SPSR_N_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_Z(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_Z_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_Z_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_Z() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_Z_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_Z() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_Z_MASK) == SPSR_Z_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_C(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_C_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_C_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_C() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_C_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_C() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_C_MASK) == SPSR_C_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_V(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_V_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_V_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_V() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_V_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_V() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_V_MASK) == SPSR_V_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_Q(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_Q_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_Q_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_Q() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_Q_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_Q() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_Q_MASK) == SPSR_Q_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_I(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_I_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_I_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_I() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_I_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_I() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_I_MASK) == SPSR_I_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_F(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_F_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_F_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_F() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_F_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_F() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_F_MASK) == SPSR_F_MASK;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_T(const bool val) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		if(val) spsr[rm] = spsr[rm] | SPSR_T_MASK;
		else spsr[rm] = spsr[rm] & ~(SPSR_T_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UnsetSPSR_T() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_T_MASK);
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	GetSPSR_T() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return (spsr[rm] & SPSR_T_MASK) == SPSR_T_MASK;
	}
	
	template<class CONFIG>
	uint32_t
	CPU<CONFIG> ::
	GetSPSR_Mode() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return spsr[rm] & (SPSR_RUNNING_MODE_MASK);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	SetSPSR_Mode(uint32_t mode) {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		spsr[rm] = spsr[rm] & ~(SPSR_RUNNING_MODE_MASK);
		spsr[rm] = spsr[rm] | mode;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	GetSPSR() {
		unsigned int rm;
		
		rm = GetSPSRIndex();
		
		return spsr[rm];
	}
	
	/* gets spsr index from current running mode */
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	GetSPSRIndex() {
		uint32_t rm;
		
		switch(cpsr & CPSR_RUNNING_MODE_MASK) {
			case USER_MODE:
				logger << DebugError
				<< "Trying to modify SPSR under USER_MODE" << endl
				<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
				<< EndDebugError;
				Stop(-1);
				break;
			case SYSTEM_MODE:
				logger << DebugError
				<< "Trying to modify SPSR under SYSTEM_MODE" << endl
				<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
				<< EndDebugError;
				Stop(-1);
				break;
			case SUPERVISOR_MODE:
				rm = 0;
				break;
			case ABORT_MODE:
				rm = 1;
				break;
			case UNDEFINED_MODE:
				rm = 2;
				break;
			case IRQ_MODE:
				rm = 3;
				break;
			case FIQ_MODE:
				rm = 4;
				break;
			default:
				logger << DebugError
				<< "unkonwn running mode." << endl
				<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
				<< EndDebugError;
				Stop(-1);
				break;
		}
		
		return rm;
	}
	
	/* other CPSR/SPSR methods */
	template<class CONFIG>
	void CPU<CONFIG> ::
	MoveSPSRtoCPSR() {
		/* SPSR needs to be moved to CPSR
		 * This means that we need to change the register mapping if the running mode has changed
		 */
		uint32_t src_mode = GetCPSR_Mode();
		uint32_t dst_mode = GetSPSR_Mode();
		uint32_t cur_spsr = GetSPSR();
		SetCPSR(cur_spsr);
		if(src_mode != dst_mode)
			SetGPRMapping(src_mode, dst_mode);
	}
	
	/* Instruction condition checking method */
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CheckCondition(uint32_t cond) { // check condition type
		//	return CheckCondition(cond, GetCPSR());
		return (cond == 0xe) || ((check_condition_table[cond] >> (cpsr >> 28)) & 1);
	}
	
	/* Condition codes update method */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	UpdateConditionCodes() {
		cout << "TODO: "
		<< __FUNCTION__ << ":"
		<< __FILE__ << ":"
		<< __LINE__ << endl;
	}
	
	/* Result condition methods */
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CarryFrom(const typename CONFIG::reg_t res, const typename CONFIG::reg_t s1, const typename CONFIG::reg_t s2,
			  const typename CONFIG::reg_t carry_in) {
		if((res < s1) || (res < s2)) return true;
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	BorrowFrom(const typename CONFIG::reg_t res, const typename CONFIG::reg_t s1, const typename CONFIG::reg_t s2,
			   const typename CONFIG::reg_t carry_in) {
		if(s1 > s2) 
			return false;
		else {
			if(s1 == s2)
				if(carry_in) return true;
				else return false;
				else return true;
		}
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	AdditionOverflowFrom(const typename CONFIG::reg_t res, const typename CONFIG::reg_t s1, const typename CONFIG::reg_t s2,
						 const typename CONFIG::reg_t carry_in) {
		if(((((int32_t)s1 >= 0) &&
			 ((int32_t)s2 >= 0)) ||
			(((int32_t)s1 < 0) &&
			 ((int32_t)s2 < 0))) &&
		   (((((int32_t)res >= 0) &&
			  ((int32_t)s1 < 0)) ||
			 (((int32_t)res < 0) &&
			  ((int32_t)s1 >= 0))))) return true;
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	SubtractionOverflowFrom(const typename CONFIG::reg_t res, const typename CONFIG::reg_t s1, const typename CONFIG::reg_t s2,
							const typename CONFIG::reg_t carry_in) {
		if((((((int32_t)s1) < 0) &&
			 (((int32_t)s2) >= 0)) ||
			((((int32_t)s1) >= 0) &&
			 (((int32_t)s2) < 0))) &&
		   ((((((int32_t)res) >= 0) &&
			  (((int32_t)s1) < 0)) ||
			 ((((int32_t)res) < 0) &&
			  (((int32_t)s1) >= 0))))) return true;
		return false;
	}
	
	/**************************************************************/
	/* Registers access methods    END                            */
	/**************************************************************/
	
	/**************************************************************/
	/* Memory access methods       START                          */
	/**************************************************************/
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	ReadInsn(address_t address, uint16_t &val) 
	{
		if ( CONFIG::MODEL == ARM926EJS && linux_os_import )
		{
			// we are running simulation the linux OS
			//   tcm memories are ignored on this mode
			// check the instruction cache
			uint32_t cache_tag = arm926ejs_icache.GetTag(address);
			uint32_t cache_set = arm926ejs_icache.GetSet(address);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( arm926ejs_icache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_icache.GetValid(cache_set, cache_way) )
				{
					// the data is in the cache, just read it
					uint32_t cache_index =
						arm926ejs_icache.GetIndex(address);
					uint32_t read_data_size =
						arm926ejs_icache.GetDataCopy(cache_set, cache_way, cache_index, 2, (uint8_t *)&val);
					cache_hit = true;
					if ( unlikely(read_data_size != 2 ) )
					{
						logger << DebugWarning
							<< "While reading instruction cache, only " << (unsigned int)read_data_size
							<< " byte(s) could be read, instead of the 2 that were requested (base address = 0x"
							<< (unsigned int)address << ")"
							<< EndDebugWarning;
					}
				}
			}
			if ( unlikely(!cache_hit) )
			{
				memory_interface->PrRead(address, (uint8_t *)&val, 2);
			}

			if ( unlikely(il1_power_estimator_import != 0) )
				il1_power_estimator_import->ReportReadAccess();

			return;
		}
		if ( CONFIG::MODEL == ARM926EJS && !linux_os_import )
		{
			// TODO
		}
		memory_interface->PrRead(address, (uint8_t *)&val, 2);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	ReadInsn(address_t address, uint32_t &val) 
	{
		if ( CONFIG::MODEL == ARM926EJS && linux_os_import )
		{
			if ( unlikely(verbose_memory) )
				logger << DebugInfo
					<< "Fetching 0x" << hex << address << dec
					<< EndDebugInfo;
			// we are running simulation the linux OS
			//   tcm memories are ignored on this mode
			// check the instruction cache
			uint32_t cache_tag = arm926ejs_icache.GetTag(address);
			uint32_t cache_set = arm926ejs_icache.GetSet(address);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( arm926ejs_icache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_icache.GetValid(cache_set, cache_way) )
				{
					// the data is in the cache, just read it
					uint32_t cache_index =
						arm926ejs_icache.GetIndex(address);
					uint32_t read_data_size =
						arm926ejs_icache.GetDataCopy(cache_set, cache_way, cache_index, 4, (uint8_t *)&val);
					cache_hit = true;
					if ( unlikely(read_data_size != 4 ) )
					{
						logger << DebugWarning
								<< "While reading instruction cache, only "
								<< (unsigned int)read_data_size
								<< " byte(s) could be read, instead of the 4 "
								<< " that were requested (base address = 0x"
								<< (unsigned int)address << ")"
								<< EndDebugWarning;
					}
				}
			}
			if ( unlikely(!cache_hit) )
			{
				memory_interface->PrRead(address, (uint8_t *)&val, 4);
			}
			if ( unlikely(il1_power_estimator_import != 0) )
				il1_power_estimator_import->ReportReadAccess();
			return;
		}
		
		if ( CONFIG::MODEL == ARM926EJS && !linux_os_import )
		{
			// TODO
		}
		
		memory_interface->PrRead(address, (uint8_t *)&val, 4);
	}
		
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	ReadPrefetch(address_t address) 
	{
		address = address & ~(0x3);
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetPrefetch(address);
		lsQueue.push(memop);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Read32toPCUpdateT(address_t address) 
	{
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetReadToPCUpdateT(address);
		lsQueue.push(memop);
		
		if (requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address & ~((address_t)0x3), 4);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Read32toPC(address_t address) 
	{
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetReadToPC(address);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address & ~((address_t)0x3), 4);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Read32toGPR(address_t address, uint32_t reg) 
	{
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetRead(address, 4, reg, false, false);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address & ~((address_t)0x3), 4);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Read32toGPRAligned(address_t address, uint32_t reg) 
	{
		MemoryOp<CONFIG> *memop;
		
		address = address & ~((address_t)0x3);
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetRead(address, 4, reg, true, false);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 4);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Read16toGPRAligned(address_t address, uint32_t reg) 
	{
		MemoryOp<CONFIG> *memop;
		
		address = address & ~((address_t)0x1);
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetRead(address, 2, reg, true, false);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 2);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	ReadS16toGPRAligned(address_t address, uint32_t reg) 
	{
		MemoryOp<CONFIG> *memop;
		
		address = address & ~((address_t)0x1);
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetRead(address, 2, reg, /* aligned */true, /* signed */true);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 2);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	ReadS8toGPR(address_t address, uint32_t reg) 
	{
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetRead(address, 1, reg, /* aligned */true, /* signed */true);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 1);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Read8toGPR(address_t address, uint32_t reg) 
	{
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetRead(address, 1, reg, /* aligned */true, /* signed */false);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_READ,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 1);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Write32(address_t address, uint32_t value) 
	{
		MemoryOp<CONFIG> *memop;
		
		address = address & ~((address_t)0x3);
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetWrite(address, 4, value);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_WRITE,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 4);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Write16(address_t address, uint16_t value) 
	{
		address = address & ~((address_t)0x1);
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetWrite(address, 2, value);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_WRITE,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 2);
	}
	
	template<class CONFIG> 
	void 
	CPU<CONFIG> ::
	Write8(address_t address, uint8_t value) 
	{
		MemoryOp<CONFIG> *memop;
		
		if ( freeLSQueue.empty() )
			memop = new MemoryOp<CONFIG>();
		else 
		{
			memop = freeLSQueue.front();
			freeLSQueue.pop();
		}
		memop->SetWrite(address, 1, value);
		lsQueue.push(memop);
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import ) 
				memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<uint64_t>::MAT_WRITE,
																   MemoryAccessReporting<uint64_t>::MT_DATA,
																   address, 1);
	}
	
	/**************************************************************/
	/* Memory access methods       END                            */
	/**************************************************************/
	
	/**************************************************************/
	/* Coprocessor methods          START                         */
	/**************************************************************/
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CoprocessorLoad(uint32_t cp_num, address_t address) {
		
		logger << DebugError
		<< "TODO" << endl
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
		
		Stop(-1);
		
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CoprocessorLoad(uint32_t cp_num, address_t address, uint32_t option) {
		
		logger << DebugError
		<< "TODO" << endl
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
		
		Stop(-1);
		
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CoprocessorStore(uint32_t cp_num, address_t address) {
		
		logger << DebugError
		<< "TODO" << endl
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
		
		Stop(-1);
		
		return false;
	}
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CoprocessorStore(uint32_t cp_num, address_t address, uint32_t option) {
		
		logger << DebugError
		<< "TODO" << endl
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
		
		
		Stop(-1);
		
		return false;
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	CoprocessorDataProcess(uint32_t cp_num, uint32_t op1, uint32_t op2,
						   uint32_t crd, uint32_t crn, uint32_t crm) {
		
		logger << DebugError
		<< "TODO" << endl
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
		
		Stop(-1);
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	MoveToCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
					  uint32_t rd, uint32_t crn, uint32_t crm) {
		if(cp[cp_num] == 0) {
			
			logger << DebugError
			<< "TODO" << endl
			<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
			<< EndDebugError;
			
			Stop(-1);
		}
		cp[cp_num]->WriteRegister(op1, op2, crn, crm, GetGPR(rd));
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	MoveFromCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
						uint32_t rd, uint32_t crn, uint32_t crm) {
		if(cp[cp_num] == 0) {
			logger << DebugError
			<< "TODO" << endl
			<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
			<< EndDebugError;
			
			Stop(-1);
		}
		cp[cp_num]->ReadRegister(op1, op2, crn, crm, gpr[rd]);
	}
	
	/**************************************************************/
	/* Coprocessor methods          END                           */
	/**************************************************************/
	
	/**************************************************************/
	/* Exception methods            START                         */
	/**************************************************************/
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	PerformResetException() {
		// - get current mode
		// - switch to reset mode
		// - R14_svc undefined
		// - SPSR_svc undefined
		// - set ARM state 
		// - disable fast interrupts
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		//	uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(SUPERVISOR_MODE);
		SetGPRMapping(cur_mode, SUPERVISOR_MODE);
		SetGPR(14, 0);
		SetSPSR(0);
		UnsetCPSR_T();
		SetCPSR_F();
		SetCPSR_I();
		SetGPR(PC_reg, GetResetExceptionAddr());	
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	PerformUndefInsnException() {
		// - get current mode
		// - switch to supervisor mode
		// - address of this insn + 4 ==> R14_svc (pc already set to the right value)
		// - CPSR (before switching to supervisor mode) ==> SPSR_svc
		// - set ARM state 
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(UNDEFINED_MODE);
		SetGPRMapping(cur_mode, UNDEFINED_MODE);
		SetGPR(14, GetGPR(PC_reg));
		SetSPSR(cur_cpsr);
		UnsetCPSR_T();
		SetCPSR_I();
		SetGPR(PC_reg, GetUndefInsnExceptionAddr());
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	PerformSWIException() {
		// - get current mode
		// - switch to supervisor mode
		// - address of this swi + 4 ==> R14_svc (pc already set to the right value)
		// - CPSR (before switching to supervisor mode) ==> SPSR_svc
		// - set ARM state 
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(SUPERVISOR_MODE);
		SetGPRMapping(cur_mode, SUPERVISOR_MODE);
		SetGPR(14, GetGPR(PC_reg));
		SetSPSR(cur_cpsr);
		UnsetCPSR_T();
		SetCPSR_I();
		SetGPR(PC_reg, GetSWIExceptionAddr());
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	PerformPrefetchAbortException() {
		// - get current mode
		// - switch to abort mode
		// - address of this swi + 4 ==> R14_svc (pc already set to the right value)
		// - CPSR (before switching to supervisor mode) ==> SPSR_svc
		// - set ARM state 
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(ABORT_MODE);
		SetGPRMapping(cur_mode, ABORT_MODE);
		SetGPR(14, GetGPR(PC_reg));
		SetSPSR(cur_cpsr);
		UnsetCPSR_T();
		SetCPSR_I();
		SetGPR(PC_reg, GetPrefetchAbortExceptionAddr());
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	PerformDataAbortException() {
		// - get current mode
		// - switch to abort mode
		// - address of the instrucction that produced the data abort + 8 ==> R14_svc
		// - CPSR (before switching to supervisor mode) ==> SPSR_svc
		// - set ARM state 
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(SUPERVISOR_MODE);
		SetGPRMapping(cur_mode, ABORT_MODE);
		SetGPR(14, GetGPR(PC_reg) + 0x4);
		SetSPSR(cur_cpsr);
		UnsetCPSR_T();
		SetCPSR_I();
		SetGPR(PC_reg, GetDataAbortExceptionAddr());
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	PerformIRQException() {
		// - get current mode
		// - switch to abort mode
		// - address of the next instruction to be executed + 4 ==> R14_svc
		// - CPSR (before switching to supervisor mode) ==> SPSR_svc
		// - set ARM state 
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(IRQ_MODE);
		SetGPRMapping(cur_mode, IRQ_MODE);
		SetGPR(14, GetGPR(PC_reg) + 0x4);
		SetSPSR(cur_cpsr);
		UnsetCPSR_T();
		SetCPSR_I();
		SetGPR(PC_reg, GetIRQExceptionAddr());
	}
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	PerformFIQException() {
		// - get current mode
		// - switch to reset mode
		// - address of this swi + 4 ==> R14_svc (pc already set to the right value)
		// - CPSR (before switching to supervisor mode) ==> SPSR_svc
		// - set ARM state 
		// - disable fast interrupts
		// - disable normal interrupts
		// - jump to interruption vector
		uint32_t cur_mode = GetCPSR_Mode();
		uint32_t cur_cpsr = GetCPSR();
		SetCPSR_Mode(FIQ_MODE);
		SetGPRMapping(cur_mode, FIQ_MODE);
		SetGPR(14, GetGPR(PC_reg));
		SetSPSR(cur_cpsr);
		UnsetCPSR_T();
		SetCPSR_F();
		SetCPSR_I();
		SetGPR(PC_reg, GetFIQExceptionAddr());	
	}
	
	template<class CONFIG>
	typename CONFIG::address_t
	CPU<CONFIG> ::
	GetExceptionVectorAddr() {
		// action depends on the cpu model 
		if(CONFIG::MODEL == ARM966E_S) 
			// the cp15 should be able to get us the right value
			return cp15_966es->GetExceptionVectorAddr();
		if(CONFIG::MODEL == ARM7TDMI)
			return 0x0;
		
		logger << DebugError
		<< "TODO: exception vector address reporting is not implemented "
		<< "for this architecture" << endl
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
		
		Stop(-1);
		return 0;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetResetExceptionAddr() {
		// vector + 0
		return GetExceptionVectorAddr() + (address_t)0x0;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetUndefInsnExceptionAddr() {
		// vector + 0x4
		return GetExceptionVectorAddr() + (address_t)0x04;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetSWIExceptionAddr() {
		// vector + 0x8
		return GetExceptionVectorAddr() + (address_t)0x08;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetPrefetchAbortExceptionAddr() {
		// vector + 0xc
		return GetExceptionVectorAddr() + (address_t)0x0c;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetDataAbortExceptionAddr() {
		// vector + 0x10
		return GetExceptionVectorAddr() + (address_t)0x010;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetIRQExceptionAddr() {
		// vector + 0x18
		return GetExceptionVectorAddr() + (address_t)0x018;
	}
	
	template<class CONFIG>
	typename CONFIG::address_t 
	CPU<CONFIG> ::
	GetFIQExceptionAddr() {
		// vector + 0x1c
		return GetExceptionVectorAddr() + (address_t)0x01c;
	}
	
	/**************************************************************/
	/* Exception methods            END                           */
	/**************************************************************/
	
	/**************************************************************/
	/* Condition table methods    START                           */
	/**************************************************************/
	
	template<class CONFIG>
	bool 
	CPU<CONFIG> ::
	CheckCondition(unsigned int cond, unsigned int cpsr_val) {
		switch(cond) {
			case COND_EQ:
				if((cpsr_val & CPSR_Z_MASK) == CPSR_Z_MASK)
					return true;
				break;
			case COND_NE:
				if((cpsr_val & CPSR_Z_MASK) == 0)
					return true;
				break;
			case COND_CS_HS:
				if((cpsr_val & CPSR_C_MASK) == CPSR_C_MASK)
					return true;
				break;
			case COND_CC_LO:
				if((cpsr_val & CPSR_C_MASK) == 0)
					return true;
				break;
			case COND_MI:
				if((cpsr_val & CPSR_N_MASK) == CPSR_N_MASK)
					return true;
				break;
			case COND_PL:
				if((cpsr_val & CPSR_N_MASK) == 0)
					return true;
				break;
			case COND_VS:
				if((cpsr_val & CPSR_V_MASK) == CPSR_V_MASK)
					return true;
				break;
			case COND_VC:
				if((cpsr_val & CPSR_V_MASK) == 0)
					return true;
				break;
			case COND_HI:
				if(((cpsr_val & CPSR_C_MASK) == CPSR_C_MASK) &&
				   ((cpsr_val & CPSR_Z_MASK) == 0))
					return true;
				break;
			case COND_LS:
				if(((cpsr_val & CPSR_C_MASK) == 0) ||
				   ((cpsr_val & CPSR_Z_MASK) == CPSR_Z_MASK))
					return true;
				break;
			case COND_GE:
				if((cpsr_val & (CPSR_N_MASK | CPSR_V_MASK)) == (CPSR_N_MASK | CPSR_V_MASK))
					return true;
				if((cpsr_val & (CPSR_N_MASK | CPSR_V_MASK)) == 0)
					return true;
				break;
			case COND_LT:
				if(((cpsr_val & CPSR_N_MASK) == CPSR_N_MASK) &&
				   ((cpsr_val & CPSR_V_MASK) == 0))
					return true;
				if(((cpsr_val & CPSR_N_MASK) == 0) &&
				   ((cpsr_val & CPSR_V_MASK) == CPSR_V_MASK))
					return true;
				break;
			case COND_GT:
				if((cpsr_val & CPSR_Z_MASK) == 0) {
					if((cpsr_val & (CPSR_N_MASK | CPSR_V_MASK)) == (CPSR_N_MASK | CPSR_V_MASK))
						return true;
					if((cpsr_val & (CPSR_N_MASK | CPSR_V_MASK)) == 0)
						return true;
				}
				break;
			case COND_LE:
				if((cpsr_val & CPSR_Z_MASK) == CPSR_Z_MASK) 
					return true;
				if(((cpsr_val & CPSR_N_MASK) == CPSR_N_MASK) &&
				   ((cpsr_val & CPSR_V_MASK) == 0))
					return true;
				if(((cpsr_val & CPSR_N_MASK) == 0) &&
				   ((cpsr_val & CPSR_V_MASK) == CPSR_V_MASK))
					return true;    
				break;
			case COND_AL:
				return true;
				break;
			default:
				return false;
				break;
		}
		
		return false; 
	}
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	InitializeCheckConditionTable() { // check condition type
		for(unsigned int cond = 0; cond < 16; cond++) {
			uint16_t entry = 0;
			for(uint32_t cpsr_val = 0; cpsr_val < 16; cpsr_val++) {
				uint16_t val = 0;
				if(CheckCondition(cond, cpsr_val << 28)) 
					val = 1;
				else 
					val = 0;
				val = val << cpsr_val;
				entry += val;
			}
			check_condition_table[cond] = entry;
		}
	}
	
	/**************************************************************/
	/* Condition table methods    START                           */
	/**************************************************************/
	
	/**************************************************************/
	/* Instructions size methods  START                           */
	/**************************************************************/
	
	/* Returns current instruction size 
	 * NOTE: for the moment thumb is not supported, so only an instruction
	 *   size is supported (32bits).
	 */
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	InstructionByteSize() {
		if(GetCPSR_T()) return 2; // we are running in thumb mode
		return 4;
	}
	
	template<class CONFIG>
	uint32_t 
	CPU<CONFIG> ::
	InstructionBitSize() {
		if(GetCPSR_T()) return 16; // we are running in thumb mode
		return 32;
	}
	
	/**************************************************************/
	/* Instructions size methods    END                           */
	/**************************************************************/
	
	/**************************************************************/
	/* Alignment checking methods  START                          */
	/**************************************************************/
	
	/* Check that the address is aligned
	 */
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	CheckAlignmentExcep(typename CONFIG::address_t addr) {
		//	if(CONFIG::DEBUG_ENABLE && logger_import)
		//		(*logger_import) << DebugError << LOCATION
		//			<< "TODO"
		//			<< endl << EndDebugError;
	}
	
	/**************************************************************/
	/* Alignment checking methods  END                            */
	/**************************************************************/
	
	/**************************************************************/
	/* CPUARMLinuxOSInterface methods                       START */
	/**************************************************************/
	
	template<class CONFIG>
	void 
	CPU<CONFIG> ::
	PerformExit(int ret) {
		running = false;
		Stop(ret);
	}
	
	/**************************************************************/
	/* CPUARMLinuxOSInterface methods                         END */
	/**************************************************************/
	
	/* endianness methods */
	template<class CONFIG>
	endian_type 
	CPU<CONFIG> ::
	GetEndianness() {
		return default_endianness;
		//	return CONFIG::ENDIANESS;
	}
	
	/**************************************************************/
	/* Coprocessors creation methods (private)              START */
	/**************************************************************/
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	CreateCpSystem() {
		for(unsigned int i = 0; i < 16; i++)
			cp[i] = 0;
		
		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es = new cp15_966es_t("cp15", 15, this, dtcm, itcm, memory_interface, this);
			cp[15] = cp15_966es;
		}
	}
	
	/**************************************************************/
	/* Coprocessors creation methods (private)              END   */
	/**************************************************************/
	
	/**************************************************************/
	/* TCM creation methods (private)                       START */
	/**************************************************************/
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	CreateTCMSystem() {
		dtcm = 0;
		itcm = 0;
		
		if(CONFIG::HAS_DTCM) {
			
			dtcm = new dtcm_t("dtcm", this);
		}
		
		if(CONFIG::HAS_ITCM) {
			itcm = new itcm_t("itcm", this);
		}
	}
	
	/**************************************************************/
	/* TCM creation methods (private)                       END   */
	/**************************************************************/
	
	/**************************************************************/
	/* Memory system creation methods (private)             START */
	/**************************************************************/
	
	template<class CONFIG>
	inline INLINE
	void 
	CPU<CONFIG> ::
	CreateMemorySystem() {
#if 0
		/* TODO: remake all the memory system for the different components without using the memory_interface */
		if(CONFIG::HAS_CACHE_L2) {
			cerr << "Configuring cache level 2" << endl;
			cache_l2 = new Cache<typename CONFIG::cache_l2_t>("cache_l2", memory_interface, this);
			// cache_l2->memory_import >> memory_import;
			if(CONFIG::HAS_INSN_CACHE_L1) {
				cerr << "Configuring instruction cache level 1" << endl;
				cache_il1 = new Cache<typename CONFIG::insn_cache_l1_t>("cache_il1", cache_l2, this);
				// cache_il1->memory_import >> cache_l2->memory_export;
				if(CONFIG::HAS_DATA_CACHE_L1) {
					cerr << "Configuring data cache level 1" << endl;
					cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_dl1", cache_l2, this);
					//	cache_l1->memory_import >> cache_l2->memory_export;
					//			memory_export >> cache_l1->memory_export;
				} else {
					cerr << "No data data cache level 1" << endl;
					cache_l1 = cache_l2;
					//	memory_export >> cache_l2->memory_export;
				}
			} else {
				if(CONFIG::HAS_DATA_CACHE_L1) {
					cerr << "Configuring unisified cache level 1" << endl;
					cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_l1", cache_l2, this);
					//	cache_l1->memory_import >> cache_l2->memory_export;
					//	memory_export >> cache_l1->memory_export;
				}
			}
		} else {
			if(CONFIG::HAS_INSN_CACHE_L1) {
				cache_il1 = new Cache<typename CONFIG::insn_cache_l1_t>("cache_il1", memory_interface, this);
				cerr << "Configuring instruction cache level 1" << endl;
				if(CONFIG::HAS_DATA_CACHE_L1) {
					cerr << "Configuring data cache level 1" << endl;
					cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_dl1", memory_interface, this);
					// cache_l1->memory_import >> memory_import;
					//	memory_export >> cache_l1->memory_export;
				}
			} else {
				if(CONFIG::HAS_DATA_CACHE_L1) {
					cerr << "Configuring unified cache level 1" << endl;
					cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_dl1", memory_interface, this);
					// cache_l1->memory_import >> memory_import;
					//	memory_export >> cache_l1->memory_export;
				} else {
					cerr << "No caches present in this system" << endl;
					//			cache_l1 = memory_interface;
					//			memory_export >> memory_import;
				}
			}
		}
#endif 
	}
	
	/**************************************************************/
	/* Memory system creation methods (private)               END */
	/**************************************************************/
	
	/**************************************************************/
	/* Memory system methods (private)                      START */
	/**************************************************************/
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	PerformLoadStoreAccesses()
	{
		// while the lsQueue is not empty process entries
		while(!lsQueue.empty()) {
			MemoryOp<CONFIG> *memop = lsQueue.front();
			lsQueue.pop();
			switch(memop->GetType()) {
				case MemoryOp<CONFIG>::PREFETCH:
					PerformPrefetchAccess(memop);
					break;
				case MemoryOp<CONFIG>::WRITE:
					PerformWriteAccess(memop);
					break;
				case MemoryOp<CONFIG>::READ:
					PerformReadAccess(memop);
					break;
				case MemoryOp<CONFIG>::READ_TO_PC_UPDATE_T:
					PerformReadToPCUpdateTAccess(memop);
					break;
				case MemoryOp<CONFIG>::READ_TO_PC:
					PerformReadToPCAccess(memop);
					break;
			}
			freeLSQueue.push(memop);
		}
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	PerformPrefetchAccess(MemoryOp<CONFIG> *memop)
	{
		uint32_t addr = memop->GetAddress();
		uint32_t value = 0;
		
		if ( CONFIG::MODEL == ARM926EJS && linux_os_import )
		{
			// running arm926ejs with linux simulation
			//   tcm are ignored
			uint32_t cache_tag = arm926ejs_dcache.GetTag(addr);
			uint32_t cache_set = arm926ejs_dcache.GetSet(addr);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
				{
					// the access is a hit, nothing needs to be done
					cache_hit = true;
				}
			}
			// if the access was a miss, data needs to be fetched from main
			//   memory and placed into the cache
			if ( !cache_hit )
			{
				// get a way to replace
				cache_way = arm926ejs_dcache.GetNewWay(cache_set);
				// get the valid and dirty bits from the way to replace
				bool cache_valid = arm926ejs_dcache.GetValid(cache_set,
						cache_way);
				bool cache_dirty = arm926ejs_dcache.GetDirty(cache_set,
						cache_way);
				if ( cache_valid & cache_dirty )
				{
					// the cache line to replace is valid and dirty so it needs
					//   to be sent to the main memory
					uint8_t *rep_cache_data = 0;
					uint32_t rep_cache_address =
							arm926ejs_dcache.GetBaseAddress(cache_set,
									cache_way);
					arm926ejs_dcache.GetData(cache_set, cache_way,
							&rep_cache_data);
					memory_interface->PrWrite(rep_cache_address, rep_cache_data,
							arm926ejs_dcache.LINE_SIZE);
				}
				// the new data can be requested
				uint8_t *cache_data = 0;
				uint32_t cache_address =
						arm926ejs_dcache.GetBaseAddressFromAddress(addr);
				// when getting the data we get the pointer to the cache line
				//   containing the data, so no need to write the cache
				//   afterwards
				uint32_t cache_line_size = arm926ejs_dcache.GetData(cache_set,
						cache_way, &cache_data);
				memory_interface->PrRead(cache_address, cache_data,
						cache_line_size);
				arm926ejs_dcache.SetTag(cache_set, cache_way, cache_tag);
				arm926ejs_dcache.SetValid(cache_set, cache_way, 1);
				arm926ejs_dcache.SetDirty(cache_set, cache_way, 0);
			}
			if ( unlikely(dl1_power_estimator_import != 0) )
				dl1_power_estimator_import->ReportReadAccess();
		}
		if ( CONFIG::MODEL == ARM926EJS && !linux_os_import )
		{
			// TODO
		}

		/* should we report a memory access for a prefetch???? */
		
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(
						MemoryAccessReporting<uint64_t>::MAT_READ,
						MemoryAccessReporting<uint64_t>::MT_DATA,
						addr, 4);
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	PerformWriteAccess(MemoryOp<CONFIG> *memop)
	{
		uint32_t addr = memop->GetAddress();
		uint32_t size = memop->GetSize();
		uint32_t write_addr = addr;
		uint8_t val8 = 0;
		uint16_t val16 = 0;
		uint32_t val32 = 0;
		uint8_t *data;

		if ( unlikely(verbose_memory) )
			logger << DebugInfo << "Perform write (addr = 0x" << hex
				<< addr << dec << ", size = " << size << ")" << EndDebugInfo;

		// fix the write address depending on the request size and endianess
		//   and prepare the data to write
		switch(size)
		{
		case 1:
			write_addr = write_addr ^ munged_address_mask8;
			val8 = (uint8_t)memop->GetWriteValue();
			data = &val8;
			break;
		case 2:
			write_addr = write_addr ^ munged_address_mask16;
			val16 = (uint16_t)memop->GetWriteValue();
			val16 = Host2Target(GetEndianness(), val16);
			data = (uint8_t *)&val16;
			break;
		case 4:
			val32 = memop->GetWriteValue();
			val32 = Host2Target(GetEndianness(), val32);
			data = (uint8_t *)&val32;
			break;
		default: // should never happen
			break;
		}

		if ( (CONFIG::MODEL == ARM926EJS) && linux_os_import )
		{
			// running arm926ejs with linux simulation
			//   tcm are ignored
			uint32_t cache_tag = arm926ejs_dcache.GetTag(write_addr);
			uint32_t cache_set = arm926ejs_dcache.GetSet(write_addr);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_dcache.GetValid(cache_set, cache_way) != 0 )
				{
					// the access is a hit
					cache_hit = true;
				}
			}
			// if the access was a hit the data needs to be written into
			//   the cache, if the access was a miss the data needs to be
			//   written into memory, but the cache doesn't need to be updated
			if ( cache_hit )
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache hit, updating cache."
						<< EndDebugInfo;
				uint32_t cache_index = arm926ejs_dcache.GetIndex(write_addr);
				arm926ejs_dcache.SetData(cache_set, cache_way, cache_index,
						size, data);
				arm926ejs_dcache.SetDirty(cache_set, cache_way, 1);
			}
			else
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache miss" << EndDebugInfo;
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Performing write-through ..."
						<< EndDebugInfo;
				memory_interface->PrWrite(write_addr, data, size);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "... write-through finished."
						<< EndDebugInfo;
			}

			if ( unlikely(dl1_power_estimator_import != 0) )
				dl1_power_estimator_import->ReportWriteAccess();
		}
		if ( (CONFIG::MODEL == ARM926EJS) && !linux_os_import )
		{
			// TODO
		}

		/* report read memory access if necessary */
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(
						MemoryAccessReporting<uint64_t>::MAT_WRITE,
						MemoryAccessReporting<uint64_t>::MT_DATA,
						addr, size);
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	PerformReadAccess(MemoryOp<CONFIG> *memop)
	{
		uint32_t addr = memop->GetAddress();
		uint32_t size = memop->GetSize();
		uint32_t read_addr = addr & ~(uint32_t)(size - 1);
		uint8_t *data;

		if ( unlikely(verbose_memory) )
			logger << DebugInfo << "Perform read (addr = 0x" << hex
				<< addr << dec << ", size = " << size << ")" << EndDebugInfo;

		// fix the read address depending on the request size and endianess
		switch(size)
		{
		case 1: read_addr = read_addr ^ munged_address_mask8; break;
		case 2: read_addr = read_addr ^ munged_address_mask16; break;
		case 4: // nothing to do
		default: // nothing to do
			break;
		}

		if ( (CONFIG::MODEL == ARM926EJS) && linux_os_import )
		{
			// running arm926ejs with linux simulation
			//   tcm are ignored
			uint32_t cache_tag = arm926ejs_dcache.GetTag(read_addr);
			uint32_t cache_set = arm926ejs_dcache.GetSet(read_addr);
			uint32_t cache_way;
			bool cache_hit = false;
			if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
				{
					// the access is a hit, nothing needs to be done
					cache_hit = true;
				}
			}
			// if the access was a miss, data needs to be fetched from main
			//   memory and placed into the cache
			if ( !cache_hit )
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache miss" << EndDebugInfo;
				// get a way to replace
				cache_way = arm926ejs_dcache.GetNewWay(cache_set);
				// get the valid and dirty bits from the way to replace
				uint8_t cache_valid = arm926ejs_dcache.GetValid(cache_set,
						cache_way);
				uint8_t cache_dirty = arm926ejs_dcache.GetDirty(cache_set,
						cache_way);

				if ( (cache_valid != 0) & (cache_dirty != 0) )
				{
					// the cache line to replace is valid and dirty so it needs
					//   to be sent to the main memory
					uint8_t *rep_cache_data = 0;
					uint32_t rep_cache_address =
							arm926ejs_dcache.GetBaseAddress(cache_set,
									cache_way);
					arm926ejs_dcache.GetData(cache_set, cache_way,
							&rep_cache_data);
					if ( unlikely(verbose_memory) )
						logger << DebugInfo << "Performing writeback ..."
							<< EndDebugInfo;
					memory_interface->PrWrite(rep_cache_address, rep_cache_data,
							arm926ejs_dcache.LINE_SIZE);
					if ( unlikely(verbose_memory) )
						logger << DebugInfo << "... writeback performed."
							<< EndDebugInfo;
				}
				// the new data can be requested
				uint8_t *cache_data = 0;
				uint32_t cache_address =
						arm926ejs_dcache.GetBaseAddressFromAddress(read_addr);
				// when getting the data we get the pointer to the cache line
				//   containing the data, so no need to write the cache
				//   afterwards
				uint32_t cache_line_size = arm926ejs_dcache.GetData(cache_set,
						cache_way, &cache_data);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Requesting data ..."
						<< EndDebugInfo;
				memory_interface->PrRead(cache_address, cache_data,
						cache_line_size);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "... data received" << EndDebugInfo;
				arm926ejs_dcache.SetTag(cache_set, cache_way, cache_tag);
				arm926ejs_dcache.SetValid(cache_set, cache_way, 1);
				arm926ejs_dcache.SetDirty(cache_set, cache_way, 0);
			}
			else
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache hit" << EndDebugInfo;
			}

			// at this point the data is in the cache, we can read it from the
			//   cache
			uint32_t cache_index = arm926ejs_dcache.GetIndex(read_addr);
			(void)arm926ejs_dcache.GetData(cache_set, cache_way, cache_index,
					size, &data);
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
				uint16_t val16 =
						Target2Host(GetEndianness(), *((uint16_t *)data));
				if (memop->IsSigned())
					value = (int32_t)(int16_t)val16;
				else
					value = val16;
			}
			else if (size == 4)
			{
				uint32_t val32;
				uint32_t val32_l, val32_r;
				uint32_t mask32_l, mask32_r;
				uint32_t align;

				val32 = Target2Host(GetEndianness(), *((uint32_t *)data));
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
			}

			if ( unlikely(verbose_memory) )
				logger << DebugInfo << "Setting r"
					<< (unsigned int)memop->GetTargetReg()
					<< " to 0x" << hex << value << dec
					<< EndDebugInfo;
			SetGPR(memop->GetTargetReg(), value);

			if ( unlikely(dl1_power_estimator_import != 0) )
				dl1_power_estimator_import->ReportReadAccess();
		}
		if ( CONFIG::MODEL == ARM926EJS && !linux_os_import )
		{
			// TODO
		}

		/* report read memory access if necessary */
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(
						MemoryAccessReporting<uint64_t>::MAT_READ,
						MemoryAccessReporting<uint64_t>::MT_DATA,
						addr, size);
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	PerformReadToPCAccess(MemoryOp<CONFIG> *memop) {
		uint32_t addr = memop->GetAddress();
		const uint32_t size = 4;
		uint32_t read_addr = addr & ~(uint32_t)0x03;
		uint8_t *data;

		if ( unlikely(verbose_memory) )
			logger << DebugInfo << "Perform read to PC (addr = 0x" << hex
				<< addr << dec << ", size = " << size << ")" << EndDebugInfo;

		if ( (CONFIG::MODEL == ARM926EJS) && linux_os_import )
		{
			// running arm926ejs with linux simulation
			//   tcm are ignored
			uint32_t cache_tag = arm926ejs_dcache.GetTag(read_addr);
			uint32_t cache_set = arm926ejs_dcache.GetSet(read_addr);
			uint32_t cache_way;
			bool cache_hit = false;

			if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
				{
					// the access is a hit, nothing needs to be done
					cache_hit = true;
				}
			}
			// if the access was a miss, data needs to be fetched from main
			//   memory and placed into the cache
			if ( !cache_hit )
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache miss" << EndDebugInfo;
				// get a way to replace
				cache_way = arm926ejs_dcache.GetNewWay(cache_set);
				// get the valid and dirty bits from the way to replace
				uint8_t cache_valid = arm926ejs_dcache.GetValid(cache_set,
						cache_way);
				uint8_t cache_dirty = arm926ejs_dcache.GetDirty(cache_set,
						cache_way);

				if ( (cache_valid != 0) && (cache_dirty != 0) )
				{
					// the cache line to replace is valid and dirty so it needs
					//   to be sent to the main memory
					uint8_t *rep_cache_data = 0;
					uint32_t rep_cache_address =
							arm926ejs_dcache.GetBaseAddress(cache_set,
									cache_way);
					arm926ejs_dcache.GetData(cache_set, cache_way,
							&rep_cache_data);
					if ( unlikely(verbose_memory) )
						logger << DebugInfo << "Performing writeback ..."
							<< EndDebugInfo;
					memory_interface->PrWrite(rep_cache_address, rep_cache_data,
							arm926ejs_dcache.LINE_SIZE);
					if ( unlikely(verbose_memory) )
						logger << DebugInfo << "... writeback performed."
							<< EndDebugInfo;
				}
				// the new data can be requested
				uint8_t *cache_data = 0;
				uint32_t cache_address =
						arm926ejs_dcache.GetBaseAddressFromAddress(read_addr);
				// when getting the data we get the pointer to the cache line
				//   containing the data, so no need to write the cache
				//   afterwards
				uint32_t cache_line_size = arm926ejs_dcache.GetData(cache_set,
						cache_way, &cache_data);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Requesting data ..."
						<< EndDebugInfo;
				memory_interface->PrRead(cache_address, cache_data,
						cache_line_size);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "... data received" << EndDebugInfo;
				arm926ejs_dcache.SetTag(cache_set, cache_way, cache_tag);
				arm926ejs_dcache.SetValid(cache_set, cache_way, 1);
				arm926ejs_dcache.SetDirty(cache_set, cache_way, 0);
			}
			else
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache hit" << EndDebugInfo;
			}

			// at this point the data is in the cache, we can read it from the
			//   cache
			uint32_t cache_index = arm926ejs_dcache.GetIndex(read_addr);
			(void)arm926ejs_dcache.GetData(cache_set, cache_way, cache_index,
					size, &data);
			// fix the data depending on its size
			uint32_t value;
			uint32_t val32;
			uint32_t val32_l, val32_r;
			uint32_t mask32_l, mask32_r;
			uint32_t align;

			val32 = Target2Host(GetEndianness(), *((uint32_t *)data));
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

			if ( unlikely(verbose_memory) )
				logger << DebugInfo << "Setting r"
					<< (unsigned int)PC_reg
					<< " to 0x" << hex << value << dec
					<< EndDebugInfo;
			SetGPR(PC_reg, value & ~(uint32_t)0x01);

			if ( unlikely(dl1_power_estimator_import != 0) )
				dl1_power_estimator_import->ReportReadAccess();
		}
		if ( (CONFIG::MODEL == ARM926EJS) && !linux_os_import )
		{
			// TODO
		}

		/* report read memory access if necessary */
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(
						MemoryAccessReporting<uint64_t>::MAT_READ,
						MemoryAccessReporting<uint64_t>::MT_DATA,
						addr, size);
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	PerformReadToPCUpdateTAccess(MemoryOp<CONFIG> *memop) {
		uint32_t addr = memop->GetAddress();
		const uint32_t size = 4;
		uint32_t read_addr = addr & ~(uint32_t)0x03;
		uint8_t *data;

		if ( unlikely(verbose_memory) )
			logger << DebugInfo << "Perform read to PC update T (addr = 0x"
				<< hex << addr << dec << ", size = " << size << ")"
				<< EndDebugInfo;

		if ( (CONFIG::MODEL == ARM926EJS) && linux_os_import )
		{
			// running arm926ejs with linux simulation
			//   tcm are ignored
			uint32_t cache_tag = arm926ejs_dcache.GetTag(read_addr);
			uint32_t cache_set = arm926ejs_dcache.GetSet(read_addr);
			uint32_t cache_way;
			bool cache_hit = false;

			if ( arm926ejs_dcache.GetWay(cache_tag, cache_set, &cache_way) )
			{
				if ( arm926ejs_dcache.GetValid(cache_set, cache_way) )
				{
					// the access is a hit, nothing needs to be done
					cache_hit = true;
				}
			}
			// if the access was a miss, data needs to be fetched from main
			//   memory and placed into the cache
			if ( !cache_hit )
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache miss" << EndDebugInfo;
				// get a way to replace
				cache_way = arm926ejs_dcache.GetNewWay(cache_set);
				// get the valid and dirty bits from the way to replace
				bool cache_valid = arm926ejs_dcache.GetValid(cache_set,
						cache_way);
				bool cache_dirty = arm926ejs_dcache.GetDirty(cache_set,
						cache_way);
				if ( cache_valid & cache_dirty )
				{
					// the cache line to replace is valid and dirty so it needs
					//   to be sent to the main memory
					uint8_t *rep_cache_data = 0;
					uint32_t rep_cache_address =
							arm926ejs_dcache.GetBaseAddress(cache_set,
									cache_way);
					arm926ejs_dcache.GetData(cache_set, cache_way,
							&rep_cache_data);
					if ( unlikely(verbose_memory) )
						logger << DebugInfo << "Performing writeback ..."
							<< EndDebugInfo;
					memory_interface->PrWrite(rep_cache_address, rep_cache_data,
							arm926ejs_dcache.LINE_SIZE);
					if ( unlikely(verbose_memory) )
						logger << DebugInfo << "... writeback performed."
							<< EndDebugInfo;
				}
				// the new data can be requested
				uint8_t *cache_data = 0;
				uint32_t cache_address =
						arm926ejs_dcache.GetBaseAddressFromAddress(read_addr);
				// when getting the data we get the pointer to the cache line
				//   containing the data, so no need to write the cache
				//   afterwards
				uint32_t cache_line_size = arm926ejs_dcache.GetData(cache_set,
						cache_way, &cache_data);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Requesting data ..."
						<< EndDebugInfo;
				memory_interface->PrRead(cache_address, cache_data,
						cache_line_size);
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "... data received" << EndDebugInfo;
				arm926ejs_dcache.SetTag(cache_set, cache_way, cache_tag);
				arm926ejs_dcache.SetValid(cache_set, cache_way, 1);
				arm926ejs_dcache.SetDirty(cache_set, cache_way, 0);
			}
			else
			{
				if ( unlikely(verbose_memory) )
					logger << DebugInfo << "Cache hit" << EndDebugInfo;
			}

			// at this point the data is in the cache, we can read it from the
			//   cache
			uint32_t cache_index = arm926ejs_dcache.GetIndex(read_addr);
			(void)arm926ejs_dcache.GetData(cache_set, cache_way, cache_index,
					size, &data);
			// fix the data depending on its size
			uint32_t value;
			uint32_t val32;
			uint32_t val32_l, val32_r;
			uint32_t mask32_l, mask32_r;
			uint32_t align;

			val32 = Target2Host(GetEndianness(), *((uint32_t *)data));
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

			if ( unlikely(verbose_memory) )
				logger << DebugInfo << "Setting r"
					<< (unsigned int)memop->GetTargetReg()
					<< " to 0x" << hex << value << dec
					<< ", and setting CPSR[T] to "
					<< (unsigned int)(value & (uint32_t)0x01)
					<< EndDebugInfo;
			SetGPR(PC_reg, value & ~(uint32_t)0x01);
			SetCPSR_T((value & (uint32_t)0x01) == 0x01);

			if ( unlikely(dl1_power_estimator_import != 0) )
				dl1_power_estimator_import->ReportReadAccess();
		}
		if ( (CONFIG::MODEL == ARM926EJS) && !linux_os_import )
		{
			// TODO
		}

		/* report read memory access if necessary */
		if ( requires_memory_access_reporting )
			if ( memory_access_reporting_import )
				memory_access_reporting_import->ReportMemoryAccess(
						MemoryAccessReporting<uint64_t>::MAT_READ,
						MemoryAccessReporting<uint64_t>::MT_DATA,
						addr, size);
	}
	
	/**************************************************************/
	/* Memory system methods (private)                        END */
	/**************************************************************/
	
	/**************************************************************/
	/* Unpredictable instruction behavior (private)         START */
	/**************************************************************/
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	Unpredictable() {
		logger << DebugError
		<< "Trying to execute unpredictable behavior instruction"
		<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": "
		<< EndDebugError;
	}
	/**************************************************************/
	/* Unpredictable instruction behavior (private)           END */
	/**************************************************************/
	
	/**************************************************************/
	/* Verbose methods (protected)                          START */
	/**************************************************************/
	
	template<class CONFIG>
	inline INLINE
	bool
	CPU<CONFIG> ::
	VerboseSetup() 
	{
		
		return CONFIG::DEBUG_ENABLE && verbose_setup;
		
	}
	
	template<class CONFIG>
	inline INLINE
	bool
	CPU<CONFIG> ::
	VerboseStep() 
	{
		
		return CONFIG::DEBUG_ENABLE && verbose_step;
		
	}
	
	template<class CONFIG>
	inline INLINE
	bool
	CPU<CONFIG> ::
	VerboseException()
	{
		return verbose_exception;
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	VerboseDumpRegs() {
		
		for(unsigned int i = 0; i < 4; i++) {
			for(unsigned int j = 0; j < 4; j++)
				logger
				<< "\t- r" << ((i*4) + j) << " = 0x" << hex << GetGPR((i*4) + j) << dec;
			logger << endl;
		}
		logger << "\t- cpsr = (" << hex << GetCPSR() << dec << ") ";
		typename CONFIG::reg_t mask;
		for(mask = 0x80000000; mask != 0; mask = mask >> 1) {
			if((mask & GetCPSR()) != 0) logger << "1";
			else logger << "0";
		}
		logger << endl;
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	VerboseDumpRegsStart() {
		if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_start) {
			logger << DebugInfo
			<< "Register dump before starting instruction execution: " << endl;
			VerboseDumpRegs();
			logger << EndDebugInfo;
		}
	}
	
	template<class CONFIG>
	inline INLINE
	void
	CPU<CONFIG> ::
	VerboseDumpRegsEnd() {
		if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_end) {
			logger << DebugInfo
			<< "Register dump at the end of instruction execution: " << endl;
			VerboseDumpRegs();
			logger << EndDebugInfo;
		}
	}
	
	template<class CONFIG>
	inline INLINE
	bool
	CPU<CONFIG>::
	TrapOnException()
	{
		return trap_on_exception && trap_reporting_import;
	}
	
	/**************************************************************/
	/* Verbose methods (protected)                            END */
	/**************************************************************/
	
	/**************************************************************/
	/* Verbose methods (protected)                          START */
	/**************************************************************/
	
	template<class CONFIG>
	void
	CPU<CONFIG> ::
	DumpInstructionProfile(ostream *output) {
		(*output) << "Profile" << endl;
		class item {
			uint64_t opcode;
			uint32_t profile;
		};
		
		vector<uint64_t> opcode;
		vector<uint32_t> counter;
		for(map<uint64_t, uint32_t>::iterator it = profile.begin();
			it != profile.end();
			it++) {
			bool done = false;
			vector<uint64_t>::iterator oit = opcode.begin();
			for(vector<uint32_t>::iterator cit = counter.begin();
				!done && cit != counter.end();
				cit++, oit++) {
				if(it->second > *cit) {
					counter.insert(cit, it->second);
					opcode.insert(oit, it->first);
					done = true;
				}
			}
			if(!done) {
				counter.push_back(it->second);
				opcode.push_back(it->first);
			}
		}
		
		
		uint32_t index = 0;
		vector<uint32_t>::iterator cit = counter.begin();
		for(vector<uint64_t>::iterator oit = opcode.begin();
			oit != opcode.end();
			oit++, cit++) {
			typename isa::arm32::Operation<CONFIG> *op = NULL;
			typename isa::thumb::Operation<CONFIG> *top = NULL;
			uint32_t opcode = (uint32_t)(*oit & (uint64_t)0x0ffffffffULL);
			bool thumb = *oit != opcode;
			stringstream s;
			if(thumb) {
				top = thumb_decoder.Decode(0, opcode);
				top->disasm(*this, s);
			} else {
				op = arm32_decoder.Decode(0, opcode);
				op->disasm(*this, s);
			}
			
			
			(*output) << "0x" << hex << (*oit) << dec << "\t " << (*cit) << "\t " << s.str() << endl;
			index++;
		}
	}
	
	/**************************************************************/
	/* Verbose methods (protected)                            END */
	/**************************************************************/
	
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#undef INLINE

#include "unisim/component/cxx/processor/arm/cpu.step_instruction.tcc"
#include "unisim/component/cxx/processor/arm/cpu.step_cycle.tcc"
#include "unisim/component/cxx/processor/arm/cpu.soclib_step_cycle.tcc"

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__

