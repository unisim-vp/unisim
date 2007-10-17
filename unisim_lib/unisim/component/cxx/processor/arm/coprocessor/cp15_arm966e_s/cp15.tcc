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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_CP15_ARM966E_S_CP15_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_CP15_ARM966E_S_CP15_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace coprocessor {
namespace cp15_arm966e_s {

template<class CONFIG>
CP15<CONFIG> ::
CP15() {
	
}

template<class CONFIG>
const char *
CP15<CONFIG> ::
Name() {
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
}

template<class CONFIG>
void  
CP15<CONFIG> ::
WriteRegister(uint8_t opcode1, 
		uint8_t opcode2, 
		uint8_t crn, 
		uint8_t crm, 
		reg_t value) {
}

} // end of namespace cp15_arm966e_s
} // end of namespace coprocessor
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_CP15_ARM966E_S_CP15_TCC__
