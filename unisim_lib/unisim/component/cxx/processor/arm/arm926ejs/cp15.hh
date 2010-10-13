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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__


#include <inttypes.h>
#include "unisim/component/cxx/processor/arm/arm926ejs/cp15interface.hh"
namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

class CP15
{
public:
	/* TODO check if we really need the pointer to the cpu */
	/** Constructor
	 *
	 * @param _cpu the cpu the coprocessor is attached to
	 */
	//CP15(unisim::component::cxx::processor::arm::arm926ejs::CPU* _cpu);
	CP15(CP15Interface *_cpu);
	/** Destructor */
	~CP15();

	/** Read the value of a register
	 *
	 * @param opcode1 the "opcode1" field of the instruction code 
	 * @param opcode2 the "opcode2" field of the instruction code
	 * @param crn     the "crn" field of the instruction code
	 * @param crm     the "crm" field of the instruction code
	 * @param reg     the read value
	 */
	void ReadRegister(uint8_t opcode1, 
			uint8_t opcode2, 
			uint8_t crn, 
			uint8_t crm, 
			uint32_t& reg);

	/** Write a value in a register
	 * 
	 * @param opcode1 the "opcode1" field of the instruction code
	 * @param opcode2 the "opcode2" field of the instruction code
	 * @param crn     the "crn" field of the instruction code
	 * @param crm     the "crm" field of the instruction code
	 * @param val     value to be written in the register
	 */
	void WriteRegister(uint8_t opcode1,
			uint8_t opcode2, 
			uint8_t crn, 
			uint8_t crm, 
			uint32_t value);

	/** Perform a coprocessor operation
	 *
	 * @param opcode1 the "opcode1" field of the instruction code
	 * @param opcode2 the "opcode2" field of the instruction code
	 * @param crd     the "crd" field of the instruction code
	 * @param crn     the "crn" field of the instruction code
	 * @param crm     the "crm" field of the instruction code
	 */
	void Operation(uint8_t opcode1,
			uint8_t opcode2,
			uint8_t crd,
			uint8_t crn,
			uint8_t crm);

	/** Perform a coprocessor load
	 *
	 * @param crd     the "crd" field of the instruction code
	 * @param address the address to load data from
	 */
	void Load(uint8_t crd,
			uint32_t address);

	/** Perform a coprocessor load
	 *
	 * @param crd     the "crd" field of the instruction code
	 * @param address the address to store the data to
	 */
	void Store(uint8_t crd,
			uint32_t address);

private:
	CP15Interface *cpu;

	uint32_t control_register;
};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__
