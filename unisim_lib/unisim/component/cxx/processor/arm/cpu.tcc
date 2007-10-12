/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__

#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/cache/cache_interface.hh"
#include "unisim/util/debug/symbol.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include <sstream>

#define LOCATION Function << __FUNCTION__ << File <<  __FILE__ << Line << __LINE__
// #define ARM_OPTIMIZATION

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;
using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::LittleEndian2Host;
using unisim::component::cxx::cache::CC_NONE;
using unisim::util::arithmetic::RotateRight;
using std::stringstream;
//using full_system::utils::registers::SimpleRegisterInterface;
//using full_system::utils::debug::SymbolInterface;
//using full_system::utils::arithmetic::RotateRight;
//using full_system::utils::endian::Host2Target;
//using full_system::utils::endian::Target2Host;
//using full_system::utils::endian::BigEndian2Host;
//using full_system::utils::endian::LittleEndian2Host;
//using full_system::generic::cache::CC_NONE;
//using full_system::plugins::debug::DBG_STEP;
//using full_system::plugins::debug::DBG_SYNC;
//using full_system::plugins::debug::DBG_KILL;
//using full_system::plugins::debug::DBG_RESET;
//using full_system::plugins::debug::DebugCommand;
//using std::stringstream;

// Constructor
template<class CONFIG>
CPU<CONFIG> ::
CPU(const char *name, CacheInterface<typename CONFIG::address_t> *_memory_interface, Object *parent) :
	Object(name, parent),
	Client<Loader<typename CONFIG::address_t> >(name, parent),
	Client<LinuxOS>(name, parent),
	Service<CPULinuxOS>(name, parent),
	Client<DebugControl<typename CONFIG::address_t> >(name, parent),
	Client<MemoryAccessReporting<typename CONFIG::address_t> >(name, parent),
	Service<Disassembly<typename CONFIG::address_t> >(name, parent),
	Service<Registers>(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
	Client<Logger>(name, parent),
	disasm_export("disasm_export", this),
	registers_export("registers_export", this),
	memory_export("memory_export", this),
	cpu_linux_os_export("cpu_linux_os_export", this),
	debug_control_import("debug_control_import", this),
	memory_access_reporting_import("memory_access_reporting_import", this),
	symbol_table_lookup_import("symbol_table_lookup_import", this),
	memory_import("memory_import", this),
	linux_os_import("linux_os_import", this),
	logger_import("logger_import", this),
	cache_l1_logger_import("cache_l1_logger_import", this),
	cache_il1_logger_import("cache_il1_logger_import", this),
	cache_l2_logger_import("cache_l2_logger_import", this),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup),
	verbose_step(false),
	param_verbose_step("verbose-step", this, verbose_step),
	verbose_dump_regs_start(false),
	param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start),
	verbose_dump_regs_end(false),
	param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end),
	memory_interface(_memory_interface),
	instruction_counter(0),
	insn_cache_line_address(0),
	cache_l1(0),
	cache_il1(0),
	cache_l2(0),
	running(true) {
	/* setting setup dependencies */
	SetupDependsOn(logger_import);
//	SetupDependsOn(linux_os_import);
	
	/* Reset all the registers */
	InitGPR();

	/* User running mode */
	cpsr = 0;
	for(unsigned int i = 0; i < num_phys_spsrs; i++) {
		spsr[i] = 0;
	}
	for(unsigned int i = 0; i < 8; i++) {
		fake_fpr[i] = 0;
	}
	fake_fps = 0;

	/* initialize the check condition table */
	InitializeCheckConditionTable();
	
	CreateMemorySystem();
}

// Destructor
template<class CONFIG> 
CPU<CONFIG> ::
~CPU() {
}

//=====================================================================
//=                  Client/Service setup methods               START =
//=====================================================================

template<class CONFIG>
bool 
CPU<CONFIG> ::
Setup() {
	/* check verbose settings */
	if(CONFIG::DEBUG_ENABLE && verbose_all) {
		verbose_setup = true;
		verbose_step = true;
		verbose_dump_regs_start = true;
		verbose_dump_regs_end = true;
	}
	if(CONFIG::DEBUG_ENABLE && verbose_all && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "verbose-all = true"
			<< Endl << EndDebugInfo;
	} else {
		if(CONFIG::DEBUG_ENABLE && verbose_setup && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-setup = true"
				<< Endl << EndDebugInfo;
		if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-step = true"
				<< Endl << EndDebugInfo;
		if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_start && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-dump-regs-end = true"
				<< Endl << EndDebugInfo;
		if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_end && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "verbose-dump-regs-start = true"
				<< Endl << EndDebugInfo;
	}
	
	/* check if the emulator is running in user or system mode and perform
	 *   the corresponding initialization
	 * if linux_os_import is connected then we are running in user mode,
	 *   otherwise we are running in system mode
	 */ 
	if(linux_os_import) {
		/* we are running in user mode:
		 * - set CPSR to user mode
		 * - initialize the cache system
		 */
		// set CPSR to user mode
		SetCPSR_Mode(USER_MODE);
		if(VerboseSetup()) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Running \"" << GetName() << "\" in user mode"
				<< Endl << EndDebugInfo;
		}
		// initialize the cache system
		if(cache_l1 != 0) cache_l1->Enable();
		if(cache_il1 != 0) cache_il1->Enable();
		if(cache_l2 != 0) cache_l2->Enable();
	} else {
		/* we are running in system mode */
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< "TODO: Running \"" << GetName() << "\" in system mode"
				<< Endl << EndDebugError;
		}
		return false;
	}

	/* setting debugging registers */
	if(verbose_setup && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Initializing debugging registers"
			<< Endl << EndDebugInfo;
	for(int i = 0; i < 13; i++) {
		stringstream str;
	    str << "r" << i;
	    registers_registry[str.str()] = new SimpleRegister<reg_t>(str.str().c_str(), &gpr[i]);
	}
	registers_registry["sp"] = new SimpleRegister<reg_t>("sp", &gpr[13]);
	registers_registry["lr"] = new SimpleRegister<reg_t>("lr", &gpr[14]);
	registers_registry["pc"] = new SimpleRegister<reg_t>("pc", &gpr[15]);
	registers_registry["cpsr"] = new SimpleRegister<reg_t>("cpsr", &cpsr);

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
//=             memory interface methods                        START =
//=====================================================================
template<class CONFIG>
void 
CPU<CONFIG> :: Reset() {	
}

template<class CONFIG>
bool 
CPU<CONFIG> :: 
ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size) {
	if(memory_import) 
		return memory_import->ReadMemory(addr, buffer, size);
	return false;
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size) {
	if(memory_import)
		return memory_import->WriteMemory(addr, buffer, size);
	return false;
}

//=====================================================================
//=             memory interface methods                        END   =
//=====================================================================

//=====================================================================
//=             Registers interface methods        START  =
//=====================================================================

template<class CONFIG>
Register *
CPU<CONFIG> ::
GetRegister(const char *name) {
	if(registers_registry.find(*(new string(name))) != registers_registry.end())
		return registers_registry[*(new string(name))];
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
Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t &next_addr) {
  Operation<CONFIG> *op = NULL;
  op = Decoder<CONFIG>::Decode(addr);
  stringstream buffer;
  op->disasm(*this, buffer);
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
GetObjectFriendlyName(typename CONFIG::address_t addr) {
	stringstream sstr;
	
	const Symbol<typename CONFIG::address_t> *symbol = 
		symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<typename CONFIG::address_t>::SYM_OBJECT);
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();	
}

template<class CONFIG>
string
CPU<CONFIG> ::
GetFunctionFriendlyName(typename CONFIG::address_t addr) {
	stringstream sstr;
	
	const Symbol<typename CONFIG::address_t> *symbol = 
		symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<typename CONFIG::address_t>::SYM_FUNC);
	if(symbol)
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
OnBusCycle() {
	
}

template<class CONFIG>
void
CPU<CONFIG> ::
Step() {
	reg_t current_pc;
	reg_t next_pc;

	Operation<CONFIG> *op = NULL;
	current_pc = GetGPR(PC_reg);

	VerboseDumpRegsStart();
	
	if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Starting step at PC = 0x"
			<< Hex << current_pc << Dec
			<< Endl << EndDebugInfo;
	
	if(debug_control_import) {
		typename DebugControl<typename CONFIG::address_t>::DebugCommand dbg_cmd;

		do {
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Fetching debug command (PC = 0x"
					<< Hex << current_pc << Dec << ")"
					<< Endl << EndDebugInfo;
			dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
	
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_STEP) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_STEP command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				break;
			}
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_SYNC) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_SYNC command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				Sync();
				continue;
			}

			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_KILL) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_KILL command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				Stop(0);
			}
			if(dbg_cmd == DebugControl<typename CONFIG::address_t>::DBG_RESET) {
				if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< "Received debug DBG_RESET command (PC = 0x"
						<< Hex << current_pc << Dec << ")"
						<< Endl << EndDebugInfo;
				// TODO : memory_interface->Reset(); 
			}
		} while(1);
	}

	try {
		if(memory_access_reporting_import) {
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Reporting memory acces for fetch at address 0x"
					<< Hex << current_pc << Dec
					<< Endl << EndDebugInfo;
			memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<typename CONFIG::address_t>::MAT_READ, MemoryAccessReporting<typename CONFIG::address_t>::MT_INSN, current_pc, 4);
		}

		insn_t insn;
		if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Fetching (reading) instruction at address 0x"
				<< Hex << current_pc << Dec
				<< Endl << EndDebugInfo;
		
		ReadInsn(current_pc, insn);
//		if(insn_cache_line_address != (current_pc & ~(typename CONFIG::address_t)0x1F)) {
//			insn_cache_line_address = (current_pc & ~(typename CONFIG::address_t)0x1F);
//			ReadInsnLine();
//			//	memory_interface->PrRead(insn_cache_line_address, insn_cache_line, 4 * 8);
//		} 
//		insn = insn_cache_line[(current_pc & (typename CONFIG::address_t)0x1F) >> 2];
		//	insn_t insn_check;
		//	memory_interface->PrRead(current_pc, &insn_check, 4, CC_NONE);
		//	if(insn != insn_check) {
		//		cerr << "error at pc address = 0x" << hex << current_pc << dec << endl;
		//		cerr << "insn = 0x" << hex << insn << dec 
		//			<< ", insn_check = 0x" << hex << insn_check << dec << endl;
		//		cerr << "align = " << ((current_pc & (typename CONFIG::address_t)0x1F) >> 2) << endl;
		//		cerr << "insn_cache_line =";
		//		for(unsigned int i = 0; i < 8; i++)
		//			cerr << " 0x" << hex << insn_cache_line[i] << dec;
		//		cerr << endl;
		//		exit(-1);
		//	}
	
		if(CONFIG::ENDIANESS == E_BIG_ENDIAN)
			insn = BigEndian2Host(insn);
		else
			insn = LittleEndian2Host(insn);
	
		/* Decode current PC */
		if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Decoding instruction at 0x" 
				<< Hex << current_pc << Dec 
				<< Endl << EndDebugInfo;
		op = Decoder<CONFIG>::Decode(current_pc, insn);
		/* Execute instruction */
		if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import) {
			stringstream disasm_str;
			op->disasm(*this, disasm_str);
			(*logger_import) << DebugInfo << LOCATION
				<< "Executing instruction "
				<< disasm_str.str()
				<< " at 0x" << Hex << current_pc << Dec 
				<< " (0x" << Hex << insn << Dec << ")"
				<< Endl << EndDebugInfo;
		}
		op->execute(*this);
		
		VerboseDumpRegsEnd();

		instruction_counter++;
	} 
	catch(ResetException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor reset exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(UndefinedInstructionException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor undefined instruction exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(SoftwareInterruptException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor software interrupt exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(PrefetchAbortException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor prefetch abort exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(DataAbortException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor data abort exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(IRQException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor IRQ exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(FIQException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor FIQ exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	catch(Exception &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	
	if(memory_access_reporting_import)
		memory_access_reporting_import->ReportFinishedInstruction(GetGPR(PC_reg));

	
}

template<class CONFIG>
void
CPU<CONFIG> ::
Run() {
	
}

template<class CONFIG>
void
CPU<CONFIG> ::
Stop(int ret) {
	exit(ret);
}

template<class CONFIG>
void
CPU<CONFIG> ::
Sync() {
	
}

//=====================================================================
//=                    execution handling methods               END   =
//=====================================================================

/**************************************************************/
/* Operand decoding methods     START                         */
/**************************************************************/

/* Data processing operand decoding */
template<class CONFIG>
typename CONFIG::reg_t 
CPU<CONFIG> ::
ShiftOperand32imm(const uint32_t rotate_imm, 
		const uint32_t imm, 
		bool *shift_carry_out) {
	reg_t shifter_operand = 0;
  
	if(rotate_imm == 0) {
		shifter_operand = imm;
		if(shift_carry_out != NULL)
			*shift_carry_out = GetCPSR_C();
	} else {
		shifter_operand = RotateRight(imm, rotate_imm * 2);
		if(shift_carry_out != NULL)
			*shift_carry_out = ((((typename CONFIG::reg_t)(-1)) & ~(((typename CONFIG::reg_t)(-1)) >> 1)) & shifter_operand) != 0;
	}

	return shifter_operand;
}

template<class CONFIG>
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
		if(shift_carry_out != NULL)
			*shift_carry_out = GetCPSR_C();
		return shifter_operand;
	} else {
    
		if(shift == 0) {
			shifter_operand = val_reg << shift_imm;
			//     shifter_operand |= val_reg >> (32 - shift_imm);
			if(shift_carry_out != NULL) {
				mask = mask << (32 - shift_imm);
				*shift_carry_out = ((mask & val_reg) != 0);
			}
			return shifter_operand;
		} else {
      
			if(shift == 0x01) {
#ifndef ARM_OPTIMIZATION
				if(shift_imm == 0) {
					shifter_operand = 0;
					if(shift_carry_out != NULL) {
						mask = mask << 31;
						*shift_carry_out = ((mask & val_reg) != 0);
					}
				} else {
					shifter_operand = val_reg >> shift_imm;
					if(shift_carry_out != NULL) {
						mask = mask << (shift_imm - 1);
						*shift_carry_out = ((mask & val_reg) != 0);
					}
				}
#else
				shifter_operand = (val_reg & 
						(uint32_t)(((int32_t)((int32_t)0x7fffffff << shift_imm)) >> 31)) >> shift_imm;
				if(shift_carry_out != NULL) 
					*shift_carry_out = (val_reg >> ((shift_imm - 1) & 0x01F)) & 0x01;
#endif
				return shifter_operand;
			} else {
				
				if(shift == 0x02) {
					if(shift_imm == 0) {
						if(((typename CONFIG::sreg_t)val_reg) > 0) {
							shifter_operand = 0;
							if(shift_carry_out != NULL) *shift_carry_out = false;
						} else {
							shifter_operand = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
							if(shift_carry_out != NULL) *shift_carry_out = true;
						}
					} else {
						shifter_operand = ((typename CONFIG::sreg_t)val_reg) >> shift_imm;
						if(shift_carry_out != NULL) {
							mask = mask << (shift_imm - 1);
							*shift_carry_out = ((mask & val_reg) != 0);
						}
					}
					return shifter_operand;
				} else {
					
					if(shift == 0x03) { /* ROR */
						if(shift_imm == 0) {
							shifter_operand = 0;
#ifndef ARM_OPTIMIZATION
							if(GetCPSR_C()) {
								shifter_operand = ((typename CONFIG::reg_t)1) << 31;
							}
							shifter_operand |= (val_reg >> 1);
#else
							shifter_operand = (val_reg >> 1) + ((((typename CONFIG::reg_t)1) << 31) * GetCPSR_C());
#endif
							if(shift_carry_out != NULL) {
								*shift_carry_out = ((0x01 & val_reg) != 0);
							}
						} else {
							shifter_operand = RotateRight(val_reg, shift_imm);
							if(shift_carry_out != NULL) {
								*shift_carry_out = (val_reg >> (shift_imm - 1)) & 0x1;
							}
						}
						return shifter_operand;
					}
				}
			}
		}
	}
	
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "Could not compute shifter operand (shift_imm = " << shift_imm << ", shift = " << shift << ")" 
			<< Endl << EndDebugError;
	exit(-1);
	return 0;
}

template<class CONFIG>
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
			if(shift_carry_out != NULL) *shift_carry_out = GetCPSR_C();
		} else if(sr8 < 32) {
			shifter_operand = val_reg << sr8;
			if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & (1 << (32 - sr8))) != 0);
		} else if(sr8 == 32) {
			shifter_operand = 0;
			if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & 0x01) != 0);
		} else {
			shifter_operand = 0;
			if(shift_carry_out) *shift_carry_out = false;
		}
		return shifter_operand;
	} else {

		if(shift == 0x01) {
			if(sr8 == 0) {
				shifter_operand = val_reg;
				if(shift_carry_out != NULL) *shift_carry_out = GetCPSR_C();
			} else if(sr8 < 32) {
				shifter_operand = val_reg >> sr8;
				if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & (1 << (sr8 - 1))) != 0);
			} else if(sr8 == 32) {
				shifter_operand = 0;
				if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & (1 << 31)) != 0);
			} else {
				shifter_operand = 0;
				if(shift_carry_out) *shift_carry_out = false;
			}
			return shifter_operand;
		} else {
			
			if(shift == 0x02) {
				if(sr8 == 0) {
					shifter_operand = val_reg;
					if(shift_carry_out != NULL) *shift_carry_out = GetCPSR_C();
				} else if(sr8 < 32) {
					shifter_operand = ((typename CONFIG::sreg_t)val_reg) >> sr8;
					if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & ((typename CONFIG::reg_t)1 << (sr8 - 1))) != 0);
				} else {
					if((val_reg & ((typename CONFIG::reg_t)1 << 31)) == 0) {
						shifter_operand = 0;
						if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & ((typename CONFIG::reg_t)1 << 31)) != 0);
					} else {
						shifter_operand = (typename CONFIG::reg_t)((typename CONFIG::sreg_t)-1);
						if(shift_carry_out != NULL) *shift_carry_out = ((val_reg & ((typename CONFIG::reg_t)1 << 31)) != 0);
					}
				}
				return shifter_operand;
			} else {
				
				if(shift == 0x03) {
					shifter_operand = RotateRight(val_reg, sr5);
					if(shift_carry_out != NULL) {
						if(sr8 == 0) {
							*shift_carry_out = GetCPSR_C();
						} else {
							*shift_carry_out = (val_reg & (1 << ((sr5 - 1) & 0x01F)));
						}
					}
					return shifter_operand;
				}
			}
		}
	}

	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "Could not compute shifter operand (shift_reg = " << shift_reg << ", shift = " << shift << ")"
			<< Endl << EndDebugError;
	exit(-1);
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
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "unknow shift value (" << shift << ")" 
				<< Endl << EndDebugError;
		exit(-1);
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
    exit(-1);
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
  uint32_t imm_r_m, imm_l_m; // the masks
  uint32_t imm_f; // final immediate
  
  imm_r = imm >> (rotate_imm * 2);
  imm_l = imm << (32 - (rotate_imm * 2));
  
  imm_f = imm_r + imm_l;
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
    exit(-1);
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
      exit(-1);
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
      exit(-1);
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
      exit(-1);
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

  if((src_mode != USER_MODE) &&
     (src_mode != SYSTEM_MODE) &&
     (src_mode != SUPERVISOR_MODE) &&
     (src_mode != ABORT_MODE) &&
     (src_mode != UNDEFINED_MODE) &&
     (src_mode != IRQ_MODE) &&
     (src_mode != FIQ_MODE)) {
    cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
	 << " unknown source running mode (0x" << hex << src_mode << dec << ")" << endl;
    exit(-1);
  }
  if((tar_mode != USER_MODE) &&
     (tar_mode != SYSTEM_MODE) &&
     (tar_mode != SUPERVISOR_MODE) &&
     (tar_mode != ABORT_MODE) &&
     (tar_mode != UNDEFINED_MODE) &&
     (tar_mode != IRQ_MODE) &&
     (tar_mode != FIQ_MODE)) {
    cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
	 << " unknown target running mode (0x" << hex << tar_mode << dec << ")" << endl;
    exit(-1);
  }
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
    cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
	 << " unknown running mode (0x" << hex << src_mode << dec << ")" << endl;
    exit(-1);
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
    cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
	 << " unknown running mode (0x" << hex << tar_mode << dec << ")" << endl;
    exit(-1);
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
	uint32_t mode = cpsr & ~(CPSR_RUNNING_MODE_MASK);
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
		cerr << "ERROR(" << __FUNCTION__ << "): ";
		cerr << "trying to modify SPSR under USER_MODE." << endl;
		exit(-1);
		break;
	case SYSTEM_MODE:
		cerr << "ERROR(" << __FUNCTION__ << "): ";
		cerr << "trying to modify SPSR under SYSTEM_MODE." << endl;
		exit(-1);
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
		cerr << "ERROR(" << __FUNCTION__ << "): ";
		cerr << "unknown running mode." << endl;
		exit(-1);
		break;
	}

	return rm;
}

/* other CPSR/SPSR methods */
template<class CONFIG>
void CPU<CONFIG> ::
MoveSPSRtoCPSR() {

}

/* Instruction condition checking method */
template<class CONFIG>
bool 
CPU<CONFIG> ::
CheckCondition(uint32_t cond) { // check condition type
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
bool
CPU<CONFIG> ::
ReadInsn(typename CONFIG::address_t address, uint32_t &val) {
	if(CONFIG::HAS_INSN_CACHE_L1) {
		cache_il1->PrRead(address, &val, 4);
		return true;
	}
	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrRead(address, &val, 4);
		return true;
	}
	
	memory_interface->PrRead(address, &val, 4);
	return true;
}


template<class CONFIG>
bool 
CPU<CONFIG> ::
Read8(typename CONFIG::address_t address, uint8_t &val) {
	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrRead(address, &val, 1);
	} else
		memory_interface->PrRead(address, &val, 1);

	if(memory_access_reporting_import)
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ,
					MemoryAccessReporting<address_t>::MT_DATA,
				 	address, 1);

	return true;
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
Read16(typename CONFIG::address_t address, uint16_t &val) {
	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrRead(address, &val, 2);
	} else 
		memory_interface->PrRead(address, &val, 2);

	val = Host2Target(CONFIG::ENDIANESS, val);
  
	if(memory_access_reporting_import) 
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ,
					MemoryAccessReporting<address_t>::MT_DATA,
 				   	address, 2);
  
	return true;
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
Read32(typename CONFIG::address_t address, uint32_t &val) {
	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrRead(address, &val, 4);
	} else
		memory_interface->PrRead(address, &val, 4);

	val = Target2Host(CONFIG::ENDIANESS, val);

	if(memory_access_reporting_import) 
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_READ,
				MemoryAccessReporting<address_t>::MT_DATA,
				address, 4);

	return true;
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
Write8(typename CONFIG::address_t address, uint8_t &val) {
	if(logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "address = 0x" << Hex << address << Dec
			<< ", value = " << Hex << val << Dec
			<< Endl << EndDebugInfo;

	uint8_t value;
  
	value = val;
	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrWrite(address, &value, 1);
	} else
		memory_interface->PrWrite(address, &value, 1);

	if(memory_access_reporting_import) 
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_WRITE,
				MemoryAccessReporting<address_t>::MT_DATA,
 				   	address, 1);

	return true;
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
Write16(typename CONFIG::address_t address, uint16_t &val) {
	if(logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "address = 0x" << Hex << address << Dec
			<< ", value = " << Hex << val << Dec
			<< Endl << EndDebugInfo;

	uint16_t cp;
	cp = Host2Target(CONFIG::ENDIANESS, val);

	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrWrite(address, &cp, 2);
	} else
		memory_interface->PrWrite(address, &cp, 2);

	if(memory_access_reporting_import)
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_WRITE,
				MemoryAccessReporting<address_t>::MT_DATA,
				 	address, 2);

	return true;
}

template<class CONFIG>
bool CPU<CONFIG> ::
Write32(typename CONFIG::address_t address, uint32_t &val) {
	uint32_t cp;
  
	cp = Host2Target(CONFIG::ENDIANESS, val);

	if(CONFIG::HAS_DATA_CACHE_L1 || CONFIG::HAS_CACHE_L2) {
		cache_l1->PrWrite(address, &cp, 4);
	} else
		memory_interface->PrWrite(address, &cp, 4);

	if(memory_access_reporting_import) 
		memory_access_reporting_import->ReportMemoryAccess(MemoryAccessReporting<address_t>::MAT_WRITE,
				MemoryAccessReporting<address_t>::MT_DATA,
				 address, 4);

	return true;
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
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
CoprocessorLoad(uint32_t cp_num, address_t address, uint32_t option) {
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
CoprocessorStore(uint32_t cp_num, address_t address) {
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

template<class CONFIG>
bool 
CPU<CONFIG> ::
CoprocessorStore(uint32_t cp_num, address_t address, uint32_t option) {
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

template<class CONFIG>
void 
CPU<CONFIG> ::
CoprocessorDataProcess(uint32_t cp_num, uint32_t op1, uint32_t op2,
		uint32_t crd, uint32_t crn, uint32_t crm) {
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

template<class CONFIG>
void 
CPU<CONFIG> ::
MoveToCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
		uint32_t rd, uint32_t crn, uint32_t crm) {
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

template<class CONFIG>
void
CPU<CONFIG> ::
MoveFromCoprocessor(uint32_t cp_num, uint32_t op1, uint32_t op2, 
		uint32_t rd, uint32_t crn, uint32_t crm) {
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	Stop(-1);
}

/**************************************************************/
/* Coprocessor methods          END                           */
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
	return 4;
}

template<class CONFIG>
uint32_t 
CPU<CONFIG> ::
InstructionBitSize() {
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
	if(logger_import)
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
}

/**************************************************************/
/* Alignment checking methods  END                            */
/**************************************************************/

/**************************************************************/
/* Decode fecth method  START                                 */
/**************************************************************/

template<class CONFIG>
void 
CPU<CONFIG> :: 
Fetch(void *buffer, typename CONFIG::address_t addr, uint32_t size) {
	memory_interface->PrRead(addr, buffer, size);
}

/**************************************************************/
/* Decode fecth method  END                                   */
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

/* endianess methods */
template<class CONFIG>
endian_type 
CPU<CONFIG> ::
GetEndianess() {
	return CONFIG::ENDIANESS;
}

/**************************************************************/
/* Memory system creation methods (private)             START */
/**************************************************************/

template<class CONFIG>
inline INLINE
void 
CPU<CONFIG> ::
CreateMemorySystem() {
	if(CONFIG::HAS_CACHE_L2) {
		cerr << "Configuring cache level 2" << endl;
		cache_l2 = new Cache<typename CONFIG::cache_l2_t>("cache_l2", memory_interface, this);
		cache_l2->memory_import >> memory_import;
		cache_l2->logger_import >> cache_l2_logger_import;
		if(CONFIG::HAS_INSN_CACHE_L1) {
			cerr << "Configuring instruction cache level 1" << endl;
			cache_il1 = new Cache<typename CONFIG::insn_cache_l1_t>("cache_il1", cache_l2, this);
			cache_il1->logger_import >> cache_il1_logger_import;
			if(CONFIG::HAS_DATA_CACHE_L1) {
				cerr << "Configuring data cache level 1" << endl;
				cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_dl1", cache_l2, this);
				cache_l1->memory_import >> cache_l2->memory_export;
				memory_export >> cache_l1->memory_export;
				cache_l1->logger_import >> cache_l1_logger_import;
			} else {
				cerr << "No data data cache level 1" << endl;
				cache_l1 = cache_l2;
				memory_export >> cache_l2->memory_export;
			}
		} else {
			if(CONFIG::HAS_DATA_CACHE_L1) {
				cerr << "Configuring unisified cache level 1" << endl;
				cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_l1", cache_l2, this);
				cache_l1->memory_import >> cache_l2->memory_export;
				memory_export >> cache_l1->memory_export;
				cache_l1->logger_import >> cache_l1_logger_import;
			}
		}
	} else {
		if(CONFIG::HAS_INSN_CACHE_L1) {
			cache_il1 = new Cache<typename CONFIG::insn_cache_l1_t>("cache_il1", memory_interface, this);
			cerr << "Configuring instruction cache level 1" << endl;
			cache_il1->logger_import >> cache_il1_logger_import;
			if(CONFIG::HAS_DATA_CACHE_L1) {
				cerr << "Configuring data cache level 1" << endl;
				cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_dl1", memory_interface, this);
				cache_l1->memory_import >> memory_import;
				memory_export >> cache_l1->memory_export;
				cache_l1->logger_import >> cache_l1_logger_import;
			}
		} else {
			if(CONFIG::HAS_DATA_CACHE_L1) {
				cerr << "Configuring unified cache level 1" << endl;
				cache_l1 = new Cache<typename CONFIG::cache_l1_t>("cache_dl1", memory_interface, this);
				cache_l1->memory_import >> memory_import;
				memory_export >> cache_l1->memory_export;
				cache_l1->logger_import >> cache_l1_logger_import;
			} else {
				cerr << "No caches present in this system" << endl;
				//			cache_l1 = memory_interface;
				//			memory_export >> memory_import;
			}
		}
	}
}

/**************************************************************/
/* Memory system creation methods (private)               END */
/**************************************************************/

/**************************************************************/
/* Verbose methods (protected)                          START */
/**************************************************************/

template<class CONFIG>
inline INLINE
bool
CPU<CONFIG> ::
VerboseSetup() {
	return CONFIG::DEBUG_ENABLE && verbose_setup && logger_import;
}

template<class CONFIG>
inline INLINE
bool
CPU<CONFIG> ::
VerboseStep() {
	return CONFIG::DEBUG_ENABLE && verbose_step && logger_import;
}

template<class CONFIG>
inline INLINE
void
CPU<CONFIG> ::
VerboseDumpRegs() {
	for(unsigned int i = 0; i < 4; i++) {
		for(unsigned int j = 0; j < 4; j++)
			(*logger_import)
				<< "\t- r" << ((i*4) + j) << " = 0x" << Hex << GetGPR((i*4) + j) << Dec;
		(*logger_import) << Endl;
	}
	(*logger_import) << "\t- cpsr = (" << hex << GetCPSR() << dec << ") ";
	typename CONFIG::reg_t mask;
	for(mask = 0x80000000; mask != 0; mask = mask >> 1) {
		if((mask & GetCPSR()) != 0) (*logger_import) << "1";
		else (*logger_import) << "0";
	}
	(*logger_import) << Endl;
	
}

template<class CONFIG>
inline INLINE
void
CPU<CONFIG> ::
VerboseDumpRegsStart() {
	if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_start && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Register dump before starting instruction execution: " << Endl;
		VerboseDumpRegs();
		(*logger_import) << EndDebugInfo;
	}
}

template<class CONFIG>
inline INLINE
void
CPU<CONFIG> ::
VerboseDumpRegsEnd() {
	if(CONFIG::DEBUG_ENABLE && verbose_dump_regs_end && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Register dump at the end of instruction execution: " << Endl;
		VerboseDumpRegs();
		(*logger_import) << EndDebugInfo;
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

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__

