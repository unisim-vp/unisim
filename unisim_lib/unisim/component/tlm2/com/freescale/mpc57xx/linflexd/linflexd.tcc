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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__

#include <unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace linflexd {

using unisim::kernel::tlm2::tlm_bitstream_sync_status;
using unisim::kernel::tlm2::TLM_BITSTREAM_SYNC_OK;
using unisim::kernel::tlm2::TLM_BITSTREAM_NEED_SYNC;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::NUM_IRQS;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::NUM_IDENTIFIERS;

template <typename CONFIG>
const bool LINFlexD<CONFIG>::threaded_model;

template <typename CONFIG>
LINFlexD<CONFIG>::LINFlexD(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, peripheral_slave_if("peripheral_slave_if")
	, LINTX("LINTX")
	, LINRX("LINRX")
	, m_clk("m_clk")
	, lin_clk("lin_clk")
	, reset_b("reset_b")
	, INT_RX("INT_RX")
	, INT_TX("INT_TX")
	, INT_ERR("INT_ERR")
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, lin_clk_prop_proxy(lin_clk)
	, linflexd_lincr1(this) 
	, linflexd_linier(this) 
	, linflexd_linsr(this)  
	, linflexd_linesr(this) 
	, linflexd_uartcr(this) 
	, linflexd_uartsr(this) 
	, linflexd_lintcsr(this)
	, linflexd_linocr(this) 
	, linflexd_lintocr(this)
	, linflexd_linfbrr(this)
	, linflexd_linibrr(this)
	, linflexd_lincfr(this) 
	, linflexd_lincr2(this) 
	, linflexd_bidr(this)   
	, linflexd_bdrl(this)   
	, linflexd_bdrm(this)   
	, linflexd_ifer(this)   
	, linflexd_ifmi(this)   
	, linflexd_ifmr(this)   
	, linflexd_ifcr(this)   
	, linflexd_gcr(this)    
	, linflexd_uartpto(this)
	, linflexd_uartcto(this)
	, linflexd_dmatxe(this) 
	, linflexd_dmarxe(this) 
	, linflexd_ptd(this)
	, rx_input()
	, window3(0)
	, rx_fifo_cnt(0)
	, gen_int_rx_event("gen_int_rx_event")
	, gen_int_tx_event("gen_int_tx_event")
	, gen_int_err_event("gen_int_err_event")
	, tx_event("tx_event")
	, lins_int_rx_mask(false)
	, last_run_time(sc_core::SC_ZERO_TIME)
	, tx_ready_time(sc_core::SC_ZERO_TIME)
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
	, lin_clock_period(10.0, sc_core::SC_NS)          
	, lin_clock_start_time(sc_core::SC_ZERO_TIME)
	, lin_clock_posedge_first(true)
	, lin_clock_duty_cycle(0.5)
	, baud_period(sc_core::SC_ZERO_TIME)
{
	peripheral_slave_if(*this);
	
	LINTX.register_nb_receive(this, &LINFlexD<CONFIG>::nb_receive, LINTX_IF);
	LINRX.register_nb_receive(this, &LINFlexD<CONFIG>::nb_receive, LINRX_IF);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX LINFlexD:" << std::endl;
	description_sstr << "  - " << NUM_IRQS << " IRQ(s)" << std::endl;
	description_sstr << "  - " << NUM_TX_DMA_CHANNELS << " Tx DMA channel(s)" << std::endl;
	description_sstr << "  - " << NUM_RX_DMA_CHANNELS << " Rx DMA channel(s)" << std::endl;
	description_sstr << "  - " << NUM_FILTERS << " filter(s) implemented" << std::endl;
	description_sstr << "  - " << ((GENERIC_SLAVE == 1) ? "master/slave" : "master") << " LIN operation mode" << std::endl;
	description_sstr << "  - generic psi5=" << GENERIC_PSI5 << std::endl;
	if(HAS_AUTO_SYNCHRONIZATION_SUPPORT)
	{
		description_sstr << "  - Autosynchronization support" << std::endl;
	}
	if(NUM_IDENTIFIERS > 0)
	{
		description_sstr << "  - " << NUM_IDENTIFIERS << " identifier(s)" << std::endl;
	}
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 59" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	unsigned int tx_dma_req_num;
	for(tx_dma_req_num = 0; tx_dma_req_num < NUM_TX_DMA_CHANNELS; tx_dma_req_num++)
	{
		std::stringstream tx_dma_req_num_name_sstr;
		tx_dma_req_num_name_sstr << "tx_dma_req_" << tx_dma_req_num;
		tx_dma_req[tx_dma_req_num] = new sc_core::sc_out<bool>(tx_dma_req_num_name_sstr.str().c_str());
	}
	
	unsigned int rx_dma_req_num;
	for(rx_dma_req_num = 0; rx_dma_req_num < NUM_RX_DMA_CHANNELS; rx_dma_req_num++)
	{
		std::stringstream rx_dma_req_num_name_sstr;
		rx_dma_req_num_name_sstr << "rx_dma_req_" << rx_dma_req_num;
		rx_dma_req[rx_dma_req_num] = new sc_core::sc_out<bool>(rx_dma_req_num_name_sstr.str().c_str());
	}

	SC_HAS_PROCESS(LINFlexD);

	// Map LINFlexD registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(linflexd_lincr1.ADDRESS_OFFSET,  &linflexd_lincr1);  
	reg_addr_map.MapRegister(linflexd_linier.ADDRESS_OFFSET,  &linflexd_linier);  
	reg_addr_map.MapRegister(linflexd_linsr.ADDRESS_OFFSET,   &linflexd_linsr);   
	reg_addr_map.MapRegister(linflexd_linesr.ADDRESS_OFFSET,  &linflexd_linesr);  
	reg_addr_map.MapRegister(linflexd_uartcr.ADDRESS_OFFSET,  &linflexd_uartcr);  
	reg_addr_map.MapRegister(linflexd_uartsr.ADDRESS_OFFSET,  &linflexd_uartsr);  
	reg_addr_map.MapRegister(linflexd_lintcsr.ADDRESS_OFFSET, &linflexd_lintcsr); 
	reg_addr_map.MapRegister(linflexd_linocr.ADDRESS_OFFSET,  &linflexd_linocr);  
	reg_addr_map.MapRegister(linflexd_lintocr.ADDRESS_OFFSET, &linflexd_lintocr); 
	reg_addr_map.MapRegister(linflexd_linfbrr.ADDRESS_OFFSET, &linflexd_linfbrr); 
	reg_addr_map.MapRegister(linflexd_linibrr.ADDRESS_OFFSET, &linflexd_linibrr); 
	reg_addr_map.MapRegister(linflexd_lincfr.ADDRESS_OFFSET,  &linflexd_lincfr);  
	reg_addr_map.MapRegister(linflexd_lincr2.ADDRESS_OFFSET,  &linflexd_lincr2);  
	reg_addr_map.MapRegister(linflexd_bidr.ADDRESS_OFFSET,    &linflexd_bidr);    
	reg_addr_map.MapRegister(linflexd_bdrl.ADDRESS_OFFSET,    &linflexd_bdrl);    
	reg_addr_map.MapRegister(linflexd_bdrm.ADDRESS_OFFSET,    &linflexd_bdrm);    
	reg_addr_map.MapRegister(linflexd_ifer.ADDRESS_OFFSET,    &linflexd_ifer);    
	reg_addr_map.MapRegister(linflexd_ifmi.ADDRESS_OFFSET,    &linflexd_ifmi);    
	reg_addr_map.MapRegister(linflexd_ifmr.ADDRESS_OFFSET,    &linflexd_ifmr);
	if(NUM_IDENTIFIERS > 0)
	{
		reg_addr_map.MapRegisterFile(LINFlexD_IFCR::ADDRESS_OFFSET, &linflexd_ifcr);
	}
	reg_addr_map.MapRegister(linflexd_gcr.ADDRESS_OFFSET,     &linflexd_gcr);    
	reg_addr_map.MapRegister(linflexd_uartpto.ADDRESS_OFFSET, &linflexd_uartpto);
	reg_addr_map.MapRegister(linflexd_uartcto.ADDRESS_OFFSET, &linflexd_uartcto);
	reg_addr_map.MapRegister(linflexd_dmatxe.ADDRESS_OFFSET,  &linflexd_dmatxe); 
	reg_addr_map.MapRegister(linflexd_dmarxe.ADDRESS_OFFSET,  &linflexd_dmarxe); 
	reg_addr_map.MapRegister(linflexd_ptd.ADDRESS_OFFSET,     &linflexd_ptd);    

	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();
	
	SC_METHOD(INT_RX_Process);
	sensitive << gen_int_rx_event;
	
	SC_METHOD(INT_TX_Process);
	sensitive << gen_int_tx_event;
	
	SC_METHOD(INT_ERR_Process);
	sensitive << gen_int_err_event;
	
	SC_THREAD(TX_Process);
	sensitive << tx_event;
	
	SC_THREAD(RX_Process);
	sensitive << rx_input.event();
}

template <typename CONFIG>
LINFlexD<CONFIG>::~LINFlexD()
{
	unsigned int tx_dma_req_num;
	for(tx_dma_req_num = 0; tx_dma_req_num < NUM_TX_DMA_CHANNELS; tx_dma_req_num++)
	{
		delete tx_dma_req[tx_dma_req_num];
	}
	
	unsigned int rx_dma_req_num;
	for(rx_dma_req_num = 0; rx_dma_req_num < NUM_RX_DMA_CHANNELS; rx_dma_req_num++)
	{
		delete rx_dma_req[rx_dma_req_num];
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn Process
	sc_core::sc_spawn_options process_spawn_options;
	if(!threaded_model) process_spawn_options.spawn_method();
	process_spawn_options.set_sensitivity(&schedule.GetKernelEvent());
	process_spawn_options.set_sensitivity(&lin_clk_prop_proxy.GetClockPropertiesChangedEvent());
	
	sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::Process, this), "Process", &process_spawn_options);

	Reset();
	
// 	unisim::kernel::tlm2::tlm_serial_payload p;
// 	sc_dt::sc_bv_base v = sc_dt::sc_bv_base(1);
// 	p.set_data_ptr(&v);
// 	tx_if->b_send(p);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool LINFlexD<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int LINFlexD<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum LINFlexD<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void LINFlexD<CONFIG>::nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload)
{
	if(id == LINRX_IF)
	{
		if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RxEn>()) // receiver enable ?
		{
			const std::vector<bool>& data = payload.get_data();
			const sc_core::sc_time& period = payload.get_period();
			
			if(period != baud_period)
			{
				logger << DebugWarning << "Receiving over " << LINRX.name() << " with a period " << period << " instead of " << baud_period << EndDebugWarning;
			}
			
			rx_input.fill(payload);
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ProcessEvent(Event *event)
{
	switch(event->GetType())
	{
		case EV_NONE:
			break;
		
		case EV_CPU_PAYLOAD:
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
			
			break;
		}
		
		case EV_WAKE_UP:
			break;
		
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	RunToTime(time_stamp); // Run to time
	
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
	
	ScheduleRun();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Process()
{
	return;
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
LINFlexD_OperatingMode LINFlexD<CONFIG>::GetOperatingMode() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::SLEEP>()
	              ? LINFLEXD_SLEEP_MODE
	              : (linflexd_lincr1.template Get<typename LINFlexD_LINCR1::INIT>() ? LINFLEXD_INIT_MODE
	                                                                                : LINFLEXD_NORMAL_MODE);
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UARTMode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::UART>();
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::InitMode() const
{
	return GetOperatingMode() == LINFLEXD_INIT_MODE;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::SleepMode() const
{
	return GetOperatingMode() == LINFLEXD_SLEEP_MODE;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::NormalMode() const
{
	return GetOperatingMode() == LINFLEXD_NORMAL_MODE;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LINTimeoutCounterMode() const
{
	return linflexd_lintcsr.template Get<typename LINFlexD_LINTCSR::MODE>() == 0 /* LIN mode */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::OutputCompareTimeoutCounterMode() const
{
	return linflexd_lintcsr.template Get<typename LINFlexD_LINTCSR::MODE>() == 1 /* output compare mode */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::HardwareChecksumCalculation() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::CCD>() == 0 /* checksum calculation is done by hardware */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::SoftwareChecksumCalculation() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::CCD>() == 1 /* checksum calculation is done by software */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LIN_MasterMode() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::MME>() == 1;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LIN_SlaveMode() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::MME>() == 0;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_RX_BufferMode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFBM>() == 0;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_RX_FIFO_Mode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFBM>() == 1;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_TX_BufferMode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFBM>() == 0;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_TX_FIFO_Mode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFBM>() == 1;
}

template <typename CONFIG>
unsigned int LINFlexD<CONFIG>::UARTWordLength() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WLS>() ? 2 : (linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>() ? 2 : 1);
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LINS_INT_RX() const
{
	return  ((linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_DEL) ||
	         (linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_FIELD) ||
	         (linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_IDENTIFIER_FIELD) ||
	         (linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_CHECKSUM)) && 
	       !((linflexd_linier.template Get<typename LINFlexD_LINIER::WUIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::WUF>()) || 
	        (linflexd_linier.template Get<typename LINFlexD_LINIER::DBFIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::DBFF>()) ||
	        (linflexd_linier.template Get<typename LINFlexD_LINIER::DRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::DRF>()) ||
	        (linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>()));
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Reset()
{
	unsigned int ident_num;
	
	schedule.Clear();

	linflexd_lincr1.Reset();
	linflexd_linier.Reset();
	linflexd_linsr.Reset();
	linflexd_linesr.Reset();
	linflexd_uartcr.Reset();
	linflexd_uartsr.Reset();
	linflexd_lintcsr.Reset();
	linflexd_linocr.Reset();
	linflexd_lintocr.Reset();
	linflexd_linfbrr.Reset();
	linflexd_linibrr.Reset();
	linflexd_lincfr.Reset();
	linflexd_lincr2.Reset();
	linflexd_bidr.Reset();
	linflexd_bdrl.Reset();
	linflexd_bdrm.Reset();
	linflexd_ifer.Reset();
	linflexd_ifmi.Reset();
	linflexd_ifmr.Reset();
	for(ident_num = 0; ident_num < NUM_IDENTIFIERS; ident_num++)
	{
		linflexd_ifcr[ident_num].Reset();
	}
	linflexd_gcr.Reset();
	linflexd_uartpto.Reset();
	linflexd_uartcto.Reset();
	linflexd_dmatxe.Reset(); 
	linflexd_dmarxe.Reset();
	linflexd_ptd.Reset();
	
	if(!m_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "master clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::service::Object::Stop(-1);
		return;
	}
	
	if(!lin_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "LIN clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::service::Object::Stop(-1);
		return;
	}

	UpdateMasterClock();
	UpdateLINClock();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::SoftReset()
{
	linflexd_linsr.SoftReset();
	linflexd_linesr.SoftReset();
	linflexd_lintcsr.SoftReset();
	linflexd_uartsr.SoftReset();
	linflexd_uartcr.SoftReset();
	linflexd_uartcto.SoftReset();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::SetState(LIN_State lin_state)
{
	lins_int_rx_mask = false;
	linflexd_linsr.template Set<typename LINFlexD_LINSR::LINS>(lin_state);
	UpdateINT_RX();
	UpdateINT_TX();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::IncrementTimeout(sc_dt::uint64 delta)
{
	if(delta)
	{
		uint32_t uart_cto = linflexd_uartcto.template Get<typename LINFlexD_UARTCTO::CTO>();
		
		uart_cto += delta;
		
		linflexd_uartcto.template Set<typename LINFlexD_UARTCTO::CTO>(uart_cto);
		
		if(linflexd_uartcto.template Get<typename LINFlexD_UARTCTO::CTO>() == linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>())
		{
			// reset timeout
			linflexd_uartcto.Reset();
			
			// timeout
			linflexd_uartsr.template Set<typename LINFlexD_UARTSR::TO>(1);
			
			// An interrupt will be generated when LINIER.DBEIE/TOIE bit is set on the Error interrupt line in UART mode
			UpdateINT_ERR();
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ResetTimeout()
{
	last_run_time = sc_core::sc_time_stamp();
	linflexd_uartcto.Reset();
}

template <typename CONFIG>
sc_dt::int64 LINFlexD<CONFIG>::TicksToNextRun()
{
	uint32_t uart_cto = linflexd_uartcto.template Get<typename LINFlexD_UARTCTO::CTO>();
	uint32_t uart_pto = linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>();
	
	return uart_pto - uart_cto;
}

template <typename CONFIG>
sc_core::sc_time LINFlexD<CONFIG>::TimeToNextRun()
{
	return TicksToNextRun() * baud_period;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RunToTime(const sc_core::sc_time& time_stamp)
{
	if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RxEn>() && linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>())
	{
		if(time_stamp > last_run_time)
		{
			// Compute the elapsed time since last run
			sc_core::sc_time delay_since_last_run(time_stamp);
			delay_since_last_run -= last_run_time;
			
			// Compute number of timers ticks since last run
			sc_dt::uint64 delta = delay_since_last_run / baud_period;
			
			if(delta)
			{
				sc_core::sc_time run_time(baud_period);
				run_time *= delta;
			
				IncrementTimeout(delta);
				last_run_time += run_time;
			}
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ScheduleRun()
{
	// Clocks properties may have changed that may affect time to next run
	UpdateLINClock();
	
	sc_core::sc_time time_to_next_run = TimeToNextRun();
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next run is " << time_to_next_run << EndDebugInfo;
	}
	
	sc_core::sc_time next_run_time_stamp(sc_core::sc_time_stamp());
	next_run_time_stamp += time_to_next_run;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": timers/RTI timer will run at " << next_run_time_stamp << EndDebugInfo;
	}
	
	Event *event = schedule.AllocEvent();
	event->WakeUp();
	event->SetTimeStamp(next_run_time_stamp);
	schedule.Notify(event);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_RX()
{
	gen_int_rx_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_TX()
{
	gen_int_tx_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_ERR()
{
	gen_int_err_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateLINClock()
{
	lin_clock_period = lin_clk_prop_proxy.GetClockPeriod();
	lin_clock_start_time = lin_clk_prop_proxy.GetClockStartTime();
	lin_clock_posedge_first = lin_clk_prop_proxy.GetClockPosEdgeFirst();
	lin_clock_duty_cycle = lin_clk_prop_proxy.GetClockDutyCycle();
	
	unsigned int rose = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::ROSE>();
	unsigned int osr = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::OSR>();
	unsigned int ibr = linflexd_linibrr.template Get<typename LINFlexD_LINIBRR::IBR>();
	
	if(ibr)
	{
		unsigned int fbr = linflexd_linfbrr.template Get<typename LINFlexD_LINFBRR::FBR>();
		unsigned int period_multiplicator = ((16 * ibr) + fbr) * (rose ? osr : 16);
		baud_period = lin_clock_period;
		baud_period *= (double) period_multiplicator;
	}
	else
	{
		baud_period = sc_core::SC_ZERO_TIME;
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_RX_Process()
{
	INT_RX = (!lins_int_rx_mask && linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_DEL) ||
	         (!lins_int_rx_mask && linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_FIELD) ||
	         (!lins_int_rx_mask && linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_IDENTIFIER_FIELD) ||
	         (!lins_int_rx_mask && linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_CHECKSUM) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::WUIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::WUF>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::DBFIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::DBFF>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::DRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::DRF>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::DRIE>()  && linflexd_linsr.template Get<typename LINFlexD_UARTSR::DRFRFE>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>());
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_TX_Process()
{
	INT_TX = (linflexd_linier.template Get<typename LINFlexD_LINIER::DTIE>()      && linflexd_linsr.template Get<typename LINFlexD_LINSR::DTF>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::DTIE>()      && linflexd_linsr.template Get<typename LINFlexD_UARTSR::DTFTFF>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::DBEIETOIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::DBEF>()) ||
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()      && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>());
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_ERR_Process()
{
	INT_ERR = (linflexd_linier.template Get<typename LINFlexD_LINIER::BOIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::BOF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::FEIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::FEF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::HEIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::SFEF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::HEIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::SDEF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::HEIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::IDPEF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::CEIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::CEF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::BEIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::BEF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::OCIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::OCF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::SZIE>() && linflexd_linesr.template Get<typename LINFlexD_LINESR::SZF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::SZIE>() && linflexd_linesr.template Get<typename LINFlexD_UARTSR::SZF>()) ||
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::DBEIETOIE>() && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::TO>());
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RX_FIFO_Pop()
{
	if(UARTMode())
	{
		// UART mode
		if(UART_RX_FIFO_Mode())
		{
			// FIFO mode
			
			unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
		
			// pop front of Rx FIFO in BDRM
			linflexd_bdrm = linflexd_bdrm >> (wl1 ? 16 : 8);
			
			// shift accordling parity error bits
			unsigned int pe = linflexd_uartsr.template Get<typename LINFlexD_UARTSR::PE>();
			pe = pe >> (wl1 ? 2 : 1);
			linflexd_uartsr.template Set<typename LINFlexD_UARTSR::PE>(pe);
			
			// decrement Rx FIFO count
			unsigned int rx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFC>();
			rx_fifo_cnt -= (wl1 ? 2 : 1);
			linflexd_uartcr.template Set<typename LINFlexD_UARTCR::RFC>(rx_fifo_cnt);
			
			linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFE>(rx_fifo_cnt == 0); // Rx FIFO Empty ?
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateUART_SZF(bool bit_value)
{
	window3 = ((window3 << 1) | bit_value) & 7;
	if(window3 == 4) // detect 100b
	{
		linflexd_uartsr.template Set<typename LINFlexD_UARTSR::SZF>(1);
		UpdateINT_ERR();
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RX_Process()
{
	wait(); // wait for RX input
	sc_core::sc_time rx_time = sc_core::SC_ZERO_TIME;  // Rx time offset relative to current simulation time

	while(1)
	{
		if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RxEn>())
		{
			// RX is enabled
			
			bool bit_value = rx_input.read();
			UpdateUART_SZF(bit_value);
			if(!bit_value) // start bit ?
			{
				rx_time = baud_period;
				
				// start of frame
				unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
				unsigned int wl = (wl1 << 1) | linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL0>();
				unsigned int rdfbm = linflexd_gcr.template Get<typename LINFlexD_GCR::RDFBM>();
				unsigned int rdlis = linflexd_gcr.template Get<typename LINFlexD_GCR::RDLIS>();
				
				unsigned int rx_word_length = 0;       // Rx word length in bits
				unsigned int rx_frame_word_length = 0; // Rx frame word length in bits
				bool receive_parity = false;           // whether to expect a parity bit 
				uint64_t rx_shift_reg = 0;             // 64-bit receiver shift register
				uint8_t rx_parity_error_reg = 0;       // 4-bit parity error register
				unsigned int rec_byte_cnt = 0;         // received byte counter
				unsigned int num_of_frames = 0;        // number of frames received
				
				switch(wl)
				{
					case 0: // 7 bits data + parity
						rx_frame_word_length = 7;
						receive_parity = true;
						rx_word_length = 8;
						break;
					case 1: // 8 bits data when PCE = 0 or 8 bits data + parity when PCE = 1
						rx_frame_word_length = 8;
						receive_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
						rx_word_length = 8;
						break;
					case 2: // 15 bits data + parity
						rx_frame_word_length = 15;
						receive_parity = true;
						rx_word_length = 16;
						break;
					case 3: // 16 bits data when PCE = 0 or 16 bits data + parity when PCE = 1
						rx_frame_word_length = 16;
						receive_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
						rx_word_length = 16;
						break;
				}

				unsigned int rx_msg_length = 0;
				
				if(UART_RX_BufferMode())
				{
					// Buffer mode
					unsigned int rdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RDFL>();
					
					rx_msg_length = 8 * (rdfl + 1); // 8, 16, 24 or 32
				}
				else
				{
					// FIFO mode
					rx_msg_length = rx_word_length; // 8 or 16
				}
				
				// receive payload
				unsigned int i;
				bool parity_bit = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>();
				for(i = 0; i < rx_word_length; i++)
				{
					if(rdfbm)
					{
						// MSB first
						rx_shift_reg <<= 1;
					}
					else
					{
						// LSB first
						rx_shift_reg >>= 1;
					}
					
					if(i < rx_frame_word_length)
					{
						if(rx_input.seek(rx_time) == TLM_BITSTREAM_NEED_SYNC)
						{
							wait();
							rx_time = sc_core::SC_ZERO_TIME;
						}
						bool bit_value = rx_input.read(); // read bitstream
						rx_time += baud_period;
						UpdateUART_SZF(bit_value);
						if(rdlis) bit_value = !bit_value; // invert received data if requested
						if(rdfbm)
						{
							// MSB first
							rx_shift_reg |= bit_value;
						}
						else
						{
							// LSB first
							rx_shift_reg |= (bit_value << (rx_msg_length - 1));
						}
						parity_bit ^= bit_value; // update computed parity bit
					}
				}
				
				if(UART_RX_BufferMode())
				{
					rec_byte_cnt += rx_word_length / 8; // increment received byte count
				}
					
				if(receive_parity) // looking for a parity bit ?
				{
					if(rx_input.seek(rx_time) == TLM_BITSTREAM_NEED_SYNC)
					{
						wait();
						rx_time = sc_core::SC_ZERO_TIME;
					}
					bool bit_value = rx_input.read();
					rx_time += baud_period;
					UpdateUART_SZF(bit_value);
					
					bool parity_error = false;
					if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC1>())
					{
						// logical 0/1 always transmitted
						parity_error = (bit_value != linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>());
					}
					else
					{
						// event/odd parity
						parity_error = (bit_value != parity_bit);
					}
					
					rx_parity_error_reg = ((rx_parity_error_reg << (rx_word_length / 8)) | parity_error) & 0xf; // fill and shift 4-bit parity error register
				}
				
				unsigned int rx_stop_bits = 0;
				bool end_of_frame = false;
				bool framing_error = false;
				bool discard_message = false;
				
				while(!end_of_frame && !framing_error)
				{
					if(rx_input.seek(rx_time) == TLM_BITSTREAM_NEED_SYNC)
					{
						wait();
						rx_time = sc_core::SC_ZERO_TIME;
					}
					bool bit_value = rx_input.read();
					rx_time += baud_period;
					UpdateUART_SZF(bit_value);
					if(bit_value) // stop bit ?
					{
						unsigned int sbur = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::SBUR>();
						if(sbur == 3) sbur = 0; // sbur=11b is reserved, treat it as 00b
						
						if(++rx_stop_bits >= (sbur + 1))
						{
							// received all stop bits
							end_of_frame = true;
							
							if(!discard_message)
							{
								if(UART_RX_BufferMode())
								{
									// Buffer mode
									if(linflexd_uartsr.template Get<typename LINFlexD_UARTSR::RMB>())
									{
										// software did not clear RMB
										linflexd_uartsr.template Set<typename LINFlexD_UARTSR::BOF>(1); // Buffer overrun
										
										if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::RBLM>())
										{
											// if RBLM is set then the new message received will be discarded
											discard_message = true;
										}
										else
										{
											// if RBLM is reset then the new message will overwrite the buffer
										}
									}
								}
								else
								{
									// FIFO mode
									unsigned int rx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFC>();
									if((rx_fifo_cnt + (rx_word_length / 8)) > 4) // Rx FIFO Full ?
									{
										discard_message = true;
									}
								}
							}
							
							if(!discard_message)
							{
								// make parity error flags available to software
								linflexd_uartsr.template Set<typename LINFlexD_UARTSR::PE>(rx_parity_error_reg);
							}
							
							if(UART_RX_BufferMode())
							{
								// Buffer mode
					
								unsigned int rdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RDFL>();
					
								if(rec_byte_cnt == (rdfl + 1)) // end of message ?
								{
									if(!discard_message)
									{
										// latch receiver shift register into BDRM
										linflexd_bdrm = rx_shift_reg;
									}
									
									// clear receiver shift register
									rx_shift_reg = 0;
									
									// reset received byte counter
									rec_byte_cnt = 0;
									
									if(!discard_message)
									{
										// buffer data ready to be read by software
										linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RMB>(1);
										
										// Data Reception Completed
										linflexd_uartsr.template Set<typename LINFlexD_UARTSR::DRF>(1);
										
										// update interrupt signals
										UpdateINT_RX();
									}
								}
							}
							else
							{
								// FIFO mode
								
								if(!discard_message)
								{
									unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
									
									// latch receiver shift register into back of FIFO in BDRM
									linflexd_bdrm = linflexd_bdrm | ((rx_shift_reg << (8 * rx_fifo_cnt)) & (wl1 ? 0xffff : 0xff));
									
									// increment FIFO counter
									unsigned int rx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFC>();
									rx_fifo_cnt += (wl1 ? 2 : 1);
									linflexd_uartcr.template Set<typename LINFlexD_UARTCR::RFC>(rx_fifo_cnt);
									
									linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFE>(0); // Rx FIFO is not empty
								}
								
								// shift receiver shift register
								rx_shift_reg >>= (wl1 ? 16 : 8);
								// shift parity bit register
								rx_parity_error_reg >>= (wl1 ? 2 : 1);
								
								if(!discard_message)
								{
									linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFNE>(1); // Receive FIFO Not Empty
									UpdateINT_RX();
								}
							}
							
							unsigned int nef = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::NEF>(); // number of expected frame
							if(++num_of_frames == nef)
							{
								// received configured number of frames
								
								num_of_frames = 0;

								unsigned int dtu = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::DTU_PCETX>();
								
								// if DTU bit is set, then the UART timeout counter will be reset after the configured number of frames have been received
								if(dtu)
								{
									linflexd_uartcto.Reset();
								}
							}
						}
					}
					else
					{
						framing_error = true;
						linflexd_uartsr.template Set<typename LINFlexD_UARTSR::FEF>(1); // framing error
						UpdateINT_ERR(); // it will generate an interrupt if the FEIE bit of LINIER is set
					}
				}
			}
			else
			{
				// not a start bit
				if(rx_input.next() == TLM_BITSTREAM_NEED_SYNC)
				{
					wait();
				}
			}
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::TX_FIFO_Push()
{
	if(UART_TX_FIFO_Mode())
	{
		// Tx FIFO mode
		if(linflexd_uartsr.template Get<typename LINFlexD_UARTSR::TFF>()) // Tx FIFO Full ?
		{
			// If TFF bit is set and a write is performed to the FIFO, the data transmitted may be erroneous.
			logger << DebugWarning << "The data transmitted may be erroneous if " << linflexd_uartsr.GetName() << " is set and a write is performed to the FIFO" << EndDebugWarning;
			return;
		}
		
		unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
		
		// Push Tx FIFO elements in BDRM, to make space
		linflexd_bdrl = linflexd_bdrl << (wl1 ? 16 : 8);
			
		// increment Tx FIFO counter
		unsigned int cnt = wl1 ? 2 : 1;
		
		unsigned int tx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFC>();
		tx_fifo_cnt += cnt;
		linflexd_uartcr.template Set<typename LINFlexD_UARTCR::TFC>(tx_fifo_cnt);
			
		if(tx_fifo_cnt >= 4)
		{
			// Tx FIFO Full
			linflexd_uartsr.template Set<typename LINFlexD_UARTSR::TFF>(1);
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Transmit()
{
	if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TxEn>()) // UART Tx enable ?
	{
		const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
		if(tx_ready_time > time_stamp)
		{
			// not ready to transmit immediately
			sc_core::sc_time notify_delay = time_stamp;
			notify_delay -= tx_ready_time;
			tx_event.notify(notify_delay);
		}
		else
		{
			// ready to transmit immediately
			tx_event.notify(sc_core::SC_ZERO_TIME);
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::TX_Process()
{
	while(1)
	{
		wait(); // wait Tx event

		tx_ready_time = sc_core::sc_time_stamp();
		
		unisim::kernel::tlm2::tlm_serial_payload payload;
		std::vector<bool>& data = payload.get_data();

		payload.set_period(baud_period);
					
		unsigned int wl = (linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>() << 1) | linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL0>();
		unsigned int tdfbm = linflexd_gcr.template Get<typename LINFlexD_GCR::TDFBM>();
		unsigned int tdlis = linflexd_gcr.template Get<typename LINFlexD_GCR::TDLIS>();
		
		unsigned int tx_word_length = 0;       // Tx word length in bits
		unsigned int tx_frame_word_length = 0; // Tx frame word length in bits
		bool transmit_parity = false;          // whether to send a parity bit 

		switch(wl)
		{
			case 0: // 7 bits data + parity
				tx_frame_word_length = 7;
				transmit_parity = true;
				tx_word_length = 8;
				break;
			case 1: // 8 bits data when PCE = 0 or 8 bits data + parity when PCE = 1
				tx_frame_word_length = 8;
				transmit_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
				tx_word_length = 8;
				break;
			case 2: // 15 bits data + parity
				tx_frame_word_length = 15;
				transmit_parity = true;
				tx_word_length = 16;
				break;
			case 3: // 16 bits data when PCE = 0 or 16 bits data + parity when PCE = 1
				tx_frame_word_length = 16;
				transmit_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
				tx_word_length = 16;
				break;
		}
		
		unsigned int rem_byte_cnt = 0;
		unsigned int tx_msg_length = 0;
		uint64_t tx_shift_reg;                        // 64-bit transmitter shift register

		
		if(UART_TX_BufferMode())
		{
			// Buffer mode
			unsigned int tdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TDFL>();
			rem_byte_cnt = tdfl;
			tx_msg_length = 8 * tdfl; // 8, 16, 24 or 32

			tx_shift_reg = linflexd_bdrl; // latch message into transmitter shift register
		}
		else
		{
			// FIFO mode
			unsigned int tx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFC>();
			rem_byte_cnt = tx_fifo_cnt;
			tx_msg_length = 8 * tx_fifo_cnt; // 8, 16, 24 or 32
			
			tx_shift_reg = 0;
			while(tx_fifo_cnt > 0)
			{
				tx_shift_reg |= (linflexd_bdrl >> (8 * (tx_fifo_cnt - 1))) & 0xff; // latch message info transmitter shift register
				tx_fifo_cnt--;
			}
			
			linflexd_bdrl = 0; // clear Rx FIFO
			// Tx FIFO is not full
			linflexd_uartsr.template Set<typename LINFlexD_UARTSR::TFF>(0);
			
			// Tx FIFO is empty
			linflexd_uartcr.template Set<typename LINFlexD_UARTCR::TFC>(0); // Tx FIFO count = 0
		}
		
		do
		{
			data.push_back(0); // start bit
			tx_ready_time += baud_period;

			// transmit payload
			unsigned int i;
			bool parity_bit = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>();
			for(i = 0; i < tx_word_length; i++)
			{
				if(i < tx_frame_word_length)
				{
					bool bit_value = tdfbm ? ((tx_shift_reg >> (tx_msg_length - 1)) & 1) : (tx_shift_reg & 1);
					if(tdlis) bit_value = !bit_value; // invert transmitted data if requested
					data.push_back(bit_value); // transmit data bit
					tx_ready_time += baud_period;
					parity_bit ^= bit_value; // update computed parity bit
				}
				
				if(tdfbm)
				{
					// MSB first
					tx_shift_reg <<= 1;
				}
				else
				{
					// LSB first
					tx_shift_reg >>= 1;
				}
			}
			
			rem_byte_cnt -= tx_word_length / 8; // increment transmitted byte count
			
			if(transmit_parity)
			{
				if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC1>())
				{
					// logical 0/1 always transmitted
					data.push_back(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>());
				}
				else
				{
					// event/odd parity
					data.push_back(parity_bit);
				}
			}
			tx_ready_time += baud_period;
			
			unsigned int num_stop_bits = linflexd_gcr.template Get<typename LINFlexD_GCR::STOP>() ? 2 : 1;
			
			do
			{
				data.push_back(1); // stop bit
				tx_ready_time += baud_period;
			}
			while(--num_stop_bits == 0);
		}
		while(rem_byte_cnt > 0);
		
		LINTX->b_send(payload);
	}
}

} // end of namespace linflexd
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__
