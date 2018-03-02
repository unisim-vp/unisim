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

using unisim::kernel::tlm2::tlm_input_bitstream_sync_status;
using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_SYNC_OK;
using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_NEED_SYNC;

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
	, SS_b("SS_b")
	, m_clk("m_clk")
	, dspi_clk("dspi_clk")
	, reset_b("reset_b")
	, debug("debug")
	, HT("HT")
	, DSI_INPUT("DSI_INPUT", NUM_DSI_INPUTS)
	, PCS("PCS", NUM_CTARS)
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
	, txbackptr(0)
	, rxbackptr(0)
	, cmdbackptr(0)
	, rxpart(0)
	, prev_dconf(DCONF_RES)
	, curr_dconf(DCONF_RES)
	, prev_cmd(0)
	, cmd(0)
	, cmd_cycling_cnt(0)
	, trigger_spi(false)
	, trigger_dsi(false)
	, sin_bitstream()
	, trigger_event("trigger_event")
	, gen_int_eoqf_event("gen_int_eoqf_event")
	, gen_int_tfff_event("gen_int_tfff_event")
	, gen_int_cmdfff_event("gen_int_cmdfff_event")
	, gen_int_tfiwf_event("gen_int_tfiwf_event")
	, gen_int_tcf_event("gen_int_tcf_event")
	, gen_int_cmdtcf_event("gen_int_cmdtcf_event")
	, gen_int_spitcf_event("gen_int_spitcf_event")
	, gen_int_dsitcf_event("gen_int_dsitcf_event")
	, gen_int_tfuf_event("gen_int_tfuf_event")
	, gen_int_rfdf_event("gen_int_rfdf_event")
	, gen_int_rfof_event("gen_int_rfof_event")
	, gen_int_spef_event("gen_int_spef_event")
	, gen_int_dpef_event("gen_int_dpef_event")
	, gen_int_ddif_event("gen_int_ddif_event")
	, gen_dma_rx_event("gen_dma_rx_event")
	, gen_dma_tx_event("gen_dma_tx_event")
	, gen_dma_cmd_event("gen_dma_cmd_event")
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
	
	SIN.register_nb_receive(this, &DSPI<CONFIG>::nb_receive, SIN_IF);
	SS_b.register_nb_receive(this, &DSPI<CONFIG>::nb_receive, SS_IF);

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
	
	SC_METHOD(INT_EOQF_Process);
	sensitive << gen_int_eoqf_event;
	
	SC_METHOD(INT_TFFF_Process);
	sensitive << gen_int_tfff_event;
	
	SC_METHOD(INT_CMDFFF_Process);
	sensitive << gen_int_cmdfff_event;
	
	SC_METHOD(INT_TFIWF_Process); 
	sensitive << gen_int_tfiwf_event;
	
	SC_METHOD(INT_TCF_Process); 
	sensitive << gen_int_tcf_event;
	
	SC_METHOD(INT_CMDTCF_Process);
	sensitive << gen_int_cmdtcf_event;
	
	SC_METHOD(INT_SPITCF_Process);
	sensitive << gen_int_spitcf_event;
	
	SC_METHOD(INT_DSITCF_Process);
	sensitive << gen_int_dsitcf_event;
	
	SC_METHOD(INT_TFUF_Process);  
	sensitive << gen_int_tfuf_event;
	
	SC_METHOD(INT_RFDF_Process);
	sensitive << gen_int_rfdf_event;
	
	SC_METHOD(INT_RFOF_Process);
	sensitive << gen_int_rfof_event;
	
	SC_METHOD(INT_SPEF_Process);
	sensitive << gen_int_spef_event;
	
	SC_METHOD(INT_DPEF_Process);
	sensitive << gen_int_dpef_event;

	SC_METHOD(INT_DDIF_Process);
	sensitive << gen_int_ddif_event;
	
	SC_METHOD(DMA_RX_Process);
	sensitive << gen_dma_rx_event;
	
	SC_METHOD(DMA_TX_Process);
	sensitive << gen_dma_tx_event;
	
	SC_METHOD(DMA_CMD_Process);
	sensitive << gen_dma_cmd_event;
	
	SC_METHOD(HT_Process);
	sensitive << HT;
	
	SC_THREAD(TransferProcess);
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
	for(i = 0; i < TX_FIFO_DEPTH; i++)
	{
		dspi_txfr[i].Reset();
	}
	for(i = 0; i < RX_FIFO_DEPTH; i++)
	{
		dspi_rxfr[i].Reset();
	}
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
	
	txbackptr = 0;
	rxbackptr = 0;
	cmdbackptr = 0;
	rxpart = 0;
	prev_cmd = 0;
	cmd = 0;
	cmd_cycling_cnt = 0;
	trigger_spi = false;
	trigger_dsi = false;
	prev_dconf = DCONF_RES;
	curr_dconf = DCONF_RES;
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
		reg_addr_map.MapRegister(dspi_pushr.ADDRESS_OFFSET, &dspi_pushr);
	}
	else
	{
		reg_addr_map.MapRegister(dspi_pushr_slave.ADDRESS_OFFSET, &dspi_pushr_slave);
	}
	
	reg_addr_map.MapRegister(dspi_popr.ADDRESS_OFFSET, &dspi_popr);
	
	if(!dspi_mcr.template Get<typename DSPI_MCR::DIS_TXF>())
	{
		reg_addr_map.MapRegisterFile(DSPI_TXFR ::ADDRESS_OFFSET, &dspi_txfr);
	}
	if(!dspi_mcr.template Get<typename DSPI_MCR::DIS_RXF>())
	{
		reg_addr_map.MapRegisterFile(DSPI_RXFR ::ADDRESS_OFFSET, &dspi_rxfr);
	}
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
bool DSPI<CONFIG>::ExtendedMode() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::XSPI>() != 0;
}

template <typename CONFIG>
DSPI_Configuration DSPI<CONFIG>::Configuration() const
{
	return DSPI_Configuration(dspi_mcr.template Get<typename DSPI_MCR::DCONF>());
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::FrameSize(DSPI_Configuration dconf, unsigned int ctar_num) const
{
	switch(dconf)
	{
		case DCONF_SPI:
		{
			unsigned int fmsz = dspi_ctar[ctar_num].template Get<typename DSPI_CTAR::FMSZ>();
			if(ExtendedMode())
			{
				unsigned int fmsze = dspi_ctare[ctar_num].template Get<typename DSPI_CTARE::FMSZE>();
				return fmsz + (fmsze << DSPI_CTAR::FMSZ::BITWIDTH) + 1;
			}
			
			return fmsz + 1;
		}
		
		case DCONF_DSI:
		{
			unsigned int fmsz = dspi_ctar[ctar_num].template Get<typename DSPI_CTAR::FMSZ>();
			unsigned int fmsz4 = dspi_dsicr0.template Get<typename DSPI_DSICR0::FMSZ4>();
			return fmsz + (fmsz4 << DSPI_CTAR::FMSZ::BITWIDTH) + 1;
		}
		
		default:
			assert(false);
	}
	
	return 0;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::TX_FIFO_Depth() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::DIS_TXF>() ? 1 : TX_FIFO_DEPTH;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::CMD_FIFO_Depth() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::DIS_TXF>() ? 1 : CMD_FIFO_DEPTH;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::RX_FIFO_Depth() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::DIS_RXF>() ? 1 : RX_FIFO_DEPTH;
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Master_TX_FIFO_Push(uint16_t txcmd, uint16_t txdata)
{
	assert(MasterMode());
	assert(!ExtendedMode());
	DSPI_Configuration dconf = Configuration();
	switch(dconf)
	{
		case DCONF_SPI:
		case DCONF_CSI:
		{
			unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
			if(txctr < TX_FIFO_Depth())
			{
				// Fill-in TX FIFO
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXCMD_TXDATA>(txcmd);
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
				txbackptr = (txbackptr + 1) % TX_FIFO_Depth();
				
				// Increment TX FIFO Counter
				txctr++;
				dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
				
				TriggerSPI();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push command/data to a full TX FIFO" << EndDebugWarning;
			}
			break;
		}
			
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push command/data" << EndDebugWarning;
			break;
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::XSPI_Master_TX_FIFO_Push(uint16_t txdata)
{
	assert(MasterMode());
	assert(ExtendedMode());
	DSPI_Configuration dconf = Configuration();
	switch(dconf)
	{
		case DCONF_SPI:
		case DCONF_CSI:
		{
			unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
			if(txctr < TX_FIFO_Depth())
			{
				// Fill-in TX FIFO
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
				txbackptr = (txbackptr + 1) % TX_FIFO_Depth();
				
				// Increment TX FIFO Counter
				txctr++;
				dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
				
				TriggerSPI();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push data to a full TX FIFO" << EndDebugWarning;
			}
			break;
		}
		
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push data" << EndDebugWarning;
			break;
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::XSPI_Master_CMD_FIFO_Push(uint16_t txcmd)
{
	assert(MasterMode());
	assert(ExtendedMode());
	DSPI_Configuration dconf = Configuration();
	switch(dconf)
	{
		case DCONF_SPI:
		case DCONF_CSI:
		{
			unsigned int cmdctr = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>();
			if(cmdctr < CMD_FIFO_Depth())
			{
				// Fill-in CMD FIFO
				dspi_txfr[cmdbackptr].template Set<typename DSPI_TXFR::TXCMD_TXDATA>(txcmd);
				cmdbackptr = (cmdbackptr + 1) % CMD_FIFO_Depth();
				
				// Increment CMD FIFO counter
				cmdctr++;
				dspi_srex.template Set<typename DSPI_SREX::CMDCTR>(cmdctr);
				
				TriggerSPI();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": While in " << dconf << ", master attempt to push cmd to a full CMD FIFO" << EndDebugWarning;
			}
			break;
		}
		
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push command" << EndDebugWarning;
			break;
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Slave_TX_FIFO_Push(uint16_t txdata)
{
	assert(SlaveMode());
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
	if(txctr < TX_FIFO_Depth())
	{
		if(FrameSize(DCONF_SPI, SPI_CTAR_SLAVE_NUM) > 16)
		{
			if(rxpart)
			{
				// Fill-in TX FIFO with 16 MSB bits
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXCMD_TXDATA>(txdata);
				txbackptr = (txbackptr + 1) % TX_FIFO_Depth();
				
				// Increment TX FIFO Counter
				txctr++;
				dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
				
				TriggerSPI();
			}
			else
			{
				// Fill-in TX FIFO with 16 LSB bits
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
			}
			
			rxpart ^= 1;
		}
		else
		{
			// Fill-in TX FIFO
			dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
			txbackptr = (txbackptr + 1) % TX_FIFO_Depth();
			
			// Increment TX FIFO Counter
			txctr++;
			dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::TX_FIFO_Clear()
{
	dspi_sr.template Set<typename DSPI_SR::TXCTR>(0);
	dspi_srex.template Set<typename DSPI_SREX::CMDCTR>(0);
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::TX_FIFO_Size() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXCTR>();
}

template <typename CONFIG>
bool DSPI<CONFIG>::TX_FIFO_Empty() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXCTR>() == 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::CMD_FIFO_Empty() const
{
	return dspi_srex.template Get<typename DSPI_SREX::CMDCTR>() == 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::TX_FIFO_Full() const
{
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
	return txctr >= TX_FIFO_Depth();
}

template <typename CONFIG>
bool DSPI<CONFIG>::CMD_FIFO_Full() const
{
	unsigned int cmdctr = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>();
	return cmdctr >= CMD_FIFO_Depth();
}

template <typename CONFIG>
uint16_t DSPI<CONFIG>::SPI_Master_TX_FIFO_Front() const
{
	unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>();
	return dspi_txfr[txnxtptr].template Get<typename DSPI_TXFR::TXDATA>();
}

template <typename CONFIG>
uint16_t DSPI<CONFIG>::SPI_Master_CMD_FIFO_Front() const
{
	if(ExtendedMode())
	{
		unsigned int cmdnxtptr = dspi_srex.template Get<typename DSPI_SREX::CMDNXTPTR>();
		return dspi_txfr[cmdnxtptr].template Get<typename DSPI_TXFR::TXCMD_TXDATA>();
	}
	else
	{
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>();
		return dspi_txfr[txnxtptr].template Get<typename DSPI_TXFR::TXCMD_TXDATA>();
	}
}

template <typename CONFIG>
uint32_t DSPI<CONFIG>::SPI_Slave_TX_FIFO_Front() const
{
	unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>();
	return dspi_txfr[txnxtptr];
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Master_TX_FIFO_Pop()
{
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
	if(txctr)
	{
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>();
		txnxtptr = (txnxtptr + 1) % TX_FIFO_Depth();
		txctr--;
		dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
		dspi_sr.template Set<typename DSPI_SR::TXNXTPTR>(txnxtptr);
		
		if(txctr) TriggerSPI();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::XSPI_Master_CMD_FIFO_Pop()
{
	unsigned int cmdctr = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>();
	if(cmdctr)
	{
		unsigned int cmdnxtptr = dspi_sr.template Get<typename DSPI_SREX::CMDNXTPTR>();
		cmdnxtptr = (cmdnxtptr + 1) % CMD_FIFO_Depth();
		cmdctr--;
		dspi_srex.template Set<typename DSPI_SREX::CMDCTR>(cmdctr);
		dspi_srex.template Set<typename DSPI_SREX::CMDNXTPTR>(cmdnxtptr);
	}
	else
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": master attempts to pop data from an empty RX FIFO are ignored and the RX FIFO Counter remains unchanged; The data read from the empty RX FIFO is undetermined." << EndDebugWarning;
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Slave_TX_FIFO_Pop()
{
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
	if(txctr)
	{
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>();
		txnxtptr = (txnxtptr + 1) % TX_FIFO_Depth();
		txctr--;
		dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
		dspi_sr.template Set<typename DSPI_SR::TXNXTPTR>(txnxtptr);
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::RX_FIFO_Clear()
{
	dspi_sr.template Set<typename DSPI_SR::RXCTR>(0);
}

template <typename CONFIG>
bool DSPI<CONFIG>::RX_FIFO_Empty() const
{
	return dspi_sr.template Get<typename DSPI_SR::RXCTR>() == 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::RX_FIFO_Full() const
{
	unsigned int rxctr = dspi_sr.template Get<typename DSPI_SR::RXCTR>();
	return rxctr >= RX_FIFO_Depth();
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_RX_FIFO_Push(uint32_t rxdata)
{
	unsigned int rxctr = dspi_sr.template Get<typename DSPI_SR::RXCTR>();
	
	if(rxctr < RX_FIFO_Depth())
	{
		dspi_rxfr[rxbackptr].template Set<typename DSPI_RXFR::RXDATA>(rxdata);
		rxbackptr = (rxbackptr + 1) % RX_FIFO_Depth();
	}
}

template <typename CONFIG>
uint32_t DSPI<CONFIG>::SPI_RX_FIFO_Front()
{
	unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>();
	return dspi_rxfr[popnxtptr].template Get<typename DSPI_RXFR::RXDATA>();
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_RX_FIFO_Pop()
{
	unsigned int rxctr = dspi_sr.template Get<typename DSPI_SR::RXCTR>();
	
	if(rxctr)
	{
		unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>();
		popnxtptr = (popnxtptr + 1) % RX_FIFO_Depth();
		rxctr--;
		dspi_sr.template Set<typename DSPI_SR::RXCTR>(rxctr);
		dspi_sr.template Set<typename DSPI_SR::POPNXTPTR>(popnxtptr);
	}
}


template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_EOQF()
{
	gen_int_eoqf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TFFF()
{
	gen_int_tfff_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_CMDFFF()
{
	gen_int_cmdfff_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TFIWF()
{
	gen_int_tfiwf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TCF()
{
	gen_int_tcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_CMDTCF()
{
	gen_int_cmdtcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_SPITCF()
{
	gen_int_spitcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_DSITCF()
{
	gen_int_dsitcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TFUF()
{
	gen_int_tfuf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_RFDF()
{
	gen_int_rfdf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_RFOF()
{
	gen_int_rfof_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_SPEF()
{
	gen_int_spef_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_DPEF()
{
	gen_int_dpef_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_DDIF()
{
	gen_int_ddif_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_RX()
{
	gen_dma_rx_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_TX()
{
	gen_dma_tx_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_CMD()
{
	gen_dma_cmd_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDSPIClock()
{
	dspi_clock_period = dspi_clk_prop_proxy.GetClockPeriod();
	dspi_clock_start_time = dspi_clk_prop_proxy.GetClockStartTime();
	dspi_clock_posedge_first = dspi_clk_prop_proxy.GetClockPosEdgeFirst();
	dspi_clock_duty_cycle = dspi_clk_prop_proxy.GetClockDutyCycle();
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
void DSPI<CONFIG>::nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	switch(id)
	{
		case SIN_IF:
			if(verbose)
			{
				const unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
				const sc_core::sc_time& period = payload.get_period();
				
				logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": receiving [" << data << "] over SIN with a period of " << period << EndDebugInfo;
			}
			
			sin_bitstream.fill(payload, t);
			break;
		
		case SS_IF:
			if(verbose)
			{
				const unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
				const sc_core::sc_time& period = payload.get_period();
				
				logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": receiving [" << data << "] over SS_b with a period of " << period << EndDebugInfo;
			}
			
			ss_bitstream.fill(payload, t);
			break;
	}
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

template <typename CONFIG>
void DSPI<CONFIG>::INT_EOQF_Process()
{
	bool eoqf = dspi_sr.template Get<typename DSPI_SR::EOQF>();
	bool eoqf_re = dspi_rser.template Get<typename DSPI_RSER::EOQF_RE>();
	bool int_eoqf = eoqf && eoqf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_EOQF.name() << " <- " << int_eoqf << EndDebugInfo;
	}
	
	INT_EOQF = int_eoqf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TFFF_Process()
{
	bool tfff = dspi_sr.template Get<typename DSPI_SR::TFFF>();
	bool tfff_re = dspi_rser.template Get<typename DSPI_RSER::TFFF_RE>();
	bool tfff_dirs = dspi_rser.template Get<typename DSPI_RSER::TFFF_DIRS>();
	bool int_tfff = !tfff_dirs && tfff && tfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TFFF.name() << " <- " << int_tfff << EndDebugInfo;
	}
	
	INT_TFFF = int_tfff;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_CMDFFF_Process()
{
	bool cmdfff = dspi_sr.template Get<typename DSPI_SR::CMDFFF>();
	bool cmdfff_re = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_RE>();
	bool cmdfff_dirs = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_DIRS>();
	bool int_cmdfff = !cmdfff_dirs && cmdfff && cmdfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_CMDFFF.name() << " <- " << int_cmdfff << EndDebugInfo;
	}
	
	INT_CMDFFF = int_cmdfff;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TFIWF_Process()
{
	bool tfiwf = dspi_sr.template Get<typename DSPI_SR::TFIWF>();
	bool tfiwf_re = dspi_rser.template Get<typename DSPI_RSER::TFIWF_RE>();
	bool int_tfiwf = tfiwf && tfiwf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TFIWF.name() << " <- " << int_tfiwf << EndDebugInfo;
	}
	
	INT_TFIWF = int_tfiwf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TCF_Process()
{
	bool tcf = dspi_sr.template Get<typename DSPI_SR::TCF>();
	bool tcf_re = dspi_rser.template Get<typename DSPI_RSER::TCF_RE>();
	bool int_tcf = tcf && tcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TCF.name() << " <- " << int_tcf << EndDebugInfo;
	}
	
	INT_TCF = int_tcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_CMDTCF_Process()
{
	bool cmdtcf = dspi_sr.template Get<typename DSPI_SR::CMDTCF>();
	bool cmdtcf_re = dspi_rser.template Get<typename DSPI_RSER::CMDTCF_RE>();
	bool int_cmdtcf = cmdtcf && cmdtcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_CMDTCF.name() << " <- " << int_cmdtcf << EndDebugInfo;
	}
	
	INT_CMDTCF = int_cmdtcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_SPITCF_Process()
{
	bool spitcf = dspi_sr.template Get<typename DSPI_SR::SPITCF>();
	bool spitcf_re = dspi_rser.template Get<typename DSPI_RSER::SPITCF_RE>();
	bool int_spitcf = spitcf && spitcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_SPITCF.name() << " <- " << int_spitcf << EndDebugInfo;
	}
	
	INT_SPITCF = int_spitcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_DSITCF_Process()
{
	bool dsitcf = dspi_sr.template Get<typename DSPI_SR::DSITCF>();
	bool dsitcf_re = dspi_rser.template Get<typename DSPI_RSER::DSITCF_RE>();
	bool int_dsitcf = dsitcf && dsitcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_DSITCF.name() << " <- " << int_dsitcf << EndDebugInfo;
	}
	
	INT_DSITCF = int_dsitcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TFUF_Process()
{
	bool tfuf = dspi_sr.template Get<typename DSPI_SR::TFUF>();
	bool tfuf_re = dspi_rser.template Get<typename DSPI_RSER::TFUF_RE>();
	bool int_tfuf = tfuf && tfuf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TFUF.name() << " <- " << int_tfuf << EndDebugInfo;
	}
	
	INT_TFUF = int_tfuf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_RFDF_Process()
{
	bool rfdf = dspi_sr.template Get<typename DSPI_SR::RFDF>();
	bool rfdf_re = dspi_rser.template Get<typename DSPI_RSER::RFDF_RE>();
	bool rfdf_dirs = dspi_rser.template Get<typename DSPI_RSER::RFDF_DIRS>();
	bool int_rfdf = !rfdf_dirs && rfdf && rfdf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_RFDF.name() << " <- " << int_rfdf << EndDebugInfo;
	}
	
	INT_RFDF = int_rfdf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_RFOF_Process()
{
	bool rfof = dspi_sr.template Get<typename DSPI_SR::RFOF>();
	bool rfof_re = dspi_rser.template Get<typename DSPI_RSER::RFOF_RE>();
	bool int_rfof = rfof && rfof_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_RFOF.name() << " <- " << int_rfof << EndDebugInfo;
	}
	
	INT_RFOF = int_rfof;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_SPEF_Process()
{
	bool spef = dspi_sr.template Get<typename DSPI_SR::SPEF>();
	bool spef_re = dspi_rser.template Get<typename DSPI_RSER::SPEF_RE>();
	bool int_spef = spef && spef_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_SPEF.name() << " <- " << int_spef << EndDebugInfo;
	}
	
	INT_SPEF = int_spef;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_DPEF_Process()
{
	bool dpef = dspi_sr.template Get<typename DSPI_SR::DPEF>();
	bool dpef_re = dspi_rser.template Get<typename DSPI_RSER::DPEF_RE>();
	bool int_dpef = dpef && dpef_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_DPEF.name() << " <- " << int_dpef << EndDebugInfo;
	}
	
	INT_DPEF = int_dpef;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_DDIF_Process()
{
	bool ddif = dspi_sr.template Get<typename DSPI_SR::DDIF>();
	bool ddif_re = dspi_rser.template Get<typename DSPI_RSER::DDIF_RE>();
	bool int_ddif = ddif && ddif_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_DDIF.name() << " <- " << int_ddif << EndDebugInfo;
	}
	
	INT_DDIF = int_ddif;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_RX_Process()
{
	bool rfdf = dspi_sr.template Get<typename DSPI_SR::RFDF>();
	bool rfdf_re = dspi_rser.template Get<typename DSPI_RSER::RFDF_RE>();
	bool rfdf_dirs = dspi_rser.template Get<typename DSPI_RSER::RFDF_DIRS>();
	bool dma_rx = rfdf_dirs && rfdf && rfdf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_RX.name() << " <- " << dma_rx << EndDebugInfo;
	}
	
	DMA_RX = dma_rx;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_TX_Process()
{
	bool tfff = dspi_sr.template Get<typename DSPI_SR::TFFF>();
	bool tfff_re = dspi_rser.template Get<typename DSPI_RSER::TFFF_RE>();
	bool tfff_dirs = dspi_rser.template Get<typename DSPI_RSER::TFFF_DIRS>();
	bool dma_tx = tfff_dirs && tfff && tfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_TX.name() << " <- " << dma_tx << EndDebugInfo;
	}
	
	DMA_TX = dma_tx;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_CMD_Process()
{
	bool cmdfff = dspi_sr.template Get<typename DSPI_SR::CMDFFF>();
	bool cmdfff_re = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_RE>();
	bool cmdfff_dirs = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_DIRS>();
	bool dma_cmd = cmdfff_dirs && cmdfff && cmdfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_CMD.name() << " <- " << dma_cmd << EndDebugInfo;
	}
	
	DMA_CMD = dma_cmd;
}

template <typename CONFIG>
void DSPI<CONFIG>::TriggerSPI()
{
	trigger_spi = true;
	Trigger();
}

template <typename CONFIG>
void DSPI<CONFIG>::TriggerDSI()
{
	trigger_dsi = true;
	Trigger();
}

template <typename CONFIG>
void DSPI<CONFIG>::Trigger()
{
	sc_core::sc_time notify_delay(transfer_ready_time);
	unisim::kernel::tlm2::AlignToClock(notify_delay, dspi_clock_period, dspi_clock_start_time, dspi_clock_posedge_first, dspi_clock_duty_cycle);
	notify_delay -= sc_core::sc_time_stamp();
		
	trigger_event.notify(notify_delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::CheckChangeInData()
{
	bool cid = dspi_dsicr0.template Get<typename DSPI_DSICR0::CID>();
	bool txss = dspi_dsicr0.template Get<typename DSPI_DSICR0::TXSS>();
	
	if(cid && ((txss ? uint32_t(dspi_asdr0) : uint32_t(dspi_sdr0)) != uint32_t(dspi_compr0)))
	{
		TriggerDSI();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::HT_Process()
{
	bool trre = dspi_dsicr0.template Get<typename DSPI_DSICR0::TRRE>();

	if(trre)
	{
		bool tpol = dspi_dsicr0.template Get<typename DSPI_DSICR0::TPOL>();
		
		if(tpol)
		{
			// rising edge
			if(HT.posedge()) TriggerDSI();
		}
		else
		{
			// falling edge
			if(HT.negedge()) TriggerDSI();
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::LatchDSI_INPUT()
{
	unsigned int i;
	for(i = 0; i < NUM_DSI_INPUTS; i++)
	{
		const sc_core::sc_in<bool>& dsi_input = DSI_INPUT[i];
		dspi_sdr0.Set(i, dsi_input.read());
	}
	
	CheckChangeInData();
}

template <typename CONFIG>
void DSPI<CONFIG>::MasterTransfer(uint32_t& shift_reg, const TransferControl& ctrl)
{
	bool cont = ctrl.cont;
	unsigned int ctas = ctrl.ctas;
	bool mtfe = dspi_mcr.template Get<typename DSPI_MCR::MTFE>();
	bool lsbfe = dspi_ctar[ctas].template Get<typename DSPI_CTAR::LSBFE>();
	bool cpha = ctrl.cpha;
	bool pe = ctrl.pe;
	bool pp = ctrl.pp;
	unsigned int frame_size = ctrl.frame_size;
	bool mcsc = ctrl.mcsc;
	bool masc = ctrl.masc;
	unsigned int pcs = ctrl.pcs;
	
	const DSPI_CTAR& ctar = dspi_ctar[ctas];
	
	
	const sc_core::sc_time& sck_period = ctar.GetSCKPeriod();
	const sc_core::sc_time& sck_half_period = ctar.GetSCKHalfPeriod();
	const sc_core::sc_time& t_DT = ctar.GetDelayAfterTransfer();
	const sc_core::sc_time& t_CSC = ctar.GetPCS2SCKDelay();
	const sc_core::sc_time& t_ASC = ctar.GetAfterSCKDelay();
	
	// Continous selection format (controlled by DSPI_MCR[CONT]): no t_DT between frames, slaves keep selected
	
	// Fast continous selection format (controlled by DSPI_MCR[FCPCS])
	//  - no t_CSC at start of next frame (controlled by CMD[MCSC]),
	//  - no t_CSC at start of next frame (controlled by CMD[MCSC]) and no t_ASC at end of current frame (controlled by CMD[MASC])
	
	// Select Slaves
	sc_core::sc_time sof_time = sc_core::sc_time_stamp();
	
	sc_core::sc_time pcs_time(sck_half_period);
	pcs_time *= (double)((2 * frame_size) - 1);
	
	if(mcsc && masc)
	{
		// Masking of both tASC and tCSC delays is allowed. In this case, the delay between two
		// frames is equal to half the baud rate set by the user software.
		pcs_time += sck_period;
		pcs_time += sck_period;
	}
	else
	{
		if(!mcsc) pcs_time += t_CSC;
		if(!masc) pcs_time += t_ASC;
	}
	
	unsigned int pcsis = dspi_mcr.template Get<typename DSPI_MCR::PCSIS>(); // Peripheral Chip Select Inactive State
	
	unsigned int slave_num;
	for(slave_num = 0; slave_num < NUM_CTARS; slave_num++)
	{
		bool pcs_value = ((pcs & (1 << slave_num)) != 0);
		SetPCS(slave_num, pcs_time, pcsis ? !pcs_value : pcs_value);
	}

	// Transmit frame
	unisim::kernel::tlm2::tlm_serial_payload outcoming_payload;
	
	outcoming_payload.set_period(sck_period);
	
	unisim::kernel::tlm2::tlm_serial_payload::data_type& outcoming_data = outcoming_payload.get_data();
	
	bool parity_bit = pp; // initialize parity bit
	
	sc_core::sc_time tx_time = sc_core::SC_ZERO_TIME;
	
	unsigned int i;
	for(i = 0; i < frame_size; i++)
	{
		if(!pe || (i < (frame_size - 1)))
		{
			// transmit a data bit
			bool bit_value = lsbfe ? (shift_reg & 1) : ((shift_reg >> (frame_size - 1)) & 1); // get data bit from shift register
			
			outcoming_data.push_back(bit_value); // transmit data bit
			
			// shift
			if(lsbfe)
			{
				shift_reg >>= 1;
			}
			else
			{
				shift_reg <<= 1;
			}
			
			parity_bit ^= bit_value; // update computed parity bit
		}
		else
		{
			// transmit parity bit
			outcoming_data.push_back(parity_bit);
		}
		
		tx_time += sck_period;
	}
	
	sc_core::sc_time transmit_time = cpha ? t_CSC : sc_core::SC_ZERO_TIME; // master places its first data on SOUT pin at the first SCK edge (now or after t_CSC if CPHA=1)
	SOUT->nb_send(outcoming_payload, transmit_time);

	// Receive frame
	sc_core::sc_time sample_point(t_CSC);
	
	if(cpha)
	{
		sample_point += sck_half_period; // CPHA=1: sample point is after one half SCK period
	}

	if(mtfe)
	{
		// If SCK/protocol clock ratio is less than 4, the actually sample
		// point is delayed by 1 protocol clock cycle automatically by the design.
		unsigned int pbr = ctar.template Get<typename DSPI_CTAR::PBR>();
		unsigned int dbr = ctar.template Get<typename DSPI_CTAR::DBR>();
		unsigned int br = ctar.template Get<typename DSPI_CTAR::BR>();
		sample_point += ((pbr * (1 + dbr)) >= (4 * br)) ? dspi_mcr.GetSamplePoint() : dspi_clock_period;
	}

	wait(sample_point); // Slave sends the response though nb_receive call back in that interval
	sin_bitstream.latch();
	
	sc_core::sc_time rx_time = sc_core::SC_ZERO_TIME;
	
	parity_bit = pp; // initialize parity bit
	
	for(i = 0; i < frame_size; i++)
	{
		if(sin_bitstream.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
		{
			logger << DebugWarning << (sc_core::sc_time_stamp() + rx_time) << ": missing bit #" << i << " from slave" << EndDebugWarning;
		}
		
		bool bit_value = sin_bitstream.read(); // sample SIN
		
		if(!pe || (i < (frame_size - 1)))
		{
			// receive a data bit
			
			// insert data bit in shift register
			shift_reg |= lsbfe ? bit_value : (1 << (frame_size - 1));
			
			// shift
			if(lsbfe)
			{
				shift_reg <<= 1;
			}
			else
			{
				shift_reg >>= 1;
			}
			
			parity_bit ^= bit_value; // update computed parity bit
		}
		else
		{
			// receive parity bit
			
			if(parity_bit != bit_value)
			{
				if(verbose)
				{
					logger << DebugInfo << (sc_core::sc_time_stamp() + rx_time) << ": parity error" << EndDebugInfo;
				}
			}
		}
		
		rx_time += sck_period;
	}
	
	assert(tx_time == rx_time);
	assert(tx_time < pcs_time);
	
	transfer_ready_time = sof_time;
	transfer_ready_time += pcs_time;
	
	if(!cont) // continuous selection format?
	{
		// no: deselect slaves
		for(slave_num = 0; slave_num < NUM_CTARS; slave_num++)
		{
			SetPCS(slave_num, t_DT, pcsis);
		}
		
		transfer_ready_time += t_DT;
	}
}

template <typename CONFIG>
DSPI_Configuration DSPI<CONFIG>::SelectConfiguration()
{
	DSPI_Configuration configuration = Configuration();
	
	switch(configuration)
	{
		case DCONF_CSI:
			{
				bool tsbc = dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>();
				if(tsbc) // timed serial bus configuration enabled?
				{
					bool itsb = dspi_dsicr0.template Get<typename DSPI_DSICR0::ITSB>();
					if(itsb) // interleave TSB mode enabled?
					{
						// On every trigger, frames from DSI are sent when there
						// are no frames in the SPI or the previous transmission was a frame from SPI
						return ((prev_dconf == DCONF_SPI) || !trigger_spi) ? DCONF_DSI : DCONF_SPI;
					}
					else
					{
						// TSB configuration: transmission of SPI data has higher priority than DSI data
						return trigger_spi ? DCONF_SPI : DCONF_DSI;
					}
				}
				else
				{
					// The normal CSI configuration is a combination of the SPI and DSI configuration.
					// The DSPI interleaves DSI data frames with SPI data frames.
					// Interleaving is done on the frame boundaries.
					if(trigger_spi && trigger_dsi)
					{
						return (prev_dconf == DCONF_SPI) ? DCONF_DSI : DCONF_SPI;
					}
					else if(trigger_spi)
					{
						return DCONF_SPI;
					}
					else if(trigger_dsi)
					{
						return DCONF_DSI;
					}
				}
			}
			
		default:
			return configuration;
	}
	
	return DCONF_RES;
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_MasterTransfer()
{
	bool has_cmd = false;
	
	if(ExtendedMode()) // XSPI?
	{
		// XSPI mode
		if(cmd_cycling_cnt) // is a command cycling in progress?
		{
			// yes: reuse command
			has_cmd = true;
		}
		else if(!CMD_FIFO_Empty()) // is CMD FIFO empty?
		{
			// no: pop command from CMD FIFO
			cmd = SPI_Master_CMD_FIFO_Front();
			XSPI_Master_CMD_FIFO_Pop();
			has_cmd = true;
		}
	}
	else
	{
		// SPI mode
		if(!TX_FIFO_Empty()) // is CMD FIFO empty?
		{
			// no: get command from CMD FIFO
			cmd = SPI_Master_CMD_FIFO_Front();
			has_cmd = true;
		}
	}
	
	if(has_cmd)
	{
		TransferControl ctrl;
		
		bool transfer = false;
		
		bool fcpcs = dspi_mcr.template Get<typename DSPI_MCR::FCPCS>();                             // fast continous PCS mode
		ctrl.cont = DSPI_PUSHR::CONT::template Get<uint32_t>(cmd << 16);                            // continuous peripheral chip select
		bool prev_cont = DSPI_PUSHR::CONT::template Get<uint32_t>(prev_cmd << 16);                  // continuous peripheral chip select
		bool cont_posedge = (!prev_cont && ctrl.cont);                                              // first frame in the continuous selection format?
		bool cont_negedge = (prev_cont && !ctrl.cont);                                              // last frame in the continuous selection format?
		
		bool scke = dspi_mcr.template Get<typename DSPI_MCR::CONT_SCKE>();
		
		ctrl.ctas = DSPI_PUSHR::CTAS::template Get<uint32_t>(cmd << 16);                            // clock and transfer attribute select

		bool masc = DSPI_PUSHR::PE_MASC::template Get<uint32_t>(cmd << 16);                         // mask tASC delay in the current frame
		bool mcsc = DSPI_PUSHR::PP_MCSC::template Get<uint32_t>(prev_cmd << 16);                    // mask tCSC delay in the next frame (from previous command)
		
		if(fcpcs && !ctrl.cont && (mcsc || masc))
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": software must not mask the \"After SCK\" (tASC) delay or the \"PCS to SCK\" (tCSC) delay if the continuous selection format is not used and " << dspi_mcr.GetName() << "[FCPCS] is asserted" << EndDebugWarning;
		}
	
		if(ctrl.cont && fcpcs) // continuous selection format?
		{
			if(unlikely(masc && cont_negedge))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": the \"After SCK\" (tASC) delay must not be masked when the current frame is the last frame in the continuous selection format" << EndDebugWarning;
			}
			
			if(unlikely(mcsc && cont_posedge))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": the \"PCS to SCK\" (tCSC) delay for the first frame in the continuous selection format cannot be masked" << EndDebugWarning;
			}
			
			if(unlikely(masc && !mcsc))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": masking of only tASC is not allowed; If tASC is masked then tCSC must be masked too" << EndDebugWarning;
			}
			
			if(unlikely(mcsc && !masc && (dspi_ctar[ctrl.ctas].GetAfterSCKDelay() < dspi_ctar[ctrl.ctas].GetSCKPeriod())))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": when masking only tCSC, software must ensure that the tASC time is greater than the baud rate" << EndDebugWarning;
			}
			
			ctrl.mcsc = mcsc && !cont_posedge;
			ctrl.masc = mcsc && masc && !cont_negedge;
		}
		else
		{
			// classic SPI transfer format
			ctrl.mcsc = false;
			ctrl.masc = false;
		}
		
		ctrl.pcs = DSPI_PUSHR::PCS::template Get<uint32_t>(cmd << 16);                              // peripheral chip select
		ctrl.pe = (!ctrl.cont || !fcpcs) && DSPI_PUSHR::PE_MASC::template Get<uint32_t>(cmd << 16); // parity enable
		ctrl.pp = DSPI_PUSHR::PP_MCSC::template Get<uint32_t>(cmd << 16);                           // parity polarity
		ctrl.cpha = scke || dspi_ctar[ctrl.ctas].template Get<typename DSPI_CTAR::CPHA>();          // clock phase
		
		prev_cmd = cmd;
		
		if(ExtendedMode() && !cmd_cycling_cnt)
		{
			cmd_cycling_cnt = dspi_ctare[ctrl.ctas].template Get<typename DSPI_CTARE::DTCP>(); // data transfer count preload
		}
		
		uint32_t shift_reg = 0;
		unsigned int frame_size = FrameSize(DCONF_SPI, ctrl.ctas);
		ctrl.frame_size = frame_size;
	
		if(frame_size >= 16)
		{
			// When Extended SPI Mode (DSPIx_MCR[XSPI]) is enabled and the frame size of SPI
			// Data to be transmitted is more than 16 bits, then two Data entries are popped from TX
			// FIFO simultaneously and transferred to the shift register. The first of the two popped
			// entries forms the 16 LSB bits of the SPI frame. Such an operation also causes TX FIFO
			// Counter to decrement by two.
		
			if(TX_FIFO_Size() >= 2)
			{
				uint16_t lsb_txdata = SPI_Master_TX_FIFO_Front();
				SPI_Master_TX_FIFO_Pop();
				
				uint16_t msb_txdata = SPI_Master_TX_FIFO_Front();
				SPI_Master_TX_FIFO_Pop();
				
				shift_reg = (uint32_t) lsb_txdata | ((uint32_t) msb_txdata << 16);
				
				transfer = true;
			}
			else
			{
				// Command with no data
				dspi_sr.template Set<typename DSPI_SR::TFIWF>(1); // Transmit FIFO Invalid Write interrupt request
				UpdateINT_TFIWF();
			}
		}
		else if(!TX_FIFO_Empty())
		{
			uint16_t txdata = SPI_Master_TX_FIFO_Front();
			SPI_Master_TX_FIFO_Pop();
			
			shift_reg = (uint32_t) txdata;
			
			transfer = true;
		}
		else
		{
			// Command with no data
			dspi_sr.template Set<typename DSPI_SR::TFIWF>(1); // Transmit FIFO Invalid Write interrupt request
			UpdateINT_TFIWF();
		}
		
		if(transfer)
		{
			MasterTransfer(shift_reg, ctrl);
			
			if(RX_FIFO_Full())
			{
				if(verbose)
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": Receive FIFO overflow" << EndDebugInfo;
				}
			}
			else
			{
				SPI_RX_FIFO_Push(shift_reg);
			}
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_MasterTransfer()
{
	TransferControl ctrl;
	
	bool txss = dspi_dsicr0.template Get<typename DSPI_DSICR0::TXSS>();
	bool tsbc = dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>();
	bool scke = dspi_mcr.template Get<typename DSPI_MCR::CONT_SCKE>();
	
	ctrl.cont = dspi_dsicr0.template Get<typename DSPI_DSICR0::DCONT>();
	ctrl.ctas = dspi_dsicr0.template Get<typename DSPI_DSICR0::DSICTAS>();
	ctrl.pcs = dspi_dsicr0.template Get<typename DSPI_DSICR0::DPCSx>();
	ctrl.mcsc = false;
	ctrl.masc = false;
	ctrl.pe = dspi_dsicr0.template Get<typename DSPI_DSICR0::PE>();
	ctrl.pp = dspi_dsicr0.template Get<typename DSPI_DSICR0::PP>();
	ctrl.cpha = scke || tsbc || dspi_ctar[ctrl.ctas].template Get<typename DSPI_CTAR::CPHA>();
	ctrl.frame_size = FrameSize(DCONF_DSI, ctrl.ctas);
	
	uint32_t shift_reg = txss ? uint32_t(dspi_asdr0) : uint32_t(dspi_sdr0);

	dspi_compr0 = shift_reg;
	
	MasterTransfer(shift_reg, ctrl);
	
	dspi_ddr0 = shift_reg;
}

template <typename CONFIG>
void DSPI<CONFIG>::SlaveTransfer(uint32_t& shift_reg, const TransferControl& ctrl)
{
	unisim::kernel::tlm2::tlm_serial_payload outcoming_payload;
	
	if(!ss_bitstream.read())
	{
		sin_bitstream.seek();
		bool incoming_bit_value = sin_bitstream.read();
		
		const sc_core::sc_time& sck_period = sin_bitstream.get_period();
		
		if(outcoming_payload.get_period() == sc_core::SC_ZERO_TIME)
		{
			outcoming_payload.set_period(sck_period);
		}
		else if(outcoming_payload.get_period() != sck_period)
		{
			// flush output
			
			outcoming_payload.set_period(sck_period);
		}

		unisim::kernel::tlm2::tlm_serial_payload::data_type& outcoming_data = outcoming_payload.get_data();
		
		bool outcoming_bit_value = false;
		
		outcoming_data.push_back(outcoming_bit_value);
		
		
		if(sin_bitstream.next() == TLM_INPUT_BITSTREAM_NEED_SYNC)
		{
			wait(sin_bitstream.event());
			sin_bitstream.latch();
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_SlaveTransfer()
{
	TransferControl ctrl;
	ctrl.ctas = SPI_CTAR_SLAVE_NUM;
	ctrl.pe = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::PE>();
	ctrl.pp = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::PP>();
	ctrl.cpha = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::CPHA>();
	ctrl.frame_size = FrameSize(DCONF_SPI, ctrl.ctas);
	
	uint32_t shift_reg = 0;
	if(TX_FIFO_Empty())
	{
		// TX FIFO underflow
	}
	else
	{
		uint32_t txdata = SPI_Slave_TX_FIFO_Front();
		SPI_Slave_TX_FIFO_Pop();
		
		shift_reg = txdata;
	}
	
	SlaveTransfer(shift_reg, ctrl);
	
	SPI_RX_FIFO_Push(shift_reg);
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_SlaveTransfer()
{
}

template <typename CONFIG>
void DSPI<CONFIG>::TransferProcess()
{
	while(1)
	{
		if(MasterMode())
		{
			// Master Mode
			wait(trigger_event);
			assert(trigger_spi || trigger_dsi);
			LatchDSI_INPUT();
			
			curr_dconf = SelectConfiguration();
			
			switch(curr_dconf)
			{
				case DCONF_SPI:
					SPI_MasterTransfer();
					trigger_spi = false;
					break;
				case DCONF_DSI:
					DSI_MasterTransfer();
					trigger_dsi = false;
					break;
				default:
					assert(false);
			}
		}
		else
		{
			// Slave Mode
			wait(ss_bitstream.event());
			ss_bitstream.latch();
			if(ss_bitstream.negedge())
			{
				curr_dconf = Configuration();
				
				switch(curr_dconf)
				{
					case DCONF_SPI:
						SPI_SlaveTransfer();
						break;
					case DCONF_DSI:
						DSI_SlaveTransfer();
						break;
					default:
						assert(false);
				}
			}
		}
	}
	
#if 0
	while(1)
	{
		unsigned int ctas = 0;
		unsigned int pcs = 0;
		uint32_t shift_reg = 0;
		unsigned int frame_size = 0;
		unsigned int pe = 0;
		unsigned int pp = 0;
		bool transfer = false;
		
		if(MasterMode())
		{
			// Master Mode
			wait(trigger_event);
			assert(trigger_spi || trigger_dsi);
			LatchDSI_INPUT();
			
			DSPI_Configuration configuration = Configuration();
			
			DSPI_Configuration active_configuration = DCONF_RES;
			
			switch(configuration)
			{
				case DCONF_CSI:
					{
						bool tsbc = dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>();
						if(tsbc) // timed serial bus configuration enabled?
						{
							bool itsb = dspi_dsicr0.template Get<typename DSPI_DSICR0::ITSB>();
							if(itsb) // interleave TSB mode enabled?
							{
								// On every trigger, frames from DSI are sent when there
								// are no frames in the SPI or the previous transmission was a frame from SPI
								active_configuration = ((prev_dconf == DCONF_SPI) || !trigger_spi) ? DCONF_DSI : DCONF_SPI;
							}
							else
							{
								// TSB configuration: transmission of SPI data has higher priority than DSI data
								active_configuration = trigger_spi ? DCONF_SPI : DCONF_DSI;
							}
						}
						else
						{
							// The normal CSI configuration is a combination of the SPI and DSI configurationthis configuration the DSPI interleaves DSI data frames with SPI data frames.
							// Interleaving is done on the frame boundaries.
							if(trigger_spi && trigger_dsi)
							{
								active_configuration = (prev_dconf == DCONF_SPI) ? DCONF_DSI : DCONF_SPI;
							}
							else if(trigger_spi)
							{
								active_configuration = DCONF_SPI;
							}
							else if(trigger_dsi)
							{
								active_configuration = DCONF_DSI;
							}
						}
						break;
					}
					
				case DCONF_SPI:
					active_configuration = DCONF_SPI;
					break;
				
				case DCONF_DSI:
					active_configuration = DCONF_DSI;
					break;
					
				default:
					break;
			}

			switch(active_configuration)
			{
				case DCONF_SPI:
				{
					trigger_spi = false;
					bool has_cmd = false;
					
					if(ExtendedMode())
					{
						if(cmd_cycling_cnt)
						{
							has_cmd = true;
						}
						else if(!CMD_FIFO_Empty())
						{
							cmd = SPI_Master_CMD_FIFO_Front();
							XSPI_Master_CMD_FIFO_Pop();
							has_cmd = true;
						}
					}
					else
					{
						if(!TX_FIFO_Empty())
						{
							cmd = SPI_Master_CMD_FIFO_Front();
							has_cmd = true;
						}
					}
					
					if(has_cmd)
					{
						unsigned int fcpcs = dspi_mcr.template Get<typename DSPI_MCR::FCPCS>();
						
						cont = DSPI_PUSHR::CONT::template Get<uint32_t>(cmd << 16);
						ctas = DSPI_PUSHR::CTAS::template Get<uint32_t>(cmd << 16);
						pcs = DSPI_PUSHR::PCS::template Get<uint32_t>(cmd << 16);
						pe = (!cont || !fcpcs) ? DSPI_PUSHR::PE_MASC::template Get<uint32_t>(cmd << 16);
						pp = DSPI_PUSHR::PP_MCSC::template Get<uint32_t>(cmd << 16);
					
						if(ExtendedMode() && !cmd_cycling_cnt)
						{
							cmd_cycling_cnt = dspi_ctare[ctas].template Get<typename DSPI_CTARE::DTCP>();
						}
						
						frame_size = FrameSize(DCONF_SPI, ctas);
					
						if(frame_size >= 16)
						{
							// When Extended SPI Mode (DSPIx_MCR[XSPI]) is enabled and the frame size of SPI
							// Data to be transmitted is more than 16 bits, then two Data entries are popped from TX
							// FIFO simultaneously and transferred to the shift register. The first of the two popped
							// entries forms the 16 LSB bits of the SPI frame. Such an operation also causes TX FIFO
							// Counter to decrement by two.
						
							if(TX_FIFO_Size() >= 2)
							{
								uint16_t lsb_txdata = SPI_Master_TX_FIFO_Front();
								SPI_Master_TX_FIFO_Pop();
								
								uint16_t msb_txdata = SPI_Master_TX_FIFO_Front();
								SPI_Master_TX_FIFO_Pop();
								
								shift_reg = (uint32_t) lsb_txdata | ((uint32_t) msb_txdata << 16);
								
								transfer = true;
							}
							else
							{
								// Command with no data
								dspi_sr.template Set<typename DSPI_SR::TFIWF>(1); // Transmit FIFO Invalid Write interrupt request
								UpdateINT_TFIWF();
							}
						}
						else if(!TX_FIFO_Empty())
						{
							uint16_t txdata = SPI_Master_TX_FIFO_Front();
							SPI_Master_TX_FIFO_Pop();
							
							shift_reg = (uint32_t) txdata;
							
							transfer = true;
						}
						else
						{
							// Command with no data
							dspi_sr.template Set<typename DSPI_SR::TFIWF>(1); // Transmit FIFO Invalid Write interrupt request
							UpdateINT_TFIWF();
						}
					}
					break;
				}
					
				case DCONF_DSI:
				{
					trigger_dsi = false;
					ctas = dspi_dsicr0.template Get<typename DSPI_DSICR0::DSICTAS>();
					pcs = dspi_dsicr0.template Get<typename DSPI_DSICR0::DPCSx>();
					unsigned int txss = dspi_dsicr0.template Get<typename DSPI_DSICR0::TXSS>();
					
					frame_size = FrameSize(DCONF_DSI, ctas);
					
					shift_reg = txss ? uint32_t(dspi_asdr0) : uint32_t(dspi_sdr0);

					dspi_compr0 = shift_reg;
					
					transfer = true;
					
					break;
				}
					
				default:
					break;
			}
			
			if(transmit)
			{
				prev_dconf = active_configuration;
			}
			
			bool bsyf = false;
					
			if(transmit && MasterMode() && ExtendedMode() && (cmd_cycling_cnt != 0))
			{
				bsyf = (--cmd_cycling_cnt != 0);
			}
			
			dspi_sr.template Set<typename DSPI_SR::BSYF>(bsyf); // Update Busy Flag
			
			unsigned int pcsis = dspi_mcr.template Get<typename DSPI_MCR::PCSIS>();
			
			for(i = 0; i < NUM_CTARS; i++)
			{
				bool pcs_value = ((pcs & (1 << i)) != 0)
				PCS[i] = pcsis ? pcs_value : !pcs_value;
			}
			
			wait(sc_core::SC_ZERO_TIME);

			const sc_core::sc_time& sck_period = dspi_ctar[ctas].GetSCKPeriod();

			unsigned int lsbfe = dspi_ctar[ctas].template Get<typename DSPI_CTAR::LSBFE>();
			
			unisim::kernel::tlm2::tlm_serial_payload outcoming_payload;
			
			outcoming_payload.set_period(sck_period);
			
			unisim::kernel::tlm2::tlm_serial_payload::data_type& outcoming_data = outcoming_payload.get_data();
			
			sc_core::sc_time transfer_time = sc_core::SC_ZERO_TIME;
			
			unsigned int parity_bit = pp;
			
			unsigned int i;
			for(i = 0; i < frame_size; i++)
			{
				if(!pe || (i < (frame_size - 1)))
				{
					bool bit_value = lsbfe ? (shift_reg & 1) : ((shift_reg >> (frame_size - 1)) & 1);
					parity_bit ^= bit_value;
					
					data.push_back(bit_value);
					
					if(lsbfe)
					{
						shift_reg >>= 1;
					}
					else
					{
						shift_reg <<= 1;
					}
					
					transfer_time += sck_period;
				}
			}
			
			if(pe)
			{
				data.push_back(parity_bit);
				transfer_time += sck_period;
			}
			
			SOUT->b_send(payload);
			
			sc_core::sc_time t;
			
			wait(sc_core::SC_ZERO_TIME);
			
			shift_reg = 0;
			
			for(i = 0; i < frame_size; i++)
			{
				bool bit_value = sin_bitstream.read();
				
				shift_reg |= lsbfe ? bit_value : (1 << (frame_size - 1));
				
				if(lsbfe)
				{
					shift_reg <<= 1;
				}
				else
				{
					shift_reg >>= 1;
				}

				t += sck_period;
				
				if(sin_bitstream.seek(t) == TLM_INPUT_BITSTREAM_NEED_SYNC)
				{
					wait(t);
					t = sc_core::SC_ZERO_TIME;
				}
			}
			
			if(t != sc_core::SC_ZERO_TIME)
			{
				wait(t);
				t = sc_core::SC_ZERO_TIME;
			}
		}
		else
		{
			// Slave mode
			wait(SS_b.event());
			
			if(!SS_b.read())
			{
				// Slave Select
				DSPI_Configuration configuration = Configuration();

				switch(configuration)
				{
					case DCONF_SPI:
						ctas = SPI_CTAR_SLAVE_NUM;
						if(TX_FIFO_Empty())
						{
							// Transmit FIFO underflow
							dspi_sr.template Set<typename DSPI_SR::TFUF>(1);
							UpdateINT_TFUF();
						}
						else
						{
							transfer = true;
						}
						break;
					case DCONF_DSI:
						ctas = DSI_CTAR_SLAVE_NUM;
						transfer = true;
						break;
					default:
						break;
				}
				
				dspi_sr.template Set<typename DSPI_SR::BSYF>(0); // Update Busy Flag
			
			frame_size = FrameSize(configuration, ctas);
			pe = dspi_ctar_slave[ctas].template Get<typename DSPI_CTAR_SLAVE::PE>();
			pp = dspi_ctar_slave[ctas].template Get<typename DSPI_CTAR_SLAVE::PP>();
			
			if(sin_bitstream.next() == TLM_INPUT_BITSTREAM_NEED_SYNC)
			{
				wait(sin_bitstream.event());
			}
		}

		
		const sc_core::sc_time& sck_period = MasterMode() ? dspi_ctar[ctas].GetSCKPeriod() : sin_bitstream.get_period();
		
		if(transfer)
		{
			if(MasterMode())
			{
				unsigned int pcsis = dspi_mcr.template Get<typename DSPI_MCR::PCSIS>();
				
				for(i = 0; i < NUM_CTARS; i++)
				{
					bool pcs_value = ((pcs & (1 << i)) != 0)
					PCS[i] = pcsis ? pcs_value : !pcs_value;
				}
				
				wait(sc_core::SC_ZERO_TIME);
			}
			
			unsigned int lsbfe = dspi_ctar[ctas].template Get<typename DSPI_CTAR::LSBFE>();
			
			unisim::kernel::tlm2::tlm_serial_payload outcoming_payload;
			
			outcoming_payload.set_period(sck_period);
			
			unisim::kernel::tlm2::tlm_serial_payload::data_type& outcoming_data = outcoming_payload.get_data();
			
			sc_core::sc_time transfer_time = sc_core::SC_ZERO_TIME;
			
			unsigned int parity_bit = pp;
			
			unsigned int i;
			for(i = 0; i < frame_size; i++)
			{
				if(!pe || (i < (frame_size - 1)))
				{
					bool bit_value = lsbfe ? (shift_reg & 1) : ((shift_reg >> (frame_size - 1)) & 1);
					parity_bit ^= bit_value;
					
					data.push_back(bit_value);
					
					if(lsbfe)
					{
						shift_reg >>= 1;
					}
					else
					{
						shift_reg <<= 1;
					}
					
					transfer_time += sck_period;
				}
			}
			
			if(pe)
			{
				data.push_back(parity_bit);
				transfer_time += sck_period;
			}
			
			SOUT->b_send(payload);
			
			sc_core::sc_time t;
			
			wait(sc_core::SC_ZERO_TIME);
			
			shift_reg = 0;
			
			for(i = 0; i < frame_size; i++)
			{
				bool bit_value = sin_bitstream.read();
				
				shift_reg |= lsbfe ? bit_value : (1 << (frame_size - 1));
				
				if(lsbfe)
				{
					shift_reg <<= 1;
				}
				else
				{
					shift_reg >>= 1;
				}

				t += sck_period;
				
				if(sin_bitstream.seek(t) == TLM_INPUT_BITSTREAM_NEED_SYNC)
				{
					wait(t);
					t = sc_core::SC_ZERO_TIME;
				}
			}
			
			if(t != sc_core::SC_ZERO_TIME)
			{
				wait(t);
				t = sc_core::SC_ZERO_TIME;
			}
		}
		
	}
#endif
}

template <typename CONFIG>
void DSPI<CONFIG>::SetPCS(unsigned int slave_num, const sc_core::sc_time& t, bool value)
{
	if(value != pcs_sigs[slave_num])
	{
		
		unisim::kernel::tlm2::tlm_serial_payload payload;
		payload.set_period(t);
		unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
		data.push_back(value);
		
		pcs_sigs[slave_num] = value;
		PCS[slave_num]->nb_send(payload);
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
