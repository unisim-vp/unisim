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
 
#include "unisim/component/cxx/processor/arm/memory_op.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

MemoryOp::
MemoryOp() 
{}

MemoryOp::
~MemoryOp() 
{}

void 
MemoryOp::
SetReadToPCUpdateT(uint32_t address) 
{
	type = READ_TO_PC_UPDATE_T;
	this->address = address;
	this->size = 4;
	target_reg = 15; // the pc register
	this->read_signed = false;
	this->aligned = true;
}

void 
MemoryOp::
SetReadToPC(uint32_t address) 
{
	type = READ_TO_PC;
	this->address = address;
	this->size = 4;
	target_reg = 15; // the pc register
	this->read_signed = false;
	this->aligned = true;
}

void 
MemoryOp::
SetRead(uint32_t address, uint32_t size, uint32_t dest, 
		bool aligned, bool read_signed) 
{
	type = READ;
	this->address = address;
	this->size = size;
	target_reg = dest;
	this->read_signed = read_signed;
	this->aligned = aligned;
}

void 
MemoryOp::
SetUserRead(uint32_t address, uint32_t size, uint32_t dest, 
		bool aligned, bool read_signed) 
{
	type = USER_READ;
	this->address = address;
	this->size = size;
	target_reg = dest;
	this->read_signed = read_signed;
	this->aligned = aligned;
}

void 
MemoryOp::
SetWrite(uint32_t address, uint32_t size, uint32_t value) 
{
	type = WRITE;
	this->address = address;
	this->size = size;
	write_value = value;
}
	
void 
MemoryOp::
SetPrefetch(uint32_t address)
{
	type = PREFETCH;
	this->address = address;
}
	
MemoryOp::type_t
MemoryOp::
GetType() const 
{
	return type;
}

uint32_t 
MemoryOp::
GetAddress() const 
{
	return address;
}

uint32_t 
MemoryOp::
GetSize() const 
{
	return size;
}

uint32_t 
MemoryOp::
GetTargetReg() const 
{
	return target_reg;
}

uint32_t 
MemoryOp::
GetWriteValue() const 
{
	return write_value;
}

bool 
MemoryOp::
NeedAlignment() const 
{
	return !aligned;
}

bool 
MemoryOp::
IsSigned() const 
{
	return read_signed;
}
	
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
