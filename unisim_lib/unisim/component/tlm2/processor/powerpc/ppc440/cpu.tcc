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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_PPC440_CPU_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_PPC440_CPU_TCC__

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace ppc440 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
CPU<CONFIG>::CPU(const sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a PPC440 CPU core")
	, sc_module(name)
	, unisim::component::cxx::processor::powerpc::ppc440::CPU<CONFIG>(name, parent)
	, icurd_plb_master_sock("icurd-plb-master-sock")
	, dcuwr_plb_master_sock("dcuwr-plb-master-sock")
	, dcurd_plb_master_sock("dcurd-plb-master-sock")
	, external_input_interrupt_slave_sock("external-input-interrupt-slave-sock")
	, critical_input_interrupt_slave_sock("critical-input-interrupt-slave-sock")
	, dcr_master_sock("dcr-master-sock")
	, payload_fabric()
	, cpu_cycle_time()
	, bus_cycle_time()
	, ext_timer_cycle_time()
	, cpu_time()
	, nice_time()
	, max_idle_time()
	, run_time()
	, idle_time()
	, ev_max_idle()
	, ev_irq()
	, ipc(1.0)
	, one(1.0)
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time, "bus cycle time")
	, param_ext_timer_cycle_time("ext-timer-cycle-time", this, ext_timer_cycle_time, "external timer cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "targeted average instructions per second")
	, stat_one("one", this, one, "one")
	, stat_run_time("run-time", this, run_time, "run time")
	, stat_idle_time("idle-time", this, idle_time, "idle time")
	, formula_idle_rate("idle-rate", this, Formula<double>::OP_DIV, &stat_idle_time, &stat_run_time, "idle rate")
	, formula_load_rate("load-rate", this, Formula<double>::OP_SUB, &stat_one, &formula_idle_rate, "load rate")
	, external_event_schedule()
	, critical_input_interrupt_redirector(0)
	, external_input_interrupt_redirector(0)
	, icurd_plb_redirector(0)
	, dcuwr_plb_redirector(0)
	, dcurd_plb_redirector(0)
	, dcr_redirector(0)
{
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	
	icurd_plb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG> >(
		IF_ICURD_PLB,
		this,
		&CPU<CONFIG>::nb_transport_bw,
		&CPU<CONFIG>::invalidate_direct_mem_ptr
	);
	icurd_plb_master_sock(*icurd_plb_redirector); // Bind socket to the interface implementer
	
	dcuwr_plb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG> >(
		IF_DCUWR_PLB,
		this,
		&CPU<CONFIG>::nb_transport_bw,
		&CPU<CONFIG>::invalidate_direct_mem_ptr
	);
	dcuwr_plb_master_sock(*dcuwr_plb_redirector); // Bind socket to the interface implementer

	dcurd_plb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG> >(
		IF_DCURD_PLB,
		this,
		&CPU<CONFIG>::nb_transport_bw,
		&CPU<CONFIG>::invalidate_direct_mem_ptr
	);
	dcurd_plb_master_sock(*dcurd_plb_redirector); // Bind socket to the interface implementer
	
	critical_input_interrupt_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_CRITICAL_INPUT,
		this, &CPU<CONFIG>::interrupt_nb_transport_fw,
		&CPU<CONFIG>::interrupt_b_transport,
		&CPU<CONFIG>::interrupt_transport_dbg,
		&CPU<CONFIG>::interrupt_get_direct_mem_ptr
	);
	critical_input_interrupt_slave_sock(*critical_input_interrupt_redirector); // Bind socket to implementer of interface
	
	external_input_interrupt_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_EXTERNAL_INPUT,
		this,
		&CPU<CONFIG>::interrupt_nb_transport_fw,
		&CPU<CONFIG>::interrupt_b_transport,
		&CPU<CONFIG>::interrupt_transport_dbg,
		&CPU<CONFIG>::interrupt_get_direct_mem_ptr
	);
	external_input_interrupt_slave_sock(*external_input_interrupt_redirector); // Bind socket to implementer of interface
	
	dcr_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG> >(
		IF_DCR,
		this,
		&CPU<CONFIG>::nb_transport_bw,
		&CPU<CONFIG>::invalidate_direct_mem_ptr
	);
	dcr_master_sock(*dcr_redirector);
	
	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
	if(icurd_plb_redirector)
	{
		delete icurd_plb_redirector;
	}
	
	if(dcuwr_plb_redirector)
	{
		delete dcuwr_plb_redirector;
	}
	
	if(dcurd_plb_redirector)
	{
		delete dcurd_plb_redirector;
	}
	
	if(critical_input_interrupt_redirector)
	{
		delete critical_input_interrupt_redirector;
	}
	
	if(external_input_interrupt_redirector)
	{
		delete external_input_interrupt_redirector;
	}
	
	if(dcr_redirector)
	{
		delete dcr_redirector;
	}
	
	//std::cerr << "total time=" << total_time << std::endl;
}

template <class CONFIG>
bool CPU<CONFIG>::EndSetup()
{
	if(!inherited::EndSetup()) return false;
	cpu_cycle_time = sc_time((double) inherited::cpu_cycle_time, SC_PS);
	return true;
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::nb_transport_bw(unsigned int if_id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <class CONFIG>
void CPU<CONFIG>::invalidate_direct_mem_ptr(unsigned int if_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
	wait(cpu_time);
	cpu_time = SC_ZERO_TIME;
	run_time = sc_time_stamp();
}

template <class CONFIG>
void CPU<CONFIG>::ProcessExternalEvents()
{
	sc_time time_stamp = sc_time_stamp();
	time_stamp += cpu_time;
	Event *event = external_event_schedule.GetNextEvent(time_stamp);
	
	if(event)
	{
		do
		{
			switch(event->GetType())
			{
				case Event::EV_IRQ:
					ProcessIRQEvent(event);
					external_event_schedule.FreeEvent(event);
					break;
			}
		}
		while((event = external_event_schedule.GetNextEvent(time_stamp)) != 0);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ProcessIRQEvent(Event *event)
{
	if(inherited::IsVerboseException())
	{
		inherited::logger << DebugInfo << (sc_time_stamp() + cpu_time) << ": processing an IRQ event that occured at " << event->GetTimeStamp() << " (";
		switch(event->GetIRQ())
		{
			case CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT:
				inherited::logger << "external";
				break;
			case CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT:
				inherited::logger << "critical";
				break;
			default:
				inherited::logger << "?";
				break;
		}
		inherited::logger << " input goes " << (event->GetLevel() ? "high" : "low") << "). Event skew is " << (sc_time_stamp() + cpu_time - event->GetTimeStamp()) << "." << EndDebugInfo;
	}
	if(event->GetLevel())
		inherited::SetIRQ(event->GetIRQ());
	else
		inherited::ResetIRQ(event->GetIRQ());
}

template <class CONFIG>
void CPU<CONFIG>::interrupt_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t)
{
	if(irq < 2)
	{
		bool level = payload.GetValue();
		sc_time notify_time_stamp(sc_time_stamp());
		notify_time_stamp += t;

		AlignToBusClock(notify_time_stamp);
		if(inherited::IsVerboseException())
		{
			inherited::logger << DebugInfo << notify_time_stamp << ": " << (irq ? "External" : "Critical") << " input interrupt signal goes " << (level ? "high" : "low") << EndDebugInfo;
		}
		external_event_schedule.NotifyIRQEvent(irq ? CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT : CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT, level, notify_time_stamp);
	}
	else
	{
		inherited::logger << DebugError << "protocol error (invalid IRQ number)" << EndDebugError;
		Object::Stop(-1);
	}
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				if(irq < 2)
				{
					bool level = payload.GetValue();
					sc_time notify_time_stamp(sc_time_stamp());
					notify_time_stamp += t;

					AlignToBusClock(notify_time_stamp);
					if(inherited::IsVerboseException())
					{
						inherited::logger << DebugInfo << notify_time_stamp << ": " << (irq ? "External" : "Critical") << " input interrupt signal goes " << (level ? "high" : "low") << EndDebugInfo;
					}
					external_event_schedule.NotifyIRQEvent(irq ? CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT : CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT, level, notify_time_stamp);
				}
				else
				{
					inherited::logger << DebugError << "protocol error (invalid IRQ number)" << EndDebugError;
					Object::Stop(-1);
				}
				
				phase = tlm::END_REQ;
				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
unsigned int CPU<CONFIG>::interrupt_transport_dbg(unsigned int, InterruptPayload& trans)
{
	return 0;
}

template <class CONFIG>
bool CPU<CONFIG>::interrupt_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateTime()
{
	if(unlikely(run_time > timer_time))
	{
		const sc_time& timer_cycle_time = inherited::GetCCR1_TCS() ? ext_timer_cycle_time : cpu_cycle_time;
		do
		{
			//std::cerr << "timer_time=" << timer_time << std::endl;
			inherited::RunTimers(1);
			timer_time += timer_cycle_time;
		}
		while(unlikely(run_time > timer_time));
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::AlignToBusClock()
{
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	sc_dt::uint64 time_tu = sc_time_stamp().value();
	sc_dt::uint64 cpu_time_tu = cpu_time.value() + time_tu;
	sc_dt::uint64 modulo = cpu_time_tu % bus_cycle_time_tu;
	if(!modulo) return; // already aligned

	cpu_time_tu += bus_cycle_time_tu - modulo;
	cpu_time = sc_time(cpu_time_tu - time_tu, false);
}

template <class CONFIG>
void CPU<CONFIG>::AlignToBusClock(sc_time& t)
{
	sc_dt::uint64 time_tu = t.value();
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	sc_dt::uint64 modulo = time_tu % bus_cycle_time_tu;
	if(!modulo) return; // already aligned

	time_tu += bus_cycle_time_tu - modulo;
	t = sc_time(time_tu, false);
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
	// This is called within thread Run()
	const sc_time& timer_cycle_time = inherited::GetCCR1_TCS() ? ext_timer_cycle_time : cpu_cycle_time;
	max_idle_time = timer_cycle_time;
	max_idle_time *= inherited::GetMaxIdleTime();
#if 0
	usleep(max_idle_time.to_seconds() * 1.0e6);
#endif
	ev_max_idle.notify(max_idle_time);
	//std::cerr << sc_time_stamp() << ": Idle wait for at most " << max_idle_time << std::endl;
	sc_time old_time_stamp(sc_time_stamp());
	wait(ev_max_idle | ev_irq);
	ev_max_idle.cancel();
	sc_time new_time_stamp(sc_time_stamp());
	
	sc_time delta_time(new_time_stamp);
	delta_time -= old_time_stamp;
	
	if(delta_time > cpu_time)
	{
		idle_time += delta_time;
		idle_time -= cpu_time;
		cpu_time = SC_ZERO_TIME;
	}
	else
	{
		cpu_time -= delta_time;
	}
	run_time = new_time_stamp;
	run_time += cpu_time;
	UpdateTime();
	ProcessExternalEvents();
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
	sc_time time_per_instruction = cpu_cycle_time * ipc;
	
	while(1)
	{
		inherited::StepOneInstruction();
		cpu_time += time_per_instruction;
		UpdateTime();
		ProcessExternalEvents();
		if(unlikely(cpu_time >= nice_time))
		{
			Synchronize();
		}
	}
}

template <class CONFIG>
bool CPU<CONFIG>::PLBInsnRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	icurd_plb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
bool CPU<CONFIG>::PLBDataRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	dcurd_plb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;

	tlm::tlm_response_status status = payload->get_response_status();
	
	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
bool CPU<CONFIG>::PLBDataWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	dcuwr_plb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;

	tlm::tlm_response_status status = payload->get_response_status();

	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
void CPU<CONFIG>::DCRRead(unsigned int dcrn, uint32_t& value)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	uint32_t buffer = 0;
	
	payload->set_address(4 * dcrn);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(4);
	payload->set_data_ptr((unsigned char *) &buffer);
	
	dcr_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;

	tlm::tlm_response_status status = payload->get_response_status();
	
	payload->release();

	value = unisim::util::endian::BigEndian2Host(buffer);
}

template <class CONFIG>
void CPU<CONFIG>::DCRWrite(unsigned int dcrn, uint32_t value)
{
	uint32_t buffer = unisim::util::endian::Host2BigEndian(value);
	
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(4 * dcrn);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(4);
	payload->set_data_ptr((unsigned char *) &buffer);
	
	dcr_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;

	tlm::tlm_response_status status = payload->get_response_status();

	payload->release();

}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
