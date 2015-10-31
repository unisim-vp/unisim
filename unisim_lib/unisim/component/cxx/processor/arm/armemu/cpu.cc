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
#include <unisim/component/cxx/processor/arm/armemu/cpu.hh>
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/cache.hh>
#include <unisim/component/cxx/processor/arm/memory_op.hh>
#include <unisim/kernel/debug/debug.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/likely/likely.hh>

#include <sstream>
#include <string>
#include <cstring>
#include <cassert>
#include <inttypes.h>

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

// class ProgramCounterRegister (unisim::util::debug::Register) used for PC/R15 view
class ProgramCounterRegister : public Register
{
public:
  ProgramCounterRegister(const char* _name, CPU& _cpu) : name(_name), cpu(_cpu) {}
  virtual ~ProgramCounterRegister() {}
  virtual const char *GetName() const { return name.c_str(); }
  virtual void GetValue(void *buffer) const { *((uint32_t*)buffer) = cpu.GetNPC(); }
  virtual void SetValue(const void *buffer) { uint32_t address = *((uint32_t*)buffer); cpu.BranchExchange( address ); }
  virtual int GetSize() const { return 4; }
private:
  std::string name;
  CPU&        cpu;
};

/** Constructor.
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */

CPU::CPU(const char *name, Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , unisim::component::cxx::processor::arm::CPU<ARMv7emu>(name, parent)
  , unisim::kernel::service::Service<unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
  , unisim::kernel::service::Client<unisim::service::interfaces::MemoryAccessReporting<uint32_t> >(name, parent)
  , Client<LinuxOS>(name, parent)
  , Service<MemoryInjection<uint32_t> >(name, parent)
  , Client<DebugControl<uint32_t> >(name, parent)
  , Client<TrapReporting>(name, parent)
  , Service<Disassembly<uint32_t> >(name, parent)
  , Service<Registers>(name, parent)
  , Service< Memory<uint32_t> >(name, parent)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , disasm_export("disasm-export", this)
  , registers_export("registers-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , debug_control_import("debug-control-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , linux_os_import("linux-os-import", this)
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  , requires_finished_instruction_reporting(true)
  , requires_memory_access_reporting(true)
  , logger(*this)
  , icache("icache", this)
  , arm32_decoder()
  , instruction_counter(0)
  , voltage(0)
  , verbose(0)
  , trap_on_instruction_counter(0)
  , default_endianness_string(GetEndianness() == unisim::util::endian::E_BIG_ENDIAN ? "big-endian" : "little-endian")
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
  , reg_cpsr("CPSR", this, cpsr.m_value,
           "The CPSR register.")
  , ipb_base_address( -1 )
{
  // initialize the registers debugging interface
  for(int i = 0; i < 15; i++) {
    std::stringstream str;
    str << "r" << i;
    registers_registry[str.str()] =
      new SimpleRegister<uint32_t>(str.str(), &gpr[i]);
  }
  registers_registry["r15"] =  new ProgramCounterRegister("r15", *this);
  registers_registry["pc"] =   new ProgramCounterRegister("pc", *this);
  registers_registry["sp"] =   new SimpleRegister<uint32_t>("sp", &gpr[13]);
  registers_registry["lr"] =   new SimpleRegister<uint32_t>("lr", &gpr[14]);
  registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &(cpsr.m_value));
  
  registers_registry["sl"] = new SimpleRegister<uint32_t>("sl", &gpr[10]);
  registers_registry["fp"] = new SimpleRegister<uint32_t>("fp", &gpr[11]);
  registers_registry["ip"] = new SimpleRegister<uint32_t>("ip", &gpr[12]);
  
  // TODO: Provide access to Banked Registers
  // for (unsigned int i = 0; i < num_phys_gprs; i++)
  //   {
  //     std::stringstream ss, ss_desc;
  //     ss << "PHYS_GPR[" << i << "]";
  //     ss_desc << "Physical register " << i;
  //     reg_phys_gpr[i] =  
  //       new unisim::kernel::service::Register<uint32_t>( ss.str().c_str(), this, phys_gpr[i], ss_desc.str().c_str() );
  //   }
  for (unsigned int i = 0; i < (num_log_gprs - 1); i++)
    {
      std::stringstream ss, ss_desc;
      ss << "GPR[" << i << "]";
      ss_desc << "Logical register " << i;
      reg_gpr[i] = 
        new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), 
                                                        this, gpr[i], ss_desc.str().c_str());
    }
  reg_gpr[15] = new unisim::kernel::service::Register<uint32_t>("GPR[15]", this, this->next_pc, "Logical register 15");
  // TODO: provide unisim registers for SPSRs
  // for (unsigned int i = 0; i < num_phys_spsrs; i++)
  //   {
  //     std::stringstream ss, ss_desc;
  //     ss << "SPSR[" << i << "]";
  //     ss_desc << "SPSR[" << i << "] register";
  //     reg_spsr[i] =
  //       new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), 
  //                                                       this, spsr[i].m_value, ss_desc.str().c_str());
  //   }

  // This implementation of the arm architecture can only run in user mode,
  //   so we can already set CPSR to that mode.
  cpsr.Set( M, USER_MODE );

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
CPU::~CPU()
{
  for (unsigned int i = 0; i < num_log_gprs; i++)
    if (reg_gpr[i]) delete reg_gpr[i];

  for (RegistersRegistry::iterator itr = registers_registry.begin(), end = registers_registry.end(); itr != end; itr++)
    delete itr->second;
  registers_registry.clear();
}

/** Get SPSR index from current running mode
 *
 * @return the SPSR index from current running mode
 */
uint32_t 
CPU::
GetSPSRIndex()
{
  uint32_t rm = 0;
  uint32_t run_mode = cpsr.Get( M );
  switch (run_mode)
    {
    case USER_MODE: case SYSTEM_MODE: {
      /* In user or system mode, access to SPSR are unpredictable,
       * thus the code whould never try to access SPSR in such
       * modes. */
      
      logger << DebugWarning
             << "trying to access SPSR while running in "
             << ((run_mode == USER_MODE) ? "user" : "system")
             << " mode with the following instruction: "
             << std::endl
             << "Location: " << __FUNCTION__
             << ":" << __FILE__
             << ":" << __LINE__
             << EndDebugWarning;
      Stop(-1);
    } break;
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
      assert(0);
      break;
    }
	
  return rm;
}

/** Copy the value of current SPSR register into CPSR.
 */
void 
CPU::MoveSPSRtoCPSR()
{
  /* SPSR needs to be moved to CPSR
   * This means that we need to change the register mapping if the running mode has changed
   */
  uint32_t src_mode = CPSR().Get( M );
  uint32_t dst_mode = SPSR().Get( M );
  CPSR().bits() = SPSR().bits();
  if (src_mode != dst_mode)
    {
      GetMode(src_mode).Swap(*this); // OUT
      GetMode(dst_mode).Swap(*this); // IN
    }
}

/** Object setup method.
 * This method is required for all UNISIM objects and will be called during
 *   the setup phase.
 * 
 * @return true on success, false otherwise
 */
bool 
CPU::EndSetup()
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
                cpsr.Set( E, default_endianness_string.compare("little-endian") == 0 ? 0 : 1 );
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
				<< " hardware !" << std::endl;
			logger << "cpu cycle time should be >= "
				<< min_cycle_time << " ps." << std::endl;
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
			<< " mV." << std::endl;
		if ( icache.power_mode_import )
			logger << "  - instruction cache voltage = "
				<< il1_def_voltage
				<< " mV";
		if ( dcache.power_mode_import )
		{
			if ( icache.power_mode_import )
				logger << std::endl;
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
          std::stringstream str;
		str << "r" << i;
		registers_registry[str.str().c_str()] =
			new SimpleRegister<uint32_t>(str.str().c_str(), &gpr[i]);
	}
	registers_registry["sp"] = new SimpleRegister<uint32_t>("sp", &gpr[13]);
	registers_registry["lr"] = new SimpleRegister<uint32_t>("lr", &gpr[14]);
	registers_registry["pc"] = new SimpleRegister<uint32_t>("pc", &next_pc);
	registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &(cpsr.m_value));
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
CPU::OnDisconnect()
{
	/* TODO */
}

/** Execute one complete instruction.
 */
void 
CPU::StepInstruction()
{
  uint32_t current_pc = this->current_pc = GetNPC();

  if (debug_control_import) 
    {
      DebugControl<uint32_t>::DebugCommand dbg_cmd;

      do 
        {
          dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
	
          if (likely(dbg_cmd == DebugControl<uint32_t>::DBG_STEP)) 
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
	
  if (cpsr.Get( T )) {
    /* Thumb state */
    isa::thumb2::CodeType insn;
    ReadInsn(current_pc, insn);
		
    /* Decode current PC */
    isa::thumb2::Operation<unisim::component::cxx::processor::arm::armemu::ARMv7emu>* op;
    op = thumb_decoder.Decode(current_pc, insn);
    unsigned insn_length = op->GetLength();
    if (insn_length % 16) throw 0;
		
    /* update PC registers value before execution */
    this->gpr[15] = this->next_pc + 4;
    this->next_pc += insn_length / 8;
		
    /* Execute instruction */
    asm volatile( "thumb2_operation_execute:" );
    op->execute( *this );
    
    this->ITAdvance();
    //op->profile(profile);
  }
	
  else {
    /* Arm32 state */
		
    /* fetch instruction word from memory */
    isa::arm32::CodeType insn;
    ReadInsn(current_pc, insn);
			
    /* Decode current PC */
    isa::arm32::Operation<unisim::component::cxx::processor::arm::armemu::ARMv7emu>* op;
    op = arm32_decoder.Decode(current_pc, insn);
		
    /* update PC registers value before execution */
    this->gpr[15] = this->next_pc + 8;
    this->next_pc += 4;
		
    /* Execute instruction */
    asm volatile( "arm32_operation_execute:" );
    op->execute( *this );
    //op->profile(profile);
  }
	
  /* check that an exception has not occurred, if so 
   * stop the simulation */
  if ( unlikely(GetVirtualExceptionVector()) )
    {
      logger << DebugError
             << "An exception has been found, this should never happen "
             << "when simulating at user level."
             << EndDebugError;
      Stop(-1);
    }
  instruction_counter++;
  if ( unlikely(instruction_counter_trap_reporting_import &&
		trap_on_instruction_counter == instruction_counter) )
    instruction_counter_trap_reporting_import->ReportTrap(*this);
	
  if(unlikely(requires_finished_instruction_reporting && memory_access_reporting_import))
    memory_access_reporting_import->ReportFinishedInstruction(this->current_pc, this->next_pc);
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
CPU::InjectReadMemory( uint32_t addr, void *buffer, uint32_t size )
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
CPU::InjectWriteMemory( uint32_t addr, const void* buffer, uint32_t size )
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

/** Set/unset the reporting of finishing instructions.
 * 
 * @param report if true set the reporting of finishing instructions, 
 *   unset otherwise
 */
void 
CPU::RequiresFinishedInstructionReporting( bool report )
{
	requires_finished_instruction_reporting = report;
}

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true/false sets/unsets the reporting of memory
 *        acesseses
 */
void
CPU::RequiresMemoryAccessReporting( bool report )
{
	requires_memory_access_reporting = report;
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
CPU::ReadMemory( uint32_t addr, void* buffer, uint32_t size )
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
CPU::WriteMemory(uint32_t addr, 
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
CPU::GetRegister( const char *name )
{
  RegistersRegistry::iterator itr = registers_registry.find( name );
  if (itr != registers_registry.end())
    return itr->second;
  else
    return 0;
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
std::string 
CPU::Disasm(uint32_t addr, uint32_t &next_addr)
{
	std::stringstream buffer;
	if (cpsr.Get( T ))
	{
		buffer << "[THUMB2]";
		
		uint8_t insn_bytes[4];
		isa::thumb2::CodeType insn;
		isa::thumb2::Operation<unisim::component::cxx::processor::arm::armemu::ARMv7emu> *op = 0;
		if (!ReadMemory(addr, insn_bytes, 4)) 
		{
			buffer << "Could not read from memory";
			return buffer.str();
		}
  
		// Instruction fetch ignores "Endianness execution state bit"
		insn.str[0] = insn_bytes[0];
		insn.str[1] = insn_bytes[1];
		insn.str[2] = insn_bytes[2];
		insn.str[3] = insn_bytes[3];
		insn.size = 32;
		
		op = thumb_decoder.Decode(addr, insn);
		unsigned insn_length = op->GetLength();
		if (insn_length % 16) throw 0;
		
		buffer << "0x";
		buffer << op->GetEncoding() << " ";
		op->disasm(*this, buffer);

		next_addr = addr + (insn_length / 8);
	} 
	else 
	{
		buffer << "[ARM32]";
		
		isa::arm32::Operation<unisim::component::cxx::processor::arm::armemu::ARMv7emu> * op = NULL;
		uint32_t insn;
		if (!ReadMemory(addr, &insn, 4)) 
		{
			buffer << "Could not read from memory";
			return buffer.str();
		}
		if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN)
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
CPU::PerformExit(int ret)
{
	// running = false;
	Stop(ret);
}

/** Refill the Instruction Prefetch Buffer from the memory system
 * (through the instruction cache if present).
 *
 * This method allows the user to prefetch instructions from the memory
 * system, that is, it tries to read from the pertinent caches and if
 * failed from the external memory system.
 * 
 * @param address the address of the requiered instruction that the
 *                prefetch instruction buffer should encompass, once
 *                the refill is complete.
 */
void 
CPU::RefillInsnPrefetchBuffer(uint32_t base_address)
{
  this->ipb_base_address = base_address;
  
  if (likely(icache.GetSize()))
    {
      icache.read_accesses++;
      // check the instruction cache
      uint32_t cache_tag = icache.GetTag(base_address);
      uint32_t cache_set = icache.GetSet(base_address);
      uint32_t cache_way;
      // Check for a cache hit
      bool cache_hit =
        icache.GetWay(cache_tag, cache_set, &cache_way) and
        icache.GetValid(cache_set, cache_way);
      
      if (likely(cache_hit))
        {
          // cache hit
          icache.read_hits++;
        }
      else
        {
          // get a way to replace (no need to check valid and dirty
          // bits, the new data can be requested immediately)
          cache_way = icache.GetNewWay(cache_set);
          uint8_t *cache_data = 0;
          // when getting the physical data of the cache line
          icache.GetData(cache_set, cache_way, &cache_data);
          PrRead(base_address, cache_data, Cache::LINE_SIZE);
          icache.SetTag(cache_set, cache_way, cache_tag);
          icache.SetValid(cache_set, cache_way, 1);
        }
      
      // At this point data is in the cache, so we can read it from it
      uint32_t cache_index = icache.GetIndex(base_address);
      icache.GetDataCopy(cache_set, cache_way, cache_index, Cache::LINE_SIZE, &this->ipb_bytes[0]);

      if ( unlikely(icache.power_estimator_import != 0) )
        icache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      // No instruction cache present, just request the insn to the
      // memory system.
      PrRead(base_address, &this->ipb_bytes[0], Cache::LINE_SIZE);
    }

  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, base_address, Cache::LINE_SIZE);
}

/** Reads ARM32 instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * 
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 */
void 
CPU::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn)
{
  uint32_t base_address = address & -(Cache::LINE_SIZE);
  uint32_t buffer_index = address % (Cache::LINE_SIZE);
  
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  uint32_t word = *((uint32_t*)(&ipb_bytes[buffer_index]));
  insn = Target2Host(GetEndianness(), word);
}

/** Reads THUMB instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * 
 * @param address the address to read data from
 * @param insn the resulting instruction word (output reference)
 */
void
CPU::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb2::CodeType& insn)
{
  uint32_t base_address = address & -(Cache::LINE_SIZE);
  intptr_t buffer_index = address % (Cache::LINE_SIZE);
    
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  // Instruction fetch ignores "Endianness execution state bit"
  insn.str[0] = ipb_bytes[buffer_index+0];
  insn.str[1] = ipb_bytes[buffer_index+1];
  if (unlikely((buffer_index+2) >= Cache::LINE_SIZE)) {
    RefillInsnPrefetchBuffer( base_address + Cache::LINE_SIZE );
    buffer_index = intptr_t(-2);
  }
  insn.str[2] = ipb_bytes[buffer_index+2];
  insn.str[3] = ipb_bytes[buffer_index+3];
  insn.size = 32;
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
CPU::CoprocessorLoad(uint32_t cp_num, uint32_t address)
{
	logger << DebugError << "CoprocessorLoad not implemented in" << std::endl
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
CPU::CoprocessorLoad(uint32_t cp_num, uint32_t address, uint32_t option)
{
	logger << DebugError << "CoprocessorLoad not implemented in" << std::endl
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
CPU::CoprocessorStore(uint32_t cp_num, uint32_t address)
{
	logger << DebugError << "CoprocessorStore not implemented in" << std::endl
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
CPU::CoprocessorStore(uint32_t cp_num, uint32_t address, uint32_t option)
{
	logger << DebugError << "CoprocessorStore not implemented in" << std::endl
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
CPU::CoprocessorDataProcess(uint32_t cp_num, uint32_t op1, uint32_t op2,
		uint32_t crd, uint32_t crn, uint32_t crm)
{
	logger << DebugError << "CoprocessorDataProcess not implemented in" << std::endl
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
CPU::MoveToCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
		uint32_t rd, uint32_t crn, uint32_t crm)
{
	logger << DebugError << "MoveToCoprocessor not implemented in" << std::endl
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
CPU::MoveFromCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
		uint32_t rd, uint32_t crn, uint32_t crm)
{
	logger << DebugError << "MoveFromCoprocessor not implemented in" << std::endl
	       << unisim::kernel::debug::BackTrace() << EndDebugError;
	Stop(-1);
}

#define CP15_ENCODE( CODE, OP1, CRN, CRM, OP2 ) ((CODE<<16) | (OP1<<12) | (CRN<<8) | (CRM<<4) | (OP2<<0))

uint32_t
CPU::Coproc_GetOneWord( unsigned code, unsigned cp_num, unsigned op1, unsigned op2, unsigned crn, unsigned crm )
{
  switch (cp_num)
    {
    default: throw 0;
    case 15: {
      /* CP15, ARM System coprocessor */
      switch (CP15_ENCODE( code, op1, crn, crm, op2 ))
        {
        default: throw 0;
        case CP15_ENCODE( 1, 0, 13, 0, 3 ):
           /* TODO: check if there exist two uncoherent value (ex. memory and register). */
          return MemRead32( 0xffff0ff0 );
        }
    } break;
    }
  
  return 0;
}

/** Software Interrupt
 *  This method is called by SWI instructions to handle software interrupts.
 */
void
CPU::SWI( uint32_t imm )
{
  // we are executing on linux emulation mode, use linux_os_import
  try
    {
      this->linux_os_import->ExecuteSystemCall(imm);
    }
  catch(std::exception &e)
    {
      std::cerr << e.what() << std::endl;
    }
}

/** Breakpoint
 *  This method is called by BKPT instructions to handle breakpoints.
 */
void
CPU::BKPT( uint32_t imm )
{
  // we are executing on linux emulation mode
  // what should we do with this kind of call? ignore it
}
	
/** Unpredictable Instruction Behaviour.
 * This method is just called when an unpredictable behaviour is detected to
 *   notifiy the processor.
 */
void 
CPU::UnpredictableInsnBehaviour()
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

} // end of namespace armemu
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
