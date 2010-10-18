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
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

class CP15
	: public unisim::kernel::service::Object
{
public:
	/* TODO check if we really need the pointer to the cpu */
	/** Constructor
	 *
	 * @param _cpu the cpu the coprocessor is attached to
	 */
	//CP15(unisim::component::cxx::processor::arm::arm926ejs::CPU* _cpu);
	CP15(CP15Interface *_cpu, 
			const char *name, 
			unisim::kernel::service::Object *parent = 0);
	/** Destructor */
	~CP15();

	/** Object setup method.
	 * This method is required for all UNISIM objects and will be called during
	 *   the setup phase.
	 * 
	 * @return true on success, false otherwise
	 */
	virtual bool Setup();

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

	/** Get the endianness of the processor
	 *
	 * @return the current endianness defined in the control register
	 */
	unisim::util::endian::endian_type
		GetEndianness() const
	{
		return 
			(control_register_c1 & CONTROL_REGISTER_C1_B)
			? unisim::util::endian::E_BIG_ENDIAN
			: unisim::util::endian::E_LITTLE_ENDIAN;
	};

	/** Get location of the exception vector
	 *
	 * @return 1 if CONTROL_REGISTER_C1_V is set, 0 otherwise
	 */
	uint32_t GetVINITHI() const
	{
		return 
			(control_register_c1 & CONTROL_REGISTER_C1_V)
			? 1
			: 0;
	}

private:
	CP15Interface *cpu;

	/** String describing the endianness of the processor. */
	std::string bigendinit_string;
	/** UNISIM Parameter to set the default endianness.
	 */
	unisim::kernel::service::Parameter<std::string> param_bigendinit;

	/** String describing the location of exception vectors
	 * This corresponds to the signal VINITHI. */
	std::string vinithi_string;
	/** UNISIM Parameter to set the location of exception vectors. */
	unisim::kernel::service::Parameter<std::string> param_vinithi;

	/** Verbose. */
	uint32_t verbose;
	/** UNISIM Parameter to set the verbose mode. */
	unisim::kernel::service::Parameter<uint32_t> param_verbose;

	/** UNISIM logger */
	unisim::kernel::logger::Logger logger;

	/** CP15 control register */
	uint32_t control_register_c1;

	static const uint32_t CONTROL_REGISTER_C1_SBZ = 0xfffa0c00UL;
	static const uint32_t CONTROL_REGISTER_C1_SBO = 0x00050078UL;
	static const uint32_t CONTROL_REGISTER_C1_L4 = 0x08000UL;
	static const uint32_t CONTROL_REGISTER_C1_RR = 0x04000UL;
	static const uint32_t CONTROL_REGISTER_C1_V  = 0x02000UL;
	static const uint32_t CONTROL_REGISTER_C1_I  = 0x01000UL;
	static const uint32_t CONTROL_REGISTER_C1_R  = 0x00200UL;
	static const uint32_t CONTROL_REGISTER_C1_S  = 0x00100UL;
	static const uint32_t CONTROL_REGISTER_C1_B  = 0x00080UL;
	static const uint32_t CONTROL_REGISTER_C1_C  = 0x00004UL;
	static const uint32_t CONTROL_REGISTER_C1_A  = 0x00002UL;
	static const uint32_t CONTROL_REGISTER_C1_M  = 0x00001UL;
};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__
