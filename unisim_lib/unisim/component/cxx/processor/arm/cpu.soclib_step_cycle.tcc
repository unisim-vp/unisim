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
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

template<class CONFIG>
void 
CPU<CONFIG> ::
Reset() {
	cerr << "+++ Reset" << endl;
	FlushPipeline();
	fetch_pc = GetGPR(PC_reg);
	cerr << "    current fetch_pc = 0x" << hex << fetch_pc << dec << endl;
	fetchQueue = InstructionFactory<CONFIG>::New();
	fetchQueue->SetFetchAddress(fetch_pc);
	fetch_pc += InstructionByteSize();
	fetchQueue->SetPredictedNextFetchAddress(fetch_pc);
	cerr << "    next fetch_pc = 0x" << hex << fetch_pc << dec << endl;
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
	cerr << "+++ IsBusy" << endl;
	if(fetchQueue) {
		cerr << "    something in the fetchQueue" << endl;
		if(fetchQueue->IsRequested() && !fetchQueue->IsFetched()) {
			cerr << "    waiting for fetched instruction (1)" << endl;
			return 1;
		}
	}
	cerr << "    not busy (0)" << endl;
	return 0;
	bool busy = false;

	cerr << "+++ IsBusy" << endl;

	busy = executeQueue != 0 &&
		executeQueue->GetRemainingExecCycles() == 0 &&
		lsQueue.empty();
	cerr << "    - busy = " << busy << endl;
	return busy?1:0;
}

template<class CONFIG>
void
CPU<CONFIG> ::
FlushPipeline() {
	cerr << "+++ FlushPipeline" << endl;
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
		InstructionFactory<CONFIG>::Destroy(fetchQueue);
		fetchQueue = 0;
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
		cerr << "    - executing 0x" << hex << executeQueue->GetFetchAddress() << dec << endl;
		if(executeQueue->IsArm32()) {
			typename isa::arm32::Operation<CONFIG> *op = 
				arm32_decoder.Decode(
					executeQueue->GetFetchAddress(),
					executeQueue->GetArm32Encoding());
			op->execute(*this);
			op->disasm(*this, str);
			cerr << "    - arm32 instruction: " << str.str() << endl;
			executeQueue->SetOpcode(op);
		} else {
			typename isa::thumb::Operation<CONFIG> *op = 
				thumb_decoder.Decode(
					executeQueue->GetFetchAddress(),
					executeQueue->GetThumbEncoding());
			op->execute(*this);
			op->disasm(*this, str);
			cerr << "    - thumb instruction: " << str.str() << endl;
			executeQueue->SetOpcode(op);
		}
		executeQueue->SetExecuted();
		/* set the instruction latency 
		 * TODO: get the instruction latency from the opcode */
		executeQueue->SetExecCycles(1);
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
	cerr << "+++ Step" << endl;
	/* start by removing the instruction in the executeQueue (if any) if it
	 *   has finished its operation
	 * check if the pipeline needs to be flushed */
	cerr << "    Execute" << endl;
	if(executeQueue != 0 &&
			executeQueue->GetRemainingExecCycles() == 0 &&
			lsQueue.empty()) {
		if(GetGPR(PC_reg) != executeQueue->GetPredictedNextFetchAddress()) {
			cerr << "    - misspredicted pc 0x" << hex << executeQueue->GetPredictedNextFetchAddress() << dec << " -> 0x" << hex << GetGPR(PC_reg) << dec << endl;
			FlushPipeline();
			cerr << "    - flushing pipeline" << endl;
			fetch_pc = GetGPR(PC_reg);
			cerr << "    - setting pc to 0x" << hex << fetch_pc << dec << endl;
		}
		InstructionFactory<CONFIG>::Destroy(executeQueue);
		executeQueue = 0;
	}

	/* if execute queue is empty then move the decode queue entry to execute
	 *   queue and execute the instruction. if the instruction is finished then
	 *   remove it */
	/* if the execute queue is not empty then perform the necessary actions
	 *   to finish the instruction, i.e.: send a load, store, etc...*/
	if(executeQueue == 0) {
		if(decodeQueue != 0) {
			cerr << "    - moving from decodeQueue 0x" << hex 
				<< decodeQueue->GetFetchAddress() << dec << endl;
			executeQueue = decodeQueue;
			decodeQueue = 0;
			StepExecute();
		}
	}

		
	/* if the decode queue is not empty, then do nothing */
	/* if the decode queue is empty, then move the instruction present in
	 *   the fetch queue to the decode queue */
	cerr << "    Decode" << endl;
	if(decodeQueue == 0) {
		if(fetchQueue != 0) {
			if(fetchQueue->IsFetched()) {
				cerr << "    - moving from fetchQueue 0x" << hex
					<< fetchQueue->GetFetchAddress() << dec << endl;
				decodeQueue = fetchQueue;
				fetchQueue = 0;
			}
		}
	}

	/* if the fetch queue is not empty, the do nothing */
	/* if the fetch queue is empty, then send a read request to fetch fetch_pc
	 *   instruction, after that increase fetch_pc */
	cerr << "    Fetch" << endl;
	if(fetchQueue == 0) {
		fetchQueue = InstructionFactory<CONFIG>::New();
		fetchQueue->SetFetchAddress(fetch_pc);
		cerr << "    - current fetch_pc = 0x" << hex << fetch_pc << dec << endl;
		fetch_pc += InstructionByteSize();
		fetchQueue->SetPredictedNextFetchAddress(fetch_pc);
		cerr << "    - next fetch_pc = 0x" << hex << fetch_pc << dec << endl;
	}
}

template<class CONFIG>
void 
CPU<CONFIG> ::
NullStep(uint32_t time_passed) {
	cerr << "+++ NullStep(" << time_passed << ")" << endl;
	// TODO: check
	for(uint32_t i = 0; i < time_passed; i++)
		Step(); // ???? is this correct ???
}

template<class CONFIG>
void 
CPU<CONFIG> ::
GetInstructionRequest(bool &req, uint32_t &addr) const {
	cerr << "+++ GetInstructionRequest" << endl;
	/* check if the instruction on the fetch queue has been requested,
	 *   if not then return the address to fetch */
	if(fetchQueue != 0) {
		if(fetchQueue->IsRequested() == false) {
			addr = fetchQueue->GetFetchAddress();
			addr = addr & UINT32_C(0xfffffffc);
			req = true;
			fetchQueue->SetRequested(true);
		}
	} else {
		cerr << "    4" << endl;
		req = false;
	}
	cerr << "    req = " << req;
	if(req) cerr << ", addr = 0x" << hex << addr << dec;
	cerr << endl;
	return;
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetInstruction(bool error, uint32_t val) {
	cerr << "+++ SetInstruction (error = " << error << ", val = 0x" << hex << val << dec << ")" << endl;
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
		typename isa::thumb::Operation<CONFIG> *op = NULL;
		uint16_t val16;
		if(addr & UINT32_C(0X03)) {
			// not aligned read the higher bits
			val = val >> 16;
		}
		val16 = val & UINT32_C(0x0000ffff);
		thumb_insn_t insn = val16;
		insn = BigEndian2Host(insn);
		fetchQueue->SetThumbEncoding(insn);
		op = thumb_decoder.Decode(addr, insn);
		fetchQueue->SetOpcode(op);
	} else {
		// it is an arm32 instruction
		typename isa::arm32::Operation<CONFIG> *op = NULL;
		insn_t insn = val;
		insn = BigEndian2Host(insn);
		fetchQueue->SetArm32Encoding(insn);
		op = arm32_decoder.Decode(addr, insn);
		fetchQueue->SetOpcode(op);
	}
}

template<class CONFIG>
void 
CPU<CONFIG> ::
GetDataRequest(bool &reg, bool &is_read, int &size, uint32_t &addr,
		uint32_t &data) const {
	cerr << "+++ GetDataRequest" << endl;
	MemoryOp<CONFIG> *memop = 0;
	/* check if the instruction in the execute queue has any load/store to
	 *   be done, if so, execute it. */
	if(lsQueue.empty()) {
		reg = false;
		cerr << "    no memory operation ready" << endl;
		return;
	}
//	if(firstLS != 0) {
//		reg = false;
//		return; // nothing to make, the firstLS was already sent
//	} else {
		reg = true;
		memop = lsQueue.front();
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
				break;
		}
		cerr << "    memory operation ready:" << endl;
		cerr << "    - " << (is_read?"read":"write") << endl;
		cerr << "    - size = " << size << endl;
		cerr << "    - address = 0x" << hex << addr << dec << endl;
		if(!is_read)
			cerr << "    - data = " << hex << data << dec << endl;
		return;
		
//	}
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetDataResponse(bool error, uint32_t rdata) {
	/* set the data using rdata, to the register indicated by the instruction
	 *   in the execute queue. */
	if(error) {
		cerr << "TODO(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< "Received a data response with an error" << endl;
		exit(-1);
	}
	if(lsQueue.empty()) {
		cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< "Received data in response that was not expected" << endl;
		exit(-1);
	}
	int dest = 0;
	bool flushPipeline = false;
	MemoryOp<CONFIG> *memop = lsQueue.front();
	lsQueue.pop();
	switch(memop->GetType()) {
		case MemoryOp<CONFIG>::WRITE:
			break;
		case MemoryOp<CONFIG>::READ:
			dest = memop->GetTargetReg();
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
			rdata = rdata & UINT32_C(0xfffffffe);
			if(rdata != executeQueue->GetNextFetchAddress()) flushPipeline = true;
			SetGPR(PC_reg, rdata);
			break;
		case MemoryOp<CONFIG>::READ_TO_PC:
			dest = memop->GetTargetReg();
			SetGPR(PC_reg, rdata);
			if(rdata != executeQueue->GetNextFetchAddress()) flushPipeline = true;
			break;
		case MemoryOp<CONFIG>::PREFETCH:
			break;
	}
	freeLSQueue.push(memop);
	memop = 0;
	if(flushPipeline) executeQueue->SetFlushPipelineRequired(true);
	if(lsQueue.empty()) {
		/* instruction is finished */
		if(executeQueue->IsFlushPipelineRequired()) {
			/* flush the pipeline and set the new pc to fetch */
			FlushPipeline();
			fetch_pc = GetGPR(PC_reg);
		} else {
			InstructionFactory<CONFIG>::Destroy(executeQueue);
		}
	}
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
	// TODO ?? Set the corresponding irq
	cerr << "TODO(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
		<< "?? Set the corresponding irq" << endl;
	exit(-1);
}
		
template<class CONFIG>
unsigned int 
CPU<CONFIG> ::
GetDebugRegisterCount() const {
	// TODO
	return 0;
}
 
template<class CONFIG>
uint32_t 
CPU<CONFIG> ::
GetDebugRegisterValue(unsigned int reg) const {
	// TODO
	return 0;
}

template<class CONFIG>
void 
CPU<CONFIG> ::
SetDebugRegisterValue(unsigned int reg, uint32_t value) {
	// TODO
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
