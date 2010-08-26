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
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace coprocessor {
namespace arm966e_s {

using std::cerr;
using std::endl;
using std::hex;
using std::dec;

#ifndef SOCLIB
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebug;
#endif

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" << ":" << __LINE__

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

using unisim::util::endian::E_BIG_ENDIAN;

#ifdef SOCLIB

template<class CONFIG>
CP15<CONFIG> ::
CP15(unsigned int _cp_id,
		CPUCPInterface *_cpu,
		DTCM<CONFIG> *_dtcm,
		ITCM<CONFIG> *_itcm,
		CacheInterface<address_t> *_memory_interface)
	: CPInterface<CONFIG::DEBUG_ENABLE>(_cp_id, _cpu)
	, id_code_reg(0)
	, tcm_size_reg(0)
	, control_reg(0)
	, core_reg(0)
	, tpi_reg(0)
	, conf_control_reg(0)
	, bist_control_reg(0)
	, ibist_addr_reg(0)
	, ibist_gen_reg(0)
	, dbist_addr_reg(0)
	, dbist_gen_reg(0)
	, dtcm(_dtcm)
	, itcm(_itcm)
	, memory_interface(_memory_interface)
	, silicon_revision_number(0)
	, verbose_all(false)
	, verbose_read_reg(false)
	, verbose_write_reg(false)
	, verbose_pr_read(false)
	, verbose_pr_write(false)
	, verbose_debug_read(false)
	, verbose_debug_write(false)
{
	InitRegs();
}	


#else // SOCLIB

template<class CONFIG>
CP15<CONFIG> ::
CP15(const char *name,
		unsigned int _cp_id,
		CPUCPInterface *_cpu,
		DTCM<CONFIG> *_dtcm,
		ITCM<CONFIG> *_itcm,
		CacheInterface<address_t> *_memory_interface,
		Object *parent)
	: Object(name, parent)
	, CPInterface<CONFIG::DEBUG_ENABLE>(_cp_id, _cpu)
	, id_code_reg(0)
	, tcm_size_reg(0)
	, control_reg(0)
	, core_reg(0)
	, tpi_reg(0)
	, conf_control_reg(0)
	, bist_control_reg(0)
	, ibist_addr_reg(0)
	, ibist_gen_reg(0)
	, dbist_addr_reg(0)
	, dbist_gen_reg(0)
	, dtcm(_dtcm)
	, itcm(_itcm)
	, memory_interface(_memory_interface)
	, silicon_revision_number(0)
	, verbose_all(false)
	, verbose_read_reg(false)
	, verbose_write_reg(false)
	, verbose_pr_read(false)
	, verbose_pr_write(false)
	, verbose_debug_read(false)
	, verbose_debug_write(false)
	, param_silicon_revision_number("silicon-revision-number", this, silicon_revision_number)
	, param_verbose_all("verbose-all", this, verbose_all)
	, param_verbose_read_reg("verbose-read-reg", this, verbose_read_reg)
	, param_verbose_write_reg("verbose-write-reg", this, verbose_write_reg)
	, param_verbose_pr_read("verbose-pr-read", this, verbose_pr_read)
	, param_verbose_pr_write("verbose-pr-write", this, verbose_pr_write)
	, param_verbose_debug_read("verbose-debug-read", this, verbose_debug_read)
	, param_verbose_debug_write("verbose-debug-write", this, verbose_debug_write)
	, logger(*this)
{
}

template<class CONFIG>
bool
CP15<CONFIG> ::
Setup() {
	// check verbose parameters
	if(verbose_all) {
		verbose_read_reg = true;
		verbose_write_reg = true;
		verbose_pr_read = true;
		verbose_pr_write = true;
		verbose_debug_read = true;
		verbose_debug_write = true;
	}
	// check parameters
	if(silicon_revision_number > (uint32_t)0x0fff) {
		logger << DebugError << LOCATION
			<< "Revision number should be smaller or equal than 0x0fff"
			<< EndDebugError;
		return false;
	}
	
	InitRegs();

	return true;
}

#endif // SOCLIB

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
	const char *name;
	
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

#ifdef SOCLIB
		
		cerr << "Error("
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "):"
			<< "Trying to read an inexistent register in :" << endl
			<< hex
			<< " - opcode1 = 0x" << opcode1
			<< " - opcode2 = 0x" << opcode2
			<< dec
			<< " - crn = " << crn
			<< " - crm = " << crm
			<< endl;
			
#else // SOCLIB
		
		logger << DebugWarning << LOCATION
			<< "Trying to read an inexistent register in :" << endl
			<< hex
			<< " - opcode1 = 0x" << opcode1
			<< " - opcode2 = 0x" << opcode2
			<< dec
			<< " - crn = " << crn
			<< " - crm = " << crm
			<< EndDebugWarning;
		
#endif // SOCLIB
		
	} else {
		if(VerboseReadReg()) {
			
#ifdef SOCLIB
			
			cerr << "Info("
				<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "):"
				<< "Reading '" << name << "' register (value = 0x"
				<< hex << reg << dec << ")" << endl;
			
#else // SOCLIB
			
			logger << DebugInfo << LOCATION
				<< "Reading '" << name << "' register (value = 0x"
				<< hex << reg << dec << ")" << endl
				<< EndDebugInfo;
			
#endif // SOCLIB
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
#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Operations not supported on this coprocessor: "
		<< " - opcode1 = 0x" << hex << opcode1 << endl
		<< " - opcode2 = 0x" << opcode2 << dec << endl
		<< " - crd = " << crd << endl
		<< " - crn = " << crn << endl
		<< " - crm = " << crm << endl;
	
#else // SOCLIB
		
	logger << DebugError << LOCATION
		<< "Operations not supported on this coprocessor: "
		<< " - opcode1 = 0x" << hex << opcode1 << endl
		<< " - opcode2 = 0x" << opcode2 << dec << endl
		<< " - crd = " << crd << endl
		<< " - crn = " << crn << endl
		<< " - crm = " << crm 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void
CP15<CONFIG> :: 
Load(uint8_t crd,
		reg_t address) {

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Loads not supported on this coprocessor: "
		<< " - crd = " << crd << endl
		<< " - address = 0x" << hex << address << dec << endl;

#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Loads not supported on this coprocessor: "
		<< " - crd = " << crd << endl
		<< " - address = 0x" << hex << address << dec 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void
CP15<CONFIG> :: 
Store(uint8_t crd,
		reg_t address) {
	
#ifdef SOCLIB

	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Stores not supported on this coprocessor: "
		<< " - crd = " << crd << endl
		<< " - address = 0x" << hex << address << dec << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Stores not supported on this coprocessor: "
		<< " - crd = " << crd << endl
		<< " - address = 0x" << hex << address << dec 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

/************************************************************/
/* Specific arm966e_s cp15 methods                    START */
/************************************************************/

template<class CONFIG>
INLINE
typename CONFIG::address_t
CP15<CONFIG> ::
GetExceptionVectorAddr() {
	// read bit 13 of the control register (alternate vector select) 
	// if 0 return 0x0
	// if 1 return 0xffff0000
	if(control_reg & (((uint32_t)1) << 13))
		return (address_t)0xffff0000;
	return (address_t)0x0;
}

/************************************************************/
/* Specific arm966e_s cp15 methods                    END   */
/************************************************************/

/************************************************************/
/* Cache interface methods                            START */
/************************************************************/
/* Note: the only available methods are PrWrite and PrRead  */

template<class CONFIG>
void 
CP15<CONFIG> ::
PrInvalidateBlock(uint32_t set, uint32_t way) {
	
#ifdef SOCLIB

	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrInvalidateBlock) not supported on this coprocessor: "
		<< " - set = " << set << endl
		<< " - way = " << way << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrInvalidateBlock) not supported on this coprocessor: "
		<< " - set = " << set << endl
		<< " - way = " << way 
		<< EndDebugError;

#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrFlushBlock(uint32_t set, uint32_t way) {

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrFlushBlock) not supported on this coprocessor: "
		<< " - set = " << set << endl
		<< " - way = " << way << endl;

#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrFlushBlock) not supported on this coprocessor: "
		<< " - set = " << set << endl
		<< " - way = " << way 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrCleanBlock(uint32_t set, uint32_t way){

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrCleanBlock) not supported on this coprocessor: "
		<< " - set = " << set << endl
		<< " - way = " << way << endl;
	
#else // SOCLIB

	logger << DebugError << LOCATION
		<< "Method (PrCleanBlock) not supported on this coprocessor: "
		<< " - set = " << set << endl
		<< " - way = " << way 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrReset(){

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrReset) not supported on this coprocessor" << endl;

#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrReset) not supported on this coprocessor" 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrInvalidate() {

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrInvalidate) not supported on this coprocessor" << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrInvalidate) not supported on this coprocessor" 
		<< EndDebugError;

#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrInvalidateSet(uint32_t set){

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrInvalidateSet) not supported on this coprocessor: "
		<< endl << " - set = " << set << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrInvalidateSet) not supported on this coprocessor: "
		<< endl << " - set = " << set 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrInvalidateBlock(address_t addr) {

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrInvalidateBlock) not supported on this coprocessor: "
		<< endl << " - addr = 0x" << hex << addr << dec << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrInvalidateBlock) not supported on this coprocessor: "
		<< endl << " - addr = 0x" << hex << addr << dec 
		<< EndDebugError;
	
#endif
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrFlushBlock(address_t addr) {

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrFlushBlock) not supported on this coprocessor: " << endl
		<< " - addr = 0x" << hex << addr << dec << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrFlushBlock) not supported on this coprocessor: "
		<< endl << " - addr = 0x" << hex << addr << dec 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}


template<class CONFIG>
void 
CP15<CONFIG> ::
PrCleanBlock(address_t addr) {
	
#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrFlushBlock) not supported on this coprocessor: " << endl
		<< " - addr = 0x" << hex << addr << dec << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrFlushBlock) not supported on this coprocessor: "
		<< endl << " - addr = 0x" << hex << addr << dec 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrZeroBlock(address_t addr) {

#ifdef SOCLIB
	
	cerr << "Error"
		<< "(" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
		<< "Method (PrZeroBlock) not supported on this coprocessor: " << endl
		<< " - addr = 0x" << hex << addr << dec << endl;
	
#else // SOCLIB
	
	logger << DebugError << LOCATION
		<< "Method (PrZeroBlock) not supported on this coprocessor: "
		<< endl << " - addr = 0x" << hex << addr << dec 
		<< EndDebugError;
	
#endif // SOCLIB
	
	inherited::cpu->CoprocessorStop(inherited::cp_id, -1);
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrWrite(address_t addr, const uint8_t *buffer, uint32_t size) {
	/* First check if the acces if for a TCM memory, if so check that
	 *   the corresponding TCM is enabled, otherwise send the request 
	 *   through the memory interface
	 * If it is not a TCM access then send it to the memory interface
	 */
	if(VerbosePrWrite()) {

#ifdef SOCLIB
		
		cerr << "Info(" 
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
			<< "Write received:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			cerr << " " << (unsigned int)buffer[i];
		cerr << dec << endl;

#else // SOCLIB
		
		logger << DebugInfo << LOCATION
			<< "Write received:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)buffer[i];
		logger << dec << endl;
		
#endif // SOCLIB
		
	}
	if(IsTCMAddress(addr)) {
		/* it is a TCM address */
		if(IsITCMAddress(addr)) {
			/* it is a ITCM address, check that the ITCM is enabled */
			if(ITCMEnabled()) {
				if(VerbosePrWrite())
					
#ifdef SOCLIB					

					cerr << " - sending to ITCM" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to ITCM" 
						<< EndDebugInfo;
				
#endif // SOCLIB
				
				ITCMWrite(addr, buffer, size);
			} else {
				if(VerbosePrWrite())
					
#ifdef SOCLIB

					cerr << " - sending to main memory (ITCM disabled)" << endl;
					
#else // SOCLIB
						
					logger
						<< " - sending to main memory (ITCM disabled)" 
						<< EndDebugInfo;
				
#endif // SOCLIB
				
				MemWrite(addr, buffer, size);
			}
		} else {
			/* it is a DTCM address, check that the DTCM is enabled */
			if(DTCMEnabled()) {
				if(VerbosePrWrite())

#ifdef SOCLIB

					cerr << " - sending to DTCM" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to DTCM" 
						<< EndDebugInfo;
					
#endif
				DTCMWrite(addr, buffer, size);
			} else {
				if(VerbosePrWrite())

#ifdef SOCLIB

					cerr << " - sending to main memory (DTCM disabled)" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to main memory (DTCM disabled)" << endl
						<< EndDebugInfo;
					
#endif // SOCLIB
					
				MemWrite(addr, buffer, size);
			}
		}
	} else {
		if(VerbosePrWrite())
			
#ifdef SOCLIB
			
			cerr << " - sending to main memory" << endl;
		
#else // SOCLIB
		
			logger
				<< " - sending to main memory" << endl
				<< EndDebugInfo;

#endif // SOCLIB
			
		MemWrite(addr, buffer, size);
	}
}

template<class CONFIG>
void 
CP15<CONFIG> ::
PrRead(address_t addr, uint8_t *buffer, uint32_t size) {
	/* First check if the acces if for a TCM memory, if so check that
	 *   the corresponding TCM is enabled, otherwise send the request 
	 *   through the memory interface
	 * If it is not a TCM access then send it to the memory interface
	 */
	if(VerbosePrRead()) {

#ifdef SOCLIB
		
		cerr << "Info(" 
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
			<< "Read received:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl;

#else // SOCLIB
		
		logger << DebugInfo << LOCATION
			<< "Read received:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl;
		
#endif
	}
	if(IsTCMAddress(addr)) {
		/* it is a TCM address */
		if(IsITCMAddress(addr)) {
			/* it is a ITCM address, check that the ITCM is enabled */
			if(ITCMEnabled()) {
				if(VerbosePrRead())
					
#ifdef SOCLIB
					
					cerr << " - sending to ITCM" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to ITCM" 
						<< EndDebugInfo;
					
#endif // SOCLIB
					
				ITCMRead(addr, buffer, size);
			} else {
				if(VerbosePrRead())
					
#ifdef SOCLIB
					
					cerr << " - sending to main memory (ITCM disabled)" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to main memory (ITCM disabled)" 
						<< EndDebugInfo;
					
#endif // SOCLIB
					
				MemRead(addr, buffer, size);
			}
		} else {
			/* it is a DTCM address, check that the DTCM is enabled */
			if(DTCMEnabled()) {
				if(VerbosePrRead())
					
#ifdef SOCLIB
					cerr << " - sending to DTCM" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to DTCM" 
						<< EndDebugInfo;
					
#endif // SOCLIB
					
				DTCMRead(addr, buffer, size);
			} else {
				if(VerbosePrRead())
					
#ifdef SOCLIB
					
					cerr << " - sending to main memory (DTCM disabled)" << endl;
				
#else // SOCLIB
				
					logger
						<< " - sending to main memory (DTCM disabled)" 
						<< EndDebugInfo;
					
#endif // SOCLIB
					
				MemRead(addr, buffer, size);
			}
		}
	} else {
		if(VerbosePrRead())
			
#ifdef SOCLIB
			
			cerr << " - sending to main memory" << endl;
		
#else // SOCLIB
		
			logger
				<< " - sending to main memory" 
				<< EndDebugInfo;
			
#endif // SOCLIB
			
		MemRead(addr, buffer, size);
	}
	if(VerbosePrRead()) {
		
#ifdef SOCLIB
		
		cerr << "Info(" 
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
			<< "Read done:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++) {
			cerr << " " << (unsigned int)buffer[i];
		}
		cerr << endl;
		
#else // SOCLIB
		
		logger << DebugInfo << LOCATION
			<< "Read done:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++) {
			logger << " " << (unsigned int)buffer[i];
		}
		logger
			<< EndDebugInfo;
		
#endif // SOCLIB
		
	}
}

/************************************************************/
/* Cache interface methods                            END   */
/************************************************************/

/************************************************************/
/* Memory interface methods                           START */
/************************************************************/

#ifndef SOCLIB

template<class CONFIG>
bool 
CP15<CONFIG> ::
ReadMemory(uint64_t addr, void *buffer, uint32_t size) {
	bool success = false;
	
	/* First check if the acces if for a TCM memory, if so check that
	 *   the corresponding TCM is enabled, otherwise send the request 
	 *   through the memory interface
	 * If it is not a TCM access then send it to the memory interface
	 */
	if(VerboseDebugRead()) {
		logger << DebugInfo << LOCATION
			<< "Read received:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl;
	}
	if(IsTCMAddress(addr)) {
		/* it is a TCM address */
		if(IsITCMAddress(addr)) {
			/* it is a ITCM address, check that the ITCM is enabled */
			if(ITCMEnabled()) {
				success = itcm->ReadMemory(addr, buffer, size);
			} else {
				if(VerboseDebugRead())
					logger
						<< " - could not send it because memory_import not connected"
						<< EndDebugInfo;
				return false;
			}
		} else {
			/* it is a DTCM address, check that the DTCM is enabled */
			if(DTCMEnabled()) {
				success = dtcm->ReadMemory(addr, buffer, size);
			} else {
				if(VerboseDebugRead())
					logger
						<< " - could not send it because memory_import not connected"
						<< EndDebugInfo;
				return false;
			}
		}
	} else {
		success = inherited::cpu->CoprocessorReadMemory(addr, buffer, size);
	}
	if(VerboseDebugRead()) {
		if(success) {
			logger << DebugInfo << LOCATION
				<< "Read done:" << endl
				<< " - address = 0x" << hex << addr << dec << endl
				<< " - size = " << size << endl
				<< " - data =" << hex;
			for(unsigned int i = 0; i < size; i++) {
				logger << " " << (unsigned int)((uint8_t *)buffer)[i];
			}
			logger << dec
				<< EndDebugInfo;
		} else {
			logger << DebugInfo << LOCATION
				<< "Read failed:" << endl
				<< " - address = 0x" << hex << addr << dec << endl
				<< " - size = " << size
				<< EndDebugInfo;
		}
	}
	return success;
}

template<class CONFIG>
bool 
CP15<CONFIG> ::
WriteMemory(uint64_t addr, const void *buffer, uint32_t size) {
	bool success = false;
	
	/* First check if the acces if for a TCM memory, if so check that
	 *   the corresponding TCM is enabled, otherwise send the request 
	 *   through the memory interface
	 * If it is not a TCM access then send it to the memory interface
	 */
	if(VerboseDebugWrite()) {
		logger << DebugInfo << LOCATION
			<< "Write received:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
			for(unsigned int i = 0; i < size; i++) {
				logger << " " << (unsigned int)((uint8_t *)buffer)[i];
			}
			logger << dec << endl;
	}
	if(IsTCMAddress(addr)) {
		/* it is a TCM address */
		if(IsITCMAddress(addr)) {
			/* it is a ITCM address, check that the ITCM is enabled */
			if(ITCMEnabled()) {
				success = itcm->WriteMemory(addr, buffer, size);
			} else {
				if(VerboseDebugWrite())
					logger
						<< " - could not send it because memory_import not connected"
						<< EndDebugInfo;
				return false;
			}
		} else {
			/* it is a DTCM address, check that the DTCM is enabled */
			if(DTCMEnabled()) {
				success = dtcm->WriteMemory(addr, buffer, size);
			} else {
				if(VerboseDebugWrite())
					logger
						<< " - could not send it because memory_import not connected"
						<< EndDebugInfo;
				return false;
			}
		}
	} else {
		success = inherited::cpu->CoprocessorWriteMemory(addr, buffer, size);
	}
	if(VerboseDebugWrite() && !success) {
		logger << DebugInfo << LOCATION
			<< "Write failed:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++) {
			logger << " " << (unsigned int)((uint8_t *)buffer)[i];
		}
		logger 
			<< EndDebugInfo;
	}
	return success;
}

#endif // SOCLIB

/************************************************************/
/* Memory interface methods                           END   */
/************************************************************/

/************************************************************/
/* Private register methods                           START */
/************************************************************/

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
	} else {
		tcm_size_reg += ((reg_t)1) << 14;
	}
	if(CONFIG::HAS_ITCM) {
		tcm_size_reg += ((reg_t)CONFIG::ITCM_SIZE) << 6;
	} else {
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
	if(inherited::cpu->CoprocessorGetVinithi())
		control_reg += ((reg_t)1) << 13;
	if(inherited::cpu->CoprocessorGetInitram())
		control_reg += ((reg_t)1) << 12;
	control_reg += ((reg_t)0x0f) << 8;
	//if(CONFIG::ENDIANESS != E_BIG_ENDIAN)
	if(inherited::cpu->CoprocessorGetEndianness() != E_BIG_ENDIAN)
		control_reg += ((reg_t)1) << 7;
	control_reg += ((reg_t)0x07) << 4;
	if(inherited::cpu->CoprocessorGetInitram())
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

#ifdef SOCLIB
			
		cerr << "Error(" 
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
			<< "Trying to set to one reserved bits in the 'Control'"
			<< " register that should be zero, resetting them:" << endl
			<< " - original value = 0x" << hex << orig_value << dec << endl
			<< " - modified value = 0x" << hex << final_value << dec 
			<< endl;
			
#else // SOCLIB
			
		logger << DebugError << LOCATION
			<< "Trying to set to one reserved bits in the 'Control' register"
			<< " that should be zero, resetting them:" << endl
			<< " - original value = 0x" << hex << orig_value << dec << endl
			<< " - modified value = 0x" << hex << final_value << dec 
			<< EndDebugError;

#endif // SOCLIB
		
	}
	if(one_mask & value != one_mask) {
		orig_value = final_value;
		final_value = final_value | one_mask;

#ifdef SOCLIB
			
		cerr << "Error(" 
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
			<< "Trying to set to zero reserved bits in the 'Control' register"
			<< " that should be one, resetting them:" << endl
			<< " - original value = 0x" << hex << orig_value << dec << endl
			<< " - modified value = 0x" << hex << final_value << dec << endl;
		
#else // SOCLIB
		
		logger << DebugError << LOCATION
			<< "Trying to set to zero reserved bits in the 'Control' register"
			<< " that should be one, resetting them:" << endl
			<< " - original value = 0x" << hex << orig_value << dec << endl
			<< " - modified value = 0x" << hex << final_value << dec 
			<< EndDebugError;
		
#endif // SOCLIB
		
	}
	
	if(VerboseWriteReg()) {
		const char *endian;
		if((final_value >> 12) & 1) {
			endian = "big-endian";
		} else {
			endian = "little-endian";
		}
		
#ifdef SOCLIB
		
		cerr << "Info(" 
			<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
			<< "Writing 0x" << hex << final_value << dec << " into the"
			<< " 'Control' register:" << endl
			<< " - configure disable loading bit = "
			<< ((final_value >> 15) & 1) << endl
			<< " - alternate vector select = "
			<< ((final_value >> 13) & 1) << endl
			<< " - instruction TCM enable = "
			<< ((final_value >> 12) & 1) << endl
			<< " - endianness = "
			<< ((final_value >> 7) & 1) 
			<< "(" << endian << ")" << endl
			<< " - BIU write buffer enable = "
			<< ((final_value >> 3) & 1) << endl
			<< " - data TCM enable = "
			<< ((final_value >> 2) & 1) << endl
			<< " - alignment fault check enable = "
			<< ((final_value >> 1) & 1) << endl;

#else // SOCLIB
			
		logger << DebugInfo << LOCATION
			<< "Writing 0x" << hex << final_value << dec << " into the"
			<< " 'Control' register:" << endl
			<< " - configure disable loading bit = "
			<< ((final_value >> 15) & 1) << endl
			<< " - alternate vector select = "
			<< ((final_value >> 13) & 1) << endl
			<< " - instruction TCM enable = "
			<< ((final_value >> 12) & 1) << endl
			<< " - endianness = "
			<< ((final_value >> 7) & 1) 
			<< "(" << endian << ")" << endl
			<< " - BIU write buffer enable = "
			<< ((final_value >> 3) & 1) << endl
			<< " - data TCM enable = "
			<< ((final_value >> 2) & 1) << endl
			<< " - alignment fault check enable = "
			<< ((final_value >> 1) & 1) 
			<< EndDebugInfo;
		
#endif // SOCLIB
	}
	control_reg = final_value;
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
DTCMEnabled() {
	if(control_reg & ((reg_t)1 << 2))
		return true;
	return false;
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
ITCMEnabled() {
	if(control_reg & ((reg_t)1 << 12))
		return true;
	return false;	
}

/************************************************************/
/* Private register methods                           START */
/************************************************************/

/************************************************************/
/* Private memory methods                            START */
/************************************************************/

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
IsTCMAddress(address_t addr) {
	return addr < (address_t)0x08000000;
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
IsDTCMAddress(address_t addr) {
	return IsTCMAddress(addr) && addr >= (address_t)0x04000000;
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
IsITCMAddress(address_t addr) {
	return addr < (address_t)0x04000000;
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
MemWrite(address_t addr, const uint8_t *buffer, uint32_t size) {
	memory_interface->PrWrite(addr, buffer, size);
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
MemRead(address_t addr, uint8_t *buffer, uint32_t size) {
	memory_interface->PrRead(addr, buffer, size);
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
DTCMWrite(address_t addr, const uint8_t *buffer, uint32_t size) {
	dtcm->PrWrite(addr, buffer, size);
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
DTCMRead(address_t addr, uint8_t *buffer, uint32_t size) {
	dtcm->PrRead(addr, buffer, size);
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
ITCMWrite(address_t addr, const uint8_t *buffer, uint32_t size) {
	itcm->PrWrite(addr, buffer, size);
}

template<class CONFIG>
INLINE
void
CP15<CONFIG> ::
ITCMRead(address_t addr, uint8_t *buffer, uint32_t size) {
	itcm->PrRead(addr, buffer, size);
}

/************************************************************/
/* Private memory methods                            END    */
/************************************************************/

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseAll() {

#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_all;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_all;
	
#endif // SOCLIB
	
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseReadReg() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_read_reg;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_read_reg;
	
#endif // SOCLIB
	
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseWriteReg() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_write_reg;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_write_reg;
	
#endif // SOCLIB
	
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerbosePrRead() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_pr_read;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_pr_read;
	
#endif // SOCLIB
	
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerbosePrWrite() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_pr_write;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_pr_write;
	
#endif // SOCLIB
	
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseDebugRead() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_debug_read;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_debug_read;
	
#endif // SOCLIB
	
}

template<class CONFIG>
INLINE
bool
CP15<CONFIG> ::
VerboseDebugWrite() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_debug_write;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && verbose_debug_write;
	
#endif // SOCLIB
}

#undef INLINE

#ifndef SOCLIB

#undef LOCATION

#endif // SOCLIB

} // end of namespace arm966e_s
} // end of namespace coprocessor
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_CP15_ARM966E_S_CP15_TCC__
