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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_INSTRUCTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_INSTRUCTION_TCC__

#include <inttypes.h>
#include "unisim/component/cxx/processor/arm/instruction.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

template<class CONFIG>
Instruction<CONFIG> ::
Instruction() :
	fetch_address(0),
	is_requested(false),
	is_fetched(false),
	thumb_op(0),
	arm32_op(0),
	executed(false),
	flush_pipeline_required(false),
	is_flushed(false) {
}

template<class CONFIG>
Instruction<CONFIG> ::
~Instruction() {
	Reset();
}

template<class CONFIG>
void 
Instruction<CONFIG> ::
Reset() {
	fetch_address = 0;
	is_requested = false;
	is_fetched = false;
	thumb_op = 0;
	arm32_op = 0;
	executed = false;
	is_flushed = false;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetFetchAddress(typename CONFIG::address_t address) {
	fetch_address = address;
	is_fetched = false;
	is_requested = false;
}

template<class CONFIG>
typename CONFIG::address_t
Instruction<CONFIG> ::
GetFetchAddress() const {
	return fetch_address;
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
IsRequested() const {
	return is_requested;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetRequested(bool req) {
	is_requested = req;
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
IsFetched() const {
	return is_fetched;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetNextFetchAddress(typename CONFIG::address_t address) {
	next_fetch_address = address;
}

template<class CONFIG>
typename CONFIG::address_t
Instruction<CONFIG> ::
GetNextFetchAddress() const {
	return next_fetch_address;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetPredictedNextFetchAddress(typename CONFIG::address_t address) {
	predicted_next_fetch_address = address;
}

template<class CONFIG>
typename CONFIG::address_t
Instruction<CONFIG> ::
GetPredictedNextFetchAddress() {
	return predicted_next_fetch_address;
}

template<class CONFIG>
void 
Instruction<CONFIG> :: 
SetArm32Encoding(typename CONFIG::insn_t encoding) {
	this->arm32_encoding = encoding;
}

template<class CONFIG>
typename CONFIG::insn_t 
Instruction<CONFIG> ::
GetArm32Encoding() {
	return arm32_encoding;
}

template<class CONFIG>
void 
Instruction<CONFIG> ::
SetThumbEncoding(typename CONFIG::thumb_insn_t encoding) {
	this->thumb_encoding = encoding;
}

template<class CONFIG>
typename CONFIG::thumb_insn_t 
Instruction<CONFIG> :: 
GetThumbEncoding() {
	return thumb_encoding;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetOpcode(typename isa::thumb::Operation<CONFIG> *op) {
	thumb_op = op;
//	is_fetched = true;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetOpcode(typename isa::arm32::Operation<CONFIG> *op) {
	arm32_op = op;
//	is_fetched = true;
}

template<class CONFIG>
isa::thumb::Operation<CONFIG> *
Instruction<CONFIG> ::
GetThumbOpcode() const {
	return thumb_op;
}

template<class CONFIG>
isa::arm32::Operation<CONFIG> * 
Instruction<CONFIG> ::
GetArm32Opcode() const {
	return arm32_op;
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
IsThumb() {
	return (thumb_op != 0);
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
IsArm32() {
	return (arm32_op != 0);
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetExecuted(bool is_executed) {
	executed = is_executed;
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
HasBeenExecuted() const {
	return executed;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetExecCycles(uint32_t num_cycles) {
	exec_cycles = num_cycles;
}

template<class CONFIG>
uint32_t
Instruction<CONFIG> ::
GetRemainingExecCycles() const {
	return exec_cycles;
}

template<class CONFIG>
uint32_t
Instruction<CONFIG> ::
DecExecCycles() {
	exec_cycles--;
	
	return exec_cycles;
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
BranchTaken() const {
	if(next_fetch_address != predicted_next_fetch_address)
		return true;
	return false;
}

template<class CONFIG>
int
Instruction<CONFIG> ::
NumMemAccesses() {
	return 0;
}

template<class CONFIG>
bool
Instruction<CONFIG> ::
IsFlushPipelineRequired() const {
	return flush_pipeline_required;
}

template<class CONFIG>
void
Instruction<CONFIG> ::
SetFlushPipelineRequired(bool flush) {
	flush_pipeline_required = true;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_INSTRUCTION_TCC__
