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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__

#include <systemc.h>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/component/cxx/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceExportBase;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::kernel::tlm2::PayloadFabric;

template <class CONFIG>
class XPS_IntC
	: public sc_module
	, public unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG>
	, public tlm::tlm_fw_transport_if<>
	, public tlm::tlm_bw_transport_if<InterruptProtocolTypes>
{
public:
	typedef unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG> inherited;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> irq_master_socket;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH> slave_sock;
	
	// Input interrupts
	irq_slave_socket *irq_slave_sock[CONFIG::C_NUM_INTR_INPUTS];
	
	// Output interrupt request port
	irq_master_socket irq_master_sock;

	XPS_IntC(const sc_module_name& name, Object *parent = 0);
	virtual ~XPS_IntC();
	
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);

	virtual tlm::tlm_sync_enum nb_transport_bw(InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	virtual void interrupt_b_transport(unsigned int irq, InterruptPayload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	void Process();
protected:
	virtual void SetOutputLevel(bool level);
	virtual void SetOutputEdge(bool final_level);
private:
	bool IsVerbose() const;
	void AlignToClock(sc_time& t);

	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_IRQ,               // an IRQ occured
			EV_CPU,               // a CPU request occured
		} Type;
		
		Event()
			: type(EV_IRQ)
			, time_stamp(SC_ZERO_TIME)
			, cpu_payload(0)
			, irq(0)
			, level(false)
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
			irq = 0;
			level = false;
			ev_completed = _ev_completed;
		}
		
		void InitializeIRQEvent(unsigned int _irq, bool _level, const sc_time& _time_stamp)
		{
			type = EV_IRQ;
			time_stamp = _time_stamp;
			irq = _irq;
			level = _level;
		}

		void Clear()
		{
			if(cpu_payload) cpu_payload->release();
			type = EV_IRQ;
			time_stamp = SC_ZERO_TIME;
			cpu_payload = 0;
			irq = 0;
			level = false;
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
		
		unsigned int GetIRQ() const
		{
			return irq;
		}

		unsigned int GetLevel() const
		{
			return level;
		}

		sc_event *GetCompletionEvent() const
		{
			return ev_completed;
		}
		
	private:
		Type type;
		sc_time time_stamp;
		tlm::tlm_generic_payload *cpu_payload;
		unsigned int irq;
		bool level;
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
		ScheduleKey(const sc_time& _time_stamp, typename Event::Type _type, unsigned int _irq)
			: time_stamp(_time_stamp)
			, type(_type)
			, irq(_irq)
		{
		}
		
		int operator < (const ScheduleKey& sk) const
		{
			return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type) || ((type == sk.type) && (irq < sk.irq))));
		}
		
		const sc_time& GetTimeStamp() const
		{
			return time_stamp;
		}
	private:
		sc_time time_stamp;
		typename Event::Type type;
		unsigned int irq;
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
		
		void NotifyIRQEvent(unsigned int irq, bool level, const sc_time& time_stamp)
		{
			ScheduleKey key = ScheduleKey(time_stamp, Event::EV_IRQ, irq);
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.find(key);
			if(it != schedule.end())
			{
				if((*it).second->GetLevel() == level) return; // Already scheduled
			}
			Event *event = event_allocator.AllocEvent();
			event->InitializeIRQEvent(irq, level, time_stamp);
			schedule.insert(std::pair<ScheduleKey, Event *>(key, event));
			sc_time t(time_stamp);
			t -= sc_time_stamp();
			kernel_event.notify(t);
		}
		
		void Notify(Event *event)
		{
			const sc_time& time_stamp = event->GetTimeStamp();
			schedule.insert(std::pair<ScheduleKey, Event *>(ScheduleKey(time_stamp, event->GetType(), event->GetIRQ()), event));
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

	class FwRedirector : public tlm::tlm_fw_transport_if<unisim::kernel::tlm2::SimpleProtocolTypes<bool> >
	{
	public:
		FwRedirector(unsigned int id, XPS_IntC<CONFIG> *xps_intc, tlm::tlm_sync_enum (XPS_IntC<CONFIG>::*cb_nb_transport_fw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&), void (XPS_IntC<CONFIG>::*cb_b_transport)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, sc_core::sc_time&));
		
		virtual void b_transport(unisim::kernel::tlm2::SimplePayload<bool>& trans, sc_core::sc_time& t);

		virtual tlm::tlm_sync_enum nb_transport_fw(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

		virtual unsigned int transport_dbg(unisim::kernel::tlm2::SimplePayload<bool>& trans);
		
		virtual bool get_direct_mem_ptr(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_dmi& dmi_data);

	private:
		unsigned int id;
		XPS_IntC<CONFIG> *xps_intc;
		tlm::tlm_sync_enum (XPS_IntC<CONFIG>::*cb_nb_transport_fw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&);
		void (XPS_IntC<CONFIG>::*cb_b_transport)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, sc_core::sc_time&);
	};
	
	/** Cycle time */
	sc_time cycle_time;
	/** Latencies */
	sc_time read_latency;
	sc_time write_latency;

	sc_time time_stamp;
	sc_time ready_time_stamp;
	
	/** The parameter for the cycle time */
	Parameter<sc_time> param_cycle_time;
	/** The parameters to set the latencies */
	Parameter<sc_time> param_read_latency;
	Parameter<sc_time> param_write_latency;

	FwRedirector *irq_redirector[CONFIG::C_NUM_INTR_INPUTS];
	bool interrupt_input[CONFIG::C_NUM_INTR_INPUTS];
	bool output_level;
	Schedule schedule;
	
	PayloadFabric<InterruptPayload> interrupt_payload_fabric;

	void ProcessCPUEvent(Event *event);
	void ProcessIRQEvent(Event *event);
};

} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
