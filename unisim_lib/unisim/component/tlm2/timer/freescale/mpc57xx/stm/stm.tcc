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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_TCC__
#define __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_TCC__

#include <unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace freescale {
namespace mpc57xx {
namespace stm {

template <typename CONFIG>
STM<CONFIG>::STM(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, ahb_if("ahb_if")
	, m_clk("m_clk")
	, debug_mode("debug_mode")
	, p_irq()
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, stm_cr(this)
	, stm_cnt(this)
	, stm_ccr(this)
	, stm_cir(this)
	, stm_cmp(this)
	, reg_addr_map()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, irq_level()
	, gen_irq_event()
	, last_counters_run_time(sc_core::SC_ZERO_TIME)
	, prescaled_clock_period(sc_core::SC_ZERO_TIME)
	, counters_run_event("counters_run_event")
	, freeze(false)
{
	ahb_if(*this); // bind interface
	
	unsigned int channel_num;
	
	SC_HAS_PROCESS(STM);

	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		std::stringstream p_irq_name_sstr;
		p_irq_name_sstr << "p_irq_" << channel_num;
		p_irq[channel_num] = new sc_core::sc_out<bool>(p_irq_name_sstr.str().c_str()); 
	}

	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		std::stringstream gen_irq_event_name_sstr;
		gen_irq_event_name_sstr << "gen_irq_event_" << channel_num;
		gen_irq_event[channel_num] = new sc_core::sc_event(gen_irq_event_name_sstr.str().c_str());
	}

	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(stm_cr.ADDRESS_OFFSET, &stm_cr);
	reg_addr_map.MapRegister(stm_cnt.ADDRESS_OFFSET, &stm_cnt);
	reg_addr_map.MapRegisterFile(STM_CCR::ADDRESS_OFFSET, &stm_ccr, 4, 3 * 4);
	reg_addr_map.MapRegisterFile(STM_CIR::ADDRESS_OFFSET, &stm_cir, 4, 3 * 4);
	reg_addr_map.MapRegisterFile(STM_CMP::ADDRESS_OFFSET, &stm_cmp, 4, 3 * 4);

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	// Spawn an P_IRQ_Process for each channel
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		sc_core::sc_spawn_options p_irq_process_spawn_options;
		p_irq_process_spawn_options.spawn_method();
		p_irq_process_spawn_options.dont_initialize();
		p_irq_process_spawn_options.set_sensitivity(gen_irq_event[channel_num]);
		
		std::stringstream p_irq_process_name_sstr;
		p_irq_process_name_sstr << "P_IRQ_Process_" << channel_num;
		sc_core::sc_spawn(sc_bind(&STM<CONFIG>::P_IRQ_Process, this, channel_num), p_irq_process_name_sstr.str().c_str(), &p_irq_process_spawn_options);
	}
}

template <typename CONFIG>
STM<CONFIG>::~STM()
{
	unsigned int channel_num;
	
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		delete p_irq[channel_num];
	}
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		delete gen_irq_event[channel_num];
	}
}

template <typename CONFIG>
void STM<CONFIG>::end_of_elaboration()
{
	sc_core::sc_time start_time;
	
	if(m_clk_prop_proxy.GetClockPosEdgeFirst())
	{
		// clock rising edge comes first
		start_time = m_clk_prop_proxy.GetClockStartTime();
	}
	else
	{
		// clock falling edge comes first: skip time until first rising edge
		start_time = m_clk_prop_proxy.GetClockStartTime();
		start_time += (1.0 - m_clk_prop_proxy.GetClockDutyCycle()) * m_clk_prop_proxy.GetClockPeriod();
	}
	
	if(verbose)
	{
		logger << DebugInfo << "First MAIN_CLK rising edge is at " << start_time << EndDebugInfo;
	}

	last_counters_run_time = start_time;

	UpdatePrescaledClockPeriod();
	RefreshFreeze();

	// Spawn an RunCounterProcess sensitive scheduled when counters should be run or clock properties have changed
	sc_core::sc_spawn_options run_counters_process_spawn_options;
	run_counters_process_spawn_options.spawn_method();
	run_counters_process_spawn_options.set_sensitivity(&counters_run_event);
	run_counters_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());
	
	sc_core::sc_spawn(sc_bind(&STM<CONFIG>::RunCounterProcess, this), "RunCounterProcess", &run_counters_process_spawn_options);
}

template <typename CONFIG>
void STM<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool STM<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int STM<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
	
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
			return reg_addr_map.DebugWrite(time_stamp, start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(time_stamp, start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum STM<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void STM<CONFIG>::ProcessEvent(Event *event)
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
		else if((start_addr & 3) != 0)
		{
			logger << DebugWarning << "only 32-bit aligned access to register are allowed" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		}
		else if(streaming_width != 4)
		{
			logger << DebugWarning << "only 32-bit access to register are allowed" << EndDebugWarning;
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
				logger << DebugError << "while mapped read/write access, " << rws << std::endl;
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
		
		tlm::tlm_sync_enum sync = ahb_if->nb_transport_bw(*payload, phase, completion_time);
		
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
void STM<CONFIG>::ProcessEvents()
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
void STM<CONFIG>::Process()
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
void STM<CONFIG>::UpdatePrescaledClockPeriod()
{
	prescaled_clock_period = m_clk_prop_proxy.GetClockPeriod();
	prescaled_clock_period *= stm_cr.template Get<typename STM_CR::CPS>() + 1;
}

template <typename CONFIG>
void STM<CONFIG>::SetIRQLevel(unsigned int channel_num, bool level)
{
	irq_level[channel_num] = level;
	gen_irq_event[channel_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void STM<CONFIG>::TriggerInterrupt(unsigned int channel_num)
{
	stm_cir[channel_num].template Set<typename STM_CIR::CIF>(1);
	SetIRQLevel(channel_num, true);
}

template <typename CONFIG>
void STM<CONFIG>::AckInterrupt(unsigned int channel_num)
{
	SetIRQLevel(channel_num, false);
}

template <typename CONFIG>
void STM<CONFIG>::IncrementCounter(sc_dt::uint64 delta)
{
	if(stm_cr.template Get<typename STM_CR::TEN>())
	{
		// counter is enabled
		uint32_t cnt = stm_cnt.template Get<typename STM_CNT::CNT>();
		cnt += delta;
		stm_cnt.template Set<typename STM_CNT::CNT>(cnt);
		
		unsigned int channel_num;
		
		for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
		{
			if(stm_ccr[channel_num].template Get<typename STM_CCR::CEN>())
			{
				// channel is enabled
				uint32_t cmp = stm_cmp[channel_num].template Get<typename STM_CMP::CMP>();
				
				if(cnt == cmp)
				{
					TriggerInterrupt(channel_num);
				}
			}
		}
	}
}

template <typename CONFIG>
sc_core::sc_time STM<CONFIG>::TimeToNextCountersRun()
{
	sc_dt::int64 min_ticks = 0x7fffffffffffffffLL;
	
	if(stm_cr.template Get<typename STM_CR::TEN>())
	{
		// counter is enabled
		uint32_t cnt = stm_cnt.template Get<typename STM_CNT::CNT>();

		unsigned channel_num;
		
		for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
		{
			if(stm_ccr[channel_num].template Get<typename STM_CCR::CEN>())
			{
				uint32_t cmp = stm_cmp[channel_num].template Get<typename STM_CMP::CMP>();
				
				sc_dt::int64 ticks_to_interrupt = (cnt <= cmp) ? (cmp - cnt) : ((0xffffffffUL - cmp + 1) + cmp);
				
				if(ticks_to_interrupt < min_ticks) min_ticks = ticks_to_interrupt;
			}
		}
	}
	
	return min_ticks * prescaled_clock_period;
}

template <typename CONFIG>
void STM<CONFIG>::RunCounterToTime(const sc_core::sc_time& time_stamp)
{
	sc_core::sc_time delay_since_last_counters_run(time_stamp);
	delay_since_last_counters_run -= last_counters_run_time;
	sc_dt::uint64 delta = delay_since_last_counters_run / prescaled_clock_period;
	sc_core::sc_time run_time(prescaled_clock_period);
	run_time *= delta;

	if(!freeze)
	{
		// in the past, until time stamp, counter was not frozen
		IncrementCounter(delta);
	}
	
	RefreshFreeze();
}

template <typename CONFIG>
void STM<CONFIG>::ScheduleCountersRun()
{
	UpdatePrescaledClockPeriod();
	sc_core::sc_time time_next_counters_run = TimeToNextCountersRun();
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next counters run is " << time_next_counters_run << EndDebugInfo;
	}
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	time_next_counters_run += last_counters_run_time;
	time_next_counters_run -= time_stamp;
	counters_run_event.notify(time_next_counters_run); // schedule next counters run
}

template <typename CONFIG>
void STM<CONFIG>::RunCounterProcess()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if(verbose)
	{
		logger << DebugInfo << time_stamp << ":RunCounterProcess()" << EndDebugInfo;
	}
	
	RunCounterToTime(time_stamp);
	ScheduleCountersRun();
}

template <typename CONFIG>
void STM<CONFIG>::P_IRQ_Process(unsigned int channel_num)
{
	*p_irq[channel_num] = irq_level[channel_num];
}

template <typename CONFIG>
void STM<CONFIG>::RefreshFreeze()
{
	freeze = debug_mode && stm_cr.template Get<typename STM_CR::FRZ>();
}

} // end of namespace stm
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_STM_STM_TCC__
