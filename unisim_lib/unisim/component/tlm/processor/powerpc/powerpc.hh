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
#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/component/tlm/message/snooping_fsb.hh>
#include <unisim/kernel/tlm/tlm.hh>
#include <unisim/component/tlm/message/interrupt.hh>

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace powerpc {

using namespace unisim::component::cxx::processor::powerpc;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::service::Parameter;
using unisim::component::tlm::message::InterruptRequest;
using unisim::component::tlm::message::SnoopingFSBRequest;
using unisim::component::tlm::message::SnoopingFSBResponse;

template <class CONFIG>
class PowerPC :
	public sc_module,
	public unisim::component::cxx::processor::powerpc::CPU<CONFIG>,
	public TlmSendIf<SnoopingFSBRequest<typename CONFIG::physical_address_t, CONFIG::FSB_BURST_SIZE>, SnoopingFSBResponse<CONFIG::FSB_BURST_SIZE> >
{
public:
	typedef unisim::component::cxx::processor::powerpc::CPU<CONFIG> inherited;
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::virtual_address_t virtual_address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;
	typedef SnoopingFSBRequest<physical_address_t, CONFIG::FSB_BURST_SIZE> FSBReq;
	typedef SnoopingFSBResponse<CONFIG::FSB_BURST_SIZE> FSBRsp;
	
//	// Bus SnoopingFSBRequest port
//	sc_port<TlmSendIf<BusSnoopingFSBRequest, BusGrant> > bus_req_port;
	
	// Bus port
	sc_port<TlmSendIf<FSBReq, FSBRsp> > bus_port;
	
	// Snoop port
	sc_export<TlmSendIf<FSBReq, FSBRsp> > snoop_port;
	
	// Interrupt ports
	sc_export<TlmSendIf<InterruptRequest> > external_interrupt_port;
	sc_export<TlmSendIf<InterruptRequest> > hard_reset_port;
	sc_export<TlmSendIf<InterruptRequest> > soft_reset_port;
	sc_export<TlmSendIf<InterruptRequest> > mcp_port;
	sc_export<TlmSendIf<InterruptRequest> > tea_port;
	sc_export<TlmSendIf<InterruptRequest> > smi_port;
	
	PowerPC(const sc_module_name& name, Object *parent = 0);
	virtual ~PowerPC();
	virtual void Stop(int ret);
	virtual void Synchronize();
	virtual bool Setup();
	inline void BusSynchronize()
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

	void Run();
	void BusMaster();
	virtual bool Send(const Pointer<TlmMessage<FSBReq, FSBRsp> >& message);
	virtual void Reset();
	virtual void BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT, bool rwitm = false);
	virtual void BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT);
	virtual void BusZeroBlock(physical_address_t physical_addr);
	virtual void BusFlushBlock(physical_address_t physical_addr);
	virtual void Idle();
protected:
	virtual void DoBusAccess(BusAccess<CONFIG> *bus_access);
private:
	sc_time cpu_cycle_sctime;
	sc_time bus_cycle_sctime;
	sc_time cpu_sctime;
	sc_time bus_sctime;
	sc_time last_sync_sctime;
	sc_time nice_sctime;
	sc_time next_nice_sctime;
	//uint64_t nice_time;
	double ipc;
	sc_event ev_max_idle;
	sc_event ev_interrupt;
	sc_time max_idle_time;
	sc_fifo<BusAccess<CONFIG> *> bus_access_queue;
	
	Parameter<sc_time> param_bus_cycle_time;
	Parameter<sc_time> param_nice_time;
	Parameter<double> param_ipc;

	inline void UpdateBusTime()
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;
	
	class ExternalInterruptListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		ExternalInterruptListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		unisim::component::cxx::processor::powerpc::CPU<CONFIG> *cpu;
		sc_event *ev;
	};
	
	class HardResetListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		HardResetListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		unisim::component::cxx::processor::powerpc::CPU<CONFIG> *cpu;
		sc_event *ev;
	};
	
	class SoftResetListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		SoftResetListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		unisim::component::cxx::processor::powerpc::CPU<CONFIG> *cpu;
		sc_event *ev;
	};
	
	class MCPListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		MCPListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		unisim::component::cxx::processor::powerpc::CPU<CONFIG> *cpu;
		sc_event *ev;
	};
	
	class TEAListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		TEAListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		unisim::component::cxx::processor::powerpc::CPU<CONFIG> *cpu;
		sc_event *ev;
	};
	
	class SMIListener :
		public sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		SMIListener(const sc_module_name& name, unisim::component::cxx::processor::powerpc::CPU<CONFIG> *_cpu, sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		unisim::component::cxx::processor::powerpc::CPU<CONFIG> *cpu;
		sc_event *ev;
	};

	ExternalInterruptListener external_interrupt_listener;
	HardResetListener hard_reset_listener;
	SoftResetListener soft_reset_listener;
	MCPListener mcp_listener;
	TEAListener tea_listener;
	SMIListener smi_listener;
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
