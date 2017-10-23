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
	, rx_fifo()
	, tx_fifo()
	, rx_event("rx_event")
	, tx_event("tx_event")
	, gen_int_rx_event("gen_int_rx_event")
	, gen_int_tx_event("gen_int_tx_event")
	, gen_int_err_event("gen_int_err_event")
	, lins_int_rx_mask(false)
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

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();
	
	SC_THREAD(TX_Process);
	sensitive << tx_event;

	SC_THREAD(RX_Process);
	sensitive << rx_event;
	
	SC_METHOD(INT_RX_Process);
	sensitive << gen_int_rx_event;
	
	SC_METHOD(INT_TX_Process);
	sensitive << gen_int_tx_event;
	
	SC_METHOD(INT_ERR_Process);
	sensitive << gen_int_err_event;
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
	
	// Spawn ClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options clock_properties_changed_process_spawn_options;
	
	clock_properties_changed_process_spawn_options.spawn_method();
	clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());
	clock_properties_changed_process_spawn_options.set_sensitivity(&lin_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::ClockPropertiesChangedProcess, this), "ClockPropertiesChangedProcess", &clock_properties_changed_process_spawn_options);

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
		std::cerr << sc_core::sc_module::name() << ":LINRX_IF" << std::endl;
		const std::vector<bool>& data = payload.get_data();
		
		int data_length = data.size();
		
		int bit_offset;
		for(bit_offset = 0; bit_offset < data_length; bit_offset++)
		{
			rx_fifo.push(data[bit_offset]);
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ProcessEvent(Event *event)
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
void LINFlexD<CONFIG>::ProcessEvents()
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
void LINFlexD<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			wait(schedule.GetKernelEvent());
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		next_trigger(schedule.GetKernelEvent());
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
	linflexd_uartsr.Initialize(0x0);
	linflexd_lintcsr.Initialize(0x0);
	linflexd_linocr.Initialize(LINFlexD_LINOCR::OC2::template MakeValue<uint32_t>(~uint32_t(0)) | LINFlexD_LINOCR::OC1::template MakeValue<uint32_t>(~uint32_t(0)));
	linflexd_lintocr.Initialize(LINFlexD_LINTOCR::RTO::template MakeValue<uint32_t>(0xe) | LINFlexD_LINTOCR::HTO::template MakeValue<uint32_t>(GENERIC_SLAVE ? 0x2c : 0x1c));
	linflexd_linfbrr.Initialize(0x0);
	linflexd_linibrr.Initialize(0x0);
	linflexd_lincfr.Initialize(0x0);
	linflexd_lincr2.Initialize(LINFlexD_LINCR2::IOBE::template MakeValue<uint32_t>(1) | (GENERIC_SLAVE ? LINFlexD_LINCR2::IOPE::template MakeValue<uint32_t>(1) : 0));
	linflexd_bidr.Initialize(0x0);
	linflexd_bdrl.Initialize(0x0);
	linflexd_bdrm.Initialize(0x0);
	linflexd_ifer.Initialize(0x0);
	linflexd_ifmi.Initialize(0x0);
	linflexd_ifmr.Initialize(0x0);
	for(ident_num = 0; ident_num < NUM_IDENTIFIERS; ident_num++)
	{
		linflexd_ifcr[ident_num].Initialize(0x0);
	}
	linflexd_gcr.Initialize(0x0);
	linflexd_uartpto.Initialize(LINFlexD_UARTPTO::PTO::template MakeValue<uint32_t>(~uint32_t(0)));
	linflexd_uartcto.Initialize(0x0);
	linflexd_dmatxe.Initialize(0x0); 
	linflexd_dmarxe.Initialize(0x0);
	linflexd_ptd.Initialize(0x0);
	
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
void LINFlexD<CONFIG>::UpdateINT_RX()
{
	gen_int_rx_event.notify();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_TX()
{
	gen_int_tx_event.notify();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_ERR()
{
	gen_int_err_event.notify();
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
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ClockPropertiesChangedProcess()
{
	// Clock properties have changed
	UpdateMasterClock();
	UpdateLINClock();
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
void LINFlexD<CONFIG>::TX_Process()
{
	
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RX_Process()
{
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
	         (linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>());
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_TX_Process()
{
	INT_TX = (linflexd_linier.template Get<typename LINFlexD_LINIER::DTIE>()      && linflexd_linsr.template Get<typename LINFlexD_LINSR::DTF>()) ||
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
	          (linflexd_linier.template Get<typename LINFlexD_LINIER::DBEIETOIE>() && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::TO>());
}

} // end of namespace linflexd
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__
