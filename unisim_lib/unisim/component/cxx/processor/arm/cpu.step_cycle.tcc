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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_CYCLE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_CYCLE_TCC__

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
StepCycle() {
	/* arm processors have three different pipeline stages:
	 * - fetch
	 * - decode
	 * - execute
	 * As it is the case with most of the processor simulators this
	 * implementation executes those steps backwards.
	 * */

	/* check the execution queue.
	 * When an instruction is in the execution queue the instruction is
	 * executed during cycle 1, the following cycles must be accounted not
	 * to remove the instruction from the pipeline unless the total number
	 * of execution cycles is finished.
	 * Once an instruction is finished (all the execution cycles have been
	 * executed) then remove the instruction from the pipeline (that is from
	 * the execution queue).
	 */

	/* check the decoding queue.
	 * Nothings needs to be done here, just move the instruction to the
	 * execution queue if there are no instructions in the execution queue and
	 * one instruction is ready in the decoding queue.
	 */

	/* fill the fetch queue with the address of the instruction to fetch.
	 * Just put the address to fetch in the fetch queue. Another mechanism
	 * will be responsible to physically fetch the instruction from the 
	 * cache and put it in the fetch queue.
	 */
	
	StepCycleExecute();
	StepCycleDecode();
	StepCycleFetch();
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleFetch() {
	/* check if a previous instruction is not still in the fetch queue */
	if(fetchQueue != 0) return;
	
	/* create a new instruction using current counter */
	Instruction<CONFIG> *insn = InstructionFactory<CONFIG>::New();
	/* set the fetch address of the current instruction */
	insn->SetFetchAddress(fetch_pc);
	if(GetCPSR_T()) {
		// thumb mode
		fetch_pc += 2;
	} else {
		// arm32 mode
		fetch_pc += 4;
	}
	/* set the fetch address of the next instruction (predicted) */
	insn->SetNextFetchAddress(fetch_pc);
	/* set the predicted fetch address of the next instruction (it is the same
	 * than the fetch address we have used before) */
	insn->SetPredictedNextFetchAddress(fetch_pc);
	/* put the instruction in the fetch queue */
	fetchQueue = insn;
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleDecode() {
	/* check if we have an instruction in the decoding queue, if so do
	 * nothing */
	if(decodeQueue != 0) return;
	
	/* check if the instruction in the fetch queue is ready (fetched) put
	 * it in the decode queue */
	if(fetchQueue == 0) return;
	decodeQueue = fetchQueue;
	fetchQueue = 0;
	/* decode the instruction */
	if(GetCPSR_T()) {
		typename isa::thumb::Operation<CONFIG> *op = NULL;
		op = thumb_decoder.Decode(decodeQueue->GetFetchAddress(), 
				decodeQueue->GetThumbEncoding());
		decodeQueue->SetOpcode(op);
	} else {
		typename isa::arm32::Operation<CONFIG> *op = NULL;
		op = arm32_decoder.Decode(decodeQueue->GetFetchAddress(), 
				decodeQueue->GetArm32Encoding());
		decodeQueue->SetOpcode(op);
	}
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleExecute() {
	/* check if there is an instruction in the execute queue that needs to be
	 * finished */
	if(executeQueue != 0) {
		uint32_t cycles = executeQueue->GetRemainingExecCycles();
		if(cycles != 0) cycles = executeQueue->DecExecCycles();
		if(cycles == 0) {
			if(executeQueue->NumMemAccesses() == 0) {
				/* the instruction is completelly finished, it can be removed
				 * from the pipeline */
				StepCycleExecuteFinishInstruction();
			}
		}
		return;
	}


	/* the executeQueue is empty and a new instruction can be executed, get it
	 * from the decodeQueue. If there is no instruction in the decode queue
	 * then nothing can be done, the execute step is finished, return */
	if(decodeQueue == 0) return;
	executeQueue = decodeQueue;
	decodeQueue = 0;

	/* the instruction can be executed, we chose to do it in the cpu class, but
	 * it could have been done in the instruction class */
	StepCycleExecuteInstruction();
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleExecuteInstruction() {
	if(executeQueue->IsThumb())
		StepCycleExecuteThumbInstruction();
	else
		StepCycleExecuteArm32Instruction();
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleExecuteThumbInstruction() {
	
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleExecuteArm32Instruction() {
	
}

template<class CONFIG>
void
CPU<CONFIG> ::
StepCycleExecuteFinishInstruction() {
	/* check if a branch was taken, if so, clean the pipeline and set the fetch
	 * address */
	if(executeQueue->BranchTaken()) {
		InstructionFactory<CONFIG>::Destroy(fetchQueue);
		InstructionFactory<CONFIG>::Destroy(decodeQueue);
		fetchQueue = 0;
		decodeQueue = 0;
		fetch_pc = executeQueue->GetNextFetchAddress();
	}
	/* the instruction can be removed from the queue */
	InstructionFactory<CONFIG>::Destroy(executeQueue);
	executeQueue = 0;
}

template<class CONFIG>
uint32_t
CPU<CONFIG> ::
StepCycle(uint32_t max_num_cycles) {
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#undef INLINE

#endif // SOCLIB

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_STEP_CYCLE_TCC__
