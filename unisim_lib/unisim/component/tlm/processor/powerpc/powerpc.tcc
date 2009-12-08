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
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_POWERPC_TCC__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_POWERPC_TCC__

#include <unistd.h>

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace powerpc {

template <class CONFIG>
PowerPC<CONFIG>::PowerPC(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	CPU<CONFIG>(name, parent),
	bus_port("bus-port"),
	snoop_port("snoop-port"),
	cpu_cycle_sctime(),
	bus_cycle_sctime(),
	nice_sctime(),
	next_nice_sctime(),
	nice_time(100000000000ULL), // leave at least 10 times per simulated seconds the host processor to the SystemC simulation kernel 
	param_nice_time("nice-time", this, nice_time),
	param_ipc("ipc", this, ipc),
	cpu_sctime(),
	bus_sctime(),
	last_sync_sctime(),
	ipc(1.0),
	external_interrupt_listener("external_interrupt_listener",this, &ev_interrupt),
	hard_reset_listener("hard_reset_listener",this, &ev_interrupt),
	soft_reset_listener("soft_reset_listener",this, &ev_interrupt),
	mcp_listener("mcp_listener",this, &ev_interrupt),
	tea_listener("tea_listener",this, &ev_interrupt),
	smi_listener("smi_listener",this, &ev_interrupt)
{
	SC_HAS_PROCESS(PowerPC);
	
	snoop_port(*this);
	external_interrupt_port(external_interrupt_listener);
	hard_reset_port(hard_reset_listener);
	soft_reset_port(soft_reset_listener);
	mcp_port(mcp_listener);
	tea_port(tea_listener);
	smi_port(smi_listener);

	SC_THREAD(Run);
}

template <class CONFIG>
PowerPC<CONFIG>::~PowerPC()
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::Stop(int ret)
{
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	if(unlikely(inherited::IsVerboseStep()))
	{
		inherited::logger << DebugInfo << "Program exited with status " << ret << EndDebugInfo;
	}
	BusSynchronize();
	sc_stop();
	wait();
}

template <class CONFIG>
void PowerPC<CONFIG>::Synchronize()
{
	sc_time time_spent = cpu_sctime - last_sync_sctime;
	wait(time_spent);
	last_sync_sctime = sc_time_stamp();
	next_nice_sctime = last_sync_sctime + nice_sctime;
	UpdateTime(last_sync_sctime - cpu_sctime);
}

	
template <class CONFIG>
bool PowerPC<CONFIG>::Setup()
{
	if(!CPU<CONFIG>::Setup()) return false;
	cpu_cycle_sctime = sc_time((double) inherited::cpu_cycle_time, SC_PS);
	bus_cycle_sctime = sc_time((double) inherited::bus_cycle_time, SC_PS);
	nice_sctime = sc_time((double) nice_time, SC_PS); // 10000 * cpu_cycle_sctime;//
	return true;
}

template <class CONFIG>
void PowerPC<CONFIG>::BusSynchronize()
{
	while(cpu_sctime > bus_sctime)
	{
		bus_sctime += bus_cycle_sctime;
		CPU<CONFIG>::OnBusCycle();
	}
	
	cpu_sctime = bus_sctime;
	
	Synchronize();
}

/*template <class CONFIG>
void PowerPC<CONFIG>::BusSynchronize()
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
// void PowerPC<CONFIG>::Run()
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
void PowerPC<CONFIG>::Idle()
{
	UpdateTime(SC_ZERO_TIME);
	sc_time max_idle_time(CPU<CONFIG>::GetDEC() * 4 * bus_cycle_sctime);
	usleep(max_idle_time.to_seconds() * 1.0e6);
	ev_max_idle.notify(max_idle_time);
	wait(ev_max_idle | ev_interrupt);
	last_sync_sctime = sc_time_stamp();
	ev_max_idle.cancel();
	//UpdateTime(sc_time_stamp() - cpu_sctime);
	while(!CPU<CONFIG>::HasAsynchronousInterrupt())
	{
		UpdateTime(bus_cycle_sctime);
	}
}

template <class CONFIG>
inline void PowerPC<CONFIG>::UpdateTime(const sc_time& delta)
{
	cpu_sctime += delta;
	while(cpu_sctime > bus_sctime)
	{
		bus_sctime += bus_cycle_sctime;
		CPU<CONFIG>::OnBusCycle();
	}
	
	if(cpu_sctime > next_nice_sctime)
	{
		Synchronize();
	}
}

template <class CONFIG>
void PowerPC<CONFIG>::Run()
{
	sc_time time_per_instruction = cpu_cycle_sctime * ipc;
	
	while(1)
	{
		CPU<CONFIG>::StepOneInstruction();
		UpdateTime(time_per_instruction);
	}
}

	
template <class CONFIG>
bool PowerPC<CONFIG>::Send(const Pointer<TlmMessage<FSBReq, FSBRsp> >& message)
{
	return true;
}

template <class CONFIG>
void PowerPC<CONFIG>::Reset()
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg, bool rwitm)
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
			UpdateTime(bus_cycle_sctime);
			BusSynchronize();
//			wait(bus_cycle_sctime);
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
		UpdateTime(sc_time_stamp() - cpu_sctime);
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
void PowerPC<CONFIG>::BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg)
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
		UpdateTime(bus_cycle_sctime);
		BusSynchronize();
//		wait(bus_cycle_sctime);
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
}

template <class CONFIG>
void PowerPC<CONFIG>::BusZeroBlock(physical_address_t physical_addr)
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
		UpdateTime(bus_cycle_sctime);
		BusSynchronize();
//		wait(bus_cycle_sctime);
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
void PowerPC<CONFIG>::BusFlushBlock(physical_address_t physical_addr)
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
		UpdateTime(bus_cycle_sctime);
		BusSynchronize();
//		wait(bus_cycle_sctime);
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
PowerPC<CONFIG>::ExternalInterruptListener::ExternalInterruptListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	cpu(_cpu),
	ev(_ev)
{
}
	
template <class CONFIG>
bool PowerPC<CONFIG>::ExternalInterruptListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqExternalInterrupt();
	else
		cpu->AckExternalInterrupt();
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::HardResetListener::HardResetListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	cpu(_cpu),
	ev(_ev)
{
}
	
template <class CONFIG>
bool PowerPC<CONFIG>::HardResetListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqHardReset();
	else
		cpu->AckHardReset();
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::SoftResetListener::SoftResetListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	cpu(_cpu),
	ev(_ev)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::SoftResetListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqSoftReset();
	else
		cpu->AckSoftReset();
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::MCPListener::MCPListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	cpu(_cpu),
	ev(_ev)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::MCPListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqMCP();
	else
		cpu->AckMCP();
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::TEAListener::TEAListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	cpu(_cpu),
	ev(_ev)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::TEAListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqTEA();
	else
		cpu->AckTEA();
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::SMIListener::SMIListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	cpu(_cpu),
	ev(_ev)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::SMIListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqSMI();
	else
		cpu->AckSMI();
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
