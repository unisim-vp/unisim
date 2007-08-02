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
 
#ifndef __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29LV_AM29LV_HH__
#define __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29LV_AM29LV_HH__

#include <systemc.h>
#include "unisim/component/tlm/message/memory.hh"
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/component/c++/memory/flash/am29lv/am29lv.hh"

namespace unisim {
namespace component {
namespace tlm {
namespace memory {
namespace flash {
namespace am29lv {

using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::util::garbage_collector::Pointer;
using unisim::component::cxx::memory::am29lv::CMD_READ;
using unisim::component::cxx::memory::am29lv::CMD_WRITE;

using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;

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

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
AM29LV<CONFIG, BYTESIZE, IO_WIDTH, MAX_TRANSACTION_DATA_SIZE>::AM29LV(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	unisim::component::cxx::memory::flash::am29lv::AM29LV<CONFIG, BYTESIZE, IO_WIDTH>(name, parent),
	slave_port("slave-port"),
	frequency(0),
	param_frequency("frequency", this, frequency)
{
	SC_HAS_PROCESS(AM29LV);

	slave_port(*this);

	SC_THREAD(Process);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
AM29LV<CONFIG, BYTESIZE, IO_WIDTH, MAX_TRANSACTION_DATA_SIZE>::~AM29LV()
{
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH, MAX_TRANSACTION_DATA_SIZE>::Setup()
{
	if(inherited::logger_import)
	{
		(*inherited::logger_import) << DebugInfo;
		(*inherited::logger_import) << "Frequency of " << frequency << " Mhz" << Endl;
		(*inherited::logger_import) << EndDebugInfo;
	}
	if(!frequency) return false;
	cycle_time = sc_time(1.0 / (double) frequency, SC_US);

	return inherited::Setup();
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH, MAX_TRANSACTION_DATA_SIZE>::Send(const Pointer<TlmMessage<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > >& message)
{
	return input_queue.nb_write(message);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH, MAX_TRANSACTION_DATA_SIZE>::Process()
{
	while(1)
	{
		const Pointer<TlmMessage<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > > message = input_queue.read();
		const Pointer<Request<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE> >& req = message->GetRequest();
		Pointer<Response<MAX_TRANSACTION_DATA_SIZE> > rsp = new(rsp) MemoryResponse<MAX_TRANSACTION_DATA_SIZE>();
		message->SetResponse(rsp);
		
		switch(req->type)
		{
			case MemoryRequest<typename CONFIG::ADDRESS, IO_WIDTH>::READ:
				{
					if(!inherited::FSM(CMD_READ, req->addr, (uint8_t *) rsp->read_data, req->size))
					{
						if(inherited::logger_import)
						{
							(*inherited::logger_import) << DebugWarning;
							(*inherited::logger_import) << "Invalid read" << Endl;
							(*inherited::logger_import) << EndDebugWarning;
						}
						memset(rsp->read_data, 0, req->size);
					}
					sc_event *rsp_ev = message->GetResponseEvent();
					if(rsp_ev) rsp_ev->notify(cycle_time);
				}
				break;
				
			case MemoryRequest<typename CONFIG::ADDRESS, IO_WIDTH>::WRITE:
				if(!inherited::FSM(CMD_WRITE, req->addr, req->write_data, req->size))
				{
					if(inherited::logger_import)
					{
						(*inherited::logger_import) << DebugWarning;
						(*inherited::logger_import) << "Invalid write" << Endl;
						(*inherited::logger_import) << EndDebugWarning;
					}
				}
				break;
		}
		wait(cycle_time);
	}
}

} // end of namespace am29lv
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29LV_AM29LV_HH__
