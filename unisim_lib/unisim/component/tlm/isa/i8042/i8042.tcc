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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_TCC__
#define __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_TCC__

namespace unisim {
namespace component {
namespace tlm {
namespace isa {
namespace i8042 {
	
using unisim::component::cxx::isa::TransactionType;
using unisim::component::cxx::isa::ISASpace;
using unisim::component::cxx::isa::isa_address_t;
using unisim::component::cxx::isa::SP_IO;
using unisim::component::cxx::isa::SP_MEM;
using unisim::component::cxx::isa::TT_WRITE;
using unisim::component::cxx::isa::TT_READ;

using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;


template <uint32_t MAX_DATA_SIZE>
I8042<MAX_DATA_SIZE>::I8042(const sc_module_name& name, Object *parent) :
	sc_module(name),
	Object(name, parent),
	unisim::component::cxx::isa::i8042::I8042(name, parent),
	isa_bus_cycle_time(),
	bus_cycle_time()
{
	irq_level[inherited::KBD_INDEX] = false;
	irq_level[inherited::AUX_INDEX] = false;

	SC_HAS_PROCESS(I8042);
	
	bus_port(*this);
	
	SC_THREAD(KbdIrqMaster);
	SC_THREAD(AuxIrqMaster);

	SC_THREAD(Capture);
}

template <uint32_t MAX_DATA_SIZE>
I8042<MAX_DATA_SIZE>::~I8042()
{
}

template <uint32_t MAX_DATA_SIZE>
bool I8042<MAX_DATA_SIZE>::Setup()
{
	if(!inherited::Setup()) return false;
	isa_bus_cycle_time = sc_time(1.0 / (double) (*this)["isa-bus-frequency"], SC_US);
	bus_cycle_time = sc_time(1.0 / (double) (*this)["fsb-frequency"], SC_US);
	irq_level[inherited::KBD_INDEX] = false;
	irq_level[inherited::AUX_INDEX] = false;
	return true;
}

template <uint32_t MAX_DATA_SIZE>
bool I8042<MAX_DATA_SIZE>::Send(const Pointer<TlmMessage<ISAReq, ISARsp> > &message)
{
	const Pointer<ISAReq>& isa_req = message->GetRequest();
	isa_address_t isa_addr = isa_req->addr;
	ISASpace isa_space = isa_req->space;
	unsigned int isa_req_size = isa_req->size;
	TransactionType isa_transaction_type = isa_req->type;
	
	switch(isa_space)
	{
		case SP_IO:
			switch(isa_transaction_type)
			{
				case TT_READ:
				{
					Pointer<ISARsp> isa_rsp = new(isa_rsp) ISARsp();
				
					if(inherited::ReadIO(isa_addr, isa_rsp->read_data, isa_req_size))
					{
						message->rsp = isa_rsp;
						sc_event *rsp_ev = message->GetResponseEvent();
						rsp_ev->notify(isa_bus_cycle_time);
					}
					else
					{
						// Respond anyway for now
						memset(isa_rsp->read_data, 0, isa_req_size);
						message->rsp = isa_rsp;
						sc_event *rsp_ev = message->GetResponseEvent();
						rsp_ev->notify(isa_bus_cycle_time);
					}
				}
				break;
				case TT_WRITE:
					inherited::WriteIO(isa_addr, isa_req->write_data, isa_req_size);
					break;
			}
			break;
		case SP_MEM:
			break;
	}
		
	return true;
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::KbdIrqMaster()
{
	while(1)
	{
		wait(set_irq_ev[inherited::KBD_INDEX]);
		
		Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
		Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
		irq->level = irq_level[inherited::KBD_INDEX];
		irq->serial_id = 0;
		message->req = irq;
		
		while(!kbd_irq_port->Send(message))
		{
			wait(bus_cycle_time);
		}
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::AuxIrqMaster()
{
	while(1)
	{
		wait(set_irq_ev[inherited::AUX_INDEX]);
		
		Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
		Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
		irq->level = irq_level[inherited::AUX_INDEX];
		irq->serial_id = 0;
		message->req = irq;
		
		while(!aux_irq_port->Send(message))
		{
			wait(bus_cycle_time);
		}
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::Capture()
{
	while(1)
	{
		inherited::CaptureKey();
		wait(sc_time(1.0 / inherited::typematic_rate, SC_SEC));
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::TriggerInterrupt(unsigned int index, bool level)
{
	this->irq_level[index] = level;
	set_irq_ev[index].notify(SC_ZERO_TIME);
}

} // end of namespace i8042
} // end of namespace isa
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
