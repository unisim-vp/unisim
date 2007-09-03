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
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_POWERPC_HH__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_POWERPC_HH__

#include <systemc.h>
#include <unisim/component/cxx/processor/powerpc/powerpc.hh>
#include <unisim/component/tlm/tlm/shared_memory/snooping_bus/message.hh>
#include <tlm/tlm.hh>
#include <tlm/interrupt/interrupt.hh>

namespace full_system {
namespace tlm {
namespace shared_memory {
namespace snooping_bus {

using namespace full_system::processors::powerpc;
using full_system::generic::bus::BS_OK;
using full_system::generic::bus::CS_SHARED;
using full_system::generic::bus::CS_MISS;
using full_system::tlm::TlmMessage;
using full_system::tlm::TlmSendIf;
using full_system::utils::garbage_collector::Pointer;
using full_system::utils::services::Parameter;
using full_system::tlm::interrupt::InterruptRequest;

template <class CONFIG>
class PowerPC :
	public sc_module,
	public full_system::processors::powerpc::CPU<CONFIG>,
	public BusInterface<typename CONFIG::physical_address_t>,
	public TlmSendIf<Request<typename CONFIG::physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> >
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::virtual_address_t virtual_address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;
	
//	// Bus Request port
//	sc_port<TlmSendIf<BusRequest, BusGrant> > bus_req_port;
	
	// Bus port
	sc_port<TlmSendIf<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > bus_port;
	
	// Snoop port
	sc_export<TlmSendIf<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > snoop_port;
	
	// Interrupt ports
	sc_export<TlmSendIf<InterruptRequest> > external_interrupt_port;
	sc_export<TlmSendIf<InterruptRequest> > hard_reset_port;
	sc_export<TlmSendIf<InterruptRequest> > soft_reset_port;
	sc_export<TlmSendIf<InterruptRequest> > mcp_port;
	sc_export<TlmSendIf<InterruptRequest> > tea_port;
	sc_export<TlmSendIf<InterruptRequest> > smi_port;
	
	PowerPC(const sc_module_name& name, Object *parent = 0) :
		Object(name, parent),
		sc_module(name),
		CPU<CONFIG>(name, this, parent),
//		bus_req_port("bus-req-port"),
		bus_port("bus-port"),
		snoop_port("snoop-port"),
		cpu_cycle_time(),
		bus_cycle_time(),
		nice_time(),
		next_nice_time(),
		nice_frequency(10.0), // leave at least 10 times per simulated seconds the host processor to the SystemC simulation kernel 
		param_nice_frequency("nice-frequency", this, nice_frequency),
		param_ipc("ipc", this, ipc),
		cpu_time(),
		bus_time(),
		last_cpu_time(),
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

	virtual ~PowerPC()
	{
	}
	
	virtual void Stop(int ret)
	{
		// Call BusSynchronize to account for the remaining time spent in the cpu core
		BusSynchronize();
		sc_stop();
		wait();
	}

	virtual void Sync()
	{
		sc_time time_spent = cpu_time - last_cpu_time;
		last_cpu_time = cpu_time;
		wait(time_spent);
	}

	
	virtual bool Setup()
	{
		if(!CPU<CONFIG>::Setup()) return false;
		cpu_cycle_time = sc_time(1.0 / (double) (*this)["cpu-frequency"], SC_US);
		bus_cycle_time = sc_time(1.0 / (double) (*this)["bus-frequency"], SC_US);
		nice_time = sc_time(1.0 / nice_frequency, SC_SEC);
		cerr << "nice_time = " << nice_time << endl;
		return true;
	}

	void BusSynchronize()
	{
		sc_time time_spent = cpu_time - last_cpu_time;
		last_cpu_time = cpu_time;
	
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
		sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
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
	
	void Run()
	{
		sc_time time_per_instruction = cpu_cycle_time * ipc;
		
		while(1)
		{
			if(cpu_time >= bus_time)
			{
				bus_time += bus_cycle_time;
				CPU<CONFIG>::OnBusCycle();
				if(cpu_time >= next_nice_time)
				{
					next_nice_time += nice_time;
					wait(SC_ZERO_TIME); // be nice with other threads: leave host execution resources
				}
			}
			CPU<CONFIG>::Step();
			cpu_time += time_per_instruction;
		}
	}

	
	virtual bool Send(const Pointer<TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > >& message)
	{
		const Pointer<Request<physical_address_t, CONFIG::FSB_BURST_SIZE> >& req = message->GetRequest();
		
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " " << name() << "::Send()" << endl;
#endif

		switch(req->type)
		{
			case Request<physical_address_t, CONFIG::FSB_BURST_SIZE>::READ:
			case Request<physical_address_t, CONFIG::FSB_BURST_SIZE>::READX:
				{
					Pointer<Response<CONFIG::FSB_BURST_SIZE> > rsp = new(rsp) Response<CONFIG::FSB_BURST_SIZE>();
					message->SetResponse(rsp);
					sc_event *rsp_ev = message->GetResponseEvent();
					rsp->read_status = Response<CONFIG::FSB_BURST_SIZE>::RS_MISS;
					if(rsp_ev) rsp_ev->notify(bus_cycle_time);
				}
				break;
			case Request<physical_address_t, CONFIG::FSB_BURST_SIZE>::WRITE:
				break;
			default:
				cerr << sc_time_stamp() << " " << name() << "::Send: receiving an unhandled request" << endl;
				sc_stop();
		}
		return true;
	}

	virtual void Reset()
	{
	}
	
	virtual void BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc, CacheStatus& cs)
	{
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " " << name() << "::BusRead()" << endl;
#endif
		// synchonize with bus cycle time
		BusSynchronize();
		
//		Pointer<BusRequest> bus_req = new(bus_req) BusRequest(); // bus request
//		Pointer<BusGrant> bus_grant;
//		sc_event bus_grant_ev; // event notified when bus access is either granted or denied
//		Pointer<TlmMessage<BusRequest, BusGrant> > br = new(br) TlmMessage<BusRequest, BusGrant>(bus_req, bus_grant_ev); // message for bus request/bus grant
//		
//		// loop until bus is granted
//#ifdef DEBUG_POWERPC
//		if(DebugEnabled())
//			cerr << sc_time_stamp() << " PPC::BusRead: loop until bus is granted" << endl;
//#endif
//		do
//		{
//			// loop until bus request is accepted
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//				cerr << sc_time_stamp() << " PPC::BusRead: try a bus request" << endl;
//#endif
//			while(!bus_req_port->Send(br))
//			{
//				// if bus request is not accepted then retry later
//				wait(bus_cycle_time);
//#ifdef DEBUG_POWERPC
//				if(DebugEnabled())
//					cerr << sc_time_stamp() << " PPC::BusRead: retry bus request" << endl;
//#endif
//			}
//			
//			// bus request has been accepted at this point
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//			{
//				cerr << sc_time_stamp() << " PPC::BusRead: bus request accepted" << endl;
//				cerr << sc_time_stamp() << " PPC::BusRead: waiting for bus grant decision" << endl;
//			}
//#endif
//			// wait for a bus grant decision
//			wait(bus_grant_ev);
//
//			bus_grant = br->GetResponse(); // bus grant
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//			{
//				if(!bus_grant->grant) cerr << sc_time_stamp() << " PPC::BusRead: bus not granted, retrying bus request" << endl;
//			}
//#endif
//		} while(!bus_grant->grant);
//		
//		// bus access has been granted at this point
//#ifdef DEBUG_POWERPC
//		if(DebugEnabled())
//		{
//			cerr << sc_time_stamp() << " PPC::BusRead: bus granted" << endl;
//		}
//#endif
		
		Pointer<Request<physical_address_t, CONFIG::FSB_BURST_SIZE> > req = new(req) Request<physical_address_t, CONFIG::FSB_BURST_SIZE>(); // bus transaction request
		Pointer<Response<CONFIG::FSB_BURST_SIZE> > rsp; // bus transaction response
		sc_event rsp_ev; // event notified when bus transaction response is ready
		Pointer<TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > msg =
				new(msg) TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> >(req, rsp_ev); // message for bus transaction
		
		req->type = Request<physical_address_t, CONFIG::FSB_BURST_SIZE>::READ; // transaction is a READ
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
				wait(bus_cycle_time);
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
				if(!rsp->read_status & (Response<CONFIG::FSB_BURST_SIZE>::RS_BUSY | Response<CONFIG::FSB_BURST_SIZE>::RS_MODIFIED))
					cerr << sc_time_stamp() << " PPC::BusRead: response read status is neither RS_BUSY nor RS_MODIFIED" << endl;
			}
#endif
		} while(rsp->read_status & (Response<CONFIG::FSB_BURST_SIZE>::RS_BUSY | Response<CONFIG::FSB_BURST_SIZE>::RS_MODIFIED));
		
		// bus transaction response has been received at this point
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusRead: copying response data" << endl;
#endif
		// copy the data from the response
		memcpy(buffer, rsp->read_data, size);
		
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			if(rsp->read_status & Response<CONFIG::FSB_BURST_SIZE>::RS_SHARED)
				cerr << sc_time_stamp() << " PPC::BusRead: read status contains RS_SHARED" << endl;
#endif

		// get the bus transaction response read status in order to update the block state
		cs = (rsp->read_status & Response<CONFIG::FSB_BURST_SIZE>::RS_SHARED) ? CS_SHARED : CS_MISS;
	}
	
	virtual void BusReadX(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc, CacheStatus& cs)
	{
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " " << name() << "::BusReadX()" << endl;
#endif
		// synchonize with bus cycle time
		BusSynchronize();
		
//		Pointer<BusRequest> bus_req = new(bus_req) BusRequest(); // bus request
//		Pointer<BusGrant> bus_grant;
//		sc_event bus_grant_ev; // event notified when bus access is either granted or denied
//		Pointer<TlmMessage<BusRequest, BusGrant> > br = new(br) TlmMessage<BusRequest, BusGrant>(bus_req, bus_grant_ev); // message for bus request/bus grant
//		
//		// loop until bus is granted
//#ifdef DEBUG_POWERPC
//		if(DebugEnabled())
//			cerr << sc_time_stamp() << " PPC::BusReadX: loop until bus is granted" << endl;
//#endif
//		do
//		{
//			// loop until bus request is accepted
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//				cerr << sc_time_stamp() << " PPC::BusReadX: try a bus request" << endl;
//#endif
//			while(!bus_req_port->Send(br))
//			{
//				// if bus request is not accepted then retry later
//				wait(bus_cycle_time);
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//				cerr << sc_time_stamp() << " PPC::BusReadX: retry bus request" << endl;
//#endif
//			}
//			
//			// bus request has been accepted at this point
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//			{
//				cerr << sc_time_stamp() << " PPC::BusReadX: bus request accepted" << endl;
//				cerr << sc_time_stamp() << " PPC::BusReadX: waiting for bus grant decision" << endl;
//			}
//#endif
//			// wait for a bus grant decision
//			wait(bus_grant_ev);
//
//			bus_grant = br->GetResponse(); // bus grant
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//				if(!bus_grant->grant) cerr << sc_time_stamp() << " PPC::BusReadX: bus not granted, retrying bus request" << endl;
//#endif
//		} while(!bus_grant->grant);
//		
//		// bus access has been granted at this point
//#ifdef DEBUG_POWERPC
//		if(DebugEnabled())
//			cerr << sc_time_stamp() << " PPC::BusReadX: bus granted" << endl;
//#endif
//		
		Pointer<Request<physical_address_t, CONFIG::FSB_BURST_SIZE> > req = new(req) Request<physical_address_t, CONFIG::FSB_BURST_SIZE>(); // bus transaction request
		Pointer<Response<CONFIG::FSB_BURST_SIZE> > rsp; // bus transaction response
		sc_event rsp_ev; // event notified when bus transaction response is ready
		Pointer<TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > msg =
				new(msg) TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> >(req, rsp_ev); // message for bus transaction
		
		req->type = Request<physical_address_t, CONFIG::FSB_BURST_SIZE>::READX; // transaction is a READX
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
				wait(bus_cycle_time);
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
				if(!rsp->read_status & (Response<CONFIG::FSB_BURST_SIZE>::RS_BUSY | Response<CONFIG::FSB_BURST_SIZE>::RS_MODIFIED))
					cerr << sc_time_stamp() << " PPC::BusReadX: response read status is neither RS_BUSY nor RS_MODIFIED" << endl;
			}
#endif
		} while(rsp->read_status & (Response<CONFIG::FSB_BURST_SIZE>::RS_BUSY | Response<CONFIG::FSB_BURST_SIZE>::RS_MODIFIED));
		
		// bus transaction response has been received at this point
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " PPC::BusReadX: copying response data" << endl;
#endif
		// copy the data from the response
		memcpy(buffer, rsp->read_data, size);
		
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			if(rsp->read_status & Response<CONFIG::FSB_BURST_SIZE>::RS_SHARED)
				cerr << sc_time_stamp() << " PPC::BusReadX: read status contains RS_SHARED" << endl;
#endif

		// get the bus transaction response read status in order to update the block state
		cs = (rsp->read_status & Response<CONFIG::FSB_BURST_SIZE>::RS_SHARED) ? CS_SHARED : CS_MISS;
	}
	
	virtual void BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, BusControl bc)
	{
#ifdef DEBUG_POWERPC
		if(DebugEnabled())
			cerr << sc_time_stamp() << " " << name() << "::BusWrite()" << endl;
#endif
		// synchonize with bus cycle time
		BusSynchronize();
		
//		Pointer<BusRequest> bus_req = new(bus_req) BusRequest(); // bus request
//		Pointer<BusGrant> bus_grant; // bus grant
//		sc_event bus_grant_ev; // event notified when bus access is either granted or denied
//		Pointer<TlmMessage<BusRequest, BusGrant> > br = new(br) TlmMessage<BusRequest, BusGrant>(bus_req, bus_grant_ev); // message for bus request/bus grant
//		
//		// loop until bus is granted
//#ifdef DEBUG_POWERPC
//		if(DebugEnabled())
//			cerr << sc_time_stamp() << " PPC::BusWrite: loop until bus is granted" << endl;
//#endif
//		do
//		{
//			// loop until bus request is accepted
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//				cerr << sc_time_stamp() << " PPC::BusWrite: try a bus request" << endl;
//#endif
//			while(!bus_req_port->Send(br))
//			{
//				// if bus request is not accepted then retry later
//				wait(bus_cycle_time);
//#ifdef DEBUG_POWERPC
//				if(DebugEnabled())
//					cerr << sc_time_stamp() << " PPC::BusWrite: retry bus request" << endl;
//#endif
//			}
//			
//			// bus request has been accepted at this point
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//			{
//				cerr << sc_time_stamp() << " PPC::BusWrite: bus request accepted" << endl;
//				cerr << sc_time_stamp() << " PPC::BusWrite: waiting for bus grant decision" << endl;
//			}
//#endif
//			// wait for a bus grant decision
//			wait(bus_grant_ev);
//			
//			bus_grant = br->GetResponse(); // bus grant
//#ifdef DEBUG_POWERPC
//			if(DebugEnabled())
//				if(!bus_grant->grant) cerr << sc_time_stamp() << " PPC::BusWrite: bus not granted, retrying bus request" << endl;
//#endif
//		} while(!bus_grant->grant);
//		
//		// bus access has been granted at this point
//#ifdef DEBUG_POWERPC
//		if(DebugEnabled())
//			cerr << sc_time_stamp() << " PPC::BusWrite: bus granted" << endl;
//#endif
		
		Pointer<Request<physical_address_t, CONFIG::FSB_BURST_SIZE> > req = new(req) Request<physical_address_t, CONFIG::FSB_BURST_SIZE>(); // bus transaction request
		Pointer<TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > msg =
				new(msg) TlmMessage<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> >(req); // message for bus transaction
		
		req->type = Request<physical_address_t, CONFIG::FSB_BURST_SIZE>::WRITE; // transaction is a WRITE
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
			wait(bus_cycle_time);
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
	
	virtual void BusInvalidateBlock(physical_address_t physical_addr, BusControl bc)
	{
	}
	
	virtual void BusFlushBlock(physical_address_t physical_addr, BusControl bc)
	{
	}
	
	virtual void BusZeroBlock(physical_address_t physical_addr, BusControl bc)
	{
	}
	
	virtual void BusInvalidateTLBEntry(physical_address_t addr, BusControl bc)
	{
	}
	
	virtual void BusSetCacheStatus(CacheStatus cs)
	{
	}

	BusStatus DevRead(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc)
	{
		return BS_OK;
	}
	
	BusStatus DevReadX(physical_address_t physical_addr, void *buffer, uint32_t size, BusControl bc)
	{
		return BS_OK;
	}
	
	BusStatus DevWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, BusControl bc)
	{
		return BS_OK;
	}
	
	BusStatus DevInvalidateBlock(physical_address_t physical_addr, BusControl bc)
	{
		return BS_OK;
	}
	
	BusStatus DevFlushBlock(physical_address_t physical_addr, BusControl bc)
	{
		return BS_OK;
	}
	
	BusStatus DevZeroBlock(physical_address_t physical_addr, BusControl bc)
	{
		return BS_OK;
	}
	
	BusStatus DevInvalidateTLBEntry(physical_address_t addr, BusControl bc)
	{
		return BS_OK;
	}
	
	void DevOnBusError(BusStatus bs)
	{
	}

private:
	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time cpu_time;
	sc_time bus_time;
	sc_time last_cpu_time;
	sc_time nice_time;
	sc_time next_nice_time;
	double nice_frequency;
	double ipc;
	
	Parameter<double> param_nice_frequency;
	Parameter<double> param_ipc;

	class ExternalInterruptListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		ExternalInterruptListener(const sc_module_name& name, full_system::processors::powerpc::CPU<CONFIG> *_cpu) :
			sc_module(name),
			cpu(_cpu)
		{
		}
		
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message)
		{
			if(message->req->level)
				cpu->ReqExternalInterrupt();
			else
				cpu->AckExternalInterrupt();
			
			return true;
		}
	private:
		full_system::processors::powerpc::CPU<CONFIG> *cpu;
	};
	
	class HardResetListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		HardResetListener(const sc_module_name& name, full_system::processors::powerpc::CPU<CONFIG> *_cpu) :
			sc_module(name),
			cpu(_cpu)
		{
		}
		
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message)
		{
			if(message->req->level)
				cpu->ReqHardReset();
			else
				cpu->AckHardReset();
			
			return true;
		}
	private:
		full_system::processors::powerpc::CPU<CONFIG> *cpu;
	};
	
	class SoftResetListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		SoftResetListener(const sc_module_name& name, full_system::processors::powerpc::CPU<CONFIG> *_cpu) :
			sc_module(name),
			cpu(_cpu)
		{
		}
		
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message)
		{
			if(message->req->level)
				cpu->ReqSoftReset();
			else
				cpu->AckSoftReset();
			
			return true;
		}
	private:
		full_system::processors::powerpc::CPU<CONFIG> *cpu;
	};

	class MCPListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		MCPListener(const sc_module_name& name, full_system::processors::powerpc::CPU<CONFIG> *_cpu) :
			sc_module(name),
			cpu(_cpu)
		{
		}
		
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message)
		{
			if(message->req->level)
				cpu->ReqMCP();
			else
				cpu->AckMCP();
			
			return true;
		}
	private:
		full_system::processors::powerpc::CPU<CONFIG> *cpu;
	};
	
	class TEAListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		TEAListener(const sc_module_name& name, full_system::processors::powerpc::CPU<CONFIG> *_cpu) :
			sc_module(name),
			cpu(_cpu)
		{
		}
		
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message)
		{
			if(message->req->level)
				cpu->ReqTEA();
			else
				cpu->AckTEA();
			
			return true;
		}
	private:
		full_system::processors::powerpc::CPU<CONFIG> *cpu;
	};

	class SMIListener :	
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		SMIListener(const sc_module_name& name, full_system::processors::powerpc::CPU<CONFIG> *_cpu) :
			sc_module(name),
			cpu(_cpu)
		{
		}
		
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message)
		{
			if(message->req->level)
				cpu->ReqSMI();
			else
				cpu->AckSMI();
			
			return true;
		}
	private:
		full_system::processors::powerpc::CPU<CONFIG> *cpu;
	};
	
	ExternalInterruptListener external_interrupt_listener;
	HardResetListener hard_reset_listener;
	SoftResetListener soft_reset_listener;
	MCPListener mcp_listener;
	TEAListener tea_listener;
	SMIListener smi_listener;
};

} // end of namespace snooping_bus
} // end of namespace shared_memory
} // end of namespace tlm
} // end of namespace full_system

#endif
