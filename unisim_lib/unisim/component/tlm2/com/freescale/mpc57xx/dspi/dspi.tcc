/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_TCC__

#include <unisim/component/tlm2/com/freescale/mpc57xx/dspi/dspi.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace dspi {

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::NUM_CTARS;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TX_FIFO_DEPTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::RX_FIFO_DEPTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::CMD_FIFO_DEPTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::NUM_SLAVE_CONFIGURATIONS;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::SPI_CTAR_SLAVE_NUM;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::DSI_CTAR_SLAVE_NUM;

template <typename CONFIG>
const bool DSPI<CONFIG>::threaded_model;

template <typename CONFIG>
DSPI<CONFIG>::DSPI(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, peripheral_slave_if("peripheral_slave_if")
	, SOUT("SOUT")
	, SIN("SIN")
	, m_clk("m_clk")
	, dspi_clk("dspi_clk")
	, reset_b("reset_b")
	, debug("debug")
	, HT("HT")
	, DSI_INPUT("DSI_INPUT", NUM_DSI_INPUTS)
	, DSI_OUTPUT("DSI_OUTPUT", NUM_DSI_OUTPUTS)
	, INT_EOQF("INT_EOQF")  
	, INT_TFFF("INT_TFFF")  
	, INT_CMDFFF("INT_CMDFFF")
	, INT_TFIWF("INT_TFIWF") 
	, INT_TCF("INT_TCF")   
	, INT_CMDTCF("INT_CMDTCF")
	, INT_SPITCF("INT_SPITCF")
	, INT_DSITCF("INT_DSITCF")
	, INT_TFUF("INT_TFUF")  
	, INT_RFDF("INT_RFDF")  
	, INT_RFOF("INT_RFOF")  
	, INT_SPEF("INT_SPEF")  
	, INT_DPEF("INT_DPEF")  
	, INT_DDIF("INT_DDIF")  
	, DMA_RX("DMA_RX")
	, DMA_TX("DMA_TX")
	, DMA_CMD("DMA_CMD")
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, dspi_clk_prop_proxy(dspi_clk)
	, dspi_mcr(this)
	, dspi_tcr(this)
	, dspi_ctar(this)
	, dspi_ctar_slave(this)
	, dspi_sr(this)
	, dspi_rser(this)
	, dspi_pushr(this)
	, dspi_pushr_slave(this)
	, dspi_popr(this)
	, dspi_txfr(this)
	, dspi_rxfr(this)
	, dspi_dsicr0(this)
	, dspi_sdr0(this)
	, dspi_asdr0(this)
	, dspi_compr0(this)
	, dspi_ddr0(this)
	, dspi_dsicr1(this)
	, dspi_ssr0(this)
	, dspi_dimr0(this)
	, dspi_dpir0(this)
	, dspi_ctare(this)
	, dspi_srex(this)
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
	, dspi_clock_period(10.0, sc_core::SC_NS)          
	, dspi_clock_start_time(sc_core::SC_ZERO_TIME)
	, dspi_clock_posedge_first(true)
	, dspi_clock_duty_cycle(0.5)
{
	peripheral_slave_if(*this);
	
	SOUT.register_nb_receive(this, &DSPI<CONFIG>::nb_receive, SOUT_IF);
	SIN.register_nb_receive(this, &DSPI<CONFIG>::nb_receive, SIN_IF);

	std::stringstream description_sstr;
	description_sstr << "MPC57XX DSPI:" << std::endl;
	description_sstr << "  - " << NUM_CTARS << " CTARs" << std::endl;
	description_sstr << "  - TX FIFO Depth of " << TX_FIFO_DEPTH << std::endl;
	description_sstr << "  - RX FIFO Depth of " << RX_FIFO_DEPTH << std::endl;
	description_sstr << "  - CMD FIFO Depth of " << CMD_FIFO_DEPTH << std::endl;
	if(HAS_DATA_SERIALIZATION_SUPPORT)
	{
		description_sstr << "  - Data Serialization support" << std::endl;
	}
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 49" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	SC_HAS_PROCESS(DSPI);

	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	
	MapRegisters();

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
}

template <typename CONFIG>
DSPI<CONFIG>::~DSPI()
{
}

template <typename CONFIG>
void DSPI<CONFIG>::end_of_elaboration()
{
	Reset();
}

template <typename CONFIG>
void DSPI<CONFIG>::Reset()
{
	unsigned int i;
	
	dspi_mcr.Reset();
	dspi_tcr.Reset();
	for(i = 0; i < NUM_CTARS; i++)
	{
		dspi_ctar[i].Reset();
		switch(i)
		{
			case SPI_CTAR_SLAVE_NUM:
			case DSI_CTAR_SLAVE_NUM:
				dspi_ctar_slave[i].Reset();
				break;
		}
	}
	dspi_sr.Reset();
	dspi_rser.Reset();
	dspi_pushr.Reset();
	dspi_pushr_slave.Reset();
	dspi_popr.Reset();
	dspi_txfr.Reset();
	dspi_rxfr.Reset();
	dspi_dsicr0.Reset();
	dspi_sdr0.Reset();
	dspi_asdr0.Reset();
	dspi_compr0.Reset();
	dspi_ddr0.Reset();
	dspi_dsicr1.Reset();
	dspi_ssr0.Reset();
	dspi_dimr0.Reset();
	dspi_dpir0.Reset();
	for(i = 0; i < NUM_CTARS; i++)
	{
		dspi_ctare[i].Reset();
	}
	dspi_srex.Reset();
}

template <typename CONFIG>
void DSPI<CONFIG>::MapRegisters()
{
	// Map DSPI registers regarding there address offsets
	reg_addr_map.Clear();
	reg_addr_map.MapRegister(dspi_mcr.ADDRESS_OFFSET, &dspi_mcr);  
	reg_addr_map.MapRegister(dspi_tcr.ADDRESS_OFFSET, &dspi_tcr);
	unsigned int i;
	for(i = 0; i < NUM_CTARS; i++)
	{
		switch(i)
		{
			case SPI_CTAR_SLAVE_NUM:
			case DSI_CTAR_SLAVE_NUM:
				if(SlaveMode()) // slave mode?
				{
					// slave mode
					reg_addr_map.MapRegister(DSPI_CTAR::ADDRESS_OFFSET + (4 * i), &dspi_ctar_slave[i]);
					break;
				}
			default:
				reg_addr_map.MapRegister(DSPI_CTAR::ADDRESS_OFFSET + (4 * i), &dspi_ctar[i]);
				break;
		}
	}
	reg_addr_map.MapRegister(dspi_sr  .ADDRESS_OFFSET, &dspi_sr  );
	reg_addr_map.MapRegister(dspi_rser.ADDRESS_OFFSET, &dspi_rser);
	if(MasterMode()) // master mode?
	{
		// master mode
		reg_addr_map.MapRegister(dspi_pushr      .ADDRESS_OFFSET, &dspi_pushr      );
	}
	else
	{
		reg_addr_map.MapRegister(dspi_pushr_slave.ADDRESS_OFFSET, &dspi_pushr_slave);
	}
	reg_addr_map.MapRegister(dspi_popr      .ADDRESS_OFFSET, &dspi_popr  );
	reg_addr_map.MapRegister(dspi_txfr      .ADDRESS_OFFSET, &dspi_txfr  );
	reg_addr_map.MapRegister(dspi_rxfr      .ADDRESS_OFFSET, &dspi_rxfr  );
	reg_addr_map.MapRegister(dspi_dsicr0    .ADDRESS_OFFSET, &dspi_dsicr0);
	reg_addr_map.MapRegister(dspi_sdr0      .ADDRESS_OFFSET, &dspi_sdr0  );
	reg_addr_map.MapRegister(dspi_asdr0     .ADDRESS_OFFSET, &dspi_asdr0 );
	reg_addr_map.MapRegister(dspi_compr0    .ADDRESS_OFFSET, &dspi_compr0);
	reg_addr_map.MapRegister(dspi_ddr0      .ADDRESS_OFFSET, &dspi_ddr0  );
	reg_addr_map.MapRegister(dspi_dsicr1    .ADDRESS_OFFSET, &dspi_dsicr1);
	reg_addr_map.MapRegister(dspi_ssr0      .ADDRESS_OFFSET, &dspi_ssr0  );
	reg_addr_map.MapRegister(dspi_dimr0     .ADDRESS_OFFSET, &dspi_dimr0 );
	reg_addr_map.MapRegister(dspi_dpir0     .ADDRESS_OFFSET, &dspi_dpir0 );
	reg_addr_map.MapRegisterFile(DSPI_CTARE::ADDRESS_OFFSET, &dspi_ctare );
	reg_addr_map.MapRegister(dspi_srex      .ADDRESS_OFFSET, &dspi_srex  );
}

template <typename CONFIG>
bool DSPI<CONFIG>::MasterMode() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::MSTR>() != 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::SlaveMode() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::MSTR>() == 0;
}

template <typename CONFIG>
void DSPI<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool DSPI<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum DSPI<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
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
void DSPI<CONFIG>::nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload)
{
// 	if(verbose)
// 	{
// 		logger << DebugInfo << sc_core::sc_time_stamp() << ": receiving [";
// 		std::vector<bool>::size_type data_length = data.size();
// 		std::vector<bool>::size_type i;
// 		for(i = 0; i < data_length; i++)
// 		{
// 			if(i != 0) logger << " ";
// 			logger << data[i];
// 		}
// 		logger << "] over SIN with a period of " << period << EndDebugInfo;
// 	}
}

template <typename CONFIG>
void DSPI<CONFIG>::ProcessEvent(Event *event)
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
void DSPI<CONFIG>::ProcessEvents()
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
void DSPI<CONFIG>::Process()
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


} // end of namespace dspi
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_TCC__
