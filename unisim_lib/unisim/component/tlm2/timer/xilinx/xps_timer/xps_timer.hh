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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__
#define __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__

#include <systemc.h>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/component/cxx/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <tlm_utils/peq_with_get.h>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;

typedef unisim::kernel::tlm2::SimplePayload<bool> GenerateOutPayload;
typedef unisim::kernel::tlm2::SimplePayload<bool> CaptureTriggerPayload;
typedef unisim::kernel::tlm2::SimplePayload<bool> PWMPayload;

typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> GenerateOutProtocolTypes;
typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> CaptureTriggerProtocolTypes;
typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> PWMProtocolTypes;

template <class CONFIG>
class XPS_Timer
	: public sc_module
	, public unisim::component::cxx::timer::xilinx::xps_timer::XPS_Timer<CONFIG>
	, public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
{
public:
	typedef unisim::component::cxx::timer::xilinx::xps_timer::XPS_Timer<CONFIG> inherited;
	typedef tlm::tlm_target_socket<0, CaptureTriggerProtocolTypes> capture_trigger_slave_socket;
	typedef tlm::tlm_initiator_socket<0, GenerateOutProtocolTypes> generate_out_master_socket;
	typedef tlm::tlm_initiator_socket<0, PWMProtocolTypes> pwm_master_socket;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> interrupt_master_socket;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH, tlm::tlm_base_protocol_types> slave_sock;
	
	// Capture trigger inputs
	capture_trigger_slave_socket *capture_trigger_slave_sock[CONFIG::C_ONE_TIMER_ONLY ? 1 : 2];
	
	// GenerateOut outputs
	generate_out_master_socket *generate_out_master_sock[CONFIG::C_ONE_TIMER_ONLY ? 1 : 2];
	
	// PWM output
	pwm_master_socket pwm_master_sock;
	
	// Interrupt
	interrupt_master_socket interrupt_master_sock;

	XPS_Timer(const sc_module_name& name, Object *parent = 0);
	virtual ~XPS_Timer();
	
	virtual bool BeginSetup();
	
	// PLB interface
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Forward b_transport/nb_transport callbacks for CaptureTrigger
	virtual void capture_trigger_b_transport(unsigned int channel, CaptureTriggerPayload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum capture_trigger_nb_transport_fw(unsigned int channel, CaptureTriggerPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	// Backward nb_transport callbacks for GenerateOut, PWM, and interrupt
	virtual tlm::tlm_sync_enum generate_out_nb_transport_bw(unsigned int channel, GenerateOutPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum pwm_nb_transport_bw(unsigned int channel, PWMPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum interrupt_nb_transport_bw(unsigned int channel, InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	void Process();
	
protected:
private:
	void AlignToClock(sc_time& t);

	class FwRedirector : public tlm::tlm_fw_transport_if<unisim::kernel::tlm2::SimpleProtocolTypes<bool> >
	{
	public:
		FwRedirector(unsigned int id, XPS_Timer<CONFIG> *xps_timer, tlm::tlm_sync_enum (XPS_Timer<CONFIG>::*cb_nb_transport_fw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&), void (XPS_Timer<CONFIG>::*cb_b_transport)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, sc_core::sc_time&));
		
		virtual void b_transport(unisim::kernel::tlm2::SimplePayload<bool>& trans, sc_core::sc_time& t);

		virtual tlm::tlm_sync_enum nb_transport_fw(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

		virtual unsigned int transport_dbg(unisim::kernel::tlm2::SimplePayload<bool>& trans);
		
		virtual bool get_direct_mem_ptr(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_dmi& dmi_data);

	private:
		unsigned int id;
		XPS_Timer<CONFIG> *xps_timer;
		tlm::tlm_sync_enum (XPS_Timer<CONFIG>::*cb_nb_transport_fw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&);
		void (XPS_Timer<CONFIG>::*cb_b_transport)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, sc_core::sc_time&);
	};
	
	class BwRedirector : public tlm::tlm_bw_transport_if<unisim::kernel::tlm2::SimpleProtocolTypes<bool> >
	{
	public:
		BwRedirector(unsigned int id, XPS_Timer<CONFIG> *xps_timer, tlm::tlm_sync_enum (XPS_Timer<CONFIG>::*cb_nb_transport_bw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&));
		
		virtual tlm::tlm_sync_enum nb_transport_bw(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

		virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	private:
		unsigned int id;
		XPS_Timer<CONFIG> *xps_timer;
		tlm::tlm_sync_enum (XPS_Timer<CONFIG>::*cb_nb_transport_bw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&);
	};

	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_WAKE_UP,           // a simple event to make process wake up
			EV_LOAD,              // an event to make load or reload counters
			EV_CAPTURE_TRIGGER,   // a capture trigger input is available
			EV_CPU,               // a CPU request occured
		} Type;
		
		Event()
			: type(EV_WAKE_UP)
			, time_stamp(SC_ZERO_TIME)
			, cpu_payload(0)
			, channel(0)
			, ev_completed(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void InitializeCPUEvent(tlm::tlm_generic_payload *_payload, const sc_time& _time_stamp, sc_event *_ev_completed = 0)
		{
			_payload->acquire();
			type = EV_CPU;
			time_stamp = _time_stamp;
			cpu_payload = _payload;
			channel = 0;
			ev_completed = _ev_completed;
		}
		
		void InitializeCaptureTriggerEvent(unsigned int _channel, const sc_time& _time_stamp)
		{
			type = EV_CAPTURE_TRIGGER;
			time_stamp = _time_stamp;
			channel = _channel;
		}

		void InitializeLoadEvent(unsigned int _channel, const sc_time& _time_stamp)
		{
			type = EV_LOAD;
			time_stamp = _time_stamp;
			channel = _channel;
		}

		void InitializeWakeUpEvent(const sc_time& _time_stamp)
		{
			type = EV_WAKE_UP;
			time_stamp = _time_stamp;
			channel = 0;
		}

		void Clear()
		{
			if(cpu_payload) cpu_payload->release();
			type = EV_WAKE_UP;
			time_stamp = SC_ZERO_TIME;
			cpu_payload = 0;
			channel = 0;
			ev_completed = 0;
		}
		
		Type GetType() const
		{
			return type;
		}
		
		void SetTimeStamp(const sc_time& _time_stamp)
		{
			time_stamp = _time_stamp;
		}
		
		const sc_time& GetTimeStamp() const
		{
			return time_stamp;
		}
		
		tlm::tlm_generic_payload *GetCPUPayload() const
		{
			return cpu_payload;
		}
		
		unsigned int GetChannel() const
		{
			return channel;
		}
		
		sc_event *GetCompletionEvent() const
		{
			return ev_completed;
		}
		
	private:
		Type type;
		sc_time time_stamp;
		tlm::tlm_generic_payload *cpu_payload;
		unsigned int channel;
		sc_event *ev_completed;
	};
	
	class EventAllocator
	{
	public:
		EventAllocator()
			: free_list()
		{
		}
		
		~EventAllocator()
		{
			while(!free_list.empty())
			{
				Event *event = free_list.top();
				delete event;
				free_list.pop();
			}
		}
		
		Event *AllocEvent()
		{
			if(!free_list.empty())
			{
				Event *event = free_list.top();
				free_list.pop();
				return event;
			}
			
			return new Event();
		}
		
		void FreeEvent(Event *event)
		{
			event->Clear();
			free_list.push(event);
		}
	private:
		std::stack<Event *, std::vector<Event *> > free_list;
	};
	
	class ScheduleKey
	{
	public:
		ScheduleKey(const sc_time& _time_stamp, typename Event::Type _type, unsigned int _channel)
			: time_stamp(_time_stamp)
			, type(_type)
			, channel(_channel)
		{
		}
		
		int operator < (const ScheduleKey& sk) const
		{
			return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type) || ((type == sk.type) && (channel < sk.channel))));
		}
		
		const sc_time& GetTimeStamp() const
		{
			return time_stamp;
		}
	private:
		sc_time time_stamp;
		typename Event::Type type;
		unsigned int channel;
	};
	
	class Schedule
	{
	public:
		Schedule()
			: schedule()
			, kernel_event()
			, event_allocator()
		{
		}
		
		~Schedule()
		{
			typename std::multimap<ScheduleKey, Event *>::iterator it;
			for(it = schedule.begin(); it != schedule.end(); it++)
			{
				Event *event = (*it).second;
				
				event_allocator.FreeEvent(event);
			}
		}
		
		void NotifyCPUEvent(tlm::tlm_generic_payload *payload, const sc_time& time_stamp, sc_event *ev_completed = 0)
		{
			std::cerr << "NotifyCPUEvent at " << time_stamp << std::endl;
			ScheduleKey key = ScheduleKey(time_stamp, Event::EV_CPU, 0);
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.find(key);
			if(it != schedule.end())
			{
				if((*it).second->GetCPUPayload() == payload) return; // Already scheduled
			}
			Event *event = event_allocator.AllocEvent();
			event->InitializeCPUEvent(payload, time_stamp, ev_completed);
			schedule.insert(std::pair<ScheduleKey, Event *>(key, event));
			sc_time t(time_stamp);
			t -= sc_time_stamp();
			kernel_event.notify(t);
		}
		
		void NotifyCaptureTriggerEvent(unsigned int channel, const sc_time& time_stamp)
		{
			std::cerr << "NotifyCaptureTriggerEvent at " << time_stamp << std::endl;
			ScheduleKey key = ScheduleKey(time_stamp, Event::EV_CAPTURE_TRIGGER, channel);
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.find(key);
			if(it != schedule.end())
			{
				if((*it).second->GetChannel() == channel) return; // Already scheduled
			}
			Event *event = event_allocator.AllocEvent();
			event->InitializeCaptureTriggerEvent(channel, time_stamp);
			schedule.insert(std::pair<ScheduleKey, Event *>(key, event));
			sc_time t(time_stamp);
			t -= sc_time_stamp();
			kernel_event.notify(t);
		}

		void NotifyLoadEvent(unsigned int channel, const sc_time& time_stamp)
		{
			std::cerr << "NotifyLoadEvent at " << time_stamp << std::endl;
			ScheduleKey key = ScheduleKey(time_stamp, Event::EV_LOAD, channel);
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.find(key);
			if(it != schedule.end())
			{
				if((*it).second->GetChannel() == channel)
				{
					std::cerr << "!!! Already scheduled" << std::endl;
					return; // Already scheduled
				}
			}
			Event *event = event_allocator.AllocEvent();
			event->InitializeLoadEvent(channel, time_stamp);
			schedule.insert(std::pair<ScheduleKey, Event *>(key, event));
			sc_time t(time_stamp);
			t -= sc_time_stamp();
			kernel_event.notify(t);
		}

		void NotifyWakeUpEvent(const sc_time& time_stamp)
		{
			std::cerr << "NotifyWakeUpEvent at " << time_stamp << std::endl;
			ScheduleKey key = ScheduleKey(time_stamp, Event::EV_WAKE_UP, 0);
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.find(key);
			if(it != schedule.end()) return; // Already scheduled
			Event *event = event_allocator.AllocEvent();
			event->InitializeWakeUpEvent(time_stamp);
			schedule.insert(std::pair<ScheduleKey, Event *>(key, event));
			sc_time t(time_stamp);
			t -= sc_time_stamp();
			kernel_event.notify(t);
		}
		
		void Notify(Event *event)
		{
			const sc_time& time_stamp = event->GetTimeStamp();
			schedule.insert(std::pair<ScheduleKey, Event *>(ScheduleKey(time_stamp, event->GetType(), event->GetChannel()), event));
			sc_time t(time_stamp);
			t -= sc_time_stamp();
			kernel_event.notify(t);
		}
		
		Event *GetNextEvent(const sc_time& time_stamp)
		{
			if(schedule.empty()) return 0;
			
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.begin();
			const sc_time& event_time_stamp = (*it).first.GetTimeStamp();
			if(event_time_stamp <= time_stamp)
			{
				Event *event = (*it).second;
				schedule.erase(it);
				return event;
			}
			
			sc_time t(event_time_stamp);
			t -= time_stamp;
			kernel_event.notify(t);
			
			return 0;
		}
		
		void FreeEvent(Event *event)
		{
			event_allocator.FreeEvent(event);
		}
		
		const sc_event& GetKernelEvent() const
		{
			return kernel_event;
		}
		
		void Flush(const sc_time& time_stamp, typename Event::Type event_type)
		{
			typename std::multimap<ScheduleKey, Event *>::iterator it;
			for(it = schedule.begin(); it != schedule.end(); it++)
			{
				Event *event = (*it).second;
				
				if(event->GetType() == event_type)
				{
					event_allocator.FreeEvent(event);
					schedule.erase(it);
				}
			}
			kernel_event.cancel();

			if(schedule.empty()) return;
			
			it = schedule.begin();
			const sc_time& event_time_stamp = (*it).first.GetTimeStamp();
			if(event_time_stamp <= time_stamp)
			{
				return;
			}
			
			sc_time t(event_time_stamp);
			t -= time_stamp;
			kernel_event.notify(t);
		}

		void Clear()
		{
			typename std::multimap<ScheduleKey, Event *>::iterator it;
			for(it = schedule.begin(); it != schedule.end(); it++)
			{
				Event *event = (*it).second;
				
				event_allocator.FreeEvent(event);
			}
			schedule.clear();
			kernel_event.cancel();
		}

	private:
		std::multimap<ScheduleKey, Event *> schedule;
		
		sc_event kernel_event;
		EventAllocator event_allocator;
	};
	
	sc_time process_local_time_offset;
	/** Cycle time */
	sc_time cycle_time;
	/** Latencies */
	sc_time read_latency;
	sc_time write_latency;
	
	sc_time time_stamp;
	sc_time ready_time_stamp;

	bool capture_trigger_input[2];
	bool interrupt_output;
	bool generate_output[2];
	bool pwm_output;
	sc_time last_timer_counter_update_time_stamp[2];
	sc_time last_generate_output_time_stamp[2];
	bool tcr0_roll_over;
	bool tcr1_roll_over;
	
	/** The parameter for the cycle time */
	Parameter<sc_time> param_cycle_time;
	/** The parameters to set the latencies */
	Parameter<sc_time> param_read_latency;
	Parameter<sc_time> param_write_latency;

	FwRedirector *capture_trigger_redirector[2];
	BwRedirector *generate_out_redirector[2];
	BwRedirector *pwm_redirector;
	BwRedirector *interrupt_redirector;

	PayloadFabric<GenerateOutPayload> generate_out_payload_fabric;
	PayloadFabric<PWMPayload> pwm_payload_fabric;
	PayloadFabric<InterruptPayload> interrupt_payload_fabric;
	Schedule schedule;
	
	void ProcessLoadEvent(Event *event);
	void ProcessCaptureTriggerEvent(Event *event);
	void ProcessCPUEvent(Event *event);
	void RunCounters();
	void Update();
	void GenerateOutput();
};

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
