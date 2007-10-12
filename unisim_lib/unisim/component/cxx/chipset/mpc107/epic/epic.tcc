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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_EPIC_TCC__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_EPIC_TCC__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace epic {

using unisim::service::interfaces::Logger;
using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;

#define LOCATION File << __FILE__ << Function << __FUNCTION__ << Line << __LINE__
	
template <class PHYSICAL_ADDR, 
	bool DEBUG>
EPIC<PHYSICAL_ADDR, DEBUG> ::
EPIC(const char *name, Object *parent) :
	Object(name, parent),
	Service<Memory<PHYSICAL_ADDR> >(name, parent),
	Client<Logger>(name, parent),
	memory_export("memory_export", this),
	logger_import("logger_import", this),
	pending_reg(0),
	regs(),
	inservice_reg() {
} 

template <class PHYSICAL_ADDR, 
	bool DEBUG>
EPIC<PHYSICAL_ADDR, DEBUG> ::
~EPIC() {
} 

template <class PHYSICAL_ADDR, 
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
Setup() {
	return true;
} 

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
Reset() {
} 

template <class PHYSICAL_ADDR, 
	bool DEBUG>
uint32_t 
EPIC<PHYSICAL_ADDR, DEBUG> ::
ReadMemoryIACK() {
	uint32_t irq;
	uint32_t vpr;
	
	/* Return the interrupt vector corresponding to the highest priority pending
	 *   interrupt. Other actions:
	 * - the associated bit in the IPR (irq_pending_reg) is cleared (if it is 
	 *     configured as edge-sensitive)
	 * - the ISR (inservice_reg) is updated
	 * - the int signal is negated
	 * If there is no interrupt pending returns the spurious vector value.
	 */
	/* check if there is at least an interruption in the irq_pending_reg, if 
	 *   not return the spurious vector */
	if(pending_reg == 0)
		return regs.svr;
	/* get the irq from the irq_selector. if there is no irq the return the 
	 *   spurious vector */
	//irq = irq_selector;
	irq = GetHighestIRQ();
	if(irq == 0)
		return regs.svr;
	/* get the vpr from the corresponding irq */
	vpr = GetVPR(irq);
	/* check the priority of the irq, if smaller than the processor or current
	 *   irq in the inservice_reg, return the spurious vector */
	uint32_t prio = GetPriority(vpr);
	if(prio <= GetProcessorPriority())
		return regs.svr;
	if(inservice_reg.Priority())
		if(prio <= inservice_reg.Priority())
			return regs.svr;
	return GetVector(vpr);
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size) {
	string name;
	uint32_t data = 0;
	bool handled = false;
	
	if(size != sizeof(uint8_t)) {
		for(uint32_t i = 0; i < size; i++) {
			if(!ReadMemory(addr + i, &((uint8_t *)buffer)[i], sizeof(uint8_t)))
				return false;
		}
		return true;
	}

	PHYSICAL_ADDR base_addr = addr & ~((PHYSICAL_ADDR)0x03);
	PHYSICAL_ADDR offset = addr & (PHYSICAL_ADDR)0x03;
	
	switch(base_addr & ADDR_MASK) {
	case Registers::FRR : name = Registers::FRR_NAME; break;
	case Registers::GCR : 
		handled = true; 
		data = regs.gcr;
		name = Registers::GCR_NAME; 
		break;
	case Registers::EICR : 
		handled = true;
		data = regs.eicr;
		name = Registers::EICR_NAME; 
		break;
	case Registers::EVI : name = Registers::EVI_NAME; break;
	case Registers::PI : name = Registers::PI_NAME; break;
	case Registers::SVR : name = Registers::SVR_NAME; break;
	case Registers::TFRR : name = Registers::TFRR_NAME; break;
	case Registers::GTCCR0 : 
		handled = true;
		data = regs.gtccr[0];
		name = Registers::GTCCR0_NAME; 
		break;
	case Registers::GTBCR0 : 
		handled = true;
		data = regs.gtbcr[0];
		name = Registers::GTBCR0_NAME; 
		break;
	case Registers::GTVPR0 : 
		handled = true;
		data = regs.gtvpr[0];
		name = Registers::GTVPR0_NAME; 
		break;
	case Registers::GTDR0 : name = Registers::GTDR0_NAME; break;
	case Registers::GTCCR1 :
		handled = true;
		data = regs.gtccr[1]; 
		name = Registers::GTCCR1_NAME; 
		break;
	case Registers::GTBCR1 : 
		handled = true;
		data = regs.gtbcr[1];
		name = Registers::GTBCR1_NAME; 
		break;
	case Registers::GTVPR1 : 
		handled = true;
		data = regs.gtvpr[1];
		name = Registers::GTVPR1_NAME; 
		break;
	case Registers::GTDR1 : name = Registers::GTDR1_NAME; break;
	case Registers::GTCCR2 : 
		handled = true;
		data = regs.gtccr[2];
		name = Registers::GTCCR2_NAME; 
		break;
	case Registers::GTBCR2 : 
		handled = true;
		data = regs.gtbcr[2];
		name = Registers::GTBCR2_NAME; 
		break;
	case Registers::GTVPR2 : 
		handled = true;
		data = regs.gtvpr[2];
		name = Registers::GTVPR2_NAME; 
		break;
	case Registers::GTDR2 : name = Registers::GTDR2_NAME; break;
	case Registers::GTCCR3 : 
		handled = true;
		data = regs.gtccr[3];
		name = Registers::GTCCR3_NAME; 
		break;
	case Registers::GTBCR3 : 
		handled = true;
		data = regs.gtbcr[3];
		name = Registers::GTBCR3_NAME; 
		break;
	case Registers::GTVPR3 : 
		handled = true;
		data = regs.gtvpr[3];
		name = Registers::GTVPR3_NAME; 
		break;
	case Registers::GTDR3 : name = Registers::GTDR3_NAME; break;
	case Registers::IVPR0 :
//	case Registers::SVPR0 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[0];
			name = Registers::IVPR0_NAME;
		} else {
			data = regs.svpr[0];
			name = Registers::SVPR0_NAME;
		} 
		break;
	case Registers::IDR0 : name = Registers::IDR0_NAME; break;
//	case Registers::SDR0 : name = Registers::SDR0_NAME; break;
	case Registers::IVPR1 : 
//	case Registers::SVPR1 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[1];
			name = Registers::IVPR1_NAME;
		} else {
			data = regs.svpr[1];
			name = Registers::SVPR1_NAME;
		} 
		break;
	case Registers::IDR1 : name = Registers::IDR1_NAME; break;
//	case Registers::SDR1 : name = Registers::SDR1_NAME; break;
	case Registers::IVPR2 : 
//	case Registers::SVPR2 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[2];
			name = Registers::IVPR2_NAME;
		} else {
			data = regs.svpr[2];
			name = Registers::SVPR2_NAME;
		} 
		break;
	case Registers::IDR2 : name = Registers::IDR2_NAME; break;
//	case Registers::SDR2 : name = Registers::SDR2_NAME; break;
	case Registers::IVPR3 : 
//	case Registers::SVPR3 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[3];
			name = Registers::IVPR3_NAME;
		} else {
			data = regs.svpr[3];
			name = Registers::SVPR3_NAME;
		} 
		break;
	case Registers::IDR3 : name = Registers::IDR3_NAME; break;
//	case Registers::SDR3 : name = Registers::SDR3_NAME; break;
	case Registers::IVPR4 : 
//	case Registers::SVPR4 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[4];
			name = Registers::IVPR4_NAME;
		} else {
			data = regs.svpr[4];
			name = Registers::SVPR4_NAME;
		} 
		break;
	case Registers::IDR4 : name = Registers::IDR4_NAME; break;
//	case Registers::SDR4 : name = Registers::SDR4_NAME; break;
	case Registers::SVPR5 : 
		handled = true;
		data = regs.svpr[5];
		name = Registers::SVPR5_NAME;
		break;
	case Registers::SDR5 : name = Registers::SDR5_NAME; break;
	case Registers::SVPR6 : 
		handled = true;
		data = regs.svpr[6];
		name = Registers::SVPR6_NAME;
		break;
	case Registers::SDR6 : name = Registers::SDR6_NAME; break;
	case Registers::SVPR7 : 
		handled = true;
		data = regs.svpr[7];
		name = Registers::SVPR7_NAME;
		break;
	case Registers::SDR7 : name = Registers::SDR7_NAME; break;
	case Registers::SVPR8 : 
		handled = true;
		data = regs.svpr[8];
		name = Registers::SVPR8_NAME;
		break;
	case Registers::SDR8 : name = Registers::SDR8_NAME; break;
	case Registers::SVPR9 : 
		handled = true;
		data = regs.svpr[9];
		name = Registers::SVPR9_NAME;
		break;
	case Registers::SDR9 : name = Registers::SDR9_NAME; break;
	case Registers::SVPR10 :
		handled = true;
		data = regs.svpr[10];
		name = Registers::SVPR10_NAME;
		break;
	case Registers::SDR10 : name = Registers::SDR10_NAME; break;
	case Registers::SVPR11 :
		handled = true;
		data = regs.svpr[11];
		name = Registers::SVPR11_NAME;
		break;
	case Registers::SDR11 : name = Registers::SDR11_NAME; break;
	case Registers::SVPR12 :
		handled = true;
		data = regs.svpr[12];
		name = Registers::SVPR12_NAME;
		break;
	case Registers::SDR12 : name = Registers::SDR12_NAME; break;
	case Registers::SVPR13 :
		handled = true;
		data = regs.svpr[13];
		name = Registers::SVPR13_NAME;
		break;
	case Registers::SDR13 : name = Registers::SDR13_NAME; break;
	case Registers::SVPR14 :
		handled = true;
		data = regs.svpr[14];
		name = Registers::SVPR14_NAME;
		break;
	case Registers::SDR14 : name = Registers::SDR14_NAME; break;
	case Registers::SVPR15 :
		handled = true;
		data = regs.svpr[15];
		name = Registers::SVPR15_NAME;
		break;
	case Registers::SDR15 : name = Registers::SDR15_NAME; break;
	case Registers::IIVPR0 : name = Registers::IIVPR0_NAME; break;
	case Registers::IIDR0 : name = Registers::IIDR0_NAME; break;
	case Registers::IIVPR1 : name = Registers::IIVPR1_NAME; break;
	case Registers::IIDR1 : name = Registers::IIDR1_NAME; break;
	case Registers::IIVPR2 : name = Registers::IIVPR2_NAME; break;
	case Registers::IIDR2 : name = Registers::IIDR2_NAME; break;
	case Registers::IIVPR3 : name = Registers::IIVPR3_NAME; break;
	case Registers::IIDR3 : name = Registers::IIDR3_NAME; break;
	case Registers::PCTPR : 
		handled = true;
		data = regs.pctpr;
		name = Registers::PCTPR_NAME; 
		break;
	case Registers::IACK :
		handled = true;
		data = ReadMemoryIACK(); 
		name = Registers::IACK_NAME; 
		break;
	case Registers::EOI : 
		handled = true;
		data = regs.eoi;
		name = Registers::EOI_NAME; 
		break;
	default:
		if(logger_import)
			(*logger_import) << DebugWarning << LOCATION
				<< "Trying to read EPIC register at reserved address 0x" << Hex << addr << Dec
				<< ", reading 0" << Endl
				<< EndDebugWarning;
		*(uint8_t *)buffer = 0;
		return false;
		break;
	}
	
	if(handled) {
		data = data >> (offset * 8);
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "Reading register " << name << " (address = 0x"
				<< Hex << addr << Dec
				<< ", size = " << size << " bytes"
				<< ", data = " << data << ")"
				<< Endl << EndDebugInfo;
		*(uint8_t *)buffer = (uint8_t)data;
		return true;
	}
	if(DEBUG && logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "TODO: trying to read register " << name << " (address 0x"
			<< Hex << addr << Dec
			<< ") of " << size << " bytes"
			<< Endl << EndDebugWarning;
	*(uint8_t *)buffer = 0;
	return false;
} 

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryGCR(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryEICR(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryGTVPR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryGTBCR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryVPR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryPCTPR(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryIACK(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryEOI(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryEVI(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register EVI with data 0x" 
			<< Hex << data << Dec << ", resetting it to 0x"
			<< Hex << regs.evi << Dec << Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryPI(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemorySVR(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryTFRR(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryGTCCR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryGTDR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryDR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryIIVPR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemoryIIDR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to modify register value through the memory interface, ignored"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemory(PHYSICAL_ADDR addr, uint32_t data) {
	string name;
	bool handled = false;
	
	switch(addr & ADDR_MASK) {
	case Registers::GCR : 
		handled = true;
		WriteMemoryGCR(data);
		break;
	case Registers::EICR :
		handled = true;
		WriteMemoryEICR(data);
		break;
	case Registers::EVI :
		handled = true;
		WriteMemoryEVI(data); 
		break;
	case Registers::PI :
		handled = true;
		WriteMemoryPI(data); 
		break;
	case Registers::SVR :
		handled = true;
		WriteMemorySVR(data); 
		break;
	case Registers::TFRR :
		handled = true;
		WriteMemoryTFRR(data); 
		break;
	case Registers::GTVPR0 :
		handled = true;
		WriteMemoryGTVPR(data, 0);
		break;
	case Registers::GTVPR1 :
		handled = true;
		WriteMemoryGTVPR(data, 1);
		break;
	case Registers::GTVPR2 :
		handled = true;
		WriteMemoryGTVPR(data, 2);
		break;
	case Registers::GTVPR3 :
		handled = true;
		WriteMemoryGTVPR(data, 3);
		break;
	case Registers::GTBCR0 :
		handled = true;
		WriteMemoryGTBCR(data, 0);
		break;
	case Registers::GTBCR1 :
		handled = true;
		WriteMemoryGTBCR(data, 1);
		break;
	case Registers::GTBCR2 :
		handled = true;
		WriteMemoryGTBCR(data, 2);
		break;
	case Registers::GTBCR3 :
		handled = true;
		WriteMemoryGTBCR(data, 3);
		break;
	case Registers::GTCCR0 :
		handled = true;
		WriteMemoryGTCCR(data, 0); 
		break;
	case Registers::GTCCR1 :
		handled = true;
		WriteMemoryGTCCR(data, 1); 
		break;
	case Registers::GTCCR2 :
		handled = true;
		WriteMemoryGTCCR(data, 2); 
		break;
	case Registers::GTCCR3 :	
		handled = true;
		WriteMemoryGTCCR(data, 3); 
		break;
	case Registers::GTDR0 :
		handled = true;
		WriteMemoryGTDR(data, 0); 
		break;
	case Registers::GTDR1 :
		handled = true;
		WriteMemoryGTDR(data, 1); 
		break;
	case Registers::GTDR2 :
		handled = true;
		WriteMemoryGTDR(data, 2); 
		break;
	case Registers::GTDR3 :
		handled = true;
		WriteMemoryGTDR(data, 3); 
		break;
	case Registers::SDR0 : // and IDR0
		handled = true;
		WriteMemoryDR(data, 0); 
		break;
	case Registers::SDR1 : // and IDR1
		handled = true;
		WriteMemoryDR(data, 1); 
		break;
	case Registers::SDR2 : // and IDR2
		handled = true;
		WriteMemoryDR(data, 2); 
		break;
	case Registers::SDR3 : // and IDR3
		handled = true;
		WriteMemoryDR(data, 3); 
		break;
	case Registers::SDR4 : // and IDR4
		handled = true;
		WriteMemoryDR(data, 4); 
		break;
	case Registers::SDR5 :
		handled = true;
		WriteMemoryDR(data, 5); 
		break;
	case Registers::SDR6 :
		handled = true;
		WriteMemoryDR(data, 6); 
		break;
	case Registers::SDR7 :
		handled = true;
		WriteMemoryDR(data, 7); 
		break;
	case Registers::SDR8 :
		handled = true;
		WriteMemoryDR(data, 8); 
		break;
	case Registers::SDR9 :
		handled = true;
		WriteMemoryDR(data, 9); 
		break;
	case Registers::SDR10 :
		handled = true;
		WriteMemoryDR(data, 10); 
		break;
	case Registers::SDR11 :
		handled = true;
		WriteMemoryDR(data, 11); 
		break;
	case Registers::SDR12 :
		handled = true;
		WriteMemoryDR(data, 12); 
		break;
	case Registers::SDR13 :
		handled = true;
		WriteMemoryDR(data, 13); 
		break;
	case Registers::SDR14 :
		handled = true;
		WriteMemoryDR(data, 14); 
		break;
	case Registers::SDR15 :
		handled = true;
		WriteMemoryDR(data, 15); 
		break;
	case Registers::SVPR0 : // and IVPR0
		handled = true;
		WriteMemoryVPR(data, 0);
		break;
	case Registers::SVPR1 : // and IVPR1
		handled = true;
		WriteMemoryVPR(data, 1);
		break;
	case Registers::SVPR2 : // and IVPR2
		handled = true;
		WriteMemoryVPR(data, 2);
		break;
	case Registers::SVPR3 : // and IVPR3
		handled = true;
		WriteMemoryVPR(data, 3);
		break;
	case Registers::SVPR4 :
		handled = true;
		WriteMemoryVPR(data, 4);
		break;
	case Registers::SVPR5 :
		handled = true;
		WriteMemoryVPR(data, 5);
		break;
	case Registers::SVPR6 :
		handled = true;
		WriteMemoryVPR(data, 6);
		break;
	case Registers::SVPR7 :
		handled = true;
		WriteMemoryVPR(data, 7);
		break;
	case Registers::SVPR8 :
		handled = true;
		WriteMemoryVPR(data, 8);
		break;
	case Registers::SVPR9 :
		handled = true;
		WriteMemoryVPR(data, 9);
		break;
	case Registers::SVPR10 :
		handled = true;
		WriteMemoryVPR(data, 10);
		break;
	case Registers::SVPR11 :
		handled = true;
		WriteMemoryVPR(data, 11);
		break;
	case Registers::SVPR12 :
		handled = true;
		WriteMemoryVPR(data, 12);
		break;
	case Registers::SVPR13 :
		handled = true;
		WriteMemoryVPR(data, 13);
		break;
	case Registers::SVPR14 :
		handled = true;
		WriteMemoryVPR(data, 14);
		break;
	case Registers::SVPR15 :
		handled = true;
		WriteMemoryVPR(data, 15);
		break;
	case Registers::IIVPR0 :
		handled = true;
		WriteMemoryIIVPR(data, 0); 
		break;
	case Registers::IIVPR1 :
		handled = true;
		WriteMemoryIIVPR(data, 1); 
		break;
	case Registers::IIVPR2 :
		handled = true;
		WriteMemoryIIVPR(data, 2); 
		break;
	case Registers::IIVPR3 :
		handled = true;
		WriteMemoryIIVPR(data, 3); 
		break;
	case Registers::IIDR0 :
		handled = true;
		WriteMemoryIIDR(data, 0); 
		break;
	case Registers::IIDR1 :
		handled = true;
		WriteMemoryIIDR(data, 1); 
		break;
	case Registers::IIDR2 :
		handled = true;
		WriteMemoryIIDR(data, 2); 
		break;
	case Registers::IIDR3 :
		handled = true;
		WriteMemoryIIDR(data, 4); 
		break;
	case Registers::PCTPR :
		handled = true;
		WriteMemoryPCTPR(data);
		break;
	case Registers::IACK :
		handled = true;
		WriteMemoryEICR(data);
		break;
	case Registers::EOI :
		handled = true;
		WriteMemoryEOI(data);
		break;
	}

	if(!handled) {
		if(DEBUG && logger_import) {
			(*logger_import) << DebugWarning << LOCATION
				<< "TODO: trying to write address 0x"
				<< Hex << addr << Dec
				<< " with data 0x" << Hex << data << Dec
				<< Endl << EndDebugWarning;
		}
	}

}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size) {
	for(uint32_t i = 0; i < size; i++) {
		PHYSICAL_ADDR cur_addr = addr + i;
		if(writememory_address != (cur_addr & ~(PHYSICAL_ADDR)0x03)) {
			writememory_address = cur_addr & ~(PHYSICAL_ADDR)0x03;
			writememory_data = 0;
			writememory_mask = 0;
		}
		PHYSICAL_ADDR offset =
			cur_addr & (PHYSICAL_ADDR)0x03;
		writememory_data = ((uint8_t *)buffer)[i] << offset;
		writememory_mask |= WRITEMEMORY_MASK_BYTE << offset;
		if(writememory_mask == WRITEMEMORY_MASK_FULL) {
			WriteMemory(writememory_address, writememory_data);
			writememory_mask = 0;
		}
	}
	return true;
	
} 

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
TimeSignal(uint64_t cycles) {
	bool activated = false;
	/* update global timers and check if a interrupt must be raised */
	for(unsigned int i = 0; i < 4; i++) {
		if((regs.gtbcr[i] & (uint32_t)0x80000000) == 0) {
			activated = true;
			/* decrement counter */
			regs.gtccr[i] = (regs.gtccr[i] & 0x80000000) |
				((regs.gtccr[i] - 1) & 0x7fffffff);
			/* check if an interruption must be raised */
			if((regs.gtccr[i] & (uint32_t)0x7fffffff) == 0) {
				/* toggle T bit */
				if(regs.gtccr[i] & (uint32_t)0x80000000)
					regs.gtccr[i] = 0;
				else
					regs.gtccr[i] = (uint32_t)0x80000000;
				/* signal the global timer interruption */
				SetGlobalTimerIRQ(i);
			}
		}
	}
	if(!activated) {
		DeactivateSDRAMClock();
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
SetIRQ(unsigned int id, unsigned int serial_id) {
	/* check if the EPIC is configured in pass-through mode,
	 *   if so, bypass the interrupt signal to the output directly
	 *   without doing anything */
	if(PassthroughMode()) {
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "Received IRQ on port " << id
				<< Endl << EndDebugInfo;
		if(id != 0) {
			if(logger_import) {
				(*logger_import) << DebugError
					<< LOCATION
					<< "Received a wrong irq id (id = " << id 
					<< ", serial id = " << serial_id << ") when working in passthrough mode "
					<< "(id should be 0), stopping simulation"
					<< Endl << EndDebugError;
			}
			StopSimulation();
		}
		SetINT();
		return;
	}
	/* we are in mixed mode */
	/* check if the EPIC is configured in direct mode or in serial mode,
	 *   if in serial mode id should be equal to 0 and the serial_id will
	 *   be used to decide were the interrupt comes from.
	 *   If in direct mode the serial_id will be ignored.
	 */
	if(DirectMode()) {
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "Received IRQ on port " << id
				<< Endl << EndDebugInfo;
		/* direct mode */
		/* check that id isn't bigger than 3 (only 4 direct irqs) */
		if(id > 3) {
			if(logger_import) {
				(*logger_import) << DebugError
					<< LOCATION
					<< "Received a wrong irq id (id = " << id << "), "
					<< "stopping simulation"
					<< Endl << EndDebugError;
			}
			StopSimulation();
			return;
		}
		SetDirectIRQ(id);
	} else {
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "Received IRQ on port " << id << " (serial_id = " << serial_id << ")"
				<< Endl << EndDebugInfo;
		/* Serial mode */
		/* check that id is 0 (only port for serial irqs), and serial_id
		 *   is smaller than 16 */
		if(id == 0 && serial_id > 15) {
			if(logger_import) {
				(*logger_import) << DebugError
					<< LOCATION
					<< "Received a wrong irq serial_id (serial_id = " 
					<< serial_id << "), "
					<< "stopping simulation"
					<< Endl << EndDebugError;
			}
			StopSimulation();
			return;
		}
		SetSerialIRQ(serial_id);
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
UnsetIRQ(unsigned int id, unsigned int serial_id) {
	/* check if the EPIC is configured in pass-through mode,
	 *   if so, bypass the interrupt signal to the output directly
	 *   without doing anything */
	if(PassthroughMode()) {
		if(id != 0) {
			if(logger_import) {
				(*logger_import) << DebugError
					<< LOCATION
					<< "Received a wrong irq id (id = " 
					<< serial_id << ") when working in passthrough mode "
					<< "(id should be 0), stopping simulation"
					<< Endl << EndDebugError;
			}
			StopSimulation();
		}
		UnsetINT();
		return;
	}
	/* we are in mixed mode */
	/* check if the EPIC is configured in direct mode or in serial mode,
	 *   if in serial mode id should be equal to 0 and the serial_id will
	 *   be used to decide were the interrupt comes from.
	 *   If in direct mode the serial_id will be ignored.
	 */
	if(DirectMode()) {
		/* direct mode */
		/* check that id isn't bigger than 3 (only 4 direct irqs) */
		if(id > 3) {
			if(logger_import) {
				(*logger_import) << DebugError
					<< LOCATION
					<< "Received a wrong irq id (id = " << id << "), "
					<< "stopping simulation"
					<< Endl << EndDebugError;
			}
			StopSimulation();
			return;
		}
	} else {
		/* Serial mode */
		/* check that id is 0 (only port for serial irqs), and serial_id
		 *   is smaller than 16 */
		if(id == 0 && serial_id > 15) {
			if(logger_import) {
				(*logger_import) << DebugError
					<< LOCATION
					<< "Received a wrong irq serial_id (serial_id = " 
					<< serial_id << "), "
					<< "stopping simulation"
					<< Endl << EndDebugError;
			}
			StopSimulation();
			return;
		}
	}
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetVPRFromIRQMask(uint32_t mask) {
	uint32_t vpr;
	switch(mask) {
	case IRQ_T0:
		vpr = regs.gtvpr[0];
		break;
	case IRQ_T1:
		vpr = regs.gtvpr[1];
		break;
	case IRQ_T2:
		vpr = regs.gtvpr[2];
		break;
	case IRQ_T3:
		vpr = regs.gtvpr[3];
		break;
	case IRQ_DMA0:
		vpr = regs.iivpr[0];
		break;
	case IRQ_DMA1:
		vpr = regs.iivpr[1];
		break;
	case IRQ_MU:
		vpr = regs.iivpr[2];
		break;
	case IRQ_I2C:
		vpr = regs.iivpr[3];
		break;
	case IRQ_0:
		if(SerialMode())
			vpr = regs.svpr[0];
		else
			vpr = regs.ivpr[0];
		break;
	case IRQ_1:
		if(SerialMode())
			vpr = regs.svpr[1];
		else
			vpr = regs.ivpr[1];
		break;
	case IRQ_2:
		if(SerialMode())
			vpr = regs.svpr[2];
		else
			vpr = regs.ivpr[2];
		break;
	case IRQ_3:
		if(SerialMode())
			vpr = regs.svpr[3];
		else
			vpr = regs.ivpr[3];
		break;
	case IRQ_4:
		if(SerialMode())
			vpr = regs.svpr[4];
		else
			vpr = regs.ivpr[4];
		break;
	case IRQ_5:
		vpr = regs.svpr[5];
		break;
	case IRQ_6:
		vpr = regs.svpr[6];
		break;
	case IRQ_7:
		vpr = regs.svpr[7];
		break;
	case IRQ_8:
		vpr = regs.svpr[8];
		break;
	case IRQ_9:
		vpr = regs.svpr[9];
		break;
	case IRQ_10:
		vpr = regs.svpr[10];
		break;
	case IRQ_11:
		vpr = regs.svpr[11];
		break;
	case IRQ_12:
		vpr = regs.svpr[12];
		break;
	case IRQ_13:
		vpr = regs.svpr[13];
		break;
	case IRQ_14:
		vpr = regs.svpr[14];
		break;
	case IRQ_15:
		vpr = regs.svpr[15];
		break;
	default:
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "Unknown mask received (mask = 0x" << Hex << mask << Dec
				<< Endl << DebugError;
		StopSimulation();
	}
	return vpr;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetGlobalTimerIRQMask(uint32_t id) {
	uint32_t mask = 0;
	switch(id) {
	case 0: mask = IRQ_T0; break;
	case 1: mask = IRQ_T1; break;
	case 2: mask = IRQ_T2; break;
	case 3: mask = IRQ_T3; break;
	}
	return mask;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetDirectIRQMask(uint32_t id) {
	uint32_t mask = 0;
	switch(id) {
	case 0: mask = IRQ_0; break;
	case 1: mask = IRQ_1; break;
	case 2: mask = IRQ_2; break;
	case 3: mask = IRQ_3; break;
	}
	return mask;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetSerialIRQMask(uint32_t id) {
	uint32_t mask = 0;
	switch(id) {
	case 0: mask = IRQ_0; break;
	case 1: mask = IRQ_1; break;
	case 2: mask = IRQ_2; break;
	case 3: mask = IRQ_3; break;
	case 4: mask = IRQ_4; break;
	case 5: mask = IRQ_5; break;
	case 6: mask = IRQ_6; break;
	case 7: mask = IRQ_7; break;
	case 8: mask = IRQ_8; break;
	case 9: mask = IRQ_9; break;
	case 10: mask = IRQ_10; break;
	case 11: mask = IRQ_11; break;
	case 12: mask = IRQ_12; break;
	case 13: mask = IRQ_13; break;
	case 14: mask = IRQ_14; break;
	case 15: mask = IRQ_15; break;
	}
	return mask;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
SetGlobalTimerIRQ(unsigned int id) {
	uint32_t vpr;
	
	/* get the vpr */
	vpr = regs.gtvpr[id];
	/* set the activity bit */
	if(GetActivity(vpr)) {
		/* nothing needs to be done, the irq was already set up */
		return;
	}
	SetActivity(vpr);
	if(!GetActivity(vpr)) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< LOCATION
				<< "Error while trying to set the activity bit of gtvpr["
				<< id << "]"
				<< Endl << EndDebugError;
		}
		StopSimulation();
		return;
	}
	
	/* set corresponding bit in the pending_reg register */
	uint32_t mask = GetGlobalTimerIRQMask(id);
	pending_reg = pending_reg | mask;
	
	CheckInterruptions();	
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
SetDirectIRQ(unsigned int id) {
	uint32_t vpr;

	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Handling direct IRQ received on port " << id
			<< Endl << EndDebugInfo;

	/* get the vpr */
	vpr = regs.ivpr[id];
	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ivpr[" << id << "] = 0x" << Hex << vpr << Dec
			<< Endl << EndDebugInfo;
	/* set the activity bit */
	if(GetActivity(vpr)) {
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "nothing needs to be done, the irq was already set up"
				<< Endl << EndDebugInfo;
		/* nothing needs to be done, the irq was already set up */
		return;
	}
	SetActivity(vpr);
	if(!GetActivity(vpr)) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< LOCATION
				<< "Error while trying to set the activity bit of ivpr["
				<< id << "]"
				<< Endl << EndDebugError;
		}
		StopSimulation();
		return;
	}
	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "activity bit set (ivpr[" << id << "] = 0x" << Hex << vpr << Dec << ")"
			<< "(previous ivprt[" << id << "] = 0x" << Hex << regs.ivpr[id] << Dec << ")"
			<< Endl << EndDebugInfo;
	regs.ivpr[id] = vpr;
	/* set corresponding bit in the pending_reg register */
	uint32_t mask = GetDirectIRQMask(id);
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating pending register with the mask correspondent to ivpr[" << id << "] = 0x"
			<< Hex << regs.ivpr[id] << Dec << ":" << Endl
			<< "- mask = 0x" << Hex << mask << Dec << Endl
			<< "- previous pending_reg = 0x" << Hex << pending_reg << Dec << Endl;
	pending_reg = pending_reg | mask;
	if(DEBUG && logger_import)
		(*logger_import)
			<< "- new pending_reg = 0x" << Hex << pending_reg << Dec << Endl
			<< EndDebugInfo;
	CheckInterruptions();
	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Finished handling direct IRQ received on port " << id
			<< Endl << EndDebugInfo;

}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
SetSerialIRQ(unsigned int id) {
	uint32_t vpr;
	
	/* get the vpr */
	vpr = regs.svpr[id];
	/* set the activity bit */
	if(GetActivity(vpr)) {
		/* nothing needs to be done, the irq was already set up */
		return;
	}
	SetActivity(vpr);
	if(!GetActivity(vpr)) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< LOCATION
				<< "Error while trying to set the activity bit of ivpr["
				<< id << "]"
				<< Endl << EndDebugError;
		}
		StopSimulation();
		return;
	}
	/* set corresponding bit in the pending_reg register */
	uint32_t mask = GetSerialIRQMask(id);
	pending_reg = pending_reg | mask;
	
	CheckInterruptions();
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
PassthroughMode() {
	if(regs.gcr & (uint32_t)0x20000000) return false;
	return true;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
MixedMode() {
	return !PassthroughMode();
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
SerialMode() {
	if(regs.eicr & (uint32_t)0x08000000) return true;
	return false;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
DirectMode() {
	return !SerialMode();
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
EdgeSensitive(uint32_t reg_val) {
	if(reg_val & (uint32_t)0x00400000) return false;
	return true;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
EdgeSensitive(uint32_t in_irq, uint32_t reg_val) {
	bool edge_sensitive = true;
	
	switch(in_irq) {
	case IRQ_T0: case IRQ_T1: case IRQ_T2: case IRQ_T3:
		break;  
	case IRQ_DMA0: case IRQ_DMA1: case IRQ_MU: case IRQ_I2C:
		break;
	case IRQ_0: case IRQ_1: case IRQ_2: case IRQ_3: 
	case IRQ_4: case IRQ_5: case IRQ_6: case IRQ_7:  
	case IRQ_8: case IRQ_9: case IRQ_10: case IRQ_11: 
	case IRQ_12: case IRQ_13: case IRQ_14: case IRQ_15:
		edge_sensitive = EdgeSensitive(reg_val);
		break; 
	}
	if(reg_val & (uint32_t)0x00400000) return false;
	return true;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
LevelSensitive(uint32_t reg_val) {
	return !EdgeSensitive(reg_val);
}


/* check if there are interruptions in the pending_reg that should be notified
 *   to the processor */
template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
CheckInterruptions() {
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Checking interruptions"
			<< Endl << EndDebugInfo;

	uint32_t irq = GetHighestIRQ();
	
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Highest IRQ = 0x" << Hex << irq << Dec
			<< Endl << EndDebugInfo;
	
	if(irq == 0) {
		/* no interruption pending */
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "No interrupt pending, unsetting external interruptions to the processor"
				<< Endl << EndDebugInfo;
		UnsetINT();
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Finished checking interruptions"
				<< Endl << EndDebugInfo;
		return;
	}
	
	uint32_t vpr = GetVPRFromIRQMask(irq);
	uint32_t prio = GetPriority(vpr);
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "vpr of irq mask (0x" << Hex << irq << Dec << ")"
			<< " is 0x" << Hex << vpr << Dec << Endl
			<< "- priority = 0x" << Hex << prio << Dec
			<< Endl << EndDebugInfo;
	
	if(prio == 0) {
		/* if the highest irq has a priority of 0, the interruption
		 *   is disabled, do nothing */
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "The highest irq has a priority of 0, the interruption is disabled"
				<< Endl << EndDebugInfo;
		UnsetINT();
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Finished checking interruptions"
				<< Endl << EndDebugInfo;
		return;
	}
	
	if(prio <= GetProcessorPriority()) {
		/* the processor has more priority than the highest irq in the
		 *   pending reg, do nothing */
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "processor priority (0x" << Hex << GetProcessorPriority() << Dec << ")"
				<< " higher than the interruption priority (0x" << Hex << prio << Dec << ")"
				<< ", unsetting external interruptions to the processor"
				<< Endl << EndDebugInfo;
		UnsetINT();
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Finished checking interruptions"
				<< Endl << EndDebugInfo;
		return;
	}
	
	if(inservice_reg.HasIRQ() &&
		(prio <= inservice_reg.Priority())) {
		/* the current interruption in the inservice register has a higher
		 *   priority than the highest irq in the pending reg, do nothing */
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "the current interruption in the inservice register has a higher priority "
				<< "(0x" << Hex << inservice_reg.Priority() << Dec << ")"
				<< " than the highest irq in the pending register "
				<< "(max_prio = 0x" << Hex << prio << Dec << ")"
				<< ", unsetting external interruptions to the processor"
				<< Endl << EndDebugInfo;
		UnsetINT();
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Finished checking interruptions"
				<< Endl << EndDebugInfo;
		return;
	}
	
	/* an interruption needs to be notified to the processor */
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "interruption with mask 0x" << Hex << irq << Dec 
			<< " needs to be notified to the processor"
			<< ", setting external interruptions to the processor"
			<< Endl << EndDebugInfo;
	SetINT();
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Finished checking interruptions"
			<< Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteGCR(uint32_t data) {
	uint32_t orig_data = data;
	
	/* check reserved bits */
	if(data & (uint32_t)0x40000000) {
		data = data & ~((uint32_t)0x40000000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bit 30 of register GCR, resetting it to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x1fffffff) {
		data = data & ~((uint32_t)0x1fffffff);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 28-0 of register GCR, resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating register GCR with value 0x" << Hex << data << Dec
			<< " (previous value 0x" << Hex << regs.gcr << Dec << ")"
			<< ", modified fields: " << Endl;
		if(data & (uint32_t)0x80000000)
			(*logger_import) 
				<< "   - setting (1) reset EPIC unit bit (bit 31), EPIC will be resetted and this bit put to 0 again" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) reset EPIC unit bit (bit 31), does nothing" << Endl;
		if(data & (uint32_t)0x20000000)
			(*logger_import)
				<< "   - setting EPIC mixed-mode (bit 29 = 1)" << Endl;
		else
			(*logger_import)
				<< "   - setting EPIC Pass-through mode (bit 29 = 0)" << Endl;
	}
	if(data & (uint32_t)0x80000000)
		ResetEPIC();
	else
		if(data & (uint32_t)0x20000000)
			regs.gcr = data;
	if(DEBUG && logger_import) 
		(*logger_import) << "Checking GCR = 0x" << Hex << regs.gcr << Dec << Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteEICR(uint32_t data) {
	uint32_t orig_data = data;
	
	/* checking reserved bits */
	if(data & (uint32_t)0x80000000) {
		data = data & ~((uint32_t)0x80000000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bit 31 of register EICR, resetting it to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x07ffffff) {
		data = data & ~((uint32_t)0x07ffffff);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 26-0 of register EICR, resetting it to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating register EICR with value 0x" << Hex << data << Dec
			<< " (previous value 0x" << Hex << regs.eicr << Dec << ")"
			<< ", modified fields: " << Endl;
		uint32_t val = (data >> 28) & (uint32_t)0x07;
		(*logger_import) 
			<< "   - setting clock ratio to 0x" << Hex << val << Dec << Endl;
		if(data & (uint32_t)0x08000000)
			(*logger_import)
				<< "   - setting (1) serial interrupt bit (bit 27), serial interrupt mode" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) serial interrupt bit (bit 27), direct interrupt mode" << Endl;
	}
	regs.eicr = data;
	if(DEBUG && logger_import) 
		(*logger_import) << "Checking EICR = 0x" << Hex << regs.eicr << Dec << Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteGTVPR(uint32_t data, unsigned int id) {
	uint32_t orig_data = data;
	uint32_t *reg = NULL;
	string name;
	
	/* check which register are we modifying */
	reg = &regs.gtvpr[id];
	switch(id) {
	case 0: name = Registers::GTVPR0_NAME; break;
	case 1: name = Registers::GTVPR1_NAME; break;
	case 2: name = Registers::GTVPR2_NAME; break;
	case 3: name = Registers::GTVPR3_NAME; break;
	}
	
	/* cheking reserved bits */
	if(data & (uint32_t)0x3ff00000) {
		data = data & ~((uint32_t)0x3ff00000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 29-20 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x0000ff00) {
		data = data & ~((uint32_t)0x0000ff00);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 15-8 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	/* keeping activity bit (bit 30) */
	if((data & (uint32_t)0x40000000) != (*reg & (uint32_t)0x40000000)) {
		data = (data & ~((uint32_t)0x40000000)) | (*reg & (uint32_t)0x40000000);
		if(logger_import)
			(*logger_import) << DebugWarning << LOCATION
				<< "Trying to modify read-only bit 30 (activity bit) of register " << name << ", resetting it (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating register " << name << " with value 0x" << Hex << data << Dec
			<< " (previous value 0x" << Hex << (*reg) << Dec << ")"
			<< ", modified fields: " << Endl;
		if(data & (uint32_t)0x80000000)
			(*logger_import) 
				<< "   - setting (1) mask bit (bit 31), interrupts from this source disabled" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) mask bit (bit 31), interrupts form this source enabled" << Endl;
		uint32_t val;
		val = (data >> 16) & (uint32_t)0x0f;
		(*logger_import)
			<< "   - setting priority (bits 19-16) to 0x" << Hex << val << Dec << Endl;
		val = data & (uint32_t)0xff;
		(*logger_import)
			<< "   - setting vector (bits 7-0) to 0x" << Hex << val << Dec << Endl;
	}
	*reg = data;
	if(DEBUG && logger_import) 
		(*logger_import) << "Checking " << name << " = 0x" << Hex << *reg << Dec << Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteGTBCR(uint32_t data, unsigned int id) {
	uint32_t orig_reg = regs.gtbcr[id];
	bool counter_previously_activated = 
		(orig_reg & (uint32_t)0x80000000) == (uint32_t)0x80000000; 
	
	/* set the register with the new data */
	regs.gtbcr[id] = data;
	
	/* check if the counter has been inhibited or enabled */
	if(data & (uint32_t)0x80000000) { 
		/* the counter has been inhibited */
		/* nothing needs to be done */
	} else {
		/* the counter has been activated */
		/* if the counter had been previously inhibited then
		 *   this counter does need to be activated. Otherwise
		 *   it doesn't need to be activated again, the counter
		 *   will be set the next time it reaches the 0 value */
		if(!counter_previously_activated) {
			/* copy the counter value to the correspondent GTCCR */
			regs.gtccr[id] = data & (uint32_t)0x7fffffff;
			/* activate the process that signals SDRAM clocks */
			ActivateSDRAMClock();
		}
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteVPR(uint32_t data, unsigned int id) {
	uint32_t orig_data = data;
	uint32_t *reg = NULL;
	uint32_t prev_reg = 0;
	string name;
	
	/* check with register are we modifying IVPR or SVPR */
	switch(id) {
	case 0:
		if(SerialMode()) {
			reg = &regs.svpr[0];
			name = Registers::SVPR0_NAME;
		} else {
			reg = &regs.ivpr[0];
			name = Registers::IVPR0_NAME;
		}
		break;
	case 1:
		if(SerialMode()) {
			reg = &regs.svpr[1];
			name = Registers::SVPR1_NAME;
		} else {
			reg = &regs.ivpr[1];
			name = Registers::IVPR1_NAME;
		}
		break;
	case 2:
		if(SerialMode()) {
			reg = &regs.svpr[2];
			name = Registers::SVPR2_NAME;
		} else {
			reg = &regs.ivpr[2];
			name = Registers::IVPR2_NAME;
		}
		break;
	case 3:
		if(SerialMode()) {
			reg = &regs.svpr[3];
			name = Registers::SVPR3_NAME;
		} else {
			reg = &regs.ivpr[3];
			name = Registers::IVPR3_NAME;
		}
		break;
	case 4:
		if(SerialMode()) {
			reg = &regs.svpr[4];
			name = Registers::SVPR4_NAME;
		} else {
			reg = &regs.ivpr[4];
			name = Registers::IVPR4_NAME;
		}
		break;
	case 5:	reg = &regs.svpr[5]; name = Registers::SVPR5_NAME; break;
	case 6: reg = &regs.svpr[6]; name = Registers::SVPR6_NAME; break;
	case 7:	reg = &regs.svpr[7]; name = Registers::SVPR7_NAME; break;
	case 8:	reg = &regs.svpr[8]; name = Registers::SVPR8_NAME; break;
	case 9:	reg = &regs.svpr[9]; name = Registers::SVPR9_NAME; break;
	case 10: reg = &regs.svpr[10]; name = Registers::SVPR10_NAME;	break;
	case 11: reg = &regs.svpr[11]; name = Registers::SVPR11_NAME;	break;
	case 12: reg = &regs.svpr[12]; name = Registers::SVPR12_NAME;	break;
	case 13: reg = &regs.svpr[13]; name = Registers::SVPR13_NAME;	break;
	case 14: reg = &regs.svpr[14]; name = Registers::SVPR14_NAME;	break;
	case 15: reg = &regs.svpr[15]; name = Registers::SVPR15_NAME;	break;
	}
	
	prev_reg = *reg;
	
	/* cheking reserved bits */
	if(data & (uint32_t)0x3f000000) {
		data = data & ~((uint32_t)0x3f000000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 29-24 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x00300000) {
		data = data & ~((uint32_t)0x00300000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 21-20 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x0000ff00) {
		data = data & ~((uint32_t)0x0000ff00);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 15-8 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	/* keeping activity bit (bit 30) */
	if((data & (uint32_t)0x40000000) != (*reg & (uint32_t)0x40000000)) {
		data = (data & ~((uint32_t)0x40000000)) | (*reg & (uint32_t)0x40000000);
		if(logger_import)
			(*logger_import) << DebugWarning << LOCATION
				<< "Trying to modify read-only bit 30 (activity bit) of register " << name << ", resetting it (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating register " << name << " with value 0x" << Hex << data << Dec
			<< " (previous value 0x" << Hex << (*reg) << Dec << ")"
			<< ", modified fields: " << Endl;
		if(data & (uint32_t)0x80000000)
			(*logger_import) 
				<< "   - setting (1) mask bit (bit 31), interrupts from this source disabled" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) mask bit (bit 31), interrupts form this source enabled" << Endl;
		if(data & (uint32_t)0x00800000)
			(*logger_import)
				<< "   - setting (1) polarity (bit 23), polarity is active-high or positive-edge triggered" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) polarity (bit 23), polarity is active-low or negative-edge triggered" << Endl;
		if(data & (uint32_t)0x00400000)
			(*logger_import)
				<< "   - setting (1) sense (bit 22), external interrupt is level-sensitive" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) sense (bit 22), external interrupt is edge-sensitive" << Endl;
		uint32_t val;
		val = (data >> 16) & (uint32_t)0x0f;
		(*logger_import)
			<< "   - setting priority (bits 19-16) to 0x" << Hex << val << Dec << Endl;
		val = data & (uint32_t)0xff;
		(*logger_import)
			<< "   - setting vector (bits 7-0) to 0x" << Hex << val << Dec << Endl;
	}
	*reg = data;
	if(DEBUG && logger_import) 
		(*logger_import) << "Checking " << name << " = 0x" << Hex << *reg << Dec << Endl << EndDebugInfo;
	// if the vpr activity bit is set and the previous mask was set and unset with the new data
	//   then we need to check if a new interruption needs to be requested to the processor
	if(GetActivity(data)) {
		if(GetMask(prev_reg)) {
			if(!GetMask(data)) {
				if(DEBUG && logger_import) {
					(*logger_import) << DebugInfo << LOCATION
						<< "Mask bit of " << name << " changed from 1 (disable) to 0 (enable), "
						<< "checking interruptions to check if the processor should be signaled with "
						<< "an interruption" << Endl
						<< EndDebugInfo;
				}
				CheckInterruptions();
			}
		}
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WritePCTPR(uint32_t data) {
	uint32_t orig_data = data;
	
	/* check reserved bits */
	if(data & (uint32_t)0xfffffff0) {
		data = data & (uint32_t)0x0f;
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 31-4 of register PCTPR, resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
		
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating register PCTPR with value 0x" << Hex << data << Dec
			<< " (previous value 0x" << Hex << regs.pctpr << Dec << ")"
			<< ", modified fields: " << Endl;
		(*logger_import)
			<< "   - task priority (bits 3-0) to 0x" << Hex << data << Dec << Endl;
	}
	regs.pctpr = data;
	if(DEBUG && logger_import) 
		(*logger_import) << "Checking PCTPR = 0x" << Hex << regs.pctpr << Dec << Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteIACK(uint32_t data) {
	uint32_t orig_data = data;
	
	if(logger_import) {
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register IACK with value 0x" << Hex << data << Dec
			<< ", ignoring write command" << Endl
			<< EndDebugWarning;
	}
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteEOI(uint32_t data) {
	if(data != 0 && logger_import) {
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write a value different than 0 (0x" 
			<< Hex << data << Dec << ") in register EIO, resetting it to 0"
			<< Endl << EndDebugWarning;
	}
	
	/* Remove the highest irq in service (in inservice_reg) */
	if(!inservice_reg.HasIRQ()) {
		if(logger_import) 
			(*logger_import) << DebugWarning << LOCATION
				<< "Write EIO received when there is no interruption in the "
				<< "inservice register" << Endl << EndDebugWarning;
		return;
	}
	inservice_reg.RemoveIRQ();
	/* Check if a new irq should be raised */
	CheckInterruptions();
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteEVI(uint32_t data) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register EVI with data 0x" 
			<< Hex << data << Dec << ", resetting it to 0x"
			<< Hex << regs.evi << Dec << Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WritePI(uint32_t data) {
	if(data > 1) {
		if(logger_import)
			(*logger_import) << DebugWarning << LOCATION
				<< "Writing reserved bits 31-1 of register PI with data 0x"
				<< Hex << (data >> 1) << Dec << ", resetting them to 0 "
				<< "(previous data = 0x" << Hex << data << Dec
				<< ", new data = 0x" << Hex << (data & (uint32_t)0x1) << Dec << ")"
				<< Endl << EndDebugWarning;
		data = data & (uint32_t)0x1; 
	}
	
	if(data) {
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Signaling a soft reset interrupt" << Endl
				<< EndDebugInfo;  
		SetSoftReset();
	} else {
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Disabling the soft reset interrupt" << Endl
				<< EndDebugInfo;  
		UnsetSoftReset();
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteSVR(uint32_t data) {
	if(data & 0xffffff00) {
		if(logger_import)
			(*logger_import) << DebugWarning << LOCATION
				<< "Writing reserved bits 31-8 of register SVR with data 0x"
				<< Hex << (data >> 8) << Dec << ", resetting them to 0 "
				<< "(previous data = 0x" << Hex << data << Dec
				<< ", new data = 0x" << Hex << (data & (uint32_t)0x00ff) << Dec << ")"
				<< Endl << EndDebugWarning;
		data = data & (uint32_t)0x0ff;
	}
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Setting Spurious Vector Register (SVR) to 0x"
			<< Hex << data << Dec
			<< Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteTFRR(uint32_t data) {
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Setting Timer Frequency Reporting Register (TFRR) to 0x"
			<< Hex << data << Dec << " (previous value = 0x"
			<< Hex << regs.tfrr << Dec << ")"
			<< Endl << EndDebugInfo;
	regs.tfrr = data;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteGTCCR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register GTCCR[" << id << "] with value 0x"
			<< Hex << data << Dec << ", ignoring write"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteGTDR(uint32_t data, unsigned int id) {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register GTDR[" << id << "] with value 0x"
			<< Hex << data << Dec << ", ignoring write"
			<< Endl << EndDebugWarning;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteDR(uint32_t data, unsigned int id) {
	string name;

	if(logger_import) {
		/* check with register are we modifying IVPR or SVPR */
		switch(id) {
		case 0:
			if(SerialMode()) {
				name = Registers::SVPR0_NAME;
			} else {
				name = Registers::IVPR0_NAME;
			}
			break;
		case 1:
			if(SerialMode()) {
				name = Registers::SVPR1_NAME;
			} else {
				name = Registers::IVPR1_NAME;
			}
			break;
		case 2:
			if(SerialMode()) {
				name = Registers::SVPR2_NAME;
			} else {
				name = Registers::IVPR2_NAME;
			}
			break;
		case 3:
			if(SerialMode()) {
				name = Registers::SVPR3_NAME;
			} else {
				name = Registers::IVPR3_NAME;
			}
			break;
		case 4:
			if(SerialMode()) {
				name = Registers::SVPR4_NAME;
			} else {
				name = Registers::IVPR4_NAME;
			}
			break;
		case 5: name = Registers::SVPR5_NAME; break;
		case 6: name = Registers::SVPR6_NAME; break;
		case 7:	name = Registers::SVPR7_NAME; break;
		case 8:	name = Registers::SVPR8_NAME; break;
		case 9:	name = Registers::SVPR9_NAME; break;
		case 10: name = Registers::SVPR10_NAME;	break;
		case 11: name = Registers::SVPR11_NAME;	break;
		case 12: name = Registers::SVPR12_NAME;	break;
		case 13: name = Registers::SVPR13_NAME;	break;
		case 14: name = Registers::SVPR14_NAME;	break;
		case 15: name = Registers::SVPR15_NAME;	break;
		}
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register " << name << " with value 0x"
			<< Hex << data << Dec << ", ignoring write"
			<< Endl << EndDebugWarning;
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteIIVPR(uint32_t data, unsigned int id) {
	uint32_t orig_data = data;
	uint32_t *reg = NULL;
	string name;
	
	/* check with register are we modifying IVPR or SVPR */
	switch(id) {
	case 0: reg = &regs.svpr[0]; name = Registers::IVPR0_NAME; break;
	case 1: reg = &regs.svpr[1]; name = Registers::IVPR1_NAME; break;
	case 2: reg = &regs.svpr[2]; name = Registers::IVPR2_NAME; break;
	case 3: reg = &regs.svpr[3]; name = Registers::IVPR3_NAME; break;
	}
	
	/* cheking reserved bits */
	if(data & (uint32_t)0x3f000000) {
		data = data & ~((uint32_t)0x3f000000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 29-24 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x00300000) {
		data = data & ~((uint32_t)0x00300000);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 21-20 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	if(data & (uint32_t)0x0000ff00) {
		data = data & ~((uint32_t)0x0000ff00);
		if(logger_import)
			(*logger_import) << DebugWarning
				<< LOCATION
				<< "Trying to modify bits 15-8 of register " << name << ", resetting them to 0 (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	/* keeping activity bit (bit 30) */
	if((data & (uint32_t)0x40000000) != (*reg & (uint32_t)0x40000000)) {
		data = (data & ~((uint32_t)0x40000000)) | (*reg & (uint32_t)0x40000000);
		if(logger_import)
			(*logger_import) << DebugWarning << LOCATION
				<< "Trying to modify read-only bit 30 (activity bit) of register " << name << ", resetting it (original value = 0x"
				<< Hex << orig_data << Dec
				<< ", new value = 0x" << Hex << data << Dec << ")"
				<< Endl << EndDebugWarning;
	}
	
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Updating register " << name << " with value 0x" << Hex << data << Dec
			<< " (previous value 0x" << Hex << (*reg) << Dec << ")"
			<< ", modified fields: " << Endl;
		if(data & (uint32_t)0x80000000)
			(*logger_import) 
				<< "   - setting (1) mask bit (bit 31), interrupts from this source disabled" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) mask bit (bit 31), interrupts form this source enabled" << Endl;
		if(data & (uint32_t)0x00800000)
			(*logger_import)
				<< "   - setting (1) polarity (bit 23), polarity is active-high or positive-edge triggered" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) polarity (bit 23), polarity is active-low or negative-edge triggered" << Endl;
		if(data & (uint32_t)0x00400000)
			(*logger_import)
				<< "   - setting (1) sense (bit 22), external interrupt is level-sensitive" << Endl;
		else
			(*logger_import)
				<< "   - unsetting (0) sense (bit 22), external interrupt is edge-sensitive" << Endl;
		uint32_t val;
		val = (data >> 16) & (uint32_t)0x0f;
		(*logger_import)
			<< "   - setting priority (bits 19-16) to 0x" << Hex << val << Dec << Endl;
		val = data & (uint32_t)0xff;
		(*logger_import)
			<< "   - setting vector (bits 7-0) to 0x" << Hex << val << Dec << Endl;
	}
	*reg = data;
	if(DEBUG && logger_import) 
		(*logger_import) << "Checking " << name << " = 0x" << Hex << *reg << Dec << Endl << EndDebugInfo;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteIIDR(uint32_t data, unsigned int id) {
	if(DEBUG && logger_import) {
		(*logger_import) << DebugWarning << LOCATION
			<< "Trying to write read-only register IIDR[" << id << "] with value 0x"
			<< Hex << data << Dec << ", ignoring write"
			<< Endl << EndDebugWarning;
	}
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
WriteRegister(PHYSICAL_ADDR addr, uint32_t data, uint32_t size) {
	bool handled = false;

	if(size != sizeof(uint32_t)) {
		(*logger_import) << DebugWarning
			<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
			<< "Trying to write EPIC register at address 0x" << Hex << addr << Dec
			<< " with an unaligned size (size = " << size << "), doing nothing" << Endl
			<< EndDebugWarning;
		return;
	}

	switch(addr & ADDR_MASK) {
	case Registers::GCR : 
		handled = true;
		WriteGCR(data);
		break;
	case Registers::EICR :
		handled = true;
		WriteEICR(data);
		break;
	case Registers::EVI :
		handled = true;
		WriteEVI(data); 
		break;
	case Registers::PI :
		handled = true;
		WritePI(data); 
		break;
	case Registers::SVR :
		handled = true;
		WriteSVR(data); 
		break;
	case Registers::TFRR :
		handled = true;
		WriteTFRR(data); 
		break;
	case Registers::GTVPR0 :
		handled = true;
		WriteGTVPR(data, 0);
		break;
	case Registers::GTVPR1 :
		handled = true;
		WriteGTVPR(data, 1);
		break;
	case Registers::GTVPR2 :
		handled = true;
		WriteGTVPR(data, 2);
		break;
	case Registers::GTVPR3 :
		handled = true;
		WriteGTVPR(data, 3);
		break;
	case Registers::GTBCR0 :
		handled = true;
		WriteGTBCR(data, 0);
		break;
	case Registers::GTBCR1 :
		handled = true;
		WriteGTBCR(data, 1);
		break;
	case Registers::GTBCR2 :
		handled = true;
		WriteGTBCR(data, 2);
		break;
	case Registers::GTBCR3 :
		handled = true;
		WriteGTBCR(data, 3);
		break;
	case Registers::GTCCR0 :
		handled = true;
		WriteGTCCR(data, 0); 
		break;
	case Registers::GTCCR1 :
		handled = true;
		WriteGTCCR(data, 1); 
		break;
	case Registers::GTCCR2 :
		handled = true;
		WriteGTCCR(data, 2); 
		break;
	case Registers::GTCCR3 :	
		handled = true;
		WriteGTCCR(data, 3); 
		break;
	case Registers::GTDR0 :
		handled = true;
		WriteGTDR(data, 0); 
		break;
	case Registers::GTDR1 :
		handled = true;
		WriteGTDR(data, 1); 
		break;
	case Registers::GTDR2 :
		handled = true;
		WriteGTDR(data, 2); 
		break;
	case Registers::GTDR3 :
		handled = true;
		WriteGTDR(data, 3); 
		break;
	case Registers::SDR0 : // and IDR0
		handled = true;
		WriteDR(data, 0); 
		break;
	case Registers::SDR1 : // and IDR1
		handled = true;
		WriteDR(data, 1); 
		break;
	case Registers::SDR2 : // and IDR2
		handled = true;
		WriteDR(data, 2); 
		break;
	case Registers::SDR3 : // and IDR3
		handled = true;
		WriteDR(data, 3); 
		break;
	case Registers::SDR4 : // and IDR4
		handled = true;
		WriteDR(data, 4); 
		break;
	case Registers::SDR5 :
		handled = true;
		WriteDR(data, 5); 
		break;
	case Registers::SDR6 :
		handled = true;
		WriteDR(data, 6); 
		break;
	case Registers::SDR7 :
		handled = true;
		WriteDR(data, 7); 
		break;
	case Registers::SDR8 :
		handled = true;
		WriteDR(data, 8); 
		break;
	case Registers::SDR9 :
		handled = true;
		WriteDR(data, 9); 
		break;
	case Registers::SDR10 :
		handled = true;
		WriteDR(data, 10); 
		break;
	case Registers::SDR11 :
		handled = true;
		WriteDR(data, 11); 
		break;
	case Registers::SDR12 :
		handled = true;
		WriteDR(data, 12); 
		break;
	case Registers::SDR13 :
		handled = true;
		WriteDR(data, 13); 
		break;
	case Registers::SDR14 :
		handled = true;
		WriteDR(data, 14); 
		break;
	case Registers::SDR15 :
		handled = true;
		WriteDR(data, 15); 
		break;
	case Registers::SVPR0 : // and IVPR0
		handled = true;
		WriteVPR(data, 0);
		break;
	case Registers::SVPR1 : // and IVPR1
		handled = true;
		WriteVPR(data, 1);
		break;
	case Registers::SVPR2 : // and IVPR2
		handled = true;
		WriteVPR(data, 2);
		break;
	case Registers::SVPR3 : // and IVPR3
		handled = true;
		WriteVPR(data, 3);
		break;
	case Registers::SVPR4 :
		handled = true;
		WriteVPR(data, 4);
		break;
	case Registers::SVPR5 :
		handled = true;
		WriteVPR(data, 5);
		break;
	case Registers::SVPR6 :
		handled = true;
		WriteVPR(data, 6);
		break;
	case Registers::SVPR7 :
		handled = true;
		WriteVPR(data, 7);
		break;
	case Registers::SVPR8 :
		handled = true;
		WriteVPR(data, 8);
		break;
	case Registers::SVPR9 :
		handled = true;
		WriteVPR(data, 9);
		break;
	case Registers::SVPR10 :
		handled = true;
		WriteVPR(data, 10);
		break;
	case Registers::SVPR11 :
		handled = true;
		WriteVPR(data, 11);
		break;
	case Registers::SVPR12 :
		handled = true;
		WriteVPR(data, 12);
		break;
	case Registers::SVPR13 :
		handled = true;
		WriteVPR(data, 13);
		break;
	case Registers::SVPR14 :
		handled = true;
		WriteVPR(data, 14);
		break;
	case Registers::SVPR15 :
		handled = true;
		WriteVPR(data, 15);
		break;
	case Registers::IIVPR0 :
		handled = true;
		WriteIIVPR(data, 0); 
		break;
	case Registers::IIVPR1 :
		handled = true;
		WriteIIVPR(data, 1); 
		break;
	case Registers::IIVPR2 :
		handled = true;
		WriteIIVPR(data, 2); 
		break;
	case Registers::IIVPR3 :
		handled = true;
		WriteIIVPR(data, 3); 
		break;
	case Registers::IIDR0 :
		handled = true;
		WriteIIDR(data, 0); 
		break;
	case Registers::IIDR1 :
		handled = true;
		WriteIIDR(data, 1); 
		break;
	case Registers::IIDR2 :
		handled = true;
		WriteIIDR(data, 2); 
		break;
	case Registers::IIDR3 :
		handled = true;
		WriteIIDR(data, 4); 
		break;
	case Registers::PCTPR :
		handled = true;
		WritePCTPR(data);
		break;
	case Registers::IACK :
		handled = true;
		WriteEICR(data);
		break;
	case Registers::EOI :
		handled = true;
		WriteEOI(data);
		break;
	}

	if(!handled) {
		if(DEBUG && logger_import) {
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "TODO: trying to write address 0x"
				<< Hex << addr << Dec
				<< " with data 0x" << Hex << data << Dec
				<< " (" << size << " bytes)"
				<< Endl << EndDebugWarning;
		}
	}
}

/* returns the highest priority IRQ in the pending_reg
 * NOTE: it will return 0 if no irq was found */
template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetHighestIRQ() {
	uint32_t vpr = 0;
	uint32_t priority = 0;
	uint32_t max_prio = 0;
	uint32_t irq = 0;

	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Checking the pending register for the highest present IRQ" << Endl
			<< " - pending register = 0x" << Hex << pending_reg << Dec
			<< Endl << EndDebugInfo;	

	for(uint32_t i = IRQ_T0; i <= IRQ_15; i = i << 1) {
		if(pending_reg & i) {
			vpr = GetVPRFromIRQMask(i);
			// ignore the irq if the interruption is masked
			if(!GetMask(vpr)) {
				// get the priority of the irq
				priority = GetPriority(vpr);
				if(irq == 0) {
					irq = i;
					max_prio = priority;
				} else {
					if(priority > max_prio) {
						irq = i;
						max_prio = priority;
					}
				}
			}
		}
	}
	
	return irq;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t 
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetVPR(uint32_t in_irq) {
	uint32_t vpr = 0;
	
	switch(in_irq) {
	case IRQ_T0: vpr = regs.gtvpr[0]; break;
	case IRQ_T1: vpr = regs.gtvpr[1]; break;
	case IRQ_T2: vpr = regs.gtvpr[2]; break;
	case IRQ_T3: vpr = regs.gtvpr[3]; break; 
	case IRQ_DMA0: vpr = regs.iivpr[1]; break;
	case IRQ_DMA1: vpr = regs.iivpr[2]; break;
	case IRQ_MU: vpr = regs.iivpr[3]; break;
	case IRQ_I2C: vpr = regs.iivpr[0]; break; 
	case IRQ_0: 
		if(SerialMode()) vpr = regs.ivpr[0];
		else vpr = regs.svpr[0];
		break;
	case IRQ_1: 
		if(SerialMode()) vpr = regs.ivpr[1];
		else vpr = regs.svpr[1];
		break;
	case IRQ_2: 
		if(SerialMode()) vpr = regs.ivpr[2];
		else vpr = regs.svpr[2];
		break;
	case IRQ_3: 
		if(SerialMode()) vpr = regs.ivpr[3];
		else vpr = regs.svpr[3];
		break;
	case IRQ_4: 
		if(SerialMode()) vpr = regs.ivpr[4];
		else vpr = regs.svpr[4];
		break;
	case IRQ_5: vpr = regs.svpr[5]; break; 
	case IRQ_6: vpr = regs.svpr[6]; break;
	case IRQ_7: vpr = regs.svpr[7]; break; 
	case IRQ_8: vpr = regs.svpr[8]; break; 
	case IRQ_9: vpr = regs.svpr[9]; break; 
	case IRQ_10: vpr = regs.svpr[10]; break;
	case IRQ_11: vpr = regs.svpr[11]; break;
	case IRQ_12: vpr = regs.svpr[12]; break;
	case IRQ_13: vpr = regs.svpr[13]; break;
	case IRQ_14: vpr = regs.svpr[14]; break;
	case IRQ_15: vpr = regs.svpr[15]; break;
	}
	
	return vpr;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetMask(uint32_t reg_val) {
	return reg_val & (uint32_t)0x80000000;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetActivity(uint32_t reg_val) {
	return reg_val & (uint32_t)0x40000000;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
SetActivity(uint32_t &reg_val) {
	reg_val = reg_val | (uint32_t)0x40000000;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, DEBUG> ::
UnsetActivity(uint32_t &reg_val) {
	reg_val = reg_val & ~(uint32_t)0x40000000;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
uint32_t 
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetPriority(uint32_t reg_val) {
	return (reg_val >> 16) & (uint32_t)0x0f;
}

template <class PHYSICAL_ADDR,
	bool DEBUG>
uint32_t
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetVector(uint32_t reg_val) {
	return reg_val & (uint32_t)0x0ff;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
uint32_t 
EPIC<PHYSICAL_ADDR, DEBUG> ::
GetProcessorPriority() {
	return regs.pctpr & (uint32_t)0x0f;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
uint32_t 
EPIC<PHYSICAL_ADDR, DEBUG> ::
ReadIACK() {
	uint32_t irq;
	uint32_t vpr;
	
	/* Return the interrupt vector corresponding to the highest priority pending
	 *   interrupt. Other actions:
	 * - the associated bit in the IPR (irq_pending_reg) is cleared (if it is 
	 *     configured as edge-sensitive)
	 * - the ISR (inservice_reg) is updated
	 * - the int signal is negated
	 * If there is no interrupt pending returns the spurious vector value.
	 */
	/* check if there is at least an interruption in the irq_pending_reg, if 
	 *   not return the spurious vector */
	if(pending_reg == 0)
		return regs.svr;
	/* get the irq from the irq_selector. if there is no irq the return the 
	 *   spurious vector */
	//irq = irq_selector;
	irq = GetHighestIRQ();
	if(irq == 0)
		return regs.svr;
	/* get the vpr from the corresponding irq */
	vpr = GetVPR(irq);
	/* check the priority of the irq, if smaller than the processor or current
	 *   irq in the inservice_reg, return the spurious vector */
	uint32_t prio = GetPriority(vpr);
	if(prio <= GetProcessorPriority())
		return regs.svr;
	if(inservice_reg.Priority())
		if(prio <= inservice_reg.Priority())
			return regs.svr;
	/* clear the corresponding pending register if configured as 
	 *   edge-sensitive */
	bool edge_sensitive = EdgeSensitive(irq, vpr);
	if(edge_sensitive) {
		pending_reg = pending_reg & ~irq;
	}
	/* put the irq vector in the inservice_reg */
	// TODO: inservice_reg = vpr;
	inservice_reg.PushIRQ(vpr, prio, irq);
	/* disable the int signal */
	UnsetINT();
	return GetVector(vpr);
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
uint32_t 
EPIC<PHYSICAL_ADDR, DEBUG> ::
ReadRegister(PHYSICAL_ADDR addr, uint32_t size) {
	string name;
	uint32_t data = 0;
	bool handled = false;
	
	if(size != sizeof(uint32_t)) {
		(*logger_import) << DebugWarning
			<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
			<< "Trying to read EPIC register at address 0x" << Hex << addr << Dec
			<< " with an unaligned size (size = " << size << "), reading 0" << Endl
			<< EndDebugWarning;
		return data;
	}
	
	switch(addr & ADDR_MASK) {
	case Registers::FRR : name = Registers::FRR_NAME; break;
	case Registers::GCR : 
		handled = true; 
		data = regs.gcr;
		name = Registers::GCR_NAME; 
		break;
	case Registers::EICR : 
		handled = true;
		data = regs.eicr;
		name = Registers::EICR_NAME; 
		break;
	case Registers::EVI : name = Registers::EVI_NAME; break;
	case Registers::PI : name = Registers::PI_NAME; break;
	case Registers::SVR : name = Registers::SVR_NAME; break;
	case Registers::TFRR : name = Registers::TFRR_NAME; break;
	case Registers::GTCCR0 : 
		handled = true;
		data = regs.gtccr[0];
		name = Registers::GTCCR0_NAME; 
		break;
	case Registers::GTBCR0 : 
		handled = true;
		data = regs.gtbcr[0];
		name = Registers::GTBCR0_NAME; 
		break;
	case Registers::GTVPR0 : 
		handled = true;
		data = regs.gtvpr[0];
		name = Registers::GTVPR0_NAME; 
		break;
	case Registers::GTDR0 : name = Registers::GTDR0_NAME; break;
	case Registers::GTCCR1 :
		handled = true;
		data = regs.gtccr[1]; 
		name = Registers::GTCCR1_NAME; 
		break;
	case Registers::GTBCR1 : 
		handled = true;
		data = regs.gtbcr[1];
		name = Registers::GTBCR1_NAME; 
		break;
	case Registers::GTVPR1 : 
		handled = true;
		data = regs.gtvpr[1];
		name = Registers::GTVPR1_NAME; 
		break;
	case Registers::GTDR1 : name = Registers::GTDR1_NAME; break;
	case Registers::GTCCR2 : 
		handled = true;
		data = regs.gtccr[2];
		name = Registers::GTCCR2_NAME; 
		break;
	case Registers::GTBCR2 : 
		handled = true;
		data = regs.gtbcr[2];
		name = Registers::GTBCR2_NAME; 
		break;
	case Registers::GTVPR2 : 
		handled = true;
		data = regs.gtvpr[2];
		name = Registers::GTVPR2_NAME; 
		break;
	case Registers::GTDR2 : name = Registers::GTDR2_NAME; break;
	case Registers::GTCCR3 : 
		handled = true;
		data = regs.gtccr[3];
		name = Registers::GTCCR3_NAME; 
		break;
	case Registers::GTBCR3 : 
		handled = true;
		data = regs.gtbcr[3];
		name = Registers::GTBCR3_NAME; 
		break;
	case Registers::GTVPR3 : 
		handled = true;
		data = regs.gtvpr[3];
		name = Registers::GTVPR3_NAME; 
		break;
	case Registers::GTDR3 : name = Registers::GTDR3_NAME; break;
	case Registers::IVPR0 :
//	case Registers::SVPR0 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[0];
			name = Registers::IVPR0_NAME;
		} else {
			data = regs.svpr[0];
			name = Registers::SVPR0_NAME;
		} 
		break;
	case Registers::IDR0 : name = Registers::IDR0_NAME; break;
//	case Registers::SDR0 : name = Registers::SDR0_NAME; break;
	case Registers::IVPR1 : 
//	case Registers::SVPR1 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[1];
			name = Registers::IVPR1_NAME;
		} else {
			data = regs.svpr[1];
			name = Registers::SVPR1_NAME;
		} 
		break;
	case Registers::IDR1 : name = Registers::IDR1_NAME; break;
//	case Registers::SDR1 : name = Registers::SDR1_NAME; break;
	case Registers::IVPR2 : 
//	case Registers::SVPR2 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[2];
			name = Registers::IVPR2_NAME;
		} else {
			data = regs.svpr[2];
			name = Registers::SVPR2_NAME;
		} 
		break;
	case Registers::IDR2 : name = Registers::IDR2_NAME; break;
//	case Registers::SDR2 : name = Registers::SDR2_NAME; break;
	case Registers::IVPR3 : 
//	case Registers::SVPR3 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[3];
			name = Registers::IVPR3_NAME;
		} else {
			data = regs.svpr[3];
			name = Registers::SVPR3_NAME;
		} 
		break;
	case Registers::IDR3 : name = Registers::IDR3_NAME; break;
//	case Registers::SDR3 : name = Registers::SDR3_NAME; break;
	case Registers::IVPR4 : 
//	case Registers::SVPR4 : 
		handled = true;
		if(DirectMode()) {
			data = regs.ivpr[4];
			name = Registers::IVPR4_NAME;
		} else {
			data = regs.svpr[4];
			name = Registers::SVPR4_NAME;
		} 
		break;
	case Registers::IDR4 : name = Registers::IDR4_NAME; break;
//	case Registers::SDR4 : name = Registers::SDR4_NAME; break;
	case Registers::SVPR5 : 
		handled = true;
		data = regs.svpr[5];
		name = Registers::SVPR5_NAME;
		break;
	case Registers::SDR5 : name = Registers::SDR5_NAME; break;
	case Registers::SVPR6 : 
		handled = true;
		data = regs.svpr[6];
		name = Registers::SVPR6_NAME;
		break;
	case Registers::SDR6 : name = Registers::SDR6_NAME; break;
	case Registers::SVPR7 : 
		handled = true;
		data = regs.svpr[7];
		name = Registers::SVPR7_NAME;
		break;
	case Registers::SDR7 : name = Registers::SDR7_NAME; break;
	case Registers::SVPR8 : 
		handled = true;
		data = regs.svpr[8];
		name = Registers::SVPR8_NAME;
		break;
	case Registers::SDR8 : name = Registers::SDR8_NAME; break;
	case Registers::SVPR9 : 
		handled = true;
		data = regs.svpr[9];
		name = Registers::SVPR9_NAME;
		break;
	case Registers::SDR9 : name = Registers::SDR9_NAME; break;
	case Registers::SVPR10 :
		handled = true;
		data = regs.svpr[10];
		name = Registers::SVPR10_NAME;
		break;
	case Registers::SDR10 : name = Registers::SDR10_NAME; break;
	case Registers::SVPR11 :
		handled = true;
		data = regs.svpr[11];
		name = Registers::SVPR11_NAME;
		break;
	case Registers::SDR11 : name = Registers::SDR11_NAME; break;
	case Registers::SVPR12 :
		handled = true;
		data = regs.svpr[12];
		name = Registers::SVPR12_NAME;
		break;
	case Registers::SDR12 : name = Registers::SDR12_NAME; break;
	case Registers::SVPR13 :
		handled = true;
		data = regs.svpr[13];
		name = Registers::SVPR13_NAME;
		break;
	case Registers::SDR13 : name = Registers::SDR13_NAME; break;
	case Registers::SVPR14 :
		handled = true;
		data = regs.svpr[14];
		name = Registers::SVPR14_NAME;
		break;
	case Registers::SDR14 : name = Registers::SDR14_NAME; break;
	case Registers::SVPR15 :
		handled = true;
		data = regs.svpr[15];
		name = Registers::SVPR15_NAME;
		break;
	case Registers::SDR15 : name = Registers::SDR15_NAME; break;
	case Registers::IIVPR0 : name = Registers::IIVPR0_NAME; break;
	case Registers::IIDR0 : name = Registers::IIDR0_NAME; break;
	case Registers::IIVPR1 : name = Registers::IIVPR1_NAME; break;
	case Registers::IIDR1 : name = Registers::IIDR1_NAME; break;
	case Registers::IIVPR2 : name = Registers::IIVPR2_NAME; break;
	case Registers::IIDR2 : name = Registers::IIDR2_NAME; break;
	case Registers::IIVPR3 : name = Registers::IIVPR3_NAME; break;
	case Registers::IIDR3 : name = Registers::IIDR3_NAME; break;
	case Registers::PCTPR : 
		handled = true;
		data = regs.pctpr;
		name = Registers::PCTPR_NAME; 
		break;
	case Registers::IACK :
		handled = true;
		data = ReadIACK(); 
		name = Registers::IACK_NAME; 
		break;
	case Registers::EOI : 
		handled = true;
		data = regs.eoi;
		name = Registers::EOI_NAME; 
		break;
	default:
		if(logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Trying to read EPIC register at reserved address 0x" << Hex << addr << Dec
				<< ", reading 0" << Endl
				<< EndDebugWarning;
		return 0;
		break;
	}
	
	if(handled) {
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo 
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Reading register " << name << " (address = 0x"
				<< Hex << addr << Dec
				<< ", size = " << size << " bytes"
				<< ", data = " << data << ")"
				<< Endl << EndDebugInfo;
		return data;
	}
	if(DEBUG && logger_import)
		(*logger_import) << DebugWarning 
			<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
			<< "TODO: trying to read register " << name << " (address 0x"
			<< Hex << addr << Dec
			<< ") of " << size << " bytes"
			<< Endl << EndDebugWarning;
	return 0;
}

template <class PHYSICAL_ADDR, 
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, DEBUG> ::
ResetEPIC() {
	// What is being done at reset:
	// - all pending and in-service interrupts are cleared
	pending_reg = 0;
	while(inservice_reg.HasIRQ()) inservice_reg.RemoveIRQ();
	irq_selector = 0; irq_req_reg = 0; 
	// - all interrupt masks bits are set
	for(unsigned int i = 0; i < 4; i++) 
		regs.ivpr[i] = regs.ivpr[i] | (uint32_t)0x80000000;
	for(unsigned int i = 0; i < 16; i++)
		regs.svpr[i] = regs.svpr[i] | (uint32_t)0x80000000;
	// - all timer base count values are reset to zero and count inhibited
	for(unsigned int i = 0; i < 4; i++)
		regs.gtbcr[i] = (uint32_t)0x80000000;
	// - the processor current task priority is reset to 0xF thus disabling
	//   interrupt delivery to the processor
	regs.pctpr = (uint32_t)0x0f; 
	// - spurious vector resets to 0xFF
	regs.svr = (uint32_t)0x0ff;
	// - EPIC defaults to pass-through mode
	regs.gcr = 0;
	// - the serial clock ratio resets to 0x4
	regs.eicr = regs.eicr & (uint32_t)0x0fffffff;
	regs.eicr = regs.eicr | (uint32_t)0x40000000;
	// All other registers remain at their pre-reset programmed values
}

#undef LOCATION

} // end of namespace epic
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_EPIC_TCC__
