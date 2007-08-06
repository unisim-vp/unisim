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
 
#ifndef __FS_TLM_CHIPSETS_MPC107_EPIC_EPIC_TPP__
#define __FS_TLM_CHIPSETS_MPC107_EPIC_EPIC_TPP__

#include <string>
#include <systemc.h>
#include "tlm/tlm.hh"
#include "utils/services/service.hh"
#include "utils/garbage_collector/garbage_collector.hh"
#include "tlm/interrupt/interrupt.hh"
#include "tlm/shared_memory/snooping_bus/message.hh"
#include "chipsets/mpc107/epic/epic.hh"
#include "plugins/logger/logger_interface.hh"

namespace full_system {
namespace tlm {
namespace chipsets {
namespace mpc107 {
namespace epic {
	
using full_system::tlm::TlmSendIf;
using full_system::tlm::TlmMessage;
using full_system::tlm::RequestPortIdentifier;
using full_system::utils::services::Object;
using full_system::tlm::interrupt::InterruptRequest;
using full_system::tlm::shared_memory::snooping_bus::Request;
using full_system::tlm::shared_memory::snooping_bus::Response;
using namespace full_system::plugins::logger;
using namespace std;

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
EPIC(const sc_module_name &name, Object *parent) :
	sc_module(name),
	full_system::tlm::chipsets::mpc107::epic::EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG>::Object(name, parent),
	full_system::chipsets::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>("epic_impl", parent),
	int_outport("int_outport"),
	soft_reset_outport("soft_reset_outport"),
	req_inport("req_inport"),
	sdram_inport("sdram_inport"),
	sdram_clock_activated_mutex("sdram_clock_activated_mutex"),
	sdram_clock_activated(false) {
//	sdram_clock_event("sdram_clock_event") {
	req_inport(*this);
	/* create irq ports and the objects to handle them */
	for(unsigned int i = 0; i < inherited::NUM_IRQS; i++) {
		stringstream irq_inport_name;
		stringstream irq_inport_handler_name;
		
		irq_inport_name << "irq_inport[" << i << "]";
		irq_inport[i] = new sc_export<TlmSendIf<IntType> >(irq_inport_name.str().c_str());
		irq_inport_handler_name << "irq_inport_handler[" << i << "]";
		irq_handler[i] = new RequestPortIdentifier<InterruptRequest>(irq_inport_handler_name.str().c_str(), i, *this);
		(*irq_inport[i])(*irq_handler[i]);  
	}
	
	SC_THREAD(SDRAMClock);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
~EPIC() {
	for(unsigned int i = 0; i < inherited::NUM_IRQS; i++) {
		delete irq_inport[i];
		delete irq_handler[i];
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
Send(const PMsgType &message) {
	const PReqType& req = message->GetRequest();
	PHYSICAL_ADDR addr = req->addr & (PHYSICAL_ADDR)0x0fffff;
	uint32_t data = 0;

	switch(req->type) {
	case ReqType::READ: 
		{
			if(inherited::logger_import)
				(*inherited::logger_import) << DebugInfo
																		<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
																		<< "Read received at address 0x" << Hex << req->addr << Dec
																		<< Endl << EndDebugInfo;
			data = ReadRegister(req->addr, req->size);
			PRspType rsp = new(rsp) RspType();
			for(unsigned int i = 0; i < req->size; i ++) {
				rsp->read_data[i] = (uint8_t)((data >> (i * 8)) & (uint32_t)0x0ff);
			}
			rsp->read_status = RspType::RS_SHARED;
			message->rsp = rsp;
			message->GetResponseEvent()->notify();
			return true;
		}
		break;
	case ReqType::WRITE:
			if(inherited::logger_import)
				(*inherited::logger_import) << DebugInfo
																		<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
																		<< "Write received at address 0x" << Hex << req->addr << Dec
																		<< Endl << EndDebugInfo;
		for(unsigned int i = 0; i < req->size; i++) {
			data = data + (((uint32_t)(req->write_data[i])) << (i * 8));
		}
		WriteRegister(req->addr, data, req->size);
		return true;
		break;
	case ReqType::READX:
	case ReqType::INV_BLOCK:
	case ReqType::FLUSH_BLOCK:
	case ReqType::ZERO_BLOCK:
	case ReqType::INV_TLB:
		if(inherited::logger_import) {
			(*inherited::logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Received unknown command ";
			switch(req->type) {
			case ReqType::READX:
				(*inherited::logger_import) << "READX";
				break;
			case ReqType::INV_BLOCK:
				(*inherited::logger_import) << "INV_BLOCK";
				break;
			case ReqType::FLUSH_BLOCK:
				(*inherited::logger_import) << "FLUSH_BLOCK";
				break;
			case ReqType::ZERO_BLOCK:
				(*inherited::logger_import) << "ZERO_BLOCK";
				break;
			case ReqType::INV_TLB:
				(*inherited::logger_import) << "INV_TLB";
				break;
			}
			(*inherited::logger_import) << " (address = 0x"
				<< Hex << req->addr << Dec
				<< ")" << Endl << EndDebugError;
		}
		break;
	}
	return false;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
Send(const PIntMsgType &message, unsigned int id) {
	
	if(message->req->level)
		inherited::SetIRQ(id, message->req->serial_id);
	else
		inherited::UnsetIRQ(id, message->req->serial_id);
	return true;
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
SetINT() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = true;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!int_outport->Send(message)) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "TODO: handle interrupt requests that could not be handled"
				<< Endl << EndDebugError; 
		else
			cerr << "TODO("
				<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
				<< "handle interrupt requests that could not be handled"
				<< endl;
		
		sc_stop();
		wait();
	}
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
UnsetINT() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = false;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!int_outport->Send(message)) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "TODO: handle interrupt requests that could not be handled"
				<< Endl << EndDebugError; 
		else
			cerr << "TODO("
				<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
				<< "handle interrupt requests that could not be handled"
				<< endl;
		
		sc_stop();
		wait();
	}
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
SetSoftReset() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = true;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!soft_reset_outport->Send(message)) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "TODO: handle soft reset interrupt requests that could not be handled"
				<< Endl << EndDebugError; 
		else
			cerr << "TODO("
				<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
				<< "handle soft reset interrupt requests that could not be handled"
				<< endl;
		
		sc_stop();
		wait();
	}
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
UnsetSoftReset() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = false;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!soft_reset_outport->Send(message)) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "TODO: handle soft reset interrupt requests that could not be handled"
				<< Endl << EndDebugError; 
		else
			cerr << "TODO("
				<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "): "
				<< "handle soft reset interrupt requests that could not be handled"
				<< endl;
		
		sc_stop();
		wait();
	}
}

//template <class PHYSICAL_ADDR,
//	uint32_t MAX_TRANSACTION_DATA_SIZE,
//	bool DEBUG>
//void
//EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
//Notify(uint64_t sdram_cycles) {
//	/* this method creates a thread that when the sdram cycles are out
//	 *   calls the TimeSignal method of the epic functional implementation */
//}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
ActivateSDRAMClock() {
	sdram_clock_activated_mutex.lock();
	if(!sdram_clock_activated) {
		/* TODO: fix this function to set the initial delay time to the difference
		 *   between current time and the sdram(multiplied by eight) time */
		sc_time sdram_time = sc_time((double)(sdram_inport * 8), SC_FS);
		sdram_clock_event.notify(sdram_time);
		sdram_clock_activated = true;
	}
	sdram_clock_activated_mutex.unlock();
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
DeactivateSDRAMClock() {
	sdram_clock_activated_mutex.lock();
	sdram_clock_activated = false;
	sdram_clock_activated_mutex.unlock();
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
SDRAMClock() {
	bool cont = false;
	
	while(1) {
		wait(sdram_clock_event);
		cont = true;
		while(cont) {
			bool activated = false;
			sdram_clock_activated_mutex.lock();
			activated = sdram_clock_activated;
			sdram_clock_activated_mutex.unlock();
			if(activated) {
				sc_time time = sc_time((double)(sdram_inport * 8), SC_FS);
				uint64_t sdram_clock_cycles = 
					(uint64_t)(sc_time_stamp() / time);
				inherited::TimeSignal(sdram_clock_cycles * 8);
				wait(time);
			} else 
				cont = false;
		}
	}
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
StopSimulation() {
	sc_stop();
	wait();
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
TimeEvent() {
}

} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipsets namespace
} // end of tlm namespace	
} // end of full_system namespace

#endif /* __FS_TLM_CHIPSETS_MPC107_EPIC_HH__ */
