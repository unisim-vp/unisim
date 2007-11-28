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

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace powerpc {

using unisim::component::cxx::cache::BS_OK;
using unisim::component::cxx::cache::CS_SHARED;
using unisim::component::cxx::cache::CS_MISS;

template <class CONFIG>
PowerPC<CONFIG>::PowerPC(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	CPU<CONFIG>(name, this, parent),
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
	last_cpu_sctime(),
	ipc(1.0),
	external_interrupt_listener("external_interrupt_listener",this),
	hard_reset_listener("hard_reset_listener",this),
	soft_reset_listener("soft_reset_listener",this),
	mcp_listener("mcp_listener",this),
	tea_listener("tea_listener",this),
	smi_listener("smi_listener",this)
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
	BusSynchronize();
	sc_stop();
	wait();
}

template <class CONFIG>
void PowerPC<CONFIG>::Synchronize()
{
	sc_time time_spent = cpu_sctime - last_cpu_sctime;
	last_cpu_sctime = cpu_sctime;
	wait(time_spent);
	next_nice_sctime = sc_time_stamp() + nice_sctime;
}

	
template <class CONFIG>
bool PowerPC<CONFIG>::Setup()
{
	if(!CPU<CONFIG>::Setup()) return false;
	cpu_cycle_sctime = sc_time((double) inherited::cpu_cycle_time, SC_PS);
	bus_cycle_sctime = sc_time((double) inherited::bus_cycle_time, SC_PS);
	nice_sctime = sc_time((double) nice_time, SC_PS);
	return true;
}

template <class CONFIG>
void PowerPC<CONFIG>::BusSynchronize()
{
	sc_time time_spent = cpu_sctime - last_cpu_sctime;
	last_cpu_sctime = cpu_sctime;

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
		wait(sc_time(delay_tu, false));
	}
}
	
template <class CONFIG>
void PowerPC<CONFIG>::Run()
{
	sc_time time_per_instruction = cpu_cycle_sctime * ipc;
	
	while(1)
	{
		if(cpu_sctime >= bus_sctime)
		{
			bus_sctime += bus_cycle_sctime;
			CPU<CONFIG>::OnBusCycle();
			if(cpu_sctime >= next_nice_sctime)
			{
				Synchronize();
				//next_nice_sctime += nice_sctime;
				//wait(SC_ZERO_TIME); // be nice with other threads: leave host execution resources
			}
		}
		CPU<CONFIG>::Step();
		cpu_sctime += time_per_instruction;
	}
}

	
template <class CONFIG>
bool PowerPC<CONFIG>::Send(const Pointer<TlmMessage<FSBReq, FSBRsp> >& message)
{
	const Pointer<FSBReq>& req = message->GetRequest();
	
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " " << name() << "::Send()" << endl;
#endif

	switch(req->type)
	{
		case FSBReq::READ:
		case FSBReq::READX:
			{
				Pointer<FSBRsp> rsp = new(rsp) FSBRsp();
				message->SetResponse(rsp);
				sc_event *rsp_ev = message->GetResponseEvent();
				rsp->read_status = FSBRsp::RS_MISS;
				if(rsp_ev) rsp_ev->notify(bus_cycle_sctime);
			}
			break;
		case FSBReq::WRITE:
			break;
		default:
			cerr << sc_time_stamp() << " " << name() << "::Send: receiving an unhandled request" << endl;
			sc_stop();
	}
	return true;
}

template <class CONFIG>
void PowerPC<CONFIG>::Reset()
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc, CacheStatus& cs)
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
	
	req->type = FSBReq::READ; // transaction is a READ
	req->global = (bc & BC_GLOBAL) ? true : false; // whether transaction is global or not
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
	cs = (rsp->read_status & FSBRsp::RS_SHARED) ? CS_SHARED : CS_MISS;
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusReadX(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc, CacheStatus& cs)
{
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " " << name() << "::BusReadX()" << endl;
#endif
	// synchonize with bus cycle time
	BusSynchronize();
	
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<FSBRsp > rsp; // bus transaction response
	sc_event rsp_ev; // event notified when bus transaction response is ready
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req, rsp_ev); // message for bus transaction
	
	req->type = FSBReq::READX; // transaction is a READX
	req->global = (bc & BC_GLOBAL) ? true : false; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = size; // actual transaction size
	
	// loop until the request succeeds
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusReadX: loop until the request succeeds" << endl;
#endif
	// request succeeds when none of the processors is busy or have a modified copy of the request data in there caches
	do
	{
		// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusReadX: loop until bus transaction request is accepted" << endl;
#endif
		while(!bus_port->Send(msg))
		{
			// if bus transaction request is not accepted then retry later
			wait(bus_cycle_sctime);
#ifdef DEBUG_POWERPC
			if(DebugEnabled())
				cerr << sc_time_stamp() << " PPC::BusReadX: retry transaction request" << endl;
#endif
		}
		
		// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
		{
			cerr << sc_time_stamp() << " PPC::BusReadX: transaction request accepted" << endl;
			cerr << sc_time_stamp() << " PPC::BusReadX: waiting for response" << endl;
		}
#endif
		// wait for the bus transaction response
		wait(rsp_ev);
		rsp = msg->GetResponse();
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
		{
			cerr << sc_time_stamp() << " PPC::BusReadX: received response" << endl;
			if(!rsp->read_status & (FSBRsp::RS_BUSY | FSBRsp::RS_MODIFIED))
				cerr << sc_time_stamp() << " PPC::BusReadX: response read status is neither RS_BUSY nor RS_MODIFIED" << endl;
		}
#endif
	} while(rsp->read_status & (FSBRsp::RS_BUSY | FSBRsp::RS_MODIFIED));
	
	// bus transaction response has been received at this point
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		cerr << sc_time_stamp() << " PPC::BusReadX: copying response data" << endl;
#endif
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
#ifdef DEBUG_POWERPC
	if(DebugEnabled())
		if(rsp->read_status & FSBRsp::RS_SHARED)
			cerr << sc_time_stamp() << " PPC::BusReadX: read status contains RS_SHARED" << endl;
#endif

	// get the bus transaction response read status in order to update the block state
	cs = (rsp->read_status & FSBRsp::RS_SHARED) ? CS_SHARED : CS_MISS;
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, BusControl bc)
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
	req->global = (bc & BC_GLOBAL) ? true : false; // whether transaction is global or not
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
void PowerPC<CONFIG>::BusInvalidateBlock(physical_address_t physical_addr, BusControl bc)
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusFlushBlock(physical_address_t physical_addr, BusControl bc)
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusZeroBlock(physical_address_t physical_addr, BusControl bc)
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusInvalidateTLBEntry(physical_address_t addr, BusControl bc)
{
}
	
template <class CONFIG>
void PowerPC<CONFIG>::BusSetCacheStatus(CacheStatus cs)
{
}

template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevRead(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevReadX(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevInvalidateBlock(physical_address_t physical_addr, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevFlushBlock(physical_address_t physical_addr, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevZeroBlock(physical_address_t physical_addr, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
BusStatus PowerPC<CONFIG>::DevInvalidateTLBEntry(physical_address_t addr, BusControl bc)
{
	return BS_OK;
}
	
template <class CONFIG>
void PowerPC<CONFIG>::DevOnBusError(BusStatus bs)
{
}

template <class CONFIG>
PowerPC<CONFIG>::ExternalInterruptListener::ExternalInterruptListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu) :
	sc_module(name),
	cpu(_cpu)
{
}
	
template <class CONFIG>
bool PowerPC<CONFIG>::ExternalInterruptListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqExternalInterrupt();
	else
		cpu->AckExternalInterrupt();
	
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::HardResetListener::HardResetListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu) :
	sc_module(name),
	cpu(_cpu)
{
}
	
template <class CONFIG>
bool PowerPC<CONFIG>::HardResetListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqHardReset();
	else
		cpu->AckHardReset();
	
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::SoftResetListener::SoftResetListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu) :
	sc_module(name),
	cpu(_cpu)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::SoftResetListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqSoftReset();
	else
		cpu->AckSoftReset();
	
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::MCPListener::MCPListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu) :
	sc_module(name),
	cpu(_cpu)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::MCPListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqMCP();
	else
		cpu->AckMCP();
	
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::TEAListener::TEAListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu) :
	sc_module(name),
	cpu(_cpu)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::TEAListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqTEA();
	else
		cpu->AckTEA();
	
	return true;
}

template <class CONFIG>
PowerPC<CONFIG>::SMIListener::SMIListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu) :
	sc_module(name),
	cpu(_cpu)
{
}

template <class CONFIG>
bool PowerPC<CONFIG>::SMIListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->ReqSMI();
	else
		cpu->AckSMI();
	
	return true;
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
