/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__

#include <unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace freescale {
namespace mpc57xx {
namespace intc {

template <typename CONFIG>
INTC<CONFIG>::INTC(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, ahb_if()
	, p_hw_irq()
	, p_iack()
	, p_avec_b()
	, p_voffset()
	, logger(*this)
	, intc_bcr()
	, intc_mprot()
	, intc_cpr()
	, intc_iack()
	, intc_eoir()
	, intc_sscir()
	, intc_psr()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
{
	unsigned int sw_irq_num;
	unsigned int hw_irq_num;
	unsigned int irq_num;
	unsigned int prc_num;

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream ahb_if_name_sstr;
		ahb_if_name_sstr << "ahb_if_" << prc_num;
		
		ahb_if[prc_num] = new ahb_slave_if_type(ahb_if_name_sstr.str().c_str());
		
		ahb_if[prc_num]->register_b_transport(this, &INTC<CONFIG>::b_transport, prc_num);
		ahb_if[prc_num]->register_get_direct_mem_ptr(this, &INTC<CONFIG>::get_direct_mem_ptr, prc_num);
		ahb_if[prc_num]->register_transport_dbg(this, &INTC<CONFIG>::transport_dbg, prc_num);
		ahb_if[prc_num]->register_nb_transport_fw(this, &INTC<CONFIG>::nb_transport_fw, prc_num);
	}
	
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		std::stringstream p_hw_irq_name_sstr;
		p_hw_irq_name_sstr << "p_hw_irq_" << hw_irq_num;
		p_hw_irq[hw_irq_num] = new sc_core::sc_in<bool>(p_hw_irq_name_sstr.str().c_str()); 
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_iack_name_sstr;
		p_iack_name_sstr << "p_iack_" << prc_num;
		p_iack[prc_num] = new sc_core::sc_in<bool>(p_iack_name_sstr.str().c_str());
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_avec_b_name_sstr;
		p_avec_b_name_sstr << "p_avec_b_" << prc_num;
		p_avec_b[prc_num] = new sc_core::sc_out<bool>(p_avec_b_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_voffset_name_sstr;
		p_voffset_name_sstr << "p_voffset_" << prc_num;
		p_voffset[prc_num] = new sc_core::sc_out<sc_dt::sc_uint<14> >(p_voffset_name_sstr.str().c_str());
	}
	
	reg_addr_map.MapRegister(intc_bcr.ADDRESS_OFFSET, &intc_bcr);
	reg_addr_map.MapRegister(intc_mprot.ADDRESS_OFFSET, &intc_mprot);
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		reg_addr_map.MapRegister(intc_cpr[prc_num].ADDRESS_OFFSET + (prc_num * 4), &intc_cpr[prc_num]);
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		reg_addr_map.MapRegister(intc_iack[prc_num].ADDRESS_OFFSET + (prc_num * 4), &intc_iack[prc_num]);
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		reg_addr_map.MapRegister(intc_eoir[prc_num].ADDRESS_OFFSET + (prc_num * 4), &intc_eoir[prc_num]);
	}
	for(sw_irq_num = 0; sw_irq_num < NUM_SW_IRQS; sw_irq_num++)
	{
		reg_addr_map.MapRegister(intc_sscir[sw_irq_num].ADDRESS_OFFSET + (sw_irq_num * 4), &intc_sscir[sw_irq_num]);
	}
	for(irq_num = 0; irq_num < NUM_IRQS; irq_num++)
	{
		reg_addr_map.MapRegister(intc_psr[irq_num].ADDRESS_OFFSET + (irq_num * 4), &intc_psr[irq_num]);
	}
}

template <typename CONFIG>
INTC<CONFIG>::~INTC()
{
	unsigned int hw_irq_num;
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete ahb_if[prc_num];
	}
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		delete p_hw_irq[hw_irq_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_iack[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_avec_b[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_voffset[prc_num];
	}
}

template <typename CONFIG>
void INTC<CONFIG>::b_transport(int prc_num, tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	Event *event = schedule.AllocEvent();
	event->SetProcessorNumber(prc_num);
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool INTC<CONFIG>::get_direct_mem_ptr(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::transport_dbg(int prc_num, tlm::tlm_generic_payload& payload)
{
	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	
	if(!data_ptr)
	{
		logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
		unisim::kernel::service::Object::Stop(-1);
		return 0;
	}
	else if(!data_length)
	{
		return 0;
	}
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map.DebugWrite(start_addr, data_ptr, data_length, endian);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(start_addr, data_ptr, data_length, endian);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum INTC<CONFIG>::nb_transport_fw(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				event->SetProcessorNumber(prc_num);
				event->SetPayload(&payload);
				event->SetTimeStamp(notify_time_stamp);
				schedule.Notify(event);
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			logger << DebugError << "protocol error" << EndDebugError;
			unisim::kernel::service::Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <typename CONFIG>
void INTC<CONFIG>::ProcessEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	tlm::tlm_command cmd = payload->get_command();

	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		unsigned int streaming_width = payload->get_streaming_width();
		unsigned int data_length = payload->get_data_length();
		unsigned char *data_ptr = payload->get_data_ptr();
		unsigned char *byte_enable_ptr = payload->get_byte_enable_ptr();
		sc_dt::uint64 start_addr = payload->get_address();
		sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
		
		if(!data_ptr)
		{
			logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			unisim::kernel::service::Object::Stop(-1);
			return;
		}
		else if(!data_length)
		{
			logger << DebugError << "data length range for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			unisim::kernel::service::Object::Stop(-1);
			return;
		}
		else if(byte_enable_ptr)
		{
			// byte enable is unsupported
			logger << DebugWarning << "byte enable for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		}
		else if(streaming_width < data_length)
		{
			// streaming is unsupported
			logger << DebugWarning << "streaming for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		}
		else if((start_addr & -4) != (end_addr & -4))
		{
			logger << DebugWarning << "access crosses 32-bit boundary" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		}
		else
		{
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					if(reg_addr_map.Write(start_addr, data_ptr, data_length, endian))
					{
						payload->set_response_status(tlm::TLM_OK_RESPONSE);
					}
					else
					{
						payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
					}
					break;
				case tlm::TLM_READ_COMMAND:
					if(reg_addr_map.Read(start_addr, data_ptr, data_length, endian))
					{
						payload->set_response_status(tlm::TLM_OK_RESPONSE);
					}
					else
					{
						payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
					}
					break;
				default:
					break;
			}
		}
	}

	payload->set_dmi_allowed(false);
	
	sc_core::sc_time completion_time(sc_core::SC_ZERO_TIME);
	sc_core::sc_event *completion_event = event->GetCompletionEvent();
	
	if(completion_event)
	{
		completion_event->notify(completion_time);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		unsigned int prc_num = event->GetProcessorNumber();
		tlm::tlm_sync_enum sync = (*ahb_if[prc_num])->nb_transport_bw(*payload, phase, completion_time);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
				break;
			case tlm::TLM_UPDATED:
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
	}
}

template <typename CONFIG>
void INTC<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				logger << DebugError << "Internal error: unexpected event time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				unisim::kernel::service::Object::Stop(-1);
			}
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

} // end of namespace intc
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__
