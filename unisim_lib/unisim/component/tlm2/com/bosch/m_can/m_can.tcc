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

#ifndef __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_TCC__

#include <unisim/component/tlm2/com/bosch/m_can/m_can.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace bosch {
namespace m_can {

using unisim::kernel::tlm2::TLM_CAN_CORE_SYNCHRONIZING;
using unisim::kernel::tlm2::TLM_CAN_CORE_IDLE;
using unisim::kernel::tlm2::TLM_CAN_CORE_RECEIVER;
using unisim::kernel::tlm2::TLM_CAN_CORE_TRANSMITTER;
using unisim::kernel::tlm2::TLM_CAN_IDLE_PHASE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_ACTIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_ACTIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_PASSIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_BUS_OFF_STATE;
using unisim::kernel::tlm2::TLM_CAN_NO_ERROR;
using unisim::kernel::tlm2::TLM_CAN_STUFF_ERROR;
using unisim::kernel::tlm2::TLM_CAN_FORM_ERROR;
using unisim::kernel::tlm2::TLM_CAN_ACK_ERROR;
using unisim::kernel::tlm2::TLM_CAN_BIT1_ERROR;
using unisim::kernel::tlm2::TLM_CAN_BIT0_ERROR;
using unisim::kernel::tlm2::TLM_CAN_CRC_ERROR;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool M_CAN<CONFIG>::threaded_model;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::ERROR_WARNING_LIMIT;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_DEDICATED_TX_BUFFERS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_TX_QUEUE_FIFO_SIZE;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_TX_BUFFERS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_STD_MSG_ID_FILTER_ELEMENTS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_XTD_MSG_ID_FILTER_ELEMENTS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_RX_FIFO_WATERMARK;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_RX_FIFO_SIZE;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_TX_EVENT_FIFO_SIZE;

template <typename CONFIG>
M_CAN<CONFIG>::M_CAN(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: Super(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, can_clk("can_clk")
	, reset_b("reset_b")
	, DMA_ACK("DMA_ACK")
	, can_message_ram_if("can_message_ram_if")
	, INT0("INT0")
	, INT1("INT1")
	, DMA_REQ("DMA_REQ")
	, m_clk_prop_proxy(m_clk)
	, can_clk_prop_proxy(can_clk)
	, m_can_crel(this)
	, m_can_endn(this)
	, m_can_fbtp(this)
	, m_can_test(this)
	, m_can_rwd(this)
	, m_can_cccr(this)
	, m_can_btp(this)
	, m_can_tscc(this)
	, m_can_tscv(this)
	, m_can_tocc(this)
	, m_can_tocv(this)
	, m_can_ecr(this)
	, m_can_psr(this)
	, m_can_ir(this)
	, m_can_ie(this)
	, m_can_ils(this)
	, m_can_ile(this)
	, m_can_gfc(this)
	, m_can_sidfc(this)
	, m_can_xidfc(this)
	, m_can_xidam(this)
	, m_can_hpms(this)
	, m_can_ndat1(this)
	, m_can_ndat2(this)
	, m_can_rxf0c(this)
	, m_can_rxf0s(this)
	, m_can_rxf0a(this)
	, m_can_rxbc(this)
	, m_can_rxf1c(this)
	, m_can_rxf1s(this)
	, m_can_rxf1a(this)
	, m_can_rxesc(this)
	, m_can_txbc(this)
	, m_can_txfqs(this)
	, m_can_txesc(this)
	, m_can_txbrp(this)
	, m_can_txbar(this)
	, m_can_txbcr(this)
	, m_can_txbto(this)
	, m_can_txbcf(this)
	, m_can_txbtie(this)
	, m_can_txbcie(this)
	, m_can_txefc(this)
	, m_can_txefs(this)
	, m_can_txefa(this)
	, gen_int0_event("gen_int0_event")
	, gen_int1_event("gen_int1_event")
	, gen_dma_req_event("gen_dma_req_event")
	, reg_addr_map()
	, schedule()
	, endian(unisim::util::endian::E_LITTLE_ENDIAN) // the subsystem follows the little-endian format
	, param_endian("endian", this, endian, "endian")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, can_clock_period(10.0, sc_core::SC_NS)          
	, can_clock_start_time(sc_core::SC_ZERO_TIME)
	, can_clock_posedge_first(true)
	, can_clock_duty_cycle(0.5)
	, max_time_to_next_timers_run(1.0, sc_core::SC_MS)
	, last_timers_run_time()
	, start_request(false)
	, init_request(false)
	, core_config()
	, phase(TLM_CAN_IDLE_PHASE)
	, can_error(TLM_CAN_NO_ERROR)
	, can_error_changed(false)
	, tx_scan_event("tx_scan_event")
	, tx_event_store_event("tx_event_store_event")
	, rx_filter_event("rx_filter_event")
	, flip_flop(0)
	, tx_msg_pipe()
	, pending_tx_msg(0)
	, curr_tx_msg(0)
	, rx_msg()
{
	peripheral_slave_if(*this);
	can_message_ram_if(*this);

	std::stringstream description_sstr;
	description_sstr << "Bosch M_CAN Controller Area Network:" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - Endianess: " << endian << std::endl;
	description_sstr << "  - Core release: " << CONFIG::CORE_RELEASE << '.' << CONFIG::CORE_RELEASE_STEP << '.' << CONFIG::CORE_RELEASE_SUBSTEP << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: M_CAN Controller Area Network User's Manual, Revision 3.0.2, 14.02.2013" << std::endl;
	
	this->SetDescription(description_sstr.str().c_str());

	SC_HAS_PROCESS(M_CAN);

	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	
	reg_addr_map.MapRegister(m_can_crel  .ADDRESS_OFFSET, &m_can_crel  );
	reg_addr_map.MapRegister(m_can_endn  .ADDRESS_OFFSET, &m_can_endn  );
	reg_addr_map.MapRegister(m_can_fbtp  .ADDRESS_OFFSET, &m_can_fbtp  );
	reg_addr_map.MapRegister(m_can_test  .ADDRESS_OFFSET, &m_can_test  );
	reg_addr_map.MapRegister(m_can_rwd   .ADDRESS_OFFSET, &m_can_rwd   );
	reg_addr_map.MapRegister(m_can_cccr  .ADDRESS_OFFSET, &m_can_cccr  );
	reg_addr_map.MapRegister(m_can_btp   .ADDRESS_OFFSET, &m_can_btp   );
	reg_addr_map.MapRegister(m_can_tscc  .ADDRESS_OFFSET, &m_can_tscc  );
	reg_addr_map.MapRegister(m_can_tscv  .ADDRESS_OFFSET, &m_can_tscv  );
	reg_addr_map.MapRegister(m_can_tocc  .ADDRESS_OFFSET, &m_can_tocc  );
	reg_addr_map.MapRegister(m_can_tocv  .ADDRESS_OFFSET, &m_can_tocv  );
	reg_addr_map.MapRegister(m_can_ecr   .ADDRESS_OFFSET, &m_can_ecr   );
	reg_addr_map.MapRegister(m_can_psr   .ADDRESS_OFFSET, &m_can_psr   );
	reg_addr_map.MapRegister(m_can_ir    .ADDRESS_OFFSET, &m_can_ir    );
	reg_addr_map.MapRegister(m_can_ie    .ADDRESS_OFFSET, &m_can_ie    );
	reg_addr_map.MapRegister(m_can_ils   .ADDRESS_OFFSET, &m_can_ils   );
	reg_addr_map.MapRegister(m_can_ile   .ADDRESS_OFFSET, &m_can_ile   );
	reg_addr_map.MapRegister(m_can_gfc   .ADDRESS_OFFSET, &m_can_gfc   );
	reg_addr_map.MapRegister(m_can_sidfc .ADDRESS_OFFSET, &m_can_sidfc );
	reg_addr_map.MapRegister(m_can_xidfc .ADDRESS_OFFSET, &m_can_xidfc );
	reg_addr_map.MapRegister(m_can_xidam .ADDRESS_OFFSET, &m_can_xidam );
	reg_addr_map.MapRegister(m_can_hpms  .ADDRESS_OFFSET, &m_can_hpms  );
	reg_addr_map.MapRegister(m_can_ndat1 .ADDRESS_OFFSET, &m_can_ndat1 );
	reg_addr_map.MapRegister(m_can_ndat2 .ADDRESS_OFFSET, &m_can_ndat2 );
	reg_addr_map.MapRegister(m_can_rxf0c .ADDRESS_OFFSET, &m_can_rxf0c );
	reg_addr_map.MapRegister(m_can_rxf0s .ADDRESS_OFFSET, &m_can_rxf0s );
	reg_addr_map.MapRegister(m_can_rxf0a .ADDRESS_OFFSET, &m_can_rxf0a );
	reg_addr_map.MapRegister(m_can_rxbc  .ADDRESS_OFFSET, &m_can_rxbc  );
	reg_addr_map.MapRegister(m_can_rxf1c .ADDRESS_OFFSET, &m_can_rxf1c );
	reg_addr_map.MapRegister(m_can_rxf1s .ADDRESS_OFFSET, &m_can_rxf1s );
	reg_addr_map.MapRegister(m_can_rxf1a .ADDRESS_OFFSET, &m_can_rxf1a );
	reg_addr_map.MapRegister(m_can_rxesc .ADDRESS_OFFSET, &m_can_rxesc );
	reg_addr_map.MapRegister(m_can_txbc  .ADDRESS_OFFSET, &m_can_txbc  );
	reg_addr_map.MapRegister(m_can_txfqs .ADDRESS_OFFSET, &m_can_txfqs );
	reg_addr_map.MapRegister(m_can_txesc .ADDRESS_OFFSET, &m_can_txesc );
	reg_addr_map.MapRegister(m_can_txbrp .ADDRESS_OFFSET, &m_can_txbrp );
	reg_addr_map.MapRegister(m_can_txbar .ADDRESS_OFFSET, &m_can_txbar );
	reg_addr_map.MapRegister(m_can_txbcr .ADDRESS_OFFSET, &m_can_txbcr );
	reg_addr_map.MapRegister(m_can_txbto .ADDRESS_OFFSET, &m_can_txbto );
	reg_addr_map.MapRegister(m_can_txbcf .ADDRESS_OFFSET, &m_can_txbcf );
	reg_addr_map.MapRegister(m_can_txbtie.ADDRESS_OFFSET, &m_can_txbtie);
	reg_addr_map.MapRegister(m_can_txbcie.ADDRESS_OFFSET, &m_can_txbcie);
	reg_addr_map.MapRegister(m_can_txefc .ADDRESS_OFFSET, &m_can_txefc );
	reg_addr_map.MapRegister(m_can_txefs .ADDRESS_OFFSET, &m_can_txefs );
	reg_addr_map.MapRegister(m_can_txefa .ADDRESS_OFFSET, &m_can_txefa );

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	SC_METHOD(INT0_Process);
	sensitive << gen_int0_event;
	
	SC_METHOD(INT1_Process);
	sensitive << gen_int1_event;

	SC_METHOD(DMA_REQ_Process);
	sensitive << gen_dma_req_event;
	
	SC_METHOD(DMA_ACK_Process);
	sensitive << DMA_ACK.pos();
}

template <typename CONFIG>
M_CAN<CONFIG>::~M_CAN()
{
}

template <typename CONFIG>
void M_CAN<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&M_CAN<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn CANClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options can_clock_properties_changed_process_spawn_options;
	
	can_clock_properties_changed_process_spawn_options.spawn_method();
	can_clock_properties_changed_process_spawn_options.set_sensitivity(&can_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&M_CAN<CONFIG>::CANClockPropertiesChangedProcess, this), "CANClockPropertiesChangedProcess", &can_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void M_CAN<CONFIG>::Reset()
{
	m_can_crel.Reset();
	m_can_endn.Reset();
	m_can_fbtp.Reset();
	m_can_test.Reset();
	m_can_rwd.Reset();
	m_can_cccr.Reset();
	m_can_btp.Reset();
	m_can_tscc.Reset();
	m_can_tscv.Reset();
	m_can_tocc.Reset();
	m_can_tocv.Reset();
	m_can_ecr.Reset();
	m_can_psr.Reset();
	m_can_ir.Reset();
	m_can_ie.Reset();
	m_can_ils.Reset();
	m_can_ile.Reset();
	m_can_gfc.Reset();
	m_can_sidfc.Reset();
	m_can_xidfc.Reset();
	m_can_xidam.Reset();
	m_can_hpms.Reset();
	m_can_ndat1.Reset();
	m_can_ndat2.Reset();
	m_can_rxf0c.Reset();
	m_can_rxf0s.Reset();
	m_can_rxf0a.Reset();
	m_can_rxbc.Reset();
	m_can_rxf1c.Reset();
	m_can_rxf1s.Reset();
	m_can_rxf1a.Reset();
	m_can_rxesc.Reset();
	m_can_txbc.Reset();
	m_can_txfqs.Reset();
	m_can_txesc.Reset();
	m_can_txbrp.Reset();
	m_can_txbar.Reset();
	m_can_txbcr.Reset();
	m_can_txbto.Reset();
	m_can_txbcf.Reset();
	m_can_txbtie.Reset();
	m_can_txbcie.Reset();
	m_can_txefc.Reset();
	m_can_txefs.Reset();
	m_can_txefa.Reset();
	start_request = false;
	init_request = false;
	phase = TLM_CAN_IDLE_PHASE;
	can_error = TLM_CAN_NO_ERROR;
	can_error_changed = false;
	flip_flop = 0;
	pending_tx_msg = 0;
	curr_tx_msg = 0;
	
	last_timers_run_time = sc_core::sc_time_stamp();
	unisim::kernel::tlm2::AlignToClock(last_timers_run_time, m_can_tscc.GetTimerPeriod());
}

template <typename CONFIG>
void M_CAN<CONFIG>::RequestStart()
{
	if(init_request) init_request = false;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":requesting start (leaving init mode)" << EndDebugInfo;
	}
	
	start_request = true;
}

template <typename CONFIG>
void M_CAN<CONFIG>::RequestInit()
{
	if(start_request) start_request = false;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":requesting init mode" << EndDebugInfo;
	}
	
	init_request = true;
}

template <typename CONFIG>
bool M_CAN<CONFIG>::InitMode() const
{
	return m_can_cccr.template Get<typename M_CAN_CCCR::INIT>();
}

template <typename CONFIG>
bool M_CAN<CONFIG>::ConfigurationChangedEnabled() const
{
	return m_can_cccr.template Get<typename M_CAN_CCCR::CCE>();
}

template <typename CONFIG>
bool M_CAN<CONFIG>::CheckProtectedWrite() const
{
	if(!InitMode() || !ConfigurationChangedEnabled())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":Access to protected write (P) register" << EndDebugWarning;
		return false;
	}
	return true;
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateLastErrorCode()
{
	if(can_error_changed)
	{
		unsigned int lec = LEC_NO_CHANGE;
		
		if(can_error == TLM_CAN_NO_ERROR)
		{
			lec = LEC_NO_ERROR;
		}
		else
		{
			switch(can_error)
			{
				case TLM_CAN_STUFF_ERROR:
					lec = LEC_STUFF_ERROR;
					m_can_ir.template Set<typename M_CAN_IR::STE>(1); // Stuff Error
					break;
				case TLM_CAN_FORM_ERROR :
					lec = LEC_FORM_ERROR;
					m_can_ir.template Set<typename M_CAN_IR::FOE>(1); // Format Error
					break;
				case TLM_CAN_ACK_ERROR  :
					lec = LEC_ACK_ERROR;
					m_can_ir.template Set<typename M_CAN_IR::ACKE>(1); // Acknowledge Error
					break;
				case TLM_CAN_BIT1_ERROR :
					lec = LEC_BIT1_ERROR;
					m_can_ir.template Set<typename M_CAN_IR::BE>(1); // Bit Error
					break;
				case TLM_CAN_BIT0_ERROR :
					lec = LEC_BIT0_ERROR;
					m_can_ir.template Set<typename M_CAN_IR::BE>(1); // Bit Error
					break;
				case TLM_CAN_CRC_ERROR  :
					lec = LEC_CRC_ERROR;
					m_can_ir.template Set<typename M_CAN_IR::CRCE>(1); // CRC Error
					break;
				default:
					break;
			}
			
			UpdateInterrupts();
		}
		
		can_error_changed = false;
		m_can_psr.template Set<typename M_CAN_PSR::LEC>(lec);
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateWarningStatus()
{
	bool old_ew = m_can_psr.template Get<typename M_CAN_PSR::EW>();
	bool new_ew = (m_can_ecr.GetTransmitErrorCount() >= ERROR_WARNING_LIMIT) ||
	              (m_can_ecr.GetReceiveErrorCount() >= ERROR_WARNING_LIMIT);
	
	m_can_psr.template Set<typename M_CAN_PSR::EW>(new_ew);
	
	if(old_ew != new_ew)
	{
		m_can_ir.template Set<typename M_CAN_IR::EW>(1);
		UpdateInterrupts();
	}
}

template <typename CONFIG>
bool M_CAN<CONFIG>::is_enabled() const
{
	return (!m_can_cccr.template Get<typename M_CAN_CCCR::INIT>() || start_request) && !init_request && !m_can_cccr.template Get<typename M_CAN_CCCR::CSR>();
}

template <typename CONFIG>
const tlm_can_core_config& M_CAN<CONFIG>::get_config() const
{
	core_config.reset();
	core_config.set_sample_point(m_can_btp.GetSamplePoint());
	core_config.set_phase_seg2(m_can_btp.GetPhaseSeg2());
	core_config.set_loopback_mode(m_can_cccr.template Get<typename M_CAN_CCCR::TEST>() && m_can_test.template Get<typename M_CAN_TEST::LBCK>());
	core_config.set_bus_monitoring_mode(m_can_cccr.template Get<typename M_CAN_CCCR::MON>());
	core_config.set_automatic_retransmission(!m_can_cccr.template Get<typename M_CAN_CCCR::DAR>());
	core_config.set_restricted_operation_mode(m_can_cccr.template Get<typename M_CAN_CCCR::ASM>());
	return core_config;
}

template <typename CONFIG>
tlm_can_error M_CAN<CONFIG>::get_can_error() const
{
	return can_error;
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_can_error(tlm_can_error _can_error)
{
	can_error_changed = (can_error != _can_error);
	can_error = _can_error;
}

template <typename CONFIG>
tlm_can_core_activity M_CAN<CONFIG>::get_core_activity() const
{
	ACTIVITY act = static_cast<ACTIVITY>(m_can_psr.template Get<typename M_CAN_PSR::ACT>());
	switch(act)
	{
		case ACT_SYNCHRONIZING: return TLM_CAN_CORE_SYNCHRONIZING;
		case ACT_IDLE         : return TLM_CAN_CORE_IDLE;
		case ACT_RECEIVER     : return TLM_CAN_CORE_RECEIVER;
		case ACT_TRANSMITTER  : return TLM_CAN_CORE_TRANSMITTER;
	}
	return TLM_CAN_CORE_IDLE;
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_core_activity(tlm_can_core_activity core_activity)
{
	if(core_activity != get_core_activity())
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "core is now " << core_activity << EndDebugInfo;
		}
	}
	
	ACTIVITY new_act = ACT_IDLE;
	switch(core_activity)
	{
		case TLM_CAN_CORE_SYNCHRONIZING: new_act = ACT_SYNCHRONIZING; break;
		case TLM_CAN_CORE_IDLE         : new_act = ACT_IDLE; break;
		case TLM_CAN_CORE_RECEIVER     : new_act = ACT_RECEIVER; break;
		case TLM_CAN_CORE_TRANSMITTER  : new_act = ACT_TRANSMITTER; break;
	}
	
	unsigned int old_act = m_can_psr.template Get<typename M_CAN_PSR::ACT>();
	if(init_request && (old_act != ACT_IDLE) && (new_act == ACT_IDLE)) // init request (software wrote 1 in CCCR[INIT]) AND node is now idle?
	{
		init_request = false;
		EnterInitMode();
		
		if(m_can_cccr.template Get<typename M_CAN_CCCR::CSR>()) // clock stop request?
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "acknowledging clock stop request" << EndDebugInfo;
			}
			
			m_can_cccr.template Set<typename M_CAN_CCCR::CSA>(1); // acknowledge clock stop request
		}
	}
	else if(start_request && (old_act == ACT_IDLE) && (new_act != ACT_IDLE)) // start request (software wrote 0 in CCCR[INIT]) AND node is no longer idle?
	{
		start_request = false;
		LeaveInitMode();
	}
	
	m_can_psr.template Set<typename M_CAN_PSR::ACT>(new_act);
}

template <typename CONFIG>
tlm_can_phase M_CAN<CONFIG>::get_phase() const
{
	return phase;
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_phase(tlm_can_phase _phase)
{
	if(phase != _phase)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "entering " << _phase << EndDebugInfo;
		}
	}
	phase = _phase;
}

template <typename CONFIG>
uint8_t M_CAN<CONFIG>::get_transmit_error_count() const
{
	return m_can_ecr.GetTransmitErrorCount();
}

template <typename CONFIG>
uint8_t M_CAN<CONFIG>::get_receive_error_count() const
{
	return m_can_ecr.GetReceiveErrorCount();
}

template <typename CONFIG>
tlm_can_state M_CAN<CONFIG>::get_state() const
{
	return m_can_psr.template Get<typename M_CAN_PSR::BO>() ? TLM_CAN_BUS_OFF_STATE
	                                                        : (m_can_psr.template Get<typename M_CAN_PSR::EP>() ? TLM_CAN_ERROR_PASSIVE_STATE
	                                                                                                            : TLM_CAN_ERROR_ACTIVE_STATE);
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_transmit_error_count(uint8_t v)
{
	m_can_ecr.SetTransmitErrorCount(v);
	UpdateWarningStatus();
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_receive_error_count(uint8_t v)
{
	m_can_ecr.SetReceiveErrorCount(v);
	UpdateWarningStatus();
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_state(tlm_can_state state)
{
	if(state != get_state())
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "entering " << state << EndDebugInfo;
		}
	}
	
	bool old_ep = m_can_psr.template Get<typename M_CAN_PSR::EP>();
	bool old_bo = m_can_psr.template Get<typename M_CAN_PSR::BO>();
	bool new_ep = old_ep;
	bool new_bo = old_bo;
	
	switch(state)
	{
		case TLM_CAN_ERROR_ACTIVE_STATE :
			new_ep = 0;
			new_bo = 0;
			break;
		case TLM_CAN_ERROR_PASSIVE_STATE: 
			new_ep = 1;
			new_bo = 0;
			break;
		case TLM_CAN_BUS_OFF_STATE      :
			new_ep = 0;
			new_bo = 1;
			break;
	}
	
	m_can_psr.template Set<typename M_CAN_PSR::EP>(new_ep);
	m_can_psr.template Set<typename M_CAN_PSR::BO>(new_bo);
	
	if(old_bo != new_bo)
	{
		m_can_ir.template Set<typename M_CAN_IR::BO>(1);
		UpdateInterrupts();
		
		if(new_bo)
		{
			RequestInit();
		}
	}
	
	if(old_ep != new_ep)
	{
		m_can_ir.template Set<typename M_CAN_IR::EP>(1);
		UpdateInterrupts();
	}
}

template <typename CONFIG>
bool M_CAN<CONFIG>::has_pending_transmission_request() const
{
	return pending_tx_msg != 0;
}

template <typename CONFIG>
const M_CAN_Message& M_CAN<CONFIG>::fetch_pending_transmission_request()
{
	assert(curr_tx_msg == 0);
	
	curr_tx_msg = pending_tx_msg;
	pending_tx_msg = 0;
	TxScan();
	return *curr_tx_msg;
}

template <typename CONFIG>
M_CAN_Message& M_CAN<CONFIG>::get_receive_message()
{
	rx_msg.Reset();
	return rx_msg;
}

template <typename CONFIG>
void M_CAN<CONFIG>::process_message_event(const tlm_can_message_event<M_CAN_TYPES>& msg_event)
{
	const M_CAN_Message& msg = msg_event.get_message();
	
	switch(msg_event.get_type())
	{
		case TLM_CAN_START_OF_FRAME_TRANSMISSION_EVENT:
			assert(curr_tx_msg == &msg);
			RunTimersToTime(sc_core::sc_time_stamp()); // make run timestamp counter until event time stamp
			curr_tx_msg->SetTimeStamp(m_can_tscv.template Get<typename M_CAN_TSCV::TSC>()); // timestamp message
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":start of frame transmission of message " << msg << EndDebugInfo;
			}
			break;

		case TLM_CAN_START_OF_FRAME_RECEPTION_EVENT:
			assert(&rx_msg == &msg);
			RunTimersToTime(sc_core::sc_time_stamp()); // make run timestamp counter
			rx_msg.SetTimeStamp(m_can_tscv.template Get<typename M_CAN_TSCV::TSC>()); // timestamp message
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":start of frame reception of message " << msg << EndDebugInfo;
			}
			break;
			
		case TLM_CAN_TRANSMISSION_ERROR_EVENT:
			assert(curr_tx_msg == &msg);
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":transmission error of message " << msg << EndDebugInfo;
			}
			UpdateLastErrorCode();
			break;
			
		case TLM_CAN_TRANSMISSION_CANCELLED_EVENT:
		{
			assert(curr_tx_msg == &msg);
			assert(msg.is_transmission_cancelled());
			
			unsigned int tx_buffer_element_index = msg.GetBufferIndex();
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":cancelled transmission of message " << msg << EndDebugInfo;
			}
			
			// Clear corresponding request in Tx Buffer Request Pending Register
			m_can_txbrp.ClearRequest(tx_buffer_element_index);

			// Clear corresponding cancellation request in Tx Buffer Cancellation Request Register
			m_can_txbcr.ClearRequest(tx_buffer_element_index);
			
			// Set corresponding bit in Tx Buffer Cancellation Finished Register
			m_can_txbcf.CancellationFinished(tx_buffer_element_index);
			
			if(m_can_txbcie.IsInterruptEnabled(tx_buffer_element_index)) // Tx Buffer Interrupt enabled?
			{
				m_can_ir.template Set<typename M_CAN_IR::TCF>(1); // Transmission Cancellation Finished
				UpdateInterrupts();
			}
			break;
		}
			
		case TLM_CAN_TRANSMISSION_OCCURRED_EVENT:
		{
			assert(curr_tx_msg == &msg);

			unsigned int tx_buffer_element_index = msg.GetBufferIndex();
			bool event_fifo_control = msg.GetEFC();
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":transmitted message " << msg << EndDebugInfo;
			}
			curr_tx_msg = 0;
			
			if(event_fifo_control) // Store a TX Event?
			{
				StoreTxEvent(msg);
			}
			
			// Clear corresponding request in Tx Buffer Request Pending Register
			m_can_txbrp.ClearRequest(tx_buffer_element_index);
			
			if(msg.is_transmission_cancelled())
			{
				assert(m_can_txbcr.IsCancelled(tx_buffer_element_index));
				
				// Clear corresponding cancellation request in Tx Buffer Cancellation Request Register
				m_can_txbcr.ClearRequest(tx_buffer_element_index);
				
				// Set corresponding bit in Tx Buffer Cancellation Finished Register
				m_can_txbcf.CancellationFinished(tx_buffer_element_index);
			
				if(m_can_txbcie.IsInterruptEnabled(tx_buffer_element_index)) // Tx Buffer Interrupt enabled?
				{
					m_can_ir.template Set<typename M_CAN_IR::TCF>(1); // Transmission Cancellation Finished
					UpdateInterrupts();
				}
			}
			
			// Set corresponding bit in Tx Buffer Transmission Occurred register 
			m_can_txbto.TransmissionOccurred(tx_buffer_element_index);
			
			if(tx_buffer_element_index >= GetNumDedicatedTxBuffers()) // buffer is in Tx Queue/FIFO?
			{
				switch(m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>()) // Tx Queue/FIFO Mode
				{
					case TQFM_FIFO:
						// Tx FIFO
						IncrementTxFIFOGetIndex();
						break;
					
					case TQFM_QUEUE:
						// Tx Queue
						m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQF>(0);     // Tx Queue is not full
						break;
				}
			}
			
			if(m_can_txbtie.IsInterruptEnabled(tx_buffer_element_index)) // Tx Buffer Interrupt enabled?
			{
				m_can_ir.template Set<typename M_CAN_IR::TC>(1); // Transmission Completed
				UpdateInterrupts();
			}
			break;
		}
			
		case TLM_CAN_RECEPTION_ERROR_EVENT:
			assert(&rx_msg == &msg);
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":reception error of message " << msg << EndDebugInfo;
			}
			UpdateLastErrorCode();
			break;
			
		case TLM_CAN_MESSAGE_RECEIVED_EVENT:
		{
			assert(&rx_msg == &msg);
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":received message " << msg << EndDebugInfo;
			}
			Filter(msg);
			break;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::received_bit(bool value)
{
	if(m_can_cccr.template Get<typename M_CAN_CCCR::TEST>()) // Test Mode?
	{
		m_can_test.template Set<typename M_CAN_TEST::RX>(value);
	}
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::get_transmit_pause() const
{
	return m_can_cccr.template Get<typename M_CAN_CCCR::TXP>();
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateCANClock()
{
	can_clock_period = can_clk_prop_proxy.GetClockPeriod();
	can_clock_start_time = can_clk_prop_proxy.GetClockStartTime();
	can_clock_posedge_first = can_clk_prop_proxy.GetClockPosEdgeFirst();
	can_clock_duty_cycle = can_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void M_CAN<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Master Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void M_CAN<CONFIG>::CANClockPropertiesChangedProcess()
{
	// CAN Clock properties have changed
	UpdateCANClock();
}

template <typename CONFIG>
tlm::tlm_sync_enum M_CAN<CONFIG>::nb_transport_bw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		//trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <typename CONFIG>
void M_CAN<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <typename CONFIG>
void M_CAN<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool M_CAN<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum M_CAN<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void M_CAN<CONFIG>::ProcessEvent(Event *event)
{
	switch(event->GetType())
	{
		case EV_NONE:
			break;
		
		case EV_CPU_PAYLOAD:
		{
			tlm::tlm_generic_payload *payload = event->GetPayload();
			sc_core::sc_time time_stamp(event->GetTimeStamp());
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
							rws = reg_addr_map.Write(time_stamp, start_addr, data_ptr, data_length);
							break;
						case tlm::TLM_READ_COMMAND:
							rws = reg_addr_map.Read(time_stamp, start_addr, data_ptr, data_length);
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
void M_CAN<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	RunTimersToTime(time_stamp); // Run timers to time
	
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
	
	ScheduleTimersRun();
}

template <typename CONFIG>
void M_CAN<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			sc_core::sc_module::wait(schedule.GetKernelEvent());
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		sc_core::sc_module::next_trigger(schedule.GetKernelEvent());
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::IncrementTimestampCounter(sc_dt::uint64 delta)
{
	if(delta)
	{
		if(!m_can_cccr.template Get<typename M_CAN_CCCR::INIT>()) // not in init mode?
		{
			uint32_t tss = m_can_tscc.template Get<typename M_CAN_TSCC::TSS>(); // Timestamp select
		
			if(tss == TSS_INCREMENTED) // Timestamp count is incremented according to TSCC[TCP] ?
			{
				uint32_t tsc = m_can_tscv.template Get<typename M_CAN_TSCV::TSC>();
				tsc += delta;
				m_can_tscv.template Set<typename M_CAN_TSCV::TSC>(tsc);
				
				if(tsc == 0) // wrap-around?
				{
					m_can_ir.template Set<typename M_CAN_IR::TSW>(1); // Timestamp wrap-around
					UpdateInterrupts();
				}
			}
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::DecrementTimeoutCounter(sc_dt::uint64 delta)
{
	if(delta)
	{
		if(!m_can_cccr.template Get<typename M_CAN_CCCR::INIT>()) // not in init mode?
		{
			if(m_can_tocc.template Get<typename M_CAN_TOCC::ETOC>()) // Timeout Counter enabled?
			{
				uint32_t toc = m_can_tocv.template Get<typename M_CAN_TOCV::TOC>();
				
				if(toc > 0)
				{
					toc = (toc > delta) ? (toc - delta) : 0;
					m_can_tocv.template Set<typename M_CAN_TOCV::TOC>(toc);
					
					if(toc == 0)
					{
						m_can_ir.template Set<typename M_CAN_IR::TOO>(1); // Timeout Occurred
						UpdateInterrupts();
					}
				}
			}
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::RunTimersToTime(const sc_core::sc_time& time_stamp)
{
	if(time_stamp > last_timers_run_time)
	{
		// Compute the elapsed time since last timestamp counter run
		sc_core::sc_time delay_since_last_timers_run(time_stamp);
		delay_since_last_timers_run -= last_timers_run_time;
		
		// Compute number of timers ticks since last timestamp counter run
		sc_dt::uint64 delta = delay_since_last_timers_run / m_can_tscc.GetTimerPeriod();
		
		if(delta)
		{
			sc_core::sc_time run_time(m_can_tscc.GetTimerPeriod());
			run_time *= delta;
		
			IncrementTimestampCounter(delta);
			DecrementTimeoutCounter(delta);
			delay_since_last_timers_run += run_time;
		}
	}
}

template <typename CONFIG>
sc_dt::uint64 M_CAN<CONFIG>::TicksToNextTimersRun()
{
	bool init = m_can_cccr.template Get<typename M_CAN_CCCR::INIT>(); // init mode
	uint32_t tss = m_can_tscc.template Get<typename M_CAN_TSCC::TSS>();
	uint32_t tsc = m_can_tscv.template Get<typename M_CAN_TSCV::TSC>();
	uint32_t max_tsc = M_CAN_TSCV::TSC::template GetMask<uint32_t>();
	uint32_t toc = m_can_tocv.template Get<typename M_CAN_TOCV::TOC>();
	bool etoc = m_can_tocc.template Get<typename M_CAN_TOCC::ETOC>(); // Timeout Counter enabled
	
	sc_dt::uint64 ticks_to_timestamp_counter_wrap_around = (!init && (tss == TSS_INCREMENTED)) ? (max_tsc + 1 - tsc) : 0;
	sc_dt::uint64 ticks_to_timeout_counter_zero = (!init && etoc) ? toc : 0;
	
	if(ticks_to_timestamp_counter_wrap_around == 0) return ticks_to_timeout_counter_zero;
	if(ticks_to_timeout_counter_zero == 0) return ticks_to_timestamp_counter_wrap_around;
	
	return std::min(ticks_to_timeout_counter_zero, ticks_to_timestamp_counter_wrap_around);
}

template <typename CONFIG>
sc_core::sc_time M_CAN<CONFIG>::TimeToNextTimersRun()
{
	sc_dt::int64 ticks_to_next_timers_run = TicksToNextTimersRun();
	
	if(ticks_to_next_timers_run)
	{
		sc_core::sc_time time_to_next_timers_run(m_can_tscc.GetTimerPeriod());
		time_to_next_timers_run *= ticks_to_next_timers_run;
		if(time_to_next_timers_run < max_time_to_next_timers_run) return time_to_next_timers_run;
	}
	
	return max_time_to_next_timers_run;
}

template <typename CONFIG>
void M_CAN<CONFIG>::ScheduleTimersRun()
{
	sc_core::sc_time time_to_next_timers_run = TimeToNextTimersRun();
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next timers run is " << time_to_next_timers_run << EndDebugInfo;
	}
	
	sc_core::sc_time next_timers_run_time_stamp(sc_core::sc_time_stamp());
	next_timers_run_time_stamp += time_to_next_timers_run;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": timers will run at " << next_timers_run_time_stamp << EndDebugInfo;
	}
	
	Event *event = schedule.AllocEvent();
	event->WakeUp();
	event->SetTimeStamp(next_timers_run_time_stamp);
	schedule.Notify(event);
}

template <typename CONFIG>
void M_CAN<CONFIG>::ReadWords(sc_dt::uint64 addr, uint32_t *data_ptr, unsigned int n)
{
	tlm::tlm_generic_payload payload;
	payload.set_address(addr * 4);
	payload.set_data_length(n * 4);
	payload.set_data_ptr((unsigned char *) data_ptr);
	payload.set_read();
	
	can_message_ram_if->transport_dbg(payload);
}

template <typename CONFIG>
void M_CAN<CONFIG>::WriteWords(sc_dt::uint64 addr, uint32_t *data_ptr, unsigned int n)
{
	tlm::tlm_generic_payload payload;
	payload.set_address(addr * 4);
	payload.set_data_length(n * 4);
	payload.set_data_ptr((unsigned char *) data_ptr);
	payload.set_write();
	
	can_message_ram_if->transport_dbg(payload);
}

static inline bool BitScan(unsigned int& n, uint32_t v)
{
	unsigned int i = 0;
	if(unisim::util::arithmetic::BitScanReverse(i, v))
	{
		n = 31 - i;
		return true;
	}
	
	return false;
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetNumDedicatedTxBuffers() const
{
	return std::min(m_can_txbc.template Get<typename M_CAN_TXBC::NDTB>(), MAX_DEDICATED_TX_BUFFERS);
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetTxQueueFIFOSize() const
{
	return std::min(m_can_txbc.template Get<typename M_CAN_TXBC::TQFS>(), MAX_TX_QUEUE_FIFO_SIZE);
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetNumTxBuffers() const
{
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int tx_fifo_queue_size = GetTxQueueFIFOSize();
	unsigned int num_tx_buffers = std::min(num_dedicated_tx_buffers + tx_fifo_queue_size, MAX_TX_BUFFERS);
	return num_tx_buffers;
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetTxEventFIFOSize() const
{
	return std::min(m_can_txefc.template Get<typename M_CAN_TXEFC::EFS>(), MAX_TX_EVENT_FIFO_SIZE);
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetRxFIFOSize(unsigned int fifo_id) const
{
	assert(fifo_id < 2);
	return std::min(fifo_id ? m_can_rxf1c.template Get<typename M_CAN_RXF1C::F1S>() : m_can_rxf0c.template Get<typename M_CAN_RXF0C::F0S>(), MAX_RX_FIFO_SIZE);
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxDedicatedMask() const
{
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	
	return num_dedicated_tx_buffers ? (~uint32_t(0) << (32 - num_dedicated_tx_buffers)) : 0;
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxQueueFIFOMask() const
{
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	if(num_dedicated_tx_buffers >= 32) return 0;
	
	unsigned int num_tx_buffers = GetNumTxBuffers();
	
	return num_tx_buffers ? (~uint32_t(0) >> num_dedicated_tx_buffers) & (~uint32_t(0) << (32 - num_tx_buffers)) : 0;
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxFIFOFillMask() const
{
	if(m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQF>()) // Tx FIFO Full?
	{
		return GetTxQueueFIFOMask();
	}

	uint32_t tx_fifo_put_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQPI>();  // Tx FIFO Put Index
	uint32_t tx_fifo_get_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFGI>();   // Tx FIFO Get Index
		
	if(tx_fifo_put_index != tx_fifo_get_index) // Tx FIFO not empty?
	{
		// compute a binary mask between Get Index and Put Index excluded
		return ((~uint32_t(0) >> tx_fifo_get_index) & (~uint32_t(0) << (32 - tx_fifo_put_index))) & GetTxQueueFIFOMask();
	}
	
	return 0;
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxBuffersMask() const
{
	unsigned int num_tx_buffers = GetNumTxBuffers();
	
	return num_tx_buffers ? ~uint32_t(0) << (32 - num_tx_buffers) : 0;
}

template <typename CONFIG>
void M_CAN<CONFIG>::IncrementTxFIFOQueuePutIndex()
{
	bool tfqf = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQF>(); // Tx Queue/FIFO Full
	assert(!tfqf);
	
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int num_tx_buffers = GetNumTxBuffers();
	
	switch(m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>()) // Tx Queue/FIFO mode
	{
		case TQFM_FIFO:
		{
			// Tx FIFO
			uint32_t tx_fifo_put_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQPI>();  // Read Tx FIFO Put Index
			uint32_t tx_fifo_get_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFGI>();    // Read Tx FIFO Get Index
			
			// Increment Put Index and wrap around
			tx_fifo_put_index = tx_fifo_put_index + 1;                                                         // Increment by 1 Put Index
			if(tx_fifo_put_index >= num_tx_buffers) tx_fifo_put_index = num_dedicated_tx_buffers;               // Wrap around
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQPI>(tx_fifo_put_index);              // Commit Tx FIFO Put Index
			
			// Compute Tx FIFO Free Level
			unsigned int tx_fifo_free_level = (tx_fifo_get_index >= tx_fifo_put_index) ? (tx_fifo_get_index - tx_fifo_put_index)
			                                                                           : ((num_tx_buffers - tx_fifo_put_index) + (tx_fifo_get_index - num_dedicated_tx_buffers));
			
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFFL>(tx_fifo_free_level);          // Tx FIFO Free level
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQF>(tx_fifo_put_index == tx_fifo_get_index); // Tx FIFO Full
				
			break;
		}
		
		case TQFM_QUEUE:
		{
			// Tx Queue
			uint32_t old_tx_queue_put_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQPI>();
			uint32_t new_tx_queue_put_index = old_tx_queue_put_index;
			
			// search for next free buffer
			uint32_t transmission_request_pending = m_can_txbrp.template Get<typename M_CAN_TXBRP::TRP>();
			bool found = false;
			do
			{
				new_tx_queue_put_index = new_tx_queue_put_index + 1;
				if(new_tx_queue_put_index >= num_tx_buffers) new_tx_queue_put_index = num_dedicated_tx_buffers;
				found = !(transmission_request_pending & (0x80000000 >> new_tx_queue_put_index));
			}
			while(!found && (new_tx_queue_put_index != old_tx_queue_put_index));

			if(found)
			{
				// Found a free Tx buffer
				m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQPI>(new_tx_queue_put_index);
			}
			else
			{
				// No free Tx buffer
				m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQF>(1); // Tx Queue is full
			}
			
			break;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::IncrementTxFIFOGetIndex()
{
	assert(m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>() == TQFM_FIFO);

	uint32_t tx_fifo_put_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQPI>();  // Read Tx FIFO Put Index
	uint32_t tx_fifo_get_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFGI>();    // Read Tx FIFO Get Index
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int num_tx_buffers = GetNumTxBuffers();
	tx_fifo_get_index = tx_fifo_get_index + 1;
	if(tx_fifo_get_index > num_tx_buffers) tx_fifo_get_index = num_dedicated_tx_buffers;                 // wrap around
	m_can_txfqs.template Set<typename M_CAN_TXFQS::TFGI>(tx_fifo_get_index);
	// Compute Tx FIFO Free Level
	unsigned int tffl = (tx_fifo_get_index > tx_fifo_put_index) ? (tx_fifo_get_index - tx_fifo_put_index) : ((num_tx_buffers - tx_fifo_put_index) + (tx_fifo_get_index - num_dedicated_tx_buffers));
	
	bool tx_fifo_empty = (tx_fifo_get_index == tx_fifo_put_index);
	
	m_can_txfqs.template Set<typename M_CAN_TXFQS::TFFL>(tffl);  // TX FIFO Free Level
	m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQF>(0);     // Tx FIFO is not full
	
	if(tx_fifo_empty)
	{
		m_can_ir.template Set<typename M_CAN_IR::TFE>(1); // TX FIFO Empty
		UpdateInterrupts();
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::AddTxRequests()
{
	// The bits in TXBTO are reset when a new transmission is
	// requested by writing a '1' to the corresponding bit of register TXBAR
	m_can_txbto = m_can_txbto & ~m_can_txbar;
	// The bits in TXBCF are reset when a new transmission is
	// requested by writing a '1' to the corresponding bit of register TXBAR
	m_can_txbcf = m_can_txbcf & ~m_can_txbar;
	
	if(m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>() == TQFM_FIFO) // Tx FIFO Mode?
	{
		// For each add request in m_can_txbar, increment Tx FIFO Put Index
		do
		{
			uint32_t tx_fifo_put_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFQPI>();  // Tx FIFO Put Index
			if(!(m_can_txbar & (uint32_t(0x80000000) >> tx_fifo_put_index))) break; // No add request corresponding to Put Index: stop
			IncrementTxFIFOQueuePutIndex();
		}
		while(1);
	}
	
	// Add requests in TXBRP
	if(m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>() == TQFM_FIFO) // Tx FIFO Mode?
	{
		// Add Dedicated Tx Buffers and Tx FIFO Fill requests
		m_can_txbrp = m_can_txbrp | (m_can_txbar & (GetTxFIFOFillMask() | GetTxDedicatedMask()));
	}
	else
	{
		// Add all requests
		m_can_txbrp = (m_can_txbrp | m_can_txbar) & GetTxBuffersMask();
	}
	
	// Clear add requests
	m_can_txbar = m_can_txbar & ~m_can_txbrp;
	
	// Tx scan
	TxScan();
}

template <typename CONFIG>
void M_CAN<CONFIG>::CancelTxRequests()
{
	// immediately cancel requests but request corresponding to current message being transmitted (it will be cancelled later)
	uint32_t curr_tx_msg_mask = curr_tx_msg ? (0x80000000 >> curr_tx_msg->GetBufferIndex()) : 0;
	if(curr_tx_msg)
	{
		if(m_can_txbcr.IsCancelled(curr_tx_msg->GetBufferIndex()))
		{
			curr_tx_msg->set_transmission_cancelled();
		}
	}
	m_can_txbrp = m_can_txbrp & ~m_can_txbcr & ~curr_tx_msg_mask;
	m_can_txbar = m_can_txbar & ~m_can_txbar & ~curr_tx_msg_mask;
	m_can_txbcf = m_can_txbcf | (m_can_txbcr & ~curr_tx_msg_mask);
	m_can_txbcr = m_can_txbcr & m_can_txbrp; // the bits remains set until the corresponding bits in TXBRP are reset
	
	TxScan();
}

template <typename CONFIG>
void M_CAN<CONFIG>::EnterInitMode()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":entering init mode" << EndDebugInfo;
	}
	
	m_can_cccr.template Set<typename M_CAN_CCCR::INIT>(1);  // enter init mode (that acknowledges software write to CCCR[INIT])
}

template <typename CONFIG>
void M_CAN<CONFIG>::LeaveInitMode()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":leaving init mode" << EndDebugInfo;
	}
	
	m_can_cccr.template Set<typename M_CAN_CCCR::INIT>(0); // leave init mode (that acknowledges software write to CCCR[INIT])
	
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	
	// Initialize TX FIFO/Queue Status
	m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQF>(0); // TX FIFO/Queue is not full
	m_can_txfqs.template Set<typename M_CAN_TXFQS::TFQPI>(num_dedicated_tx_buffers); // Put Index is after last dedicated Tx buffer
	
	switch(m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>()) // Tx Queue/FIFO Mode
	{
		case TQFM_FIFO:
		{
			// Tx FIFO
			unsigned int num_tx_buffers = GetNumTxBuffers();
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFGI>(num_dedicated_tx_buffers); // Gut Index is after last dedicated Tx buffer
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFFL>(num_tx_buffers - num_dedicated_tx_buffers); // Tx FIFO Free level is the actual Tx FIFO size
			break;
		}
		
		case TQFM_QUEUE:
		{
			// Tx Queue
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFGI>(0); // read as 0
			m_can_txfqs.template Set<typename M_CAN_TXFQS::TFFL>(0); // read as 0
			break;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreTxEvent(const M_CAN_Message& msg)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing Tx Event in Tx Event FIFO" << EndDebugInfo;
	}

	unsigned int tx_event_fifo_size = GetTxEventFIFOSize();
	bool event_fifo_full = m_can_txefs.template Get<typename M_CAN_TXEFS::EFF>();
	
	if(tx_event_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":No Tx Event FIFO" << EndDebugInfo;
		}
	}
	
	if((tx_event_fifo_size == 0) || event_fifo_full)
	{
		// Tx Event FIFO is full
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":Tx Event FIFO Element lost" << EndDebugInfo;
		}
		
		m_can_ir.template Set<typename M_CAN_IR::TEFL>(1); // Tx Event FIFO Element Lost
		m_can_txefs.template Set<typename M_CAN_TXEFS::TEFL>(1); // Tx Event FIFO Element Lost
	}
	else
	{
		// Tx Event FIFO is not full

		Tx_Event_FIFO_Element tx_event;
		tx_event.e[0] = 0;
		Tx_Event_FIFO_Element::E0::ESI::template Set<uint32_t>(tx_event.e[0], msg.get_error_state_indicator());
		Tx_Event_FIFO_Element::E0::XTD::template Set<uint32_t>(tx_event.e[0], msg.is_identifier_extended());
		Tx_Event_FIFO_Element::E0::RTR::template Set<uint32_t>(tx_event.e[0], msg.is_remote_transmission_request());
		Tx_Event_FIFO_Element::E0::ID::template Set<uint32_t>(tx_event.e[0], msg.get_identifier());
		tx_event.e[1] = 0;
		Tx_Event_FIFO_Element::E1::MM::template Set<uint32_t>(tx_event.e[1], msg.GetMessageMarker());
		Tx_Event_FIFO_Element::E1::ET::template Set<uint32_t>(tx_event.e[1], msg.is_transmission_cancelled() ? 0x2 : 0x1);
		Tx_Event_FIFO_Element::E1::EDL::template Set<uint32_t>(tx_event.e[1], msg.is_fd());
		Tx_Event_FIFO_Element::E1::BRS::template Set<uint32_t>(tx_event.e[1], msg.get_bit_rate_switch());
		Tx_Event_FIFO_Element::E1::DLC::template Set<uint32_t>(tx_event.e[1], msg.get_data_length_code());
		Tx_Event_FIFO_Element::E1::TXTS::template Set<uint32_t>(tx_event.e[1], msg.GetTimeStamp());
		
		unsigned int tx_event_fifo_start_addr = m_can_txefc.template Get<typename M_CAN_TXEFC::EFSA>(); // word address
		unsigned int tx_event_fifo_watermark = m_can_txefc.template Get<typename M_CAN_TXEFC::EFWM>(); // Read Tx Event FIFO Watermark
		unsigned int tx_event_fifo_put_index = m_can_txefs.template Get<typename M_CAN_TXEFS::EFPI>(); // Read Tx Event FIFO Put Index
		unsigned int tx_event_fifo_get_index = m_can_txefs.template Get<typename M_CAN_TXEFS::EFGI>(); // Read Tx Event FIFO Get Index
		
		sc_dt::uint64 tx_event_addr = tx_event_fifo_start_addr + (tx_event_fifo_put_index * 2); // word address
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":storing Tx Event FIFO Element " << tx_event << " at @0x" << std::hex << tx_event_addr << std::dec << EndDebugInfo;
		}
		
		tx_event.e[0] = unisim::util::endian::Host2Target(endian, tx_event.e[0]);
		tx_event.e[1] = unisim::util::endian::Host2Target(endian, tx_event.e[1]);
		WriteWords(tx_event_addr, &tx_event.e[0], 2); // Write the Tx Event (2 words) in CAN message RAM
		
		// Increment Put Index and wrap around
		tx_event_fifo_put_index = tx_event_fifo_put_index + 1;      // Increment by 1 Put Index
		if(tx_event_fifo_put_index >= tx_event_fifo_size) tx_event_fifo_put_index = 0; // Wrap around
		m_can_txefs.template Set<typename M_CAN_TXEFS::EFPI>(tx_event_fifo_put_index);   // Commit Tx Event FIFO Put Index
		
		// Determine Fill level of Tx Event FIFO
		unsigned int tx_event_fifo_fill_level = (tx_event_fifo_get_index >= tx_event_fifo_put_index) ? (tx_event_fifo_size - tx_event_fifo_get_index + tx_event_fifo_put_index)
		                                                                                             : (tx_event_fifo_put_index - tx_event_fifo_get_index);
		// Determine whether Tx Event FIFO is full
		bool tx_event_fifo_full = (tx_event_fifo_put_index == tx_event_fifo_get_index);
		
		if(unlikely(verbose))
		{
			if(tx_event_fifo_full)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":Tx Event FIFO is full" << EndDebugInfo;
			}
		}

		m_can_txefs.template Set<typename M_CAN_TXEFS::EFFL>(tx_event_fifo_fill_level);  // Event FIFO Fill Level
		m_can_txefs.template Set<typename M_CAN_TXEFS::EFF>(tx_event_fifo_full);         // Event FIFO Full
		
		if(tx_event_fifo_fill_level == tx_event_fifo_watermark)
		{
			m_can_ir.template Set<typename M_CAN_IR::TEFW>(1); // Tx Event FIFO Watermark Reached
		}
		
		if(tx_event_fifo_full)
		{
			m_can_ir.template Set<typename M_CAN_IR::TEFF>(1); // Tx Event FIFO Full
		}
		
		m_can_ir.template Set<typename M_CAN_IR::TEFN>(1); // Tx Event FIFO New Entry
	}
	
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::AcknowledgeTxEventFIFO()
{
	unsigned int tx_event_fifo_ack_index = m_can_txefa.template Get<typename M_CAN_TXEFA::EFAI>();
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":Tx Event FIFO acknowledged up to Element #" << tx_event_fifo_ack_index << EndDebugInfo; 
	}
	
	unsigned int tx_event_fifo_size = GetTxEventFIFOSize();
	
	if(tx_event_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Tx Event while there's no Tx Event FIFO" << EndDebugWarning;
		}
		return;
	}

	if(tx_event_fifo_ack_index >= tx_event_fifo_size)
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Tx Event which is outside of Tx Event FIFO" << EndDebugWarning;
		return;
	}
	
	unsigned int tx_event_fifo_put_index = m_can_txefs.template Get<typename M_CAN_TXEFS::EFPI>(); // Read Tx Event FIFO Put Index
	unsigned int tx_event_fifo_get_index = m_can_txefs.template Get<typename M_CAN_TXEFS::EFGI>(); // Read Tx Event FIFO Get Index
	
	if((tx_event_fifo_ack_index >= tx_event_fifo_put_index) && (tx_event_fifo_ack_index < tx_event_fifo_get_index))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Tx Event which is freed in Tx Event FIFO" << EndDebugWarning;
		return;
	}
	
	tx_event_fifo_get_index = tx_event_fifo_ack_index + 1;   // New Tx Event FIFO Get Index is M_CAN_TXEFA[EFAI] + 1
	if(tx_event_fifo_get_index >= tx_event_fifo_size) tx_event_fifo_get_index = 0; // Wrap around
	
	m_can_txefs.template Set<typename M_CAN_TXEFS::EFGI>(tx_event_fifo_get_index); // Set Tx Event FIFO Get Index to M_CAN_TXEFA[EFAI] + 1
	
	// Determine Fill level of Tx Event FIFO
	unsigned int event_fifo_fill_level = (tx_event_fifo_get_index > tx_event_fifo_put_index) ? (tx_event_fifo_size - tx_event_fifo_get_index + tx_event_fifo_put_index)
	                                                                                         : (tx_event_fifo_put_index - tx_event_fifo_get_index);
	
	m_can_txefs.template Set<typename M_CAN_TXEFS::EFFL>(event_fifo_fill_level);  // Event FIFO Fill Level
	m_can_txefs.template Set<typename M_CAN_TXEFS::EFF>(0);                       // Event FIFO is not Full
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreRx(sc_dt::uint64 addr, unsigned int element_size, const M_CAN_Message& msg, bool match, unsigned int filter_index)
{
	Rx_Buffer_FIFO_Element rx_buffer_fifo_element;
	rx_buffer_fifo_element.element_size = element_size;
	memset(&rx_buffer_fifo_element.r[0], 0, 4 * element_size);
	Rx_Buffer_FIFO_Element::R0::ESI::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.get_error_state_indicator());
	Rx_Buffer_FIFO_Element::R0::XTD::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.is_identifier_extended());
	Rx_Buffer_FIFO_Element::R0::RTR::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.is_remote_transmission_request());
	Rx_Buffer_FIFO_Element::R0::ID::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.get_identifier());
	Rx_Buffer_FIFO_Element::R1::ANMF::template Set<uint32_t>(rx_buffer_fifo_element.r[1], !match);
	Rx_Buffer_FIFO_Element::R1::FIDX::template Set<uint32_t>(rx_buffer_fifo_element.r[1], filter_index);
	Rx_Buffer_FIFO_Element::R1::EDL::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.is_fd());
	Rx_Buffer_FIFO_Element::R1::BRS::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.get_bit_rate_switch());
	Rx_Buffer_FIFO_Element::R1::DLC::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.get_data_length_code());
	Rx_Buffer_FIFO_Element::R1::RXTS::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.GetTimeStamp());
	
	unsigned int data_field_size = 4 * (element_size - 2);
	unsigned int data_length = msg.get_data_length();
	if(data_length > 0)
	{
		msg.get_data((uint8_t *) &rx_buffer_fifo_element.r[2], (data_length <= data_field_size) ? data_length : data_field_size); // Note: if data in message exceeds data field size crop data from message
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing Rx Buffer FIFO Element " << rx_buffer_fifo_element << " at @0x" << std::hex << addr << std::dec << EndDebugInfo;
	}
	
	rx_buffer_fifo_element.r[0] = unisim::util::endian::Host2Target(endian, rx_buffer_fifo_element.r[0]);
	rx_buffer_fifo_element.r[1] = unisim::util::endian::Host2Target(endian, rx_buffer_fifo_element.r[1]);
	for(unsigned int i = 2; i < element_size; i++)
	{
		rx_buffer_fifo_element.r[i] = unisim::util::endian::BigEndian2Host(rx_buffer_fifo_element.r[i]);
		rx_buffer_fifo_element.r[i] = unisim::util::endian::Host2Target(endian, rx_buffer_fifo_element.r[i]);
	}
	
	WriteWords(addr, &rx_buffer_fifo_element.r[0], element_size);
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreRxFIFO(unsigned int fifo_id, const M_CAN_Message& msg, bool match, unsigned int filter_index, bool set_priority)
{
	assert(fifo_id < 2);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing message " << msg << " in Rx FIFO #" << fifo_id << EndDebugInfo;
	}
	
	unsigned int rx_fifo_size = GetRxFIFOSize(fifo_id);
	
	if(rx_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":No Rx FIFO #" << fifo_id << EndDebugInfo;
		}
		return;
	}
	
	bool rx_fifo_full = fifo_id ? m_can_rxf1s.template Get<typename M_CAN_RXF1S::F1F>() : m_can_rxf0s.template Get<typename M_CAN_RXF0S::F0F>();
	bool rx_fifo_overwrite_mode = fifo_id ? m_can_rxf1c.template Get<typename M_CAN_RXF1C::F1OM>() : m_can_rxf0c.template Get<typename M_CAN_RXF0C::F0OM>();
	
	if(rx_fifo_full && !rx_fifo_overwrite_mode)
	{
		// Rx FIFO is full and in blocking mode
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":Rx FIFO #" << fifo_id << " overrun, a message is lost" << EndDebugInfo;
		}

		if(set_priority)
		{
			SetPriority(msg.is_identifier_extended(), filter_index, MSI_FIFO_OVERRUN);
		}
		
		if(fifo_id)
		{
			m_can_ir.template Set<typename M_CAN_IR::RF1L>(1); // Rx FIFO #1 Message Lost
		}
		else
		{
			m_can_ir.template Set<typename M_CAN_IR::RF0L>(1); // Rx FIFO #0 Message Lost
		}
	}
	else
	{
		// Rx FIFO is either not full or in overwrite mode
		
		if(unlikely(verbose))
		{
			if(rx_fifo_full)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":overwriting oldest element in Rx FIFO #" << fifo_id << EndDebugInfo;
			}
		}
		
		unsigned int rx_fifo_put_index = fifo_id ? m_can_rxf1s.template Get<typename M_CAN_RXF1S::F1PI>() : m_can_rxf0s.template Get<typename M_CAN_RXF0S::F0PI>();
		unsigned int rx_fifo_get_index = fifo_id ? m_can_rxf1s.template Get<typename M_CAN_RXF1S::F1GI>() : m_can_rxf0s.template Get<typename M_CAN_RXF0S::F0GI>();
		unsigned int rx_fifo_watermark = fifo_id ? m_can_rxf1c.template Get<typename M_CAN_RXF1C::F1WM>() : m_can_rxf0c.template Get<typename M_CAN_RXF0C::F0WM>();
		
		if(set_priority)
		{
			SetPriority(msg.is_identifier_extended(), filter_index, fifo_id ? MSI_MESSAGE_STORE_IN_FIFO_1 : MSI_MESSAGE_STORE_IN_FIFO_0, rx_fifo_put_index);
		}
		
		unsigned int rx_fifo_start_addr = fifo_id ? m_can_rxf1c.template Get<typename M_CAN_RXF1C::F1SA>() : m_can_rxf0c.template Get<typename M_CAN_RXF0C::F0SA>();
		unsigned int fds = fifo_id ? m_can_rxesc.template Get<typename M_CAN_RXESC::F1DS>() : m_can_rxesc.template Get<typename M_CAN_RXESC::F0DS>();
		unsigned int rx_fifo_data_field_size = (fds <= 4) ? (8 + (fds * 4)) : (16 + ((fds & 3) * 16)); // in bytes
		unsigned int rx_fifo_element_size = 2 + (rx_fifo_data_field_size / 4); // in words
		sc_dt::uint64 rx_fifo_element_addr = rx_fifo_start_addr + (rx_fifo_put_index * rx_fifo_element_size);

		StoreRx(rx_fifo_element_addr, rx_fifo_element_size, msg, match, filter_index);
		
		rx_fifo_put_index = rx_fifo_put_index + 1;                   // Increment Put Index
		if(rx_fifo_put_index >= rx_fifo_size) rx_fifo_put_index = 0; // Wrap around
		if(fifo_id)
		{
			m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1PI>(rx_fifo_put_index); // Commit Rx FIFO #1 Put Index
		}
		else
		{
			m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0PI>(rx_fifo_put_index); // Commit Rx FIFO #0 Put Index
		}
		
		if(rx_fifo_full)
		{
			// Rx FIFO was already full: we've just overwriting oldest element 
			assert(rx_fifo_overwrite_mode);
			
			unsigned int rx_fifo_get_index = fifo_id ? m_can_rxf1s.template Get<typename M_CAN_RXF1S::F1GI>() : m_can_rxf0s.template Get<typename M_CAN_RXF0S::F0GI>();
			rx_fifo_get_index = rx_fifo_get_index + 1;                   // Increment Get Index
			if(rx_fifo_get_index >= rx_fifo_size) rx_fifo_get_index = 0; // Wrap around
			if(fifo_id)
			{
				m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1GI>(rx_fifo_get_index); // Commit Rx FIFO #1 Get Index
			}
			else
			{
				m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0GI>(rx_fifo_get_index); // Commit Rx FIFO #0 Get Index
			}
		}
		
		// Compute Rx FIFO Fill Level
		unsigned int rx_fifo_fill_level = (rx_fifo_get_index >= rx_fifo_put_index) ? (rx_fifo_size - rx_fifo_get_index + rx_fifo_put_index)
		                                                                           : (rx_fifo_put_index - rx_fifo_get_index);
		
		// Determine whether Rx FIFO is full
		bool rx_fifo_full = (rx_fifo_put_index == rx_fifo_get_index);
		
		// Determine whether watermark interrupt is enabled
		bool watermark_interrupt_enabled = (rx_fifo_watermark > 0) && (rx_fifo_watermark <= MAX_RX_FIFO_WATERMARK);
		
		// Determine whether watermark is reached
		bool watermark_reached = (rx_fifo_fill_level == rx_fifo_watermark);
		
		if(unlikely(verbose))
		{
			if(rx_fifo_full)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":Rx FIFO #" << fifo_id << " is full" << EndDebugInfo;
			}
		}

		if(fifo_id)
		{
			m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1FL>(rx_fifo_fill_level);
			m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1F>(rx_fifo_full);
		}
		else
		{
			m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0FL>(rx_fifo_fill_level);
			m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0F>(rx_fifo_full);
		}

		if(fifo_id)
		{
			if(watermark_interrupt_enabled && watermark_reached)
			{
				m_can_ir.template Set<typename M_CAN_IR::RF1W>(1); // Rx FIFO #1 Watermark Reached
			}

			if(rx_fifo_full)
			{
				m_can_ir.template Set<typename M_CAN_IR::RF1F>(1); // Rx FIFO #1 is full
			}
			
			m_can_ir.template Set<typename M_CAN_IR::RF1N>(1); // Rx FIFO #1 New Message
		}
		else
		{
			if(watermark_interrupt_enabled && watermark_reached)
			{
				m_can_ir.template Set<typename M_CAN_IR::RF0W>(1); // Rx FIFO #0 Watermark Reached
			}

			if(rx_fifo_full)
			{
				m_can_ir.template Set<typename M_CAN_IR::RF0F>(1); // Rx FIFO #0 is full
			}

			m_can_ir.template Set<typename M_CAN_IR::RF0N>(1); // Rx FIFO #0 New Message
		}
	}
	
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::AcknowledgeRxFIFO(unsigned int fifo_id)
{
	unsigned int rx_fifo_ack_index = fifo_id ? m_can_rxf1a.template Get<typename M_CAN_RXF1A::F1AI>() : m_can_rxf0a.template Get<typename M_CAN_RXF0A::F0AI>();
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":Rx FIFO #" << fifo_id << " acknowledged up to Element #" << rx_fifo_ack_index << EndDebugInfo; 
	}
	
	unsigned int rx_fifo_size = GetRxFIFOSize(fifo_id);
	
	if(rx_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ":Acknowledging a Rx FIFO element while there's no Rx FIFO #" << fifo_id << EndDebugWarning;
		}
		return;
	}

	if(rx_fifo_ack_index >= rx_fifo_size)
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Rx FIFO element which is outside of Rx FIFO #" << fifo_id << EndDebugWarning;
		return;
	}
	
	unsigned int rx_fifo_put_index = fifo_id ? m_can_rxf1s.template Get<typename M_CAN_RXF1S::F1PI>() : m_can_rxf0s.template Get<typename M_CAN_RXF0S::F0PI>();
	unsigned int rx_fifo_get_index = fifo_id ? m_can_rxf1s.template Get<typename M_CAN_RXF1S::F1GI>() : m_can_rxf0s.template Get<typename M_CAN_RXF0S::F0GI>();
	
	if((rx_fifo_ack_index >= rx_fifo_put_index) && (rx_fifo_ack_index < rx_fifo_get_index))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Rx FIFO element which is freed in Rx FIFO #" << fifo_id << EndDebugWarning;
		return;
	}
	
	rx_fifo_get_index = rx_fifo_ack_index + 1;                   // New Get Index is M_CAN_RXF1A[F1AI] + 1
	if(rx_fifo_get_index >= rx_fifo_size) rx_fifo_get_index = 0; // Wrap around
	
	if(fifo_id)
	{
		m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1GI>(rx_fifo_get_index); // Set Rx FIFO #1 Get Index to M_CAN_RXF1A[F1AI] + 1
	}
	else
	{
		m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0GI>(rx_fifo_get_index); // Set Rx FIFO #0 Get Index to M_CAN_RXF0A[F0AI] + 1
	}
	
	// Determine Fill level of Rx FIFO
	unsigned int rx_fifo_fill_level = (rx_fifo_get_index > rx_fifo_put_index) ? (rx_fifo_size - rx_fifo_get_index + rx_fifo_put_index)
	                                                                          : (rx_fifo_put_index - rx_fifo_get_index);
	if(fifo_id)
	{
		m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1FL>(rx_fifo_fill_level);  // Rx FIFO #1 fill level
		m_can_rxf1s.template Set<typename M_CAN_RXF1S::F1F>(0);                    // Rx FIFO #1 is not full
	}
	else
	{
		m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0FL>(rx_fifo_fill_level);  // Rx FIFO #1 fill level
		m_can_rxf0s.template Set<typename M_CAN_RXF0S::F0F>(0);                    // Rx FIFO #1 is not full
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreRxBuffer(unsigned int rx_buffer_element_index, const M_CAN_Message& msg, bool match, unsigned int filter_index)
{
	assert(rx_buffer_element_index < 64);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing message " << msg << " in Rx Buffer #" << rx_buffer_element_index << EndDebugInfo;
	}

	unsigned int rx_buffers_start_addr = m_can_rxbc.template Get<typename M_CAN_RXBC::RBSA>(); // Rx Buffer Start Address
	unsigned int rbds = m_can_rxesc.template Get<typename M_CAN_RXESC::RBDS>();
	unsigned int rx_buffer_data_field_size = (rbds <= 4) ? (8 + (rbds * 4)) : (16 + ((rbds & 3) * 16)); // in bytes
	unsigned int rx_buffer_element_size = 2 + (rx_buffer_data_field_size / 4); // in words
	sc_dt::uint64 rx_buffer_addr = rx_buffers_start_addr + (rx_buffer_element_index * rx_buffer_element_size);
	
	StoreRx(rx_buffer_addr, rx_buffer_element_size, msg, match, filter_index);
	
	if(rx_buffer_element_index < 32)
	{
		m_can_ndat1.NewData(rx_buffer_element_index);
	}
	else
	{
		m_can_ndat2.NewData(rx_buffer_element_index - 32);
	}

	m_can_ir.template Set<typename M_CAN_IR::DRX>(1);
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::SetPriority(bool xtd, unsigned int filter_index, MESSAGE_STORAGE_INDICATOR msi, unsigned int buffer_index)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":setting high priority message" << EndDebugInfo;
	}
	
	m_can_hpms.template Set<typename M_CAN_HPMS::FLST>(xtd);          // Filter List: Standard or Extended Filter List
	m_can_hpms.template Set<typename M_CAN_HPMS::FIDX>(filter_index); // Filter Index
	m_can_hpms.template Set<typename M_CAN_HPMS::MSI>(msi);           // Message Storage Indicator
	m_can_hpms.template Set<typename M_CAN_HPMS::BIDX>(buffer_index); // Buffer Index
	
	m_can_ir.template Set<typename M_CAN_IR::HPM>(1); // High Priority Message
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::GenerateFilterEventPulse(uint32_t filter_event)
{
}

template <typename CONFIG>
void M_CAN<CONFIG>::Filter(const M_CAN_Message& msg)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":filtering message " << msg << EndDebugInfo;
	}
	
	bool xtd = rx_msg.is_identifier_extended();
	bool rtr = rx_msg.is_remote_transmission_request();
	
	if(xtd)
	{
		// extended ID
		if(rtr && m_can_gfc.template Get<typename M_CAN_GFC::RRFE>()) // Reject Remote Frames Extended?
		{
			// reject remote frame with extended IDs
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":rejecting message " << EndDebugInfo;
			}
			return;
		}
	}
	else
	{
		// standard ID
		if(rtr && m_can_gfc.template Get<typename M_CAN_GFC::RRFS>()) // Reject Remote Frames Standard?
		{
			// reject remote frame with standard IDs
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":rejecting message " << msg << EndDebugInfo;
			}
			return;
		}
	}
	
	uint32_t id_28_0 = rx_msg.get_identifier();
	uint32_t id = xtd ? id_28_0 : (id_28_0 >> (Super::ID_LENGTH - Super::STD_FMT_ID_LENGTH));
	uint32_t fid1 = 0;
	uint32_t fid2 = 0;
	uint32_t ft = 0;
	uint32_t fec = 0;
	uint32_t rx_buffer_debug_message_selector = 0; // FID2[10:9]
	uint32_t filter_event = 0;                     // FID2[8:6]
	uint32_t rx_buffer_element_index = 0;          // FID2[5:0]
	uint32_t list_size = 0;
	uint32_t filter_list_start_addr = 0;
	
	union
	{
		Standard_Message_ID_Filter_Element std_msg_id_filter_elements[MAX_STD_MSG_ID_FILTER_ELEMENTS];
		Extended_Message_ID_Filter_Element xtd_msg_id_filter_elements[MAX_XTD_MSG_ID_FILTER_ELEMENTS];
		uint32_t w[MAX_STD_MSG_ID_FILTER_ELEMENTS];
	}
	filter_elements;
	
	if(xtd)
	{
		// extended ID
		unsigned int lse = m_can_xidfc.template Get<typename M_CAN_XIDFC::LSE>(); // List Size Extended
		list_size = (lse < MAX_XTD_MSG_ID_FILTER_ELEMENTS) ? lse : MAX_XTD_MSG_ID_FILTER_ELEMENTS;
		filter_list_start_addr = m_can_xidfc.template Get<typename M_CAN_XIDFC::FLESA>(); // Filter List Extended Start Address
		
		uint32_t eidm = m_can_xidam.template Get<typename M_CAN_XIDAM::EIDM>(); // Extended ID Mask
		id = id & eidm; // Mask ID
		
		ReadWords(filter_list_start_addr, &filter_elements.w[0], 2 * list_size);
	}
	else
	{
		// standard ID
		unsigned lss = m_can_sidfc.template Get<typename M_CAN_SIDFC::LSS>(); // List Size Standard
		list_size = (lss < MAX_STD_MSG_ID_FILTER_ELEMENTS) ? lss : MAX_STD_MSG_ID_FILTER_ELEMENTS;
		filter_list_start_addr = m_can_sidfc.template Get<typename M_CAN_SIDFC::FLSSA>();
		
		ReadWords(filter_list_start_addr, &filter_elements.w[0], list_size);
	}
	
	// Matching
	if(unlikely(verbose))
	{
		if(list_size)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":filter list has " << list_size << " filter elements" << EndDebugInfo;
		}
		else
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":filter list is empty" << EndDebugInfo;
		}
	}
	
	bool match = false;
	unsigned int filter_index;
	for(filter_index = 0, match = false; (filter_index < list_size) && !match; filter_index++)
	{
		if(xtd)
		{
			uint32_t f0 = filter_elements.xtd_msg_id_filter_elements[filter_index].f0 = unisim::util::endian::Target2Host(endian, filter_elements.xtd_msg_id_filter_elements[filter_index].f0);
			
			fec = Extended_Message_ID_Filter_Element::F0::EFEC::template Get<uint32_t>(f0);
			
			if(fec != FEC_DISABLE_FILTER_ELEMENT) // filter element enabled?
			{
				fid1 = Extended_Message_ID_Filter_Element::F0::EFID1::template Get<uint32_t>(f0);
				
				uint32_t f1 = filter_elements.xtd_msg_id_filter_elements[filter_index].f1 = unisim::util::endian::Target2Host(endian, filter_elements.xtd_msg_id_filter_elements[filter_index].f1);
			
				if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE) // store into Rx buffer or debug message?
				{
					rx_buffer_debug_message_selector = Extended_Message_ID_Filter_Element::F1::EFID2_10_9::template Get<uint32_t>(f1);
					filter_event  = Extended_Message_ID_Filter_Element::F1::EFID2_8_6 ::template Get<uint32_t>(f1);
					rx_buffer_element_index = Extended_Message_ID_Filter_Element::F1::EFID2_5_0 ::template Get<uint32_t>(f1);
				}
				else
				{
					ft = Extended_Message_ID_Filter_Element::F1::EFT::template Get<uint32_t>(f1);
					fid2 = Extended_Message_ID_Filter_Element::F1::EFID2::template Get<uint32_t>(f1);
				}
			}
		}
		else
		{
			uint32_t s0 = filter_elements.std_msg_id_filter_elements[filter_index].s0 = unisim::util::endian::Target2Host(endian, filter_elements.std_msg_id_filter_elements[filter_index].s0);
		
			fec = Standard_Message_ID_Filter_Element::S0::SFEC::template Get<uint32_t>(s0);
			
			if(fec != FEC_DISABLE_FILTER_ELEMENT) // filter element enabled?
			{
				fid1 = Standard_Message_ID_Filter_Element::S0::SFID1::template Get<uint32_t>(s0);
				
				if(fec == 7) // store into Rx buffer or debug message?
				{
					rx_buffer_debug_message_selector = Standard_Message_ID_Filter_Element::S0::SFID2_10_9::template Get<uint32_t>(s0);
					filter_event  = Standard_Message_ID_Filter_Element::S0::SFID2_8_6 ::template Get<uint32_t>(s0);
					rx_buffer_element_index  = Standard_Message_ID_Filter_Element::S0::SFID2_5_0 ::template Get<uint32_t>(s0);
				}
				else
				{
					ft = Standard_Message_ID_Filter_Element::S0::SFT::template Get<uint32_t>(s0);
					fid2 = Standard_Message_ID_Filter_Element::S0::SFID2::template Get<uint32_t>(s0);
				}
			}
		}
		
		if(fec != FEC_DISABLE_FILTER_ELEMENT) // filter element enabled?
		{
			if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE) // store into Rx buffer or debug message?
			{
				match = (id == fid1); // exact match with no masking mechanism
			}
			else
			{
				switch(ft)
				{
					case RANGE_FILTER: // range filter
						match = (id >= fid1) && (id <= fid2);
						break;
						
					case DUAL_ID_FILTER: // dual ID filter
						match = (id == fid1) || (id == fid2);
						break;
						
					case CLASSIC_FILTER: // classic filter
						match = (id & fid2) == (id & fid2);
						break;
						
					default: // reserved
						break;
				}
			}
		}
	}
	
	if(unlikely(verbose))
	{
		if(match)
		{
			if(xtd)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":message " << msg << " matches filter element #" << filter_index << " " << filter_elements.xtd_msg_id_filter_elements[filter_index] << EndDebugInfo;
			}
			else
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":message " << msg << " matches filter element #" << filter_index << " " << filter_elements.std_msg_id_filter_elements[filter_index] << EndDebugInfo;
			}
		}
		else
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":message " << msg << " does not match any filter element" << EndDebugInfo;
		}
	}

	// Acceptance
	bool accept = false;
	
	if(match)
	{
		if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE)
		{
			switch(rx_buffer_debug_message_selector)
			{
				case SEL_STORE_MESSAGE_IN_AN_RX_BUFFER: // 00b: store message into an Rx Buffer
					accept = true;
					break;
					
				case SEL_DEBUG_MESSAGE_A: // 01b: Debug Message A
					accept = DebugMessageHandlingFSM(DBG_MSG_A);
					break;
					
				case SEL_DEBUG_MESSAGE_B: // 10b: Debug Message B
					accept = DebugMessageHandlingFSM(DBG_MSG_B);
					break;
					
				case SEL_DEBUG_MESSAGE_C: // 11b: Debug Message C
					accept = DebugMessageHandlingFSM(DBG_MSG_C);
					break;
			}
			GenerateFilterEventPulse(filter_event);
		}
		else
		{
			accept = (fec != FEC_DISABLE_FILTER_ELEMENT);
		}
	}
	else if(xtd)
	{
		accept = ((m_can_gfc.template Get<typename M_CAN_GFC::ANFE>() & 2) == 0); // Accept Non-matching Frames Extended
	}
	else
	{
		accept = ((m_can_gfc.template Get<typename M_CAN_GFC::ANFS>() & 2) == 0); // Accept Non-matching Frames Standard
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":" << (accept ? "accepting" : "rejecting") << " message " << msg << EndDebugInfo;
	}
	
	// Storing
	if(accept)
	{
		if(match)
		{
			// Accept matching frame
			switch(fec)
			{
				case FEC_DISABLE_FILTER_ELEMENT: // 000b: disable filter element
					// nothing to do
					break;
					
				case FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES: // 001b: store in Rx FIFO 0
					StoreRxFIFO(0, msg, match, filter_index);
					break;
					
				case FEC_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES: // 010b: store in Rx FIFO 1
					StoreRxFIFO(1, msg, match, filter_index);
					break;
					
				case FEC_SET_PRIORITY_IF_FILTER_MATCHES: // 100b: set priority
					SetPriority(xtd, filter_index, MSI_NO_FIFO_SELECTED);
					break;
					
				case FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES: // 101b: set priority and store in FIFO 0
					StoreRxFIFO(0, msg, match, filter_index, true);
					break;
				
				case FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES: // 110b: set priority and store in FIFO 1
					StoreRxFIFO(1, msg, match, filter_index, true);
					break;
					
				case FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE: // 111b: store into Rx buffer or debug message
					switch(rx_buffer_debug_message_selector)
					{
						case SEL_STORE_MESSAGE_IN_AN_RX_BUFFER: // 00b: store message into an Rx Buffer
							StoreRxBuffer(rx_buffer_element_index, msg, match, filter_index);
							break;
							
						case SEL_DEBUG_MESSAGE_A: // 01b: Debug Message A
							StoreRxBuffer(0, msg, match, filter_index);
							break;
							
						case SEL_DEBUG_MESSAGE_B: // 10b: Debug Message B
							StoreRxBuffer(1, msg, match, filter_index);
							break;
							
						case SEL_DEBUG_MESSAGE_C: // 11b: Debug Message C
							StoreRxBuffer(2, msg, match, filter_index);
							break;
					}
					break;
			}
		}
		else
		{
			// Accepting Non-matching frame
			if((xtd && ((m_can_gfc.template Get<typename M_CAN_GFC::ANFE>() & 1) == 0)) || (!xtd && (m_can_gfc.template Get<typename M_CAN_GFC::ANFS>() & 1) == 0))
			{
				StoreRxFIFO(0, msg, match);
			}
			else
			{
				StoreRxFIFO(1, msg, match);
			}
		}
	}
}

template <typename CONFIG>
bool M_CAN<CONFIG>::DebugMessageHandlingFSM(DEBUG_MESSAGE_TYPE dbg_msg_type)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":received " << dbg_msg_type << EndDebugInfo;
	}
	
	bool accept = true;
	bool state_updated = false;
	switch(m_can_rxf1s.template Get<typename M_CAN_RXF1S::DMS>()) // Debug Message Status
	{
		case DMS_IDLE: // idle state, wait for reception of debug messages, DMA request is cleared
			if(dbg_msg_type == DBG_MSG_A) // A
			{
				m_can_rxf1s.template Set<typename M_CAN_RXF1S::DMS>(DMS_DEBUG_MESSAGE_A_RECEIVED);
				state_updated = true;
			}
			break;
			
		case DMS_DEBUG_MESSAGE_A_RECEIVED: // Debug Message A received
			if(dbg_msg_type == DBG_MSG_B) // B
			{
				m_can_rxf1s.template Set<typename M_CAN_RXF1S::DMS>(DMS_DEBUG_MESSAGES_A_B_RECEIVED);
				state_updated = true;
			}
			else if(dbg_msg_type == DBG_MSG_C) // C
			{
				m_can_rxf1s.template Set<typename M_CAN_RXF1S::DMS>(DMS_IDLE);
				state_updated = true;
			}
			break;
			
		case DMS_DEBUG_MESSAGES_A_B_RECEIVED: // Debug Messages A, B received
			if(dbg_msg_type == DBG_MSG_C) // C
			{
				m_can_rxf1s.template Set<typename M_CAN_RXF1S::DMS>(DMS_DEBUG_MESSAGES_A_B_C_RECEIVED);
				state_updated = true;
				UpdateDMA_REQ();
			}
			else // A, B
			{
				m_can_rxf1s.template Set<typename M_CAN_RXF1S::DMS>(DMS_IDLE);
				state_updated = true;
			}
			break;
			
		case DMS_DEBUG_MESSAGES_A_B_C_RECEIVED: // Debug Messages A, B, C received, DMA request is set
			accept = false; // reject
			break;
	}
	
	if(state_updated)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":" << DEBUG_MESSAGE_STATUS(m_can_rxf1s.template Get<typename M_CAN_RXF1S::DMS>()) << EndDebugInfo;
	}
	
	return accept;
}

template <typename CONFIG>
void M_CAN<CONFIG>::TxScan()
{
	uint32_t transmission_request_pending = m_can_txbrp.template Get<typename M_CAN_TXBRP::TRP>();
	uint32_t curr_tx_msg_mask = curr_tx_msg ? (0x80000000 >> curr_tx_msg->GetBufferIndex()) : 0;
	transmission_request_pending = transmission_request_pending & ~curr_tx_msg_mask;
	
	if(!transmission_request_pending)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "No pending transmission requests" << EndDebugInfo;
		}
		return;
	}
	
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int tx_buffers_start_addr = m_can_txbc.template Get<typename M_CAN_TXBC::TBSA>(); // word address
	unsigned int tbds = m_can_txesc.template Get<typename M_CAN_TXESC::TBDS>();
	unsigned int tx_buffer_data_field_size = (tbds <= 4) ? (8 + (tbds * 4)) : (16 + ((tbds & 3) * 16)); // in bytes
	unsigned int tx_buffer_element_size = 2 + (tx_buffer_data_field_size / 4); // in words
	unsigned int tx_fifo_queue_mode = m_can_txbc.template Get<typename M_CAN_TXBC::TQFM>(); 
	
	bool hit = false;
	unsigned int tx_buffer_element_index = 0;
	uint32_t tx_buffer_element_identifier = ~uint32_t(0);
	Tx_Buffer_Element tx_buffer_element;
	tx_buffer_element.element_size = tx_buffer_element_size;
	
	unsigned int tx_buffer_scan_index;
	if(BitScan(tx_buffer_scan_index, transmission_request_pending) && (tx_buffer_scan_index < num_dedicated_tx_buffers))
	{
		hit = true;
		
		do
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Pending transmission request for Tx Buffer Element #" << tx_buffer_scan_index << EndDebugInfo;
			}
		
			sc_dt::uint64 tx_buffer_addr = tx_buffers_start_addr + (tx_buffer_scan_index * tx_buffer_element_size); // word address
			
			uint32_t t0 = 0;
			ReadWords(tx_buffer_addr, &t0, 1);
			t0 = unisim::util::endian::Target2Host(endian, t0);

			uint32_t identifier = Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(t0) ? Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0)
																							: (Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0) & (~uint32_t(0) << (Super::ID_LENGTH - Super::STD_FMT_ID_LENGTH)));
			
			if(identifier < tx_buffer_element_identifier)
			{
				tx_buffer_element_identifier = identifier;
				tx_buffer_element.t[0] = t0;
				tx_buffer_element_index = tx_buffer_scan_index;
			}
			
			transmission_request_pending &= ~(uint32_t(0x80000000) >> tx_buffer_scan_index); // clear pending request
		}
		while(BitScan(tx_buffer_scan_index, transmission_request_pending) && (tx_buffer_scan_index < num_dedicated_tx_buffers));
	}
	
	if(tx_fifo_queue_mode == TQFM_FIFO)
	{
		unsigned int tqfs = GetTxQueueFIFOSize();   // Tx FIFO Size
		unsigned int tffl = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFFL>(); // Tx FIFO Free Level
		
		if(tffl != tqfs) // TX FIFO not empty?
		{
			unsigned int tx_fifo_get_index = m_can_txfqs.template Get<typename M_CAN_TXFQS::TFGI>();
			if(transmission_request_pending & (0x80000000 >> tx_fifo_get_index))
			{
				hit = true;
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Pending transmission request for Tx Buffer Element #" << tx_buffer_scan_index << " from Tx FIFO" << EndDebugInfo;
				}
				
				sc_dt::uint64 tx_buffer_element_addr = tx_buffers_start_addr + (tx_fifo_get_index * tx_buffer_element_size * 4);
				
				uint32_t t0 = 0;
				ReadWords(tx_buffer_element_addr, &t0, 1);
				t0 = unisim::util::endian::Target2Host(endian, t0);
				uint32_t identifier = Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(t0) ? Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0)
																								: (Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0) & (~uint32_t(0) << (Super::ID_LENGTH - Super::STD_FMT_ID_LENGTH)));
				
				if(identifier < tx_buffer_element_identifier)
				{
					tx_buffer_element_identifier = identifier;
					tx_buffer_element.t[0] = t0;
					tx_buffer_element_index = tx_fifo_get_index;
				}
			}
		}
	}
	
	if(hit)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Tx scan hits Tx Buffer Element #" << tx_buffer_element_index << EndDebugInfo;
		}
		
		sc_dt::uint64 tx_buffer_element_addr = tx_buffers_start_addr + (tx_buffer_element_index * tx_buffer_element_size);
		
		ReadWords(tx_buffer_element_addr + 1, &tx_buffer_element.t[1], tx_buffer_element_size - 1);
		tx_buffer_element.t[1] = unisim::util::endian::Target2Host(endian, tx_buffer_element.t[1]);
		
		for(unsigned int i = 2; i < tx_buffer_element_size; i++)
		{
			tx_buffer_element.t[i] = unisim::util::endian::Target2Host(endian, tx_buffer_element.t[i]);
			tx_buffer_element.t[i] = unisim::util::endian::Host2BigEndian(tx_buffer_element.t[i]);
		}
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Loaded Tx Buffer Element " << tx_buffer_element << " at @0x" << std::hex << tx_buffer_element_addr << std::dec << EndDebugInfo;
		}
		
		tlm_can_format fmt = Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(tx_buffer_element.t[0]) ? TLM_CAN_XTD_FMT : TLM_CAN_STD_FMT;
		bool remote_transmission_request = Tx_Buffer_Element::T0::RTR::template Get<uint32_t>(tx_buffer_element.t[0]);
		unsigned int message_marker = Tx_Buffer_Element::T1::MM::template Get<uint32_t>(tx_buffer_element.t[1]);
		bool event_fifo_control = Tx_Buffer_Element::T1::EFC::template Get<uint32_t>(tx_buffer_element.t[1]);
		unsigned int data_length_code = Tx_Buffer_Element::T1::DLC::template Get<uint32_t>(tx_buffer_element.t[1]);
		
		M_CAN_Message& new_can_msg = tx_msg_pipe[flip_flop ^= 1]; // pick one of the two CAN messages in double buffer
		
		new_can_msg.Reset();
		new_can_msg.set_format(fmt);
		new_can_msg.set_identifier(tx_buffer_element_identifier);
		new_can_msg.set_remote_transmission_request(remote_transmission_request);
		new_can_msg.set_data_length_code(data_length_code);
		
		unsigned int data_length = new_can_msg.get_data_length();
		new_can_msg.set_data((uint8_t *) &tx_buffer_element.t[2], tx_buffer_data_field_size);
		if(data_length > tx_buffer_data_field_size)
		{
			// padding
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Padding message data field" << EndDebugInfo;
			}
			for(unsigned int i = tx_buffer_data_field_size; i < data_length; i++)
			{
				new_can_msg.set_data_byte_at(i, 0xcc);
			}
		}
		
		new_can_msg.SetBufferIndex(tx_buffer_element_index);
		new_can_msg.SetEFC(event_fifo_control);
		new_can_msg.SetMessageMarker(message_marker);
		
		pending_tx_msg = &new_can_msg;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Tx scan has prepared message " << (*pending_tx_msg) << " for further transmission by CAN core" << EndDebugInfo;
		}
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Tx scan misses" << tx_buffer_element_index << EndDebugInfo;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateInterrupts()
{
	gen_int0_event.notify(sc_core::SC_ZERO_TIME);
	gen_int1_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateDMA_REQ()
{
	gen_dma_req_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void M_CAN<CONFIG>::INT0_Process()
{
	bool eint0 = m_can_ile.template Get<typename M_CAN_ILE::EINT0>();
	bool int0 = eint0 && ((m_can_ie & ~m_can_ils & m_can_ir) != 0);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT0.name() << " <- " << int0 << EndDebugInfo;
	}
	
	INT0 = int0;
}

template <typename CONFIG>
void M_CAN<CONFIG>::INT1_Process()
{
	bool eint1 = m_can_ile.template Get<typename M_CAN_ILE::EINT1>();
	bool int1 = eint1 && ((m_can_ie & m_can_ils & m_can_ir) != 0);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT1.name() << " <- " << int1 << EndDebugInfo;
	}
	
	INT1 = int1;
}

template <typename CONFIG>
void M_CAN<CONFIG>::DMA_REQ_Process()
{
	bool dma_req = (m_can_rxf1s.template Get<typename M_CAN_RXF1S::DMS>() == DMS_DEBUG_MESSAGES_A_B_C_RECEIVED);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_REQ.name() << " <- " << dma_req << EndDebugInfo;
	}
	
	DMA_REQ = dma_req;
}

template <typename CONFIG>
void M_CAN<CONFIG>::DMA_ACK_Process()
{
	if(DMA_ACK.posedge())
	{
		m_can_rxf1s.template Set<typename M_CAN_RXF1S::DMS>(DMS_IDLE); // reset Debug Message Handling FSM
		UpdateDMA_REQ();
	}
}

} // end of namespace m_can
} // end of namespace bosch
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_TCC__
