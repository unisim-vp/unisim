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

// #define CP15__DEBUG

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
	, id_code_register_c0(ID_CODE_REGISTER_C0)
	, control_register_c1(CONTROL_REGISTER_C1_SBO)
	, translation_table_base_register_c2(0)
	, domain_access_control_register_c3(0)
	, fsce_pid_register_c13(0)
	, context_id_register_c13(0)
	, reg_id_code_register_c0("c0_0", this,
			id_code_register_c0,
			"ID Code register.")
	, reg_control_register_c1("c1", this,
			control_register_c1,
			"Control register.")
	, reg_translation_table_base_register_c2("c2", this,
			translation_table_base_register_c2,
			"Translation table base register.")
	, reg_domain_access_control_register_c3("c3", this,
			domain_access_control_register_c3,
			"Domain access control register.")
	, reg_fsce_pid_register_c13("c13_0", this,
			fsce_pid_register_c13,
			"Fast Context Switch Extension (FCSE) Process Identifier (PID) register.")
	, reg_context_id_register_c13("c13_1", this,
			context_id_register_c13,
			"Context ID register.")
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
	if ( likely(opcode1 == 0) )
	{

		// ID Code, Cache Type, and TCM Status Registers
		if ( crn == 0 )
		{
		
			if  ( crm == 0 )
			{
				
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG 
					std::cerr << "CP15: Reading ID code register c0"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					reg = id_code_register_c0;
				}

				else if ( opcode2 == 1 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Reading cache type register c0"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					bool unified = false;
					uint32_t isize = 0;
					uint32_t iassoc = 0;
					uint32_t ilen = 0;
					uint32_t dsize = 0;
					uint32_t dassoc = 0;
					uint32_t dlen = 0;
					cpu->GetCacheInfo(unified,
							isize, iassoc, ilen,
							dsize, dassoc, dlen);
					reg = 0x1c000000UL;
					if ( !unified )
						reg |= 0x01000000UL;
					if ( unified )
					{
						dsize = isize;
						dassoc = iassoc;
						dlen = ilen;
					}
					switch ( isize / 1024 )
					{
						case 4:
							reg |= (0x03UL << 6);
							break;
						case 8:
							reg |= (0x04UL << 6);
							break;
						case 16:
							reg |= (0x05UL << 6);
							break;
						case 32:
							reg |= (0x06UL << 6);
							break;
						case 64:
							reg |= (0x07UL << 6);
							break;
						case 128:
							reg |= (0x08UL << 6);
							break;
						default:
							logger << DebugError
								<< "Incorrect insn cache size (" << isize << ")"
								<< EndDebugError;
							assert("Invalid insn cache size" == 0);
							break;
					}
					switch ( dsize / 1024 )
					{
						case 4:
							reg |= ((0x03UL << 6) << 12);
							break;
						case 8:
							reg |= ((0x04UL << 6) << 12);
							break;
						case 16:
							reg |= ((0x05UL << 6) << 12);
							break;
						case 32:
							reg |= ((0x06UL << 6) << 12);
							break;
						case 64:
							reg |= ((0x07UL << 6) << 12);
							break;
						case 128:
							reg |= ((0x08UL << 6) << 12);
							break;
						default:
							logger << DebugError
								<< "Incorrect data cache size (" << isize << ")"
								<< EndDebugError;
							assert("Invalid data cache size" == 0);
							break;
					}
					switch ( iassoc )
					{
						case 4:
							reg |= (0x02UL << 3);
							break;
						default:
							logger << DebugError
								<< "Incorrect insn cache associativity (" 
								<< iassoc << ")"
								<< EndDebugError;
							assert("Invalid insn cache associativity" == 0);
							break;
					}
					switch ( dassoc )
					{
						case 4:
							reg |= ((0x02UL << 3) << 12);
							break;
						default:
							logger << DebugError
								<< "Incorrect data cache associativity (" 
								<< dassoc << ")"
								<< EndDebugError;
							assert("Invalid data cache associativity" == 0);
							break;
					}
					switch ( ilen )
					{
						case 32:
							reg |= 0x02UL;
							break;
						default:
							logger << DebugError
								<< "Incorrect insn cache line length ("
								<< ilen << ")"
								<< EndDebugError;
							assert("Invalid insn cache line length" == 0);
							break;
					}
					switch ( dlen )
					{
						case 32:
							reg |= (0x02UL << 12);
							break;
						default:
							logger << DebugError
								<< "Incorrect data cache line length ("
								<< dlen << ")"
								<< EndDebugError;
							assert("Invalid data cache line length" == 0);
							break;
					}
					// no need to set the M bit which should be always 0
					//   meaning that the caches are present
				}
			}
		}

		// control registers
		else if ( crn == 1 )
		{
			
			if ( crm == 0 )
			{
			
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Reading control register c1"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					reg = control_register_c1;
				}
			}
		}

		// cache operations register
		else if ( crn == 7 )
		{
		
			if ( crm == 10 )
			{
				if ( opcode2 == 3 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Reading cache operation register c7"
						", performing test and clean DCache"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					if ( cpu->TestAndCleanDCache() )
						reg |= 0x40000000UL;
					else
						reg &= ~0x40000000UL;
				}
			}
			
			else if ( crm == 14 )
			{
				if ( opcode2 == 3 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Reading cache operations register c7"
						<< ", performing test, clean and invalidate DCache"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					if ( cpu->TestCleanAndInvalidateDCache() )
						reg |= 0x40000000UL;
					else
						reg &= ~0x40000000UL;
				}
			}
		}
	}

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
	uint32_t orig = value;
	uint32_t mod = value;
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
		
		// control registers
		if ( crn == 1 )
		{
			if ( crm == 0 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Writing control register c1"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					mod &= ~CONTROL_REGISTER_C1_SBZ;
					mod |= CONTROL_REGISTER_C1_SBO;
					if ( mod != orig )
						logger << DebugWarning
							<< "Writing value"
							<< " 0x" << std::hex << orig << std::dec
							<< " into control register c1 which is not conform"
							<< " with SBZ and SBO specification, fixing it to"
							<< " 0x" << std::hex << mod << std::dec
							<< EndDebugWarning;
					control_register_c1 = mod;
					if ( control_register_c1 &
							CONTROL_REGISTER_C1_C )
						logger << DebugInfo
							<< "Dcache Enabled !!!!!!!!"
							<< EndDebugInfo;
				}
			}
		}
		
		// translation table base register
		else if ( crn == 2 )
		{
			if ( crm == 0 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Writing translation table base register"
						<< " c2"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					mod = orig & ~TRANSLATION_TABLE_BASE_REGISTER_SBZ;
					if ( mod != orig )
						logger << DebugWarning
							<< "Writing value"
							<< " 0x" << std::hex << orig << std::dec
							<< " into cthe translation table base register c2"
							<< " which is not conform with SBZ specification,"
							<< " fixing it to 0x" << std::hex << mod << std::dec
							<< EndDebugWarning;
					translation_table_base_register_c2 = mod;
				}
			}
		}
		
		// domain access control
		else if ( crn == 3 )
		{
			if ( crm == 0 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Writing domain access control register"
						<< " c3"
						<< std::endl;
#endif // CP15__DEBUG
					handled = true;
					domain_access_control_register_c3 = orig;
				}
			}
		}
		
		// cache management functions
		else if ( crn == 7 )
		{
			if ( crm == 5 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Invalidating instruction cache"
						<< std::endl;
#endif // CP15__DEBUG
					cpu->InvalidateCache(true, false);
					handled = true;
				}

				else if ( opcode2 == 1 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Invalidating ICache single entry (MVA)"
						<< std::endl;
#endif // CP15__DEBUG
					cpu->InvalidateICacheSingleEntryWithMVA(value);
					handled = true;
				}
			}
		
			else if ( crm == 7 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
						std::cerr << "CP15: Invalidating instruction and data"
							<< " caches" << std::endl;
#endif // CP15__DEBUG
						cpu->InvalidateCache(true, true);
						handled = true;
				}
			}
			
			else if ( crm == 10 )
			{
				if ( opcode2 == 1 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Clean DCache single entry (MVA)"
						<< std::endl;
#endif // CP15__DEBUG
					cpu->CleanDCacheSingleEntryWithMVA(value, false);
					handled = true;
				}

				else if ( opcode2 == 4 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Draining write buffer"
						<< std::endl;
#endif // CP15__DEBUG
					cpu->DrainWriteBuffer();
					handled = true;
				}
			}

			else if ( crm == 14 )
			{
				if ( opcode2 == 1 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Clean and invalidate DCache"
						<< " single entry (MVA)"
						<< std::endl;
#endif // CP15__DEBUG
					cpu->CleanDCacheSingleEntryWithMVA(value, true);
					handled = true;
				}
			}
		}

		// TLB functions
		else if ( crn == 8 )
		{
			if ( crm == 7 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15__DEBUG
					std::cerr << "CP15: Invalidating set-associative TLB"
						<< std::endl;
#endif // CP15__DEBUG
					cpu->InvalidateTLB();
					handled = true;
				}
			}

			else if ( crm == 5 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15_DEBUG
					std::cerr << "CP15: Invalidating set-associative TLB"
						<< std::endl;
#endif // CP15_DEBUG
					cpu->InvalidateTLB();
					handled = true;
				}
			}

			else if ( crm == 6 )
			{
				if ( opcode2 == 0 )
				{
#ifdef CP15_DEBUG
					std::cerr << "CP15: Invalidating set-associative TLB"
						<< std::endl;
#endif // CP15_DEBUG
					cpu->InvalidateTLB();
					handled = true;
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
