/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_TCC__

#include <systemc.h>
#include <unisim/component/cxx/interrupt/xilinx/xps_intc/xps_intc.hh>

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
	
template <class CONFIG>
XPS_IntC<CONFIG>::XPS_IntC(const sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_module(name)
	, unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG>(name, parent)
	, slave_sock("slave-sock")
	, irq_master_sock("irq-master-sock")
	, verbose(false)
	, cycle_time()
	, read_latency()
	, write_latency()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_cycle_time("cycle-time", this, cycle_time, "Cycle time")
	, param_read_latency("read-latency", this, read_latency, "Latency for reading an interrupt controller register")
	, param_write_latency("write-latency", this, write_latency, "Latency for writing an interrupt controller register")
	, unified_payload_queue("unified-payload-queue")
{
	slave_sock(*this); // Bind socket to implementer of interface
	irq_master_sock(*this); // Bind socket to implementer of interface
	
	unsigned int irq;
	for(irq = 0; irq < CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		std::stringstream irq_slave_sock_name_sstr;
		irq_slave_sock_name_sstr << "irq-slave-sock" << irq;
		
		irq_slave_sock[irq] = new irq_slave_socket(irq_slave_sock_name_sstr.str().c_str());

		irq_forwarder[irq] = new IRQForwarder(irq, this);
		
		(*irq_slave_sock[irq])(*irq_forwarder[irq]); // Bind socket to implementer of interface
	}
	
	SC_HAS_PROCESS(XPS_IntC);
	
	SC_THREAD(Process);
}

template <class CONFIG>
XPS_IntC<CONFIG>::~XPS_IntC()
{
	unsigned int irq;
	for(irq = 0; irq < CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		delete irq_slave_sock[irq];
		delete irq_forwarder[irq];
	}
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <class CONFIG>
unsigned int XPS_IntC<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	payload.set_dmi_allowed(false);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status = false;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< ":" << sc_time_stamp().to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< ":" << sc_time_stamp().to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::WriteMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			// transport_dbg should not receive such a command
			inherited::logger << DebugInfo << LOCATION
					<< ":" << sc_time_stamp().to_string() 
					<< " : received an unexpected TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			Object::Stop(-1);
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	return status ? data_length : 0;
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_IntC<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				payload.set_dmi_allowed(false);

				tlm::tlm_command cmd = payload.get_command();
				unsigned int data_length = payload.get_data_length();
				unsigned int byte_enable_length = payload.get_byte_enable_ptr() ? payload.get_byte_enable_length() : 0;
				unsigned int streaming_width = payload.get_streaming_width();
				
				if(cmd != tlm::TLM_IGNORE_COMMAND)
				{
					if((data_length != 4) || (streaming_width && (streaming_width != 4)))
					{
						// streaming is not supported
						// only data length of 4 bytes is supported
						if(IsVerbose())
						{
							inherited::logger << DebugInfo << LOCATION
								<< ":" << (sc_time_stamp() + t).to_string()
								<< ": data length of " << data_length << " bytes and streaming are unsupported"
								<< EndDebugInfo;
						}
						payload.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
						return tlm::TLM_COMPLETED;
					}
					
					if(byte_enable_length)
					{
						// byte enable is not supported
						if(IsVerbose())
						{
							inherited::logger << DebugInfo << LOCATION
								<< ":" << (sc_time_stamp() + t).to_string()
								<< ": byte enable is unsupported"
								<< EndDebugInfo;
						}
						payload.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
						return tlm::TLM_COMPLETED;
					}
				}

				UnifiedPayload *unified_payload = unified_payload_fabric.allocate();
				
				phase = tlm::END_REQ;

				payload.acquire();
				unified_payload->SetPayload(&payload);
				unified_payload->SetNonBlocking();
				unified_payload_queue.notify(*unified_payload, t);
				
				return tlm::TLM_UPDATED;
			}
			break;
		default:
			inherited::logger << DebugError << LOCATION
				<< ":" << (sc_time_stamp() + t).to_string()
				<< ": unexpected phase " << (unsigned int) phase
				<< EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	payload.set_dmi_allowed(false);

	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned int byte_enable_length = payload.get_byte_enable_ptr() ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	
	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		if((data_length != 4) || (streaming_width && (streaming_width != 4)))
		{
			// streaming is not supported
			// only data length of 4 bytes is supported
			if(IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": data length of " << data_length << " bytes and streaming are unsupported"
					<< EndDebugInfo;
			}
			payload.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
			return;
		}
		
		if(byte_enable_length)
		{
			// byte enable is not supported
			if(IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": byte enable is unsupported"
					<< EndDebugInfo;
			}
			payload.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
			return;
		}
	}

	UnifiedPayload *unified_payload = unified_payload_fabric.allocate();
	sc_event ev_completed;

	payload.acquire();
	unified_payload->SetPayload(&payload);
	unified_payload->SetBlocking(&ev_completed);
	unified_payload_queue.notify(*unified_payload, t);
	
	t = local_time = SC_ZERO_TIME;
	wait(ev_completed);
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_IntC<CONFIG>::nb_transport_bw(InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// N/A
}

template <class CONFIG>
void XPS_IntC<CONFIG>::b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t)
{
	UnifiedPayload *unified_payload = unified_payload_fabric.allocate();
	sc_event ev_completed;

	payload.acquire();
	unified_payload->SetPayload(&payload);
	unified_payload->SetBlocking(&ev_completed);
	unified_payload_queue.notify(*unified_payload, t);
	
	t = local_time = SC_ZERO_TIME;
	wait(ev_completed);
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_IntC<CONFIG>::nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				UnifiedPayload *unified_payload = unified_payload_fabric.allocate();
				sc_event ev_completed;

				payload.acquire();
				unified_payload->SetPayload(&payload);
				unified_payload->SetNonBlocking();
				unified_payload_queue.notify(*unified_payload, t);
				
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			}
			break;
		default:
			inherited::logger << DebugError << LOCATION
				<< ":" << (sc_time_stamp() + t).to_string()
				<< ": unexpected phase" << (unsigned int) phase
				<< EndDebugError;
			Object::Stop(-1);
			break;
	}
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::ProcessCPUPayload(UnifiedPayload *unified_payload)
{
	tlm::tlm_generic_payload *payload = unified_payload->cpu_payload;
	typename CONFIG::MEMORY_ADDR addr = payload->get_address();
	switch(payload->get_command())
	{
		case tlm::TLM_READ_COMMAND:
			{
				if(IsVerbose())
				{
					inherited::logger << DebugInfo << LOCATION
						<< ": processing a TLM_READ_COMMAND payload at 0x"
						<< std::hex << addr << std::dec
						<< EndDebugInfo;
				}

				uint32_t value;
				if(inherited::Read(addr, value))
				{
					memcpy(payload->get_data_ptr(), &value, 4);
					payload->set_response_status(tlm::TLM_OK_RESPONSE);
				}
				else
				{
					payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
				}
				
				if(unified_payload->blocking)
				{
					unified_payload->ev_completed->notify(read_latency);
				}
				else
				{
					tlm::tlm_phase phase = tlm::BEGIN_RESP;
					tlm::tlm_sync_enum sync = slave_sock->nb_transport_bw(*payload, phase, local_time);
					
					switch(sync)
					{
						case tlm::TLM_ACCEPTED:
						case tlm::TLM_UPDATED:
							wait(local_time);
							local_time = SC_ZERO_TIME;
							payload->release();
							break;
						case tlm::TLM_COMPLETED:
							payload->release();
							break;
					}
				}
			}
			break;
		case tlm::TLM_WRITE_COMMAND:
			{
				if(IsVerbose())
				{
					inherited::logger << DebugInfo << LOCATION
						<< ": processing a TLM_WRITE_COMMAND payload at 0x"
						<< std::hex << addr << std::dec
						<< EndDebugInfo;
				}
				
				uint32_t value;
				memcpy(&value, payload->get_data_ptr(), 4);
				if(!inherited::Write(addr, value))
				{
					payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
				}
				
				if(unified_payload->blocking)
				{
					unified_payload->ev_completed->notify(write_latency);
				}
				else
				{
					tlm::tlm_phase phase = tlm::BEGIN_RESP;
					tlm::tlm_sync_enum sync = slave_sock->nb_transport_bw(*payload, phase, local_time);
					
					switch(sync)
					{
						case tlm::TLM_ACCEPTED:
						case tlm::TLM_UPDATED:
							wait(local_time);
							local_time = SC_ZERO_TIME;
							payload->release();
							break;
						case tlm::TLM_COMPLETED:
							payload->release();
							break;
					}
				}
			}
			break;
		case tlm::TLM_IGNORE_COMMAND:
			if(IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< ": received a TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< EndDebugInfo;
			}
			break;
	}
}

template <class CONFIG>
void XPS_IntC<CONFIG>::ProcessIntrPayload(UnifiedPayload *unified_payload)
{
	InterruptPayload *payload = unified_payload->intr_payload;
	
	inherited::SetInterruptInput(unified_payload->irq, payload->GetValue());
	
	if(unified_payload->blocking)
	{
		unified_payload->ev_completed->notify(SC_ZERO_TIME);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		tlm::tlm_sync_enum sync = (*irq_slave_sock[unified_payload->irq])->nb_transport_bw(*payload, phase, local_time);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
			case tlm::TLM_UPDATED:
				wait(local_time);
				local_time = SC_ZERO_TIME;
				payload->release();
				break;
			case tlm::TLM_COMPLETED:
				payload->release();
				break;
		}
	}
}

template <class CONFIG>
void XPS_IntC<CONFIG>::Process()
{
	while(1)
	{
		wait(unified_payload_queue.get_event());
		local_time = SC_ZERO_TIME;
		
		UnifiedPayload *unified_payload = unified_payload_queue.get_next_transaction();
		if(unified_payload)
		{
			do
			{
				switch(unified_payload->type)
				{
					case UnifiedPayload::CPU_PAYLOAD:
						ProcessCPUPayload(unified_payload);
						unified_payload->cpu_payload->release();
						unified_payload->cpu_payload = 0;
						break;
					case UnifiedPayload::INTR_PAYLOAD:
						ProcessIntrPayload(unified_payload);
						unified_payload->intr_payload->release();
						unified_payload->intr_payload = 0;
						break;
				}
				
				unified_payload->release();
			}
			while((unified_payload = unified_payload_queue.get_next_transaction()) != 0);
			
			inherited::DetectInterruptInput();
			inherited::GenerateRequest();
		}
	}
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetOutputLevel(bool level)
{
	if(output_level == level) return;

	InterruptPayload *intr_payload = interrupt_payload_fabric.allocate();
	
	intr_payload->SetValue(level);
	
	irq_master_sock->b_transport(*intr_payload, local_time);
	
	intr_payload->release();
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetOutputEdge(bool final_level)
{
	if(output_level == final_level)
	{
		InterruptPayload *intr_payload = interrupt_payload_fabric.allocate();
		
		intr_payload->SetValue(!final_level);
		
		irq_master_sock->b_transport(*intr_payload, local_time);
		
		intr_payload->release();
	}
	
	local_time += cycle_time;

	InterruptPayload *intr_payload = interrupt_payload_fabric.allocate();
	
	intr_payload->SetValue(final_level);
	
	irq_master_sock->b_transport(*intr_payload, local_time);
	
	intr_payload->release();
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
XPS_IntC<CONFIG>::IRQForwarder::IRQForwarder(unsigned int _irq, XPS_IntC<CONFIG> *_xps_intc)
	: irq(_irq)
	, xps_intc(_xps_intc)
{
}

template <class CONFIG>
void XPS_IntC<CONFIG>::IRQForwarder::b_transport(InterruptPayload& trans, sc_core::sc_time& t)
{
	xps_intc->b_transport(irq, trans, t);
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_IntC<CONFIG>::IRQForwarder::nb_transport_fw(InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return xps_intc->nb_transport_fw(irq, trans, phase, t);
}

template <class CONFIG>
unsigned int XPS_IntC<CONFIG>::IRQForwarder::transport_dbg(InterruptPayload& trans)
{
	return 0;
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::IRQForwarder::get_direct_mem_ptr(InterruptPayload& trans, tlm::tlm_dmi&  dmi_data)
{
	return false;
}

template <class CONFIG>
XPS_IntC<CONFIG>::UnifiedPayload::UnifiedPayload()
	: type(CPU_PAYLOAD)
	, blocking(false)
	, cpu_payload(0)
	, intr_payload(0)
	, irq(0)
	, ev_completed(0)
{
}

template <class CONFIG>
XPS_IntC<CONFIG>::UnifiedPayload::~UnifiedPayload()
{
}

template <class CONFIG>
void XPS_IntC<CONFIG>::UnifiedPayload::SetPayload(tlm::tlm_generic_payload *_cpu_payload)
{
	type = CPU_PAYLOAD;
	cpu_payload = _cpu_payload;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::UnifiedPayload::SetPayload(InterruptPayload *_intr_payload)
{
	type = INTR_PAYLOAD;
	intr_payload = _intr_payload;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::UnifiedPayload::SetNonBlocking()
{
	blocking = false;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::UnifiedPayload::SetBlocking(sc_event *_ev_completed)
{
	blocking = true;
	ev_completed = _ev_completed;
}



} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
