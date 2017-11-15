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

#ifndef __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_TCC__
#define __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_TCC__

#include <unisim/component/tlm2/dma/freescale/mpc57xx/edma/edma.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/kernel/tlm2/master_id.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace dma {
namespace freescale {
namespace mpc57xx {
namespace edma {

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::NUM_DMA_CHANNELS;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool EDMA<CONFIG>::threaded_model;

template <typename CONFIG>
EDMA<CONFIG>::EDMA(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, peripheral_slave_if("peripheral_slave_if")
	, master_if("master_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, dma_channel()
	, irq()
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, edma_cr(this)  
	, edma_es(this)  
	, edma_erqh(this)
	, edma_erql(this)
	, edma_eeih(this)
	, edma_eeil(this)
	, edma_serq(this)
	, edma_cerq(this)
	, edma_seei(this)
	, edma_ceei(this)
	, edma_cint(this)
	, edma_cerr(this)
	, edma_ssrt(this)
	, edma_cdne(this)
	, edma_inth(this)
	, edma_intl(this)
	, edma_errh(this)
	, edma_errl(this)
	, edma_hrsh(this)
	, edma_hrsl(this)
	, edma_dchpri(this)      
	, edma_dchmid(this)      
	, edma_tcd_saddr(this)   
	, edma_tcd_attr(this)    
	, edma_tcd_soff(this)    
	, edma_tcd_nbytes(this)  
	, edma_tcd_slast(this)   
	, edma_tcd_daddr(this)   
	, edma_tcd_citer(this)   
	, edma_tcd_doff(this)    
	, edma_tcd_dlastsga(this)
	, edma_tcd_biter(this)   
	, edma_tcd_csr(this)
	, reg_addr_map()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
{
	peripheral_slave_if(*this);
	master_if(*this);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Interrupt Controller (EDMA):" << std::endl;
	description_sstr << "  - " << NUM_DMA_CHANNELS << " DMA channel(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 24" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	SC_HAS_PROCESS(EDMA);

	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_channel_name_sstr;
		dma_channel_name_sstr << "dma_channel_" << dma_channel_num;
		dma_channel[dma_channel_num] = new sc_core::sc_in<bool>(dma_channel_name_sstr.str().c_str()); 
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream irq_name_sstr;
		irq_name_sstr << "irq_" << dma_channel_num;
		irq[dma_channel_num] = new sc_core::sc_out<bool>(irq_name_sstr.str().c_str()); 
	}

	// Map EDMA registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(edma_cr  .ADDRESS_OFFSET, &edma_cr  ); 
	reg_addr_map.MapRegister(edma_es  .ADDRESS_OFFSET, &edma_es  ); 
	reg_addr_map.MapRegister(edma_erqh.ADDRESS_OFFSET, &edma_erqh);
	reg_addr_map.MapRegister(edma_erql.ADDRESS_OFFSET, &edma_erql);
	reg_addr_map.MapRegister(edma_eeih.ADDRESS_OFFSET, &edma_eeih);
	reg_addr_map.MapRegister(edma_eeil.ADDRESS_OFFSET, &edma_eeil);
	reg_addr_map.MapRegister(edma_serq.ADDRESS_OFFSET, &edma_serq);
	reg_addr_map.MapRegister(edma_cerq.ADDRESS_OFFSET, &edma_cerq);
	reg_addr_map.MapRegister(edma_seei.ADDRESS_OFFSET, &edma_seei);
	reg_addr_map.MapRegister(edma_ceei.ADDRESS_OFFSET, &edma_ceei);
	reg_addr_map.MapRegister(edma_cint.ADDRESS_OFFSET, &edma_cint);
	reg_addr_map.MapRegister(edma_cerr.ADDRESS_OFFSET, &edma_cerr);
	reg_addr_map.MapRegister(edma_ssrt.ADDRESS_OFFSET, &edma_ssrt);
	reg_addr_map.MapRegister(edma_cdne.ADDRESS_OFFSET, &edma_cdne);
	reg_addr_map.MapRegister(edma_inth.ADDRESS_OFFSET, &edma_inth);
	reg_addr_map.MapRegister(edma_intl.ADDRESS_OFFSET, &edma_intl);
	reg_addr_map.MapRegister(edma_errh.ADDRESS_OFFSET, &edma_errh);
	reg_addr_map.MapRegister(edma_errl.ADDRESS_OFFSET, &edma_errl);
	reg_addr_map.MapRegister(edma_hrsh.ADDRESS_OFFSET, &edma_hrsh);
	reg_addr_map.MapRegister(edma_hrsl.ADDRESS_OFFSET, &edma_hrsl);
	reg_addr_map.MapRegisterFile(EDMA_DCHPRI      ::ADDRESS_OFFSET, &edma_dchpri      );
	reg_addr_map.MapRegisterFile(EDMA_DCHMID      ::ADDRESS_OFFSET, &edma_dchmid      );
	reg_addr_map.MapRegisterFile(EDMA_TCD_SADDR   ::ADDRESS_OFFSET, &edma_tcd_saddr   , /* reg size */ 4, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_ATTR    ::ADDRESS_OFFSET, &edma_tcd_attr    , /* reg size */ 2, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_SOFF    ::ADDRESS_OFFSET, &edma_tcd_soff    , /* reg size */ 2, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_NBYTES  ::ADDRESS_OFFSET, &edma_tcd_nbytes  , /* reg size */ 4, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_SLAST   ::ADDRESS_OFFSET, &edma_tcd_slast   , /* reg size */ 4, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_DADDR   ::ADDRESS_OFFSET, &edma_tcd_daddr   , /* reg size */ 4, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_CITER   ::ADDRESS_OFFSET, &edma_tcd_citer   , /* reg size */ 2, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_DOFF    ::ADDRESS_OFFSET, &edma_tcd_doff    , /* reg size */ 2, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_DLASTSGA::ADDRESS_OFFSET, &edma_tcd_dlastsga, /* reg size */ 4, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_BITER   ::ADDRESS_OFFSET, &edma_tcd_biter   , /* reg size */ 2, /* stride */ 32);
	reg_addr_map.MapRegisterFile(EDMA_TCD_CSR     ::ADDRESS_OFFSET, &edma_tcd_csr     , /* reg size */ 2, /* stride */ 32);

	if(threaded_model)
	{
		SC_THREAD(Process);
		sensitive << schedule.GetKernelEvent();
	}
	else
	{
		SC_METHOD(Process);
		sensitive << schedule.GetKernelEvent();
	}
	
	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();
	
// 	// Spawn an HW_IRQ_Process for each HW IRQ
// 	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
// 	{
// 		sc_core::sc_spawn_options hw_irq_process_spawn_options;
// 		hw_irq_process_spawn_options.spawn_method();
// 		//hw_irq_process_spawn_options.dont_initialize();
// 		hw_irq_process_spawn_options.set_sensitivity(hw_irq[hw_irq_num]);
// 		
// 		std::stringstream hw_irq_process_name_sstr;
// 		hw_irq_process_name_sstr << "HW_IRQ_Process_" << hw_irq_num;
// 		sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::HW_IRQ_Process, this, hw_irq_num), hw_irq_process_name_sstr.str().c_str(), &hw_irq_process_spawn_options);
// 	}
}

template <typename CONFIG>
EDMA<CONFIG>::~EDMA()
{
	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete dma_channel[dma_channel_num];
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete irq[dma_channel_num];
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void EDMA<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool EDMA<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int EDMA<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
			return reg_addr_map.DebugWrite(start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum EDMA<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
				Event *event = schedule.AllocEvent();
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
tlm::tlm_sync_enum EDMA<CONFIG>::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <typename CONFIG>
void EDMA<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <typename CONFIG>
void EDMA<CONFIG>::ProcessEvent(Event *event)
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
			ReadWriteStatus rws = RWS_OK;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map.Write(start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map.Read(start_addr, data_ptr, data_length);
					break;
				default:
					break;
			}
			
			if(IsReadWriteError(rws))
			{
				logger << DebugError << "while mapped read/write access, " << std::hex << rws << std::dec << std::endl;
				payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
			}
			else
			{
				payload->set_response_status(tlm::TLM_OK_RESPONSE);
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
		
		tlm::tlm_sync_enum sync = peripheral_slave_if->nb_transport_bw(*payload, phase, completion_time);
		
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
void EDMA<CONFIG>::ProcessEvents()
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

template <typename CONFIG>
void EDMA<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			wait();
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::Reset()
{
	edma_cr  .Reset();
	edma_es  .Reset();
	edma_erqh.Reset();
	edma_erql.Reset();
	edma_eeih.Reset();
	edma_eeil.Reset();
	edma_serq.Reset();
	edma_cerq.Reset();
	edma_seei.Reset();
	edma_ceei.Reset();
	edma_cint.Reset();
	edma_cerr.Reset();
	edma_ssrt.Reset();
	edma_cdne.Reset();
	edma_inth.Reset();
	edma_intl.Reset();
	edma_errh.Reset();
	edma_errl.Reset();
	edma_hrsh.Reset();
	edma_hrsl.Reset();
	
	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		edma_dchpri      [dma_channel_num].Reset();
		edma_dchmid      [dma_channel_num].Reset();
		edma_tcd_saddr   [dma_channel_num].Reset();
		edma_tcd_attr    [dma_channel_num].Reset();
		edma_tcd_soff    [dma_channel_num].Reset();
		edma_tcd_nbytes  [dma_channel_num].Reset();
		edma_tcd_slast   [dma_channel_num].Reset();
		edma_tcd_daddr   [dma_channel_num].Reset();
		edma_tcd_citer   [dma_channel_num].Reset();
		edma_tcd_doff    [dma_channel_num].Reset();
		edma_tcd_dlastsga[dma_channel_num].Reset();
		edma_tcd_biter   [dma_channel_num].Reset();
		edma_tcd_csr     [dma_channel_num].Reset();
	}

	schedule.Clear();
	
	if(!m_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::service::Object::Stop(-1);
		return;
	}
	
	UpdateMasterClock();
}

template <typename CONFIG>
void EDMA<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void EDMA<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Master Clock properties have changed
	UpdateMasterClock();
}

} // end of namespace edma
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace dma
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_TCC__
