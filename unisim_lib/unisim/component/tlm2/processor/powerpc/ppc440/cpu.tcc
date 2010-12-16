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

template <class CONFIG>
CPU<CONFIG>::CPU(const sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a PPC440 CPU core")
	, sc_module(name)
	, unisim::component::cxx::processor::powerpc::ppc440::CPU<CONFIG>(name, parent)
	, bus_master_sock("bus-master-sock")
	, external_input_interrupt_slave_sock("external-input-interrupt-slave-sock")
	, critical_input_interrupt_slave_sock("critical-input-interrupt-slave-sock")
	, payload_fabric()
	, cpu_cycle_time()
	, bus_cycle_time()
	, ext_timer_cycle_time()
	, cpu_time()
	, nice_time()
	, max_idle_time()
	, ev_max_idle()
	, ev_irq()
	, ipc(1.0)
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time, "bus cycle time")
	, param_ext_timer_cycle_time("ext-timer-cycle-time", this, ext_timer_cycle_time, "external timer cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "targeted average instructions per second")
	, external_input_interrupt_queue("external-input-interrupt-queue")
	, critical_input_interrupt_queue("critical-input-interrupt-queue")
{
	bus_master_sock(*this);
	external_input_interrupt_slave_sock(external_input_interrupt_queue);
	critical_input_interrupt_slave_sock(critical_input_interrupt_queue);
	
	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
	SC_THREAD(SignalExternalInputInterrupt);
	SC_THREAD(SignalCriticalInputInterrupt);
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
}

template <class CONFIG>
bool CPU<CONFIG>::EndSetup()
{
	if(!inherited::EndSetup()) return false;
	cpu_cycle_time = sc_time((double) inherited::cpu_cycle_time, SC_PS);
	return true;
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <class CONFIG>
void CPU<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
	wait(cpu_time);
	//std::cerr << "sc_time_stamp() = " << sc_time_stamp() << ":" << std::endl << unisim::kernel::debug::BackTrace() << std::endl;
	timer_time -= cpu_time;
	cpu_time = SC_ZERO_TIME;
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateTime()
{
	if(unlikely(cpu_time > timer_time))
	{
		const sc_time& timer_cycle_time = inherited::GetCCR1_TCS() ? ext_timer_cycle_time : cpu_cycle_time;
		do
		{
			//std::cerr << "timer_time=" << timer_time << std::endl;
			inherited::OnTimerClock();
			timer_time += timer_cycle_time;
		}
		while(unlikely(cpu_time > timer_time));
	}

	if(unlikely(cpu_time >= nice_time))
	{
		Synchronize();
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::AlignToBusClock()
{
	sc_dt::uint64 time_tu = sc_time_stamp().value();
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	sc_dt::uint64 cpu_time_tu = cpu_time.value() + time_tu;
	sc_dt::uint64 modulo = cpu_time_tu % bus_cycle_time_tu;
	if(!modulo) return; // already aligned

	cpu_time_tu += bus_cycle_time_tu - modulo;
	cpu_time = sc_time(cpu_time_tu - time_tu, false);
	UpdateTime();
}

template <class CONFIG>
void CPU<CONFIG>::SignalIRQ(IRQQueue& queue, unsigned int irq)
{
	while(1)
	{
		wait(queue.GetEvent());
		TLMInterruptPayload *payload;
		
		payload = queue.GetNextIRQ();
		if(payload)
		{
			do
			{
				if(payload->level)
					inherited::SetIRQ(irq);
				else
					inherited::ResetIRQ(irq);
				
				payload->release();
			}
			while((payload = queue.GetNextIRQ()) != 0);

			ev_irq.notify(SC_ZERO_TIME);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::SignalExternalInputInterrupt()
{
	SignalIRQ(external_input_interrupt_queue, CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT);
}

template <class CONFIG>
void CPU<CONFIG>::SignalCriticalInputInterrupt()
{
	SignalIRQ(critical_input_interrupt_queue, CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT);
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
	}
}

template <class CONFIG>
bool CPU<CONFIG>::BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	payload->release();

	UpdateTime();
	
	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
bool CPU<CONFIG>::BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	tlm::tlm_response_status status = payload->get_response_status();

	payload->release();

	UpdateTime();
	
	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
CPU<CONFIG>::IRQQueue::IRQQueue(const char *name)
	: queue(name)
{
}

template <class CONFIG>
void CPU<CONFIG>::IRQQueue::b_transport(TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	trans.acquire();
	queue.notify(trans, t);
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::IRQQueue::nb_transport_fw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	trans.acquire();
	queue.notify(trans, t);
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
unsigned int CPU<CONFIG>::IRQQueue::transport_dbg(TLMInterruptPayload& trans)
{
	return 0;
}

template <class CONFIG>
bool CPU<CONFIG>::IRQQueue::get_direct_mem_ptr(TLMInterruptPayload& trans, tlm::tlm_dmi&  dmi_data)
{
	return false;
}

template <class CONFIG>
TLMInterruptPayload *CPU<CONFIG>::IRQQueue::GetNextIRQ()
{
	return queue.get_next_transaction();
}

template <class CONFIG>
sc_event& CPU<CONFIG>::IRQQueue::GetEvent()
{
	return queue.get_event();
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
