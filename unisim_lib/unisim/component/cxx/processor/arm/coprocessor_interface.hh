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
 * Authors: Adriana Carloganu (adriana.carloganu@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
/****************************************************************
    ARM Coprocessor Interface definition file
*****************************************************************/

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CP_INTERFACE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CP_INTERFACE_HH__

#include <inttypes.h>
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::endian::endian_type;

// opcode1 = should be 0b000 for mcr/mrc valid instructions
// opcode2 = additional opcode; if not needed, 0b0000 must be specified
// crn     = primary register involved in instruction
// crm     = additional register; if not needed,  0 must be specified


/// Arm Coprocessor generic interface.
/** This class describes the communication interface between Arm CPU and a Coprocessor
 */

template <class CPU>
class CPInterface {

public:
	/** Constructor
	 *
	 * @param the cpu to which this coprocessor is attached with
	 */
	CPInterface(CPU *_cpu) :
		cpu(_cpu) {};
	
	/** Destructor. */
	virtual ~CPInterface() 
	{ 
		cpu = 0;
	};

	/** Set coprocessor to reset state. */
    virtual void Reset() = 0;

    /** Read the value of a register
	 *
     * @param[in]  opcode1 : the "opcode1" field of the instruction code (unsigned 8 bits integer)
     * @param[in]  opcode2 : the "opcode2" field of the instruction code ( unsigned 8 bits integer)
     * @param[in]  crn     : the "crn" field of the instruction code (unsigned 8 bits integer)
     * @param[in]  crm     : the "crm" field of the instruction code (unsigned 8 bits integer)
     * @param[out] r       : pointer to a REGISTER variable that will receive the register value
     */
    virtual void ReadRegister(uint8_t opcode1, 
    		uint8_t opcode2, 
    		uint8_t crn, 
    		uint8_t crm, 
    		uint32_t& reg) = 0;

    /** Write a value in a register
	 *
     * @param[in] opcode1 : the "opcode1" field of the instruction code (unsigned 8 bits integer)
     * @param[in] opcode2 : the "opcode2" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crn     : the "crn" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crm     : the "crm" field of the instruction code (unsigned 8 bits integer)
     * @param[in] val     : value to be written in the register (REGISTER type)
     */
    virtual void  WriteRegister(uint8_t opcode1,
    		uint8_t opcode2, 
    		uint8_t crn, 
    		uint8_t crm, 
    		uint32_t value) = 0;

    /** Perform a coprocessor operation
	 *
     * @param[in] opcode1 : the "opcode1" field of the instruction code (unsigned 8 bits integer)
     * @param[in] opcode2 : the "opcode2" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crd     : the "crd" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crn     : the "crn" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crm     : the "crm" field of the instruction code (unsigned 8 bits integer)
     */
    virtual void Operation(uint8_t opcode1,
    		uint8_t opcode2,
    		uint8_t crd,
    		uint8_t crn,
    		uint8_t crm) = 0;

    /** Perform a coprocessor load
	 *
     * @param[in] crd     : the "crd" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] address : the address to load data from
     */
    virtual void Load(uint8_t crd,
    		uint32_t address) = 0;

    /** Perform a coprocessor load
	 *
     * @param[in] crd     : the "crd" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] address : the address to store the data to
     */
    virtual void Store(uint8_t crd,
    		uint32_t address) = 0;

protected:
	CPU *cpu;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif //__UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CP_INTERFACE_HH__