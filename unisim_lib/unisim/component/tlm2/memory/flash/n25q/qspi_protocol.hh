/*
 *  Copyright (c) 2016,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_QSPI_PROTOCOL_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_QSPI_PROTOCOL_HH__

#include <unisim/kernel/tlm2/tlm.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

struct tlm_qspi_protocol_types
{
	typedef tlm::tlm_generic_payload tlm_payload_type;
	typedef tlm::tlm_phase tlm_phase_type;
};

enum tlm_qspi_protocol
{
	TLM_QSPI_EXTENDED_PROTOCOL,
	TLM_QSPI_DUAL_IO_PROTOCOL,
	TLM_QSPI_QUAD_IO_PROTOCOL
};

enum tlm_qspi_command
{
	TLM_QSPI_UNKNOWN_COMMAND = 0,
	// IDENTIFICATION Operations                                              // In combination with
	TLM_QSPI_READ_ID_COMMAND,                                                 // tlm::TLM_READ_COMMAND
	TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND,                                     // tlm::TLM_READ_COMMAND
	TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND,
	// READ Operations
	TLM_QSPI_READ_COMMAND,                                                    // tlm::TLM_READ_COMMAND
	TLM_QSPI_FAST_READ_COMMAND,                                               // tlm::TLM_READ_COMMAND
	TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND,                                   // tlm::TLM_READ_COMMAND
	TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND,                             // tlm::TLM_READ_COMMAND
	TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND,                                   // tlm::TLM_READ_COMMAND
	TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND,                             // tlm::TLM_READ_COMMAND
	// WRITE Operations                                               
	TLM_QSPI_WRITE_ENABLE_COMMAND,                                            // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_WRITE_DISABLE_COMMAND,                                           // tlm::TLM_WRITE_COMMAND
	// REGISTER Operations
	TLM_QSPI_READ_STATUS_REGISTER_COMMAND,                                    // tlm::TLM_READ_COMMAND
	TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND,                                   // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_READ_LOCK_REGISTER_COMMAND,                                      // tlm::TLM_READ_COMMAND
	TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND,                                     // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND,                               // tlm::TLM_READ_COMMAND
	TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND,                              // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND,                 // tlm::TLM_READ_COMMAND
	TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND,                // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND,                    // tlm::TLM_READ_COMMAND
	TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND,                   // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND,           // tlm::TLM_READ_COMMAND
	TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND,          // tlm::TLM_WRITE_COMMAND
	// PROGRAM Operations
	TLM_QSPI_PAGE_PROGRAM_COMMAND,                                            // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND,                                 // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND,                        // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND,                                 // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND,                        // tlm::TLM_WRITE_COMMAND
	// ERASE Operations
	TLM_QSPI_SUBSECTOR_ERASE_COMMAND,                                         // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_SECTOR_ERASE_COMMAND,                                            // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_BULK_ERASE_COMMAND,                                              // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND,                                    // tlm::TLM_WRITE_COMMAND
	TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND,                                   // tlm::TLM_WRITE_COMMAND
	// ONE-TIME PROGRAMMABLE (OTP) Operations
	TLM_QSPI_READ_OTP_ARRAY_COMMAND,                                          // tlm::TLM_READ_COMMAND
	TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND                                        // tlm::TLM_WRITE_COMMAND
};

enum tlm_qspi_signal
{
	NO_SIGNAL = 0,
	DQ0 = 1,
	DQ1 = 2,
	DQ1_0 = DQ1 + DQ0,
	DQ2 = 4,
	DQ3 = 8,
	DQ3_0 = DQ3 + DQ2 + DQ1 + DQ0
};

enum tlm_qspi_mode
{
	TLM_QSPI_STD_MODE,
	TLM_QSPI_XIP_MODE
};

inline std::ostream& operator << (std::ostream& os, const tlm_qspi_protocol& qspi_protocol)
{
	switch(qspi_protocol)
	{
		case TLM_QSPI_EXTENDED_PROTOCOL: os << "QSPI extended protocol"; break;
		case TLM_QSPI_DUAL_IO_PROTOCOL: os << "QSPI dual I/O protocol"; break;
		case TLM_QSPI_QUAD_IO_PROTOCOL: os << "QSPI quad I/O protocol"; break;
	}
	return os;
}

inline std::ostream& operator << (std::ostream& os, const tlm_qspi_command& qspi_cmd)
{
	switch(qspi_cmd)
	{
		TLM_QSPI_READ_ID_COMMAND: os << "QSPI READ ID command"; break;
		TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND: os << "QSPI MULTIPLE I/O READ ID command"; break;
		TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND: os << "QSPI SERIAL FLASH DISCOVERY PARAMETER command"; break;
		TLM_QSPI_READ_COMMAND: os << "QSPI READ command"; break;
		TLM_QSPI_FAST_READ_COMMAND: os << "QSPI FAST READ command"; break;
		TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND: os << "QSPI DUAL OUTPUT FAST READ command"; break;
		TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND: os << "QSPI DUAL INPUT/OUTPUT FAST READ command"; break;
		TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND: os << "QSPI QUAD OUTPUT FAST READ command"; break;
		TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND: os << "QSPI QUAD INPUT/OUTPUT FAST READ command"; break;
		TLM_QSPI_WRITE_ENABLE_COMMAND: os << "QSPI WRITE ENABLE command"; break;
		TLM_QSPI_WRITE_DISABLE_COMMAND: os << "QSPI WRITE DISABLE command"; break;
		TLM_QSPI_READ_STATUS_REGISTER_COMMAND: os << "QSPI READ STATUS REGISTER command"; break;
		TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND: os << "QSPI WRITE STATUS REGISTER command"; break;
		TLM_QSPI_READ_LOCK_REGISTER_COMMAND: os << "QSPI READ LOCK REGISTER command"; break;
		TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND: os << "QSPI WRITE LOCK REGISTER command"; break;
		TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND: os << "QSPI READ FLAG STATUS REGISTER command"; break;
		TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND: os << "QSPI CLEAR FLAG STATUS REGISTER command"; break;
		TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "QSPI READ NONVOLATILE CONFIGURATION REGISTER command"; break;
		TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "QSPI WRITE NONVOLATILE CONFIGURATION REGISTER command"; break;
		TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "QSPI READ VOLATILE CONFIGURATION REGISTER command"; break;
		TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "QSPI WRITE VOLATION CONFIGURATION REGISTER command"; break;
		TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "QSPI READ ENHANCED VOLATILE CONFIGURATION REGISTER command"; break;
		TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "QSPI WRITE ENHANCED VOLATILE CONFIGURATION REGISTER command"; break;
		TLM_QSPI_PAGE_PROGRAM_COMMAND: os << "QSPI PAGE PROGRAM command"; break;
		TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND: os << "DUAL INPUT FAST PROGRAM command"; break;
		TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND: os << "QSPI EXTENDED DUAL INPUT FAST PROGRAM command"; break;
		TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND: os << "QSPI QUAD INPUT FAST PROGRAM command"; break;
		TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: os << "QSPI EXTENDED QUAD INPUT FAST PROGRAM command"; break;
		TLM_QSPI_SUBSECTOR_ERASE_COMMAND: os << "QSPI SUBSECTOR ERASE command"; break;
		TLM_QSPI_SECTOR_ERASE_COMMAND: os << "QSPI SECTOR ERASE command"; break;
		TLM_QSPI_BULK_ERASE_COMMAND: os << "QSPI BULK ERASE command"; break;
		TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND: os << "QSPI PROGRAM/ERASE RESUME command"; break;
		TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND: os << "QSPI PROGRAM/ERASE SUSPEND command"; break;
		TLM_QSPI_READ_OTP_ARRAY_COMMAND: os << "QSPI READ OTP ARRAY command"; break;
		TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND: os << "QSPI PROGRAM OTP ARRAY command"; break;
	}
	
	return os;
}

inline std::ostream& operator << (std::ostream& os, const tlm_qspi_signal& s)
{
	switch(s)
	{
		case NO_SIGNAL: os << "no signal"; break;
		case DQ0: os << "DQ0"; break;
		case DQ1: os << "DQ1"; break;
		case DQ1_0: os << "DQ[1:0]"; break;
		case DQ2: os << "DQ2"; break;
		case DQ3: os << "DQ3"; break;
		case DQ3_0: os << "DQ[3:0]"; break;
	}
	return os;
}

inline bool tlm_qspi_is_program_command(tlm_qspi_command qspi_cmd)
{
	switch(qspi_cmd)
	{
		case TLM_QSPI_PAGE_PROGRAM_COMMAND:
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			return true;
		default:
			return false;
	}
	return false;
}

inline bool tlm_qspi_is_erase_command(tlm_qspi_command qspi_cmd)
{
	switch(qspi_cmd)
	{
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
		case TLM_QSPI_SECTOR_ERASE_COMMAND:
		case TLM_QSPI_BULK_ERASE_COMMAND:
			return true;
		default:
			return false;
	}
	return false;
}

inline bool tlm_qspi_command_is_supported(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol)
{
	switch(qspi_cmd)
	{
		// IDENTIFICATION Operations
		case TLM_QSPI_READ_ID_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return false; // unsupported
				case TLM_QSPI_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return false; // unsupported
				case TLM_QSPI_DUAL_IO_PROTOCOL: return true;
				case TLM_QSPI_QUAD_IO_PROTOCOL: return true;
			}
			break;
		case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
			return true;
		// READ Operations
		case TLM_QSPI_READ_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return false; // unsupported
				case TLM_QSPI_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case TLM_QSPI_FAST_READ_COMMAND:
			return true;
		case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return true;
				case TLM_QSPI_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return false; // unsupported
				case TLM_QSPI_QUAD_IO_PROTOCOL: return true;
			}
			break;
		// WRITE Operations                                               
		case TLM_QSPI_WRITE_ENABLE_COMMAND: return true;
		case TLM_QSPI_WRITE_DISABLE_COMMAND: return true;
		// REGISTER Operations
		case TLM_QSPI_READ_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_LOCK_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		// PROGRAM Operations
		case TLM_QSPI_PAGE_PROGRAM_COMMAND: return true;
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return true;
				case TLM_QSPI_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return true;
				case TLM_QSPI_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return false; // unsupported
				case TLM_QSPI_QUAD_IO_PROTOCOL: return true;
			}
			break;
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return true;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return false; // unsupported
				case TLM_QSPI_QUAD_IO_PROTOCOL: return true;
			}
			break;
		// ERASE Operations
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND: return true;
		case TLM_QSPI_SECTOR_ERASE_COMMAND: return true;
		case TLM_QSPI_BULK_ERASE_COMMAND: return true;
		case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND: return true;
		case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND: return true;
		// ONE-TIME PROGRAMMABLE (OTP) Operations
		case TLM_QSPI_READ_OTP_ARRAY_COMMAND: return true;
		case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND: return true;
	}
	return false;
}

inline bool tlm_qspi_command_has_address(tlm_qspi_command qspi_cmd)
{
	switch(qspi_cmd)
	{
		// IDENTIFICATION Operations
		case TLM_QSPI_READ_ID_COMMAND: return false;
		case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND: return false;
		case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND: return true;
		// READ Operations
		case TLM_QSPI_READ_COMMAND: return true;
		case TLM_QSPI_FAST_READ_COMMAND: return true;
		case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND: return true;
		case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND: return true;
		case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND: return true;
		case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND: return true;
		// WRITE Operations
		case TLM_QSPI_WRITE_ENABLE_COMMAND: return false;
		case TLM_QSPI_WRITE_DISABLE_COMMAND: return false;
		// REGISTER Operations
		case TLM_QSPI_READ_STATUS_REGISTER_COMMAND: return false;
		case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND: return false;
		case TLM_QSPI_READ_LOCK_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND: return false;
		case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return false;
		case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		// PROGRAM Operations
		case TLM_QSPI_PAGE_PROGRAM_COMMAND: return true;
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND: return true;
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND: return true;
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
		// ERASE Operations
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND: return true;
		case TLM_QSPI_SECTOR_ERASE_COMMAND: return true;
		case TLM_QSPI_BULK_ERASE_COMMAND: return false;
		case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND: return false;
		case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND: return false;
		// ONE-TIME PROGRAMMABLE (OTP) Operations
		case TLM_QSPI_READ_OTP_ARRAY_COMMAND: return true;
		case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND: return true;
	}
	return false;
}

inline bool tlm_qspi_command_has_data(tlm_qspi_command qspi_cmd)
{
	switch(qspi_cmd)
	{
		// IDENTIFICATION Operations
		case TLM_QSPI_READ_ID_COMMAND: return true;
		case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND: return true;
		case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND: return true;
		// READ Operations
		case TLM_QSPI_READ_COMMAND: return true;
		case TLM_QSPI_FAST_READ_COMMAND: return true;
		case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND: return true;
		case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND: return true;
		case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND: return true;
		case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND: return true;
		// WRITE Operations
		case TLM_QSPI_WRITE_ENABLE_COMMAND: return false;
		case TLM_QSPI_WRITE_DISABLE_COMMAND: return false;
		// REGISTER Operations
		case TLM_QSPI_READ_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_LOCK_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND: return true;
		case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return false;
		case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		// PROGRAM Operations
		case TLM_QSPI_PAGE_PROGRAM_COMMAND: return true;
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND: return true;
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND: return true;
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
		// ERASE Operations
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND: return false;
		case TLM_QSPI_SECTOR_ERASE_COMMAND: return false;
		case TLM_QSPI_BULK_ERASE_COMMAND: return false;
		case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND: return false;
		case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND: return false;
		// ONE-TIME PROGRAMMABLE (OTP) Operations
		case TLM_QSPI_READ_OTP_ARRAY_COMMAND: return true;
		case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND: return true;
	}
	return false;
}

inline unsigned int tlm_qspi_signal_width(tlm_qspi_signal s)
{
	switch(s)
	{
		case NO_SIGNAL: return 0;
		case DQ0: return 1;
		case DQ1: return 1;
		case DQ1_0: return 2;
		case DQ3_0: return 4;
	}
	return 0;
}

inline tlm_qspi_signal tlm_qspi_command_signal(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol)
{
	switch(qspi_cmd)
	{
		case TLM_QSPI_READ_ID_COMMAND:
		case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND:
		case TLM_QSPI_READ_COMMAND:
		case TLM_QSPI_FAST_READ_COMMAND:
		case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_WRITE_ENABLE_COMMAND:
		case TLM_QSPI_WRITE_DISABLE_COMMAND:
		case TLM_QSPI_READ_STATUS_REGISTER_COMMAND:
		case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND:
		case TLM_QSPI_READ_LOCK_REGISTER_COMMAND:
		case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND:
		case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND:
		case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
		case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case TLM_QSPI_READ_OTP_ARRAY_COMMAND:
		case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND:
			switch(qspi_protocol)
			{
				case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
				case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
				case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
			}
			break;
		case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
		case TLM_QSPI_PAGE_PROGRAM_COMMAND:
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
		case TLM_QSPI_SECTOR_ERASE_COMMAND:
		case TLM_QSPI_BULK_ERASE_COMMAND:
		case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND:
		case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND:
			return DQ0;
	}
	return DQ0; // should never occur
}

inline tlm_qspi_signal tlm_qspi_address_signal(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol, tlm_qspi_mode qspi_mode)
{
	switch(qspi_mode)
	{
		case TLM_QSPI_STD_MODE:
			switch(qspi_cmd)
			{
				case TLM_QSPI_READ_ID_COMMAND:
				case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND:
				case TLM_QSPI_WRITE_ENABLE_COMMAND:
				case TLM_QSPI_WRITE_DISABLE_COMMAND:
				case TLM_QSPI_READ_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_BULK_ERASE_COMMAND:
				case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND:
				case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND:
					return NO_SIGNAL; // N/A
				case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
				case TLM_QSPI_PAGE_PROGRAM_COMMAND:
					return DQ0;
				// READ Operations
				case TLM_QSPI_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_FAST_READ_COMMAND:
				case TLM_QSPI_READ_LOCK_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND:
				case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
				case TLM_QSPI_SECTOR_ERASE_COMMAND:
				case TLM_QSPI_READ_OTP_ARRAY_COMMAND:
				case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1_0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ0;
					}
					break;
				case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ3_0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
			}
			break;
			
		case TLM_QSPI_XIP_MODE:
			switch(qspi_cmd)
			{
				case TLM_QSPI_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				default:
					return NO_SIGNAL; // N/A
			}
			break;
	}
	return NO_SIGNAL; // should never occur
}

inline tlm_qspi_signal tlm_qspi_data_signal(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol, tlm_qspi_mode qspi_mode)
{
	switch(qspi_mode)
	{
		case TLM_QSPI_STD_MODE:
			switch(qspi_cmd)
			{
				// IDENTIFICATION Operations
				case TLM_QSPI_READ_ID_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1;  // just a guess
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0; // just a guess
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0; // just a guess
					}
					break;
				case TLM_QSPI_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_FAST_READ_COMMAND:
				case TLM_QSPI_READ_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_READ_LOCK_REGISTER_COMMAND:
				case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_READ_OTP_ARRAY_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1_0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ3_0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
				case TLM_QSPI_WRITE_ENABLE_COMMAND:
				case TLM_QSPI_WRITE_DISABLE_COMMAND:
				case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
				case TLM_QSPI_SECTOR_ERASE_COMMAND:
				case TLM_QSPI_BULK_ERASE_COMMAND:
				case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND:
				case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND:
					return NO_SIGNAL; // N/A
				case TLM_QSPI_PAGE_PROGRAM_COMMAND:
					return DQ0;
				case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
					return DQ1_0;
				case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					return DQ3_0;
			}
			break;
			
		case TLM_QSPI_XIP_MODE:
			switch(qspi_cmd)
			{
				case TLM_QSPI_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ1_0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return DQ1_0;
						case TLM_QSPI_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL: return DQ3_0;
						case TLM_QSPI_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case TLM_QSPI_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				defaut:
					return NO_SIGNAL; // N/A
			}
			break;
	}
	
	return NO_SIGNAL; // should not occur
}

inline unsigned int tlm_qspi_command_cycles(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol, tlm_qspi_mode qspi_mode)
{
	if(qspi_mode == TLM_QSPI_XIP_MODE) return 0;
	
	unsigned int w = tlm_qspi_signal_width(tlm_qspi_command_signal(qspi_cmd, qspi_protocol));
	return 1 * (8 / w);
}

inline unsigned int tlm_qspi_address_cycles(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol, tlm_qspi_mode qspi_mode)
{
	unsigned int w = tlm_qspi_signal_width(tlm_qspi_address_signal(qspi_cmd, qspi_protocol, qspi_mode));
	return 3 * (8 / w);
}

inline unsigned int tlm_qspi_data_cycles_per_byte(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol, tlm_qspi_mode qspi_mode)
{
	unsigned int w = tlm_qspi_signal_width(tlm_qspi_data_signal(qspi_cmd, qspi_protocol, qspi_mode));
	return 1 * (8 / w);
}

inline unsigned int tlm_qspi_dummy_cycles(tlm_qspi_command qspi_cmd, tlm_qspi_protocol qspi_protocol, tlm_qspi_mode qspi_mode)
{
	switch(qspi_mode)
	{
		case TLM_QSPI_STD_MODE:
			switch(qspi_cmd)
			{
				case TLM_QSPI_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
						return 15;
				case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
				case TLM_QSPI_READ_OTP_ARRAY_COMMAND:
					switch(qspi_protocol)
					{
						case TLM_QSPI_EXTENDED_PROTOCOL:
						case TLM_QSPI_DUAL_IO_PROTOCOL:
							return 8;
						case TLM_QSPI_QUAD_IO_PROTOCOL:
							return 10;
					}
					break;
				default:
					return 0;
			}
			break;
		case TLM_QSPI_XIP_MODE:
			switch(qspi_cmd)
			{
				case TLM_QSPI_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
				case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					return 15;
				default:
					return 0; // N/A
			}
			break;
	}
	return 0;
}

class tlm_qspi_extension : public tlm::tlm_extension<tlm_qspi_extension>
{
public:
	tlm_qspi_extension();
	tlm_qspi_extension(tlm_qspi_command qspi_cmd, const sc_core::sc_time& cycle_period, bool xip_confirmation_bit);

	virtual tlm::tlm_extension_base *clone() const;
	virtual void copy_from(const tlm::tlm_extension_base& ext);

	void set_command(tlm_qspi_command _qspi_cmd) { qspi_cmd = _qspi_cmd; }
	void set_cycle_period(const sc_core::sc_time& _cycle_period) { cycle_period = _cycle_period; }
	void set_xip_confirmation_bit(bool _xip_confirmation_bit) { xip_confirmation_bit = _xip_confirmation_bit; }
	
	tlm_qspi_command get_command() const { return qspi_cmd; }
	const sc_core::sc_time& get_cycle_period() const { return cycle_period; }
	bool get_xip_confirmation_bit() const { return xip_confirmation_bit; }
private:
	tlm_qspi_command qspi_cmd;
	sc_core::sc_time cycle_period;
	bool xip_confirmation_bit; // during FAST READ
};

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_QSPI_PROTOCOL_HH__
