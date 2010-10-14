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

#define CP15__DEBUG

#include "unisim/component/cxx/processor/arm/arm926ejs/cp15.hh"
#include <assert.h>

#ifdef CP15__DEBUG
#include <iostream>
#endif

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

/** Constructor
 *
 * @param _cpu the cpu the coprocessor is attached to
 */
CP15::
//CP15(unisim::component::cxx::processor::arm::arm926ejs::CPU *_cpu)
CP15(CP15Interface *_cpu,
		const char *name, 
		unisim::kernel::service::Object *parent)
	: Object(name, parent)
	, cpu(_cpu)
	, bigendinit_string("little-endian")
	, param_bigendinit("bigendinit", this,
			bigendinit_string,
			"Determines the value of the BIGENDINIT signal."
			" Available values are: little-endian and big-endian.")
	, vinithi_string("low")
	, param_vinithi("vinithi", this,
			vinithi_string,
			"Determines the value of the VINITHI signal."
			" Available values are: low and high.")
	, verbose(0)
	, param_verbose("verbose", this,
			verbose,
			"Enable verbose mode (0 = non verbose, anything else = verbose).")
	, logger(*this)
	, control_register_c1(0)
{
}

/** Destructor */
CP15::
~CP15()
{
	cpu = 0;
}

/** Object setup method.
 * This method is required for all UNISIM objects and will be called during
 *   the setup phase.
 * 
 * @return true on success, false otherwise
 */
bool 
CP15::
Setup()
{
	logger << DebugInfo << "CP15 Setup" << EndDebugInfo;
	/* check bigendinit parameter */
	if ( (bigendinit_string.compare("little-endian") != 0) &&
			(bigendinit_string.compare("big-endian") != 0) )
	{
		logger << DebugError
			<< "Error while setting the default endianness (BIGENDINIT)."
			<< " '" << bigendinit_string << "' is not a correct"
			<< " value."
			<< " Available values are: little-endian and big-endian."
			<< EndDebugError;
		return false;
	}
	/* check vinithi parameter */
	if ( (vinithi_string.compare("low") != 0) &&
			(vinithi_string.compare("high") != 0) )
	{
		logger << DebugError
			<< "Error while setting the location of the exception vectors"
			<< " (VINITHI). '" << vinithi_string << "' is not a correct value."
			<< " Available values are: low and high."
			<< EndDebugError;
		return false;
	}

	/* fix the endianness depending on the endianness parameter */
	if ( verbose )
		logger << DebugInfo
			<< "Setting endianness to the value of BIGENDINIT ("
			<< bigendinit_string
			<< ")"
			<< EndDebugInfo;
	if ( bigendinit_string.compare("little-endian") == 0 )
		control_register_c1 &= ~CONTROL_REGISTER_C1_B;
	else
		control_register_c1 |= CONTROL_REGISTER_C1_B;

	/* fix the location of the exception vectors depending on vinithi */
	if ( verbose )
		logger << DebugInfo
			<< "Setting the location of the exception to the value of VINITHI ("
			<< vinithi_string
			<< ")"
			<< EndDebugInfo;
	if ( vinithi_string.compare("low") == 0 )
		control_register_c1 &= ~CONTROL_REGISTER_C1_V;
	else
		control_register_c1 |= CONTROL_REGISTER_C1_V;

	return true;
}

/** Read the value of a register
 *
 * @param opcode1 the "opcode1" field of the instruction code 
 * @param opcode2 the "opcode2" field of the instruction code
 * @param crn     the "crn" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param reg     the read value
 */
void
CP15::
ReadRegister(uint8_t opcode1,
		uint8_t opcode2, 
		uint8_t crn, 
		uint8_t crm, 
		uint32_t& reg)
{
	bool handled = false;
#ifdef CP15__DEBUG
	std::cerr << "CP15: Received read register command with: " << std::endl;
	std::cerr << " - opcode1 = " << (unsigned int)opcode1 << std::endl;
	std::cerr << " - opcode2 = " << (unsigned int)opcode2 << std::endl;
	std::cerr << " - crn     = " << (unsigned int)crn << std::endl;
	std::cerr << " - crm     = " << (unsigned int)crm << std::endl;
#endif // CP15__DEBUG
	if ( unlikely(!handled) )
	{
		assert("CP15 read register not handled" == 0);
	}
}

/** Write a value in a register
 * 
 * @param opcode1 the "opcode1" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @param crn     the "crn" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param val     value to be written in the register
 */
void 
CP15::
WriteRegister(uint8_t opcode1,
		uint8_t opcode2, 
		uint8_t crn, 
		uint8_t crm, 
		uint32_t value)
{
	bool handled = false;
#ifdef CP15__DEBUG
	std::cerr << "CP15: Received write register command with: " << std::endl;
	std::cerr << " - opcode1 = " << (unsigned int)opcode1 << std::endl;
	std::cerr << " - opcode2 = " << (unsigned int)opcode2 << std::endl;
	std::cerr << " - crn     = " << (unsigned int)crn << std::endl;
	std::cerr << " - crm     = " << (unsigned int)crm << std::endl;
	std::cerr << " - value   = 0x" << std::hex << value << std::dec << std::endl;
#endif // CP15__DEBUG
	if ( likely(opcode1 == 0) )
	{
		// cache management functions
		if ( crn == 7 )
		{
			if ( crm == 7 )
			{
				switch ( opcode2 )
				{
					case 0:
#ifdef CP15__DEBUG
						std::cerr << "CP15: Invalidating instruction and data"
							<< " caches" << std::endl;
#endif // CP15__DEBUG
						cpu->InvalidateCache(true, true);
						handled = true;
						break;
				}
			}
		}
		else if ( crn == 8 )
		{
			if ( crm == 7 )
			{
				switch ( opcode2 )
				{
					case 0:
#ifdef CP15__DEBUG
						std::cerr << "CP15: Invalidating instruction and data"
							<< " TLBs" << std::endl;
#endif // CP15__DEBUG
						cpu->InvalidateTLB();
						handled = true;
						break;
				}
			}
		}
	}
	if ( unlikely(!handled) )
	{
		assert("CP15 write register not handled" == 0);
	}
}

/** Perform a coprocessor operation
 *
 * @param opcode1 the "opcode1" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @param crd     the "crd" field of the instruction code
 * @param crn     the "crn" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 */
void 
CP15::
Operation(uint8_t opcode1,
		uint8_t opcode2,
		uint8_t crd,
		uint8_t crn,
		uint8_t crm)
{
}

/** Perform a coprocessor load
 *
 * @param crd     the "crd" field of the instruction code
 * @param address the address to load data from
 */
void 
CP15::
Load(uint8_t crd,
		uint32_t address)
{
}

/** Perform a coprocessor load
 *
 * @param crd     the "crd" field of the instruction code
 * @param address the address to store the data to
 */
void 
CP15::
Store(uint8_t crd,
		uint32_t address)
{
}

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
