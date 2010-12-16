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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_MPC7447A_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_MPC7447A_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/mpc7447a/cpu.hh>
#include <systemc.h>
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include <inttypes.h>
#include <unisim/component/tlm2/interrupt/types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace mpc7447a {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::logger::Logger;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;

template <class CONFIG>
class CPU
	: public sc_module
	, public unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG>
	, public tlm::tlm_bw_transport_if<>
{
public:
	
	typedef unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG> inherited;
	typedef tlm::tlm_initiator_socket<CONFIG::FSB_WIDTH * 8> bus_master_socket;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;

	bus_master_socket bus_master_sock;
	irq_slave_socket external_interrupt_slave_sock;
	irq_slave_socket hard_reset_slave_sock;
	irq_slave_socket soft_reset_slave_sock;
	irq_slave_socket mcp_slave_sock;
	//irq_slave_socket tea_slave_sock;
	irq_slave_socket smi_slave_sock;
	
	CPU(const sc_module_name& name, Object *parent = 0);
	virtual ~CPU();
	
	virtual bool EndSetup();
	
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	virtual void Idle();
	virtual void Synchronize();
	
	inline void UpdateTime();
	inline void AlignToBusClock();
	void Run();
	void SignalExternalInterrupt();
	void SignalHardReset();
	void SignalSoftReset();
	void SignalMCP();
	//void SignalTEA();
	void SignalSMI();
	
protected:
	virtual void BusRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT, bool rwitm = false);
	virtual void BusWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT);
	virtual void BusZeroBlock(typename CONFIG::physical_address_t physical_addr);
	virtual void BusFlushBlock(typename CONFIG::physical_address_t physical_addr);
private:
	
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time cpu_time;
	sc_time bus_time;
	sc_time nice_time;
	sc_time max_idle_time;
	sc_event ev_max_idle;
	sc_event ev_irq;
	double ipc;

	Parameter<sc_time> param_bus_cycle_time;
	Parameter<sc_time> param_nice_time;
	Parameter<double> param_ipc;
	
	class IRQQueue : public tlm::tlm_fw_transport_if<InterruptProtocolTypes>
	{
	public:
		IRQQueue(const char *name);
		
		virtual void b_transport(TLMInterruptPayload& trans, sc_core::sc_time& t);

		virtual tlm::tlm_sync_enum nb_transport_fw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

		virtual unsigned int transport_dbg(TLMInterruptPayload& trans);
		
		virtual bool get_direct_mem_ptr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data);
		
		TLMInterruptPayload *GetNextIRQ();
		sc_event& GetEvent();

	private:
		tlm_utils::peq_with_get<TLMInterruptPayload> queue;
	};

	IRQQueue external_interrupt_queue;
	IRQQueue hard_reset_queue;
	IRQQueue soft_reset_queue;
	IRQQueue mcp_queue;
	//IRQQueue tea_queue;
	IRQQueue smi_queue;

	void SignalIRQ(IRQQueue& queue, unsigned int irq);
};

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
