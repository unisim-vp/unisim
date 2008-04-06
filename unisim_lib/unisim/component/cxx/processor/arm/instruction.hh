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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_INSTRUCTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_INSTRUCTION_HH__

#include <inttypes.h>
#include <queue>
#include "unisim/component/cxx/processor/arm/isa_arm32.hh"
#include "unisim/component/cxx/processor/arm/isa_thumb.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

template<class CONFIG>
class Instruction {
private:
	typedef typename CONFIG::address_t address_t;
	typedef typename isa::thumb::Operation<CONFIG> thumb_op_t;
	typedef typename isa::arm32::Operation<CONFIG> arm32_op_t;
	typedef typename CONFIG::insn_t insn_t;
	typedef typename CONFIG::thumb_insn_t thumb_insn_t;

public:
	Instruction();
	~Instruction();

	/** reinitializes/cleans the instruction
	 * Has the same effect than creating a new instruction in the current
	 *   instruction.
	 */
	void Reset();
	
	/** sets the instruction fetch address
	 * Sets the instruction fetch address.
	 * 
	 * @param address the instruction fetch address
	 */
	void SetFetchAddress(address_t address);
	/** returns the instruction fetch address
	 * Returns the instruction fetch address.
	 * 
	 * @return the instruction fetch address
	 */
	address_t GetFetchAddress() const;
	/** returns true if the instruction has been requested to the memory
	 * Returns true if the instruction has been requested to the memory system
	 *
	 * @return true if requested, false otherwise
	 */
	bool IsRequested() const;
	/** sets the requested information of the instruction
	 * Sets the requested information of the instruction
	 *
	 * @param req true if it has been requested, false otherwise
	 */
	void SetRequested(bool req = true);
	/** returns true if the instruction has been effectively fetched
	 * Returns true if the instruction has been effectively fetched from the
	 *   memory system.
	 * 
	 * @return true if fetched, false otherwise
	 */
	bool IsFetched() const;
	/** sets the next instruction to be executed fetch address
	 * Sets the next instruction to be executed fetch address.
	 * 
	 * @param address the next instruction fetch address
	 */
	void SetNextFetchAddress(address_t address);
	/** gets the next instruction to be executed fetch address
	 * Gets the next instruction to be executed fetch address.
	 * 
	 * @return the next instruction to be executed fetch address
	 */
	address_t GetNextFetchAddress() const;
	/** sets the predicted next instruction to be executed fetch address
	 * Sets the predicted next instruction to be executed fetch address.
	 * 
	 * @param address the predicted next instruction fetch address
	 */
	void SetPredictedNextFetchAddress(address_t address);
	/** gets the predicted next instruction to be executed fetch address
	 * Gets the predicted next instruction to be executed fetch address.
	 * 
	 * @return the predicted next instruction to be executed fetch address
	 */
	address_t GetPredictedNextFetchAddress();
	
	/** sets the arm32 instruction encoding
	 * Sets the arm32 instruction encoding.
	 * 
	 * @param encoding the instruction encoding
	 */
	void SetArm32Encoding(insn_t encoding);
	/** gets the arm32 instruction encoding
	 * Gets the arm32 instruction encoding.
	 * 
	 * @return the instruction encoding
	 */ 
	insn_t GetArm32Encoding();
	/** set the thumb instruction encoding
	 * Sets the thumb instruction encoding.
	 * 
	 * @param encoding the instruction encoding
	 */
	void SetThumbEncoding(thumb_insn_t encoding);
	/** gets the thumb instruction encoding
	 * Gets the thumb instruction encoding.
	 * 
	 * @return the instruction encoding
	 */
	thumb_insn_t GetThumbEncoding();
	
	/** sets the thumb instruction opcode
	 * Sets the instruction opcode in thumb mode.
	 * 
	 * @param op the thumb instruction opcode
	 */
	void SetOpcode(thumb_op_t *op);
	/** sets the arm32 instruction opcode
	 * Sets the instruction opcode in arm32 mode.
	 * 
	 * @param op the thumb instruction opcode
	 */
	void SetOpcode(arm32_op_t *op);
	/** gets the thumb instruction opcode
	 * Gets the instruction opcode in thumb mode.
	 *
	 * @return op pointer to the thumb instruction that we will return
	 */
	thumb_op_t *GetThumbOpcode() const;
	/** gets the arm32 instruction opcode
	 * Gets the instruction opcode in arm32 mode.
	 *
	 * @return op pointer to the arm32 instruction that we will return
	 */
	arm32_op_t *GetArm32Opcode() const;
	
	/** returns true if the instruction is a thumb instruction
	 * Returns true if the instruction is a thumb instruction. This method only
	 *   works if the instruction has been fetched.
	 * 
	 * @return true if the instruction is a thumb instruction, false otherwise
	 */
	bool IsThumb();
	/** returns true if the instruction is an arm32 instruction
	 * Returns true if the instruction is an arm32 instruction. This method only
	 *   works if the instruction has been fetched.
	 * 
	 * @return true if the instruction is an arm32 instruction, false otherwise
	 */
	bool IsArm32();
	
	/** sets the instruction executed state
	 * Sets the instruction executed state .
	 *
	 * @param is_executed informs if the instruction has been executed of not
	 */
	void SetExecuted(bool is_executed = true);
	/** informs if the instruction has been executed or not.
	 * Informs if the instruction has been executed or not.
	 *
	 * @return true if has been executed, false otherwise
	 */
	bool HasBeenExecuted() const;
	/** sets the instruction execution cycles
	 * Sets the instruction execution cycles.
	 * 
	 * @param num_cycles the instruction execution cycles
	 */
	void SetExecCycles(uint32_t num_cycles);
	/** gets the instruction remaining execution cycles
	 * Returns the instruction remaining execution cycles.
	 * 
	 * @return the instruction remaining execution cycles
	 */
	uint32_t GetRemainingExecCycles() const;
	/** decrements by one the instruction remaining execution cycles
	 * Decrements by one the instruciton remaining execution cycles and returns
	 *   the number of cycles remaining;
	 * 
	 * @return the instruction remaining execution cycles after decrementing
	 */
	uint32_t DecExecCycles();

	/** true if a branch was taken
	 * Returns true if the executed instruction has taken a branch, false
	 *   otherwise.
	 *
	 * @return true if a branch was taken, false otherwise
	 */
	bool BranchTaken() const;

	/** true if a flush is required
	 * Returns true if the executed instruction has to do a pipeline flush
	 *
	 * @return true if a flush is required, false otherwise
	 */
	bool IsFlushPipelineRequired() const;

	/** marks the instruction to flush the pipeline
	 * Marks the instruction to flush the pipeline
	 *
	 * @param flush true if the pipeline needs to be flushed, false otherwise
	 */
	void SetFlushPipelineRequired(bool flush = true);
	
	int NumMemAccesses();
private:
	/** the instruction fetch address */
	address_t fetch_address;
	/** the next instruction to be executed fetch address */
	address_t next_fetch_address;
	/** the predicted next instruction to be executed fetch address */
	address_t predicted_next_fetch_address;
	/** indicates if the instruction has been requested to memory */
	bool is_requested;
	/** indicates if the instruction has been effectively fetched */
	bool is_fetched;
	/** the instruction opcode (thumb) */
	thumb_op_t *thumb_op;
	/** the instruction opcode (arm32) */
	arm32_op_t *arm32_op;
	/** indicates if the instruction has been executed */
	bool executed;
	/** the instruction remaining execution cycles */
	uint32_t exec_cycles;
	/** indicates if the instruction requires to perform a flush of the pipeline */
	bool flush_pipeline_required;
};

using std::queue;

template<class CONFIG>
class InstructionFactory {
public:
	static Instruction<CONFIG> *New();
	static void Destroy(Instruction<CONFIG> *insn);
	
private:
	static queue<Instruction<CONFIG> *> queue;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_INSTRUCTION_HH__
