/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__

#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace xilinx {
namespace crossbar {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
Crossbar<CONFIG>::Crossbar(const sc_module_name& name, Object *parent)
	: Object(name, parent)
	, unisim::component::cxx::interconnect::xilinx::crossbar::Crossbar<CONFIG>(name, parent)
	, sc_module(name)
	, icurd_plb_slave_sock("icurd-plb-slave-sock")
	, dcuwr_plb_slave_sock("dcuwr-plb-slave-sock")
	, dcurd_plb_slave_sock("dcurd-plb-slave-sock")
	, splb0_slave_sock("splb0-slave-sock")
	, splb1_slave_sock("splb1-slave-sock")
	, mplb_master_sock("mplb-master-sock")
	, mci_master_sock("mci-master-sock")
	, crossbar_dcr_slave_sock("crossbar-dcr-slave-sock")
	, plbs0_dcr_slave_sock("plbs0-dcr-slave-sock")
	, plbs1_dcr_slave_sock("plbs1-dcr-slave-sock")
	, plbm_dcr_slave_sock("plbm-dcr-slave-sock")
	, cycle_time(SC_ZERO_TIME)
	, param_cycle_time("cycle-time", this, cycle_time, "Enable/Disable verbosity")
{
	icurd_plb_redirector = 
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_ICURD_PLB,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	icurd_plb_slave_sock(*icurd_plb_redirector);
		
	dcuwr_plb_redirector = 
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_DCUWR_PLB,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	dcuwr_plb_slave_sock(*dcuwr_plb_redirector);
		
	dcurd_plb_redirector = 
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_DCURD_PLB,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);

	dcurd_plb_slave_sock(*dcurd_plb_redirector);

	splb0_redirector =
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_SPLB0,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	splb0_slave_sock(*splb0_redirector);
	
	splb1_redirector =
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_SPLB1,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
		
	splb1_slave_sock(*splb1_redirector);
		
	crossbar_dcr_redirector =
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_CROSSBAR_DCR,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	crossbar_dcr_slave_sock(*crossbar_dcr_redirector);
	
	plbs0_dcr_redirector =
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_PLBS0_DCR,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	plbs0_dcr_slave_sock(*plbs0_dcr_redirector);
		
	plbs1_dcr_redirector =
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_PLBS1_DCR,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	plbs1_dcr_slave_sock(*plbs1_dcr_redirector);

	plbm_dcr_redirector =
		new unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> >(
			inherited::IF_PLBM_DCR,
			this,
			&Crossbar<CONFIG>::nb_transport_fw,
			&Crossbar<CONFIG>::b_transport,
			&Crossbar<CONFIG>::transport_dbg,
			&Crossbar<CONFIG>::get_direct_mem_ptr
		);
	
	plbm_dcr_slave_sock(*plbm_dcr_redirector);

	mplb_redirector =
		new unisim::kernel::tlm2::BwRedirector<Crossbar<CONFIG> >(
			inherited::IF_MPLB,
			this,
			&Crossbar<CONFIG>::nb_transport_bw,
			&Crossbar<CONFIG>::invalidate_direct_mem_ptr
		);

	mplb_master_sock(*mplb_redirector);

	mci_redirector =
		new unisim::kernel::tlm2::BwRedirector<Crossbar<CONFIG> >(
			inherited::IF_MCI,
			this,
			&Crossbar<CONFIG>::nb_transport_bw,
			&Crossbar<CONFIG>::invalidate_direct_mem_ptr
		);
		
	mci_master_sock(*mci_redirector);

	SC_HAS_PROCESS(Crossbar);
	
	SC_THREAD(Process);
}

template <class CONFIG>
Crossbar<CONFIG>::~Crossbar()
{
	delete icurd_plb_redirector;
	delete dcuwr_plb_redirector;
	delete dcurd_plb_redirector;
	delete splb0_redirector;
	delete splb1_redirector;
	delete crossbar_dcr_redirector;
	delete plbs0_dcr_redirector;
	delete plbs1_dcr_redirector;
	delete plbm_dcr_redirector;
	delete mplb_redirector;
	delete mci_redirector;
}

template <class CONFIG>
bool Crossbar<CONFIG>::BeginSetup()
{
	if(cycle_time == SC_ZERO_TIME)
	{
		inherited::logger << DebugError << "Parameter " << param_cycle_time.GetName() << " must be > " << SC_ZERO_TIME << EndDebugError;
		return false;
	}
	return true;
}

template <class CONFIG>
tlm::tlm_sync_enum Crossbar<CONFIG>::nb_transport_fw(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			switch(intf)
			{
				case inherited::IF_ICURD_PLB:
				case inherited::IF_DCUWR_PLB:
				case inherited::IF_DCURD_PLB:
				case inherited::IF_SPLB0:
				case inherited::IF_SPLB1:
				case inherited::IF_CROSSBAR_DCR:
				case inherited::IF_PLBS0_DCR:
				case inherited::IF_PLBS1_DCR:
				case inherited::IF_PLBM_DCR:
					{
						sc_time notify_time_stamp(sc_time_stamp());
						notify_time_stamp += t;
						Event *event = schedule.AllocEvent();
						event->Initialize(&payload, (typename inherited::Interface) intf, notify_time_stamp);
						schedule.Notify(event);
					}
					phase = tlm::END_REQ;
					return tlm::TLM_UPDATED;
				default:
					inherited::logger << DebugError << "Internal error" << EndDebugError;
					Object::Stop(-1);
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void Crossbar<CONFIG>::b_transport(unsigned int intf, tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	switch(intf)
	{
		case inherited::IF_ICURD_PLB:
		case inherited::IF_DCUWR_PLB:
		case inherited::IF_DCURD_PLB:
		case inherited::IF_SPLB0:
		case inherited::IF_SPLB1:
		case inherited::IF_CROSSBAR_DCR:
		case inherited::IF_PLBS0_DCR:
		case inherited::IF_PLBS1_DCR:
		case inherited::IF_PLBM_DCR:
			{
				sc_time notify_time_stamp(sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				sc_event ev_completed;
				event->Initialize(&payload, (typename inherited::Interface) intf, notify_time_stamp, &ev_completed);
				schedule.Notify(event);
				wait(ev_completed);
			}
			break;
		default:
			inherited::logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
}

template <class CONFIG>
unsigned int Crossbar<CONFIG>::transport_dbg(unsigned int intf, tlm::tlm_generic_payload& payload)
{
	return 0;
}

template <class CONFIG>
bool Crossbar<CONFIG>::get_direct_mem_ptr(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <class CONFIG>
tlm::tlm_sync_enum Crossbar<CONFIG>::nb_transport_bw(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_RESP:
			switch(intf)
			{
				case inherited::IF_MPLB:
				case inherited::IF_MCI:
					{
						sc_time notify_time_stamp(sc_time_stamp());
						notify_time_stamp += t;
						Event *event = schedule.AllocEvent();
						event->Initialize(&payload, (typename inherited::Interface) intf, notify_time_stamp);
						schedule.Notify(event);
					}
					return tlm::TLM_COMPLETED;
				default:
					inherited::logger << DebugError << "Internal error" << EndDebugError;
					Object::Stop(-1);
			}
			break;
		case tlm::END_REQ:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void Crossbar<CONFIG>::invalidate_direct_mem_ptr(unsigned int intf, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
void Crossbar<CONFIG>::Process()
{
	while(1)
	{
		wait(schedule.GetKernelEvent());
		const sc_time& time_stamp = sc_time_stamp();
		if(inherited::IsVerbose())
		{
			inherited::logger << DebugInfo << time_stamp << ": Waking up" << EndDebugInfo;
		}
		
		Event *event = schedule.GetNextEvent();
		
		if(event)
		{
			do
			{
				if(event->GetTimeStamp() != time_stamp)
				{
					inherited::logger << DebugError << "Internal error: unexpected event time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
					Object::Stop(-1);
				}
				if((time_stamp.value() % cycle_time.value()) != 0)
				{
					inherited::logger << DebugError << "Internal error: time stamp is not aligned on clock (time stamp is " << time_stamp << " while cycle time is " << cycle_time << ")" << EndDebugError;
					Object::Stop(-1);
				}
	
				switch(event->GetInterface())
				{
					case inherited::IF_ICURD_PLB:
					case inherited::IF_DCUWR_PLB:
					case inherited::IF_DCURD_PLB:
					case inherited::IF_SPLB0:
					case inherited::IF_SPLB1:
						ProcessForwardEvent(event);
						schedule.FreeEvent(event);
						break;
					case inherited::IF_MPLB:
					case inherited::IF_MCI:
						ProcessBackwardEvent(event);
						schedule.FreeEvent(event);
						break;
					case inherited::IF_CROSSBAR_DCR:
					case inherited::IF_PLBS0_DCR:
					case inherited::IF_PLBS1_DCR:
					case inherited::IF_PLBM_DCR:
						ProcessDCREvent(event);
						schedule.FreeEvent(event);
						break;
				}
			}
			while((event = schedule.GetNextEvent()) != 0);
		}
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::ProcessForwardEvent(Event *event)
{
	sc_event *ev_completed = event->GetCompletionEvent();
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_time t(cycle_time);
	
	typename inherited::Interface src_if = event->GetInterface();
	typename inherited::Interface dst_if = inherited::Route(src_if, payload->get_address());
	
	if(inherited::IsVerbose())
	{
		inherited::logger << DebugInfo << "Routing ("
			<< inherited::GetInterfaceName(src_if) << ", @0x" << std::hex << payload->get_address() << std::dec << ") -> ("
			<< inherited::GetInterfaceName(dst_if) << ", @0x" << std::hex << payload->get_address() << std::dec << ")" << EndDebugInfo;
	}
	
	if(ev_completed)
	{
		switch(dst_if)
		{
			case inherited::IF_MPLB:
				mplb_master_sock->b_transport(*payload, t);
				CheckResponseStatus(src_if, dst_if, payload);
				break;
			case inherited::IF_MCI:
				mci_master_sock->b_transport(*payload, t);
				CheckResponseStatus(src_if, dst_if, payload);
				break;
			default:
				inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": invalid destination interface for b_transport" << EndDebugError;
				Object::Stop(-1);
		}
		
		ev_completed->notify(t);
	}
	else
	{
		return_if.insert(std::pair<tlm::tlm_generic_payload *, typename inherited::Interface>(payload, src_if));
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		
		tlm::tlm_sync_enum sync = tlm::TLM_ACCEPTED;
		
		switch(dst_if)
		{
			case inherited::IF_MPLB:
				sync = mplb_master_sock->nb_transport_fw(*payload, phase, t);
				break;
			case inherited::IF_MCI:
				sync = mci_master_sock->nb_transport_fw(*payload, phase, t);
				break;
			default:
				inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": invalid destination interface for nb_transport_fw" << EndDebugError;
				Object::Stop(-1);
		}
		
		if(sync == tlm::TLM_COMPLETED)
		{
			CheckResponseStatus(src_if, dst_if, payload);
			
			phase = tlm::BEGIN_RESP;
			
			switch(src_if)
			{
				case inherited::IF_ICURD_PLB:
					icurd_plb_slave_sock->nb_transport_bw(*payload, phase, t);
					break;
				case inherited::IF_DCUWR_PLB:
					dcuwr_plb_slave_sock->nb_transport_bw(*payload, phase, t);
					break;
				case inherited::IF_DCURD_PLB:
					dcurd_plb_slave_sock->nb_transport_bw(*payload, phase, t);
					break;
				case inherited::IF_SPLB0:
					splb0_slave_sock->nb_transport_bw(*payload, phase, t);
					break;
				case inherited::IF_SPLB1:
					splb1_slave_sock->nb_transport_bw(*payload, phase, t);
					break;
					break;
				default:
					inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": invalid source interface for nb_transport_bw" << EndDebugError;
					Object::Stop(-1);
			}
		}
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::ProcessBackwardEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	typename std::map<tlm::tlm_generic_payload *, typename inherited::Interface>::iterator it = return_if.find(payload);
	
	if(it == return_if.end())
	{
		inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": can't find return interface" << EndDebugError;
		Object::Stop(-1);
		return;
	}
	
	typename inherited::Interface dst_if = (*it).second;
	return_if.erase(it);
	
	CheckResponseStatus(dst_if, event->GetInterface(), payload);

	sc_time t(cycle_time);
	tlm::tlm_phase phase = tlm::BEGIN_RESP;
	
	switch(dst_if)
	{
		case inherited::IF_ICURD_PLB:
			icurd_plb_slave_sock->nb_transport_bw(*payload, phase, t);
			break;
		case inherited::IF_DCUWR_PLB:
			dcuwr_plb_slave_sock->nb_transport_bw(*payload, phase, t);
			break;
		case inherited::IF_DCURD_PLB:
			dcurd_plb_slave_sock->nb_transport_bw(*payload, phase, t);
			break;
		case inherited::IF_SPLB0:
			splb0_slave_sock->nb_transport_bw(*payload, phase, t);
			break;
		case inherited::IF_SPLB1:
			splb1_slave_sock->nb_transport_bw(*payload, phase, t);
			break;
		default:
				inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": invalid destination interface for nb_transport_bw" << EndDebugError;
			Object::Stop(-1);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::ProcessDCREvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	
	tlm::tlm_command cmd = payload->get_command();
	uint64_t addr = payload->get_address();
	unsigned char *data_ptr = payload->get_data_ptr();
	unsigned int data_length = payload->get_data_length();
	unsigned char *byte_enable_ptr = payload->get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload->get_byte_enable_length() : 0;
	unsigned int streaming_width = payload->get_streaming_width();
	
	tlm::tlm_response_status status = tlm::TLM_OK_RESPONSE;
	
	uint32_t dcrn = addr / 4;
	
	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		if((addr & 3) != 0)
		{
			// only 32-bit aligned accesses are supported
			inherited::logger << DebugError << event->GetTimeStamp()
				<< ": DCR address 0x" << std::hex << addr << std::dec << " is not 32-bit aligned"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(data_length != 4)
		{
			// only data length of 4 bytes is supported
			inherited::logger << DebugError << event->GetTimeStamp()
				<< ": data length of " << data_length << " bytes are unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(streaming_width && (streaming_width != data_length))
		{
			// streaming is not supported if different from data length
			inherited::logger << DebugError << event->GetTimeStamp()
					<< ": data length of " << data_length << " bytes and streaming are unsupported"
					<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(byte_enable_length)
		{
			// byte enable is not supported
			inherited::logger << DebugError << event->GetTimeStamp()
				<< ": byte enable is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(!inherited::IsMappedDCR(dcrn))
		{
			inherited::logger << DebugWarning << event->GetTimeStamp()
				<< ": unmapped access at 0x" << std::hex << addr << std::dec << " ( " << data_length << " bytes)"
				<< EndDebugWarning;
			status = tlm::TLM_ADDRESS_ERROR_RESPONSE;
		}
	}
	
	if(status == tlm::TLM_OK_RESPONSE)
	{
		switch(cmd)
		{
			case tlm::TLM_READ_COMMAND:
				{
					uint32_t value = unisim::util::endian::Host2BigEndian(inherited::ReadDCR(dcrn));
					memcpy(data_ptr, &value, sizeof(value));
				}
				break;
			case tlm::TLM_WRITE_COMMAND:
				{
					uint32_t value;
					memcpy(&value, data_ptr, sizeof(value));
					inherited::WriteDCR(dcrn, unisim::util::endian::BigEndian2Host(value));
				}
				break;
			case tlm::TLM_IGNORE_COMMAND:
				inherited::logger << DebugError << event->GetTimeStamp() 
						<< " : received an unexpected TLM_IGNORE_COMMAND command in payload at 0x"
						<< std::hex << addr << std::dec
						<< EndDebugError;
				Object::Stop(-1);
		}
	}
	
	payload->set_response_status(status);
	payload->set_dmi_allowed(false);

	sc_event *ev_completed = event->GetCompletionEvent();
	if(ev_completed)
	{
		ev_completed->notify(cycle_time);
	}
	else
	{
		typename std::map<tlm::tlm_generic_payload *, typename inherited::Interface>::iterator it = return_if.find(payload);
		
		if(it == return_if.end())
		{
				inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": can't find return interface" << EndDebugError;
			Object::Stop(-1);
			return;
		}
		
		typename inherited::Interface dst_if = (*it).second;
		return_if.erase(it);
		
		sc_time t(cycle_time);
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		switch(dst_if)
		{
			case inherited::IF_CROSSBAR_DCR:
				crossbar_dcr_slave_sock->nb_transport_bw(*payload, phase, t);
				break;
			case inherited::IF_PLBS0_DCR:
				plbs0_dcr_slave_sock->nb_transport_bw(*payload, phase, t);
				break;
			case inherited::IF_PLBS1_DCR:
				plbs1_dcr_slave_sock->nb_transport_bw(*payload, phase, t);
				break;
			case inherited::IF_PLBM_DCR:
				plbm_dcr_slave_sock->nb_transport_bw(*payload, phase, t);
				break;
			default:
				inherited::logger << DebugError << "Internal error in " << __FUNCTION__ << ": invalid destination interface for nb_transport_bw" << EndDebugError;
				Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::CheckResponseStatus(typename inherited::Interface master_if, typename inherited::Interface slave_if, tlm::tlm_generic_payload *payload)
{
	if(payload->get_response_status() != tlm::TLM_OK_RESPONSE)
	{
		inherited::Error(master_if, slave_if, payload->get_address(), payload->get_command() == tlm::TLM_READ_COMMAND, payload->get_data_length());
	}
}

} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__