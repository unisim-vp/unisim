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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_SOCLIB_STEP_CYCLE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_SOCLIB_STEP_CYCLE_TCC__

#ifdef SOCLIB

#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/config.hh"
#include <inttypes.h>
#include <map>

#define SOCLIB_DEBUG

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;

	
template<class CONFIG>
void 
CPU<CONFIG> ::
Reset() {
#ifdef SOCLIB_DEBUG
	cerr << "+++ Reset" << endl;
#endif
	FlushPipeline();
	/* Reset all the registers */
	InitGPR();

	/* we are running in system mode */
	/* currently supported: arm966e_s
	 * if different report error */
	if(CONFIG::MODEL != ARM966E_S) {
		cerr << "Error(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
			<< "):" << endl
			<< "Running arm in system mode"
			<< ". Only arm966e_s can run under this mode"
			<< endl;
		exit(-1);
	}
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
	exception = 0;
	// set initial pc
	/* Depending on the configuration being used set the initial pc */
	if(CONFIG::MODEL == ARM966E_S) {
		if(arm966es_vinithi)
			SetGPR(PC_reg, (address_t)UINT32_C(0xffff0000));
		else
			SetGPR(PC_reg, (address_t)UINT32_C(0x00000000));
		/* disable normal and fast interruptions */
		SetCPSR_F();
		SetCPSR_I();
	}
	if(CONFIG::MODEL == ARM7TDMI) {
		SetGPR(PC_reg, (address_t)UINT32_C(0x00000000));
		/* disable normal and fast interruptions */
		SetCPSR_F();
		SetCPSR_I();
	}

	fetch_pc = GetGPR(PC_reg);
#ifdef SOCLIB_DEBUG
	cerr << "    current fetch_pc = 0x" << hex << fetch_pc << dec << endl;
#endif 
	fetchQueue = InstructionFactory<CONFIG>::New();
	fetchQueue->SetFetchAddress(fetch_pc);
	fetch_pc += InstructionByteSize();
	fetchQueue->SetPredictedNextFetchAddress(fetch_pc);
#ifdef SOCLIB_DEBUG
	cerr << "    next fetch_pc = 0x" << hex << fetch_pc << dec << endl;
#endif
	// check the local memory before letting the system perform an external memory access
	if(CONFIG::MODEL == ARM966E_S) {
		if(GetCPSR_T()) {
			thumb_insn_t insn;
			external_memory_request = false;
			ReadInsn(fetchQueue->GetFetchAddress(), insn);
			if(!external_memory_request) {
				insn = BigEndian2Host(insn);
				fetchQueue->SetThumbEncoding(insn);
				fetchQueue->SetRequested(true);
				fetchQueue->SetFetched(true);
				address_t addr = fetchQueue->GetFetchAddress();
				typename isa::thumb::Operation<CONFIG> *op = NULL;
				thumb_insn_t insn = fetchQueue->GetThumbEncoding();
				op = thumb_decoder.Decode(addr, insn);
				fetchQueue->SetOpcode(op);
			}
		} else {
			insn_t insn;
			external_memory_request = false;
			ReadInsn(fetchQueue->GetFetchAddress(), insn);
			if(!external_memory_request) {
				insn = BigEndian2Host(insn);
				fetchQueue->SetArm32Encoding(insn);
				fetchQueue->SetRequested(true);
				fetchQueue->SetFetched(true);
				address_t addr = fetchQueue->GetFetchAddress();
				typename isa::arm32::Operation<CONFIG> *op = NULL;
				insn_t insn = fetchQueue->GetArm32Encoding();
				op = arm32_decoder.Decode(addr, insn);
				fetchQueue->SetOpcode(op);
			}
		}

	}
	// TODO
	// set the correct fetch pc
	// initialize all the registers
	// delete all the instructions
}

template<class CONFIG>
uint32_t 
CPU<CONFIG> ::
IsBusy() {
	/* The processor is busy if one instruction remains in the execute queue
	 *   of if the current one is going to be removed */
	/* TOCHECK: what did I do here?
	 * Needs to be checked again. Why the lsQueue must be empty? */
	return 0;
	bool busy = false;

#ifdef SOCLIB_DEBUG
	cerr << "+++ IsBusy" << endl;
#endif

	busy = executeQueue != 0 &&
		executeQueue->GetRemainingExecCycles() == 0 &&
		lsQueue.empty();
#ifdef SOCLIB_DEBUG
	cerr << "    - busy = " << busy << endl;
#endif
	return busy?1:0;
}

template<class CONFIG>
void
CPU<CONFIG> ::
FlushPipeline() {
#ifdef SOCLIB_DEBUG
	cerr << "+++ FlushPipeline" << endl;
#endif
	// todo: Check
	/* the instructions in the pipeline can be flushed */
	if(executeQueue) {
		InstructionFactory<CONFIG>::Destroy(executeQueue);
		executeQueue = 0;
	}
	if(decodeQueue) {
		InstructionFactory<CONFIG>::Destroy(decodeQueue);
		decodeQueue = 0;
	}
	if(fetchQueue) {
		if(fetchQueue->IsFetched()) {
			InstructionFactory<CONFIG>::Destroy(fetchQueue);
			fetchQueue = 0;
		} else {
			fetchQueue->Flush();
		}
//		InstructionFactory<CONFIG>::Destroy(fetchQueue);
//		fetchQueue = 0;
	}
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepExecute() {
	stringstream str;
	/* if the instruction has not been executed now it is the moment to 
	 *   do so */
	if(!executeQueue->HasBeenExecuted()) {
#ifdef SOCLIB_DEBUG
		cerr << "    - executing 0x" << hex << executeQueue->GetFetchAddress() << dec << endl;
#endif
		try{
			if(executeQueue->IsArm32()) {
				typename isa::arm32::Operation<CONFIG> *op = 
					arm32_decoder.Decode(
						executeQueue->GetFetchAddress(),
						executeQueue->GetArm32Encoding());
				op->disasm(*this, str);
				op->execute(*this);
#ifdef SOCLIB_DEBUG
				cerr << "    - arm32 instruction: " << str.str() << endl;
#endif
				executeQueue->SetOpcode(op);
			} else {
				typename isa::thumb::Operation<CONFIG> *op = 
					thumb_decoder.Decode(
						executeQueue->GetFetchAddress(),
						executeQueue->GetThumbEncoding());
				op->execute(*this);
				op->disasm(*this, str);
#ifdef SOCLIB_DEBUG
				cerr << "    - thumb instruction: " << str.str() << endl;
#endif
				executeQueue->SetOpcode(op);
			}
			executeQueue->SetExecuted();
			/* set the instruction latency 
			 * TODO: get the instruction latency from the opcode */
			executeQueue->SetExecCycles(1);
			/* prepare memory accesses to internal/external memory */
			if(!lsQueue.empty()) PerformLoadStoreAccesses();
		} catch(ResetException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received a RESET exception ****" << endl;
#endif
			exception |= RESET_EXCEPTION;
		} catch(UndefinedInstructionException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received an UndefinedInstruction exception ****" << endl;
#endif
			exception |= UNDEFINED_INSTRUCTION_EXCEPTION;
		} catch(SoftwareInterruptException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received a SoftwareInterrupt(SWI) exception ****" << endl;
#endif
			exception |= SOFTWARE_INTERRUPT_EXCEPTION;
		} catch(PrefetchAbortException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received a PrefetchAbort exception ****" << endl;
#endif
			exception |= PREFETCH_ABORT_EXCEPTION;
		} catch(DataAbortException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received a DataAbort exception ****" << endl;
#endif
			exception |= DATA_ABORT_EXCEPTION;
		} catch(IRQException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received an IRQ exception ****" << endl;
			cerr << "     This could not have been received here (maybe an error?)" << endl;
#endif
			exception |= IRQ_EXCEPTION;
		} catch(FIQException<CONFIG> &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received a FIQ exception ****" << endl;
			cerr << "     This could not have been received here (maybe an error?)" << endl;
#endif
			exception |= FIQ_EXCEPTION;
		} catch(Exception &exc) {
#ifdef SOCLIB_DEBUG
			cerr << "**** Received an unhandled processor exception ****" << endl;
#endif
			Stop(1);
		}
	}
//	/* check if the current instruction has any memory request pending 
//	 * move possible memory operations from the lsQueue if the is no
//	 *   memory operation being serviced */
//	if(firstLS == 0 && !lsQueue.empty()) {
//		/* IMPORTANT: we do not remove the instruction from the lsQueue
//		 *   still, we wait for it to be completed */
//		firstLS = lsQueue.front();
//	}
	/* check if the current instruction has finished its operation, 
	 * if so it can be removed from the pipeline */
	if(executeQueue->GetRemainingExecCycles() != 0)
		executeQueue->DecExecCycles();

}

template<class CONFIG>
void
CPU<CONFIG> ::
Step() {
#ifdef SOCLIB_DEBUG
	cerr << "+++ Step" << endl;
#endif
	/* TODO 
	 * check for exceptions
	 * INFO
	 * the checking for the exceptions could also be done at the beginning of the step
	 *   or it could even be broken in separate handlers for internal and external exceptions
	 *   (and other weird possibilities)
	 */
	if(HandleExceptions()) {
		// The pipeline needs to be flushed
		FlushPipeline();
		fetch_pc = GetGPR(PC_reg);
	}

	/* when Step is called it means that the last SetInstruction was done with
	 *   the right info and that it can be used by the processor, so set the
	 *   instruction in the fetchQueue as requested and fetched
	 * if there is not instruction in the fetchQueue then something has gone wrong,
	 *   show message and exit
	 * if there is an instruction in the fetchQueue which has been fetched and 
	 *   requested then something has gone wrong, show message and exit
	 */
	if(fetchQueue == 0) {
		cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< "no instruction in the fetch queue so nothing could be received."
			<< endl;
		exit(-1);
	}
//	if(fetchQueue != 0 && (fetchQueue->IsRequested() || fetchQueue->IsFetched())) {
//		cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
//			<< "the instruction in the fetch queue was already fetched/requested."
//			<< endl;
//		exit(-1);
//	}

	if(fetchQueue->IsFlushed()) {
#ifdef SOCLIB_DEBUG
		cerr << "    - the received instruction (pc = 0x" << hex << fetchQueue->GetFetchAddress() << dec
			<< ") needs to be flushed" << endl;
#endif
		InstructionFactory<CONFIG>::Destroy(fetchQueue);
		fetchQueue = 0;
	} else {
		fetchQueue->SetRequested(true);
		fetchQueue->SetFetched(true);

		address_t addr = fetchQueue->GetFetchAddress();
		if(GetCPSR_T()) {
			// it is a thumb instruction
			typename isa::thumb::Operation<CONFIG> *op = NULL;
			thumb_insn_t insn = fetchQueue->GetThumbEncoding();
			op = thumb_decoder.Decode(addr, insn);
			fetchQueue->SetOpcode(op);
		} else {
			// it is an arm32 instruction
			typename isa::arm32::Operation<CONFIG> *op = NULL;
			insn_t insn = fetchQueue->GetArm32Encoding();
			op = arm32_decoder.Decode(addr, insn);
			fetchQueue->SetOpcode(op);
		}
	}

	StepCycle();

}

template<class CONFIG>
bool
CPU<CONFIG> ::
HandleExceptions() {
	if(exception) {
		// limitation:
		// we can raise an exception if the following conditions are true:
		// - there is no instruction in the executeQueue
		// - if there is an instruction in the executeQueue:
		//   - it has not been executed
		//   - it has been executed but the number of remaining cycles is 0
		if(executeQueue != 0) return false;
		else {
			if(!executeQueue->HasBeenExecuted()) return false;
			else {
				if(executeQueue->HasBeenExecuted() &&
						executeQueue->GetRemainingExecCycles() != 0) return false;
			}
		}

		// check the exception that was produced and if they are handled or not
		// checking exceptions by their priority
		// - 1 : Reset
		// - 2 : Data abort
		// - 3 : FIQ
		// - 4 : IRQ
		// - 5 : Prefetch abort
		// - 6 : Undefined instruction / SWI

		// 1 : checking Reset exception
		if(exception & RESET_EXCEPTION) {
			uint32_t mask = RESET_EXCEPTION;
			mask = ~mask;
			exception &= mask;
			PerformResetException();
			return true;
		}

		// 2 : checking Data abort exception
		if(exception & DATA_ABORT_EXCEPTION) {
			uint32_t mask = DATA_ABORT_EXCEPTION;
			mask = ~mask;
			exception &= mask;
			PerformDataAbortException();
			return true;
		}

		// 3 : checking FIQ exception
		if(exception & FIQ_EXCEPTION) {
			if(GetCPSR_F()) {
				// FIQ interruptions are disabled
				// do nothing
				return false;
			} else {
				uint32_t mask = FIQ_EXCEPTION;
				mask = ~mask;
				exception &= mask;
				PerformFIQException();
				return true;
			}
		}

		// 4 : checking IRQ exception
		if(exception & IRQ_EXCEPTION) {
			if(GetCPSR_I()) {
				// IRQ interruptions are disabled
				// do nothing
				return false;
			} else {
				uint32_t mask = IRQ_EXCEPTION;
				mask = ~mask;
				exception &= mask;
				PerformIRQException();
				return true;
			}
		}
		
		// 5 : checking Prefetch abort
		if(exception & PREFETCH_ABORT_EXCEPTION) {
			uint32_t mask = PREFETCH_ABORT_EXCEPTION;
			mask = ~mask;
			exception &= mask;
			PerformPrefetchAbortException();
			return true;
		}

		// 6 : checking Undefined instruction / SWI
		if(exception & UNDEFINED_INSTRUCTION_EXCEPTION) {
			uint32_t mask = UNDEFINED_INSTRUCTION_EXCEPTION;
			mask = ~mask;
			exception &= mask;
			PerformUndefInsnException();
			return true;
		}

		if(exception & SOFTWARE_INTERRUPT_EXCEPTION) {
			uint32_t mask = SOFTWARE_INTERRUPT_EXCEPTION;
			mask = ~mask;
			exception &= mask;
			PerformSWIException();
			return true;
		}
	}
	return false;
}

template<class CONFIG>
void 
CPU<CONFIG> ::
StepCycle() {
#ifdef SOCLIB_DEBUG
	cerr << "+++ StepCycle" << endl;
#endif

#ifdef SOCLIB_DEBUG
	cerr << "    Memory Ops" << endl;
#endif
	/* do memory accesses */
	if(!lsQueue.empty()) {
		MemoryOp<CONFIG> *memop = lsQueue.front();
		if(!memop->IsExternal()) {
			PerformLoadStoreAccesses();
		}
	}

	/* start by removing the instruction in the executeQueue (if any) if it
	 *   has finished its operation
	 * check if the pipeline needs to be flushed */
#ifdef SOCLIB_DEBUG
	cerr << "    Execute" << endl;
#endif
	if(executeQueue != 0 &&
			executeQueue->GetRemainingExecCycles() == 0 &&
			lsQueue.empty()) {

#ifdef PROFILE_ARM966
		map<uint32_t, insn_profile_t *>::iterator iter;
		iter = insn_profile.find(executeQueue->GetFetchAddress());
		if(iter == insn_profile.end()) {
			insn_profile_t *prof = new insn_profile_t();
			prof->ex_time = 1;
			prof->num_times_executed = 1;
			insn_profile[executeQueue->GetFetchAddress()] = prof;
		} else {
			iter->second->num_times_executed++;
			iter->second->ex_time++;
		}
#endif // PROFILE_ARM966

		if(GetGPR(PC_reg) != executeQueue->GetPredictedNextFetchAddress()) {
#ifdef SOCLIB_DEBUG
			cerr << "    - misspredicted pc 0x" << hex << executeQueue->GetPredictedNextFetchAddress() << dec << " -> 0x" << hex << GetGPR(PC_reg) << dec << endl;
#endif
			FlushPipeline();
#ifdef SOCLIB_DEBUG
			cerr << "    - flushing pipeline" << endl;
#endif
			fetch_pc = GetGPR(PC_reg);
#ifdef SOCLIB_DEBUG
			cerr << "    - setting pc to 0x" << hex << fetch_pc << dec << endl;
#endif
		} else {
#ifdef SOCLIB_DEBUG
			cerr << "    - instruction 0x" << hex << executeQueue->GetFetchAddress() << dec << " finished" << endl;
#endif
			InstructionFactory<CONFIG>::Destroy(executeQueue);
			executeQueue = 0;
		}	
		//InstructionFactory<CONFIG>::Destroy(executeQueue);
		//executeQueue = 0;
	} else {
#ifdef PROFILE_ARM966
		if(executeQueue != 0) {
			map<uint32_t, insn_profile_t *>::iterator iter;
			iter = insn_profile.find(executeQueue->GetFetchAddress());
			if(iter == insn_profile.end()) {
				insn_profile_t *prof = new insn_profile_t();
				prof->ex_time = 1;
				prof->num_times_executed = 0;
				insn_profile[executeQueue->GetFetchAddress()] = prof;
			} else {
				iter->second->ex_time++;
			}
		}
#endif // PROFILE_ARM966
	}

	/* if execute queue is empty then move the decode queue entry to execute
	 *   queue and execute the instruction. if the instruction is finished then
	 *   remove it */
	/* if the execute queue is not empty then perform the necessary actions
	 *   to finish the instruction, i.e.: send a load, store, etc...*/
	if(executeQueue == 0) {
		if(decodeQueue != 0) {
#ifdef SOCLIB_DEBUG
			cerr << "    - moving from decodeQueue 0x" << hex 
				<< decodeQueue->GetFetchAddress() << dec << endl;
#endif
			executeQueue = decodeQueue;
			decodeQueue = 0;
			StepExecute();
		}
	}

		
	/* if the decode queue is not empty, then do nothing */
	/* if the decode queue is empty, then move the instruction present in
	 *   the fetch queue to the decode queue */
#ifdef SOCLIB_DEBUG
	cerr << "    Decode" << endl;
#endif
	if(decodeQueue == 0) {
		if(fetchQueue != 0) {
			if(fetchQueue->IsFetched()) {
#ifdef SOCLIB_DEBUG
				cerr << "    - moving from fetchQueue 0x" << hex
					<< fetchQueue->GetFetchAddress() << dec << endl;
#endif
				decodeQueue = fetchQueue;
				fetchQueue = 0;
			}
		}
	}

	/* if the fetch queue is not empty, the do nothing 
	 * if the fetch queue is empty, then send a read request to fetch fetch_pc
	 *   instruction, after that increase fetch_pc */
#ifdef SOCLIB_DEBUG
	cerr << "    Fetch" << endl;
#endif
	if(fetchQueue == 0) {
		fetchQueue = InstructionFactory<CONFIG>::New();
		fetchQueue->SetFetchAddress(fetch_pc);
#ifdef SOCLIB_DEBUG
		cerr << "    - current fetch_pc = 0x" << hex << fetch_pc << dec << endl;
#endif
		fetch_pc += InstructionByteSize();
		fetchQueue->SetPredictedNextFetchAddress(fetch_pc);
#ifdef SOCLIB_DEBUG
		cerr << "    - next fetch_pc = 0x" << hex << fetch_pc << dec << endl;
#endif
		// check the local memory before letting the system perform an external memory access
		if(CONFIG::MODEL == ARM966E_S) {
			if(GetCPSR_T()) {
				thumb_insn_t insn;
				external_memory_request = false;
				ReadInsn(fetchQueue->GetFetchAddress(), insn);
				if(!external_memory_request) {
					cerr << "    - instruction found in internal memory" << endl;
					insn = BigEndian2Host(insn);
					fetchQueue->SetThumbEncoding(insn);
					fetchQueue->SetRequested(true);
					fetchQueue->SetFetched(true);
					address_t addr = fetchQueue->GetFetchAddress();
					typename isa::thumb::Operation<CONFIG> *op = NULL;
					thumb_insn_t insn = fetchQueue->GetThumbEncoding();
					op = thumb_decoder.Decode(addr, insn);
					fetchQueue->SetOpcode(op);
				}
			} else {
				insn_t insn;
				external_memory_request = false;
				ReadInsn(fetchQueue->GetFetchAddress(), insn);
				if(!external_memory_request) {
					cerr << "   - instruction found in internal memory" << endl;
					insn = BigEndian2Host(insn);
					fetchQueue->SetArm32Encoding(insn);
					fetchQueue->SetRequested(true);
					fetchQueue->SetFetched(true);
					address_t addr = fetchQueue->GetFetchAddress();
					typename isa::arm32::Operation<CONFIG> *op = NULL;
					insn_t insn = fetchQueue->GetArm32Encoding();
					op = arm32_decoder.Decode(addr, insn);
					fetchQueue->SetOpcode(op);
				}
			}
		}
	} else {
#ifdef SOCLIB_DEBUG
		cerr << "    - fetching = 0x" << hex << fetchQueue->GetFetchAddress() << dec << endl;
		cerr << "    - is_fetched = " << (fetchQueue->IsFetched()?"true":"false") << endl;
		cerr << "    - is_requested = " << (fetchQueue->IsRequested()?"true":"false") << endl;
#endif
	}
}

template<class CONFIG>
void 
CPU<CONFIG> ::
NullStep(uint32_t time_passed) {
#ifdef SOCLIB_DEBUG
	cerr << "+++ NullStep(" << time_passed << ")" << endl;
#endif
//	if(fetchQueue != 0) 
//	  fetchQueue->SetFetched(false);
	// TODO: check
	for(uint32_t i = 0; i < time_passed; i++)
		StepCycle();
}

template<class CONFIG>
void 
CPU<CONFIG> ::
GetInstructionRequest(bool &req, uint32_t &addr) const {
#ifdef SOCLIB_DEBUG
	cerr << "+++ GetInstructionRequest" << endl;
#endif
	/* check if the instruction on the fetch queue has been requested,
	 *   if not then return the address to fetch */
	req = false;
	if(fetchQueue != 0) {
		if(fetchQueue->IsRequested() == false) {
			addr = fetchQueue->GetFetchAddress();
			addr = addr & UINT32_C(0xfffffffc);
			req = true;
		//	fetchQueue->SetRequested(true);
		}
	}
#ifdef SOCLIB_DEBUG
	if(!req)
		cerr << "    No instruction to fetch" << endl;
	else {
		cerr << "    req = " << req;
		if(req) cerr << ", addr = 0x" << hex << addr << dec;
		cerr << endl;
	}
#endif
	return;
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetInstruction(bool error, uint32_t val) {
#ifdef SOCLIB_DEBUG
	cerr << "+++ SetInstruction (error = " << error << ", val = 0x" << hex << val << dec << ")" << endl;
#endif
	// TODO
	/* set the instruction opcode using val, now it could be a good moment to
	 *   decode the instruction */
	if(error) {
		// TODO: what to do in case of a bus error
		cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
			<< "): an error was received. What to do?" << endl;
		exit(-1);
	}

	address_t addr = fetchQueue->GetFetchAddress();
	if(GetCPSR_T()) {
		// it is a thumb instruction
		// TOCHECK: what do we need to read the lower part or the higher ???
		// typename isa::thumb::Operation<CONFIG> *op = NULL;
		uint16_t val16;
		if(addr & UINT32_C(0X03)) {
			// not aligned read the higher bits
			val = val >> 16;
		}
		val16 = val & UINT32_C(0x0000ffff);
		thumb_insn_t insn = val16;
		insn = BigEndian2Host(insn);
		fetchQueue->SetThumbEncoding(insn);
		// op = thumb_decoder.Decode(addr, insn);
		// fetchQueue->SetOpcode(op);
	} else {
		// it is an arm32 instruction
		// typename isa::arm32::Operation<CONFIG> *op = NULL;
		insn_t insn = val;
		insn = BigEndian2Host(insn);
		fetchQueue->SetArm32Encoding(insn);
		// op = arm32_decoder.Decode(addr, insn);
		// fetchQueue->SetOpcode(op);
	}
}

template<class CONFIG>
void 
CPU<CONFIG> ::
GetDataRequest(bool &reg, bool &is_read, int &size, uint32_t &addr,
		uint32_t &data) const {
	uint16_t hdata;
#ifdef SOCLIB_DEBUG
	cerr << "+++ GetDataRequest" << endl;
#endif
	MemoryOp<CONFIG> *memop = 0;
	reg = false;
	/* check if the instruction in the execute queue has any load/store to
	 *   be done, if so, execute it. */
	if(lsQueue.empty()) {
		reg = false;
#ifdef SOCLIB_DEBUG
		cerr << "    no memory operation ready" << endl;
#endif
		return;
	}
	memop = lsQueue.front();
	if(!memop->IsExternal()) return;
	reg = true;
	size = memop->GetSize();
	addr = memop->GetAddress();
	switch(memop->GetType()) {
	case MemoryOp<CONFIG>::READ:
	case MemoryOp<CONFIG>::READ_TO_PC_UPDATE_T:
	case MemoryOp<CONFIG>::READ_TO_PC:
	case MemoryOp<CONFIG>::PREFETCH:
		is_read = true;
		break;
	case MemoryOp<CONFIG>::WRITE:
		is_read = false;
		data = memop->GetWriteValue();
		switch(size) {
		case 1:
			// do nothing
			break;
		case 2:
			hdata = data;
			data = Host2BigEndian(hdata);
			break;
		case 4:
			data = Host2BigEndian(data);
			break;
		}
		break;
	}
#ifdef SOCLIB_DEBUG
	cerr << "    memory operation ready:" << endl;
	cerr << "    - " << (is_read?"read":"write") << endl;
	cerr << "    - size = " << size << endl;
	cerr << "    - address = 0x" << hex << addr << dec << endl;
	if(!is_read)
		cerr << "    - data = " << hex << data << dec << endl;
#endif
}

template<class CONFIG>
void
CPU<CONFIG> ::
PerformLoadStoreAccesses() {
	external_memory_request = false;
	// while the lsQueue is not empty process entries
	MemoryOp<CONFIG> *memop = lsQueue.front();
	// by default we set a memory request as internal
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
	if(!external_memory_request) {
		lsQueue.pop();
		freeLSQueue.push(memop);
//		if(lsQueue.empty()) {
//			/* instruction is finished */
//			if(executeQueue->IsFlushPipelineRequired()) {
//				/* flush the pipeline and set the new pc to fetch */
//				FlushPipeline();
//				fetch_pc = GetGPR(PC_reg);
//			} else {
//				InstructionFactory<CONFIG>::Destroy(executeQueue);
//			}
//		}
	} else
		memop->SetExternal(true);
}

template<class CONFIG>
void
CPU<CONFIG> ::
PerformPrefetchAccess(MemoryOp<CONFIG> *memop) {
	address_t read_address = memop->GetAddress();
	uint32_t value;
	
	if(CONFIG::MODEL == ARM966E_S) {
		cp15_966es->PrRead(read_address, (uint8_t *)&value, 4);
	}
	if(CONFIG::MODEL == ARM7TDMI) {
		external_memory_request = true;
	}
}

template<class CONFIG>
void
CPU<CONFIG> ::
PerformWriteAccess(MemoryOp<CONFIG> *memop) {
	address_t address = memop->GetAddress();
	uint8_t val8;
	uint16_t val16;
	uint32_t val32;
	
	if(CONFIG::MODEL == ARM7TDMI) {
		external_memory_request = true;
		return;
	}
	switch(memop->GetSize()) {
	case 1:
		address = address ^ munged_address_mask8;

		val8 = (uint8_t)memop->GetWriteValue();
		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es->PrWrite(address, &val8, 1);
		}
		break;
	case 2:
		val16 = (uint16_t)memop->GetWriteValue();
		val16 = Host2BigEndian(val16);

		address = address ^ munged_address_mask16;

		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es->PrWrite(address, (uint8_t *)&val16, 2);
		}
		break;
	case 4:
		val32 = memop->GetWriteValue();
		val32 = Host2BigEndian(val32);

		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es->PrWrite(address, (uint8_t *)&val32, 4);
		}
		break;
	}
}

template<class CONFIG>
void
CPU<CONFIG> ::
PerformReadAccess(MemoryOp<CONFIG> *memop) {
	typename CONFIG::reg_t val32, val32_l, val32_r;
	uint16_t val16; // , val16_l, val16_r;
	uint8_t val8;
	typename CONFIG::reg_t value = 0;
	address_t address = memop->GetAddress();
	uint32_t size = memop->GetSize();
	address_t read_address = address & ~(address_t)(size - 1); 
	
	if(CONFIG::MODEL == ARM7TDMI) {
		external_memory_request = true;
		return;
	}
	
	switch(size) {
	case 1:
		read_address = read_address ^ munged_address_mask8;

		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es->PrRead(read_address, &val8, 1);
			if(external_memory_request) return;
		}
		if(memop->IsSigned()) {
			value = (typename CONFIG::sreg_t)(int8_t)val8;
		} else
			value = val8;
		break;
	case 2:
		/* NOTE: 16bits reads are always aligned */
		read_address = read_address ^ munged_address_mask16;

		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es->PrRead(read_address, (uint8_t *)&val16, 2);
			if(external_memory_request) return;
		}
		
		val16 = BigEndian2Host(val16);
		
		if(memop->IsSigned()) {
			value = (typename CONFIG::sreg_t)(int16_t)val16;
		} else
			value = val16;
		break;
	case 4:
		if(CONFIG::MODEL == ARM966E_S) {
			cp15_966es->PrRead(read_address, (uint8_t *)&val32, 4);
			if(external_memory_request) return;
		}		
		
		val32 = BigEndian2Host(val32);
		
		switch(address & 0x03) {
		case 0x00:
			// nothing to do
			break;
		case 0x01:
			val32_l = (val32 << 8) & ~((typename CONFIG::reg_t)0x0FF);
			val32_r = (val32 >> 24) & ((typename CONFIG::reg_t)0x0FF);
			val32 = val32_l + val32_r;
			break;
		case 0x02:
			val32_l = (val32 << 16) & ~((typename CONFIG::reg_t)0x0FFFF);
			val32_r = (val32 >> 16) & ((typename CONFIG::reg_t)0x0FFFF);
			val32 = val32_l + val32_r;
			break;
		case 0x03:
			val32_l = (val32 << 24) & ~((typename CONFIG::reg_t)0x0FFFFFF);
			val32_r = (val32 >> 8) & ((typename CONFIG::reg_t)0x0FFFFFF);
			val32 = val32_l + val32_r;
			break;
		}
		value = val32;
		break;
	}
	SetGPR(memop->GetTargetReg(), value);
}

template<class CONFIG>
void
CPU<CONFIG> ::
PerformReadToPCUpdateTAccess(MemoryOp<CONFIG> *memop) {
	typename CONFIG::reg_t value;
	typename CONFIG::reg_t value_l, value_r;
	address_t address = memop->GetAddress();
	address_t read_address = address & ~(0x3);

	if(CONFIG::MODEL == ARM7TDMI) {
		external_memory_request = true;
		return;
	}

	if(CONFIG::MODEL == ARM966E_S) {
		cp15_966es->PrRead(read_address, (uint8_t *)&value, 4);
		if(external_memory_request) return;
	}
	
	value = BigEndian2Host(value);
	
	switch(address & 0x03) {
	case 0x00:
		// nothing to do
		break;
	case 0x01:
		value_l = (value << 8) & ~((typename CONFIG::reg_t)0x0FF);
		value_r = (value >> 24) & ((typename CONFIG::reg_t)0x0FF);
		value = value_l + value_r;
		break;
	case 0x02:
		value_l = (value << 16) & ~((typename CONFIG::reg_t)0x0FFFF);
		value_r = (value >> 16) & ((typename CONFIG::reg_t)0x0FFFF);
		value = value_l + value_r;
		break;
	case 0x03:
		value_l = (value << 24) & ~((typename CONFIG::reg_t)0x0FFFFFF);
		value_r = (value >> 8) & ((typename CONFIG::reg_t)0x0FFFFFF);
		value = value_l + value_r;
		break;
	}

	SetGPR(PC_reg, value & (typename CONFIG::reg_t)0xFFFFFFFE);
	if(GetGPR(PC_reg) != executeQueue->GetNextFetchAddress()) executeQueue->SetFlushPipelineRequired(true);
	// code valid for version 5 and above
	if(CONFIG::ARCHITECTURE == ARMV5T ||
		CONFIG::ARCHITECTURE == ARMV5TXM ||
		CONFIG::ARCHITECTURE == ARMV5TE ||
		CONFIG::ARCHITECTURE == ARMV5TEXP) {
		SetCPSR_T((value & 0x01) == 1);
	}
}

template<class CONFIG>
void
CPU<CONFIG> ::
PerformReadToPCAccess(MemoryOp<CONFIG> *memop) {
	typename CONFIG::reg_t value;
	typename CONFIG::reg_t value_l, value_r;
	address_t address = memop->GetAddress();
	address_t read_address = address & ~(0x3);

	if(CONFIG::MODEL == ARM7TDMI) {
		external_memory_request = true;
		return;
	}

	if(CONFIG::MODEL == ARM966E_S) {
		cp15_966es->PrRead(read_address, (uint8_t *)&value, 4);
		if(external_memory_request) return;
	}
	
	value = BigEndian2Host(value);
	
	switch(address & 0x03) {
	case 0x00:
		// nothing to do
		break;
	case 0x01:
		value_l = (value << 8) & ~((typename CONFIG::reg_t)0x0FF);
		value_r = (value >> 24) & ((typename CONFIG::reg_t)0x0FF);
		value = value_l + value_r;
		break;
	case 0x02:
		value_l = (value << 16) & ~((typename CONFIG::reg_t)0x0FFFF);
		value_r = (value >> 16) & ((typename CONFIG::reg_t)0x0FFFF);
		value = value_l + value_r;
		break;
	case 0x03:
		value_l = (value << 24) & ~((typename CONFIG::reg_t)0x0FFFFFF);
		value_r = (value >> 8) & ((typename CONFIG::reg_t)0x0FFFFFF);
		value = value_l + value_r;
		break;
	}

	SetGPR(PC_reg, value & (typename CONFIG::reg_t)0xFFFFFFFE);
	if(GetGPR(PC_reg) != executeQueue->GetNextFetchAddress()) executeQueue->SetFlushPipelineRequired(true);
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetDataResponse(bool error, uint32_t rdata) {
	/* set the data using rdata, to the register indicated by the instruction
	 *   in the execute queue. */
	if(error) {
		cerr << "TODO(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< "Received a data response with an error, request description: " << endl;
		MemoryOp<CONFIG> *memop = lsQueue.front();
		switch(memop->GetType()) {
		case MemoryOp<CONFIG>::WRITE:
			cerr << " + type: write" << endl;
			cerr << " + addr: 0x" << hex << memop->GetAddress() << dec << endl;
			cerr << " + size: " << memop->GetSize() << endl;
			break;
		case MemoryOp<CONFIG>::READ:
			cerr << " + type: read" << endl;
			cerr << " + dest: r" << memop->GetTargetReg() << endl;
			cerr << " + addr: 0x" << hex << memop->GetAddress() << dec << endl;
			cerr << " + size: " << memop->GetSize() << endl;
			break;
		case MemoryOp<CONFIG>::READ_TO_PC_UPDATE_T:
			cerr << " + type: read to pc with thumb update" << endl;
			break;
		case MemoryOp<CONFIG>::READ_TO_PC:
			cerr << " + type: read to pc" << endl;
			break;
		case MemoryOp<CONFIG>::PREFETCH:
			cerr << " + type: prefetch" << endl;
			break;
		}
		return;
//		exit(-1);
	}
	if(lsQueue.empty()) {
		cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< "Received data in response that was not expected" << endl;
		exit(-1);
	}
	int dest = 0;
	bool flushPipeline = false;
	uint16_t rhdata = 0;
	MemoryOp<CONFIG> *memop = lsQueue.front();
	if(!memop->IsExternal()) {
		cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< "Received data in response that was not expected" << endl;
		exit(-1);
	}
	lsQueue.pop();
#ifdef SOCLIB_DEBUG
	uint32_t data = 0;
	uint16_t hdata = 0;
	switch(memop->GetType()) {
	case MemoryOp<CONFIG>::WRITE:
		cerr << " + type: write" << endl;
		cerr << " + addr: 0x" << hex << memop->GetAddress() << dec << endl;
		cerr << " + size: " << memop->GetSize() << endl;
		data = memop->GetWriteValue();
		switch(memop->GetSize()) {
		case 1:
			// do nothing
			break;
		case 2:
			hdata = data;
			data = Host2BigEndian(hdata);
			break;
		case 4:
			data = Host2BigEndian(data);
			break;
		}
		cerr << " + data: 0x" << hex << memop->GetWriteValue() << dec << " (0x" << hex << data << dec << ")" << endl;
		break;
	case MemoryOp<CONFIG>::READ:
		cerr << " + type: read" << endl;
		cerr << " + dest: r" << memop->GetTargetReg() << endl;
		cerr << " + addr: 0x" << hex << memop->GetAddress() << dec << endl;
		cerr << " + size: " << memop->GetSize() << endl;
		data = rdata;
		switch(memop->GetSize()) {
		case 1:
			// do nothing
			break;
		case 2:
			hdata = data;
			data = Host2BigEndian(hdata);
			break;
		case 4:
			data = Host2BigEndian(data);
			break;
		}
		cerr << " + data: 0x" << hex << data << dec << " (0x" << hex << rdata << dec << ")" << endl;
		break;
	case MemoryOp<CONFIG>::READ_TO_PC_UPDATE_T:
		cerr << " + type: read to pc with thumb update" << endl;
		break;
	case MemoryOp<CONFIG>::READ_TO_PC:
		cerr << " + type: read to pc" << endl;
		break;
	case MemoryOp<CONFIG>::PREFETCH:
		cerr << " + type: prefetch" << endl;
		break;
	}
#endif // SOCLIB_DEBUG
	switch(memop->GetType()) {
		case MemoryOp<CONFIG>::WRITE:
			break;
		case MemoryOp<CONFIG>::READ:
			dest = memop->GetTargetReg();
			switch(memop->GetSize()) {
			case 1:
				// do nothing
				break;
			case 2:
				rhdata = rdata;
				rdata = BigEndian2Host(rhdata);
				break;
			case 4:
				rdata = BigEndian2Host(rdata);
				break;
			}
			SetGPR(dest, rdata);
			break;
		case MemoryOp<CONFIG>::READ_TO_PC_UPDATE_T:
			dest = memop->GetTargetReg();
			if(rdata & UINT32_C(1)) {
				if(!GetCPSR_T()) flushPipeline = true;
				SetCPSR_T(true);
			} else {
				if(GetCPSR_T()) flushPipeline = true;
				SetCPSR_T(false);
			}
			switch(memop->GetSize()) {
			case 1:
				// do nothing
				break;
			case 2:
				rhdata = rdata;
				rdata = BigEndian2Host(rhdata);
				break;
			case 4:
				rdata = BigEndian2Host(rdata);
				break;
			}
			rdata = rdata & UINT32_C(0xfffffffe);
			if(rdata != executeQueue->GetNextFetchAddress()) flushPipeline = true;
			SetGPR(PC_reg, rdata);
			break;
		case MemoryOp<CONFIG>::READ_TO_PC:
			dest = memop->GetTargetReg();
			switch(memop->GetSize()) {
			case 1:
				// do nothing
				break;
			case 2:
				rhdata = rdata;
				rdata = BigEndian2Host(rhdata);
				break;
			case 4:
				rdata = BigEndian2Host(rdata);
				break;
			}
			SetGPR(PC_reg, rdata);
			if(rdata != executeQueue->GetNextFetchAddress()) flushPipeline = true;
			break;
		case MemoryOp<CONFIG>::PREFETCH:
			break;
	}
	freeLSQueue.push(memop);
	memop = 0;
	if(flushPipeline) executeQueue->SetFlushPipelineRequired(true);
//	if(lsQueue.empty()) {
//		/* instruction is finished */
//		if(executeQueue->IsFlushPipelineRequired()) {
//			/* flush the pipeline and set the new pc to fetch */
//			FlushPipeline();
//			fetch_pc = GetGPR(PC_reg);
//		} else {
//			InstructionFactory<CONFIG>::Destroy(executeQueue);
//		}
//	}
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetWriteBerr() {
	// TODO ?? Ignore ??
	cerr << "TODO(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
		<< "?? Ignore ??" << endl;
	exit(-1);
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetIrq(uint32_t irq) {
	// Checks the irq range, if out of range, then display error and stop.
	// If irq received then set the correspondent exception flag (exception member variable).
	// List of avalaible interrupts:
	// - irq 1 (IRQ_IRQ) == normal IRQ
	// - irq 2 (FIQ_IRQ) == fast IRQ
	
	if(irq) {
		if(irq == IRQ_IRQ) {
			exception |= IRQ_EXCEPTION;
		} else if(irq == FIQ_IRQ) {
			exception |= FIQ_EXCEPTION;
		} else {
			cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
				<< "Received unknow interruption type (irq = 0x" << hex << irq << dec << ")" << endl;
			exit(-1);
		}
	}
	return;
}
		
template<class CONFIG>
unsigned int 
CPU<CONFIG> ::
GetDebugRegisterCount() const {
	return 16 /* gpr */ + 1 /* cpsr */;
}
 
template<class CONFIG>
uint32_t 
CPU<CONFIG> ::
GetDebugRegisterValue(unsigned int reg) const {
	// TODO
	if(reg < 16)
		return GetGPR(reg);
	if(reg == 16)
		return GetCPSR();
	return 0;
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetDebugRegisterValue(unsigned int reg, uint32_t value) {
	if(reg < 16) {
		SetGPR(reg, value);
		return;
	}
	if(reg == 16) {
		SetCPSR(value);
		return;
	}
}

template<class CONFIG>
size_t 
CPU<CONFIG> ::
GetDebugRegisterSize(unsigned int reg) const {
	// TODO
	return 0;
}
	
template<class CONFIG>
uint32_t 
CPU<CONFIG> ::
GetDebugPC() const {
	// TODO
	return 0;
}
 
template<class CONFIG>
void 
CPU<CONFIG> ::
SetDebugPC(uint32_t new_pc) {
	// TODO
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetICacheInfo( size_t line_size, size_t assoc, size_t n_lines ) {
	// TODO
}
 
template<class CONFIG>
void 
CPU<CONFIG> ::
SetDCacheInfo( size_t line_size, size_t assoc, size_t n_lines ) {
	// TODO
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // SOCLIB

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_SOCLIB_STEP_CYCLE_TCC__
