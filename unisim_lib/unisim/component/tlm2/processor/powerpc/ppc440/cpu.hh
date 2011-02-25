/*
 *  Copyright (c) 2007-2010,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_PPC440_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_PPC440_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/ppc440/cpu.hh>
#include <systemc.h>
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <inttypes.h>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace ppc440 {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::logger::Logger;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::InterruptPayload;

template <class CONFIG>
class CPU
	: public sc_module
	, public unisim::component::cxx::processor::powerpc::ppc440::CPU<CONFIG>
	, public tlm::tlm_bw_transport_if<>
{
public:
	
	typedef unisim::component::cxx::processor::powerpc::ppc440::CPU<CONFIG> inherited;
	typedef tlm::tlm_initiator_socket<CONFIG::FSB_WIDTH * 8> bus_master_socket;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;

	bus_master_socket bus_master_sock;
	irq_slave_socket external_input_interrupt_slave_sock;
	irq_slave_socket critical_input_interrupt_slave_sock;
	
	CPU(const sc_module_name& name, Object *parent = 0);
	virtual ~CPU();
	
	virtual bool EndSetup();
	
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	virtual void interrupt_b_transport(unsigned int irq, InterruptPayload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void Synchronize();
	
	void Run();
	
protected:
	virtual bool BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr = CONFIG::SA_DEFAULT);
	virtual bool BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr = CONFIG::SA_DEFAULT);
private:
	
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time ext_timer_cycle_time;
	sc_time cpu_time;
	sc_time timer_time;
	sc_time nice_time;
	sc_time max_idle_time;
	sc_event ev_max_idle;
	sc_event ev_irq;
	double ipc;

	Parameter<sc_time> param_bus_cycle_time;
	Parameter<sc_time> param_ext_timer_cycle_time;
	Parameter<sc_time> param_nice_time;
	Parameter<double> param_ipc;
	
	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_IRQ,               // an IRQ event
		} Type;
		
		typedef enum
		{
			IRQ_CRITICAL_INPUT = 0,
			IRQ_EXTERNAL_INPUT = 1,
		} IRQ;
		
		Event()
			: type(EV_IRQ)
			, time_stamp(SC_ZERO_TIME)
			, irq(0)
			, level(false)
		{
		}
		
		~Event()
		{
			Clear();
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
			type = EV_IRQ;
			time_stamp = SC_ZERO_TIME;
			irq = 0;
			level = false;
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
		
		unsigned int GetIRQ() const
		{
			return irq;
		}
		
		bool GetLevel() const
		{
			return level;
		}
		
	private:
		Type type;
		sc_time time_stamp;
		unsigned int irq;
		bool level;
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
		FwRedirector(unsigned int id, CPU<CONFIG> *cpu, tlm::tlm_sync_enum (CPU<CONFIG>::*cb_nb_transport_fw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&), void (CPU<CONFIG>::*cb_b_transport)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, sc_core::sc_time&));
		
		virtual void b_transport(unisim::kernel::tlm2::SimplePayload<bool>& trans, sc_core::sc_time& t);

		virtual tlm::tlm_sync_enum nb_transport_fw(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

		virtual unsigned int transport_dbg(unisim::kernel::tlm2::SimplePayload<bool>& trans);
		
		virtual bool get_direct_mem_ptr(unisim::kernel::tlm2::SimplePayload<bool>& trans, tlm::tlm_dmi& dmi_data);

	private:
		unsigned int id;
		CPU<CONFIG> *cpu;
		tlm::tlm_sync_enum (CPU<CONFIG>::*cb_nb_transport_fw)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, tlm::tlm_phase&, sc_core::sc_time&);
		void (CPU<CONFIG>::*cb_b_transport)(unsigned int, unisim::kernel::tlm2::SimplePayload<bool>&, sc_core::sc_time&);
	};

	Schedule external_event_schedule;
	FwRedirector *critical_input_interrupt_redirector;
	FwRedirector *external_input_interrupt_redirector;

	inline void UpdateTime();
	inline void AlignToBusClock();
	void AlignToBusClock(sc_time& t);
	void ProcessExternalEvents();
	void ProcessIRQEvent(Event *event);
};

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
