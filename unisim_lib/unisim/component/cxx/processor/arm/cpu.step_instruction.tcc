/*
 *  Copyright (c) 2008,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_INSTRUCTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_INSTRUCTION_TCC__

#ifndef SOCLIB

#define LOCATION Function << __FUNCTION__ \
                        << File <<  __FILE__ \
                        << Line << __LINE__

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

using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;
using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::arithmetic::RotateRight;

using std::cout;
using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::ostringstream;

template<class CONFIG>
void
CPU<CONFIG> ::
StepInstruction() {
	reg_t current_pc;
	reg_t next_pc;

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

	if(requires_memory_access_reporting) {
		if(memory_access_reporting_import) {
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Reporting memory acces for fetch at address 0x"
					<< Hex << current_pc << Dec
					<< Endl << EndDebugInfo;
			uint32_t insn_size;
			if(GetCPSR_T())
				insn_size = 2;
			else
				insn_size = 4;
			memory_access_reporting_import->ReportMemoryAccess(
				MemoryAccessReporting<typename CONFIG::address_t>::MAT_READ, 
				MemoryAccessReporting<typename CONFIG::address_t>::MT_INSN, 
				current_pc, insn_size);
		}
	}
	
	try {
		if(GetCPSR_T()) {
			/* THUMB state */
			typename isa::thumb::Operation<CONFIG> *op = NULL;
			thumb_insn_t insn;
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Fetching (reading) instruction at address 0x"
					<< Hex << current_pc << Dec
					<< Endl << EndDebugInfo;

			ReadInsn(current_pc, insn);
			/* 
			 * if(insn_cache_line_address != 
				(current_pc & ~(typename CONFIG::address_t)0x1F)) {
				insn_cache_line_address = 
					(current_pc & ~(typename CONFIG::address_t)0x1F);
				ReadInsnLine(insn_cache_line_address, 
					(uint8_t *)&insn_cache_line, 4 *8);
			}
			insn = ((uint16_t *)insn_cache_line)[(current_pc & 
						(typename CONFIG::address_t)0x1F) >> 1];
						*/

			/* arm instructions are big endian, so convert the instruction to 
			 *   host format */
			insn = BigEndian2Host(insn);
			
			/* Decode current PC */
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Decoding instruction at 0x"
					<< Hex << current_pc << Dec
					<< " (0x" << Hex << insn << Dec << ")"
					<< Endl << EndDebugInfo;
			op = thumb_decoder.Decode(current_pc, insn);
			/* Execute instruction */
			if(CONFIG::DEBUG_ENABLE && (verbose_step 
				|| verbose_step_insn) && logger_import) {
				stringstream disasm_str;
				op->disasm(*this, disasm_str);
				(*logger_import) << DebugInfo << LOCATION
					<< "Executing instruction "
					<< disasm_str.str()
					<< " at 0x" << Hex << current_pc << Dec
					<< " (0x" << Hex << insn << Dec 
					<< ", " << instruction_counter << ")"
					<< Endl << EndDebugInfo;
			}
			op->execute(*this);
			//op->profile(profile);
		} else {
			/* ARM state */
			typename isa::arm32::Operation<CONFIG> *op = NULL;
			insn_t insn;
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Fetching (reading) instruction at address 0x"
					<< Hex << current_pc << Dec
					<< Endl << EndDebugInfo;

			ReadInsn(current_pc, insn);
			/*
			 * if(insn_cache_line_address != 
				(current_pc & ~(typename CONFIG::address_t)0x1F)) {
				insn_cache_line_address = 
					(current_pc & ~(typename CONFIG::address_t)0x1F);
				ReadInsnLine(insn_cache_line_address, 
					(uint8_t *)&insn_cache_line, 4 *8);
			}
			insn = insn_cache_line[(current_pc & 
				(typename CONFIG::address_t)0x1F) >> 2];
				*/

			/* arm instructions are big endian, so convert the instruction to 
			 *   host format */
			insn = BigEndian2Host(insn);
			
			/* Decode current PC */
			if(CONFIG::DEBUG_ENABLE && verbose_step && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< "Decoding instruction at 0x"
					<< Hex << current_pc << Dec
					<< " (0x" << Hex << insn << Dec << ")"
					<< Endl << EndDebugInfo;
			op = arm32_decoder.Decode(current_pc, insn);
			/* Execute instruction */
			if(CONFIG::DEBUG_ENABLE && 
				(verbose_step || verbose_step_insn) && logger_import) {
				stringstream disasm_str;
				op->disasm(*this, disasm_str);
				(*logger_import) << DebugInfo << LOCATION
					<< "Executing instruction "
					<< disasm_str.str()
					<< " at 0x" << Hex << current_pc << Dec
					<< " (0x" << Hex << insn << Dec 
					<< ", " << instruction_counter << ")"
					<< Endl << EndDebugInfo;
			}
			op->execute(*this);
			//op->profile(profile);
		}
		/* perform the memory load/store operations */
		PerformLoadStoreAccesses();
		VerboseDumpRegsEnd();

		instruction_counter++;
	} 
	catch(ResetException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor reset exception :" << exc.what() 
				<< Endl << EndDebugError;
		PerformResetException();
	}
	catch(UndefinedInstructionException<CONFIG> &exc) {
		if(VerboseStep()) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Received undefined instruction exception: " << exc.what()
				<< Endl << EndDebugInfo;
		}
		PerformUndefInsnException();
	}
	catch(SoftwareInterruptException<CONFIG> &exc) {
		if(VerboseStep()) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Received software interrupt exception: " << exc.what()
				<< Endl << EndDebugInfo;
		}
		PerformSWIException();
	}
	catch(PrefetchAbortException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor prefetch abort exception :" << exc.what()
				<< Endl << EndDebugError;
		PerformPrefetchAbortException();
	}
	catch(DataAbortException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor data abort exception :" << exc.what() 
				<< Endl << EndDebugError;
		PerformDataAbortException();
	}
	catch(IRQException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor IRQ exception :" << exc.what() 
				<< Endl << EndDebugError;
		PerformIRQException();
	}
	catch(FIQException<CONFIG> &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor FIQ exception :" << exc.what() 
				<< Endl << EndDebugError;
		PerformFIQException();
	}
	catch(Exception &exc) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION 
				<< "uncaught processor exception :" << exc.what() 
				<< Endl << EndDebugError;
		Stop(1);
	}
	
	if(requires_finished_instruction_reporting)
		if(memory_access_reporting_import)
			memory_access_reporting_import->ReportFinishedInstruction(GetGPR(PC_reg));
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#undef INLINE

#endif // SOCLIB

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_INSTRUCTION_TCC__
