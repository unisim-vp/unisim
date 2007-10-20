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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_ARM966E_S_CP15_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_ARM966E_S_CP15_TCC__

#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace coprocessor {
namespace arm966e_s {

#define LOCATION File << __FILE__ << Function << __FUNCTION__ << Line << __LINE__

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

using unisim::util::endian::E_BIG_ENDIAN;

using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;

template<class CONFIG>
CP15<CONFIG> ::
CP15(const char *name,
		unsigned int _cp_id,
		CPUCPInterface *_cpu,
		DTCM<CONFIG> *_dtcm,
		ITCM<CONFIG> *_itcm,
		Object *parent) :
	Object(name, parent),
	CPInterface<CONFIG::DEBUG_ENABLE>(name, _cp_id, _cpu, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	memory_import("memory_import", this),
	memory_export("memory_export", this),
	silicon_revision_number(0),
	param_silicon_revision_number("silicon-revision-number", this, silicon_revision_number),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_read_reg(false),
	param_verbose_read_reg("verbose-read-reg", this, verbose_read_reg),
	verbose_write_reg(false),
	param_verbose_write_reg("verbose-write-reg", this, verbose_write_reg) {
	SetupDependsOn(inherited::logger_import);
}

template<class CONFIG>
bool
CP15<CONFIG> ::
Setup() {
	// check verbose parameters
	if(verbose_all) {
		verbose_read_reg = true;
		verbose_write_reg = true;
	}
	// check parameters
	if(silicon_revision_number > (uint32_t)0x0fff) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Revision number should be smaller or equal than 0x0fff"
				<< Endl << EndDebugError;
		return false;
	}
	
	InitRegs();

	return true;
}

template<class CONFIG>
const char *
CP15<CONFIG> ::
Description() {
	return "CP15(ARM966E_S)";
}

template<class CONFIG>
const char* 
CP15<CONFIG> ::
RegisterName(uint32_t id, uint32_t option) {
	switch(id) {
	case 0:
		switch(option) {
		case 2:
			return "TCM size";
			break;
		default:
			return "ID code";
			break;
		}
		break;
	case 1:
		return "Control register";
		break;
	case 7:
		return "Core control";
		break;
	case 13:
		return "Trace process identifier";
		break;
	case 15:
		switch(option) {
		case 0:
			return "Control configuration";
			break;
		case 1:
			return "BIST control";
			break;
		case 2:
			return "Instruction BIST address";
			break;
		case 3:
			return "Instruction BIST general";
			break;
		case 6:
			return "Data BIST address";
			break;
		case 7:
			return "Data BIST general";
			break;
		default:
			return "Undefined";
			break;
		}
		return "Test and configuration";
		break;
	default:
		return "Undefined";
	}
	
}

template<class CONFIG>
void 
CP15<CONFIG> ::
Reset() {
}

template<class CONFIG>
void 
CP15<CONFIG> ::
ReadRegister(uint8_t opcode1, 
		uint8_t opcode2, 
		uint8_t crn, 
		uint8_t crm, 
		reg_t& reg) {
	bool handled = false;
	char *name;
	
	reg = 0;

	// check which register is being read
	// id code register
	if(opcode1 == 0 && crn == 0 && crm == 0 && opcode2 != 2) {
		reg = id_code_reg;
		handled = true;
		name = "ID Code";
	}
	// tcm size register
	if(opcode1 == 0 && crn == 0 && crm == 0 && opcode2 == 2) {
		reg = tcm_size_reg;
		handled = true;
		name = "TCM Size";
	}
	// control register
	if(opcode1 == 0 && crn == 1 && crm == 0 && opcode2 == 0) {
		reg = control_reg;
		handled = true;
		name = "Control";
	}
	// trace process identifier register
	if(opcode1 == 0 && crn == 13 && opcode2 == 0) {
		reg = tpi_reg;
		handled = true;
		name = "Trace Process Identifier";
	}
	// configuration control register
	if(opcode1 == 1 && crn == 15 && crm == 1 && opcode2 == 0) {
		reg = conf_control_reg;
		handled = true;
		name = "Configuration Control";
	}
	// bist control register
	if(opcode1 == 1 && crn == 15 && crm == 1 && opcode2 == 1) {
		reg = 0;
		handled = true;
		name = "BIST Control";
	}
	// instruction bist address register
	if(opcode1 == 1 && crn == 15 && crm == 1 && opcode2 == 2) {
		reg = 0;
		handled = true;
		name = "Instruction BIST Address";
	}
	// instruction bist general register
	if(opcode1 == 1 && crn == 15 && crm == 1 && opcode2 == 3) {
		reg = 0;
		handled = true;
		name = "Instruction BIST General";
	}
	// data bist address register
	if(opcode1 == 1 && crn == 15 && crm == 1 && opcode2 == 6) {
		reg = 0;
		handled = true;
		name = "Data BIST Address";
	}
	// data bist general register
	if(opcode1 == 1 && crn == 15 && crm == 1 && opcode2 == 7) {
		reg = 0;
		handled = true;
		name = "Data BIST General";
	}
	
	if(!handled) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugWarning << LOCATION
				<< "Trying to read an inexistent register in :" << Endl
				<< Hex
				<< " - opcode1 = 0x" << opcode1
				<< " - opcode2 = 0x" << opcode2
				<< Dec
				<< " - crn = " << crn
				<< " - crm = " << crm
				<< Endl << EndDebugWarning;
	} else {
		if(VerboseReadReg()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Reading '" << name << "' register (value = 0x"
				<< Hex << reg << Dec << ")" << Endl;
		}
	}
}

template<class CONFIG>
void  
CP15<CONFIG> ::
WriteRegister(uint8_t opcode1, 
		uint8_t opcode2, 
		uint8_t crn, 
		uint8_t crm, 
		reg_t value) {
	// control register
	if(opcode1 == 0 && opcode2 == 0 && crn == 1 && crm == 0) {
		WriteControlReg(value);
		return;
	}
}

template<class CONFIG>
void
CP15<CONFIG> :: 
Operation(uint8_t opcode1,
		uint8_t opcode2,
		uint8_t crd,
		uint8_t crn,
		uint8_t crm) {
	if(inherited::logger_import)
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Operations not supported on this coprocessor: "
			<< " - opcode1 = 0x" << Hex << opcode1 << Endl
			<< " - opcode2 = 0x" << opcode2 << Dec << Endl
			<< " - crd = " << crd << Endl
			<< " - crn = " << crn << Endl
			<< " - crm = " << crm << Endl
			<< EndDebugError;
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void
CP15<CONFIG> :: 
Load(uint8_t crd,
		reg_t address) {
	if(inherited::logger_import)
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Loads not supported on this coprocessor: "
			<< " - crd = " << crd << Endl
			<< " - address = 0x" << Hex << address << Dec << Endl
			<< EndDebugError;
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void
CP15<CONFIG> :: 
Store(uint8_t crd,
		reg_t address) {
	if(inherited::logger_import)
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Stores not supported on this coprocessor: "
			<< " - crd = " << crd << Endl
			<< " - address = 0x" << Hex << address << Dec << Endl
			<< EndDebugError;
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
bool 
CP15<CONFIG> ::
ReadMemory(address_t addr, void *buffer, uint32_t size) {
	return false;
}

template<class CONFIG>
bool 
CP15<CONFIG> ::
WriteMemory(address_t addr, const void *buffer, uint32_t size) {
	return false;
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> :: 
InitRegs() {
	// initializing id code
	// 31-24: implementer 0x41
	// 23-20: variant (specification) 0x2
	// 19-16: ARM architecture v5TE 0x05
	// 15-4 : part number 0x966
	// 3-0  : silicon revision number (silicon revision specific)
	id_code_reg = 0;
	id_code_reg += ((reg_t)0x41) << 24;
	id_code_reg += ((reg_t)0x2) << 20;
	id_code_reg += ((reg_t)0x05) << 16;
	id_code_reg += ((reg_t)0x966) << 4;
	id_code_reg += (reg_t)silicon_revision_number;

	// initializing TCM size register
	// 31-23: reserved 0x0
	// 22-18: data TCM size (implementation specific)
	// 17-15: reserved 0x0
	// 14   : data TCM absent (implementation specific)
	// 13-11: reserved 0x0
	// 10-6 : instruction TCM size (implementation specific)
	// 5-3  : reserved 0x0
	// 2    : instruction TCM absent (implementation specific)
	// 1-0  : reserved 0x0
	tcm_size_reg = 0;
	if(CONFIG::HAS_DTCM) {
		tcm_size_reg += ((reg_t)CONFIG::DTCM_SIZE) << 18;
		tcm_size_reg += ((reg_t)1) << 14;
	}
	if(CONFIG::HAS_ITCM) {
		tcm_size_reg += ((reg_t)CONFIG::ITCM_SIZE) << 6;
		tcm_size_reg += ((reg_t)1) << 2;
	}
	
	// initializing control register
	// 31-16: reserved 0x0
	// 15   : configure disable loading TBIT 0x0
	// 14   : reserved 0x0
	// 13   : alternate vector select (value of VINITHI)
	// 12   : instruction TCM enable (value of INITRAM)
	// 11-8 : reserved 0xf
	// 7    : endianess CONFIG::ENDIANESS
	// 6-4  : reserved 0x7
	// 3    : BIU write buffer enable 0x0
	// 2    : data TCM enable (value of INITRAM)
	// 1    : alignment fault check enable  0x0
	// 0    : reserved 0x0
	control_reg = 0;
	if((*inherited::GetParent())["arm966es-vinithi"])
		control_reg += ((reg_t)1) << 13;
	if((*inherited::GetParent())["arm966es-initram"])
		control_reg += ((reg_t)1) << 12;
	control_reg += ((reg_t)0x0f) << 8;
	if(CONFIG::ENDIANESS != E_BIG_ENDIAN)
		control_reg += ((reg_t)1) << 7;
	control_reg += ((reg_t)0x07) << 4;
	if((*inherited::GetParent())["arm966es-initram"])
		control_reg += ((reg_t)1) << 2;
	
	// initializing core control register
	// nothing needs to be done
	core_reg = 0;
	
	// initializing trace process identifier register
	// nothing needs to be done
	tpi_reg = 0;
	
	// initializing configuration control register
	// 31-19: reserved 0x0
	// 18   : instruction TCM order 0x0
	// 17   : data TCM order 0x0
	// 16   : AHB instruction prefetch buffer disable 0x1
	// 15-3 : reserved 0x0
	// 2    : nFIQ mask during FIFOFULL 0x0
	// 1    : nIRQ mask during FIFOFULL 0x0
	// 0    : reserved 0x0
	conf_control_reg = 0;
	conf_control_reg += ((reg_t)1) << 16;
	
	// initializing bist control register
	// bist is not implemented set register to 0x0
	bist_control_reg = 0;
	// other bist registers are also set to 0
	ibist_addr_reg = 0;
	ibist_gen_reg = 0;
	dbist_addr_reg = 0;
	dbist_gen_reg = 0;
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
WriteControlReg(reg_t value) {
	reg_t final_value = value;
	reg_t orig_value;
	
	// checking reserved bits
	// 31-16 should be zero
	// 14 should be zero
	// 11-8 should be one
	// 6-4 should be one
	// 0 should be zero
	reg_t zero_mask = 0xffff4001;
	reg_t one_mask = 0x00000770;
	if(zero_mask & value) {
		orig_value = value;
		final_value = value & ~zero_mask;
		if(inherited::logger_import) {
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Trying to set to one reserved bits in the 'Control' register"
				<< " that should be zero, resetting them:" << Endl
				<< " - original value = 0x" << Hex << orig_value << Dec << Endl
				<< " - modified value = 0x" << Hex << final_value << Dec << Endl
				<< EndDebugError;
		}
	}
	if(one_mask & value != one_mask) {
		orig_value = final_value;
		final_value = final_value | one_mask;
		if(inherited::logger_import) {
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Trying to set to zero reserved bits in the 'Control' register"
				<< " that should be one, resetting them:" << Endl
				<< " - original value = 0x" << Hex << orig_value << Dec << Endl
				<< " - modified value = 0x" << Hex << final_value << Dec << Endl
				<< EndDebugError;
		}
	}
	
	if(VerboseWriteReg()) {
		char *endian;
		if((final_value >> 12) & 1) {
			endian = "big-endian";
		} else {
			endian = "little-endian";
		}
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Writing 0x" << Hex << final_value << Dec << " into the"
			<< " 'Control' register:" << Endl
			<< " - configure disable loading bit = "
			<< ((final_value >> 15) & 1) << Endl
			<< " - alternate vector select = "
			<< ((final_value >> 13) & 1) << Endl
			<< " - instruction TCM enable = "
			<< ((final_value >> 12) & 1) << Endl
			<< " - endianness = "
			<< ((final_value >> 7) & 1) 
			<< "(" << endian << ")" << Endl
			<< " - BIU write buffer enable = "
			<< ((final_value >> 3) & 1) << Endl
			<< " - data TCM enable = "
			<< ((final_value >> 2) & 1) << Endl
			<< " - alignment fault check enable = "
			<< ((final_value >> 1) & 1) << Endl
			<< EndDebugInfo;
	}
	control_reg = final_value;
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseAll() {
	return CONFIG::DEBUG_ENABLE && verbose_all && inherited::logger_import; 
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseReadReg() {
	return CONFIG::DEBUG_ENABLE && verbose_read_reg && inherited::logger_import; 
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseWriteReg() {
	return CONFIG::DEBUG_ENABLE && verbose_write_reg && inherited::logger_import; 
}

#undef INLINE

#undef LOCATION

} // end of namespace arm966e_s
} // end of namespace coprocessor
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_CP15_ARM966E_S_CP15_TCC__
