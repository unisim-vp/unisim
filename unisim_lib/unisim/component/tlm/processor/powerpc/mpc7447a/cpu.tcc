/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_MPC7447A_CPU_TCC__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_MPC7447A_CPU_TCC__

#include <unistd.h>
#include <unisim/component/cxx/processor/powerpc/exception.tcc>

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace powerpc {
namespace mpc7447a {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
	
template <class CONFIG>
CPU<CONFIG>::CPU(const sc_module_name& name, Object *parent) :
	Object(name, parent, "PowerPC MPC7447A CPU"),
	sc_module(name),
	unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG>(name, parent),
	bus_port("bus-port"),
	snoop_port("snoop-port"),
	cpu_cycle_sctime(),
	bus_cycle_sctime(),
	cpu_sctime(),
	bus_sctime(),
	last_sync_sctime(),
	nice_sctime(100.0, SC_MS),
	next_nice_sctime(),
	//nice_time(100000000000ULL), // leave at least 10 times per simulated seconds the host processor to the SystemC simulation kernel 
	ipc(1.0),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_sctime, "bus cycle time"),
	param_nice_time("nice-time", this, nice_sctime, "maximum time between synchonizations"),
	param_ipc("ipc", this, ipc, "targeted average instructions per second"),
	external_interrupt_listener("external_interrupt_listener", CONFIG::IRQ_EXTERNAL_INTERRUPT, this, &ev_interrupt),
	hard_reset_listener("hard_reset_listener", CONFIG::IRQ_HARD_RESET, this, &ev_interrupt),
	soft_reset_listener("soft_reset_listener", CONFIG::IRQ_SOFT_RESET, this, &ev_interrupt),
	mcp_listener("mcp_listener", CONFIG::IRQ_MCP, this, &ev_interrupt),
	tea_listener("tea_listener", CONFIG::IRQ_TEA, this, &ev_interrupt),
	smi_listener("smi_listener", CONFIG::IRQ_SMI, this, &ev_interrupt)
{
	//param_nice_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_ipc.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	SC_HAS_PROCESS(CPU);
	
	snoop_port(*this);
	external_interrupt_port(external_interrupt_listener);
	hard_reset_port(hard_reset_listener);
	soft_reset_port(soft_reset_listener);
	mcp_port(mcp_listener);
	tea_port(tea_listener);
	smi_port(smi_listener);

	SC_THREAD(Run);
	
	SC_THREAD(BusMaster);
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
}
	
template <class CONFIG>
void CPU<CONFIG>::Stop(int ret)
{
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	if(unlikely(inherited::IsVerboseStep()))
	{
		inherited::logger << DebugInfo << "Program exited with status " << ret << EndDebugInfo;
	}
	BusSynchronize();
	Object::Stop(ret);
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
	sc_dt::uint64 cpu_time_tu = cpu_sctime.value();
	sc_dt::uint64 last_sync_time_tu = last_sync_sctime.value();
	wait(cpu_time_tu - last_sync_time_tu, SC_PS);
//	cpu_sctime = sc_time_stamp();
	last_sync_sctime = cpu_sctime;
	next_nice_sctime = last_sync_sctime;
	next_nice_sctime += nice_sctime;
}

	
template <class CONFIG>
bool CPU<CONFIG>::Setup()
{
	if(!inherited::Setup()) return false;
	cpu_cycle_sctime = sc_time((double) inherited::cpu_cycle_time, SC_PS);
	//bus_cycle_sctime = sc_time((double) inherited::bus_cycle_time, SC_PS);
	//nice_sctime = sc_time((double) nice_time, SC_PS); // 10000 * cpu_cycle_sctime;//
	return true;
}

template <class CONFIG>
void CPU<CONFIG>::BusSynchronize()
{
	if(unlikely(cpu_sctime > bus_sctime))
	{
		//cout << "Time: " << ((cpu_sctime - bus_sctime + bus_cycle_sctime) / bus_cycle_sctime) << endl;
		do
		{
			bus_sctime += bus_cycle_sctime;
			inherited::OnBusCycle();
		} while(unlikely(cpu_sctime > bus_sctime));
	}
	
	Synchronize();
}

/*template <class CONFIG>
void CPU<CONFIG>::BusSynchronize()
{
	sc_time time_spent = cpu_sctime - bus_sctime;

#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << "PPC: Bus synchro ----------------------" << endl;
#endif
	sc_dt::uint64 current_time_tu = sc_time_stamp().value();
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << "PPC: current_time_tu = " << current_time_tu << endl;
#endif		
	sc_dt::uint64 time_spent_tu = time_spent.value();
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << "PPC: time_spent_tu = " << time_spent_tu << endl;
#endif
	sc_dt::uint64 next_time_tu = current_time_tu + time_spent_tu;
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << "PPC: next_time_tu = " << next_time_tu << endl;
#endif
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_sctime.value();
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << "PPC: bus_cycle_time_tu = " << bus_cycle_time_tu << endl;
#endif
	sc_dt::uint64 bus_time_phase_tu = next_time_tu % bus_cycle_time_tu;
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << "PPC: bus_time_phase_tu = " << bus_time_phase_tu << endl;
#endif
	if(time_spent_tu || bus_time_phase_tu)
	{
		sc_dt::uint64 delay_tu = next_time_tu - current_time_tu + (bus_cycle_time_tu - bus_time_phase_tu);
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << "PPC: delay_tu = " << delay_tu << endl;
#endif
		UpdateTime(sc_time(delay_tu, false));
		Synchronize();
//		wait(sc_time(delay_tu, false));
//		cpu_sctime = bus_sctime = sc_time_stamp();
	}
}*/
	
// template <class CONFIG>
// void CPU<CONFIG>::Run()
// {
// 	sc_time time_per_instruction = cpu_cycle_sctime * ipc;
// 	
// 	while(1)
// 	{
// 		if(cpu_sctime >= bus_sctime)
// 		{
// 			bus_sctime += bus_cycle_sctime;
// 			CPU<CONFIG>::OnBusCycle();
// 			if(cpu_sctime >= next_nice_sctime)
// 			{
// 				Synchronize();
// 			}
// 		}
// 		CPU<CONFIG>::StepOneInstruction();
// 		cpu_sctime += time_per_instruction;
// 	}
// }

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
	UpdateBusTime();
	max_idle_time = bus_cycle_sctime;
	max_idle_time *= inherited::GetDEC() * 4;
#if 0
	usleep(max_idle_time.to_seconds() * 1.0e6);
#endif
	ev_max_idle.notify(max_idle_time);
	wait(ev_max_idle | ev_interrupt);
	ev_max_idle.cancel();
	cpu_sctime = sc_time_stamp();
	last_sync_sctime = cpu_sctime;
	UpdateBusTime();
	while(likely(!inherited::HasIRQ()))
	{
		cpu_sctime += cpu_cycle_sctime;
		UpdateBusTime();
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateBusTime()
{
	if(unlikely(cpu_sctime > bus_sctime))
	{
		do
		{
			bus_sctime += bus_cycle_sctime;
			inherited::OnBusCycle();
		}
		while(unlikely(cpu_sctime > bus_sctime));

		if(unlikely(cpu_sctime > next_nice_sctime))
		{
			Synchronize();
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
#if 1
	sc_time time_per_instruction = cpu_cycle_sctime * ipc;
	
	while(1)
	{
		inherited::StepOneInstruction();
		cpu_sctime += time_per_instruction;
		UpdateBusTime();
	}
#else
	while(1)
	{
		inherited::StepOneCycle();
		UpdateTime(cpu_cycle_sctime);
	}
#endif
}

template <class CONFIG>
void CPU<CONFIG>::BusMaster()
{
	while(1)
	{
		unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG> *bus_access = 0;
		bus_access_queue.read(bus_access); // blocking read
		
		inherited::logger << DebugInfo << "BusMaster" << EndDebugInfo;
		switch(bus_access->type)
		{
			case unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG>::LOAD:
			case unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG>::REFILL:
			case unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG>::REFILLX:
				BusRead(bus_access->addr, bus_access->storage, bus_access->size, bus_access->wimg, bus_access->rwitm);
				inherited::logger << DebugInfo << "BusMaster: finished bus access" << EndDebugInfo;
				OnFinishedBusAccess(bus_access);
				break;
			case unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG>::STORE:
			case unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG>::EVICTION:
				BusWrite(bus_access->addr, bus_access->storage, bus_access->size, bus_access->wimg);
				OnFinishedBusAccess(bus_access);
				break;
		}
	}
}

	
template <class CONFIG>
bool CPU<CONFIG>::Send(const Pointer<TlmMessage<FSBReq, FSBRsp> >& message)
{
	return true;
}

template <class CONFIG>
void CPU<CONFIG>::Reset()
{
}
	
template <class CONFIG>
void CPU<CONFIG>::BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg, bool rwitm)
{
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " " << name() << "::BusRead()" << endl;
#endif
	// synchonize with bus cycle time
	BusSynchronize();
		
	Pointer<FSBReq> req = new(req) FSBReq(); // bus transaction request
	Pointer<FSBRsp> rsp; // bus transaction response
	sc_event rsp_ev; // event notified when bus transaction response is ready
	Pointer<TlmMessage<FSBReq, FSBRsp> > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp>(req, rsp_ev); // message for bus transaction
	
	req->type = rwitm ? FSBReq::READX : FSBReq::READ; // transaction is a READ/READX
	req->global = (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? true : false; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = size; // actual transaction size
	
	// loop until the request succeeds
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusRead: loop until the request succeeds" << endl;
#endif
	// request succeeds when none of the processors is busy or have a modified copy of the request data in there caches
	do
	{
		// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusRead: loop until bus transaction request is accepted" << endl;
#endif
		while(!bus_port->Send(msg))
		{
			// if bus transaction request is not accepted then retry later
			wait(bus_cycle_sctime);
			//cpu_sctime = sc_time_stamp();
			cpu_sctime += bus_cycle_sctime;
			last_sync_sctime = cpu_sctime;
			UpdateBusTime();
#ifdef DEBUG_POWERPC
			if(DebugEnabled())
				cerr << sc_time_stamp() << " PPC::BusRead: retry transaction request" << endl;
#endif
		}
		
		// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
		{
			cerr << sc_time_stamp() << " PPC::BusRead: transaction request accepted" << endl;
			cerr << sc_time_stamp() << " PPC::BusRead: waiting for response" << endl;
		}
#endif
		// wait for the bus transaction response
		wait(rsp_ev);
		cpu_sctime = sc_time_stamp();
		last_sync_sctime = cpu_sctime;
		UpdateBusTime();
		rsp = msg->GetResponse();
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
		{
			cerr << sc_time_stamp() << " PPC::BusRead: received response" << endl;
			if(!rsp->read_status & (FSBRsp::RS_BUSY | FSBRsp::RS_MODIFIED))
				cerr << sc_time_stamp() << " PPC::BusRead: response read status is neither RS_BUSY nor RS_MODIFIED" << endl;
		}
#endif
	} while(rsp->read_status & (FSBRsp::RS_BUSY | FSBRsp::RS_MODIFIED));
	
	// bus transaction response has been received at this point
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusRead: copying response data" << endl;
#endif
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		if(rsp->read_status & FSBRsp::RS_SHARED)
			cerr << sc_time_stamp() << " PPC::BusRead: read status contains RS_SHARED" << endl;
#endif

	// get the bus transaction response read status in order to update the block state
}
	
	
template <class CONFIG>
void CPU<CONFIG>::BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg)
{
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " " << name() << "::BusWrite()" << endl;
#endif
	// synchonize with bus cycle time
	BusSynchronize();
		
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req); // message for bus transaction
	
	req->type = FSBReq::WRITE; // transaction is a WRITE
	req->global = (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? true : false; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = size; // actual transaction size
	// copy the data into the bus transaction request
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusWrite: copying data into transaction request" << endl;
#endif
	memcpy(req->write_data, buffer, size);
		
	// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusWrite: loop until the request succeeds" << endl;
#endif
	while(!bus_port->Send(msg))
	{
		// if bus transaction request is not accepted then retry later
		wait(bus_cycle_sctime);
		//cpu_sctime = sc_time_stamp();
		cpu_sctime += bus_cycle_sctime;
		last_sync_sctime = cpu_sctime;
		UpdateBusTime();
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusWrite: retry transaction request" << endl;
#endif
	}
	// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusWrite: transaction request accepted" << endl;
#endif
	req = 0;
	msg = 0;
}

template <class CONFIG>
void CPU<CONFIG>::BusZeroBlock(physical_address_t physical_addr)
{
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " " << name() << "::BusZeroBlock()" << endl;
#endif
	// synchonize with bus cycle time
	BusSynchronize();
		
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req); // message for bus transaction
	
	req->type = FSBReq::ZERO_BLOCK; // transaction is a ZERO_BLOCK
	req->global = true; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = 0; // actual transaction size
		
	// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusZeroBlock: loop until the request succeeds" << endl;
#endif
	while(!bus_port->Send(msg))
	{
		// if bus transaction request is not accepted then retry later
		wait(bus_cycle_sctime);
		//cpu_sctime = sc_time_stamp();
		cpu_sctime += bus_cycle_sctime;
		last_sync_sctime = cpu_sctime;
		UpdateBusTime();
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusZeroBlock: retry transaction request" << endl;
#endif
	}
	// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusZeroBlock: transaction request accepted" << endl;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::BusFlushBlock(physical_address_t physical_addr)
{
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " " << name() << "::BusFlushBlock()" << endl;
#endif
	// synchonize with bus cycle time
	BusSynchronize();
		
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req); // message for bus transaction
	
	req->type = FSBReq::FLUSH_BLOCK; // transaction is a FLUSH_BLOCK
	req->global = true; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = 0; // actual transaction size
		
	// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusFlushBlock: loop until the request succeeds" << endl;
#endif
	while(!bus_port->Send(msg))
	{
		// if bus transaction request is not accepted then retry later
		wait(bus_cycle_sctime);
		//cpu_sctime = sc_time_stamp();
		cpu_sctime += bus_cycle_sctime;
		last_sync_sctime = cpu_sctime;
		UpdateBusTime();
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusFlushBlock: retry transaction request" << endl;
#endif
	}
	// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusFlushBlock: transaction request accepted" << endl;
#endif
}

template <class CONFIG>
void CPU<CONFIG>::DoBusAccess(unisim::component::cxx::processor::powerpc::mpc7447a::BusAccess<CONFIG> *bus_access)
{
	inherited::logger << DebugInfo << "DoBusAccess" << EndDebugInfo;
	if(!bus_access_queue.nb_write(bus_access))
	{
		inherited::logger << DebugInfo << "DoBusAccess failed" << EndDebugInfo;
		Object::Stop(-1);
	}
}

template <class CONFIG>
CPU<CONFIG>::IRQListener::IRQListener(const sc_module_name& name, unsigned int _irq, unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	irq(_irq),
	cpu(_cpu),
	ev(_ev)
{
}
	
template <class CONFIG>
bool CPU<CONFIG>::IRQListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->SetIRQ(irq);
	else
		cpu->ResetIRQ(irq);
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
