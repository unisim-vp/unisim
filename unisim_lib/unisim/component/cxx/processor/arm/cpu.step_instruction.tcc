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

#define LOCATION  __FUNCTION__ \
					<< ":" <<  __FILE__ \
					<< ":" << __LINE__

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::LittleEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::Target2Host;
using unisim::util::endian::Host2Target;
using unisim::util::arithmetic::RotateRight;

using std::cout;
using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::ostringstream;

using namespace unisim::kernel::logger;

template<class CONFIG>
void
CPU<CONFIG> ::
StepInstruction() {
	reg_t current_pc;
	reg_t next_pc;

	current_pc = GetGPR(PC_reg);

	VerboseDumpRegsStart();

	if(CONFIG::DEBUG_ENABLE && verbose_step) 
		logger << DebugInfo
			<< "Starting step at PC = 0x"
			<< hex << current_pc << dec
			<< EndDebugInfo;
	
	if(debug_control_import) {
		typename DebugControl<uint64_t>::DebugCommand dbg_cmd;

		do {
			if(CONFIG::DEBUG_ENABLE && verbose_step)
				logger << DebugInfo
					<< "Fetching debug command (PC = 0x"
					<< hex << current_pc << dec << ")"
					<< EndDebugInfo;
			dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
	
			if(dbg_cmd == DebugControl<uint64_t>::DBG_STEP) {
				if(CONFIG::DEBUG_ENABLE && verbose_step)
					logger << DebugInfo
						<< "Received debug DBG_STEP command (PC = 0x"
						<< hex << current_pc << dec << ")"
						<< EndDebugInfo;
				break;
			}
			if(dbg_cmd == DebugControl<uint64_t>::DBG_SYNC) {
				if(CONFIG::DEBUG_ENABLE && verbose_step)
					logger << DebugInfo
						<< "Received debug DBG_SYNC command (PC = 0x"
						<< hex << current_pc << dec << ")"
						<< EndDebugInfo;
				Sync();
				continue;
			}

			if(dbg_cmd == DebugControl<uint64_t>::DBG_KILL) {
				if(CONFIG::DEBUG_ENABLE && verbose_step)
					logger << DebugInfo
						<< "Received debug DBG_KILL command (PC = 0x"
						<< hex << current_pc << dec << ")"
						<< EndDebugInfo;
				Stop(0);
			}
			if(dbg_cmd == DebugControl<uint64_t>::DBG_RESET) {
				if(CONFIG::DEBUG_ENABLE && verbose_step)
					logger << DebugInfo
						<< "Received debug DBG_RESET command (PC = 0x"
						<< hex << current_pc << dec << ")"
						<< EndDebugInfo;
				// TODO : memory_interface->Reset(); 
			}
		} while(1);
	}

	if(requires_memory_access_reporting) {
		if(memory_access_reporting_import) {
			if(CONFIG::DEBUG_ENABLE && verbose_step)
				logger << DebugInfo
					<< "Reporting memory acces for fetch at address 0x"
					<< hex << current_pc << dec
					<< EndDebugInfo;
			uint32_t insn_size;
			if(GetCPSR_T())
				insn_size = 2;
			else
				insn_size = 4;
			memory_access_reporting_import->ReportMemoryAccess(
				MemoryAccessReporting<uint64_t>::MAT_READ, 
				MemoryAccessReporting<uint64_t>::MT_INSN, 
				current_pc, insn_size);
		}
	}
	
	try {
		if(GetCPSR_T()) {
			/* THUMB state */
			typename isa::thumb::Operation<CONFIG> *op = NULL;
			thumb_insn_t insn;
			if(CONFIG::DEBUG_ENABLE && verbose_step)
				logger << DebugInfo
					<< "Fetching (reading) instruction at address 0x"
					<< hex << current_pc << dec
					<< EndDebugInfo;

			ReadInsn(current_pc, insn);

			/* convert the instruction to host endianness */
			insn = Target2Host(GetEndianness(), insn);
			
			/* Decode current PC */
			if(CONFIG::DEBUG_ENABLE && verbose_step)
				logger << DebugInfo
					<< "Decoding instruction at 0x"
					<< hex << current_pc << dec
					<< " (0x" << hex << insn << dec << ")"
					<< EndDebugInfo;
			op = thumb_decoder.Decode(current_pc, insn);
			/* Execute instruction */
			if (VerboseStep())
			{
				stringstream disasm_str;
				op->disasm(*this, disasm_str);
				logger << DebugInfo
					<< "Executing instruction "
					<< disasm_str.str()
					<< " at 0x" << hex << current_pc << dec
					<< " (0x" << hex << insn << dec 
					<< ", " << instruction_counter << ")"
					<< EndDebugInfo;
			}
			op->execute(*this);
			//op->profile(profile);
		} 
		else
		{
			/* ARM state */
			typename isa::arm32::Operation<CONFIG> *op = NULL;
			insn_t insn;
			if (VerboseStep())
				logger << DebugInfo
					<< "Fetching (reading) instruction at address 0x"
					<< hex << current_pc << dec
					<< EndDebugInfo;

			ReadInsn(current_pc, insn);

			/* convert the instruction to host endianness */
			insn = Target2Host(GetEndianness(), insn);
			
			/* Decode current PC */
			if ( VerboseStep() )
				logger << DebugInfo 
					<< "Decoding instruction at 0x"
					<< hex << current_pc << dec
					<< " (0x" << hex << insn << dec << ")"
					<< EndDebugInfo;
			op = arm32_decoder.Decode(current_pc, insn);
			/* Execute instruction */
			if (VerboseStep())
			{
				stringstream disasm_str;
				op->disasm(*this, disasm_str);
				logger << DebugInfo 
					<< "Executing instruction "
					<< disasm_str.str()
					<< " at 0x" << hex << current_pc << dec
					<< " (0x" << hex << insn << dec 
					<< ", " << instruction_counter << ")"
					<< EndDebugInfo;
			}
			op->execute(*this);
			//op->profile(profile);
		}
		/* perform the memory load/store operations */
		PerformLoadStoreAccesses();
		VerboseDumpRegsEnd();

		instruction_counter++;
		if (unlikely((trap_on_instruction_counter == instruction_counter) && trap_reporting_import))
			trap_reporting_import->ReportTrap();

		if (unlikely(HasPendingException()))
		{
			if (HasPendingException(IRQ_EXCEPTION)) { ClearPendingException(IRQ_EXCEPTION); throw IRQException<CONFIG>();}
			if (HasPendingException(FIQ_EXCEPTION)) { ClearPendingException(FIQ_EXCEPTION); throw FIQException<CONFIG>();}
		}
	} 
	catch (ResetException<CONFIG> &exc) 
	{
		logger << DebugError 
				<< "Received processor reset exception :" << exc.what() 
				<< EndDebugError;
		if (TrapOnException())
			trap_reporting_import->ReportTrap();
		PerformResetException();
	}
	catch (UndefinedInstructionException<CONFIG> &exc) 
	{
		if (VerboseException()) 
			logger << DebugInfo 
				<< "Received undefined instruction exception: " << exc.what()
				<< EndDebugInfo;
		if (TrapOnException())
			trap_reporting_import->ReportTrap();
		PerformUndefInsnException();
	}
	catch (SoftwareInterruptException<CONFIG> &exc) 
	{
		if (VerboseException()) 
			logger << DebugInfo 
				<< "Received software interrupt exception: " << exc.what()
				<< EndDebugInfo;
		if (TrapOnException())
			trap_reporting_import->ReportTrap();
		PerformSWIException();
	}
	catch (PrefetchAbortException<CONFIG> &exc) 
	{
		logger << DebugError 
			<< "Received processor prefetch abort exception :" << exc.what() << endl
			<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << EndDebugError;
		if (TrapOnException())
			trap_reporting_import->ReportTrap();
		PerformPrefetchAbortException();
	}
	catch (DataAbortException<CONFIG> &exc) 
	{
		logger << DebugError 
			<< "Received processor data abort exception :" << exc.what() 
			<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << EndDebugError;
		if (TrapOnException())
			trap_reporting_import->ReportTrap();
		PerformDataAbortException();
	}
	catch (IRQException<CONFIG> &exc) 
	{
		if (VerboseException())
			logger << DebugInfo 
				<< "Received processor IRQ exception:" << exc.what() 
				<< EndDebugInfo;
		if (TrapOnException())
		{
			stringstream ss;
			ss << "Received IRQException on PC = 0x" 
				<< hex << GetGPR(PC_reg) << dec;
			ss << " while on ";
			switch(GetCPSR_Mode())
			{
				case USER_MODE:
					ss << "USER_MODE";
					break;
				case SYSTEM_MODE:
					ss << "SYSTEM_MODE";
					break;
				case SUPERVISOR_MODE:
					ss << "SUPERVISOR_MODE";
					break;
				case ABORT_MODE:
					ss << "ABORT_MODE";
					break;
				case UNDEFINED_MODE:
					ss << "UNDEFINED_MODE";
					break;
				case IRQ_MODE:
					ss << "IRQ_MODE";
					break;
				case FIQ_MODE:
					ss << "FIQ_MODE";
					break;
			}
			trap_reporting_import->ReportTrap(*this, ss.str().c_str());
		}
		PerformIRQException();
	}
	catch (FIQException<CONFIG> &exc) 
	{
		if (VerboseException())
			logger << DebugInfo
				<< "Received processor FIQ exception:" << exc.what()
				<< EndDebugInfo;
		if (TrapOnException())
			trap_reporting_import->ReportTrap();
		PerformFIQException();
	}
	catch (Exception &exc) 
	{
		logger << DebugError 
			<< "Received unhandled processor exception :" << exc.what() 
			<< "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << EndDebugError;
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

#undef likely
#undef unlikely

#endif // SOCLIB

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_INSTRUCTION_TCC__
